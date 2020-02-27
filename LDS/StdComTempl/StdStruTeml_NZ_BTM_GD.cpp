#include "stdafx.h"
#include "..\LDS.h"
#include "NodeStdComTempl.h"
#include "WireStruComTempl.h"
#include "Fitting.h"

//初始化模板参数
//IStdComTempl::STDTMPL_STRU_NZ_BTM_2_GD
void CStdComTemplWireStructure::InitComTempl_NZ_BTM_2_GD()
{
	ClearTmplParams();
	name.Copy("耐张塔底面双GD挂点");
	szSketchImgFile.Copy("JC-2-GD2.bmp");
	m_bTmplInitialized = true;
	//设定挂线架的识别关键节点或杆件
	xarrKeyRodNodes.SetSize(2);
	xarrKeyRodNodes[0].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod = 1;	//通过挂线端节点识别
	xarrKeyRodNodes[1].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[1].idNodeOrRod = 2;	//通过挂线端节点识别
	//0.设定挂孔金具信息
	FITTING_GD *pGDInfo = QueryFitting<FITTING_GD>("GD-30");
	if (pGDInfo == NULL)
	{
		IFittingLibrary<FITTING_GD>* pGDLib = FittingLibrary<FITTING_GD>();
		pGDInfo = pGDLib->GetAt(0);
	}
	FITTING_UR* pURInfo = MatchFitting<FITTING_UR>(pGDInfo->dfHoleD);
	if (pURInfo == NULL)
	{
		IFittingLibrary<FITTING_UR>* pURLib = FittingLibrary<FITTING_UR>();
		pURInfo = pURLib->GetAt(0);
	}
	m_cnWireGroupCount = 1;
	xarrWireGroups[0].cnWirePointCount = 1;
	xarrWireGroups[0].wiSpaceL = 0;
	xarrWireGroups[0].ciFittingType = CLDSFitting::TYPE_GD;
	xarrWireGroups[0].cnShaftD = ftoi(pGDInfo->dfTrunnionD);
	xarrWireGroups[0].wnFittingWidth = ftoi(pGDInfo->dfBoardL);
	StrCopy(xarrWireGroups[0].szFittingSizeCode, pGDInfo->szSizeCode, 17);
	StrCopy(xarrWireGroups[0].szUringSizeCode, pURInfo->szSizeCode, 17);
	//1.标准组件设计参数定义区
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	pParam = listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("T型挂线角钢间隙, GD挂点金具宽度+间隙，mm");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue = xarrWireGroups[0].wnReservedWidth;	//不能给固定值84，否则可能与金具冲突;
	//挂线角钢规格
	pParam = listParams.Add(KEY4C("WAS"));
	pParam->sLabel.Copy("WireAngleSizeSpec");
	pParam->sNotes.Copy("挂线角钢规格");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_STRING;
	StrCopy(pParam->value.szStrBuf, "125x10", 96);
	//底座角钢规格
	pParam = listParams.Add(KEY4C("BAS"));
	pParam->sLabel.Copy("BaseAngleSizeSpec");
	pParam->sNotes.Copy("底座角钢规格，mm");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_STRING;
	StrCopy(pParam->value.szStrBuf, "125x10", 96);
	//固接螺栓数
	pParam = listParams.Add(KEY4C("FBN"));
	pParam->sLabel.Copy("FIX_CONN_BOLTS_N");
	pParam->sNotes.Copy("挂线短角钢的固接螺栓数");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_LONG;
	pParam->value.liValue = this->m_cnFixWireAngleConnBoltsN = 4;
	//2.模型节点杆件描述
	listParaNodes.Add(N10_ID);
	listParaNodes.Add(N20_ID);
	//
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->idEndNode = N20_ID;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->idEndNode = TMPLOBJID(N10_ID, 'X');
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = N20_ID;
	pParaAngle->idEndNode = TMPLOBJID(N20_ID, 'X');
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P2#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//N10节点处的射线角钢
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(4, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(5, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//N20节点处的射线角钢
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(6, CLS_LINEANGLE);
	pParaAngle->idStartNode = N20_ID;
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P2#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(7, CLS_LINEANGLE);
	pParaAngle->idStartNode = N20_ID;
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P2#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//正面横担主材上的四根外贴短角钢
	const int ANG_F_L1 = 11;
	const int ANG_F_L2 = 12;
	const int ANG_F_R1 = 13;
	const int ANG_F_R2 = 14;
	const int ANG_F_FRT = 15;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_F_L1, CLS_LINEANGLE);
	pParaAngle->idStartNode = N20_ID;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->cbMirCreateFlag = 0x01;
	pParaAngle->blShortAngle = true;
	pParaAngle->attachangle.idBaseAngle = 1;		//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode = N20_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'Y';
	pParaAngle->attachangle.ciNormalOffset = 0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout = 2;	//垂向朝外贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection = 'Z';	//始->终与'X'轴近似
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = 0;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//根据螺栓位置计算
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->desStart.szXorCtrlWingOffset = "-P2#T";
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.szXorCtrlWingOffset = "-P2#T";
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	//
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_F_L2, CLS_LINEANGLE);
	pParaAngle->idStartNode = N20_ID;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->cbMirCreateFlag = 0x01;
	pParaAngle->blShortAngle = true;
	pParaAngle->attachangle.idBaseAngle = 1;		//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode = N20_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'Y';
	pParaAngle->attachangle.ciNormalOffset = 0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout = 3;	//垂向朝里贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection = 'Z';	//始->终与'X'轴近似
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//根据螺栓位置计算
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->desStart.szYorOthrWingOffset = "-P2#T";
	pParaAngle->desStart.szXorCtrlWingOffset[0] = '0';
	pParaAngle->desEnd.szYorOthrWingOffset = "-P2#T";
	pParaAngle->desEnd.szXorCtrlWingOffset[0] = '0';
	//
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_F_R1, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->cbMirCreateFlag = 0x01;
	pParaAngle->attachangle.idBaseAngle = 1;	//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode = N10_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'Y';
	pParaAngle->attachangle.ciNormalOffset = 0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout = 2;	//垂向朝外贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection = 'Z';	//始->终与'X'轴近似
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = 0;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//根据螺栓位置计算
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->desStart.szXorCtrlWingOffset = "-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.szXorCtrlWingOffset = "-P1#T";
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	//
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_F_R2, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->cbMirCreateFlag = 0x01;
	pParaAngle->attachangle.idBaseAngle = 1;//TMPLOBJID(2,'X');		//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode = N10_ID;//TMPLOBJID(N20_ID,'X');
	pParaAngle->attachangle.ciDatumAngleWing = 'Y';
	pParaAngle->attachangle.ciNormalOffset = 0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout = 3;	//垂向朝里贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection = 'Y';	//始->终与'X'轴近似
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = 0;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//根据螺栓位置计算
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->desStart.szYorOthrWingOffset = "-P1#T";
	pParaAngle->desStart.szXorCtrlWingOffset[0] = '0';
	pParaAngle->desEnd.szYorOthrWingOffset = "-P1#T";
	pParaAngle->desEnd.szXorCtrlWingOffset[0] = '0';
	//
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_F_FRT, CLS_LINEANGLE);
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->cbMirCreateFlag = 0x01;
	pParaAngle->attachangle.idBaseAngle = 1;
	pParaAngle->attachangle.idBaseNode = N10_ID;
	pParaAngle->attachangle.idEndTipBaseNode = N20_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'X';
	pParaAngle->attachangle.ciAssistWing = 'Y';
	pParaAngle->attachangle.ciNormalOffset = 0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout = 0;	//0:沿向顺贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection = 0;	//始->终与'X'轴近似
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = 0;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//根据螺栓位置计算
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->desStart.szYorOthrWingOffset = "-P3#T";
	pParaAngle->desStart.szXorCtrlWingOffset[0] = '0';
	pParaAngle->desEnd.szYorOthrWingOffset = "-P3#T";
	pParaAngle->desEnd.szXorCtrlWingOffset[0] = '0';
	//3.三维模型描述
	//N10节点的连接板
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Add(1);
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//表示普通连接板
	pParaPlate->_thick = 10;
	pParaPlate->cbMirCreateFlag = 0X01;
	pParaPlate->design.idBaseNode = N10_ID;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//定制外形
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 2, true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = N10_ID;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 2;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 2;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	pParaPlate->connrods[1].InitBasicInfo(ANG_F_FRT, true, 1, 2, true);
	pParaPlate->connrods[1].InitTipInfo(0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = N10_ID;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 2;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 2;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	pParaPlate->connrods[2].InitBasicInfo(2, true, 1, 4, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = N10_ID;
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 4;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	pParaPlate->connrods[3].InitBasicInfo(4, false, 1, 3, false);
	pParaPlate->connrods[3].InitTipInfo(0, 0, 1, 2);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt = N10_ID;
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 3;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	pParaPlate->connrods[4].InitBasicInfo(5, false, 1, 3, false);
	pParaPlate->connrods[4].InitTipInfo(0, 0, 1, 2);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt = N10_ID;
	pParaPlate->connrods[4].layoutbolts[0].cnCount = 3;
	pParaPlate->connrods[4].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
	//N20节点的连接板
	pParaPlate = this->listParaPlates.Add(2);
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//表示普通连接板
	pParaPlate->_thick = 10;
	pParaPlate->cbMirCreateFlag = 0X01;
	pParaPlate->design.idBaseNode = N20_ID;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//定制外形
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 2, true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = N20_ID;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 2;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 2;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	pParaPlate->connrods[1].InitBasicInfo(ANG_F_FRT, true, 1, 2, true);
	pParaPlate->connrods[1].InitTipInfo(0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = N20_ID;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 2;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 2;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	pParaPlate->connrods[2].InitBasicInfo(3, true, 1, 4, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = N20_ID;
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 4;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	pParaPlate->connrods[3].InitBasicInfo(6, false, 1, 3, false);
	pParaPlate->connrods[3].InitTipInfo(0, 0, 1, 5);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt = N20_ID;
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 3;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	pParaPlate->connrods[4].InitBasicInfo(7, false, 1, 3, false);
	pParaPlate->connrods[4].InitTipInfo(0, 0, 1, 5);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt = N20_ID;
	pParaPlate->connrods[4].layoutbolts[0].cnCount = 3;
	pParaPlate->connrods[4].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}
//翻译模板设计参数
extern CTower Ta;
bool CStdComTemplWireStructure::TransTemlInstance_NZ_BTM_2_GD()
{
	const int ANG_F_L1 = 11;
	const int ANG_F_L2 = 12;
	const int ANG_F_R1 = 13;
	const int ANG_F_R2 = 14;
	const int ANG_F_FRT = 15;
	PRESET_ARRAY4<CStdTemplParaAngle*> xParaAngleArr;
	xParaAngleArr[0] = (CStdTemplParaAngle*)listParaRods.GetValue(ANG_F_L1);
	xParaAngleArr[1] = (CStdTemplParaAngle*)listParaRods.GetValue(ANG_F_L2);
	xParaAngleArr[2] = (CStdTemplParaAngle*)listParaRods.GetValue(ANG_F_R1);
	xParaAngleArr[3] = (CStdTemplParaAngle*)listParaRods.GetValue(ANG_F_R2);
	if (xParaAngleArr[0] == NULL || xParaAngleArr[1] == NULL ||
		xParaAngleArr[2] == NULL || xParaAngleArr[3] == NULL)
		return false;
	//1号角钢为组合角钢时，不再添加挂线底座角钢
	BOOL bBaseGroupJg = FALSE;
	long *phRod = hashMatchRodHid.GetValue(1);
	CLDSLineAngle* pBaseJg = (CLDSLineAngle*)Ta.FromPartHandle(*phRod, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
	if (pBaseJg && pBaseJg->GetClassTypeId() == CLS_GROUPLINEANGLE)
	{
		CStdTemplParaAngle* pParaJg = (CStdTemplParaAngle*)listParaRods.GetValue(ANG_F_FRT);
		pParaJg->m_bNecessaryRod = false;
		bBaseGroupJg = TRUE;
	}
	//基准杆件为组合角钢时，将钢板连接杆件细化到子角钢
	if (bBaseGroupJg)
	{
		CLDSGroupLineAngle* pGroupJg = (CLDSGroupLineAngle*)pBaseJg;
		BYTE ciMasterSonJg = pGroupJg->ciMasterIndex;
		for (CStdTemplParaPlate* pStdPlate = listParaPlates.GetFirst(); pStdPlate; pStdPlate = listParaPlates.GetNext())
		{
			for (UINT i = 0; i < pStdPlate->connrods.Count; i++)
			{
				CONNECT_ROD* pConnRod = pStdPlate->connrods.GetAt(i);
				if (pConnRod == NULL)
					continue;
				if (pConnRod->idRod == 1)
					pConnRod->idRod = CMPSTID(1, ciMasterSonJg + 1);
				if (pConnRod->idRod == ANG_F_FRT)
					pConnRod->idRod = CMPSTID(1, 2 - ciMasterSonJg);
			}
		}
	}
	//挂线角钢为组合角钢时,钢板的连接杆件需要细化到子角钢
	for (int i = 0; i < 2; i++)
	{
		WORD idGXJg = (i == 0) ? 2 : 3;
		phRod = hashMatchRodHid.GetValue(idGXJg);
		CLDSLinePart* pGXJg = Ta.FromRodHandle(*phRod);
		if (pGXJg && pGXJg->GetClassTypeId() == CLS_GROUPLINEANGLE)
		{
			long idPlate = (i == 0) ? 1 : 2;
			CStdTemplParaPlate* pStdPlate = listParaPlates.GetValue(idPlate);
			if(pStdPlate==NULL)
				continue;
			int nConnRod = pStdPlate->connrods.Count;
			CONNECT_ROD* pConnRod = NULL;
			for (int i = 0; i < nConnRod; i++)
			{
				pConnRod = pStdPlate->connrods.GetAt(i);
				if (pConnRod->idRod == idGXJg)
				{
					pConnRod->idRod = CMPSTID(idGXJg, 1);
					break;
				}
			}
			if (pConnRod)
			{
				char stackpool[4096] = { 0 };
				CBuffer buffer(stackpool, 4096);
				buffer.ClearContents();
				pConnRod->ToBuffer(&buffer);
				buffer.SeekToBegin();
				pStdPlate->connrods[nConnRod].FromBuffer(&buffer);
				pStdPlate->connrods[nConnRod].idRod = CMPSTID(idGXJg, 2);
				pStdPlate->connrods.SetSize(nConnRod + 1);
			}
		}
	}
	//初始化挂线角钢规格
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	if (pParam = listParams.GetValue(KEY4C("WAS")))
	{
		for (int i = 0; i < 4; i++)
			xParaAngleArr[i]->m_sGuige.Copy(pParam->value.szStrBuf);
	}
	if (pParam = listParams.GetValue(KEY4C("BAS")))
	{
		CStdTemplParaAngle* pParaJg = (CStdTemplParaAngle*)listParaRods.GetValue(ANG_F_FRT);
		if (pParaJg)
			pParaJg->m_sGuige.Copy(pParam->value.szStrBuf);
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
	//根据固接螺栓数初始化螺栓定位参数
	CStdTemplParaBolt* pParaBolt = NULL;
	if (pParam = listParams.GetValue(KEY4C("FBN")))
	{
		for (int i = 0; i < 4; i++)
		{
			if (pParam->value.liValue == 2)
			{	//固接螺栓数为2,则与T型角钢两子角钢各有1个
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = 1;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 1);
				memcpy(pParaBolt->org.szCrossWingG, "g", 1);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//穿过钢板
				pParaBolt->through[0].val = (i < 2) ? 2 : 1;
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = bBaseGroupJg ? 1 : ANG_F_FRT;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 1);
				memcpy(pParaBolt->org.szCrossWingG, "g", 1);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//穿过钢板
				pParaBolt->through[0].val = (i < 2) ? 2 : 1;
			}
			else if (pParam->value.liValue == 4)
			{
				//g1心线交点位置螺栓
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = 1;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g1", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//穿过钢板
				pParaBolt->through[0].val = (i < 2) ? 2 : 1;
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = bBaseGroupJg ? 1 : ANG_F_FRT;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g1", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//穿过钢板
				pParaBolt->through[0].val = (i < 2) ? 2 : 1;
				//g2心线交点位置螺栓
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = 1;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//穿过钢板
				pParaBolt->through[0].val = (i < 2) ? 2 : 1;
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = bBaseGroupJg ? 1 : ANG_F_FRT;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//穿过钢板
				pParaBolt->through[0].val = (i < 2) ? 2 : 1;
			}
		}
	}
	//初始化挂孔定位参数
	char cWireJgWorkWing = xParaAngleArr[0]->ciAngleWorkWing;
	for (BYTE ciGroup = 0; ciGroup < m_cnWireGroupCount; ciGroup++)
	{
		short snCount = xarrWireGroups[ciGroup].cnWirePointCount;
		short siHalfSpaceL = (short)(xarrWireGroups[ciGroup].wiSpaceL*0.5);
		short siCurrLenOffsetToStart = 0, siCurrLenOffsetToEnd = 0;
		if (xarrWireGroups[ciGroup].cnWirePointCount % 2 == 1)
		{	//中间挂孔
			for (int i = 0; i < 3; i += 2)
			{
				CStdTemplParaBolt* pParaBolt = listParaBolts.Add(0);
				pParaBolt->cbMirCreateFlag = 0X01;
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_EARTH_WIRE;
				pParaBolt->m_iWireGroup = ciGroup;
				pParaBolt->dfHoleDia = xarrWireGroups[ciGroup].cnHoleD;
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].val = xParaAngleArr[i]->Id;
				pParaBolt->through[1].cItemType = 1;
				pParaBolt->through[1].val = xParaAngleArr[i + 1]->Id;
				pParaBolt->org.ciDatumPointCalStyle = 2;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idBaseNode = N10_ID;
				pParaBolt->org.ciDatumWing = (cWireJgWorkWing == 'X') ? 1 : 0;
				pParaBolt->org.siLenOffset = 0;
				pParaBolt->normal.ciNormalCalStyle = 1;
				pParaBolt->normal.widBasePart = xParaAngleArr[i]->Id;
			}
			//
			snCount -= 1;
		}
		for (int jI = 1; jI <= snCount; jI++)
		{	//前后对称挂孔
			for (int i = 0; i < 3; i += 2)
			{
				CStdTemplParaBolt* pParaBolt = listParaBolts.Add(0);
				pParaBolt->cbMirCreateFlag = 0X01;
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_EARTH_WIRE;
				pParaBolt->m_iWireGroup = ciGroup;
				pParaBolt->dfHoleDia = xarrWireGroups[ciGroup].cnHoleD;
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].val = xParaAngleArr[i]->Id;
				pParaBolt->through[1].cItemType = 1;
				pParaBolt->through[1].val = xParaAngleArr[i + 1]->Id;
				pParaBolt->org.ciDatumPointCalStyle = 2;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idBaseNode = N10_ID;
				pParaBolt->org.ciDatumWing = (cWireJgWorkWing == 'X') ? 1 : 0;
				if (jI % 2 == 0)
				{	//添加前侧(始端侧)挂孔
					siCurrLenOffsetToStart += siHalfSpaceL;
					pParaBolt->org.siLenOffset = -siCurrLenOffsetToStart;
				}
				else
				{	//添加后侧(终端侧)挂孔
					siCurrLenOffsetToEnd += siHalfSpaceL;
					pParaBolt->org.siLenOffset = siCurrLenOffsetToEnd;
				}
				pParaBolt->normal.ciNormalCalStyle = 1;
				pParaBolt->normal.widBasePart = xParaAngleArr[i]->Id;
			}
		}
	}
	return true;
}
//IStdComTempl::STDTMPL_STRU_NZ_BTM_3_GD
void CStdComTemplWireStructure::InitComTempl_NZ_BTM_3_GD()
{
	ClearTmplParams();
	name.Copy("耐张塔底面叁GD挂点");
	szSketchImgFile.Copy("JC-3-GD2.bmp");
	m_bTmplInitialized = true;
	//设定挂线架的识别关键节点或杆件
	xarrKeyRodNodes.SetSize(3);
	xarrKeyRodNodes[0].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod = 1;	//通过挂线端节点识别
	xarrKeyRodNodes[1].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[1].idNodeOrRod = 2;	//通过挂线端节点识别
	xarrKeyRodNodes[2].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[2].idNodeOrRod = 3;	//通过挂线端节点识别
	//0.设定挂孔金具信息
	FITTING_GD *pGDInfo = QueryFitting<FITTING_GD>("GD-30");
	if (pGDInfo == NULL)
	{
		IFittingLibrary<FITTING_GD>* pGDLib = FittingLibrary<FITTING_GD>();
		pGDInfo = pGDLib->GetAt(0);
	}
	FITTING_UR* pURInfo = MatchFitting<FITTING_UR>(pGDInfo->dfHoleD);
	if (pURInfo == NULL)
	{
		IFittingLibrary<FITTING_UR>* pURLib = FittingLibrary<FITTING_UR>();
		pURInfo = pURLib->GetAt(0);
	}
	m_cnWireGroupCount = 1;
	xarrWireGroups[0].cnWirePointCount = 1;
	xarrWireGroups[0].wiSpaceL = 0;
	xarrWireGroups[0].ciFittingType = CLDSFitting::TYPE_GD;
	xarrWireGroups[0].cnShaftD = ftoi(pGDInfo->dfTrunnionD);
	xarrWireGroups[0].wnFittingWidth = ftoi(pGDInfo->dfBoardL);
	StrCopy(xarrWireGroups[0].szFittingSizeCode, pGDInfo->szSizeCode, 17);
	StrCopy(xarrWireGroups[0].szUringSizeCode, pURInfo->szSizeCode, 17);
	//1.标准组件设计参数定义区
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	pParam = listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("T型挂线角钢间隙, GD挂点金具宽度+间隙，mm");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue = xarrWireGroups[0].wnReservedWidth;	//不能给固定值84，否则可能与金具冲突;
	//挂线角钢规格
	pParam = listParams.Add(KEY4C("WAS"));
	pParam->sLabel.Copy("WireAngleSizeSpec");
	pParam->sNotes.Copy("挂线角钢规格");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_STRING;
	StrCopy(pParam->value.szStrBuf, "125x10", 96);
	//底座角钢规格
	pParam = listParams.Add(KEY4C("BAS"));
	pParam->sLabel.Copy("BaseAngleSizeSpec");
	pParam->sNotes.Copy("底座角钢规格，mm");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_STRING;
	StrCopy(pParam->value.szStrBuf, "125x10", 96);
	//固接螺栓数
	pParam = listParams.Add(KEY4C("FBN"));
	pParam->sLabel.Copy("FIX_CONN_BOLTS_N");
	pParam->sNotes.Copy("挂线短角钢的固接螺栓数");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_LONG;//VALUETYPE_LONG;
	pParam->value.liValue = this->m_cnFixWireAngleConnBoltsN = 4;
	//2.模型节点杆件描述
	CStdTemplParaNode* pParaNode = NULL;
	pParaNode = listParaNodes.Add(N10_ID);
	pParaNode = listParaNodes.Add(N20_ID);
	pParaNode = listParaNodes.Add(N30_ID);
	//pParaNode = listParaNodes.Add(N40_ID);
	//
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->m_bNecessaryRod = true;
	/*pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = N40_ID;
	pParaAngle->idEndNode = N10_ID;
	//边相端面T型挂线组合角钢
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->idEndNode = TMPLOBJID(N10_ID, 'X');
	pParaAngle->ciSectStyle = 'T';
	pParaAngle->ciMasterIndex = 1;
	pParaAngle->ciAngleLayout = 3;
	pParaAngle->normal.ciNormalDirection = 'Z';
	pParaAngle->normal.uidRefRodOfWorkWing = 1;
	pParaAngle->m_bNeedUpdatePosition = pParaAngle->m_bNeedUpdateNormal = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "0";	//表示负角钢2的肢厚
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.szXorCtrlWingOffset = "0";	//表示负角钢2的肢厚
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';*/
	//4、5、6、7、8、9号为前后正面横担主材上的四根外贴短角钢
	for (int i = 0; i < 6; i+=2)
	{
		int ANG_B_F = i + 4;
		int ANG_B_R = i + 5;
		int ANG_NODE = i/2 + 1;
		//节点处左侧挂线角钢
		pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_B_F, CLS_LINEANGLE);
		pParaAngle->idStartNode = ANG_NODE;
		pParaAngle->ciSectStyle = 'L';
		pParaAngle->blShortAngle = true;
		pParaAngle->attachangle.idBaseAngle = 1;
		pParaAngle->attachangle.idBaseNode = ANG_NODE;
		pParaAngle->attachangle.ciDatumAngleWing = 'Y';
		pParaAngle->attachangle.ciNormalOffset = 0;	//表示自动判断，实际需要向外偏移一块连接板厚
		pParaAngle->attachangle.ciAngleLayout = 2;	//垂向朝外贴(1号基准角钢始->终为视向)
		pParaAngle->attachangle.ciLenDirection = 'Z';	//始->终与'X'轴近似
		pParaAngle->m_cbNeedUpdatePosition = 0;
		pParaAngle->m_bNeedUpdateNormal = 0;
		pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;
		pParaAngle->ciAngleWorkWing = 'X';
		pParaAngle->desStart.szYorOthrWingOffset = "-P1#T";	//表示负角钢2的肢厚
		pParaAngle->desStart.szXorCtrlWingOffset[0] = '0';
		pParaAngle->desEnd.szYorOthrWingOffset = "-P1#T";	//表示负角钢2的肢厚
		pParaAngle->desEnd.szXorCtrlWingOffset[0] = '0';
		//节点处左侧挂线角钢
		pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_B_R, CLS_LINEANGLE);
		pParaAngle->idStartNode = ANG_NODE;
		pParaAngle->ciSectStyle = 'L';
		pParaAngle->blShortAngle = true;
		pParaAngle->attachangle.idBaseAngle = 1;
		pParaAngle->attachangle.idBaseNode = ANG_NODE;
		pParaAngle->attachangle.ciDatumAngleWing = 'Y';
		pParaAngle->attachangle.ciNormalOffset = 0;	//表示自动判断，实际需要向外偏移一块连接板厚
		pParaAngle->attachangle.ciAngleLayout = 3;	//垂向朝里贴(1号基准角钢始->终为视向)
		pParaAngle->attachangle.ciLenDirection = 'Z';	//始->终与'X'轴近似
		pParaAngle->m_cbNeedUpdatePosition = 0;
		pParaAngle->m_bNeedUpdateNormal = false;
		pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;
		pParaAngle->ciAngleWorkWing = 'Y';
		pParaAngle->desStart.szXorCtrlWingOffset = "-P1#T";	//表示负角钢2的肢厚
		pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
		pParaAngle->desEnd.szXorCtrlWingOffset = "-P1#T";	//表示负角钢2的肢厚
		pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	}
	//挂线基座短角钢
	int ANG_F_FRT = 10;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_F_FRT, CLS_LINEANGLE);
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->attachangle.idBaseAngle = 1;
	pParaAngle->attachangle.idBaseNode = N10_ID;
	pParaAngle->attachangle.idEndTipBaseNode = N30_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'X';
	pParaAngle->attachangle.ciAssistWing = 'Y';
	pParaAngle->attachangle.ciNormalOffset = 0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout = 0;	//0:沿向顺贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection = 0;	//始->终与'X'轴近似
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = 0;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//根据螺栓位置计算
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->desStart.szXorCtrlWingOffset = "-P3#T";	//表示负角钢2的肢厚
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.szXorCtrlWingOffset = "-P3#T";	//表示负角钢2的肢厚
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	//3.三维模型描述
}

bool CStdComTemplWireStructure::TransTemlInstance_NZ_BTM_3_GD()
{
	PRESET_ARRAY8<CStdTemplParaAngle*> xParaAngleArr;
	xParaAngleArr[0] = (CStdTemplParaAngle*)listParaRods.GetValue(4);
	xParaAngleArr[1] = (CStdTemplParaAngle*)listParaRods.GetValue(5);
	xParaAngleArr[2] = (CStdTemplParaAngle*)listParaRods.GetValue(6);
	xParaAngleArr[3] = (CStdTemplParaAngle*)listParaRods.GetValue(7);
	xParaAngleArr[4] = (CStdTemplParaAngle*)listParaRods.GetValue(8);
	xParaAngleArr[5] = (CStdTemplParaAngle*)listParaRods.GetValue(9);
	if (xParaAngleArr[0] == NULL || xParaAngleArr[1] == NULL ||
		xParaAngleArr[2] == NULL || xParaAngleArr[3] == NULL ||
		xParaAngleArr[4] == NULL || xParaAngleArr[5] == NULL)
		return false;
	//1号角钢为组合角钢时，不再添加挂线底座角钢
	BOOL bBaseGroupJg = FALSE;
	long *phRod = hashMatchRodHid.GetValue(1);
	CLDSLineAngle* pBaseJg = (CLDSLineAngle*)Ta.FromPartHandle(*phRod, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
	if (pBaseJg && pBaseJg->GetClassTypeId() == CLS_GROUPLINEANGLE)
	{
		CStdTemplParaAngle* pParaJg = (CStdTemplParaAngle*)listParaRods.GetValue(10);
		pParaJg->m_bNecessaryRod = false;
		bBaseGroupJg = TRUE;
	}
	//初始化挂线角钢规格
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	if (pParam = listParams.GetValue(KEY4C("WAS")))
	{
		for (int i = 0; i < 6; i++)
			xParaAngleArr[i]->m_sGuige.Copy(pParam->value.szStrBuf);
	}
	if (pParam = listParams.GetValue(KEY4C("BAS")))
	{
		CStdTemplParaAngle* pParaJg = (CStdTemplParaAngle*)listParaRods.GetValue(10);
		if (pParaJg)
			pParaJg->m_sGuige.Copy(pParam->value.szStrBuf);
	}
	//根据金具宽度设置短角钢肢向偏移量
	if (pParam = listParams.GetValue(2))
	{
		for (int i = 0; i < 6; i++)
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
	//根据固接螺栓数初始化螺栓定位参数
	CStdTemplParaBolt* pParaBolt = NULL;
	if (pParam = listParams.GetValue(KEY4C("FBN")))
	{
		for (int i = 0; i < 6; i++)
		{
			if (pParam->value.liValue == 2)
			{	//固接螺栓数为2,则与T型角钢两子角钢各有1个
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = 1;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 1);
				memcpy(pParaBolt->org.szCrossWingG, "g", 1);
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = bBaseGroupJg ? 1 : 10;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 1);
				memcpy(pParaBolt->org.szCrossWingG, "g", 1);
			}
			else if (pParam->value.liValue == 4)
			{
				//g1心线交点位置螺栓
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = 1;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g1", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = bBaseGroupJg ? 1 : 10;
				if (bBaseGroupJg)
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
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = bBaseGroupJg ? 1 : 10;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
			}
		}
	}
	//初始化挂孔定位参数
	char cWireJgWorkWing = xParaAngleArr[0]->ciAngleWorkWing;
	for (BYTE ciGroup = 0; ciGroup < m_cnWireGroupCount; ciGroup++)
	{
		short snCount = xarrWireGroups[ciGroup].cnWirePointCount;
		short siHalfSpaceL = (short)(xarrWireGroups[ciGroup].wiSpaceL*0.5);
		short siCurrLenOffsetToStart = 0, siCurrLenOffsetToEnd = 0;
		if (xarrWireGroups[ciGroup].cnWirePointCount % 2 == 1)
		{	//中间挂孔
			for (int i = 0; i < 6; i += 2)
			{
				CStdTemplParaBolt* pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_EARTH_WIRE;
				pParaBolt->m_iWireGroup = ciGroup;
				pParaBolt->dfHoleDia = xarrWireGroups[ciGroup].cnHoleD;
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].val = xParaAngleArr[i]->Id;
				pParaBolt->through[1].cItemType = 1;
				pParaBolt->through[1].val = xParaAngleArr[i + 1]->Id;
				pParaBolt->org.ciDatumPointCalStyle = 2;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idBaseNode = xParaAngleArr[i]->attachangle.idBaseNode;
				pParaBolt->org.ciDatumWing = (cWireJgWorkWing == 'X') ? 1 : 0;
				pParaBolt->org.siLenOffset = 0;
				pParaBolt->normal.ciNormalCalStyle = 1;
				pParaBolt->normal.widBasePart = xParaAngleArr[i]->Id;
			}
			//
			snCount -= 1;
		}
		for (int jI = 1; jI <= snCount; jI++)
		{	//前后对称挂孔
			for (int i = 0; i < 3; i += 2)
			{
				CStdTemplParaBolt* pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_EARTH_WIRE;
				pParaBolt->m_iWireGroup = ciGroup;
				pParaBolt->dfHoleDia = xarrWireGroups[ciGroup].cnHoleD;
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].val = xParaAngleArr[i]->Id;
				pParaBolt->through[1].cItemType = 1;
				pParaBolt->through[1].val = xParaAngleArr[i + 1]->Id;
				pParaBolt->org.ciDatumPointCalStyle = 2;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idBaseNode = xParaAngleArr[i]->attachangle.idBaseNode;
				pParaBolt->org.ciDatumWing = (cWireJgWorkWing == 'X') ? 1 : 0;
				if (jI % 2 == 0)
				{	//添加前侧(始端侧)挂孔
					siCurrLenOffsetToStart += siHalfSpaceL;
					pParaBolt->org.siLenOffset = -siCurrLenOffsetToStart;
				}
				else
				{	//添加后侧(终端侧)挂孔
					siCurrLenOffsetToEnd += siHalfSpaceL;
					pParaBolt->org.siLenOffset = siCurrLenOffsetToEnd;
				}
				pParaBolt->normal.ciNormalCalStyle = 1;
				pParaBolt->normal.widBasePart = xParaAngleArr[i]->Id;
			}
		}
	}
	return true;
}