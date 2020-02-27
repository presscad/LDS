#include "stdafx.h"
#include "..\LDS.h"
#include "NodeStdComTempl.h"
#include "WireStruComTempl.h"
#include "Fitting.h"

//初始化模板参数
//IStdComTempl::STDTMPL_STRU_META_SECT_FRT_1_GD
void CStdComTemplWireStructure::InitComTempl_META_SECT_FRT_1_GD()
{
	ClearTmplParams();
	name.Copy("耐张塔塔横隔材正面单GD挂点");
	szSketchImgFile.Copy("JC-META-GD1.bmp");
	m_bTmplInitialized = true;
	//m_bEnableMirCreate = true;
	//设定挂线架的识别关键节点或杆件
	xarrKeyRodNodes.ReSize(1);
	xarrKeyRodNodes[0].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod = 1;	//通过挂线端节点识别
	//设定挂孔金具信息
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
	//挂线角钢间隙
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	pParam = listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("挂线角钢间隙, GD挂点金具宽度+间隙，mm");
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
	pParam->value.liValue = this->m_cnFixWireAngleConnBoltsN = 2;
	//2.模型节点杆件描述
	CStdTemplParaNode* pParaNode = NULL;
	pParaNode = listParaNodes.Add(N10_ID);
	pParaNode->m_bNecessaryNode = true;
	//1号为横担主材
	CStdTemplParaAngle* pParaAngle=NULL;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->vLocalNearVecS2E.Set(-1, 0, 0);
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->normal.ciNormalDirection = 'Y';
	//2、3号为前后正面横担主材上的两根外贴短角钢
	const int ANG_FRT_L = 2;
	const int ANG_FRT_R = 3;
	const int ANG_BTM = 4;	//底部外贴短角钢
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_FRT_L, CLS_LINEANGLE);
	pParaAngle->cbMirCreateFlag = 0x01;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->blShortAngle = true;
	pParaAngle->attachangle.idBaseAngle = 1;		//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode = N10_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'X';
	pParaAngle->attachangle.ciNormalOffset = 0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout = 2;	//垂向朝外贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection = 'Z';	//始->终与'X'轴近似
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = 0;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//根据螺栓位置计算
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->desStart.szYorOthrWingOffset = "-P1#T";
	pParaAngle->desStart.szXorCtrlWingOffset[0] = '0';
	pParaAngle->desEnd.szYorOthrWingOffset = "-P1#T";
	pParaAngle->desEnd.szXorCtrlWingOffset[0] = '0';
	//
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_FRT_R, CLS_LINEANGLE);
	pParaAngle->cbMirCreateFlag = 0x01;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->blShortAngle = true;
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->attachangle.idBaseAngle = 1;		//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode = N10_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'X';
	pParaAngle->attachangle.ciNormalOffset = 0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout = 3;	//垂向朝里贴(1号基准角钢始->终为视向)
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
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_BTM, CLS_LINEANGLE);
	pParaAngle->cbMirCreateFlag = 0x01;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->blShortAngle = true;
	pParaAngle->attachangle.idBaseAngle = 1;		//前侧横担主材角钢
	pParaAngle->attachangle.idBaseNode = N10_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'Y';
	pParaAngle->attachangle.ciAssistWing = 'X';
	pParaAngle->attachangle.ciNormalOffset = 0;	//表示自动判断，实际需要向外偏移一块连接板厚
	pParaAngle->attachangle.ciAngleLayout = 0;	//垂向朝里贴(1号基准角钢始->终为视向)
	pParaAngle->attachangle.ciLenDirection = 0;	//始->终与'X'轴近似
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//根据螺栓位置计算
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->desStart.szXorCtrlWingOffset = "-P2#T";
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.szXorCtrlWingOffset = "-P2#T";
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	//3.三维模型描述
	//连接板连接杆件
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(5, CLS_LINEANGLE);
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->normal.ciNormalDirection = 'Y';
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(6, CLS_LINEANGLE);
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->normal.ciNormalDirection = 'Y';
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(7, CLS_LINEANGLE);
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->normal.ciNormalDirection = 'Y';
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//连接板描述
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Add(1);
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//表示普通连接板
	pParaPlate->cbMirCreateFlag = 0x01;
	pParaPlate->design.idBaseNode = N10_ID;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//定制外形
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 2, true);
	pParaPlate->connrods[0].InitTipInfo(2, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = N10_ID;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 2;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 2;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	pParaPlate->connrods[1].InitBasicInfo(ANG_BTM, true, 1, 2, true);
	pParaPlate->connrods[1].InitTipInfo(2, 2);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = N10_ID;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 2;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 2;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	pParaPlate->connrods[2].InitBasicInfo(5, true, 1, 2, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = N10_ID;
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 2;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	pParaPlate->connrods[3].InitBasicInfo(6, false, 1, 2, false);
	pParaPlate->connrods[3].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt = N10_ID;
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 2;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	pParaPlate->connrods[4].InitBasicInfo(7, false, 1, 2, true);
	pParaPlate->connrods[4].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt = N10_ID;
	pParaPlate->connrods[4].layoutbolts[0].cnCount = 2;
	pParaPlate->connrods[4].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}

//翻译模板设计参数
extern CTower Ta;
bool CStdComTemplWireStructure::TransTemlInstance_SECT_FRT_1_GD()
{
	PRESET_ARRAY4<CStdTemplParaAngle*> xParaAngleArr;
	xParaAngleArr[0] = (CStdTemplParaAngle*)this->listParaRods.GetValue(2);
	xParaAngleArr[1] = (CStdTemplParaAngle*)this->listParaRods.GetValue(3);
	xParaAngleArr[2] = (CStdTemplParaAngle*)this->listParaRods.GetValue(4);
	if (xParaAngleArr[0] == NULL || xParaAngleArr[1] == NULL|| xParaAngleArr[2]==NULL)
		return false;
	//1号角钢为组合角钢时，不再添加挂线底座角钢
	BOOL bBaseGroupJg = FALSE;
	long *phRod = hashMatchRodHid.GetValue(1);
	CLDSLineAngle* pBaseJg = (CLDSLineAngle*)Ta.FromPartHandle(*phRod, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
	if (pBaseJg && pBaseJg->GetClassTypeId() == CLS_GROUPLINEANGLE)
	{
		xParaAngleArr[2]->m_bNecessaryRod = false;
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
				if (pConnRod->idRod == 4)
					pConnRod->idRod = CMPSTID(1, 2 - ciMasterSonJg);
			}
		}
	}
	//初始化角钢规格
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	if (pParam = listParams.GetValue(KEY4C("WAS")))
	{
		for (int i = 0; i < 2; i++)
			xParaAngleArr[i]->m_sGuige.Copy(pParam->value.szStrBuf);
	}
	if (pParam = listParams.GetValue(KEY4C("BAS")))
		xParaAngleArr[2]->m_sGuige.Copy(pParam->value.szStrBuf);
	//根据金具宽度设置短角钢肢向偏移量
	if (pParam=listParams.GetValue(2))
	{
		for (int i = 0; i < 2; i++)
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
	if (pParam= listParams.GetValue(KEY4C("FBN")))
	{
		CStdTemplParaBolt* pParaBolt = NULL;
		for (int i = 0; i < 2; i++)
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
				pParaBolt->through[0].val = 1;
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = bBaseGroupJg ? 1 : 4;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 1);
				memcpy(pParaBolt->org.szCrossWingG, "g", 1);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//穿过钢板
				pParaBolt->through[0].val = 1;
			}
			else if (pParam->value.liValue == 4)
			{	//固接螺栓数为4,则与T型角钢两子角钢各有2个
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
				pParaBolt->through[0].val = 1;
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = bBaseGroupJg ? 1 : 4;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g1", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//穿过钢板
				pParaBolt->through[0].val = 1;
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
				pParaBolt->through[0].val = 1;
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = bBaseGroupJg ? 1 : 4;
				if (bBaseGroupJg)
					pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//穿过钢板
				pParaBolt->through[0].val = 1;
			}
		}
	}
	//初始化挂孔定位参数
	char cWireJgWorkWing = xParaAngleArr[0]->ciAngleWorkWing;
	for (BYTE ciGroup = 0; ciGroup < m_cnWireGroupCount; ciGroup++)
	{
		short snCount = xarrWireGroups[ciGroup].cnWirePointCount;
		short siHalfSpaceL = xarrWireGroups[ciGroup].wiSpaceL/2;
		short siCurrLenOffsetToStart = 0, siCurrLenOffsetToEnd = 0;
		if (xarrWireGroups[ciGroup].cnWirePointCount % 2 == 1)
		{	//中间挂孔
			CStdTemplParaBolt* pParaBolt = listParaBolts.Add(0);
			pParaBolt->cbMirCreateFlag = 0X01;
			pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_EARTH_WIRE;
			pParaBolt->m_iWireGroup = ciGroup;
			pParaBolt->dfHoleDia = xarrWireGroups[ciGroup].cnHoleD;
			pParaBolt->through[0].cItemType = 1;
			pParaBolt->through[0].val = xParaAngleArr[0]->Id;
			pParaBolt->through[1].cItemType = 1;
			pParaBolt->through[1].val = xParaAngleArr[1]->Id;
			pParaBolt->org.ciDatumPointCalStyle = 2;
			pParaBolt->org.idBasePart = xParaAngleArr[0]->Id;
			pParaBolt->org.idBaseNode = N10_ID;
			pParaBolt->org.ciDatumWing = (cWireJgWorkWing == 'X') ? 1 : 0;
			pParaBolt->org.siLenOffset = 0;
			pParaBolt->normal.ciNormalCalStyle = 1;
			pParaBolt->normal.widBasePart = xParaAngleArr[0]->Id;
			//
			snCount -= 1;
		}
		for (int jI = 1; jI <= snCount; jI++)
		{	//前后对称挂孔
			CStdTemplParaBolt* pParaBolt = listParaBolts.Add(0);
			pParaBolt->cbMirCreateFlag = 0X01;
			pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_EARTH_WIRE;
			pParaBolt->m_iWireGroup = ciGroup;
			pParaBolt->dfHoleDia = xarrWireGroups[ciGroup].cnHoleD;
			pParaBolt->through[0].cItemType = 1;
			pParaBolt->through[0].val = xParaAngleArr[0]->Id;
			pParaBolt->through[1].cItemType = 1;
			pParaBolt->through[1].val = xParaAngleArr[1]->Id;
			pParaBolt->org.ciDatumPointCalStyle = 2;
			pParaBolt->org.idBasePart = xParaAngleArr[0]->Id;
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
			pParaBolt->normal.widBasePart = xParaAngleArr[0]->Id;
		}
	}
	return true;
}