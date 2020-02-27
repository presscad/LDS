#include "stdafx.h"
#include "..\LDS.h"
#include "f_ent.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
#include "lds_part.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//HD2连接点模板参数
void CStdComTemplConnPoint::InitConnPointTempl_HD2()
{
	name="HD-02-01";//HD2
	szSketchImgFile="HD2.bmp";
	description="酒杯猫头横担与地线支架外侧接点";
	widBaseRod=1;
	widParentRod=1;
	xView.ciViewPlaneNormal = 'Y';
	xView.siMinBaseRodAngle = 0;
	xView.siMaxBaseRodAngle = 20;
	rodarr.ZeroPresetMemory();
	m_bEnableIntelliConnPointDesign = true;			//已通过审核验证的标准节点板
	//1-左侧水平杆
	InitSchemaRod(rodarr[0],1,0,1,1,2,'R');
	//2-左下斜杆
	InitSchemaRod(rodarr[1],2,30,70,2,2,'R');
	//3.下侧竖杆
	InitSchemaRod(rodarr[2],3,70,100,1,3,'R');
	//4.右下侧斜杆
	InitSchemaRod(rodarr[3],4,110,170,1,3,'R');
	//5.右上斜杆
	InitSchemaRod(rodarr[4],5,200,260,1,2,'R');
	//6.左上辅材杆
	InitSchemaRod(rodarr[5],6,280,315,2,1,'R');
	//7.左上斜材杆
	InitSchemaRod(rodarr[6],7,290,335,2,1,'R');
	rodarr.ReSize(7);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD2);
	pMatchPlate->InitDefaultMatchRods(7);
}

//HD2连接板模板参数
void CStdComTempl::InitNodePlateTemp_HD2()
{
	szSketchImgFile="PHD2.bmp";
	m_bEnableIntelliConnPointDesign = true;			//已通过审核验证的标准节点板
	//节点构造基准装配坐标系定义
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//基准节点
	CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//1-左侧水平杆(终端)
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//2-左下斜杆(始端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	pParaAngle->desStart.szYorOthrWingOffset="g";
	//3-下侧竖杆(始端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//4-右下侧斜杆(始端)
	pParaAngle= (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	pParaAngle->desStart.szYorOthrWingOffset="g";
	//5-右上斜杆(终端)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idEndNode=1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->bUpdateEndPos = true;
	//6-左上辅材杆(终端)
	pParaAngle= (CStdTemplParaAngle*)this->listParaRods.Add(6,CLS_LINEANGLE);
	pParaAngle->idEndNode =1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod=false;
	pParaAngle->bUpdateEndPos = true;
	pParaAngle->desEnd.szXorCtrlWingOffset="g";
	pParaAngle->desEnd.szYorOthrWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	//7-左上斜材杆(终端)
	pParaAngle= (CStdTemplParaAngle*)this->listParaRods.Add(7,CLS_LINEANGLE);
	pParaAngle->idEndNode =1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod=false;
	pParaAngle->bUpdateEndPos = true;
	pParaAngle->desEnd.szXorCtrlWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	pParaAngle->desEnd.szYorOthrWingOffset="g";
	//节点板
	CStdTemplParaPlate* pParaPlate=this->listParaPlates.Append();
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//表示普通连接板
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode=1;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.ciProfileStyle=1;	//定制外形
	pParaPlate->design.ciDatumAngleWing='X';
	pParaPlate->design.bend.widAngleBendBy = 1;	//火曲线依赖杆件
	pParaPlate->design.bend.ciBendMode = 3;		//火曲面定义方式:指定角钢肢法线
	pParaPlate->design.bend.normal.widBendPlaneRefRod1 = 5;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2 = 1;
	pParaPlate->connrods.Clear();
	//1-左侧水平杆
	pParaPlate->connrods[0].InitBasicInfo(1,true,1,6,false);
	pParaPlate->connrods[0].InitTipInfo(1,2);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2-左下斜杆
	pParaPlate->connrods[1].InitBasicInfo(2,false,1,3,false);
	pParaPlate->connrods[1].InitTipInfo(0,0,1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3-下侧竖杆
	pParaPlate->connrods[2].InitBasicInfo(3,true,1,2,false);
	pParaPlate->connrods[2].InitTipInfo(0,0,1,2);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4-右下侧斜杆
	pParaPlate->connrods[3].InitBasicInfo(4,true,1,6,false);
	pParaPlate->connrods[3].InitTipInfo(0,0,1,5);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5-右上斜杆
	pParaPlate->connrods[4].InitBasicInfo(5,true,2,3,false);
	pParaPlate->connrods[4].InitTipInfo(1,0,1);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	//6-左上斜杆
	pParaPlate->connrods[5].InitBasicInfo(6,false,2,1,false);
	pParaPlate->connrods[5].InitTipInfo(1,0,7,5);
	pParaPlate->connrods[5].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[5].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[5].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[5].layoutbolts.ReSize(1);
	//7-左上斜杆
	pParaPlate->connrods[6].InitBasicInfo(7,false,2,1,false);
	pParaPlate->connrods[6].InitTipInfo(1,0,1,5);
	pParaPlate->connrods[6].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[6].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[6].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[6].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(7);
}