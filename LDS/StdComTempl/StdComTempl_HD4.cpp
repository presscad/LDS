#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//HD4连接点模板参数
void CStdComTemplConnPoint::InitConnPointTempl_HD4()
{
	name = "HD-04";
	szSketchImgFile = "HD4.bmp";
	description="猫头曲臂内拐点处";
	widBaseRod=1;		//以第一根杆件为节点父杆件
	widParentRod=1;	//以第一根杆件为连接定位依赖杆件
	this->ciBaseRodConnState=0;	//无限制，一般是2.衔接段末端连接
	xView.ciBaseAxisXMode=0;	//始->终方向
	xView.ciViewPlaneNormal='Y';
	xView.siMinBaseRodAngle=200;
	xView.siMaxBaseRodAngle=225;
	rodarr.ZeroPresetMemory();
	m_bEnableIntelliConnPointDesign = true;			//已通过审核验证的标准节点板
	//1.左下斜杆
	InitSchemaRod(rodarr[0],1,0,10,1,'Y','R');
	//2.下斜杆
	InitSchemaRod(rodarr[1],2,50,85,1,'X','R');
	//3.右下辅杆
	InitSchemaRod(rodarr[2],3,85,125,2,'X','R');
	//3.水平横杆
	InitSchemaRod(rodarr[3],4,140,165,1,'X','R');
	//4.右上斜杆
	InitSchemaRod(rodarr[4],5,230,250,1,'Y','R');
	rodarr.ReSize(5);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD4);
	pMatchPlate->InitDefaultMatchRods(5);
	pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD4_2);
	pMatchPlate->InitDefaultMatchRods(5);
}

//HD4连接板模板参数
void CStdComTempl::InitNodePlateTemp_HD4(bool blShrinkRodTrue2False1/*=true*/)
{
	short idBaseOddRefRod = blShrinkRodTrue2False1 ? 1 : 2;
	if (blShrinkRodTrue2False1)
		szSketchImgFile = "PHD4.bmp";
	else
		szSketchImgFile = "PHD4-2.bmp";
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
	//1.左下斜杆
	CStdTemplParaAngle* pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//2.下竖杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	//3.右下斜杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	//4.水平横杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//5.右上斜杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//普通连接板
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Add(1);
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.左下斜杆
	pParaPlate->connrods[0].InitBasicInfo(1,true,1,3,false);
	if(blShrinkRodTrue2False1)
		pParaPlate->connrods[0].InitTipInfo(0,2);
	else
		pParaPlate->connrods[0].InitTipInfo(0,0,2);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.下竖杆
	pParaPlate->connrods[1].InitBasicInfo(2,true,1,3,false);
	if (blShrinkRodTrue2False1)
		pParaPlate->connrods[1].InitTipInfo(0,0,1);
	else
		pParaPlate->connrods[1].InitTipInfo(0,2);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.右下斜杆
	pParaPlate->connrods[2].InitBasicInfo(3,false,1,1,false);
	pParaPlate->connrods[2].InitTipInfo(0,0,2,4);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.水平横杆
	pParaPlate->connrods[3].InitBasicInfo(4,true,1,3,false);
	pParaPlate->connrods[3].InitTipInfo(0,0,idBaseOddRefRod);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.右上斜杆
	pParaPlate->connrods[4].InitBasicInfo(5,true,1,3,false);
	pParaPlate->connrods[4].InitTipInfo(1,0,idBaseOddRefRod,4);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}