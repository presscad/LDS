#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);

//HD12连接点模板
void CStdComTemplConnPoint::InitConnPointTempl_HD12()
{
	name="XH_HD12";
	szSketchImgFile = "HD12.bmp";
	description="酒杯猫头横担下弦杆中间连接点";//  id 1
	widBaseRod=1;		//以第一根杆件为节点父杆件
	widParentRod=1;	//以第一根杆件为连接定位依赖杆件
	this->xView.ciViewPlaneNormal='Y';
	xView.siMinBaseRodAngle=-20;
	xView.siMaxBaseRodAngle=1;
	rodarr.ZeroPresetMemory();
	this->m_bEnableIntelliConnPointDesign = true;
	//1.右侧横担下弦杆
	InitSchemaRod(rodarr[0],1,0,1,1,'y','R');
	//2.左侧横担下弦杆
	InitSchemaRod(rodarr[1],2,180,220,1,'x','R');
	//3.横担中间竖杆
	InitSchemaRod(rodarr[2],3,90,110,1,'y','R');
	//4.向右上延伸的斜材腹杆
	InitSchemaRod(rodarr[3],4,30,70,1,'Y','R');
	//5.向左上延伸的斜材腹杆
	InitSchemaRod(rodarr[4],5,115,160,1,'y','R');
	rodarr.ReSize(5);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_HD12);
	pMatchPlate->InitDefaultMatchRods(5);
}
void CStdComTemplConnPoint::InitConnPointTempl_HD12X()
{
	name="XH_HD12";//  id 3
	szSketchImgFile = "HD12X.bmp";
	description="酒杯猫头内曲臂与横担连接点";
	widBaseRod=1;		//以第一根杆件为节点父杆件
	widParentRod=1;	//以第一根杆件为连接定位依赖杆件
	//1.水平横杆
	InitSchemaRod(rodarr[0],1,0,1,1,2,'R');
	//2.右上斜杆
	InitSchemaRod(rodarr[1],2,30,50,2,4,'R');
	//3.上立杆
	InitSchemaRod(rodarr[2],3,85,95,1,3,'R');
	//4.左上斜杆
	InitSchemaRod(rodarr[3],4,120,140,2,2,'R');
	//5.向下或左偏上伸的曲臂内弦杆
	InitSchemaRod(rodarr[4],5,170,280,1,2,'R');
	//6.右下斜杆
	InitSchemaRod(rodarr[5],6,310,330,2,4,'R');
	rodarr.ReSize(6);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_HD12_X);
	pMatchPlate->InitDefaultMatchRods(6);
}

//HD3A连接板模板
void CStdComTempl::InitNodePlateTemp_HD12()
{
	szSketchImgFile = "PHD12.bmp";
	m_bEnableIntelliConnPointDesign = true;	//已通过审核验证的标准节点板
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,3)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	this->m_cbDisableConnPlaneFlag=0x80;
	//
	CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle = NULL;
	//1.右侧横担下弦杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'y';
	pParaAngle->m_bNecessaryRod = true;
	//2.左侧横担下弦杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing = 'x';
	pParaAngle->m_bNecessaryRod=true;
	//3.横担中间竖杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing = 'y';
	pParaAngle->m_bNecessaryRod=true;
	//4.向右上延伸的斜材腹杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	//5.向左上延伸的斜材腹杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有

	CStdTemplParaPlate* pParaPlate=this->listParaPlates.Append();
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//表示普通连接板
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode=1;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.ciProfileStyle=1;	//定制外形
	pParaPlate->design.ciDatumLenAxis='Y';
	pParaPlate->design.ciDatumAngleWing='Y';
	pParaPlate->connrods.Clear();
	//1.右侧横担下弦杆
	pParaPlate->connrods[0].sidRod=1;
	pParaPlate->connrods[0].InitTipSpecOddment(0,-10);
	pParaPlate->connrods[0].ciPlaneNo=1;
	pParaPlate->connrods[0].ciBoltNCalMode=0;//blCalBoltNByRayRodsOnBaseRod=false;
	pParaPlate->connrods[0].cnMinBoltN=3;
	pParaPlate->connrods[0].dualSideEdge=true;
	pParaPlate->connrods[0].ciConnectWing='Y';
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_RODEND;
	//pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=BOLT_LAYOUT::LAYOUT_ONESIDE;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing='Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.左侧横担下弦杆
	pParaPlate->connrods[1].sidRod=2;
	pParaPlate->connrods[1].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[1].InitTipSpecOddment(0,-10);
	pParaPlate->connrods[1].ciPlaneNo=1;
	pParaPlate->connrods[1].cnMinBoltN=3;
	pParaPlate->connrods[1].dualSideEdge=false;
	pParaPlate->connrods[1].ciConnectWing=0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_RODEND;
	pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[1].layoutbolts[0].cnMaxSingularLineBolts=4;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.横担中间竖杆
	pParaPlate->connrods[2].sidRod=3;
	pParaPlate->connrods[2].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[2].InitTipInfo(0,0,1);
	pParaPlate->connrods[2].ciPlaneNo=1;
	pParaPlate->connrods[2].cnMinBoltN=3;
	pParaPlate->connrods[2].dualSideEdge=false;
	pParaPlate->connrods[2].ciConnectWing='Y';
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_RODEND;
	pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.向右上延伸的斜材腹杆
	pParaPlate->connrods[3].sidRod=4;
	pParaPlate->connrods[3].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[3].InitTipInfo(0,0,1,3);
	pParaPlate->connrods[3].ciPlaneNo=1;
	pParaPlate->connrods[3].cnMinBoltN=3;
	pParaPlate->connrods[3].dualSideEdge=false;
	pParaPlate->connrods[3].ciConnectWing='Y';
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_RODEND;
	pParaPlate->connrods[3].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.向左上延伸的斜材腹杆
	pParaPlate->connrods[4].sidRod=5;
	pParaPlate->connrods[4].ciNecessity=1;
	pParaPlate->connrods[4].InitTipInfo(0,0,2,3);
	pParaPlate->connrods[4].ciPlaneNo=1;
	pParaPlate->connrods[4].cnMinBoltN=3;
	pParaPlate->connrods[4].dualSideEdge=false;
	pParaPlate->connrods[4].ciConnectWing='Y';
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_RODEND;
	pParaPlate->connrods[4].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}
void CStdComTempl::InitNodePlateTemp_HD12X()
{
	szSketchImgFile = "PHD12X.bmp";
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8

	CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
	CStdTemplParaRod* pParaRod=this->listParaRods.Add(1);
	pParaRod->m_bNecessaryRod=true;
	pParaRod=this->listParaRods.Add(2);
	pParaRod->idStartNode=1;
	pParaRod->m_bNecessaryRod=true;
	pParaRod=this->listParaRods.Add(3);
	pParaRod->idStartNode=1;
	pParaRod->m_bNecessaryRod=true;
	pParaRod=this->listParaRods.Add(4);
	pParaRod->idStartNode=1;
	pParaRod->m_bNecessaryRod=true;
	pParaRod=this->listParaRods.Add(5);
	pParaRod->idStartNode=1;
	pParaRod->m_bNecessaryRod=true;
	pParaRod=this->listParaRods.Add(6);
	pParaRod->idStartNode=1;
	pParaRod->m_bNecessaryRod=true;
	CStdTemplParaPlate* pParaPlate=this->listParaPlates.Append();
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//表示普通连接板
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode=1;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.ciProfileStyle=1;	//定制外形
	pParaPlate->design.ciDatumLenAxis='Y';
	pParaPlate->design.ciDatumAngleWing='Y';
	pParaPlate->design.bend.widAngleBendBy=0;
	pParaPlate->connrods.Clear();
	//1.水平横杆
	pParaPlate->connrods[0].sidRod=1;
	pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=0;
	pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=1;
	pParaPlate->connrods[0].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[0].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[0].ciPlaneNo=1;
	pParaPlate->connrods[0].cnMinBoltN=4;
	pParaPlate->connrods[0].dualSideEdge=false;
	pParaPlate->connrods[0].ciConnectWing=0;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.右上斜杆
	pParaPlate->connrods[1].sidRod=2;
	pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=30;
	pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=45;
	pParaPlate->connrods[1].ciNecessity=2;
	pParaPlate->connrods[1].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[1].ciPlaneNo=1;
	pParaPlate->connrods[1].cnMinBoltN=3;
	pParaPlate->connrods[1].dualSideEdge=true;
	pParaPlate->connrods[1].ciConnectWing=0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.上立杆
	pParaPlate->connrods[2].sidRod=3;
	pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=85;
	pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle=95;
	pParaPlate->connrods[2].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[2].ciCurrTipSideS0E1=1;
	pParaPlate->connrods[2].ciPlaneNo=1;
	pParaPlate->connrods[2].cnMinBoltN=3;
	pParaPlate->connrods[2].dualSideEdge=true;
	pParaPlate->connrods[2].ciConnectWing=0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.左上斜杆
	pParaPlate->connrods[3].sidRod=4;
	pParaPlate->connrods[3].dynaRod.siRayMinDegAngle=120;
	pParaPlate->connrods[3].dynaRod.siRayMaxDegAngle=150;
	pParaPlate->connrods[3].ciNecessity=2;	//必要杆件
	pParaPlate->connrods[3].ciCurrTipSideS0E1=1;
	pParaPlate->connrods[3].ciPlaneNo=1;
	pParaPlate->connrods[3].cnMinBoltN=3;
	pParaPlate->connrods[3].dualSideEdge=true;
	pParaPlate->connrods[3].ciConnectWing=0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.向下或右下延伸的曲臂内弦杆
	pParaPlate->connrods[4].sidRod=5;
	pParaPlate->connrods[4].dynaRod.siRayMinDegAngle=160;
	pParaPlate->connrods[4].dynaRod.siRayMaxDegAngle=175;
	pParaPlate->connrods[4].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[4].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[4].ciPlaneNo=1;
	pParaPlate->connrods[4].cnMinBoltN=4;
	pParaPlate->connrods[4].dualSideEdge=false;
	pParaPlate->connrods[4].ciConnectWing=0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[4].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[4].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	//6.右下斜杆
	pParaPlate->connrods[5].sidRod=6;
	pParaPlate->connrods[5].dynaRod.siRayMinDegAngle=310;
	pParaPlate->connrods[5].dynaRod.siRayMaxDegAngle=340;
	pParaPlate->connrods[5].ciNecessity=2;	//必要杆件
	pParaPlate->connrods[5].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[5].ciPlaneNo=1;
	pParaPlate->connrods[5].cnMinBoltN=1;
	pParaPlate->connrods[5].dualSideEdge=false;
	pParaPlate->connrods[5].ciConnectWing=0;
	pParaPlate->connrods[5].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[5].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[5].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[5].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[5].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[5].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[5].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[5].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(6);
}
