#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);

//HD9连接点模板
void CStdComTemplConnPoint::InitConnPointTempl_HD9()
{
	name="HD9";//
	szSketchImgFile = "HD9.bmp";
	description="塔身与横担连接点";
	widBaseRod=1;		//以第一根杆件为节点父杆件
	widParentRod=1;	//以第一根杆件为连接定位依赖杆件
	this->m_bEnableIntelliConnPointDesign=false;
	this->xView.ciViewPlaneNormal='Y';
	xView.siMinBaseRodAngle=90;
	xView.siMaxBaseRodAngle=105;
	//竖立杆
	InitSchemaRod(rodarr[0],1,0,3,1,'X','A',true);
	rodarr[0].xFlag.bFilterWorkWing=true;
	//左上杆
	InitSchemaRod(rodarr[1],2,45,60,1,'X','R');
	rodarr[1].xFlag.bFilterWorkWing=true;
	//左杆
	InitSchemaRod(rodarr[2],3,80,100,1,'X','R');
	rodarr[2].xFlag.bFilterWorkWing=true;
	//左下斜杆
	InitSchemaRod(rodarr[3],4,125,140,2,'X','R');
	rodarr[3].xFlag.bFilterWorkWing=true;
	//右下杆
	InitSchemaRod(rodarr[4],5,245,260,1,'X','R');
	rodarr[4].ciPlaneNo=2;
	rodarr[4].xFlag.bFilterWorkWing=true;
	rodarr.ReSize(5);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD9);
	pMatchPlate->InitDefaultMatchRods(5);
	//pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD9_T);
	//pMatchPlate->InitDefaultMatchRods(5);
}

//HD9连接板模板
void CStdComTempl::InitNodePlateTemp_HD9()
{
	szSketchImgFile = "HD9.bmp";
	this->m_bEnableIntelliConnPointDesign=false;
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//
	CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine=0;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine=0;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->m_bNecessaryRod=true;
	CStdTemplParaPlate* pParaPlate=this->listParaPlates.Add(1);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//表示普通连接板
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode=1;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.ciProfileStyle=1;	//定制外形
	pParaPlate->design.ciDatumLenAxis='Y';
	pParaPlate->design.ciDatumAngleWing='Y';
	pParaPlate->design.bend.widAngleBendBy=1;
	pParaPlate->design.bend.ciBendMode=3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod1=1;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2=5;
	pParaPlate->connrods.Clear();
	//1.竖立杆
	pParaPlate->connrods[0].sidRod=1;
	pParaPlate->connrods[0].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[0].ciCurrTipSideS0E1=2;
	pParaPlate->connrods[0].ciPlaneNo=1;
	pParaPlate->connrods[0].cnMinBoltN=10;
	pParaPlate->connrods[0].dualSideEdge=true;
	pParaPlate->connrods[0].ciConnectWing='X';
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows=2;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.左上支撑件
	pParaPlate->connrods[1].sidRod=2;
	pParaPlate->connrods[1].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[1].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[1].ciOddCalStyle=0;
	pParaPlate->connrods[1].sidOddRefRod[0]=1;
	pParaPlate->connrods[1].sidOddRefRod[1]=3;
	pParaPlate->connrods[1].ciPlaneNo=1;
	pParaPlate->connrods[1].cnMinBoltN=3;
	pParaPlate->connrods[1].dualSideEdge=true;
	pParaPlate->connrods[1].ciConnectWing='X';
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.左
	pParaPlate->connrods[2].sidRod=3;
	pParaPlate->connrods[2].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[2].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[2].ciOddCalStyle=0;
	pParaPlate->connrods[2].sidOddRefRod[0]=1;
	pParaPlate->connrods[2].ciPlaneNo=1;
	pParaPlate->connrods[2].cnMinBoltN=4;
	pParaPlate->connrods[2].dualSideEdge=true;
	pParaPlate->connrods[2].ciConnectWing='X';
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.左下
	pParaPlate->connrods[3].sidRod=4;
	pParaPlate->connrods[3].ciNecessity=1;
	pParaPlate->connrods[3].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[3].ciOddCalStyle=0;
	pParaPlate->connrods[3].sidOddRefRod[0]=1;
	pParaPlate->connrods[3].sidOddRefRod[1]=3;
	pParaPlate->connrods[3].ciPlaneNo=1;
	pParaPlate->connrods[3].cnMinBoltN=3;
	pParaPlate->connrods[3].dualSideEdge=true;
	pParaPlate->connrods[3].ciConnectWing='X';
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.右下
	pParaPlate->connrods[4].sidRod=5;
	pParaPlate->connrods[4].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[4].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[4].ciOddCalStyle=0;
	pParaPlate->connrods[4].sidOddRefRod[0]=1;
	pParaPlate->connrods[4].ciPlaneNo=1;
	pParaPlate->connrods[4].cnMinBoltN=5;
	pParaPlate->connrods[4].dualSideEdge=true;
	pParaPlate->connrods[4].ciConnectWing='X';
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[4].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[4].layoutbolts[0].ciRows=2;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}

void CStdComTempl::InitNodePlateTemp_HD9_T()
{
	szSketchImgFile = "PHD9_T.bmp";
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
	//1.竖立杆
	pParaPlate->connrods[0].sidRod=1;
	pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=95;
	pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=105;
	pParaPlate->connrods[0].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[0].ciCurrTipSideS0E1=2;
	pParaPlate->connrods[0].ciPlaneNo=1;
	pParaPlate->connrods[0].cnMinBoltN=17;
	pParaPlate->connrods[0].dualSideEdge=false;
	pParaPlate->connrods[0].ciConnectWing='X';
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows=2;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.右下支撑件
	pParaPlate->connrods[1].sidRod=2;
	pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=145;
	pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=150;
	pParaPlate->connrods[1].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[1].ciCurrTipSideS0E1=1;
	pParaPlate->connrods[1].ciPlaneNo=1;
	pParaPlate->connrods[1].cnMinBoltN=13;
	pParaPlate->connrods[1].dualSideEdge=false;
	pParaPlate->connrods[1].ciConnectWing='Y';
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows=2;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.左上
	pParaPlate->connrods[2].sidRod=3;
	pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=135;
	pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle=140;
	pParaPlate->connrods[2].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[2].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[2].ciPlaneNo=1;
	pParaPlate->connrods[2].cnMinBoltN=6;
	pParaPlate->connrods[2].dualSideEdge=false;
	pParaPlate->connrods[2].ciConnectWing='Y';
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows=2;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.左水平杆
	pParaPlate->connrods[3].sidRod=4;
	pParaPlate->connrods[3].dynaRod.siRayMinDegAngle=0;
	pParaPlate->connrods[3].dynaRod.siRayMaxDegAngle=1;
	pParaPlate->connrods[3].ciNecessity=1;
	pParaPlate->connrods[3].ciCurrTipSideS0E1=1;
	pParaPlate->connrods[3].ciPlaneNo=1;
	pParaPlate->connrods[3].cnMinBoltN=12;
	pParaPlate->connrods[3].dualSideEdge=false;
	pParaPlate->connrods[3].ciConnectWing='Y';
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows=2;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.左下
	pParaPlate->connrods[4].sidRod=5;
	pParaPlate->connrods[4].dynaRod.siRayMinDegAngle=225;
	pParaPlate->connrods[4].dynaRod.siRayMaxDegAngle=235;
	pParaPlate->connrods[4].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[4].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[4].ciPlaneNo=1;
	pParaPlate->connrods[4].cnMinBoltN=11;
	pParaPlate->connrods[4].dualSideEdge=false;
	pParaPlate->connrods[4].ciConnectWing='Y';
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[4].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[4].layoutbolts[0].ciRows=2;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}