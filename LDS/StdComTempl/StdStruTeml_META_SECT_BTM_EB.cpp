#include "stdafx.h"
#include "..\LDS.h"
#include "NodeStdComTempl.h"
#include "WireStruComTempl.h"
#include "Fitting.h"

//初始化模板参数
//IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_EB
void CStdComTemplWireStructure::InitComTempl_META_SECT_BTM_1_EB()
{
	ClearTmplParams();
	name.Copy("直线塔横隔材单EB挂点");
	szSketchImgFile.Copy("CZC-EB1.bmp");
	this->m_bTmplInitialized=true;
	//设定挂线架的识别关键节点或杆件
	xarrKeyRodNodes.ReSize(1);
	xarrKeyRodNodes[0].ciNode0Rod1=MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod=1;	//通过挂线端节点识别
	//设定挂孔金具信息
	FITTING_EB *pEBInfo = QueryFitting<FITTING_EB>("EB-16/21-100");
	if (pEBInfo == NULL)
	{
		IFittingLibrary<FITTING_EB>* pEBLib = FittingLibrary<FITTING_EB>();
		pEBInfo = pEBLib->GetAt(0);
	}
	FITTING_UR* pURInfo = MatchFitting<FITTING_UR>(pEBInfo->dfHoleD);
	if (pURInfo == NULL)
	{
		IFittingLibrary<FITTING_UR>* pURLib = FittingLibrary<FITTING_UR>();
		pURInfo = pURLib->GetAt(0);
	}
	m_cnWireGroupCount = 1;
	xarrWireGroups[0].cnWirePointCount = 1;
	xarrWireGroups[0].wiSpaceL = 0;
	xarrWireGroups[0].ciFittingType = CLDSFitting::TYPE_EB;
	xarrWireGroups[0].cnShaftD = ftoi(pEBInfo->dfTrunnionD);
	xarrWireGroups[0].wnFittingWidth = ftoi(pEBInfo->dfBoardL);
	StrCopy(xarrWireGroups[0].szFittingSizeCode, pEBInfo->szSizeCode, 17);
	StrCopy(xarrWireGroups[0].szUringSizeCode, pURInfo->szSizeCode, 17);
	//1.标准组件设计参数定义区
	STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"WorkPlaneCS(1,1,'Z')");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//金具宽度
	pParam=listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("T型挂线角钢间隙, EB挂点金具宽度+间隙，mm");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=xarrWireGroups[0].wnReservedWidth;	//不能给固定值84，否则可能与金具冲突
	//挂线短角钢的规格
	pParam=listParams.Add(KEY4C("WAS"));
	pParam->sLabel.Copy("WIREANGLE_SIZE");
	pParam->sNotes.Copy("T型挂线角钢规格，mm");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_STRING;
	StrCopy(pParam->value.szStrBuf,"125x10",96);
	//挂线短角钢的固接螺栓
	pParam=listParams.Add(KEY4C("FBN"));
	pParam->sLabel.Copy("FIX_CONN_BOLTS_N");
	pParam->sNotes.Copy("挂线短角钢的固接螺栓数");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;
	pParam->value.liValue=this->m_cnFixWireAngleConnBoltsN=2;
	//2.模型节点杆件描述
	CStdTemplParaNode* pParaNode=listParaNodes.Add(N10_ID);
	pParaNode->m_bNecessaryNode = true;
	pParaNode=listParaNodes.Add(N20_ID);
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::SCALE;
	pParaNode->attach_scale=0.5;
	pParaNode->attachNodeArr[0]= N10_ID;//2;
	pParaNode->attachNodeArr[1]=TMPLOBJID(N10_ID,'X');
	pParaNode->keyLabelFatherRod=1;
	//杆件参数
	const int ANG_ARMPOST = 2;
	//1号为挂线角钢的连接基准T型组合角钢
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->idStartNode=N10_ID;
	pParaAngle->idEndNode=TMPLOBJID(N10_ID,'X');
	pParaAngle->ciSectStyle='T';
	pParaAngle->ciMasterIndex=1;
	pParaAngle->ciAngleLayout=3;
	pParaAngle->normal.ciNormalDirection='Z';
	pParaAngle->normal.uidRefRodOfWorkWing = ANG_ARMPOST;
	pParaAngle->m_bNeedUpdatePosition = false;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->desStart.uidTouchLineRod = ANG_ARMPOST;
	pParaAngle->desStart.szXorCtrlWingOffset="-$2#T";	//表示负角钢2的肢厚
	pParaAngle->desStart.szYorOthrWingOffset[0]='0';
	pParaAngle->desEnd.uidTouchLineRod = TMPLOBJID(ANG_ARMPOST, 'X');
	pParaAngle->desEnd.szXorCtrlWingOffset="-$2#T";	//表示负角钢2的肢厚
	pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
	//横担弦杆主材，作为输入条件用于纠正1号T型组合角钢的肢朝向 wjh-2019.9.10
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_ARMPOST, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->m_bNecessaryRod = false;
	//3、4号为底面前后侧挂线两根外贴挂线短角钢
	const int ANG_FL=3;
	const int ANG_BL=4;
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(ANG_FL,CLS_LINEANGLE);
	pParaAngle->idStartNode= N20_ID;//2;
	pParaAngle->ciSectStyle='L';
	pParaAngle->blShortAngle=true;
	pParaAngle->attachangle.idBaseAngle=1;		//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode=N20_ID;
	pParaAngle->attachangle.ciDatumAngleWing='X';
	pParaAngle->attachangle.ciNormalOffset=0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout=2;	//垂向朝外贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection='Z';	//始->终与'X'轴近似
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->m_bNeedUpdateNormal=true;
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//根据螺栓位置计算
	//pParaAngle->normal.ciNormalDirection='Z';
	//pParaAngle->normal.uidRefRodOfWorkWing=2;
	pParaAngle->desStart.szYorOthrWingOffset="-P1#T";	//表示负角钢2的肢厚
	pParaAngle->desStart.szXorCtrlWingOffset[0]='0';
	pParaAngle->desEnd.szYorOthrWingOffset="-P1#T";	//表示负角钢2的肢厚
	pParaAngle->desEnd.szXorCtrlWingOffset[0]='0';
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(ANG_BL,CLS_LINEANGLE);
	pParaAngle->idStartNode= N20_ID;
	//pParaAngle->idEndNode  = TMPLOBJID(N20_ID,'X');//-2;
	pParaAngle->ciSectStyle='L';
	pParaAngle->blShortAngle = true;
	pParaAngle->attachangle.idBaseAngle=1;		//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode=N20_ID;
	pParaAngle->attachangle.ciDatumAngleWing='X';
	pParaAngle->attachangle.ciNormalOffset=0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout=3;	//垂向朝里贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection='Z';	//始->终与'X'轴近似
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->m_bNeedUpdateNormal=true;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//根据螺栓位置计算
	//pParaAngle->normal.ciNormalDirection='Z';
	//pParaAngle->normal.uidRefRodOfWorkWing=2;
	pParaAngle->desStart.szXorCtrlWingOffset="-P1#T";	//表示负角钢2的肢厚
	pParaAngle->desStart.szYorOthrWingOffset[0]='0';
	pParaAngle->desEnd.szXorCtrlWingOffset="-P1#T";	//表示负角钢2的肢厚
	pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
	//3.三维模型描述
	//连接板描述
	MATCH_CONNECT_POINT* pConnPoint=this->listStdConnPoints.Append();
	pConnPoint->idNode = N10_ID;
	pConnPoint->idBaseRod= ANG_ARMPOST;
	pConnPoint->idRayRodOnAxisY = 1;
	pConnPoint->cbMirCreateFlag=MIR_CREATE_X;
	pConnPoint->idStdConnPoint=STDTEMPL_CONNPOINT_XH_DM1;
	pConnPoint->idStdNodePlate=STDTEMPL_NODEPLATE_XH_DM1;
}
//翻译模板设计参数
bool CStdComTemplWireStructure::TransTemlInstance_SECT_BTM_1_EB()
{
	//3、4号为底面前后侧挂线两根外贴挂线短角钢
	const int ANG_FL=3;
	const int ANG_BL=4;
	CStdTemplParaAngle* pParaAngleF = (CStdTemplParaAngle*)this->listParaRods.GetValue(ANG_FL);
	CStdTemplParaAngle* pParaAngleB = (CStdTemplParaAngle*)this->listParaRods.GetValue(ANG_BL);
	if (pParaAngleF == NULL || pParaAngleB == NULL)
		return false;
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	//初始化角钢规格
	if (pParam = listParams.GetValue(KEY4C("WAS")))
	{
		pParaAngleF->m_sGuige.Copy(pParam->value.szStrBuf);
		pParaAngleB->m_sGuige.Copy(pParam->value.szStrBuf);
	}
	//根据金具宽度设置短角钢肢向偏移量
	if (pParam = listParams.GetValue(2))
	{
		for (int i = 0; i < 2; i++)
		{
			CStdTemplParaAngle* pParaAngle = (i == 0) ? pParaAngleF : pParaAngleB;
			if (pParaAngle->ciAngleWorkWing == 'X')
			{
				pParaAngle->desStart.szYorOthrWingOffset.Printf("%g", -0.5*pParam->value.fValue);
				pParaAngle->desEnd.szYorOthrWingOffset = pParaAngle->desStart.szYorOthrWingOffset;
			}
			else
			{
				pParaAngle->desStart.szXorCtrlWingOffset.Printf("%g", -0.5*pParam->value.fValue);
				pParaAngle->desEnd.szXorCtrlWingOffset = pParaAngle->desStart.szXorCtrlWingOffset;
			}
		}
	}
	//根据固接螺栓数初始化螺栓定位参数
	if (pParam = this->listParams.GetValue(KEY4C("FBN")))
	{
		CStdTemplParaBolt* pParaBolt = NULL;
		for (int i = 0; i < 2; i++)
		{	
			CStdTemplParaAngle* pParaAngle = (i == 0) ? pParaAngleF : pParaAngleB;
			if (pParam->value.liValue == 2)
			{	//固接螺栓数为2,则与T型角钢两子角钢各有1个
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = pParaAngle->Id;
				pParaBolt->org.idCrossRod = 1;
				pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = pParaAngle->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 1);
				memcpy(pParaBolt->org.szCrossWingG, "g", 1);
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = pParaAngle->Id;
				pParaBolt->org.idCrossRod = 1;
				pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = pParaAngle->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 1);
				memcpy(pParaBolt->org.szCrossWingG, "g", 1);
			}
			else if (pParam->value.liValue == 4)
			{	//固接螺栓数为4,则与T型角钢两子角钢各有两个
				//g1心线交点位置螺栓
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = pParaAngle->Id;
				pParaBolt->org.idCrossRod = 1;
				pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = pParaAngle->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG,"g1",2);
				memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = pParaAngle->Id;
				pParaBolt->org.idCrossRod = 1;
				pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = pParaAngle->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g1", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
				//g2心线交点位置螺栓
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = pParaAngle->Id;
				pParaBolt->org.idCrossRod = 1;
				pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = pParaAngle->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = pParaAngle->Id;
				pParaBolt->org.idCrossRod = 1;
				pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = pParaAngle->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
			}
		}
	}
	//根据挂孔参数初始化挂孔定位参数
	if (xarrWireGroups[0].cnWirePointCount == 1)
	{
		char ciHoleWing = (pParaAngleF->ciAngleWorkWing == 'X') ? 'Y' : 'X';
		//添加孔
		CStdTemplParaBolt* pParaBolt = listParaBolts.Add(0);
		pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONDUCT_WIRE;
		pParaBolt->m_iWireGroup = 0;
		pParaBolt->dfHoleDia = xarrWireGroups[0].cnHoleD;
		pParaBolt->through[0].cItemType = 1;
		pParaBolt->through[0].val = pParaAngleF->Id;
		pParaBolt->through[1].cItemType = 1;
		pParaBolt->through[1].val = pParaAngleB->Id;
		pParaBolt->org.ciDatumPointCalStyle = 2;
		pParaBolt->org.idBasePart = pParaAngleF->Id;
		pParaBolt->org.idBaseNode = N20_ID;
		pParaBolt->org.ciDatumWing = ciHoleWing;
	}
	return  true;
}
//////////////////////////////////////////////////////////////////////////
//IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB
void CStdComTemplWireStructure::InitComTempl_META_SECT_BTM_2_EB()
{
	ClearTmplParams();
	name.Copy("直线塔横隔材双EB挂点");
	szSketchImgFile.Copy("CZC-S-EB.bmp");
	this->m_bTmplInitialized=true;
	//设定挂线架的识别关键节点或杆件
	this->xarrKeyRodNodes.ReSize(1);
	xarrKeyRodNodes[0].ciNode0Rod1=MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod=1;	//通过挂线端节点识别
	//设定挂孔金具信息
	FITTING_EB *pEBInfo = QueryFitting<FITTING_EB>("EB-16/21-100");
	if (pEBInfo == NULL)
	{
		IFittingLibrary<FITTING_EB>* pEBLib = FittingLibrary<FITTING_EB>();
		pEBInfo = pEBLib->GetAt(0);
	}
	FITTING_UR* pURInfo = MatchFitting<FITTING_UR>(pEBInfo->dfHoleD);
	if (pURInfo == NULL)
	{
		IFittingLibrary<FITTING_UR>* pURLib = FittingLibrary<FITTING_UR>();
		pURInfo = pURLib->GetAt(0);
	}
	this->m_cnWireGroupCount=1;
	xarrWireGroups[0].cnWirePointCount = 2;
	xarrWireGroups[0].wiSpaceL = 400;
	xarrWireGroups[0].ciFittingType = CLDSFitting::TYPE_EB;	
	xarrWireGroups[0].cnShaftD = ftoi(pEBInfo->dfTrunnionD);
	xarrWireGroups[0].wnFittingWidth = ftoi(pEBInfo->dfBoardL);
	StrCopy(xarrWireGroups[0].szFittingSizeCode, pEBInfo->szSizeCode, 17);
	StrCopy(xarrWireGroups[0].szUringSizeCode, pURInfo->szSizeCode, 17);
	//1.标准组件设计参数定义区
	STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"WorkPlaneCS(1,1,'Y','Z')");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	//金具宽度
	pParam=listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("T型挂线角钢间隙, EB挂点金具宽度+间隙，mm");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=xarrWireGroups[0].wnReservedWidth;	//不能给固定值84，否则可能与金具冲突
	//开档距离(用于计算偏移节点位置，值来源于xarrWireGroups[0].wiSpaceL)
	pParam = listParams.Add(KEY4C("L"));
	pParam->sLabel.Copy("SpaceL");
	pParam->sNotes.Copy("同组挂孔间的开档距离");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//挂线短角钢的规格
	pParam=listParams.Add(KEY4C("WAS"));
	pParam->sLabel.Copy("WIREANGLE_SIZE");
	pParam->sNotes.Copy("T型挂线角钢规格，mm");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_STRING;
	StrCopy(pParam->value.szStrBuf,"125x10",96);
	//挂线短角钢的固接螺栓
	pParam=listParams.Add(KEY4C("FBN"));
	pParam->sLabel.Copy("FIX_CONN_BOLTS_N");
	pParam->sNotes.Copy("挂线短角钢的固接螺栓数");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;//VALUETYPE_LONG;
	pParam->value.liValue=this->m_cnFixWireAngleConnBoltsN=2;
	//2.模型节点杆件描述
	CStdTemplParaNode* pParaNode=listParaNodes.Add(N10_ID);
	pParaNode->m_bNecessaryNode = true;
	//基准角钢中点
	pParaNode=listParaNodes.Add(N20_ID);
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::SCALE;
	pParaNode->attach_scale=0.5;
	pParaNode->attachNodeArr[0]= N10_ID;
	pParaNode->attachNodeArr[1]=TMPLOBJID(N10_ID,'X');
	pParaNode->keyLabelFatherRod=1;
	//前侧EB挂点位置
	pParaNode=listParaNodes.Add(N30_ID);
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType = CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0] = N20_ID;
	pParaNode->attachNodeArr[1] = N10_ID;
	pParaNode->attachNodeArr[2] = N20_ID;
	strcpy(pParaNode->offset_expression, "L/2");
	pParaNode->keyLabelFatherRod=1;
	//后侧EB挂点位置
	pParaNode = listParaNodes.Add(N40_ID);
	pParaNode->m_bNecessaryNode = false;
	pParaNode->ciPosCalType = CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0] = N20_ID;
	pParaNode->attachNodeArr[1] = N20_ID;
	pParaNode->attachNodeArr[2] = N10_ID;
	strcpy(pParaNode->offset_expression, "L/2");
	pParaNode->keyLabelFatherRod = 1;
	//杆件参数
	const int ANG_ARMPOST = 2;
	//1号为挂线角钢的连接基准T型组合角钢
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode=N10_ID;//1;
	pParaAngle->idEndNode=TMPLOBJID(N10_ID,'X');
	pParaAngle->ciSectStyle='T';
	pParaAngle->ciMasterIndex=1;
	pParaAngle->ciAngleLayout=3;	//ciAngleWorkWing='x'，X肢里铁
	pParaAngle->normal.ciNormalDirection='Z';
	pParaAngle->normal.uidRefRodOfWorkWing=ANG_ARMPOST;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->m_bNeedUpdatePosition=false;
	pParaAngle->desStart.uidTouchLineRod = ANG_ARMPOST;
	pParaAngle->desStart.szXorCtrlWingOffset="-$2#T";	//表示负角钢2的肢厚
	pParaAngle->desStart.szYorOthrWingOffset[0]='0';
	pParaAngle->desStart.uidTouchLineRod = TMPLOBJID(ANG_ARMPOST, 'X');
	pParaAngle->desEnd.szXorCtrlWingOffset="-$2#T";	//表示负角钢2的肢厚
	pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
	//横担弦杆主材，作为输入条件用于纠正1号T型组合角钢的肢朝向 wjh-2019.9.10
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_ARMPOST, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->m_bNecessaryRod = false;
	//3、4号为前侧挂点的两根外贴短角钢
	const int ANG_FL1=3;
	const int ANG_FL2=4;
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(ANG_FL1,CLS_LINEANGLE);
	pParaAngle->idStartNode= N30_ID;
	pParaAngle->ciSectStyle='L';
	pParaAngle->attachangle.idBaseAngle=1;		//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode=N30_ID;
	pParaAngle->attachangle.ciDatumAngleWing='X';
	pParaAngle->attachangle.ciNormalOffset=0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout=2;	//垂向朝外贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection='Z';	//始->终与'X'轴近似
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->m_bNeedUpdateNormal=false;
	pParaAngle->blShortAngle=true;
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//根据螺栓位置计算
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->desStart.szYorOthrWingOffset="-P1#T";	//表示负角钢2的肢厚
	pParaAngle->desStart.szXorCtrlWingOffset[0]='0';
	pParaAngle->desEnd.szYorOthrWingOffset="-P1#T";	//表示负角钢2的肢厚
	pParaAngle->desEnd.szXorCtrlWingOffset[0]='0';
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(ANG_FL2,CLS_LINEANGLE);
	pParaAngle->idStartNode= N30_ID;
	pParaAngle->ciSectStyle='L';
	pParaAngle->attachangle.idBaseAngle=1;		//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode=N30_ID;
	pParaAngle->attachangle.ciDatumAngleWing='X';
	pParaAngle->attachangle.ciNormalOffset=0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout=3;	//垂向朝里贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection='Z';	//始->终与'X'轴近似
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->m_bNeedUpdateNormal=false;
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//根据螺栓位置计算
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->desStart.szXorCtrlWingOffset="-P1#T";	//表示负角钢2的肢厚
	pParaAngle->desStart.szYorOthrWingOffset[0]='0';
	pParaAngle->desEnd.szXorCtrlWingOffset="-P1#T";	//表示负角钢2的肢厚
	pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
	//5、6号为后侧挂点的两根外贴短角钢
	const int ANG_BL1=5;
	const int ANG_BL2=6;
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(ANG_BL1,CLS_LINEANGLE);
	pParaAngle->idStartNode= N40_ID;
	pParaAngle->ciSectStyle='L';
	pParaAngle->attachangle.idBaseAngle=1;		//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode=N40_ID;
	pParaAngle->attachangle.ciDatumAngleWing='X';
	pParaAngle->attachangle.ciNormalOffset=0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout=2;	//垂向朝外贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection='Z';	//始->终与'X'轴近似
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->m_bNeedUpdateNormal=false;
	pParaAngle->blShortAngle=true;
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//根据螺栓位置计算
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->desStart.szYorOthrWingOffset="-P1#T";	//表示负角钢2的肢厚
	pParaAngle->desStart.szXorCtrlWingOffset[0]='0';
	pParaAngle->desEnd.szYorOthrWingOffset="-P1#T";	//表示负角钢2的肢厚
	pParaAngle->desEnd.szXorCtrlWingOffset[0]='0';
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(ANG_BL2,CLS_LINEANGLE);
	pParaAngle->idStartNode= N40_ID;
	pParaAngle->ciSectStyle='L';
	pParaAngle->attachangle.idBaseAngle=1;		//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode=N40_ID;
	pParaAngle->attachangle.ciDatumAngleWing='X';
	pParaAngle->attachangle.ciNormalOffset=0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout=3;	//垂向朝里贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection='Z';	//始->终与'X'轴近似
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->m_bNeedUpdateNormal=false;
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//根据螺栓位置计算
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->desStart.szXorCtrlWingOffset="-P1#T";	//表示负角钢2的肢厚
	pParaAngle->desStart.szYorOthrWingOffset[0]='0';
	pParaAngle->desEnd.szXorCtrlWingOffset="-P1#T";	//表示负角钢2的肢厚
	pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
	//3.三维模型描述
	//连接板描述
	MATCH_CONNECT_POINT* pConnPoint = this->listStdConnPoints.Append();
	pConnPoint->idNode = N10_ID;
	pConnPoint->idBaseRod = ANG_ARMPOST;
	pConnPoint->idRayRodOnAxisY = 1;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DM1;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DM1;
}
//
bool CStdComTemplWireStructure::TransTemlInstance_SECT_BTM_2_EB()
{
	PRESET_ARRAY4<CStdTemplParaAngle*> xParaAngleArr;
	xParaAngleArr[0] = (CStdTemplParaAngle*)this->listParaRods.GetValue(3);
	xParaAngleArr[1] = (CStdTemplParaAngle*)this->listParaRods.GetValue(4);
	xParaAngleArr[2] = (CStdTemplParaAngle*)this->listParaRods.GetValue(5);
	xParaAngleArr[3] = (CStdTemplParaAngle*)this->listParaRods.GetValue(6);
	if (xParaAngleArr[0] == NULL || xParaAngleArr[1] == NULL|| 
		xParaAngleArr[2] == NULL || xParaAngleArr[3] == NULL)
		return false;
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	//初始化角钢规格
	if (pParam = listParams.GetValue(KEY4C("WAS")))
	{
		for(int i=0;i<4;i++)
			xParaAngleArr[i]->m_sGuige.Copy(pParam->value.szStrBuf);
	}
	//根据金具宽度设置短角钢肢向偏移量
	if (pParam = listParams.GetValue(2))
	{
		for (int i = 0; i < 4; i++)
		{
			if (xParaAngleArr[i]->ciAngleWorkWing == 'X')
			{
				xParaAngleArr[i]->desStart.szYorOthrWingOffset.Printf("%g", -0.5*pParam->value.fValue);
				xParaAngleArr[i]->desEnd.szYorOthrWingOffset = xParaAngleArr[i]->desStart.szYorOthrWingOffset;
			}
			else
			{
				xParaAngleArr[i]->desStart.szXorCtrlWingOffset.Printf("%g", -0.5*pParam->value.fValue);
				xParaAngleArr[i]->desEnd.szXorCtrlWingOffset = xParaAngleArr[i]->desStart.szXorCtrlWingOffset;
			}
		}
	}
	//将开档距离值记录的listParams中
	if (pParam = listParams.GetValue(KEY4C("L")))
		pParam->value.fValue = (double)xarrWireGroups[0].wiSpaceL;
	//根据固接螺栓数初始化螺栓定位参数
	if (pParam = this->listParams.GetValue(KEY4C("FBN")))
	{
		CStdTemplParaBolt* pParaBolt = NULL;
		for (int i = 0; i < 4; i++)
		{
			if (pParam->value.liValue == 2)
			{	//固接螺栓数为2,则与T型角钢两子角钢各有1个
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = 1;
				pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 1);
				memcpy(pParaBolt->org.szCrossWingG, "g", 1);
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = 1;
				pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 1);
				memcpy(pParaBolt->org.szCrossWingG, "g", 1);
			}
			else if (pParam->value.liValue == 4)
			{	//固接螺栓数为4,则与T型角钢两子角钢各有两个
				//g1心线交点位置螺栓
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = 1;
				pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g1", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = 1;
				pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g1", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
				//g2心线交点位置螺栓
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = 1;
				pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = 1;
				pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
			}
		}
	}
	//根据挂孔参数初始化挂孔定位参数
	//if (xarrWireGroups[0].cnWirePointCount == 2)
	{
		//添加前侧挂孔
		char ciHoleWing = (xParaAngleArr[0]->ciAngleWorkWing == 'X') ? 'Y' : 'X';
		CStdTemplParaBolt* pParaBolt = listParaBolts.Add(0);
		pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONDUCT_WIRE;
		pParaBolt->m_iWireGroup = 0;
		pParaBolt->dfHoleDia = xarrWireGroups[0].cnHoleD;
		pParaBolt->through[0].cItemType = 1;
		pParaBolt->through[0].val = xParaAngleArr[0]->Id;
		pParaBolt->through[1].cItemType = 1;
		pParaBolt->through[1].val = xParaAngleArr[1]->Id;
		pParaBolt->org.ciDatumPointCalStyle = 2;
		pParaBolt->org.idBasePart = xParaAngleArr[0]->Id;
		pParaBolt->org.idBaseNode = N30_ID;
		pParaBolt->org.ciDatumWing = ciHoleWing;
		//添加后侧挂孔
		ciHoleWing = (xParaAngleArr[2]->ciAngleWorkWing == 'X') ? 'Y' : 'X';
		pParaBolt = listParaBolts.Add(0);
		pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONDUCT_WIRE;
		pParaBolt->m_iWireGroup = 0;
		pParaBolt->dfHoleDia = xarrWireGroups[0].cnHoleD;
		pParaBolt->through[0].cItemType = 1;
		pParaBolt->through[0].val = xParaAngleArr[2]->Id;
		pParaBolt->through[1].cItemType = 1;
		pParaBolt->through[1].val = xParaAngleArr[3]->Id;
		pParaBolt->org.ciDatumPointCalStyle = 2;
		pParaBolt->org.idBasePart = xParaAngleArr[2]->Id;
		pParaBolt->org.idBaseNode = N40_ID;
		pParaBolt->org.ciDatumWing = ciHoleWing;
	}
	return  true;
}