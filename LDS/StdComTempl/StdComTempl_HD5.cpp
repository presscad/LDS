#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//HD5连接点模板参数
void CStdComTemplConnPoint::InitConnPointTempl_HD5()
{	//02010006
	name="HD5";//id 10
	szSketchImgFile = "HD5.bmp";
	description = "猫头曲臂外拐点处(上)";
	widBaseRod=1;		//以第一根杆件为节点父杆件
	widParentRod=1;	//以第一根杆件为连接定位依赖杆件
	this->xView.ciViewPlaneNormal='Y';
	xView.siMinBaseRodAngle=205;
	xView.siMaxBaseRodAngle=230;
	rodarr.ZeroPresetMemory();
	m_bEnableIntelliConnPointDesign = true;			//已通过审核验证的标准节点板
	//1.左下主杆
	InitSchemaRod(rodarr[0],1,-5,5,1,'X','R');
	rodarr[0].xFlag.bFilterWorkWing=true;
	//2.右下斜杆
	InitSchemaRod(rodarr[1],2,20,60,2,'X','R');
	rodarr[1].ciPlaneNo=2;
	//3.右下主杆
	InitSchemaRod(rodarr[2],3,70,100,1,'X','R');
	rodarr[2].ciPlaneNo=2;
	//4.左上主杆
	InitSchemaRod(rodarr[3],4,210,250,1,'Y','R');
	//5.左上斜杆
	InitSchemaRod(rodarr[4],5,260,300,2,'X','R');
	rodarr[4].xFlag.bFilterWorkWing=true;
	//6.左水平杆
	InitSchemaRod(rodarr[5],6,310,350,1,'X','R');
	rodarr[5].xFlag.bFilterWorkWing=true;
	rodarr.ReSize(6);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD5);
	pMatchPlate->InitDefaultMatchRods(6);
}

//HD5连接板模板参数
void CStdComTempl::InitNodePlateTemp_HD5()
{
	szSketchImgFile = "PHD5.bmp";
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
	//1.左下主杆
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//2.左下斜杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	pParaAngle->desStart.szYorOthrWingOffset="g";
	//3.右下主杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//4.左上主杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//5.左上斜杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";	//'['表示外铁;']'表示内铁，无指定表示所有
	pParaAngle->desStart.szYorOthrWingOffset="g";
	//6.左水平杆
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(6,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
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
	pParaPlate->design.bend.normal.widBendPlaneRefRod1 = 1;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2 = 3;
	//1.左下主杆
	pParaPlate->connrods[0].InitBasicInfo(1,true,1,4,false);
	pParaPlate->connrods[0].InitTipInfo(0,2);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.左下斜杆
	pParaPlate->connrods[1].InitBasicInfo(2,false,2,1,false);
	pParaPlate->connrods[1].InitTipInfo(0,0,1,3);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.右下主杆
	pParaPlate->connrods[2].InitBasicInfo(3,true,2,2,false);
	pParaPlate->connrods[2].InitTipInfo(0,0,1);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.左上主杆
	pParaPlate->connrods[3].InitBasicInfo(4,true,1,3,false);
	pParaPlate->connrods[3].InitTipInfo(0,0,1);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.左上斜杆
	pParaPlate->connrods[4].InitBasicInfo(5,false,1,1,false);
	pParaPlate->connrods[4].InitTipInfo(0,0,4,1);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	//6.左水平杆
	pParaPlate->connrods[5].InitBasicInfo(6,true,1,2,false);
	pParaPlate->connrods[5].InitTipInfo(0,0,1,4);
	pParaPlate->connrods[5].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[5].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[5].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[5].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(6);
}