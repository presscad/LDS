#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);

void CStdComTemplConnPoint::InitConnPointTempl_HD14() 
{
	name="XH_HD14";//ID 15
	szSketchImgFile = "HD14.bmp";
	description="地线支架横担连接点";
	widBaseRod=1;		//以第一根杆件为节点父杆件
	widParentRod=1;	//以第一根杆件为连接定位依赖杆件
	this->xView.ciViewPlaneNormal='Y';
	xView.siMinBaseRodAngle=25;
	xView.siMaxBaseRodAngle=50;
	rodarr.ZeroPresetMemory();

	//右上直通杆
	InitSchemaRod(rodarr[0],1,0,10,1,'X','A',true);
	rodarr[0].xFlag.bFilterWorkWing=true;
	//.左上斜支撑杆
	InitSchemaRod(rodarr[1],2,50,70,1,'Y','R');
	//.左支撑杆
	InitSchemaRod(rodarr[2],3,110,160,1,'Y','R');
	rodarr[2].xFlag.bFilterWorkWing=true;
	//.横担水平杆
	InitSchemaRod(rodarr[3],4,320,340,1,'X','R');
	rodarr[3].ciPlaneNo=2;
	rodarr[3].xFlag.bFilterWorkWing=true;
	rodarr.ReSize(4);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_HD14);
	pMatchPlate->InitDefaultMatchRods(4);
}
void CStdComTempl::InitNodePlateTemp_HD14() 
{ 
	szSketchImgFile = "PHD14.bmp";
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8

	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	//pParaAngle->idStartNode=1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType = 0;
	pParaAngle->desStart.cTouchLine = 1;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->ciAngleWorkWing = 'X';
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Add(1);
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//表示普通连接板
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//定制外形
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 1;
	pParaPlate->design.bend.ciBendMode = 3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod1 = 1;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2 = 4;
	pParaPlate->connrods.Clear();
	//右上直通杆
	pParaPlate->connrods[0].sidRod = 1;
	pParaPlate->connrods[0].ciNecessity = 1;	//必要杆件
	pParaPlate->connrods[0].ciCurrTipSideS0E1 = 2;
	pParaPlate->connrods[0].ciPlaneNo = 1;
	pParaPlate->connrods[0].cnMinBoltN = 5;
	pParaPlate->connrods[0].dualSideEdge = true;
	pParaPlate->connrods[0].ciConnectWing = 'X';
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//上斜支撑杆
	pParaPlate->connrods[1].sidRod = 2;
	pParaPlate->connrods[1].ciNecessity = 1;	//必要杆件
	pParaPlate->connrods[1].ciCurrTipSideS0E1 = 0;
	pParaPlate->connrods[1].ciOddCalStyle = 0;
	pParaPlate->connrods[1].sidOddRefRod[0] = 1;
	pParaPlate->connrods[1].sidOddRefRod[1] = 3;
	pParaPlate->connrods[1].ciPlaneNo = 1;
	pParaPlate->connrods[1].cnMinBoltN = 2;
	pParaPlate->connrods[1].dualSideEdge = true;
	pParaPlate->connrods[1].ciConnectWing = 'Y';
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//左支撑杆
	pParaPlate->connrods[2].sidRod = 3;
	pParaPlate->connrods[2].ciNecessity = 1;	//必要杆件
	pParaPlate->connrods[2].ciCurrTipSideS0E1 = 0;
	pParaPlate->connrods[2].ciPlaneNo = 1;
	pParaPlate->connrods[2].ciOddCalStyle = 0;
	pParaPlate->connrods[2].sidOddRefRod[0] = 1;
	pParaPlate->connrods[2].sidOddRefRod[1] = 2;
	pParaPlate->connrods[2].cnMinBoltN = 2;
	pParaPlate->connrods[2].dualSideEdge = true;
	pParaPlate->connrods[2].ciConnectWing = 'Y';
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//横担水平杆
	pParaPlate->connrods[3].sidRod = 4;
	pParaPlate->connrods[3].ciNecessity = 1;	//必要杆件
	pParaPlate->connrods[3].ciCurrTipSideS0E1 = 0;
	pParaPlate->connrods[3].ciPlaneNo = 1;
	pParaPlate->connrods[3].ciOddCalStyle = 0;
	pParaPlate->connrods[3].sidOddRefRod[0] = 1;
	pParaPlate->connrods[3].cnMinBoltN = 2;
	pParaPlate->connrods[3].dualSideEdge = true;
	pParaPlate->connrods[3].ciConnectWing = 'X';
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(4);
}