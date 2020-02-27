#include "stdafx.h"
#include "Tower.h"
#include "TubeWalkWayTempl.h"

//////////////////////////////////////////////////////////////////////////
//CTubeWalkWayStdComTempl
CTubeWalkWayStdComTempl::CTubeWalkWayStdComTempl()
{
}
CTubeWalkWayStdComTempl::~CTubeWalkWayStdComTempl()
{

}
void CTubeWalkWayStdComTempl::InitComponentTempl(TEMPL_SOURCE* pTemplSource/*=NULL*/)
{
	szSketchImgFile = "WalkWay.bmp";
	//初始化设计参数
	InitTemplParams();
	//添加模板中的杆件定义区
	CStdTemplParaRod* pParaRod=listParaRods.Add(1,CLS_LINETUBE);	//依赖钢管
	pParaRod->m_bNecessaryRod=true;
	pParaRod->m_bNeedUpdatePosition=false;
	pParaRod->m_bNeedUpdateNormal=false;
	//添加模板中节点定义区
	CStdTemplParaNode* pParaNode=listParaNodes.Add(1);	//走道设计基准节点
	pParaNode->m_bNecessaryNode=true;
	pParaNode=listParaNodes.Add(KEY4C("S"));
	pParaNode->m_bNecessaryNode=true;
	pParaNode=listParaNodes.Add(KEY4C("E"));
	pParaNode->m_bNecessaryNode=true;
	//
	InitWalkWayTempl();								//角钢走道
	IStdComTempl::TEMPL_SOURCE source(0,1,1);
	if(pTemplSource==NULL)
		pTemplSource=&source;
	InitHolderTempl(pTemplSource->dwTemplParam);	//走道支架				
}
void CTubeWalkWayStdComTempl::InitTemplParams()
{
	//走道设计参数
	STDTEMPL_PARAM_ITEM* pParam=NULL;
	pParam=listParams.Add(KEY4C("W"));	//走道宽度
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=this->m_fWidth;
	pParam=listParams.Add(KEY4C("L"));	//走道长度
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=this->m_fLength;
	pParam=listParams.Add(KEY4C("YA"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fYA;	//横向偏移
	pParam=listParams.Add(KEY4C("ZH"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fZH;	//竖向高度偏移
	pParam=listParams.Add(KEY4C("Ls"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=(m_fZjOffDist-m_fZdOffDist);	//走道始端轴向偏移距离
	//支架设计参数
	pParam=listParams.Add(KEY4C("Zh"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fZh;	//斜撑下侧Z向偏移
	pParam=listParams.Add(KEY4C("D1"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fD1;	//斜撑下侧横向偏移
	pParam=listParams.Add(KEY4C("D2"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fD2;	//斜撑上侧端点沿水平杆向内的横向偏移
	pParam=listParams.Add(KEY4C("YB"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fYB;	//支架位置轴向偏移距离
	pParam=listParams.Add(KEY4C("X0"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fZjOffDist;	//支架位置轴向偏移距离
	//
	pParam=listParams.Add(KEY4C("Md"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;
	//pParam->value.liValue=this->m_nBoltMd;	//钢管上连接板上侧横螺直径
	pParam=listParams.Add(KEY4C("BN"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=this->m_nBoltNum;	//钢管上连接板上侧横螺栓数
	pParam=listParams.Add(KEY4C("BS"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=this->m_fBoltSpace;//钢管上连接板上侧横螺栓间距
	//走道板设计参数
	pParam=listParams.Add(KEY4C("PA"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=para.A;	//走道焊接板扇形夹角
	pParam=listParams.Add(KEY4C("PB"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=para.B;	//螺栓连接板扇形夹角
	pParam=listParams.Add(KEY4C("PW"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=para.W;	//
	//装配坐标系
	pParam=listParams.Add(KEY4C("acsO"));	//标准组件装配坐标系原点
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_COORD3D;
	pParam->value.objtype.wiPropValue=0;
	//strcpy(pParam->exprstr,"Node_Pos(1)");
	strcpy(pParam->exprstr,"RodNode_Pos(1,1)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	pParam=listParams.Add(KEY4C("acsX"));	//标准组件装配坐标系X轴方向（未指定终点方向时，根据起始基准点由中心向外延伸）
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_COORD3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"Rod_LenV(1)");
	pParam=listParams.Add(KEY4C("acsY"));	//标准组件装配坐标系Y轴方向
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_COORD3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"auto");	//自动根据X轴及基准钢管的位置计算
	pParam=listParams.Add(KEY4C("acsZ"));	//标准组件装配坐标系Z轴方向
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_COORD3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"auto");	//自动根据X轴计算(保证向上即与模型坐标系的-Z方向一致)
}
void CTubeWalkWayStdComTempl::InitWalkWayTempl()
{
	//走道的控制节点
	CStdTemplParaNode* pParaNode=NULL;
	pParaNode=listParaNodes.Add(KEY4C("SI"));	//走道弦杆起始端内侧节点
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=WAY_INNER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"Ls");
	strcpy(pParaNode->posy_expression,"YA-W");
	strcpy(pParaNode->posz_expression,"ZH");

	pParaNode=listParaNodes.Add(KEY4C("EI"));	//走道弦杆起终端内侧节点
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=WAY_INNER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"Ls+L");
	strcpy(pParaNode->posy_expression,"YA-W");
	strcpy(pParaNode->posz_expression,"ZH");
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("SI");
	pParaNode->attachNodeArr[1]=KEY4C("S");
	pParaNode->attachNodeArr[2]=KEY4C("E");
	strcpy(pParaNode->offset_expression,"L");

	pParaNode=listParaNodes.Add(KEY4C("DI0"));	//走道内侧弦杆基准起点
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=WAY_INNER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"0");
	strcpy(pParaNode->posy_expression,"YA-W");
	strcpy(pParaNode->posz_expression,"ZH");
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("SI");
	pParaNode->attachNodeArr[1]=KEY4C("SI");
	pParaNode->attachNodeArr[2]=KEY4C("EI");
	strcpy(pParaNode->offset_expression,"-Ls");

	pParaNode=listParaNodes.Add(KEY4C("SO"));	//走道弦杆起始端外侧节点
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=WAY_OUTER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"Ls");
	strcpy(pParaNode->posy_expression,"YA");
	strcpy(pParaNode->posz_expression,"ZH");
	
	pParaNode=listParaNodes.Add(KEY4C("EO"));	//走道弦杆起始端外侧节点
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=WAY_OUTER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"Ls+L");
	strcpy(pParaNode->posy_expression,"YA");
	strcpy(pParaNode->posz_expression,"ZH");
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("SO");
	pParaNode->attachNodeArr[1]=KEY4C("S");
	pParaNode->attachNodeArr[2]=KEY4C("E");
	strcpy(pParaNode->offset_expression,"L");

	pParaNode=listParaNodes.Add(KEY4C("DO0"));	//走道外侧弦杆基准起点
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod= WAY_OUTER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"0");
	strcpy(pParaNode->posy_expression,"YA");
	strcpy(pParaNode->posz_expression,"ZH");
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("SO");
	pParaNode->attachNodeArr[1]=KEY4C("SO");
	pParaNode->attachNodeArr[2]=KEY4C("EO");
	strcpy(pParaNode->offset_expression,"-Ls");
	//角钢
	CStdTemplParaAngle* pParaJg1=(CStdTemplParaAngle*)listParaRods.Add(WAY_INNER_ANGLE_ID,CLS_LINEANGLE);
	pParaJg1->m_idPartClsType=CLS_LINEANGLE;
	pParaJg1->m_bNecessaryRod= true;
	pParaJg1->keyStartNode=KEY4C("SI");
	pParaJg1->keyEndNode=KEY4C("EI");
	pParaJg1->normal.uidRefRodOfWorkWing=WAY_OUTER_ANGLE_ID;
	pParaJg1->normal.ciNormalDirection='Z';
	pParaJg1->ciAngleLayout=1;
	pParaJg1->desStart.cTouchLineType=pParaJg1->desEnd.cTouchLineType=3;//控制点
	pParaJg1->desStart.coordCtrlType=pParaJg1->desEnd.coordCtrlType=3;	//控制点投影
	pParaJg1->desStart.szXorCtrlWingOffset="-$4#g";
	pParaJg1->desStart.szYorOthrWingOffset="g";
	pParaJg1->desEnd.szXorCtrlWingOffset="-$4#g";
	pParaJg1->desEnd.szYorOthrWingOffset="g";
	pParaJg1->m_bNeedUpdatePosition=pParaJg1->m_bNeedUpdateNormal=false;
	//
	CStdTemplParaAngle* pParaJg2=(CStdTemplParaAngle*)listParaRods.Add(WAY_OUTER_ANGLE_ID,CLS_LINEANGLE);
	pParaJg2->m_idPartClsType=CLS_LINEANGLE;
	pParaJg2->m_bNecessaryRod= true;
	pParaJg2->keyStartNode=KEY4C("SO");
	pParaJg2->keyEndNode=KEY4C("EO");
	pParaJg2->normal.uidRefRodOfWorkWing=WAY_INNER_ANGLE_ID;	//当前角钢工作肢与2号杆件共面
	pParaJg2->normal.ciNormalDirection='Z';	//从装配坐标系Z轴看角钢的肢朝向为4号布置型式
	pParaJg2->ciAngleLayout=4;
	pParaJg2->desStart.cTouchLineType=pParaJg2->desEnd.cTouchLineType=3;//控制点
	pParaJg2->desStart.coordCtrlType=pParaJg2->desEnd.coordCtrlType=3;	//控制点投影
	pParaJg2->desStart.szXorCtrlWingOffset="g";		//X肢法向偏移自身标准心距
	pParaJg2->desStart.szYorOthrWingOffset="-$4#g";	//Y肢法向偏移负的4号角钢的标准心距
	pParaJg2->desEnd.szXorCtrlWingOffset="g";
	pParaJg2->desEnd.szYorOthrWingOffset="-$4#g";
	pParaJg2->m_bNeedUpdatePosition=pParaJg1->m_bNeedUpdateNormal=false;
}
void CTubeWalkWayStdComTempl::InitHolderTempl(int iHolerType)
{
	CStdTemplParaNode* pParaNode=NULL;
	pParaNode=listParaNodes.Add(KEY4C("BN"));
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=1;
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=1;
	pParaNode->attachNodeArr[1]=KEY4C("S");
	pParaNode->attachNodeArr[2]=KEY4C("E");
	strcpy(pParaNode->offset_expression,"X0");
	//
	pParaNode=listParaNodes.Add(KEY4C("HI"));
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=WAY_INNER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"X0");
	strcpy(pParaNode->posy_expression,"YA-W");
	strcpy(pParaNode->posz_expression,"ZH");
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("DI0");
	pParaNode->attachNodeArr[1]=KEY4C("SI");
	pParaNode->attachNodeArr[2]=KEY4C("EI");
	strcpy(pParaNode->offset_expression,"X0");
	if(iHolerType>0)
	{	//上横撑角钢始终端
		pParaNode=listParaNodes.Add(KEY4C("HS"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->keyLabelFatherRod=WAY_OUTER_ANGLE_ID;
		strcpy(pParaNode->posx_expression,"X0");
		strcpy(pParaNode->posy_expression,"YA");
		strcpy(pParaNode->posz_expression,"ZH");
		pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
		pParaNode->attachNodeArr[0]=KEY4C("DO0");
		pParaNode->attachNodeArr[1]=KEY4C("SO");
		pParaNode->attachNodeArr[2]=KEY4C("EO");
		strcpy(pParaNode->offset_expression,"X0");

		pParaNode=listParaNodes.Add(KEY4C("HE"));
		pParaNode->m_bNecessaryNode=false;
		strcpy(pParaNode->posx_expression,"X0");
		strcpy(pParaNode->posy_expression,"0");
		strcpy(pParaNode->posz_expression,"ZH");
		pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
		pParaNode->attachNodeArr[0]=KEY4C("HI");
		pParaNode->attachNodeArr[1]=KEY4C("HS");
		pParaNode->attachNodeArr[2]=KEY4C("HI");
		strcpy(pParaNode->offset_expression,"YA-W");
	}
	//
	if(iHolerType==0)
		InitPlateHolderTempl();		//钢板支架
	else if(iHolerType==1)
		InitTriangleHolderTempl();	//三角支架
	else if(iHolerType==2)
		InitPlatformHolderTempl();	//台式支架
}
void CTubeWalkWayStdComTempl::InitPlateHolderTempl()
{
	//角钢连接板
	CStdTemplParaPlate* pParaPlate=listParaPlates.Add(1);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->design.idBaseNode=KEY4C("HI");
	pParaPlate->design.idBaseRod=WAY_INNER_ANGLE_ID;
	pParaPlate->design.ciProfileStyle=1;	//定制外形
	pParaPlate->design.ciDatumLenAxis='Y';
	pParaPlate->design.ciDatumAngleWing='Y';
	CONNECT_ROD* pConnRod=pParaPlate->connrods.Append(CONNECT_ROD(2));
	pConnRod->cnMinBoltN=2;
	pConnRod->ciConnectWing='Y';
	pConnRod->ciCurrTipSideS0E1=1;
	pConnRod->dualSideEdge=true;
	BOLT_LAYOUT* pLayout=pConnRod->layoutbolts.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HI");
	pLayout->cnCount=2;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing=pConnRod->ciConnectWing;
	pLayout->ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	//走道环向加劲板
	pParaPlate=listParaPlates.Add(2);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_W_TUBECIRPLATE;
	pParaPlate->design.idBaseNode=KEY4C("BN");
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.idBasePlate=1;
}
//三角式角钢支架模板
void CTubeWalkWayStdComTempl::InitTriangleHolderTempl()
{	//节点
	CStdTemplParaNode* pParaNode=NULL;
	pParaNode=listParaNodes.Add(KEY4C("XS"));
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->keyLabelFatherRod=4;
	pParaNode->attachNodeArr[0]=KEY4C("HS");
	pParaNode->attachNodeArr[1]=KEY4C("HS");
	pParaNode->attachNodeArr[2]=KEY4C("HE");
	strcpy(pParaNode->offset_expression,"D2");

	pParaNode=listParaNodes.Add(KEY4C("XE"));
	pParaNode->m_bNecessaryNode=false;
	strcpy(pParaNode->posx_expression,"X0");
	strcpy(pParaNode->posy_expression,"D1");
	strcpy(pParaNode->posz_expression,"-Zh");
	//横撑角钢
	CStdTemplParaAngle* pParaJg1=(CStdTemplParaAngle*)listParaRods.Add(BRACE_UPPER_HORI_ANGLE_ID,CLS_LINEANGLE);
	pParaJg1->m_idPartClsType=CLS_LINEANGLE;
	pParaJg1->m_bNecessaryRod= true;
	pParaJg1->keyStartNode=KEY4C("HS");
	pParaJg1->keyEndNode=KEY4C("HE");
	pParaJg1->normal.uidRefRodOfWorkWing=2;
	pParaJg1->normal.ciNormalDirection='Z';
	pParaJg1->ciAngleLayout=3;
	pParaJg1->desStart.cTouchLineType=0;
	pParaJg1->desStart.cTouchLine=1;	//交于走道外主梁角钢Y肢心线
	pParaJg1->desStart.uidTouchLineRod = 3;
	pParaJg1->desStart.szXorCtrlWingOffset[0]='0';
	pParaJg1->desStart.szYorOthrWingOffset[0]='g';
	pParaJg1->desEnd.cTouchLineType = 3;//控制点
	pParaJg1->desEnd.coordCtrlType = 3;
	pParaJg1->desEnd.szXorCtrlWingOffset[0]='g';
	pParaJg1->desEnd.szYorOthrWingOffset[0]='g';
	pParaJg1->wiStartOddCalStyle=pParaJg1->wiEndOddCalStyle=1;	//根据螺栓计算正负头
	//始端螺栓布置参数
	BOLT_LAYOUT* pLayout=NULL;
	pLayout=pParaJg1->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HS");
	pLayout->basepoint.widBaseAngle=WAY_OUTER_ANGLE_ID;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;	//始到终
	//中间螺栓布置
	pLayout=pParaJg1->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HI");
	pLayout->basepoint.widBaseAngle=WAY_INNER_ANGLE_ID;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;	//始到终
	//终端螺栓布置参数
	pLayout=pParaJg1->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HE");
	pLayout->cnCount=0;		//从参数中获取
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='Y';
	pLayout->ciFlowDirection=BOLT_LAYOUT::FLOW_EtoS;	//终到始
	//斜撑角钢
	CStdTemplParaAngle* pParaJg2=(CStdTemplParaAngle*)listParaRods.Add(BRACE_GRADIENT_ANGLE_ID,CLS_LINEANGLE);
	pParaJg2->m_idPartClsType=CLS_LINEANGLE;
	pParaJg2->m_bNecessaryRod= true;
	pParaJg2->keyStartNode=KEY4C("XS");
	pParaJg2->keyEndNode=KEY4C("XE");
	pParaJg2->normal.uidRefRodOfWorkWing=BRACE_UPPER_HORI_ANGLE_ID;
	pParaJg2->normal.ciNormalDirection='x';
	pParaJg2->ciAngleLayout=1;
	pParaJg2->desStart.cTouchLineType=0;
	pParaJg2->desStart.cTouchLine=1;
	pParaJg2->desStart.uidTouchLineRod = 4;
	pParaJg2->desStart.coordCtrlType=3;
	pParaJg2->desStart.szXorCtrlWingOffset="-R4#T";//"g";
	pParaJg2->desStart.szYorOthrWingOffset="g";//"-g,-P2#T";
	pParaJg2->desEnd.cTouchLineType=3;	//控制点
	pParaJg2->desEnd.coordCtrlType=3;	//控制点投影
	pParaJg2->desEnd.szXorCtrlWingOffset="g";
	pParaJg2->desEnd.szYorOthrWingOffset="g";//"-g,-P2#T";
	pParaJg2->wiStartOddCalStyle=pParaJg2->wiEndOddCalStyle=1;	//根据螺栓计算正负头
	//始端螺栓布置参数
	pLayout=pParaJg2->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("XS");
	pLayout->basepoint.widBaseAngle=4;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;	//始到终
	//终端螺栓布置参数
	pLayout=pParaJg2->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("XE");
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=BOLT_LAYOUT::FLOW_EtoS;	//始到终
	//上侧走道连接板
	CStdTemplParaPlate* pParaPlate=listParaPlates.Add(1);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_W_CIRBOLTPLATE;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.idBaseNode=KEY4C("BN");
	pParaPlate->design.idBasePlate=-4;
	//下侧走道连接板
	pParaPlate=listParaPlates.Add(2);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_W_CIRBOLTPLATE;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.idBaseNode=KEY4C("BN");
	pParaPlate->design.idBasePlate=-5;
	pParaPlate->design.idAssistPart=4;
}
void CTubeWalkWayStdComTempl::InitPlatformHolderTempl()
{
	//节点
	CStdTemplParaNode* pParaNode=NULL;
	pParaNode=listParaNodes.Add(KEY4C("hS"));
	pParaNode->m_bNecessaryNode=false;
	strcpy(pParaNode->posx_expression,"X0");
	strcpy(pParaNode->posy_expression,"YB+D1");
	strcpy(pParaNode->posz_expression,"-Zh");

	pParaNode=listParaNodes.Add(KEY4C("hE"));
	pParaNode->m_bNecessaryNode=false;
	strcpy(pParaNode->posx_expression,"X0");
	strcpy(pParaNode->posy_expression,"0");
	strcpy(pParaNode->posz_expression,"-Zh");

	pParaNode=listParaNodes.Add(KEY4C("XS"));
	pParaNode->keyLabelFatherRod=4;
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("HS");
	pParaNode->attachNodeArr[1]=KEY4C("HS");
	pParaNode->attachNodeArr[2]=KEY4C("HE");
	strcpy(pParaNode->offset_expression,"D2");

	pParaNode=listParaNodes.Add(KEY4C("VS"));
	pParaNode->keyLabelFatherRod=4;
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("HE");
	pParaNode->attachNodeArr[1]=KEY4C("HE");
	pParaNode->attachNodeArr[2]=KEY4C("HS");
	strcpy(pParaNode->offset_expression,"YB");

	pParaNode=listParaNodes.Add(KEY4C("VE"));
	pParaNode->keyLabelFatherRod=6;
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("hE");
	pParaNode->attachNodeArr[1]=KEY4C("hE");
	pParaNode->attachNodeArr[2]=KEY4C("hS");
	strcpy(pParaNode->offset_expression,"YB");
	//上横撑角钢
	CStdTemplParaAngle* pParaJg1=(CStdTemplParaAngle*)listParaRods.Add(BRACE_UPPER_HORI_ANGLE_ID,CLS_LINEANGLE);
	pParaJg1->m_idPartClsType=CLS_LINEANGLE;
	pParaJg1->m_bNecessaryRod= true;
	pParaJg1->keyStartNode=KEY4C("HS");
	pParaJg1->keyEndNode=KEY4C("HE");
	pParaJg1->normal.uidRefRodOfWorkWing=2;
	pParaJg1->normal.ciNormalDirection='Z';
	pParaJg1->ciAngleLayout=3;
	pParaJg1->desStart.cTouchLineType = 0;
	pParaJg1->desStart.cTouchLine = 1;	//交于走道外主梁角钢Y肢心线
	pParaJg1->desStart.szXorCtrlWingOffset[0] = '0';
	pParaJg1->desStart.szYorOthrWingOffset[0] = 'g';
	pParaJg1->desEnd.cTouchLineType = 3;//控制点
	pParaJg1->desEnd.coordCtrlType = 3;
	pParaJg1->desEnd.szXorCtrlWingOffset[0] = 'g';
	pParaJg1->desEnd.szYorOthrWingOffset[0] = 'g';
	pParaJg1->wiStartOddCalStyle=pParaJg1->wiEndOddCalStyle=1;	//根据螺栓计算正负头
	//始端螺栓布置参数
	BOLT_LAYOUT* pLayout=NULL;
	pLayout=pParaJg1->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HS");
	pLayout->basepoint.widBaseAngle=WAY_OUTER_ANGLE_ID;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=0;	//始到终
	//中间螺栓布置
	pLayout=pParaJg1->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HI");
	pLayout->basepoint.widBaseAngle=WAY_INNER_ANGLE_ID;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=0;	//始到终
	//终端螺栓布置参数
	pLayout=pParaJg1->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HE");
	pLayout->cnCount=0;		//从参数中获取
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='Y';
	pLayout->ciFlowDirection=1;	//终到始
	//斜撑角钢
	CStdTemplParaAngle* pParaJg2=(CStdTemplParaAngle*)listParaRods.Add(BRACE_GRADIENT_ANGLE_ID,CLS_LINEANGLE);
	pParaJg2->m_idPartClsType=CLS_LINEANGLE;
	pParaJg2->m_bNecessaryRod= true;
	pParaJg2->keyStartNode=KEY4C("XS");
	pParaJg2->keyEndNode=KEY4C("hS");
	pParaJg2->normal.uidRefRodOfWorkWing=4;
	pParaJg2->normal.ciNormalDirection='x';
	pParaJg2->ciAngleLayout=1;
	pParaJg2->desStart.cTouchLineType=3;
	pParaJg2->desStart.coordCtrlType=3;
	pParaJg2->desStart.szXorCtrlWingOffset="g,-$4#T";;
	pParaJg2->desStart.szYorOthrWingOffset='g';
	pParaJg2->desEnd.cTouchLineType=3;	//控制点
	pParaJg2->desEnd.coordCtrlType=3;	//控制点投影
	pParaJg2->desEnd.szXorCtrlWingOffset='g';
	pParaJg2->desEnd.szYorOthrWingOffset='g';
	pParaJg2->wiStartOddCalStyle=1;	//根据螺栓计算正负头
	//始端螺栓布置参数
	pLayout=pParaJg2->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("XS");
	pLayout->basepoint.widBaseAngle=4;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=0;	//始到终
	//下横撑角钢
	CStdTemplParaAngle* pParaJg3=(CStdTemplParaAngle*)listParaRods.Add(BRACE_DOWN_HORI_ANGLE_ID,CLS_LINEANGLE);
	pParaJg3->m_idPartClsType=CLS_LINEANGLE;
	pParaJg3->m_bNecessaryRod= true;
	pParaJg3->keyStartNode=KEY4C("hS");
	pParaJg3->keyEndNode=KEY4C("hE");
	pParaJg3->normal.uidRefRodOfWorkWing=2;
	pParaJg3->normal.ciNormalDirection='Z';
	pParaJg3->ciAngleLayout=4;
	pParaJg3->desStart.cTouchLineType=pParaJg3->desEnd.cTouchLineType=3;//控制点
	pParaJg3->desStart.coordCtrlType=pParaJg3->desEnd.coordCtrlType=3;	//控制点投影
	pParaJg3->desStart.szXorCtrlWingOffset[0]='g';
	pParaJg3->desStart.szYorOthrWingOffset[0]='g';
	pParaJg3->desEnd.szXorCtrlWingOffset[0] = 'g';
	pParaJg3->desEnd.szYorOthrWingOffset[0] = 'g';
	pParaJg3->wiEndOddCalStyle=1;	//根据螺栓计算正负头
	//
	pLayout=pParaJg3->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("hE");
	pLayout->cnCount=0;		//从参数中获取
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=1;	//终到始
	//竖向角钢
	CStdTemplParaAngle* pParaJg4=(CStdTemplParaAngle*)listParaRods.Add(7,CLS_LINEANGLE);
	pParaJg4->m_idPartClsType=CLS_LINEANGLE;
	pParaJg4->m_bNecessaryRod= true;
	pParaJg4->keyStartNode=KEY4C("VS");
	pParaJg4->keyEndNode=KEY4C("VE");
	pParaJg4->normal.uidRefRodOfWorkWing=4;
	pParaJg4->normal.ciNormalDirection='X';
	pParaJg4->ciAngleLayout=3;
	pParaJg4->desStart.cTouchLineType=0;
	pParaJg4->desStart.cTouchLine=1;
	pParaJg4->desStart.uidTouchLineRod = 4;
	pParaJg4->desStart.szXorCtrlWingOffset="-$4#T";
	pParaJg4->desStart.szYorOthrWingOffset="g";
	pParaJg4->desEnd.cTouchLineType=0;
	pParaJg4->desEnd.cTouchLine=0;
	pParaJg4->desStart.uidTouchLineRod = 6;
	pParaJg4->desEnd.szXorCtrlWingOffset="-$6#T";
	pParaJg4->desEnd.szYorOthrWingOffset="g";
	pParaJg4->wiStartOddCalStyle=pParaJg4->wiEndOddCalStyle=1;	//根据螺栓计算正负头
	//始端螺栓
	pLayout=pParaJg4->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("VS");
	pLayout->basepoint.widBaseAngle=BRACE_UPPER_HORI_ANGLE_ID;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=0;	//始到终
	//终端螺栓
	pLayout=pParaJg4->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("hS");
	pLayout->basepoint.widBaseAngle=6;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=0;	//始到终
	//上侧走道连接板
	CStdTemplParaPlate* pParaPlate=listParaPlates.Add(1);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_W_CIRBOLTPLATE;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.idBaseNode=KEY4C("BN");
	pParaPlate->design.idBasePlate=-4;	//正:连接始端|负:连接终端
	//下侧走道连接板
	pParaPlate=listParaPlates.Add(2);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_W_CIRBOLTPLATE;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.idBaseNode=KEY4C("BN");
	pParaPlate->design.idBasePlate=-6;	//正:连接始端|负:连接终端
}
bool CTubeWalkWayStdComTempl::IsMeetConditions()
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
//
CTubeWalkWayStdComModel::CTubeWalkWayStdComModel(IModel* pModel):CStdComModel(pModel)
{
}
CTubeWalkWayStdComModel::~CTubeWalkWayStdComModel(void)
{

}
bool CTubeWalkWayStdComModel::BuildAcsOfAssmComponent()
{
	STDTEMPL_PARAM_ITEM* pParam;
	CStdComTemplExpression expr(m_pStdTempl);
	m_xNearAxisZ.Set(0,0,-1);
	//标准组件装配坐标系原点
	pParam=m_pStdTempl->listParams.GetValue(KEY4C("acsO"));
	int ciOrgFuncType=0;
	if(strstr(pParam->exprstr,"RodNode_Pos")!=NULL)
		ciOrgFuncType=2;
	else if(strstr(pParam->exprstr,"Node_Pos")!=NULL)
		ciOrgFuncType=1;
	else
		return false;
	char* pszParamStart=strchr(pParam->exprstr,'(');
	if(pszParamStart!=NULL)
		pszParamStart++;
	char* pszParamEnd=strchr(pParam->exprstr,')');
	if(pszParamEnd!=NULL)
	{
		CXhChar16 szParam;
		szParam.NCopy(pszParamStart,pszParamEnd-pszParamStart);
		EXPRESSION_FUNC* pExprFunc=ciOrgFuncType==1?expr.FromFunctionName("Node_Pos"):expr.FromFunctionName("RodNode_Pos");
		pExprFunc->Calc(&pParam->value,szParam,&expr);
		acs.origin.Set(pParam->value.coord3d[0],pParam->value.coord3d[1],pParam->value.coord3d[2]);
	}
	pParam=m_pStdTempl->listParams.GetValue(KEY4C("acsX"));
	if(strstr(pParam->exprstr,"Rod_LenV")==NULL)
		return false;
	pszParamStart=strchr(pParam->exprstr,'(');
	if(pszParamStart!=NULL)
		pszParamStart++;
	pszParamEnd=strchr(pParam->exprstr,')');
	if(pszParamEnd!=NULL)
	{
		CXhChar16 szParam;
		szParam.NCopy(pszParamStart,pszParamEnd-pszParamStart);
		EXPRESSION_FUNC* pExprFunc=expr.FromFunctionName("Rod_LenV");
		pExprFunc->Calc(&pParam->value,szParam,&expr);
		acs.axis_x.Set(pParam->value.coord3d[0],pParam->value.coord3d[1],pParam->value.coord3d[2]);
	}
	//根据基准杆件所在位置调整坐标系
	GEPOINT nearVecX(1,0,0),nearVecY(0,1,0);
	CLDSLinePart* pBaseRod=GetLdsRodFromId(1);
	if(pBaseRod)
	{	
		GEPOINT ptS=pBaseRod->xPosStart,ptE=pBaseRod->xPosEnd;
		if(fabs(acs.axis_x.x)>fabs(acs.axis_x.y))
		{	//X轴向走道
			if(ptS.x>0&&ptE.x>0)		//右侧横担
				nearVecX.Set(1,0,0);
			else if(ptS.x<0&&ptE.x<0)	//左侧横担
				nearVecX.Set(-1,0,0);
			else if(ptS.x*ptE.x<0)		//塔身横隔面
				nearVecX=(ptS.y>0)?GEPOINT(1,0,0):GEPOINT(-1,0,0);
			if(ptS.y>0&&ptE.y>0)
				nearVecY.Set(0,-1,0);
			else
				nearVecY.Set(0,1,0);
		}
		else
		{	//Y轴向走道
			if(ptS.y>0&&ptE.y>0)		//前侧横担
				nearVecX.Set(0,1,0);
			else if(ptS.y<0&&ptE.y<0)	//后侧横担
				nearVecX.Set(0,-1,0);
			else if(ptS.y*ptE.y<0)		//塔身横隔面
				nearVecX=(ptS.x>0)?GEPOINT(0,-1,0):GEPOINT(0,1,0);
			if(ptS.x>0&&ptE.x>0)
				nearVecY.Set(-1,0,0);
			else
				nearVecY.Set(1,0,0);
		}
	}
	if(acs.axis_x*nearVecX<0)
		acs.axis_x*=-1;
	acs.axis_y=acs.axis_x^GEPOINT(0,0,1);
	normalize(acs.axis_x);
	normalize(acs.axis_y);
	if(acs.axis_y*nearVecY<0)
		acs.axis_y*=-1;
	normalize(acs.axis_y);
	acs.axis_z=acs.axis_x^acs.axis_y;
	if (acs.axis_z.z>0)
	{	//走道平面坐标系Z轴始终朝上，即模型坐标系Z轴负向 wjh-2019.9.22
		acs.axis_z*=-1.0;
		acs.axis_y*=-1.0;
	}
	normalize(acs.axis_z);
	return true;
}
void CTubeWalkWayStdComModel::UpdateNodeAndRodBaseInfo()
{
	CTubeWalkWayStdComTempl* pTempl=(CTubeWalkWayStdComTempl*)m_pStdTempl;
	CLDSLinePart* pBaseTube=GetLdsRodFromId(1);
	if(pBaseTube==NULL||pBaseTube->GetClassTypeId()!=CLS_LINETUBE)
		return;
	for(CStdTemplParaNode* pParaNode=m_pStdTempl->listParaNodes.GetFirst();pParaNode;pParaNode=m_pStdTempl->listParaNodes.GetNext())
	{
		if(pParaNode->m_bNecessaryNode)
			continue;
		CLDSNode* pNode=GetLdsNodeFromId(pParaNode->Id);
		if(pNode==NULL)
		{
			logerr.Log("%d丢失节点信息",pParaNode->Id);
			continue;
		}
		pNode->SetLayer("TPG");
		pNode->iSeg=pBaseTube->iSeg;
		pNode->cfgword=pBaseTube->cfgword;
		if(pNode->m_cPosCalType==CLDSNode::COORD_OFFSET)
		{
			pNode->SetArcLift(pBaseTube->ArcLiftHandle());
			pNode->CalPosition(true);
		}
	}
	for(CStdTemplParaRod* pParaRod=m_pStdTempl->listParaRods.GetFirst();pParaRod;pParaRod=m_pStdTempl->listParaRods.GetNext())
	{
		CLDSLinePart* pRod=GetLdsRodFromId(pParaRod->Id);
		if(pRod==NULL)
		{
			logerr.Log("%d丢失杆件信息",pParaRod->Id);
			continue;
		}
		if(pRod->GetClassTypeId()==CLS_LINETUBE)
			continue;
		pRod->SetLayer("TFG");
		pRod->iSeg=pBaseTube->iSeg;
		pRod->cfgword=pBaseTube->cfgword;
		pRod->SetArcLift(pBaseTube->ArcLiftHandle());
		pRod->CalPosition(true);
		//更新螺栓基本信息
		for(CLsRef* pLsRef=pRod->GetFirstLsRef();pLsRef;pLsRef=pRod->GetNextLsRef())
		{
			CLDSBolt* pBolt=pLsRef->GetLsPtr();
			pBolt->iSeg=pBaseTube->iSeg;
			pBolt->cfgword=pBaseTube->cfgword;
		}
	}
	for(CStdTemplParaPlate* pParaPlate=m_pStdTempl->listParaPlates.GetFirst();pParaPlate;pParaPlate=m_pStdTempl->listParaPlates.GetNext())
	{
		CLDSGeneralPlate* pPlate=GetLdsPlateFromId(pParaPlate->Id);
		if(pPlate==NULL)
		{
			logerr.Log("%d丢失钢板信息",pParaPlate->Id);
			continue;
		}
		pPlate->iSeg=pBaseTube->iSeg;
		pPlate->cfgword=pBaseTube->cfgword;
	}
}
bool CTubeWalkWayStdComModel::BuildModel(CStdComTempl* pStdTempl)
{
	if(CStdComModel::BuildModel(pStdTempl))
		UpdateNodeAndRodBaseInfo();
	return true;
}
