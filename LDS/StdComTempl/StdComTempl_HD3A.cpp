#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);

//HD9连接点模板
void CStdComTemplConnPoint::InitConnPointTempl_HD3A()
{
	name="XH_HD3A";
	szSketchImgFile = "HD3A.bmp";
	description="酒杯猫头内曲臂与横担连接点";//  id 1
	widBaseRod=1;		//以第一根杆件为节点父杆件
	widParentRod=1;	//以第一根杆件为连接定位依赖杆件
	this->cnConnectPlane=2;
	this->xView.ciBaseAxisXMode=1;
	this->xView.ciViewPlaneNormal='Y';
	xView.siMinBaseRodAngle=-5;
	xView.siMaxBaseRodAngle=5;
	rodarr.ZeroPresetMemory();
	this->m_bEnableIntelliConnPointDesign = true;
	//1.右侧水平横杆
	InitSchemaRod(rodarr[0],1,0,1,1,2,'R');
	//2.上立杆
	InitSchemaRod(rodarr[1],2,89,91,1,3,'R');
	//3.向左(上)侧延伸的横担下弦杆
	InitSchemaRod(rodarr[2],3,145,181,1,3,'R');
	//4.向下或右下延伸的曲臂内弦杆
	InitSchemaRod(rodarr[3],4,260,295,1,2,'R');
	//5.右上斜材腹杆
	InitSchemaRod(rodarr[4],5,30,70,2,3,'R');
	//6.左上斜材腹杆
	InitSchemaRod(rodarr[5],6,110,140,2,3,'R');
	//7.右下辅材杆
	InitSchemaRod(rodarr[6],7,300,340,2,3,'R');
	rodarr.ReSize(7);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_HD3A);
	pMatchPlate->InitDefaultMatchRods(7);
}

//HD3A连接板模板
void CStdComTempl::InitNodePlateTemp_HD3A()
{
	szSketchImgFile = "PHD3A.bmp";
	m_bEnableIntelliConnPointDesign = true;	//已通过审核验证的标准节点板
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//
	CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;	//交于心线
	pParaAngle->desStart.cTouchLine=1;	//工作肢搭接于1号角钢Y肢心线
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->ciAngleWorkWing='x';
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;	//交于心线
	pParaAngle->desStart.cTouchLine=1;	//工作肢搭接于1号角钢Y肢心线
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	pParaAngle->desStart.szYorOthrWingOffset="g";
	pParaAngle->m_bNeedUpdateNormal=true;
	pParaAngle->normal.ciNormalCalStyle=0;
	pParaAngle->normal.blSyncOtherAngleLeg=false;
	pParaAngle->normal.uidRefRodOfWorkWing=2;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->ciAngleWorkWing='y';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLine=1;		//Y肢心线
	pParaAngle->desStart.cTouchLineType=1;	//交于火曲线
	pParaAngle->desStart.uidBendByRod=1;
	pParaAngle->m_bNeedUpdateNormal=true;
	pParaAngle->normal.ciNormalCalStyle=0;
	pParaAngle->normal.blSyncOtherAngleLeg=false;
	pParaAngle->normal.uidRefRodOfWorkWing=1;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType=0;	//交于心线
	pParaAngle->desStart.cTouchLine=1;	//工作肢搭接于1号角钢Y肢心线
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(6,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->ciAngleWorkWing = 'y';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType=0;	//交于心线
	pParaAngle->desStart.cTouchLine=1;	//工作肢搭接于1号角钢Y肢心线
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(7,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLine=1;		//Y肢心线
	pParaAngle->desStart.cTouchLineType=1;	//交于火曲线
	pParaAngle->desStart.uidBendByRod=1;
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
	pParaPlate->design.bend.ciBendMode=3;
	pParaPlate->design.bend.widAngleBendBy=1;
	pParaPlate->design.bend.normal.widBendPlaneRefRod1=1;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2=4;
	pParaPlate->connrods.Clear();
	//1.水平横杆
	pParaPlate->connrods[0].sidRod=1;
	pParaPlate->connrods[0].InitTipInfo(0,1);	//根据螺栓位置计算杆件正负头
	pParaPlate->connrods[0].ciPlaneNo=1;
	pParaPlate->connrods[0].ciBoltNCalMode=0;//blCalBoltNByRayRodsOnBaseRod=false;
	pParaPlate->connrods[0].cnMinBoltN=5;
	pParaPlate->connrods[0].dualSideEdge=true;
	pParaPlate->connrods[0].ciConnectWing='Y';
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_RODNODE;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].szLenOffsetOf1stBolt[0]='0';
	pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[0].layoutbolts[0].cnMaxSingularLineBolts=4;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=BOLT_LAYOUT::LAYOUT_ONESIDE;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing='Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.上侧竖杆
	pParaPlate->connrods[1].sidRod=2;
	pParaPlate->connrods[1].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[1].InitTipInfo(0,0,1);
	pParaPlate->connrods[1].ciPlaneNo=1;
	pParaPlate->connrods[1].cnMinBoltN=2;
	pParaPlate->connrods[1].dualSideEdge=false;
	pParaPlate->connrods[1].ciConnectWing=0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_RODEND;
	pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[1].layoutbolts[0].cnMaxSingularLineBolts=4;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.左上延伸的横担下弦杆
	pParaPlate->connrods[2].sidRod=3;
	pParaPlate->connrods[2].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[2].InitTipInfo(0,0,1,2);
	pParaPlate->connrods[2].InitTipSpecOddment(0,-40);
	pParaPlate->connrods[2].ciPlaneNo=1;
	pParaPlate->connrods[2].cnMinBoltN=3;
	pParaPlate->connrods[2].dualSideEdge=false;
	pParaPlate->connrods[2].ciConnectWing='X';
	pParaPlate->connrods[2].layoutbolts[0].cnMaxSingularLineBolts=3;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_RODEND;
	pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.下侧支撑杆
	pParaPlate->connrods[3].sidRod=4;
	pParaPlate->connrods[3].ciNecessity=1;	//必要杆件
	pParaPlate->connrods[3].InitTipInfo(0,0,1);
	pParaPlate->connrods[3].ciPlaneNo=2;
	pParaPlate->connrods[3].cnMinBoltN=3;
	pParaPlate->connrods[3].dualSideEdge=false;
	pParaPlate->connrods[3].ciConnectWing='Y';
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_RODEND;
	pParaPlate->connrods[3].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].cnMaxSingularLineBolts=4;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.右上斜材腹杆
	pParaPlate->connrods[4].sidRod=5;
	pParaPlate->connrods[4].ciNecessity=2;	//必要杆件
	pParaPlate->connrods[4].InitTipInfo(0,0,1,2);
	pParaPlate->connrods[4].ciPlaneNo=1;
	pParaPlate->connrods[4].cnMinBoltN=3;
	pParaPlate->connrods[4].dualSideEdge=false;
	pParaPlate->connrods[4].ciConnectWing='Y';
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_RODEND;
	pParaPlate->connrods[4].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].cnMaxSingularLineBolts=3;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	//6.左上斜材腹杆
	pParaPlate->connrods[5].sidRod=6;
	pParaPlate->connrods[5].ciNecessity=2;
	pParaPlate->connrods[5].InitTipInfo(0,0,2,3);
	pParaPlate->connrods[5].ciPlaneNo=1;
	pParaPlate->connrods[5].cnMinBoltN=3;
	pParaPlate->connrods[5].dualSideEdge=false;
	pParaPlate->connrods[5].ciConnectWing='Y';
	pParaPlate->connrods[5].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_RODEND;
	pParaPlate->connrods[5].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[5].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[5].layoutbolts[0].cnMaxSingularLineBolts=3;
	pParaPlate->connrods[5].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[5].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[5].layoutbolts.ReSize(1);
	//7.右下辅材杆
	pParaPlate->connrods[6].sidRod=7;
	pParaPlate->connrods[6].ciNecessity=2;
	pParaPlate->connrods[6].InitTipInfo(0,0,1,4);
	pParaPlate->connrods[6].ciPlaneNo=2;
	pParaPlate->connrods[6].cnMinBoltN=3;
	pParaPlate->connrods[6].dualSideEdge=false;
	pParaPlate->connrods[6].ciConnectWing='Y';
	pParaPlate->connrods[6].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_RODEND;
	pParaPlate->connrods[6].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[6].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[6].layoutbolts[0].cnMaxSingularLineBolts=3;
	pParaPlate->connrods[6].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[6].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[6].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(7);
}
