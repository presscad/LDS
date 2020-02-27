#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"

void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//
void CStdComTemplConnPoint::InitConnPointTempl_DM1()
{
	name = "XH_DM1";
	szSketchImgFile = "DM1.bmp";
	description = "地线支架端底面";
	widBaseRod = 1;		//以第一根杆件为节点父杆件
	widParentRod = 1;	//以第一根杆件为连接定位依赖杆件
	this->xView.ciViewPlaneNormal = 'Z';
	xView.siMinBaseRodAngle = 0;
	xView.siMaxBaseRodAngle = 20;
	this->m_bEnableIntelliConnPointDesign = true;
	rodarr.ZeroPresetMemory();

	//1.横担
	InitSchemaRod(rodarr[0], 1, -1, 1, 1, 'X', 'R', true);
	//2.T型挂线角钢
	InitSchemaRod(rodarr[1], 2, 70, 95, 1, 2, 'R');
	//3.左上交叉斜材
	InitSchemaRod(rodarr[2], 3, 110, 150, 1, 'X', 'R');
	rodarr.ReSize(3);

	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DM1);
	pMatchPlate->xarrMapConnRods.Set(0, 1);
	pMatchPlate->xarrMapConnRods.Set(1, CMPSTID(2, 1));
	pMatchPlate->xarrMapConnRods.Set(2, CMPSTID(2, 2));
	pMatchPlate->xarrMapConnRods.Set(3, 3);
	pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DM1A);
	pMatchPlate->xarrMapConnRods.Set(0, 1);
	pMatchPlate->xarrMapConnRods.Set(1, CMPSTID(2, 1));
	pMatchPlate->xarrMapConnRods.Set(2, CMPSTID(2, 2));
	pMatchPlate->xarrMapConnRods.Set(3, 3);
}
void CStdComTempl::InitNodePlateTemp_DM1()
{
	szSketchImgFile = "DM1.bmp";
	this->m_bEnableIntelliConnPointDesign = true;
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//节点参数
	CStdTemplParaNode* pParaNode = NULL;
	pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//杆件参数
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->cbGroupAngle.bAsynAnglesOdd = true;	//两角钢的子角钢正负头与虚拟组合角钢异步
	pParaAngle->cbGroupAngle.ciMasterIndex = 0;		//预定T-X型（最终会与实际类型匹配同步其它属性）
	pParaAngle->ciAngleWorkWing = 'y';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "0";
	pParaAngle->desStart.szYorOthrWingOffset = "-R1#T";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T|]=-R1#T,-R2#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//普通连接板
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Add(1);
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//表示普通连接板
	//pParaPlate->cbMirCreateFlag = 0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//定制外形
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.横担
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 3, true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].ciConnectWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 2;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPlaneType = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciStyleG = 1;	//g1心，若g1=0,则取g
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciStyleCrossG = 0;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2内侧挂线子角钢
	pParaPlate->connrods[1].InitBasicInfo(CMPSTID(2, 1), true, 1, 3, false);	//
	pParaPlate->connrods[1].InitTipInfo(0, 1, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseAngle = 1;	//当前角钢
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciStyleG = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciStyleCrossG = 1;	//g1心，若g1=0,则取g
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.widBaseAngle = 1;	//当前角钢
	pParaPlate->connrods[1].layoutbolts[1].basepoint.ciStyleG = 0;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.ciStyleCrossG = 2;	//g2心，若g2=0,则取g
	pParaPlate->connrods[1].layoutbolts[1].cnCount = 1;
	pParaPlate->connrods[1].layoutbolts[1].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[1].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[1].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[1].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(2);
	//外侧挂线子角钢
	pParaPlate->connrods[2].InitBasicInfo(CMPSTID(2, 2), true, 1, 3, false);
	pParaPlate->connrods[2].InitTipInfo(0, 1, 1);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciStyleG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciStyleCrossG = 1;	//g1心，若g1=0,则取g
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.ciStyleG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.ciStyleCrossG = 2;	//g2心，若g2=0,则取g
	pParaPlate->connrods[2].layoutbolts[1].cnCount = 1;
	pParaPlate->connrods[2].layoutbolts[1].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[1].ciLayoutStyle = 1;
	pParaPlate->connrods[2].layoutbolts[1].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[2].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.ciStyleG = BOLT_ORIGIN::POS_WaS;	//W+S
	pParaPlate->connrods[2].layoutbolts[2].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[2].layoutbolts[2].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[2].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[2].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[2].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[2].layoutbolts[2].szLenOffsetOf1stBolt[0] = 0;
	pParaPlate->connrods[2].layoutbolts[2].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(3);
	//3.左上支撑杆
	pParaPlate->connrods[3].InitBasicInfo(3, true, 1, 2, false);
	pParaPlate->connrods[3].InitTipInfo(0, 1, 1, CMPSTID(2, 1));
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].ciConnectWing = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widCrossAngle = CMPSTID(2, 1);
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts[1].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[3].layoutbolts[1].basepoint.widCrossAngle = CMPSTID(2, 1);
	pParaPlate->connrods[3].layoutbolts[1].basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_WaS;
	pParaPlate->connrods[3].layoutbolts[1].cnCount = 0;
	pParaPlate->connrods[3].layoutbolts[1].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[1].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[1].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[3].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(2);
	pParaPlate->connrods.ReSize(4);
}
void CStdComTempl::InitNodePlateTemp_DM1A()
{
	szSketchImgFile = "DM1A.bmp";
	this->m_bEnableIntelliConnPointDesign = true;
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//节点参数
	CStdTemplParaNode* pParaNode = NULL;
	pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//杆件参数
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->cbGroupAngle.bAsynAnglesOdd = true;	//两角钢的子角钢正负头与虚拟组合角钢异步
	pParaAngle->cbGroupAngle.ciMasterIndex = 1;		//预定T-Y型（最终会与实际类型匹配同步其它属性）
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType = 0;
	pParaAngle->desStart.cTouchLine = 0xff;//'X';自动判断搭接工作肢
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "0";//"-P1#T";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->ciAngleWorkWing = 'x';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType = 0;
	pParaAngle->desStart.cTouchLine = 0xff;//'X';自动判断搭接工作肢
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//普通连接板
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Add(1);
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//表示普通连接板
	//pParaPlate->cbMirCreateFlag = 0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//定制外形
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.横担
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 3, true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].ciConnectWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 2;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPlaneType = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciStyleG = 1;	//g1心，若g1=0,则取g
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciStyleCrossG = 0;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2内侧挂线子角钢
	pParaPlate->connrods[1].InitBasicInfo(CMPSTID(2, 2), true, 1, 3, false);	//
	pParaPlate->connrods[1].InitTipInfo(0, 1, 1);
	pParaPlate->connrods[1].ciConnectWing = 0;
	pParaPlate->connrods[1].cnMaxSingularLineBolts = 4;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseAngle  = 1;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciStyleG = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widCrossAngle	= 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciStyleCrossG = 1;	//g1心，若g1=0,则取g
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 'y';//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.ciStyleG = 0;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.ciStyleCrossG = 2;	//g2心，若g2=0,则取g
	pParaPlate->connrods[1].layoutbolts[1].cnCount = 1;
	pParaPlate->connrods[1].layoutbolts[1].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[1].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[1].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[1].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts[2].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[1].layoutbolts[2].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[1].layoutbolts[2].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[1].layoutbolts[2].basepoint.ciStyleG = BOLT_ORIGIN::POS_WaS;	//W+S
	pParaPlate->connrods[1].layoutbolts[2].basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[1].layoutbolts[2].cnCount = 1;
	pParaPlate->connrods[1].layoutbolts[2].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[2].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[2].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[1].layoutbolts[2].szLenOffsetOf1stBolt[0] = 0;
	pParaPlate->connrods[1].layoutbolts[2].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(3);
	//外侧挂线子角钢
	pParaPlate->connrods[2].InitBasicInfo(CMPSTID(2, 1), true, 1, 3, false);
	pParaPlate->connrods[2].InitTipInfo(0, 1, 1);
	pParaPlate->connrods[2].ciConnectWing = 0;
	pParaPlate->connrods[2].cnMaxSingularLineBolts = 4;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widCrossAngle= 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciStyleG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciStyleCrossG = 1;	//g1心，若g1=0,则取g
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 'x';//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.ciStyleG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.ciStyleCrossG = 2;	//g2心，若g2=0,则取g
	pParaPlate->connrods[2].layoutbolts[1].cnCount = 1;
	pParaPlate->connrods[2].layoutbolts[1].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[1].ciLayoutStyle = 1;
	pParaPlate->connrods[2].layoutbolts[1].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[2].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.ciStyleG = BOLT_ORIGIN::POS_WaS;	//W+S
	pParaPlate->connrods[2].layoutbolts[2].basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[2].layoutbolts[2].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[2].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[2].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[2].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[2].layoutbolts[2].szLenOffsetOf1stBolt[0] = 0;
	pParaPlate->connrods[2].layoutbolts[2].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(3);
	//3.左上支撑杆
	pParaPlate->connrods[3].InitBasicInfo(3, true, 1, 2, false);
	pParaPlate->connrods[3].InitTipInfo(0, 0, 1, CMPSTID(2, 1));
	pParaPlate->connrods[3].ciConnectWing = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(4);
}
//
void CStdComTemplConnPoint::InitConnPointTempl_DM2()
{
	name = "XH_DM2";
	szSketchImgFile = "DM2.bmp";
	description = "挂线支架端底面";
	widBaseRod = 1;		//以第一根杆件为节点父杆件
	widParentRod = 1;	//以第一根杆件为连接定位依赖杆件
	xView.ciViewPlaneNormal = 'Z';
	xView.siMinBaseRodAngle = 0;
	xView.siMaxBaseRodAngle = 20;
	this->m_bEnableIntelliConnPointDesign = true;
	rodarr.ZeroPresetMemory();

	//1.水平横担
	InitSchemaRod(rodarr[0], 1, 0, 5, 1, 2, 'A');
	//2.竖直挂线角钢
	InitSchemaRod(rodarr[3], 2, 85, 95, 1, 2, 'R');
	//3.左斜拉杆
	InitSchemaRod(rodarr[1], 3, 105, 165, 0, 2, 'R');
	//4.右斜拉杆
	InitSchemaRod(rodarr[2], 4, 15, 75, 0, 2, 'R');
	rodarr.ReSize(4);
	//
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DM2);
	pMatchPlate->InitDefaultMatchRods(4);
}
void CStdComTempl::InitNodePlateTemp_DM2()
{
	szSketchImgFile = "DM2.bmp";
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->vLocalNearVecS2E.Set(1, 0, 0);
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "g";
	pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "g";
	pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->ciAngleWorkWing = 'X';
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
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//水平横担角钢
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 5, true);
	pParaPlate->connrods[0].InitTipInfo(2, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.竖拉杆
	pParaPlate->connrods[1].InitBasicInfo(4, false, 1, 2, false);
	pParaPlate->connrods[1].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.左斜拉杆
	pParaPlate->connrods[2].InitBasicInfo(2, true, 1, 3, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1, 2);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.右斜拉杆
	pParaPlate->connrods[3].InitBasicInfo(3, true, 1, 3, false);
	pParaPlate->connrods[3].InitTipInfo(0, 0, 1, 2);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(4);
}
//
void CStdComTemplConnPoint::InitConnPointTempl_DM3()
{
	name = "XH_DM3";
	szSketchImgFile = "DM3.bmp";
	description = "挂线支架端底面";
	widBaseRod = 1;		//以第一根杆件为节点父杆件
	widParentRod = 1;	//以第一根杆件为连接定位依赖杆件
	ciBaseRodConnState = 1;		//1.中间连接点
	xView.ciViewPlaneNormal = 'Z';
	xView.ciBaseAxisXMode = 1;
	xView.siMinBaseRodAngle = 85;
	xView.siMaxBaseRodAngle = 95;
	this->m_bEnableIntelliConnPointDesign = true;
	rodarr.ZeroPresetMemory();
	//1.T型挂线角钢
	InitSchemaRod(rodarr[0], 1, 0, 5, 1, 2, 'A');
	//2.左斜拉杆
	InitSchemaRod(rodarr[1], 2, 110, 165, 1, 2, 'R');
	//3.右斜拉杆
	InitSchemaRod(rodarr[2], 3, 15, 70, 1, 2, 'R');
	//4.竖杆
	InitSchemaRod(rodarr[3], 4, 85, 95, 0, 2, 'R');
	rodarr.ReSize(4);
	//
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DM3);
	pMatchPlate->InitDefaultMatchRods(4);
}
void CStdComTempl::InitNodePlateTemp_DM3()
{
	szSketchImgFile = "DM3.bmp";
	this->m_bEnableIntelliConnPointDesign = true;
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");
	//
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
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
	pParaPlate->design.ciDatumAngleWing = 'X';
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