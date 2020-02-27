#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"

void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//横担挂线端点
void CStdComTemplConnPoint::InitConnPointTempl_GX1()
{
	name = "XH_GX1";//ID  12
	szSketchImgFile = "GX1.bmp";
	description = "挂线处控制节点";
	widBaseRod = 1;		//以第一根杆件为节点父杆件
	widParentRod = 1;	//以第一根杆件为连接定位依赖杆件
	this->ciBaseRodConnState = 2;		//2.衔接段末端连接
	this->xView.ciBaseAxisXMode = 1;	//X轴为基准材射线方向
	this->xView.ciViewPlaneNormal = 'Y';
	xView.siMinBaseRodAngle = -5;
	xView.siMaxBaseRodAngle = 5;
	this->m_bEnableIntelliConnPointDesign = true;
	rodarr.ZeroPresetMemory();
	//1.水平横杆
	InitSchemaRod(rodarr[0], 1, 0, 5, 1, 2, 'R', true);
	//2.斜拉杆
	InitSchemaRod(rodarr[1], 2, 15, 50, 1, 3, 'R');
	rodarr.ReSize(2);
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_GX1);
	pMatchPlate->InitDefaultMatchRods(2);
}
void CStdComTempl::InitNodePlateTemp_GX1()
{
	szSketchImgFile = "GX1.bmp";
	this->m_bEnableIntelliConnPointDesign = true;
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//节点和杆件描述
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->xFlag.bFilterWorkWing = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->xFlag.bFilterWorkWing = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//普通连接板模板描述
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	//pParaPlate->cbMirCreateFlag = 0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//定制外形
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.水平横杆
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 5, true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.斜拉杆
	pParaPlate->connrods[1].InitBasicInfo(2, true, 1, 3, false);
	pParaPlate->connrods[1].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(2);
}
//
void CStdComTemplConnPoint::InitConnPointTempl_GX2()
{
	name = "XH_GX2";//ID 13
	szSketchImgFile = "GX2.bmp";
	description = "挂线处控制节点";
	widBaseRod = 1;		//以第一根杆件为节点父杆件
	widParentRod = 1;	//以第一根杆件为连接定位依赖杆件
	this->ciBaseRodConnState = 2;		//2.衔接段末端连接
	this->xView.ciBaseAxisXMode = 1;	//X轴为基准材射线方向
	this->xView.ciViewPlaneNormal = 'Y';
	xView.siMinBaseRodAngle = 175;
	xView.siMaxBaseRodAngle = 185;
	this->m_bEnableIntelliConnPointDesign = true;
	rodarr.ZeroPresetMemory();
	//1.水平横杆
	InitSchemaRod(rodarr[0], 1, 0, 5, 1, 2, 'R', true);
	//2.斜拉杆
	InitSchemaRod(rodarr[1], 2, 15, 50, 1, 2, 'R');
	rodarr.ReSize(2);
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_GX2);
	pMatchPlate->InitDefaultMatchRods(2);
}
void CStdComTempl::InitNodePlateTemp_GX2()
{
	szSketchImgFile = "GX2.bmp";
	this->m_bEnableIntelliConnPointDesign = true;
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//节点和杆件描述
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	pParaAngle->desEnd.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desEnd.szYorOthrWingOffset = "g";
	//普通连接板模板描述
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	//pParaPlate->cbMirCreateFlag = 0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//定制外形
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.水平横杆
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 5, true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.斜拉杆
	pParaPlate->connrods[1].InitBasicInfo(2, true, 1, 2, false);
	pParaPlate->connrods[1].InitTipInfo(1, 0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(2);
}
//
void CStdComTemplConnPoint::InitConnPointTempl_GX3()
{
	name = "XH_GX3";
	szSketchImgFile = "GX3.bmp";
	description = "挂线处控制节点";	//导线垂直挂点处前后侧（正面）安装孔节点
	widBaseRod = 1;		//以第一根杆件为节点父杆件
	widParentRod = 1;	//以第一根杆件为连接定位依赖杆件
	this->ciBaseRodConnState = 2;		//2.衔接段末端连接
	this->xView.ciBaseAxisXMode = 1;	//X轴为基准材射线方向
	this->xView.ciViewPlaneNormal = 'Y';
	xView.siMinBaseRodAngle = 175;
	xView.siMaxBaseRodAngle = 185;
	this->m_bEnableIntelliConnPointDesign = true;
	rodarr.ZeroPresetMemory();
	//1.水平横杆
	InitSchemaRod(rodarr[0], 1, 0, 5, 1, 2, 'R', true);
	//2.竖直拉杆
	InitSchemaRod(rodarr[1], 2, 85, 100, 1, 2, 'R');
	//3.斜拉杆
	InitSchemaRod(rodarr[2], 3, 20, 60, 0, 2, 'R');
	rodarr.ReSize(3);
	//
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_GX3);
	pMatchPlate->InitDefaultMatchRods(3);
}
void CStdComTempl::InitNodePlateTemp_GX3()
{
	szSketchImgFile = "GX3.bmp";
	this->m_bEnableIntelliConnPointDesign = true;
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");
	//节点和杆件描述
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//连接板模板描述
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//定制外形
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.水平横杆
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 5, true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.竖直拉杆
	pParaPlate->connrods[1].InitBasicInfo(2, true, 1, 3, false);
	pParaPlate->connrods[1].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.斜拉杆
	pParaPlate->connrods[2].InitBasicInfo(3, false, 1, 2, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1, 2);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(3);
}
//
void CStdComTemplConnPoint::InitConnPointTempl_GX4()
{
	name = "XH_GX4";
	szSketchImgFile = "GX4.bmp";
	description = "挂线处控制节点";	//垂直于X轴的V型小隔面中底部连接节点
	widBaseRod = 1;		//以第一根杆件为节点父杆件
	widParentRod = 1;	//以第一根杆件为连接定位依赖杆件
	this->ciBaseRodConnState = 1;		//1.中间连接点
	this->xView.ciBaseAxisXMode = 1;	//X轴为基准材射线方向
	this->xView.ciViewPlaneNormal = 'X';
	xView.siMinBaseRodAngle = 175;
	xView.siMaxBaseRodAngle = 185;
	this->m_bEnableIntelliConnPointDesign = true;
	rodarr.ZeroPresetMemory();
	//1.水平横杆
	InitSchemaRod(rodarr[0], 1, 0, 5, 1, 2, 'A');
	//2.左斜拉杆
	InitSchemaRod(rodarr[1], 2, 110, 160, 1, 2, 'R');
	//3.右斜拉杆
	InitSchemaRod(rodarr[2], 3, 20, 70, 1, 2, 'R');
	//4.竖杆
	InitSchemaRod(rodarr[3], 4, 85, 95, 0, 2, 'R');
	rodarr.ReSize(4);
	//
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_GX4);
	pMatchPlate->InitDefaultMatchRods(4);
}
void CStdComTempl::InitNodePlateTemp_GX4()
{
	szSketchImgFile = "GX4.bmp";
	this->m_bEnableIntelliConnPointDesign = true;
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");
	//节点和杆件描述
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "g";
	pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "g";
	pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//连接板
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//表示普通连接板
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//定制外形
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//T型角钢1号角钢
	pParaPlate->connrods[0].InitBasicInfo(CMPSTID(1, 1), true, 1, 4, true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//T型角钢2号角钢
	pParaPlate->connrods[1].InitBasicInfo(CMPSTID(1, 2), true, 1, 4, true);
	pParaPlate->connrods[1].InitTipInfo(0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//2.左斜拉杆
	pParaPlate->connrods[2].InitBasicInfo(2, true, 1, 3, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//3.右斜拉杆
	pParaPlate->connrods[3].InitBasicInfo(3, true, 1, 3, false);
	pParaPlate->connrods[3].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//4.竖拉杆
	pParaPlate->connrods[4].InitBasicInfo(4, false, 1, 2, false);
	pParaPlate->connrods[4].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[4].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}
//
void CStdComTemplConnPoint::InitConnPointTempl_GX5()
{
	name = "XH_GX5";
	szSketchImgFile = "GX5.bmp";
	description = "挂线处控制节点";
	widBaseRod = 1;		//以第一根杆件为节点父杆件
	widParentRod = 1;	//以第一根杆件为连接定位依赖杆件
	this->ciBaseRodConnState = 2;		//2.衔接段末端连接
	this->xView.ciBaseAxisXMode = 1;	//X轴为基准材射线方向
	this->xView.ciViewPlaneNormal = 'Y';
	xView.siMinBaseRodAngle = 165;
	xView.siMaxBaseRodAngle = 185;
	this->m_bEnableIntelliConnPointDesign = false;
	rodarr.ZeroPresetMemory();
	//1.水平横杆
	InitSchemaRod(rodarr[0], 1, 0, 5, 1, 2, 'R', true);
	//2.竖直拉杆
	InitSchemaRod(rodarr[1], 2, 85, 110, 1, 2, 'R');
	//3.斜拉杆
	InitSchemaRod(rodarr[2], 3, 120, 160, 0, 2, 'R');
	rodarr.ReSize(3);
	//
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_GX5);
	pMatchPlate->InitDefaultMatchRods(3);
}
void CStdComTempl::InitNodePlateTemp_GX5()
{
	szSketchImgFile = "GX5.bmp";
	this->m_bEnableIntelliConnPointDesign = true;
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");
	//节点和杆件描述
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//连接板模板描述
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//定制外形
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.水平横杆
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 5, true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.竖直拉杆
	pParaPlate->connrods[1].InitBasicInfo(2, true, 1, 3, false);
	pParaPlate->connrods[1].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.斜拉杆
	pParaPlate->connrods[2].InitBasicInfo(3, false, 1, 2, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1, 2);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(3);
}