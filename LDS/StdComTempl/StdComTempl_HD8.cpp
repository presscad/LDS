#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);

//HD8连接点模板参数
void CStdComTemplConnPoint::InitConnPointTempl_HD8()
{
	name="HD8";//
	szSketchImgFile = "HD8.bmp";
	description="塔身与横担连接点";
	widBaseRod=1;		//以第一根杆件为节点父杆件
	widParentRod=1;	//以第一根杆件为连接定位依赖杆件
	this->xView.ciViewPlaneNormal='Y';
	xView.siMinBaseRodAngle=90;
	xView.siMaxBaseRodAngle=105;
	rodarr.ZeroPresetMemory();
	//上立杆
	InitSchemaRod(rodarr[0],1,0,5,1,2,'A');
	rodarr[0].xFlag.bFilterWorkWing=true;
	//左上斜杆
	InitSchemaRod(rodarr[1],2,30,45,1,2,'R');
	//左侧水平杆
	InitSchemaRod(rodarr[2],3,80,95,1,2,'R');
	rodarr[2].xFlag.bFilterWorkWing=true;
	//向左下延伸杆
	InitSchemaRod(rodarr[3],4,130,145,1,3,'R');
	//右侧
	InitSchemaRod(rodarr[4],5,280,290,1,0,'R');
	rodarr[4].xFlag.bFilterWorkWing=true;
	rodarr[4].ciPlaneNo=2;
	//右上斜杆
	InitSchemaRod(rodarr[5],6,325,330,2,3,'R');
	rodarr[5].ciPlaneNo=2;
	rodarr.ReSize(6);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD8);
	pMatchPlate->InitDefaultMatchRods(6);
	pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD8_YS);
	pMatchPlate->InitDefaultMatchRods(5);
}

//HD8连接板模板参数
void CStdComTempl::InitNodePlateTemp_HD8()
{
	szSketchImgFile = "HD8.bmp";
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//
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
	//.上立杆
	pParaPlate->connrods[0].sidRod=1;
	pParaPlate->connrods[0].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[0].ciCurrTipSideS0E1=2;
	pParaPlate->connrods[0].ciPlaneNo=1;
	pParaPlate->connrods[0].cnMinBoltN=12;
	pParaPlate->connrods[0].dualSideEdge=true;
	pParaPlate->connrods[0].ciConnectWing=0;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows=2;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//.左上杆件
	pParaPlate->connrods[1].sidRod=2;
	pParaPlate->connrods[1].ciNecessity=1;
	pParaPlate->connrods[1].ciCurrTipSideS0E1=1;
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
	//.左侧水平杆
	pParaPlate->connrods[2].sidRod=3;
	pParaPlate->connrods[2].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[2].ciCurrTipSideS0E1=1;
	pParaPlate->connrods[2].ciPlaneNo=1;
	pParaPlate->connrods[2].cnMinBoltN=2;
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
	//.左下
	pParaPlate->connrods[3].sidRod=4;
	pParaPlate->connrods[3].ciNecessity=1;	//必要杆件
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
	//.右侧水平杆
	pParaPlate->connrods[4].sidRod=5;
	pParaPlate->connrods[4].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[4].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[4].ciPlaneNo=1;
	pParaPlate->connrods[4].cnMinBoltN=3;
	pParaPlate->connrods[4].dualSideEdge=true;
	pParaPlate->connrods[4].ciConnectWing=0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[4].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[4].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	//.右上斜支撑杆
	pParaPlate->connrods[5].sidRod=6;
	pParaPlate->connrods[5].ciNecessity=2;	//必要杆件
	pParaPlate->connrods[5].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[5].ciPlaneNo=1;
	pParaPlate->connrods[5].cnMinBoltN=2;
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

void CStdComTempl::InitNodePlateTemp_HD8_YS()
{
	szSketchImgFile = "HD8_YS.bmp";
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8

	CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine=1;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine=1;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine=1;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";//|]=0";
	pParaAngle->desStart.szYorOthrWingOffset="g";
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine=1;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";
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
	//.上立杆
	pParaPlate->connrods[0].sidRod=1;
	pParaPlate->connrods[0].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[0].ciCurrTipSideS0E1=2;
	pParaPlate->connrods[0].ciPlaneNo=1;
	pParaPlate->connrods[0].cnMinBoltN=8;
	pParaPlate->connrods[0].dualSideEdge=true;
	pParaPlate->connrods[0].ciConnectWing=0;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows=2;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//.左侧水平杆
	pParaPlate->connrods[1].sidRod=2;
	pParaPlate->connrods[1].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[1].ciCurrTipSideS0E1=1;
	pParaPlate->connrods[1].ciPlaneNo=1;
	pParaPlate->connrods[1].cnMinBoltN=2;
	pParaPlate->connrods[1].dualSideEdge=true;
	pParaPlate->connrods[1].ciConnectWing=0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//.右侧水平杆
	pParaPlate->connrods[2].sidRod=3;
	pParaPlate->connrods[2].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[2].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[2].ciPlaneNo=1;
	pParaPlate->connrods[2].cnMinBoltN=3;
	pParaPlate->connrods[2].dualSideEdge=true;
	pParaPlate->connrods[2].ciConnectWing=0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//.左上杆件
	pParaPlate->connrods[3].sidRod=4;
	pParaPlate->connrods[3].ciNecessity=1;
	pParaPlate->connrods[3].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[3].ciPlaneNo=1;
	pParaPlate->connrods[3].cnMinBoltN=3;
	pParaPlate->connrods[3].dualSideEdge=true;
	pParaPlate->connrods[3].ciConnectWing=0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//.左下
	pParaPlate->connrods[4].sidRod=5;
	pParaPlate->connrods[4].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[4].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[4].ciPlaneNo=1;
	pParaPlate->connrods[4].cnMinBoltN=3;
	pParaPlate->connrods[4].dualSideEdge=true;
	pParaPlate->connrods[4].ciConnectWing=0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[4].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[4].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}