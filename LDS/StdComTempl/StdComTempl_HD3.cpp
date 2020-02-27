#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//HD3连接点模板参数
void CStdComTemplConnPoint::InitConnPointTempl_HD3()
{
	name = "HD-03";
	szSketchImgFile = "HD3.bmp";
	description="酒杯塔上曲臂外侧与横担外连接点";	//"酒杯猫头外曲臂与横担连接点";
	widBaseRod=1;	//以第一根杆件为节点父杆件
	widParentRod=1;	//以第一根杆件为连接定位依赖杆件
	this->cnConnectPlane=3;
	//this->ciBaseRodConnState=4;	//3.衔接段截断点连接;4.共线截断点连接,两种情况均适用 故暂不设此过滤条件 wjh-2019.9.15
	this->ciBaseRodSearchMode=1;//自动根据工作平面视图搜索定位基准杆件
	xView.ciBaseAxisXMode=1;	//射线方向
	xView.ciViewPlaneNormal = 'Y';
	xView.siMinBaseRodAngle = 175;
	xView.siMaxBaseRodAngle = 185;
	rodarr.ZeroPresetMemory();
	m_bEnableIntelliConnPointDesign = true;			//已通过审核验证的标准节点板
	//1-左侧水平杆
	InitSchemaRod(rodarr[0],1,0,1,1,'x','R');
	//2-水平杆下方外曲臂主材杆
	InitSchemaRod(rodarr[1],2,45,88,1,3,'R');
	//3-左下延伸斜辅材杆
	InitSchemaRod(rodarr[2],3,20,40,2,'X','R');
	//4-右侧水平杆
	InitSchemaRod(rodarr[3],4,175,185,1,0,'R');
	//5-右上斜杆
	InitSchemaRod(rodarr[4],5,200,250,2,3,'R');
	//6-上立杆
	InitSchemaRod(rodarr[5],6,260,280,1,2,'R');
	//7-左上斜杆
	InitSchemaRod(rodarr[6],7,300,330,2,2,'R');
	rodarr.ReSize(7);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD3);
	pMatchPlate->InitDefaultMatchRods(7);
	//pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD3_2);
	//pMatchPlate->InitDefaultMatchRods(6);
}

void CStdComTemplConnPoint::InitConnPointTempl_HD3_G5()
{
	name = "HD-03-02";
	szSketchImgFile = "HD3_G5.bmp";
	description = "酒杯塔上曲臂外侧与横担外连接点(水平杆件非断)";
	widBaseRod = 1;		//以第一根杆件为节点父杆件
	widParentRod = 1;	//以第一根杆件为连接定位依赖杆件
	this->ciBaseRodConnState=1;	//基准杆件中间连接
	xView.ciBaseAxisXMode=0;	//始->终方向
	xView.ciViewPlaneNormal = 'Y';
	xView.siMinBaseRodAngle = 0;
	xView.siMaxBaseRodAngle = 20;
	rodarr.ZeroPresetMemory();
	//1-水平贯穿主杆
	InitSchemaRod(rodarr[0], 1, 0, 1, 1, 2, 'A');
	//2-左下延伸杆
	InitSchemaRod(rodarr[1], 2, 40, 88, 1, 3, 'R');
	//3-右上斜杆
	InitSchemaRod(rodarr[2], 3, 200, 250, 2, 3, 'R');
	//4-上立杆
	InitSchemaRod(rodarr[3], 4, 260, 280, 1, 2, 'R');
	//5-左上斜杆
	InitSchemaRod(rodarr[4], 5, 300, 330, 2, 2, 'R');
	rodarr.ReSize(5);
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD3_G5);
	pMatchPlate->InitDefaultMatchRods(5);
}

//HD3连接板模板参数
void CStdComTempl::InitNodePlateTemp_HD3()
{
	szSketchImgFile = "PHD3.bmp";
	m_bEnableIntelliConnPointDesign = true;			//已通过审核验证的标准节点板
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//基准节点
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//1-左侧水平杆(始端)
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//2-左下延伸杆(始端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType=1;
	pParaAngle->desStart.cTouchLine=0;
	pParaAngle->desStart.uidBendByRod=1;
	//3-左下延伸斜辅材杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType=1;
	pParaAngle->desStart.cTouchLine=0;
	pParaAngle->desStart.uidBendByRod=1;
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	pParaAngle->desStart.szYorOthrWingOffset="g";
	//4-右侧水平杆(终端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	pParaAngle->desStart.cTouchLineType=1;
	pParaAngle->desStart.cTouchLine=0;
	pParaAngle->desStart.uidBendByRod=6;
	//5-右上斜杆(始端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType=1;
	pParaAngle->desStart.cTouchLine=0;
	pParaAngle->desStart.uidBendByRod=6;
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	//6-上立杆(终端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(6,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//7-左上斜杆(终端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(7,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateEndPos = true;
	//普通连接板
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 1;
	pParaPlate->design.bend.ciBendMode = 3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod1 = 2;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2 = 1;
	pParaPlate->design.bend2.widAngleBendBy=6;
	pParaPlate->design.bend2.ciBendMode = 3;
	pParaPlate->design.bend2.normal.widBendPlaneRefRod1 = 4;
	pParaPlate->design.bend2.normal.widBendPlaneRefRod2 = 5;
	pParaPlate->connrods.Clear();
	//1-左侧水平杆
	pParaPlate->connrods[0].InitBasicInfo(1,true,1,6,false);
	pParaPlate->connrods[0].InitTipInfo(0,1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_RODNODE;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=BOLT_LAYOUT::LAYOUT_ONESIDE;
	pParaPlate->connrods[0].layoutbolts[0].szLenOffsetOf1stBolt[0]='0';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.左下延伸杆
	pParaPlate->connrods[1].InitBasicInfo(2,true,2,6,true);
	pParaPlate->connrods[1].InitTipInfo(0,0,1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	pParaPlate->connrods[1].cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.左下延伸斜辅材杆
	pParaPlate->connrods[2].InitBasicInfo(3,true,2,1,false);
	pParaPlate->connrods[2].InitTipInfo(0,0,1,2);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.右侧水平杆
	pParaPlate->connrods[3].InitBasicInfo(4,true,3,6,true);
	pParaPlate->connrods[3].InitTipInfo(1,0,1,6);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[3].cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	pParaPlate->connrods[3].cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.右上斜杆
	pParaPlate->connrods[4].InitBasicInfo(5,false,3,2,false);
	pParaPlate->connrods[4].InitTipInfo(0,0,4,6);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	//6.上立杆
	pParaPlate->connrods[5].InitBasicInfo(6,true,1,7,false);
	pParaPlate->connrods[5].InitTipInfo(1,0,1);
	pParaPlate->connrods[5].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[5].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[5].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[5].layoutbolts.ReSize(1);
	//7.左上斜杆
	pParaPlate->connrods[6].InitBasicInfo(7,false,1,2,false);
	pParaPlate->connrods[6].InitTipInfo(1,0,1,6);
	pParaPlate->connrods[6].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[6].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[6].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[6].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(7);
}

void CStdComTempl::InitNodePlateTemp_HD3_2()
{	//TODO:新加 wjh-2019.8.16
	szSketchImgFile = "PHD3_2.bmp";
	m_bEnableIntelliConnPointDesign = true;			//已通过审核验证的标准节点板
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//基准节点
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//1-左侧水平杆(始端)
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//2-左下延伸杆(始端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//3-右侧水平杆(终端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//4-右上斜杆(始端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	//5-上立杆(终端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//6-左上斜杆(终端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(6,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateEndPos = true;
	//普通连接板
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 5;
	pParaPlate->design.bend.ciBendMode = 3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod1 = 3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2 = 5;
	pParaPlate->design.bend2.widAngleBendBy = 1;
	pParaPlate->design.bend2.ciBendMode = 3;
	pParaPlate->design.bend2.normal.widBendPlaneRefRod1 = 1;
	pParaPlate->design.bend2.normal.widBendPlaneRefRod2 = 2;
	pParaPlate->connrods.Clear();
	//1-左侧水平杆
	pParaPlate->connrods[0].InitBasicInfo(1,true,1,4,false);
	pParaPlate->connrods[0].InitTipInfo(0,2);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.左下延伸杆
	pParaPlate->connrods[1].InitBasicInfo(2,true,3,6,false);
	pParaPlate->connrods[1].InitTipInfo(0,0,1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.右侧水平杆
	pParaPlate->connrods[2].InitBasicInfo(3,true,2,6,false);
	pParaPlate->connrods[2].InitTipInfo(1,0,1,5);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.右上斜杆
	pParaPlate->connrods[3].InitBasicInfo(4,false,2,2,false);
	pParaPlate->connrods[3].InitTipInfo(0,0,3,5);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.上立杆
	pParaPlate->connrods[4].InitBasicInfo(5,true,1,6,false);
	pParaPlate->connrods[4].InitTipInfo(1,0,1);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	//6.左上斜杆
	pParaPlate->connrods[5].InitBasicInfo(6,false,1,2,false);
	pParaPlate->connrods[5].InitTipInfo(1,0,1,5);
	pParaPlate->connrods[5].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[5].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[5].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[5].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(6);
}

void CStdComTempl::InitNodePlateTemp_HD3_G5()
{	//TODO:新加 wjh-2019.8.16
	szSketchImgFile = "PHD3_G5.bmp";
	m_bEnableIntelliConnPointDesign = true;			//已通过审核验证的标准节点板
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//基准节点
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//1-水平贯穿主杆
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idMidNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	//2-左下延伸杆(始端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//3-右上斜杆(始端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "g";
	pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	//4-上立杆(终端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//5-左上斜杆(终端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateEndPos = true;
	//普通连接板
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 1;
	pParaPlate->design.bend.ciBendMode = 3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod1 = 2;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2 = 1;
	pParaPlate->connrods.Clear();
	//1-左侧水平杆
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 8, true);
	pParaPlate->connrods[0].InitTipInfo(2,2);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.左下延伸杆
	pParaPlate->connrods[1].InitBasicInfo(2,true,2,6,false);
	pParaPlate->connrods[1].InitTipInfo(0,0,1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.右上斜杆
	pParaPlate->connrods[2].InitBasicInfo(3, false, 1, 2, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1, 4);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.上立杆
	pParaPlate->connrods[3].InitBasicInfo(4, true, 1, 6, false);
	pParaPlate->connrods[3].InitTipInfo(1, 0, 1);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.左上斜杆
	pParaPlate->connrods[4].InitBasicInfo(5, false, 1, 2, false);
	pParaPlate->connrods[4].InitTipInfo(1, 0, 1, 5);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}