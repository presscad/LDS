#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);

void CStdComTemplConnPoint::InitConnPointTempl_HD13() 
{
	name="XH_HD13";//id 16
	szSketchImgFile = "HD13.bmp";
	description="地线支架横担连接点";
	widBaseRod=1;		//以第一根杆件为节点父杆件
	widParentRod=1;	//以第一根杆件为连接定位依赖杆件
	//1.右上直通杆
	InitSchemaRod(rodarr[0],1,20,80,1,3,'A');
	//2右下辅助支撑杆
	InitSchemaRod(rodarr[1],2,155,180,2,4,'R');
	//3.横担斜拉杆
	InitSchemaRod(rodarr[2],3,110,150,1,3,'R');
	//4.左上斜支撑杆
	InitSchemaRod(rodarr[3],4,90,130,2,1,'R');
	//5.水平杆
	InitSchemaRod(rodarr[4],5,179,181,1,2,'R');
	rodarr.ReSize(5);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_HD13);
	pMatchPlate->InitDefaultMatchRods(5);
}
void CStdComTempl::InitNodePlateTemp_HD13() 
{ 
	szSketchImgFile = "PHD13.bmp";
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8

	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	CStdTemplParaRod* pParaRod = this->listParaRods.Add(1);
	pParaRod->m_bNecessaryRod = true;
	pParaRod = this->listParaRods.Add(2);
	pParaRod->idStartNode = 1;
	pParaRod->m_bNecessaryRod = true;
	pParaRod = this->listParaRods.Add(3);
	pParaRod->idStartNode = 1;
	pParaRod->m_bNecessaryRod = true;
	pParaRod = this->listParaRods.Add(4);
	pParaRod->idStartNode = 1;
	pParaRod->m_bNecessaryRod = true;
	pParaRod = this->listParaRods.Add(5);
	pParaRod->idStartNode = 1;
	pParaRod->m_bNecessaryRod = true;
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//表示普通连接板
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//定制外形
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.右上直通杆
	pParaPlate->connrods[0].sidRod = 1;
	pParaPlate->connrods[0].dynaRod.siRayMinDegAngle = 20;
	pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle = 80;
	pParaPlate->connrods[0].ciNecessity = 1;	//必要杆件
	pParaPlate->connrods[0].ciCurrTipSideS0E1 = 1;
	pParaPlate->connrods[0].ciPlaneNo = 1;
	pParaPlate->connrods[0].cnMinBoltN = 5;
	pParaPlate->connrods[0].dualSideEdge = false;
	pParaPlate->connrods[0].ciConnectWing = 'Y';
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.下辅助支撑杆
	pParaPlate->connrods[1].sidRod = 2;
	pParaPlate->connrods[1].dynaRod.siRayMinDegAngle = 155;
	pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle = 180;
	pParaPlate->connrods[1].ciNecessity = 2;	//必要杆件
	pParaPlate->connrods[1].ciCurrTipSideS0E1 = 1;
	pParaPlate->connrods[1].ciPlaneNo = 1;
	pParaPlate->connrods[1].cnMinBoltN = 1;
	pParaPlate->connrods[1].dualSideEdge = false;
	pParaPlate->connrods[1].ciConnectWing = 'Y';
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.横担斜拉杆
	pParaPlate->connrods[2].sidRod = 3;
	pParaPlate->connrods[2].dynaRod.siRayMinDegAngle = 110;
	pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle = 150;
	pParaPlate->connrods[2].ciNecessity = 1;	//必要杆件
	pParaPlate->connrods[2].ciCurrTipSideS0E1 = 1;
	pParaPlate->connrods[2].ciPlaneNo = 1;
	pParaPlate->connrods[2].cnMinBoltN = 3;
	pParaPlate->connrods[2].dualSideEdge = false;
	pParaPlate->connrods[2].ciConnectWing = 'Y';
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.左上斜支撑杆
	pParaPlate->connrods[3].sidRod = 4;
	pParaPlate->connrods[3].dynaRod.siRayMinDegAngle = 90;
	pParaPlate->connrods[3].dynaRod.siRayMaxDegAngle = 130;
	pParaPlate->connrods[3].ciNecessity = 2;	//必要杆件
	pParaPlate->connrods[3].ciCurrTipSideS0E1 = 1;
	pParaPlate->connrods[3].ciPlaneNo = 1;
	pParaPlate->connrods[3].cnMinBoltN = 1;
	pParaPlate->connrods[3].dualSideEdge = false;
	pParaPlate->connrods[3].ciConnectWing = 'Y';
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.水平杆
	pParaPlate->connrods[4].sidRod = 5;
	pParaPlate->connrods[4].dynaRod.siRayMinDegAngle = 179;
	pParaPlate->connrods[4].dynaRod.siRayMaxDegAngle = 181;
	pParaPlate->connrods[4].ciNecessity = 1;	//必要杆件
	pParaPlate->connrods[4].ciCurrTipSideS0E1 = 1;
	pParaPlate->connrods[4].ciPlaneNo = 1;
	pParaPlate->connrods[4].cnMinBoltN = 3;
	pParaPlate->connrods[4].dualSideEdge = false;
	pParaPlate->connrods[4].ciConnectWing = 'Y';
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[4].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}
