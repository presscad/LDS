// DesignWireHole.cpp: implementation of the CDesignWireHole class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "LDS.h"
#include "GlobalFunc.h"
#include "tower.h"
#include "env_def.h"
#include "SortFunc.h"
#include "LogFile.h"
#include "DesignMultiLeg.h"
#include "UnifyModelLayer.h"
#include "MirTaAtom.h"
#include ".\StdComTempl\IStdComTempl.h"
#include "FootDesignTemplate.h"
#include "CommonLjSmartDesign.h"
#include "DesignJoint.h"
#include "GeneralNodeDesign.h"
#include "NewPartsListen.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CMappingBolts::CMappingBolts(CLDSLineAngle* pAngle/*=NULL*/, char ciWorkWingX0Y1/*=0*/, CLDSNode* pTipNode/*=NULL*/)
{
	if (pAngle == NULL)
		wcs.InitStdCS();
	else
		InitWorkPlaneCS(pAngle, ciWorkWingX0Y1, pTipNode);
}
void CMappingBolts::ClearMapping()
{
	hashMapBolts.Empty();
	listBoltStates.Empty();
}
void CMappingBolts::InitWorkPlaneCS(CLDSLineAngle* pAngle, char ciWorkWingX0Y1, CLDSNode* pTipNode/*=NULL*/)
{
	if (pTipNode == pAngle->pEnd)
	{
		wcs.origin = pAngle->xPosEnd;
		wcs.axis_x = pAngle->xPosStart - pAngle->xPosEnd;
	}
	else
	{
		wcs.origin = pAngle->xPosStart;
		wcs.axis_x = pAngle->xPosEnd - pAngle->xPosStart;
	}
	normalize(wcs.axis_x);
	wcs.axis_z = ciWorkWingX0Y1 == 0 ? pAngle->vxWingNorm : pAngle->vyWingNorm;
	wcs.axis_y = wcs.axis_z^wcs.axis_x;
}
bool CMappingBolts::InitExistBoltsInfo(CLDSLineAngle* pAngle, char ciWorkWingX0Y1, CLDSNode* pTipNode/* = NULL*/)
{
	InitWorkPlaneCS(pAngle, ciWorkWingX0Y1, pTipNode);	//初始化坐标系
	for (CLsRef* pLsRef = pAngle->GetFirstLsRef(); pLsRef; pLsRef = pAngle->GetNextLsRef())
	{
		//if (hashMapBolts.GetValue(pLsRef->pBolt->handle) != NULL)
		//	continue;	//已处理过的螺栓
		int wingX0Y1 = 0;
		GEPOINT xLocation;
		pAngle->GetBoltIntersPos(pLsRef->pBolt, xLocation, &wingX0Y1);
		if (wingX0Y1 != ciWorkWingX0Y1)
			continue;
		MATCHSTATE state;
		state.pos = wcs.TransPToCS(xLocation);
		state.pBolt = pLsRef->pBolt;
		listBoltStates.Append(state);
	}
	return true;
}
UINT CMappingBolts::Mapping(CLDSBoltPtr* ppBolts, UINT uiBoltCount, IXhList<BOLT_LOCATION> *pListLocations,
	double tolerance/*=10*/)
{
	UINT i, hits = 0;
	GEPOINT pos, pos2, dv;
	PRESET_ARRAY32<MATCHSTATE> states;
	if (uiBoltCount == 0)
		return 0;
	states.ReSize(uiBoltCount);
	states.ZeroPresetMemory();
	for (i = 0; i < uiBoltCount; i++)
	{
		if (ppBolts[i] == NULL)
		{
#ifdef __TMP_LOG_PROC_
			logerr.Log("ppBolts[%d]=NULL", i);
#endif
			continue;
		}
		if (hashMapBolts.GetValue(ppBolts[i]->handle) != NULL)
		{
			states[i].matched = true;
			continue;
		}
		else
		{
			states[i].pos = wcs.TransPToCS(ppBolts[i]->ucs.origin);
			states[i].pBolt = ppBolts[i];
		}
	}
	struct MATCHBOLT {
		UINT index;
		double dist;
		CLDSBolt* pMapBolt;
	}match;
	for (BOLT_LOCATION* pLocation = pListLocations->MoveFirst(); pLocation; pLocation = pListLocations->MoveNext())
	{
		pos2 = wcs.TransPToCS(pLocation->xTempPos);
		match.pMapBolt = NULL;
		match.dist = tolerance + 10;
		for (i = 0; i < uiBoltCount; i++)
		{
			if (states.At(i).matched)
				continue;
			dv = pos2 - states.At(i).pos;
			double dist = SQRT(dv.x*dv.x + dv.y*dv.y);
			if (dist <= match.dist)
			{
				match.pMapBolt = ppBolts[i];
				match.dist = dist;
				match.index = i;
			}
			if (match.dist < 10)
				break;
		}
		if (match.pMapBolt)
		{
			states[match.index].matched = true;
			hashMapBolts.SetValue(match.pMapBolt->handle, pLocation);
			pLocation->pRelaBolt = match.pMapBolt;
			hits++;
		}
	}
	return hits;
}
UINT CMappingBolts::Mapping(BOLT_LOCATION *pxarrLocations, int count/*=1*/, double tolerance/*=10*/)
{
	if (count == 0)
		return 0;
	MATCHSTATE* pState;
	for (pState = listBoltStates.EnumObjectFirst(); pState; pState = listBoltStates.EnumObjectNext())
		pState->matched = false;
	struct MATCHBOLT {
		double dist;
		CLDSBolt* pMapBolt;
		MATCHSTATE* pMatchedState;
	}match;
	int i, hits = 0;
	for (i = 0; i < count; i++)
	{
		BOLT_LOCATION* pLocation = &pxarrLocations[i];
		GEPOINT pos2 = wcs.TransPToCS(pLocation->xTempPos);
		match.pMapBolt = NULL;
		match.pMatchedState = NULL;
		match.dist = tolerance + 10;
		for (pState = listBoltStates.EnumObjectFirst(); pState; pState = listBoltStates.EnumObjectNext())
		{
			if (pState->matched)
				continue;
			GEPOINT dv = pos2 - pState->pos;
			double dist = SQRT(dv.x*dv.x + dv.y*dv.y);
			if (dist <= match.dist)
			{
				match.dist = dist;
				match.pMapBolt = pState->pBolt;
				match.pMatchedState = pState;
			}
			if (match.dist < 10)
				break;
		}
		if (match.pMapBolt)
		{
			if (match.pMatchedState)
				match.pMatchedState->matched = true;
			pLocation->pRelaBolt = match.pMapBolt;
			hits++;
		}
	}
	return hits;
}
#ifdef __RAPID_ROUGH_3DMODEL_
void BASEFOOT::CopyFromFoundationParams(FOUNDATION_PARA* pFoundationPara)
{
	ciBaseThick = (BYTE)pFoundationPara->wiBasePlateThick;
	this->W = pFoundationPara->wiBasePlateWidth;
	this->wAnchorD = pFoundationPara->wiBoltD;
	this->wAnchorHoleD = pFoundationPara->wiBasePlateHoleD;
	this->S = pFoundationPara->wiBoltSapce;
	this->cnRows = pFoundationPara->cnRows;
	this->cnMainAngleBoltNum = pFoundationPara->cnMainAngleBoltNum;
	this->cnRows = pFoundationPara->cnRows;
	//this->cnMaxSingularRowBolts;	//主材连接螺栓数及布置排数
	this->ciShoeThick = (BYTE)pFoundationPara->wiShoeThick;
	this->ciRibThick = (BYTE)pFoundationPara->wiRibThick;
	this->ciRibMargin = (BYTE)pFoundationPara->wiRibMargin;				//底板厚度、靴板厚度，肋板厚度、边距
	this->wRibWidth = pFoundationPara->wRibWidth;
	this->wRibOuterHeight = pFoundationPara->wRibOuterHeight;
	this->wRibInnerHeight = pFoundationPara->wRibInnerHeight;		//肋板宽度(底板焊接边),高度（分外侧，内侧）
}
bool CTowerMultiLegDesigner::AddBaseFootPlates(CLDSLinePart* pBasePostRod,BASEFOOT* pFoot)
{
	SmartPartPtr pPart;
	DESIGN_PLATE_ITEM* pParaItem;
	CHashSet<CLDSGeneralPlate*>hashBasePlateByPostRod;
	CHashSet<CLDSParamPlate*> hashRelaPlates;
	GEPOINT lenStdVec = pBasePostRod->xPosEnd - pBasePostRod->xPosStart;
	CLDSNode* pBaseNode = lenStdVec.z > 0 ? pBasePostRod->pEnd : pBasePostRod->pStart;
	CFootTempl foottempl;
	CLDSLineAngle* pBaseAngle = NULL;
	CLDSGroupLineAngle* pBaseGroupAngle = NULL;
	if (!pBasePostRod->IsAngle())
		return false;
	pBaseAngle = (CLDSLineAngle*)pBasePostRod;
	SLOPE_TRANSITION transit;
	if (!pBaseNode->RetrieveFootTransitionNode(&transit))
	{
		logerr.Log("0x%X节点提取基础设计信息失败!");
		return false;
	}
	//transit.pNode = pBaseNode;
	//transit.pUpRod = pBaseAngle;
	CLdsPartListStack stackparts(&m_pTower->Parts);
	bool successed= CCommonLjSmartDesign::DesignSlopOrFootPoint(&transit);
	stackparts.EarlyPopStack();
	for (pPart = m_pTower->EnumPartNext(CLS_PARAMPLATE); pPart.blValid; pPart = m_pTower->EnumPartNext(CLS_PARAMPLATE))
	{
		if (pPart.pParamPlate->m_iParamType == TYPE_BASEPLATE)
		{
			CLDSLinePart* pBaseRod = Ta.FromRodHandle(pPart.pParamPlate->m_hBaseRod);
			hashBasePlateByPostRod.SetValue(pBaseRod->handle, pPart.pParamPlate);
			//获取当前基础信息，并根据基础信息确定底板中心位置坐标 wjh-2019.8.12
			BYTE xarrBodyNo[24],xarrLegSerial[24];
			pBaseRod->cfgword.GetExistBodyNos(xarrBodyNo,24);
			pBaseRod->cfgword.GetExistLegSerials(xarrLegSerial,24);
			BYTE ciHeightSerial=xarrBodyNo[0],ciLegSerial=xarrLegSerial[0];
			BYTE ciLegQuad=pPart->chLayer3rd-'0';
			if (ciLegQuad<1||ciLegQuad>4)
				ciLegQuad=CalPoleQuad(pBaseRod->xPosStart,pBaseRod->xPosEnd);
			CLDSFoundation * pFoundation=Ta.AddFoundation(ciHeightSerial,ciLegSerial,ciLegQuad);
			pPart->ucs.origin.x=pFoundation->location.x;
			pPart->ucs.origin.y=pFoundation->location.y;

			pPart.pParamPlate->thick = pFoot->ciBaseThick;
			if ((pParaItem = pPart.pParamPlate->GetDesignItem(KEY2C("C"))) != NULL)
				pParaItem->SetItemValue(Ta.m_xFoundation.biBasePlateType);
			pParaItem = pPart.pParamPlate->GetDesignItem(KEY2C("XA"));
			pParaItem->SetItemValue(pFoot->W*0.5);
			pParaItem = pPart.pParamPlate->GetDesignItem(KEY2C("XB"));
			pParaItem->SetItemValue(pFoot->W*0.5);
			pParaItem = pPart.pParamPlate->GetDesignItem(KEY2C("YA"));
			pParaItem->SetItemValue(pFoot->W*0.5);
			pParaItem = pPart.pParamPlate->GetDesignItem(KEY2C("YB"));
			pParaItem->SetItemValue(pFoot->W*0.5);
			for (CLsRef* pLsRef = pPart.pBasicPlate->GetFirstLsRef(); pLsRef; pLsRef = pPart.pBasicPlate->GetNextLsRef())
			{
				CLDSBolt* pBolt = pLsRef->GetLsPtr();
				pBolt->d = pFoot->wAnchorD;
				pBolt->hole_d_increment = pFoot->wAnchorHoleD - pFoot->wAnchorD;
				pBolt->m_cFuncType = CLDSBolt::FUNC_BASEHOLE;
				pBolt->des_base_pos.cLocationStyle = DESIGN_LS_POS::LOCAL_COORD;
				pBolt->des_base_pos.hPart = pPart->handle;
				pBolt->des_base_pos.wing_offset_dist = (pBolt->des_base_pos.wing_offset_dist > 0) ? pFoot->S : -pFoot->S;
				pBolt->des_base_pos.len_offset_dist = (pBolt->des_base_pos.len_offset_dist > 0) ? pFoot->S : -pFoot->S;
				pBolt->correct_pos();
			}
			pPart.pParamPlate->DesignPlate();
		}
		else if (pPart.pParamPlate->m_iParamType == TYPE_SHOEPLATE)
		{
			pPart.pParamPlate->Thick = pFoot->ciShoeThick;
			//根据王猛提议临时把靴板台口高设成肋板高，以防肋板伸出靴板不好看 wjh-2019.8.2
			pPart.pParamPlate->SetDesignItemValue(KEY2C("A"), pFoot->wRibOuterHeight);
			pPart.pParamPlate->SetDesignItemValue(KEY2C("IA"), pFoot->wRibInnerHeight);
			//TODO:布置靴板上的主斜材螺栓pFoot->cnMainAngleBoltNum,cnRows;
			pPart.pParamPlate->DesignPlate();
			hashRelaPlates.SetValue(pPart->handle, pPart.pParamPlate);
		}
		else if (pPart.pParamPlate->m_iParamType == TYPE_RIBPLATE)
		{
			pParaItem = pPart.pParamPlate->GetDesignItem(KEY2C("A"));
			SmartPlatePtr pBasePlate = (CLDSParamPlate*)Ta.Parts.FromHandle(pParaItem->hValue, CLS_PARAMPLATE);//,CLS_PLATE);
			if (!pBasePlate.blValid)
				continue;	//底座为普通钢板或缺失时视为普通肋板，跳过 wjh-2019.7.15
			CLDSParamPlate::RIBPLATE_CORNER corner;
			corner.LoadParams(pPart.pParamPlate, NULL);
			if (pBasePlate.pParamPlate->m_iParamType == TYPE_BASEPLATE)
			{
				pPart.pParamPlate->Thick = pFoot->ciRibThick;
				double corner_B = pFoot->wRibWidth;
				double corner_A = pFoot->wRibOuterHeight;//pFoot->wRibInnerHeight;
				pParaItem = pPart.pParamPlate->GetDesignItem(KEY2C("E"));
				pParaItem->SetItemValue(corner_A);;
				pParaItem = pPart.pParamPlate->GetDesignItem(KEY2C("G"));
				pParaItem->SetItemValue(corner_B);;
				double Ox = corner.Ox.fValue;
				double Oy = corner.Oy.fValue;
				if (fabs(pPart->ucs.axis_x*pBasePlate->ucs.axis_x) > fabs(pPart->ucs.axis_x*pBasePlate->ucs.axis_y))
				{	//X向肋板，指定Y向偏移
					if (corner.Oy.fValue > 0)
						Oy = pFoot->W / 2 - pFoot->ciRibMargin - pFoot->ciRibThick*0.5;
					else
						Oy = -pFoot->W / 2 + pFoot->ciRibMargin + pFoot->ciRibThick*0.5;
					pParaItem = pPart.pParamPlate->GetDesignItem(KEY2C("Oy"));
					pParaItem->SetItemValue(Oy);;
				}
				else
				{
					if (corner.Ox.fValue > 0)
						Ox = pFoot->W / 2 - pFoot->ciRibMargin - pFoot->ciRibThick*0.5;
					else
						Ox = -pFoot->W / 2 + pFoot->ciRibMargin + pFoot->ciRibThick*0.5;
					pParaItem = pPart.pParamPlate->GetDesignItem(KEY2C("Ox"));
					pParaItem->SetItemValue(Ox);;
				}
				//pParaItem->SetItemValue(Ta.m_xFoundation.biBasePlateType);
			}
			pPart.pParamPlate->DesignPlate();
			hashRelaPlates.SetValue(pPart->handle, pPart.pParamPlate);
		}
	}
	for (CLDSParamPlate* pParamPlate = hashRelaPlates.GetFirst(); pParamPlate; pParamPlate = hashRelaPlates.GetNext())
	{
		pParamPlate->DesignPlate();
		if (pParamPlate->m_iParamType == TYPE_SHOEPLATE)
		{
			for (CLsRef* pLsRef = pParamPlate->GetFirstLsRef(); pLsRef; pLsRef = pParamPlate->GetNextLsRef())
			{
				pLsRef->GetLsPtr()->CorrectBoltZnL0Para();
				pLsRef->GetLsPtr()->correct_pos();
			}
		}
	}
	return successed;
}
void CTowerMultiLegDesigner::AddOrUpdateBaseFoot(BASEFOOT* pFoot, bool blOnlyUpdateBaseFoot/*=false*/)
{
	SmartPartPtr pPart;
	DESIGN_PLATE_ITEM* pParaItem;
	CHashSet<CLDSGeneralPlate*>hashBasePlateByPostRod;
	CHashSet<CLDSParamPlate*> hashRelaPlates;
	CXhChar50 szProcName = !blOnlyUpdateBaseFoot?"设计塔脚板":"更新塔脚板参数";
	DisplayProcess(0, szProcName);
	int i = 0, count = 0;
	for (pPart = Ta.Parts.GetFirst(CLS_PARAMPLATE); blOnlyUpdateBaseFoot&&pPart.blValid; pPart = Ta.Parts.GetNext(CLS_PARAMPLATE))
	{
		if (pPart.pParamPlate->m_iParamType == TYPE_BASEPLATE)
			count++;
	}
	for(pPart=Ta.Parts.GetFirst(CLS_PARAMPLATE);pPart.IsHasPtr();pPart=Ta.Parts.GetNext(CLS_PARAMPLATE))
	{
		if(pPart.pParamPlate->m_iParamType==TYPE_BASEPLATE)
		{
			CLDSLinePart* pBaseRod=Ta.FromRodHandle(pPart.pParamPlate->m_hBaseRod);
			hashBasePlateByPostRod.SetValue(pBaseRod->handle,pPart.pParamPlate);
			//获取当前基础信息，并根据基础信息确定底板中心位置坐标 wjh-2019.8.12
			BYTE xarrBodyNo[24],xarrLegSerial[24];
			pBaseRod->cfgword.GetExistBodyNos(xarrBodyNo,24);
			pBaseRod->cfgword.GetExistLegSerials(xarrLegSerial,24);
			BYTE ciHeightSerial=xarrBodyNo[0],ciLegSerial=xarrLegSerial[0];
			BYTE ciLegQuad=pPart->chLayer3rd-'0';
			if (ciLegQuad<1||ciLegQuad>4)
				ciLegQuad=CalPoleQuad(pBaseRod->xPosStart,pBaseRod->xPosEnd);
			CLDSFoundation * pFoundation=Ta.AddFoundation(ciHeightSerial,ciLegSerial,ciLegQuad);
			pPart->ucs.origin.x=pFoundation->location.x;
			pPart->ucs.origin.y=pFoundation->location.y;

			pPart.pParamPlate->thick=pFoot->ciBaseThick;
			if((pParaItem=pPart.pParamPlate->GetDesignItem(KEY2C("C")))!=NULL)
				pParaItem->SetItemValue(Ta.m_xFoundation.biBasePlateType);
			pParaItem=pPart.pParamPlate->GetDesignItem(KEY2C("XA"));
			pParaItem->SetItemValue(pFoot->W*0.5);
			pParaItem=pPart.pParamPlate->GetDesignItem(KEY2C("XB"));
			pParaItem->SetItemValue(pFoot->W*0.5);
			pParaItem=pPart.pParamPlate->GetDesignItem(KEY2C("YA"));
			pParaItem->SetItemValue(pFoot->W*0.5);
			pParaItem=pPart.pParamPlate->GetDesignItem(KEY2C("YB"));
			pParaItem->SetItemValue(pFoot->W*0.5);
			for(CLsRef* pLsRef=pPart.pBasicPlate->GetFirstLsRef();pLsRef;pLsRef=pPart.pBasicPlate->GetNextLsRef())
			{
				CLDSBolt* pBolt=pLsRef->GetLsPtr();
				pBolt->d=pFoot->wAnchorD;
				pBolt->hole_d_increment=pFoot->wAnchorHoleD-pFoot->wAnchorD;
				pBolt->m_cFuncType=CLDSBolt::FUNC_BASEHOLE;
				pBolt->des_base_pos.cLocationStyle=DESIGN_LS_POS::LOCAL_COORD;
				pBolt->des_base_pos.hPart=pPart->handle;
				pBolt->des_base_pos.wing_offset_dist=(pBolt->des_base_pos.wing_offset_dist>0)?pFoot->S:-pFoot->S;
				pBolt->des_base_pos.len_offset_dist =(pBolt->des_base_pos.len_offset_dist >0)?pFoot->S:-pFoot->S;
				pBolt->correct_pos();
			}
			pPart.pParamPlate->DesignPlate();
			i++;
			if(blOnlyUpdateBaseFoot)
				DisplayProcess((99*i)/count, szProcName);
		}
		else if(pPart.pParamPlate->m_iParamType==TYPE_SHOEPLATE)
		{
			pPart.pParamPlate->Thick=pFoot->ciShoeThick;
			//TODO:布置靴板上的主斜材螺栓pFoot->cnMainAngleBoltNum,cnRows;
			pPart.pParamPlate->DesignPlate();
			hashRelaPlates.SetValue(pPart->handle,pPart.pParamPlate);
		}
		else if(pPart.pParamPlate->m_iParamType==TYPE_RIBPLATE)
		{
			pParaItem=pPart.pParamPlate->GetDesignItem(KEY2C("A"));
			SmartPlatePtr pBasePlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pParaItem->hValue,CLS_PARAMPLATE);//,CLS_PLATE);
			if (!pBasePlate.blValid)
				continue;	//底座为普通钢板或缺失时视为普通肋板，跳过 wjh-2019.7.15
			CLDSParamPlate::RIBPLATE_CORNER corner;
			corner.LoadParams(pPart.pParamPlate,NULL);
			if(pBasePlate.pParamPlate->m_iParamType==TYPE_BASEPLATE)
			{
				pPart.pParamPlate->Thick=pFoot->ciRibThick;
				double corner_B=pFoot->wRibWidth;
				double corner_A=pFoot->wRibOuterHeight;//pFoot->wRibInnerHeight;
				pParaItem=pPart.pParamPlate->GetDesignItem(KEY2C("E"));
				pParaItem->SetItemValue(corner_A);;
				pParaItem=pPart.pParamPlate->GetDesignItem(KEY2C("G"));
				pParaItem->SetItemValue(corner_B);;
				double Ox=corner.Ox.fValue;
				double Oy=corner.Oy.fValue;
				if(fabs(pPart->ucs.axis_x*pBasePlate->ucs.axis_x)>fabs(pPart->ucs.axis_x*pBasePlate->ucs.axis_y))
				{	//X向肋板，指定Y向偏移
					if(corner.Oy.fValue>0)
						Oy=pFoot->W/2-pFoot->ciRibMargin-pFoot->ciRibThick*0.5;
					else
						Oy=-pFoot->W/2+pFoot->ciRibMargin+pFoot->ciRibThick*0.5;
					pParaItem=pPart.pParamPlate->GetDesignItem(KEY2C("Oy"));
					pParaItem->SetItemValue(Oy);;
				}
				else
				{
					if(corner.Ox.fValue>0)
						Ox=pFoot->W/2-pFoot->ciRibMargin-pFoot->ciRibThick*0.5;
					else
						Ox=-pFoot->W/2+pFoot->ciRibMargin+pFoot->ciRibThick*0.5;
					pParaItem=pPart.pParamPlate->GetDesignItem(KEY2C("Ox"));
					pParaItem->SetItemValue(Ox);;
				}
				//pParaItem->SetItemValue(Ta.m_xFoundation.biBasePlateType);
			}
			pPart.pParamPlate->DesignPlate();
			hashRelaPlates.SetValue(pPart->handle,pPart.pParamPlate);
		}
	}
	if (blOnlyUpdateBaseFoot)
	{
		CUnifyModelLayer intelli;
		const double TOLERANCE = 5.0;
		COLINEAR_TOLERANCE tolerance(TOLERANCE, 0.001);
		intelli.UnifyObjsLayer(tolerance);
		intelli.RetrievedTowerModeTree(gxModelTree);

		gxModelTree.SetBelongTower(&Ta);
		gxModelTree.AddOrUpdateFoundations();
		DisplayProcess(100, szProcName);
		return;
	}
	DisplayProcess(1, szProcName);
	CModelHeightNode* pHeightNode;
	CTowerModelTree* pModelTree=&modeltree;
	int hits=1, countOfFoot = 0;
	for (pHeightNode = pModelTree->hashHeights.GetFirst(); pHeightNode; pHeightNode = pModelTree->hashHeights.GetNext())
	{
		for (int i = 0; i < 24; i++)
		{
			if (pHeightNode->xarrSubLegs[i].Id == 0)
				break;
			countOfFoot++;
		}
	}
	for(pHeightNode=pModelTree->hashHeights.GetFirst();pHeightNode;pHeightNode=pModelTree->hashHeights.GetNext())
	{
		CLDSLinePart* pMirRod;
		for(int i=0;i<24;i++,hits++)
		{
			if(pHeightNode->xarrSubLegs[i].Id==0)
				break;
			SmartPlatePtr pBasePlate=hashBasePlateByPostRod.GetValue(pHeightNode->xarrSubLegs[i].pPostRod->handle);
			if(pBasePlate.IsHasPtr())
			{
				pBasePlate.pParamPlate->design.inited=false;
				pBasePlate.pParamPlate->ucs.origin.x=pHeightNode->xarrSubLegs[i].fBaseSizeX*0.5;
				pBasePlate.pParamPlate->ucs.origin.y=pHeightNode->xarrSubLegs[i].fBaseSizeY*0.5;
				pBasePlate.pParamPlate->ucs.origin.z=pHeightNode->xarrSubLegs[i].zFoundationTopPlane;
				pBasePlate.pParamPlate->DesignPlate();
			}
			else if(!AddBaseFootPlates(pHeightNode->xarrSubLegs[i].pPostRod,pFoot))
				logerr.Log("%d呼高%C接腿第%d象限设计失败",pHeightNode->Id,'@'+pHeightNode->xarrSubLegs[i].Id,1);
			if((pMirRod=pHeightNode->xarrSubLegs[i].pPostRod->GetMirRod(MIRMSG(1)))!=NULL)
			{	//X轴对称
				pBasePlate=hashBasePlateByPostRod.GetValue(pMirRod->handle);
				if(pBasePlate.IsHasPtr())
				{
					pBasePlate.pParamPlate->design.inited=false;
					pBasePlate.pParamPlate->ucs.origin.x= pHeightNode->xarrSubLegs[i].fBaseSizeX*0.5;
					pBasePlate.pParamPlate->ucs.origin.y=-pHeightNode->xarrSubLegs[i].fBaseSizeY*0.5;
					pBasePlate.pParamPlate->ucs.origin.z= pHeightNode->xarrSubLegs[i].zFoundationTopPlane;
					pBasePlate.pParamPlate->DesignPlate();
				}
				else if(!AddBaseFootPlates(pMirRod,pFoot))
					logerr.Log("%d呼高%C接腿第%d象限设计失败",pHeightNode->Id,'@'+pHeightNode->xarrSubLegs[i].Id,3);
			}
			if((pMirRod=pHeightNode->xarrSubLegs[i].pPostRod->GetMirRod(MIRMSG(2)))!=NULL)
			{	//Y轴对称
				pBasePlate=hashBasePlateByPostRod.GetValue(pMirRod->handle);
				if(pBasePlate.IsHasPtr())
				{
					pBasePlate.pParamPlate->design.inited=false;
					pBasePlate.pParamPlate->ucs.origin.x=-pHeightNode->xarrSubLegs[i].fBaseSizeX*0.5;
					pBasePlate.pParamPlate->ucs.origin.y= pHeightNode->xarrSubLegs[i].fBaseSizeY*0.5;
					pBasePlate.pParamPlate->ucs.origin.z= pHeightNode->xarrSubLegs[i].zFoundationTopPlane;
					pBasePlate.pParamPlate->DesignPlate();
				}
				else if(!AddBaseFootPlates(pMirRod,pFoot))
					logerr.Log("%d呼高%C接腿第%d象限设计失败",pHeightNode->Id,'@'+pHeightNode->xarrSubLegs[i].Id,2);
			}
			if((pMirRod=pHeightNode->xarrSubLegs[i].pPostRod->GetMirRod(MIRMSG(3)))!=NULL)
			{	//Z轴对称
				pBasePlate=hashBasePlateByPostRod.GetValue(pMirRod->handle);
				if(pBasePlate.IsHasPtr())
				{
					pBasePlate.pParamPlate->design.inited=false;
					pBasePlate.pParamPlate->ucs.origin.x=-pHeightNode->xarrSubLegs[i].fBaseSizeX*0.5;
					pBasePlate.pParamPlate->ucs.origin.y=-pHeightNode->xarrSubLegs[i].fBaseSizeY*0.5;
					pBasePlate.pParamPlate->ucs.origin.z= pHeightNode->xarrSubLegs[i].zFoundationTopPlane;
					pBasePlate.pParamPlate->DesignPlate();
				}
				else if(!AddBaseFootPlates(pMirRod,pFoot))
					logerr.Log("%d呼高%C接腿第%d象限设计失败",pHeightNode->Id,'@'+pHeightNode->xarrSubLegs[i].Id,4);
			}
			int percent = 1 + ftoi(hits*98.0 / countOfFoot);
			DisplayProcess(percent, szProcName);
		}
	}
	for(CLDSParamPlate* pParamPlate=hashRelaPlates.GetFirst();pParamPlate;pParamPlate=hashRelaPlates.GetNext())
	{
		pParamPlate->DesignPlate();
		if(pParamPlate->m_iParamType==TYPE_SHOEPLATE)
		{
			for(CLsRef* pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			{
				pLsRef->GetLsPtr()->CorrectBoltZnL0Para();
				pLsRef->GetLsPtr()->correct_pos();
			}
		}
	}
	DisplayProcess(100, szProcName);
}
static bool TurnAngleLayout(CLDSLineAngle* pAngle,char ciFromXtoWing,char ciFromYtoWing)
{
	if(ciFromXtoWing=='X'||ciFromYtoWing=='Y')
		return false;	//无变化
	for(CLsRef* pLsRef=pAngle->GetFirstLsRef();pLsRef;pLsRef=pAngle->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		if(pBolt->des_base_pos.hPart==pAngle->handle)
		{
			if(pBolt->des_base_pos.offset_wing==0&&(ciFromXtoWing=='Y'||ciFromXtoWing=='y'))
				pBolt->des_base_pos.offset_wing=1;
			else if(pBolt->des_base_pos.offset_wing==1&&(ciFromYtoWing=='X'||ciFromYtoWing=='x'))
				pBolt->des_base_pos.offset_wing=0;
		}
		if(pBolt->des_work_norm.hVicePart==pAngle->handle&&pBolt->des_work_norm.norm_style==1)
		{
			if(pBolt->des_work_norm.norm_wing==0&&(ciFromXtoWing=='Y'||ciFromXtoWing=='y'))
				pBolt->des_work_norm.norm_wing=1;
			if(pBolt->des_work_norm.norm_wing==1&&(ciFromYtoWing=='X'||ciFromYtoWing=='x'))
				pBolt->des_work_norm.norm_wing=0;
		}
		if(pBolt->des_base_pos.datumPoint.m_hDatumRod==pAngle->handle)
		{
			short* psiWorkWing=NULL;
			if(pBolt->des_base_pos.datumPoint.datum_pos_style==1)
				psiWorkWing=&pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing;
			else if(pBolt->des_base_pos.datumPoint.datum_pos_style==2)
				psiWorkWing=&pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing;
			if(psiWorkWing)
			{
				if(*psiWorkWing==0&&(ciFromXtoWing=='Y'||ciFromXtoWing=='y'))
					*psiWorkWing=1;
				else if(*psiWorkWing==1&&(ciFromYtoWing=='X'||ciFromYtoWing=='x'))
					*psiWorkWing=0;
			}
		}
		pBolt->correct_worknorm();
		pBolt->correct_pos();
	}
	WING_OFFSET_STYLE xTempWingOffset;
	xTempWingOffset=pAngle->desStartPos.wing_x_offset;
	pAngle->desStartPos.wing_x_offset=pAngle->desStartPos.wing_y_offset;
	pAngle->desStartPos.wing_y_offset=xTempWingOffset;
	xTempWingOffset=pAngle->desEndPos.wing_x_offset;
	pAngle->desEndPos.wing_x_offset=pAngle->desEndPos.wing_y_offset;
	pAngle->desEndPos.wing_y_offset=xTempWingOffset;
	SmartPartPtr pPart;
	CLDSLineAngle* pRayAngle;
	CLdsPartListStack stack(pAngle->BelongModel());
	for(pPart=Ta.EnumPartFirst();pPart.IsHasPtr();pPart=Ta.EnumPartNext())
	{
		if(pPart->GetClassTypeId()==CLS_BOLT)
		{
			CLDSBolt* pBolt=pPart.pBolt;
			if(pBolt->des_base_pos.datumPoint.m_hDatumRod==pAngle->handle&&(ciFromXtoWing=='Y'||ciFromXtoWing=='y'))
			{
				if(pBolt->des_base_pos.datumPoint.datum_pos_style==1)
					pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=1-pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing;
				else if(pBolt->des_base_pos.datumPoint.datum_pos_style==2)
					pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing=1-pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing;
			}
			if(pBolt->des_work_norm.norm_style==1&&pBolt->des_work_norm.hVicePart==pAngle->handle)
				pBolt->des_work_norm.norm_wing=1-pBolt->des_work_norm.norm_wing;
		}
		else if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate* pPlate=pPart.pPlate;
			if(ciFromXtoWing=='Y'||ciFromXtoWing=='y')
			{
				if(pPlate->designInfo.origin.m_hDatumRod==pAngle->handle)
				{
					if(pPlate->designInfo.origin.datum_pos_style==1)
						pPlate->designInfo.origin.des_para.RODEND.offset_wing=1-pPlate->designInfo.origin.des_para.RODEND.offset_wing;
					else if(pPlate->designInfo.origin.datum_pos_style==2)
						pPlate->designInfo.origin.des_para.RODNODE.offset_wing=1-pPlate->designInfo.origin.des_para.RODEND.offset_wing;
				}
				if(pPlate->designInfo.norm.norm_style==1&&pPlate->designInfo.norm.hVicePart==pAngle->handle)
					pPlate->designInfo.norm.norm_wing=1-pPlate->designInfo.norm.norm_wing;
				for(CDesignLjPartPara* pConnRodPara=pPlate->designInfo.partList.GetFirst();pConnRodPara;pConnRodPara=pPlate->designInfo.partList.GetNext())
				{
					if(pConnRodPara->m_nClassTypeId==CLS_LINEANGLE||pConnRodPara->m_nClassTypeId==CLS_GROUPLINEANGLE)
						pConnRodPara->angle.cur_wing_x0_y1=1-pConnRodPara->angle.cur_wing_x0_y1;
				}
			}
		}
		else if(pPart->IsAngle())
		{
			pRayAngle=pPart.pAngle;
			if(pRayAngle==pAngle||pRayAngle->group_father_jg_h==pAngle->handle)
				continue;
			//处理始端位置设计信息
			long hDatumAngle=pRayAngle->desStartPos.datum_jg_h;
			if(hDatumAngle==0&&pRayAngle->pStart)
				hDatumAngle=pRayAngle->pStart->hFatherPart;
			if(hDatumAngle==pAngle->handle){
				if(pRayAngle->desStartPos.IsInDatumJgWingX()&&(ciFromXtoWing=='y'||ciFromXtoWing=='Y'))
					pRayAngle->desStartPos.spatialOperationStyle+=1;
				else if(pRayAngle->desStartPos.IsInDatumJgWingY()&&(ciFromXtoWing=='x'||ciFromXtoWing=='X'))
					pRayAngle->desStartPos.spatialOperationStyle-=1;
			}
			//处理终端位置设计信息
			hDatumAngle=pRayAngle->desEndPos.datum_jg_h;
			if(hDatumAngle==0&&pRayAngle->pEnd)
				hDatumAngle=pRayAngle->pEnd->hFatherPart;
			if(hDatumAngle==pAngle->handle){
				if(pRayAngle->desEndPos.IsInDatumJgWingX()&&(ciFromXtoWing=='y'||ciFromXtoWing=='Y'))
					pRayAngle->desEndPos.spatialOperationStyle+=1;
				else if(pRayAngle->desEndPos.IsInDatumJgWingY()&&(ciFromXtoWing=='x'||ciFromXtoWing=='X'))
					pRayAngle->desEndPos.spatialOperationStyle-=1;
			}
		}
	}
	return true;
}
//修正V斜材朝向
GEPOINT GetDiagMirVector(char ciLegQuad,const double* vec3d)
{
	if(ciLegQuad==1||ciLegQuad==4)
		return GEPOINT( vec3d[1], vec3d[0],vec3d[2]);
	else if(ciLegQuad==2||ciLegQuad==3)
		return GEPOINT(-vec3d[1],-vec3d[0],vec3d[2]);
	else if(ciLegQuad==3)
		return GEPOINT(-vec3d[1],-vec3d[0],vec3d[2]);
	else
		return GEPOINT(vec3d);
}
int RetrieveExistBolts(BOLTSET& boltset,CLDSLineAngle* pAngle,const double* vxSearchOrg,char ciWingX0Y1,WORD wiSearchRadius)
{
	boltset.Empty();
	GEPOINT lenStdVec=pAngle->xPosEnd-pAngle->xPosStart;
	normalize(lenStdVec);

	double posx=lenStdVec*(GEPOINT(vxSearchOrg)-pAngle->xPosStart);
	for(CLsRef* pLsRef=pAngle->GetFirstLsRef();pLsRef;pLsRef=pAngle->GetNextLsRef())
	{
		int ciBoltWingX0Y1=0;
		GEPOINT xBoltPos;
		pAngle->GetBoltIntersPos(pLsRef->GetLsPtr(),xBoltPos,&ciBoltWingX0Y1);
		if(ciBoltWingX0Y1!=ciWingX0Y1)
			continue;
		double bolt_posx=lenStdVec*(xBoltPos-pAngle->xPosStart);
		if(fabs(bolt_posx-posx)>wiSearchRadius)
			continue;	//超出当前搜索范围
		boltset.append(pLsRef->GetLsPtr());
	}
	return boltset.GetNodeNum();
}
int RetrieveExistBolts(BOLTSET& boltset,CLDSLineAngle* pAngle,CLDSNode* pDatumNode,char ciWingX0Y1,WORD wiSearchRadius)
{
	boltset.Empty();
	GEPOINT lenStdVec=pAngle->xPosEnd-pAngle->xPosStart;
	normalize(lenStdVec);
	bool bIncOddment=true;
	if(pDatumNode==pAngle->pStart)
	{
		GEPOINT vxSearchOrg=pAngle->xPosStart;
		if(bIncOddment)
			vxSearchOrg-=lenStdVec*pAngle->startOdd();
		return RetrieveExistBolts(boltset,pAngle,vxSearchOrg,ciWingX0Y1,wiSearchRadius);
	}
	else if(pDatumNode==pAngle->pEnd)
	{
		GEPOINT vxSearchOrg=pAngle->xPosEnd;
		if(bIncOddment)
			vxSearchOrg+=lenStdVec*pAngle->endOdd();
		return RetrieveExistBolts(boltset,pAngle,vxSearchOrg,ciWingX0Y1,wiSearchRadius);
	}
	else
	{
		GEPOINT datumpos=pAngle->GetDatumPosBer(pDatumNode);
		return RetrieveExistBolts(boltset,pAngle,datumpos,ciWingX0Y1,wiSearchRadius);
	}
}
bool MULTILEG_POST_CONNPLATE::JudgeConnectTipAndWing(CLDSLineAngle* pAngle,BYTE* piStart0End1,BYTE* piWingX0Y1)
{
	if(vPlaneNormal.IsZero())
		return false;
	if(pAngle->pStart==this->pBaseNode)
		*piStart0End1=0;
	else if(pAngle->pEnd==this->pBaseNode)
		*piStart0End1=1;
	else
		*piStart0End1=2;
	if(fabs(pAngle->vxWingNorm*vPlaneNormal)>fabs(pAngle->vyWingNorm*vPlaneNormal))
		*piWingX0Y1=0;
	else
		*piWingX0Y1=1;
	return true;
}
bool MULTILEG_HORI_K_CONNPLATE::JudgeConnectTipAndWing(CLDSLineAngle* pAngle,BYTE* piStart0End1,BYTE* piWingX0Y1)
{
	MULTILEG_POST_CONNPLATE connplate;
	connplate.vPlaneNormal=vPlaneNormal;
	connplate.pBaseNode=pBaseNode;
	return connplate.JudgeConnectTipAndWing(pAngle,piStart0End1,piWingX0Y1);
}
bool CTowerMultiLegDesigner::CorrectLegPostKNodeConnPlate(MULTILEG_POST_CONNPLATE& despara,bool bMirCreate/*=true*/)
{
	CLDSLineAngle* pAngle=NULL;
	//计算当前工作面法线
	GEPOINT vLenStdVec=despara.pPostAngle->xPosEnd-despara.pPostAngle->xPosStart;
	GEPOINT vLineLenStdVec=despara.pPostAngle->pEnd->xOriginalPos-despara.pPostAngle->pStart->xOriginalPos;
	GEPOINT vLinePlaneNormal;	//单线模型的工作面（避免计算误差，判断选取腿部斜材相对更准确)
	normalize(vLineLenStdVec);
	if(despara.ciFront0OrSide1==0)
	{
		despara.vPlaneNormal=vLenStdVec^GEPOINT(1,0,0);
		if(despara.vPlaneNormal.y<0)
			despara.vPlaneNormal*=-1;
		vLinePlaneNormal=vLineLenStdVec^GEPOINT(1,0,0);
		if(vLinePlaneNormal.y<0)
			vLinePlaneNormal*=-1;
	}
	else //if(despara.ciFront0OrSide1==1)
	{
		despara.vPlaneNormal=vLenStdVec^GEPOINT(0,1,0);
		if(despara.vPlaneNormal.x<0)
			despara.vPlaneNormal*=-1;
		vLinePlaneNormal=vLineLenStdVec^GEPOINT(0,1,0);
		if(vLinePlaneNormal.x<0)
			vLinePlaneNormal*=-1;
	}
	normalize(despara.vPlaneNormal);
	normalize(vLinePlaneNormal);
	//
	CDesignLjPara designinfo;
	designinfo.m_bEnableFlexibleDesign=true;
	designinfo.m_hBaseNode=despara.pBaseNode->handle;
	BYTE ciTipS0E1=2,ciWorkWingX0Y1=0;
	CLDSBolt* pBolt;
	CConnectInfo* pCurrConnInfo=NULL;
	CDesignLjPartPara* pConnAnglePara;
	BOLTSET boltset,shareboltset;
	PRESET_ARRAY8<CLDSBolt*> existbolts;
	JGZJ jgzj;
	LSSPACE_STRU LsSpace;
	WORD wiSearchRadius,wiSingleRowSpace=despara.wiPostAngleBoltSpace;//nBoltSpace;
	BOLT_LOCATION* pLocation;
	CXhSimpleList<BOLT_LOCATION> listLocations;
	BOLT_LAYOUT layout;
	UINT uiBoltMd;
	char ciConnWing=0;//=designinfo.norm.norm_wing==0?'X':'Y';
	long g[4];//={jgzj.g,jgzj.g1,jgzj.g2,jgzj.g3};
	CMappingBolts mapping;
	int nActualPlateThick=6;//
	if(despara.pUpXieAngle)	//有时存在腿身连接处没有塔身连接斜材和腿部辅材情况
		nActualPlateThick=max(nActualPlateThick,(int)despara.pUpXieAngle->size_thick);
	if(despara.pHoriAngle)
		nActualPlateThick=max(nActualPlateThick,(int)despara.pHoriAngle->size_thick);
	if(nActualPlateThick%2)
		nActualPlateThick++;
	//布置塔身斜材及横材上的螺栓
	int index=0,j=0,matchcount=0;
	for(index =0; index <2; index++)
	{
		CLDSLineAngle* pUpperAngle= index==0?despara.pUpXieAngle:despara.pHoriAngle;
		if(pUpperAngle==NULL)
			continue;
		despara.JudgeConnectTipAndWing(pUpperAngle,&ciTipS0E1,&ciWorkWingX0Y1);
		//添加钢板连接杆件信息
		pConnAnglePara=designinfo.partList.Add(pUpperAngle->handle);
		pConnAnglePara->angle.bTwoEdge=false;
		pConnAnglePara->angle.cur_wing_x0_y1=ciWorkWingX0Y1;
		pConnAnglePara->iFaceNo=1;
		pConnAnglePara->start0_end1=ciTipS0E1;
		pConnAnglePara->m_nClassTypeId=CLS_LINEANGLE;//despara.pLeftUpperAngle->GetClassTypeId();
		GEPOINT vWorkNormal=ciWorkWingX0Y1==0?pUpperAngle->vxWingNorm:pUpperAngle->vyWingNorm;
		if(ciTipS0E1==0)
		{
			pUpperAngle->desStartOdd.m_iOddCalStyle=0;
			pUpperAngle->desStartOdd.m_hRefPart1=despara.pPostAngle->handle;
			pUpperAngle->desStartOdd.m_hRefPart2= index ==0?despara.pHoriAngle->handle:0;
			if(vWorkNormal*despara.vPlaneNormal<0)
			{
				if(ciWorkWingX0Y1==0)
					pUpperAngle->desStartPos.wing_x_offset.SetOffset(-nActualPlateThick);
				else
					pUpperAngle->desStartPos.wing_y_offset.SetOffset(-nActualPlateThick);
			}
			pUpperAngle->CalStartOddment();
		}
		else if(ciTipS0E1==1)
		{
			pUpperAngle->desEndOdd.m_iOddCalStyle=0;
			pUpperAngle->desEndOdd.m_hRefPart1=despara.pPostAngle->handle;
			pUpperAngle->desEndOdd.m_hRefPart2= index ==0?despara.pHoriAngle->handle:0;
			if(vWorkNormal*despara.vPlaneNormal<0)
			{
				if(ciWorkWingX0Y1==0)
					pUpperAngle->desEndPos.wing_x_offset.SetOffset(-nActualPlateThick);
				else
					pUpperAngle->desEndPos.wing_y_offset.SetOffset(-nActualPlateThick);
			}
			pUpperAngle->CalEndOddment();
		}

		jgzj=pUpperAngle->GetZhunJu(ciWorkWingX0Y1+'X');
		g[0]=jgzj.g;
		g[1]=jgzj.g1;
		g[2]=jgzj.g2;
		g[3]=jgzj.g3;
		listLocations.Empty();
		layout.ciLayoutStyle=0;
		if(ciTipS0E1==0)
			pCurrConnInfo=&pUpperAngle->connectStart;
		else
			pCurrConnInfo=&pUpperAngle->connectEnd;
		int nBoltNum=max(2,pCurrConnInfo->wnConnBoltN);
		uiBoltMd=ciTipS0E1==0?pUpperAngle->connectStart.d:pUpperAngle->connectEnd.d;
		GetLsSpace(LsSpace,uiBoltMd);
		layout.basepoint.ciPosCalStyle=0;
		layout.cnCount=max(2,(BYTE)pCurrConnInfo->wnConnBoltN);
		layout.ciRows=0;
		layout.ciLayoutStyle=0;
		layout.ciFlowDirection=(ciTipS0E1==0)?BOLT_LAYOUT::FLOW_StoE:BOLT_LAYOUT::FLOW_EtoS;
		layout.ciDatumWing=ciWorkWingX0Y1+'X';
		listLocations.Empty();
		layout.LayoutBolts(g,&listLocations,LsSpace.EndSpace,LsSpace.SingleRowSpace,LsSpace.doubleRowSpace);
		//提取相应位置的已布螺栓集合
		existbolts.ReSize(0);
		wiSearchRadius=(WORD)(nBoltNum*LsSpace.SingleRowSpace);
		boltset.Empty();
		RetrieveExistBolts(boltset,pUpperAngle,despara.pBaseNode,ciWorkWingX0Y1,wiSearchRadius);
		for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
		{
			shareboltset.append(pBolt);
			existbolts.Append(pBolt);
		}
		//布置螺栓，填充螺栓设计信息
		for(pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
			pLocation->TestCalBoltPosition(pUpperAngle,despara.pBaseNode);
		mapping.ClearMapping();
		mapping.InitWorkPlaneCS(pUpperAngle,ciWorkWingX0Y1,despara.pBaseNode);
		matchcount=mapping.Mapping(existbolts.pPresetObjs,existbolts.Count,&listLocations);
		for(pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
		{
			if(pLocation->pRelaBolt)
				pBolt=pLocation->pRelaBolt;
			else
				pBolt=(CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
			pBolt->SetLayer("SP*");
			pBolt->layer(2)=pUpperAngle->Layer(2);
			pBolt->cfgword=pUpperAngle->cfgword;
			pBolt->d=uiBoltMd;
			pBolt->des_base_pos.hPart=pUpperAngle->handle;
			pLocation->WriteParamToBolt(pBolt,pUpperAngle,despara.pBaseNode);
			pBolt->AddL0Thick(pUpperAngle->handle,TRUE,TRUE);
			pBolt->CalGuigeAuto();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			shareboltset.append(pBolt);
			pUpperAngle->AppendLsRef(pBolt->GetLsRef());
		}
	}
	//设计接腿辅材连接板
	SmartRodPtr pConnRod;
	CModelHeightNode* pHeight=modeltree.hashHeights.GetValue(despara.ciHeightSerial);
	for(j=0;j<24;j++)
	{
		if(pHeight->xarrSubLegs[j].Id==0)
			break;
		CLDSLineAngle* pCurrLegPostAngle=NULL;
		CLDSLineAngle* pLegBraceAngle=NULL;
		for(pConnRod=Ta.EnumRodFirst();pConnRod.IsHasPtr();pConnRod=Ta.EnumRodNext())
		{
			if(!pConnRod->IsLegObj()||!pHeight->xarrSubLegs[j].pPostRod->ModuleTogetherWith(pConnRod))
				continue;
			if((pConnRod->pStart&&pConnRod->pStart==despara.pBaseNode)||(pConnRod->pEnd&&pConnRod->pEnd==despara.pBaseNode))
			{
				GEPOINT vRayVec=pConnRod->pEnd->xOriginalPos-pConnRod->pStart->xOriginalPos;
				normalize(vRayVec);
				if(fabs(vLinePlaneNormal*vRayVec)>EPS2)
					continue;
				else if(pCurrLegPostAngle==NULL&&fabs(vRayVec*vLineLenStdVec)>EPS_COS2)
					despara.pPostAngle=pCurrLegPostAngle=pConnRod.pAngle;
				else if((despara.ciFront0OrSide1==0&&(fabs(vRayVec.x)>fabs(vRayVec.y)*4))||
						(despara.ciFront0OrSide1==1&&(fabs(vRayVec.x)*4<fabs(vRayVec.y))))
					despara.pLegBraceAngle=pLegBraceAngle=pConnRod.pAngle;
			}
			else
				continue;
			if(pCurrLegPostAngle!=NULL&&pLegBraceAngle!=NULL)
				break;
		}
		//查找该节点当前面是否已设计连接板
		CLDSPlate* pPlate=NULL,*pExistPlate=NULL;
		for(pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
		{
			if(pPlate->m_hBaseNode!=despara.pBaseNode->handle||pPlate->m_hBaseRod!=pCurrLegPostAngle->handle)
				continue;
			if(!pCurrLegPostAngle->ModuleTogetherWith(pPlate))
				continue;	//不属于当前接腿
			if(pPlate->designInfo.norm.hVicePart!=pCurrLegPostAngle->handle||fabs(pPlate->ucs.axis_z*despara.vPlaneNormal)<0.9)
				continue;
			if(pPlate->wiType!=CLDSPlate::CONNECT_PLATE)
				continue;
			pExistPlate=pPlate;
			break;
		}
		//搜索是否已存在相应的角脊线腿身连接板
		despara.JudgeConnectTipAndWing(pCurrLegPostAngle, &ciTipS0E1, &ciWorkWingX0Y1);
		if(pExistPlate!=NULL)
		{
			pPlate=pExistPlate;
			pPlate->designInfo.m_bEnableFlexibleDesign=true;
			pPlate->designInfo.m_hBaseNode=despara.pBaseNode->handle;
			pPlate->designInfo.m_hBasePart=pCurrLegPostAngle->handle;
		}
		else
		{
			CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(
				despara.pBaseNode->handle, pCurrLegPostAngle->handle, 'X' + ciWorkWingX0Y1,
				pHeight->xarrSubLegs[j].idModelLegSerial);
			if (pRegister->hPlate > 0x20)
				pPlate = (CLDSPlate*)Ta.Parts.FromHandle(pRegister->hPlate, CLS_PLATE);
			else
			{
				pPlate = (CLDSPlate*)Ta.Parts.append(CLS_PLATE);
				pRegister->hPlate = pPlate->handle;
			}
			if(despara.ciFront0OrSide1==0)
				pPlate->SetLayer("LPQ");
			else if(despara.ciFront0OrSide1==1)
				pPlate->SetLayer("LPY");
			pPlate->cfgword=CFGWORD(despara.ciHeightSerial,pHeight->xarrSubLegs[j].Id);
			designinfo.CloneTo(pPlate->designInfo);
			pPlate->designInfo.m_bEnableFlexibleDesign=true;
			pPlate->designInfo.m_hBaseNode=despara.pBaseNode->handle;
			pPlate->designInfo.m_hBasePart=pCurrLegPostAngle->handle;
			//设定连接板基点参数
			pPlate->designInfo.origin.datum_pos_style=2;
			pPlate->designInfo.origin.des_para.RODNODE.hNode=despara.pBaseNode->handle;
			pPlate->designInfo.origin.des_para.RODNODE.hRod=pCurrLegPostAngle->handle;
			pPlate->designInfo.origin.des_para.RODNODE.offset_wing=ciWorkWingX0Y1;
			pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=0;
			pPlate->designInfo.origin.UpdatePos(this->m_pTower);
			//设定连接板法线参数
			pPlate->designInfo.norm.norm_style=1;
			pPlate->designInfo.norm.hVicePart=pCurrLegPostAngle->handle;
			pPlate->designInfo.norm.norm_wing=ciWorkWingX0Y1;
			pPlate->designInfo.norm.direction=0;	//由里朝外
		}
		//ciTipS0E1=2,ciWorkWingX0Y1=(BYTE)pPlate->designInfo.norm.norm_wing;
		//布置基准主材上的螺栓
		jgzj=pCurrLegPostAngle->GetZhunJu(ciWorkWingX0Y1+'X');
		g[0]=jgzj.g; g[1]=jgzj.g1;
		g[2]=jgzj.g2;g[3]=jgzj.g3;
		uiBoltMd=16;
		CConnectInfo *pXieRodConn=NULL,*pHoriRodConn=NULL;
		despara.ciPostAngleBoltNum=3;
		if(despara.pUpXieAngle)
		{
			pXieRodConn=despara.pUpXieAngle->pStart==despara.pBaseNode?&despara.pUpXieAngle->connectStart:&despara.pUpXieAngle->connectEnd;
			uiBoltMd=max(uiBoltMd,(UINT)pXieRodConn->d);	//此处连接板在主材上的螺栓取决于斜材和横材
			despara.ciPostAngleBoltNum=(BYTE)max(despara.ciPostAngleBoltNum,ftoi(pXieRodConn->wnConnBoltN*1.5));
		}
		if(despara.pHoriAngle)
		{
			pHoriRodConn=despara.pHoriAngle->pStart==despara.pBaseNode?&despara.pHoriAngle->connectStart:&despara.pHoriAngle->connectEnd;
			uiBoltMd=max(uiBoltMd,(UINT)pHoriRodConn->d);	//此处连接板在主材上的螺栓取决于斜材和横材
			despara.ciPostAngleBoltNum=(BYTE)max(despara.ciPostAngleBoltNum,ftoi(pHoriRodConn->wnConnBoltN*1.5));
		}
		GetLsSpace(LsSpace,uiBoltMd);
		WORD wiSingleRowSpace=despara.wiPostAngleBoltSpace==0?(WORD)LsSpace.SingleRowSpace:despara.wiPostAngleBoltSpace;
		if(despara.ciPostAngleBoltNum<3)	//此处连接主材螺栓数最少为3个
			despara.ciPostAngleBoltNum=3;
		wiSearchRadius=(WORD)ftoi((despara.ciPostAngleBoltNum+1)*0.5*wiSingleRowSpace+wiSingleRowSpace);
		RetrieveExistBolts(boltset,pCurrLegPostAngle,pPlate->designInfo.origin.xRsltPosition,ciWorkWingX0Y1,wiSearchRadius);
		existbolts.Clear();
		for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
			existbolts.Append(pBolt);
		//更新或添加主材上的螺栓
		layout.ciRows=1;
		layout.ciLayoutStyle=BOLT_LAYOUT::LAYOUT_DUALSIDE;
		layout.cnCount=despara.ciPostAngleBoltNum;
		layout.basepoint.ciPosCalStyle=1;	//杆件节点定位
		layout.ciDatumWing=ciWorkWingX0Y1+'X';
		listLocations.Empty();
		layout.LayoutBolts(g,&listLocations,LsSpace.EndSpace,wiSingleRowSpace,LsSpace.doubleRowSpace);
		//布置螺栓，填充螺栓设计信息
		for(pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
			pLocation->TestCalBoltPosition(pCurrLegPostAngle,despara.pBaseNode);
		mapping.InitWorkPlaneCS(pCurrLegPostAngle,ciWorkWingX0Y1,despara.pBaseNode);
		matchcount=mapping.Mapping(existbolts.pPresetObjs,existbolts.Count,&listLocations);
		pConnAnglePara=pPlate->designInfo.partList.Add(pCurrLegPostAngle->handle);
		pConnAnglePara->m_nClassTypeId=CLS_LINEANGLE;//despara.pBaseHoriAngle->GetClassTypeId();
		pConnAnglePara->angle.bTwoEdge=true;
		pConnAnglePara->iFaceNo=1;
		pConnAnglePara->start0_end1=ciTipS0E1;
		pConnAnglePara->angle.cur_wing_x0_y1=ciWorkWingX0Y1;
		if(ciTipS0E1==0)
		{
			if(pCurrLegPostAngle->startOdd()<wiSearchRadius)
				pCurrLegPostAngle->SetStartOdd(wiSearchRadius);
		}
		else if(ciTipS0E1==1)
		{
			if(pCurrLegPostAngle->endOdd()<wiSearchRadius)
				pCurrLegPostAngle->SetEndOdd(wiSearchRadius);
		}
		for(pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
		{
			if(pLocation->pRelaBolt)
				pBolt=pLocation->pRelaBolt;
			else
				pBolt=(CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
			pBolt->cfgword=pCurrLegPostAngle->cfgword;
			pBolt->SetLayer(pCurrLegPostAngle->layer());
			pBolt->layer(1)='P';
			pBolt->d=uiBoltMd;
			pBolt->des_base_pos.hPart=pCurrLegPostAngle->handle;
			pLocation->WriteParamToBolt(pBolt,despara.pPostAngle,despara.pBaseNode);
			pBolt->AddL0Thick(pCurrLegPostAngle->handle,TRUE,TRUE);
			pBolt->CalGuigeAuto();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pPlate->AppendLsRef(pBolt->GetLsRef());
			despara.pPostAngle->AppendLsRef(pBolt->GetLsRef());
		}
		if(pLegBraceAngle==NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"%d号呼高#%C腿主材腿身连接处缺少腿部辅材",despara.ciHeightSerial,'@'+pHeight->xarrSubLegs[j].Id);
		else //if(pLegBraceAngle!=NULL)
		{	//腿部辅材
			//添加钢板连接杆件信息
			despara.JudgeConnectTipAndWing(pLegBraceAngle, &ciTipS0E1, &ciWorkWingX0Y1);
			pConnAnglePara = pPlate->designInfo.partList.Add(pLegBraceAngle->handle);
			pConnAnglePara->m_nClassTypeId = CLS_LINEANGLE;//pVLegAngle->GetClassTypeId();
			pConnAnglePara->angle.bTwoEdge = false;
			pConnAnglePara->iFaceNo = 1;
			despara.JudgeConnectTipAndWing(pLegBraceAngle, &ciTipS0E1, &ciWorkWingX0Y1);
			pConnAnglePara->angle.cur_wing_x0_y1 = ciWorkWingX0Y1;
			pConnAnglePara->start0_end1 = ciTipS0E1;
			GEPOINT vWorkNormal = ciWorkWingX0Y1 == 0 ? pLegBraceAngle->vxWingNorm : pLegBraceAngle->vyWingNorm;
			if (ciTipS0E1 == 0)
			{
				pLegBraceAngle->desStartOdd.m_iOddCalStyle = 0;
				pLegBraceAngle->desStartOdd.m_hRefPart1 = pCurrLegPostAngle->handle;
				pLegBraceAngle->desStartOdd.m_hRefPart2 = 0;
				if (vWorkNormal*despara.vPlaneNormal < 0)
				{
					if (ciWorkWingX0Y1 == 0)
						pLegBraceAngle->desStartPos.wing_x_offset.SetOffset(-pPlate->Thick);
					else
						pLegBraceAngle->desStartPos.wing_y_offset.SetOffset(-pPlate->Thick);
				}
				pLegBraceAngle->CalStartOddment();
			}
			else if (ciTipS0E1 == 1)
			{
				pLegBraceAngle->desEndOdd.m_iOddCalStyle = 0;
				pLegBraceAngle->desEndOdd.m_hRefPart1 = pCurrLegPostAngle->handle;
				pLegBraceAngle->desEndOdd.m_hRefPart2 = 0;
				pLegBraceAngle->CalEndOddment();
				if (vWorkNormal*despara.vPlaneNormal < 0)
				{
					if (ciWorkWingX0Y1 == 0)
						pLegBraceAngle->desEndPos.wing_x_offset.SetOffset(-pPlate->Thick);
					else
						pLegBraceAngle->desEndPos.wing_y_offset.SetOffset(-pPlate->Thick);
				}
			}

			jgzj = pLegBraceAngle->GetZhunJu(ciWorkWingX0Y1 + 'X');
			g[0] = jgzj.g;
			g[1] = jgzj.g1;
			g[2] = jgzj.g2;
			g[3] = jgzj.g3;
			layout.ciLayoutStyle = 0;
			if (ciTipS0E1 == 0)
				pCurrConnInfo = &pLegBraceAngle->connectStart;
			else
				pCurrConnInfo = &pLegBraceAngle->connectEnd;
			int nBoltNum = max(2, pCurrConnInfo->wnConnBoltN);
			uiBoltMd = ciTipS0E1 == 0 ? pLegBraceAngle->connectStart.d : pLegBraceAngle->connectEnd.d;
			if (LsSpace.M_Diameter != uiBoltMd)
				GetLsSpace(LsSpace, uiBoltMd);
			layout.cnCount = max(1, (BYTE)pCurrConnInfo->wnConnBoltN);
			layout.ciRows = 0;
			layout.ciLayoutStyle = BOLT_LAYOUT::LAYOUT_ONESIDE;
			layout.ciFlowDirection = (ciTipS0E1 == 0) ? BOLT_LAYOUT::FLOW_StoE : BOLT_LAYOUT::FLOW_EtoS;
			layout.ciDatumWing = ciWorkWingX0Y1 + 'X';
			layout.basepoint.ciPosCalStyle = 0;
			listLocations.Empty();
			layout.LayoutBolts(g, &listLocations, LsSpace.EndSpace, LsSpace.SingleRowSpace, LsSpace.doubleRowSpace);
			//提取相应位置的已布螺栓集合
			wiSearchRadius = (WORD)(nBoltNum*LsSpace.SingleRowSpace);
			boltset.Empty();
			existbolts.ReSize(0);
			RetrieveExistBolts(boltset, pLegBraceAngle, despara.pBaseNode, ciWorkWingX0Y1, wiSearchRadius);
			for (pBolt = boltset.GetFirst(); pBolt; pBolt = boltset.GetNext())
				existbolts.Append(pBolt);

			for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
				pLocation->TestCalBoltPosition(pLegBraceAngle, despara.pBaseNode);
			mapping.ClearMapping();
			mapping.InitWorkPlaneCS(pLegBraceAngle, ciWorkWingX0Y1, despara.pBaseNode);
			matchcount = mapping.Mapping(existbolts.pPresetObjs, existbolts.Count, &listLocations);

			for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
			{
				if (pLocation->pRelaBolt)
					pBolt = pLocation->pRelaBolt;
				else
				{
					pBolt = (CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
					pBolt->SetLayer(pLegBraceAngle->layer());
					pBolt->layer(1) = 'P';
					pBolt->cfgword = pLegBraceAngle->cfgword;
				}
				pBolt->d = uiBoltMd;
				pBolt->des_base_pos.hPart = pLegBraceAngle->handle;
				pLocation->WriteParamToBolt(pBolt, pLegBraceAngle, despara.pBaseNode);
				pBolt->AddL0Thick(pLegBraceAngle->handle, TRUE, TRUE);
				pBolt->CalGuigeAuto();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pLegBraceAngle->AppendLsRef(pBolt->GetLsRef());
				pPlate->AppendLsRef(pBolt->GetLsRef());
			}
		}
		for (pBolt = shareboltset.GetFirst(); pBolt; pBolt = shareboltset.GetNext())
			pPlate->AppendLsRef(pBolt->GetLsRef());
		pPlate->DesignPlate();
		RELATIVE_OBJECT* pMirRelaObj;
		CLDSPlate* pMirPlate=NULL;
		if(MirTaAtom(pPlate, MIRMSG(1), TRUE))
		{
			if(pMirRelaObj=pPlate->GetMirRelaObj(MIRMSG(1)))
				pMirPlate=(CLDSPlate*)Ta.Parts.FromHandle(pMirRelaObj->hObj,CLS_PLATE);
			CDesignLjPartPara* pBaseLj=pMirPlate==NULL?NULL:pMirPlate->designInfo.partList.GetValue(pMirPlate->designInfo.m_hBasePart);
			if(pBaseLj)
			{
				CXhChar50 key=CUnifyModelLayer::PLATE::BuildKey(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,pBaseLj->angle.cur_wing_x0_y1);
				CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,'X'+pBaseLj->angle.cur_wing_x0_y1);
				pRegister->hPlate=pMirPlate->handle;
			}
		}
		if(MirTaAtom(pPlate, MIRMSG(2), TRUE))
		{
			if(pMirRelaObj=pPlate->GetMirRelaObj(MIRMSG(2)))
				pMirPlate=(CLDSPlate*)Ta.Parts.FromHandle(pMirRelaObj->hObj,CLS_PLATE);
			CDesignLjPartPara* pBaseLj=pMirPlate==NULL?NULL:pMirPlate->designInfo.partList.GetValue(pMirPlate->designInfo.m_hBasePart);
			if(pBaseLj)
			{
				CXhChar50 key=CUnifyModelLayer::PLATE::BuildKey(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,pBaseLj->angle.cur_wing_x0_y1);
				CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,'X'+pBaseLj->angle.cur_wing_x0_y1);
				pRegister->hPlate=pMirPlate->handle;
			}
		}
		if(MirTaAtom(pPlate, MIRMSG(3), TRUE))
		{
			if(pMirRelaObj=pPlate->GetMirRelaObj(MIRMSG(3)))
				pMirPlate=(CLDSPlate*)Ta.Parts.FromHandle(pMirRelaObj->hObj,CLS_PLATE);
			CDesignLjPartPara* pBaseLj=pMirPlate==NULL?NULL:pMirPlate->designInfo.partList.GetValue(pMirPlate->designInfo.m_hBasePart);
			if(pBaseLj)
			{
				CXhChar50 key=CUnifyModelLayer::PLATE::BuildKey(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,pBaseLj->angle.cur_wing_x0_y1);
				CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,'X'+pBaseLj->angle.cur_wing_x0_y1);
				pRegister->hPlate=pMirPlate->handle;
			}
		}
	}
	return true;
}
bool CTowerMultiLegDesigner::CorrectTrunkPostKNodeConnPlate(
	TRUNK_POST_KJOINT_CONNPLATE& despara, ROD_PTR* xarrDownRods,UINT count/*=1*/,bool bMirCreate/* = true*/)
{
	CLDSLineAngle* pAngle = NULL;
	//计算当前工作面法线
	GEPOINT vLenStdVec = despara.pPostAngle->xPosEnd - despara.pPostAngle->xPosStart;
	normalize(vLenStdVec);
	if (despara.pPostAngle->pStart == despara.pBaseNode)
	{
		despara.pPostAngle->desStartOdd.m_iOddCalStyle = 2;
		despara.pPostAngle->SetStartOdd(300);	//下侧接身主材暂指定正负头值，以便于斜材负头计算
	}
	else if(despara.pPostAngle->pEnd == despara.pBaseNode)
	{
		despara.pPostAngle->desEndOdd.m_iOddCalStyle = 2;
		despara.pPostAngle->SetEndOdd(300);	//下侧接身主材暂指定正负头值，以便于斜材负头计算
	}
	if (despara.pUpperPostAngle== NULL)
	{
		logerr.Log("0x%X节点的上侧连接材主材丢失",despara.pBaseNode->handle);
		return false;
	}
#ifdef __TMP_LOG_PROC_
	else
		logerr.Log("上材主材0x%X", despara.pUpperPostAngle->handle);
	logerr.Log("*******************************DownRodsN=%d", count);
#endif
	if (vLenStdVec.z < 0)
		vLenStdVec*=-1.0;
	if (despara.ciFront0OrSide1 == 0)
		despara.vPlaneNormal.Set(0, vLenStdVec.z, -vLenStdVec.y);
	else //if(despara.ciFront0OrSide1==1)
		despara.vPlaneNormal.Set(vLenStdVec.z, 0, -vLenStdVec.x);
	//
	CDesignLjPara designinfo;
	designinfo.m_bEnableFlexibleDesign = true;
	designinfo.m_hBaseNode = despara.pBaseNode->handle;
	BYTE ciTipS0E1 = 2, ciWorkWingX0Y1 = 0;
	CLDSBolt* pBolt;
	CConnectInfo* pCurrConnInfo = NULL;
	CDesignLjPartPara* pConnAnglePara;
	BOLTSET boltset, shareboltset;
	ARRAY_LIST<CLDSBolt*> existbolts;
	JGZJ jgzj;
	LSSPACE_STRU LsSpace;
	WORD wiSearchRadius, wiSingleRowSpace = despara.wiPostAngleBoltSpace;//nBoltSpace;
	BOLT_LOCATION* pLocation;
	CXhSimpleList<BOLT_LOCATION> listLocations;
	BOLT_LAYOUT layout;
	UINT uiBoltMd;
	char ciConnWing = 0;//=designinfo.norm.norm_wing==0?'X':'Y';
	long g[4];//={jgzj.g,jgzj.g1,jgzj.g2,jgzj.g3};
	CMappingBolts mapping;
	int nActualPlateThick = 6;//
	if (despara.pUpXieAngle)	//有时存在腿身连接处没有塔身连接斜材和腿部辅材情况
		nActualPlateThick = max(nActualPlateThick, (int)despara.pUpXieAngle->size_thick);
	if (despara.pHoriAngle)
		nActualPlateThick = max(nActualPlateThick, (int)despara.pHoriAngle->size_thick);
	if (despara.pDownXieAngle)	//有时存在腿身连接处没有塔身连接斜材和腿部辅材情况
		nActualPlateThick = max(nActualPlateThick, (int)despara.pDownXieAngle->size_thick);
	if (nActualPlateThick % 2)
		nActualPlateThick++;
	nActualPlateThick+=2;//塔身主材上K板厚度比射线材大一级
	//1.布置塔身K点上斜材及横材上的螺栓
	int sideI = 0, matchcount = 0;
	for (sideI = 0; sideI < 2; sideI++)
	{
		CLDSLineAngle* pUpperAngle = sideI == 0 ? despara.pUpXieAngle : despara.pHoriAngle;
		if (pUpperAngle == NULL)
			continue;
		despara.JudgeConnectTipAndWing(pUpperAngle, &ciTipS0E1, &ciWorkWingX0Y1);
		//添加钢板连接杆件信息
		pConnAnglePara = designinfo.partList.Add(pUpperAngle->handle);
		pConnAnglePara->angle.bTwoEdge = false;
		pConnAnglePara->angle.cur_wing_x0_y1 = ciWorkWingX0Y1;
		pConnAnglePara->iFaceNo = 1;
		pConnAnglePara->start0_end1 = ciTipS0E1;
		pConnAnglePara->m_nClassTypeId = CLS_LINEANGLE;//despara.pLeftUpperAngle->GetClassTypeId();
		GEPOINT vWorkNormal = ciWorkWingX0Y1 == 0 ? pUpperAngle->vxWingNorm : pUpperAngle->vyWingNorm;
		if (ciTipS0E1 == 0)
		{
			pUpperAngle->desStartOdd.m_iOddCalStyle = 0;
			pUpperAngle->desStartOdd.m_hRefPart1 = despara.pPostAngle->handle;
			pUpperAngle->desStartOdd.m_hRefPart2 = (despara.pHoriAngle != NULL && sideI == 0) ? despara.pHoriAngle->handle : 0;
			if (vWorkNormal*despara.vPlaneNormal < 0)
			{
				if (ciWorkWingX0Y1 == 0)
					pUpperAngle->desStartPos.wing_x_offset.SetOffset(-nActualPlateThick);
				else
					pUpperAngle->desStartPos.wing_y_offset.SetOffset(-nActualPlateThick);
			}
			pUpperAngle->CalStartOddment();
		}
		else if (ciTipS0E1 == 1)
		{
			pUpperAngle->desEndOdd.m_iOddCalStyle = 0;
			pUpperAngle->desEndOdd.m_hRefPart1 = despara.pPostAngle->handle;
			pUpperAngle->desEndOdd.m_hRefPart2 = (despara.pHoriAngle!=NULL&&sideI == 0) ? despara.pHoriAngle->handle : 0;
			if (vWorkNormal*despara.vPlaneNormal < 0)
			{
				if (ciWorkWingX0Y1 == 0)
					pUpperAngle->desEndPos.wing_x_offset.SetOffset(-nActualPlateThick);
				else
					pUpperAngle->desEndPos.wing_y_offset.SetOffset(-nActualPlateThick);
			}
			pUpperAngle->CalEndOddment();
		}

		jgzj = pUpperAngle->GetZhunJu(ciWorkWingX0Y1 + 'X');
		g[0] = jgzj.g;
		g[1] = jgzj.g1;
		g[2] = jgzj.g2;
		g[3] = jgzj.g3;
		listLocations.Empty();
		layout.ciLayoutStyle = 0;
		if (ciTipS0E1 == 0)
			pCurrConnInfo = &pUpperAngle->connectStart;
		else
			pCurrConnInfo = &pUpperAngle->connectEnd;
		int nBoltNum = max(2, pCurrConnInfo->wnConnBoltN);
		uiBoltMd = ciTipS0E1 == 0 ? pUpperAngle->connectStart.d : pUpperAngle->connectEnd.d;
		GetLsSpace(LsSpace, uiBoltMd);
		layout.basepoint.ciPosCalStyle = 0;
		layout.cnCount = max(2, (BYTE)pCurrConnInfo->wnConnBoltN);
		layout.ciRows = 0;
		layout.cnMaxSingularLineBolts=4;
		layout.ciLayoutStyle = 0;
		layout.ciFlowDirection = (ciTipS0E1 == 0) ? BOLT_LAYOUT::FLOW_StoE : BOLT_LAYOUT::FLOW_EtoS;
		layout.ciDatumWing = ciWorkWingX0Y1 + 'X';
		listLocations.Empty();
		layout.LayoutBolts(g, &listLocations, LsSpace.EndSpace, LsSpace.SingleRowSpace, LsSpace.doubleRowSpace);
		//提取相应位置的已布螺栓集合
		existbolts.SetSize(0,16);
		wiSearchRadius = (WORD)(nBoltNum*LsSpace.SingleRowSpace);
		boltset.Empty();
		RetrieveExistBolts(boltset, pUpperAngle, despara.pBaseNode, ciWorkWingX0Y1, wiSearchRadius);
		for (pBolt = boltset.GetFirst(); pBolt; pBolt = boltset.GetNext())
		{
			shareboltset.append(pBolt);
			existbolts.append(pBolt);
		}
		//布置螺栓，填充螺栓设计信息
		for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
			pLocation->TestCalBoltPosition(pUpperAngle, despara.pBaseNode);
		mapping.ClearMapping();
		mapping.InitWorkPlaneCS(pUpperAngle, ciWorkWingX0Y1, despara.pBaseNode);
		matchcount = mapping.Mapping(existbolts.m_pData, existbolts.GetSize(), &listLocations,5);
		for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
		{
			if (pLocation->pRelaBolt)
				pBolt = pLocation->pRelaBolt;
			else
			{
				pBolt = (CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
				pBolt->SetLayer("SP*");
				pBolt->layer(2) = pUpperAngle->Layer(2);
				pBolt->cfgword = pUpperAngle->cfgword;
				pBolt->d = uiBoltMd;
				pBolt->des_base_pos.hPart = pUpperAngle->handle;
				pLocation->WriteParamToBolt(pBolt, pUpperAngle, despara.pBaseNode);
				pBolt->AddL0Thick(pUpperAngle->handle, TRUE, TRUE);
				pBolt->CalGuigeAuto();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				shareboltset.append(pBolt);
				pUpperAngle->AppendLsRef(pBolt->GetLsRef());
			}
		}
	}
	//2.设计接身切换主材K点连接板
	SmartRodPtr pConnRod;
	SmartRodPtr pDownRod;
	for (UINT j = 0; j < count; j++)
	{
		pDownRod=xarrDownRods[j];
		if (!pDownRod.blValid)
			break;
		CLDSLineAngle* pDownXieAngle = NULL;
		CLDSLineAngle* pCurrPostAngle = despara.pPostAngle=pDownRod.pAngle;
		if (despara.pPostAngle == NULL)
		{
			logerr.Log("0x%X节点的下侧材连接主材丢失",despara.pBaseNode->handle);
			return false;
		}
#ifdef __TMP_LOG_PROC_
		else
			logerr.Log("0x%X节点的下侧材连接主材0x%X", despara.pPostAngle->handle);
#endif
		despara.RetrieveCurrHeightKJointDownXieRod(&Ta);
#ifdef __TMP_LOG_PROC_
		logerr.Log("\tRetrieveCurrHeightKJointDownXieRod");
#endif
		despara.JudgeConnectTipAndWing(pCurrPostAngle, &ciTipS0E1, &ciWorkWingX0Y1);
		if (ciTipS0E1==0)
		{
			pCurrPostAngle->desStartOdd.m_iOddCalStyle = 2;
			pCurrPostAngle->SetStartOdd(300);	//下侧接身主材暂指定正负头值，以便于斜材负头计算
		}
		else if (ciTipS0E1==1)
		{
			pCurrPostAngle->desEndOdd.m_iOddCalStyle = 2;
			pCurrPostAngle->SetEndOdd(300);	//下侧接身主材暂指定正负头值，以便于斜材负头计算
		}
		//查找该节点当前面是否已设计连接板
		CLDSPlate* pPlate = NULL, *pExistPlate = NULL;
		for (pPlate = (CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE); pPlate; pPlate = (CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
		{
			if (pPlate->m_hBaseNode != despara.pBaseNode->handle || pPlate->m_hBaseRod != pCurrPostAngle->handle)
				continue;
			if (!pCurrPostAngle->ModuleTogetherWith(pPlate))
				continue;	//不属于当前接腿
			if (pPlate->designInfo.norm.hVicePart != pCurrPostAngle->handle || fabs(pPlate->ucs.axis_z*despara.vPlaneNormal) < 0.9)
				continue;
			if (pPlate->wiType != CLDSPlate::CONNECT_PLATE)
				continue;
			pExistPlate = pPlate;
			break;
		}
#ifdef __TMP_LOG_PROC_
		logerr.Log("\tExistPlate=0x%X", pExistPlate!=NULL? pExistPlate->handle:0);
#endif
		//搜索是否已存在相应的角脊线腿身连接板
		if (pExistPlate != NULL)
		{
			pPlate = pExistPlate;
			pPlate->designInfo.m_bEnableFlexibleDesign = true;
			pPlate->designInfo.m_hBaseNode = despara.pBaseNode->handle;
			pPlate->designInfo.m_hBasePart = pCurrPostAngle->handle;
			continue;
		}
		else
		{
			CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(
				despara.pBaseNode->handle, pCurrPostAngle->handle, 'X' + ciWorkWingX0Y1);
			if (pRegister->hPlate > 0x20)
				pPlate = (CLDSPlate*)Ta.Parts.FromHandle(pRegister->hPlate, CLS_PLATE);
			else
			{
				pPlate = (CLDSPlate*)Ta.Parts.append(CLS_PLATE);
				pRegister->hPlate = pPlate->handle;
			}
			if (despara.ciFront0OrSide1 == 0)
				pPlate->SetLayer("SPQ");
			else if (despara.ciFront0OrSide1 == 1)
				pPlate->SetLayer("SPY");
			pPlate->cfgword = pCurrPostAngle->cfgword;
			designinfo.CloneTo(pPlate->designInfo);
			pPlate->designInfo.m_bEnableFlexibleDesign = true;
		}
		pPlate->designInfo.m_hBaseNode = despara.pBaseNode->handle;
		pPlate->designInfo.m_hBasePart = pCurrPostAngle->handle;
		//设定连接板基点参数
		pPlate->designInfo.origin.datum_pos_style = 2;
		pPlate->designInfo.origin.des_para.RODNODE.hNode = despara.pBaseNode->handle;
		pPlate->designInfo.origin.des_para.RODNODE.hRod = pCurrPostAngle->handle;
		pPlate->designInfo.origin.des_para.RODNODE.offset_wing = ciWorkWingX0Y1;
		pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style = 0;
		pPlate->designInfo.origin.UpdatePos(this->m_pTower);
		//设定连接板法线参数
		pPlate->designInfo.norm.norm_style = 1;
		pPlate->designInfo.norm.hVicePart = pCurrPostAngle->handle;
		pPlate->designInfo.norm.norm_wing = ciWorkWingX0Y1;
		pPlate->designInfo.norm.direction = 0;	//由里朝外
		ciTipS0E1 = 2;
		//布置基准主材上的螺栓
		jgzj = pCurrPostAngle->GetZhunJu(ciWorkWingX0Y1 + 'X');
		g[0] = jgzj.g; g[1] = jgzj.g1;
		g[2] = jgzj.g2; g[3] = jgzj.g3;
		uiBoltMd = 16;
		CConnectInfo *pXieRodConn = NULL, *pHoriRodConn = NULL;
		//despara.ciPostAngleBoltNum = 3;
#ifdef __TMP_LOG_PROC_
		logerr.Log("\tpUpXieAngle=0x%X", despara.pUpXieAngle != NULL ? despara.pUpXieAngle->handle : 0);
#endif
		if (despara.pUpXieAngle)
		{
			pXieRodConn = despara.pUpXieAngle->pStart == despara.pBaseNode ? &despara.pUpXieAngle->connectStart : &despara.pUpXieAngle->connectEnd;
			uiBoltMd = max(uiBoltMd, (UINT)pXieRodConn->d);	//此处连接板在主材上的螺栓取决于斜材和横材
			despara.ciPostAngleBoltNum = (BYTE)max(despara.ciPostAngleBoltNum, ftoi(pXieRodConn->wnConnBoltN*1.5));
		}
#ifdef __TMP_LOG_PROC_
		logerr.Log("\tpHoriAngle=0x%X", despara.pHoriAngle != NULL ? despara.pHoriAngle->handle : 0);
#endif
		if (despara.pHoriAngle)
		{
			pHoriRodConn = despara.pHoriAngle->pStart == despara.pBaseNode ? &despara.pHoriAngle->connectStart : &despara.pHoriAngle->connectEnd;
			uiBoltMd = max(uiBoltMd, (UINT)pHoriRodConn->d);	//此处连接板在主材上的螺栓取决于斜材和横材
			despara.ciPostAngleBoltNum = (BYTE)max(despara.ciPostAngleBoltNum, ftoi(pHoriRodConn->wnConnBoltN*1.5));
		}
		//if (despara.pDownXieAngle)
		//{
		//	pXieRodConn = despara.pDownXieAngle->pStart == despara.pBaseNode ? &despara.pDownXieAngle->connectStart : &despara.pDownXieAngle->connectEnd;
		//	uiBoltMd = max(uiBoltMd, (UINT)pXieRodConn->d);	//此处连接板在主材上的螺栓取决于斜材和横材
		//	despara.ciPostAngleBoltNum = (BYTE)max(despara.ciPostAngleBoltNum, ftoi(pXieRodConn->wnConnBoltN*1.5));
		//}
		GetLsSpace(LsSpace, uiBoltMd);
		WORD wiSingleRowSpace = despara.wiPostAngleBoltSpace == 0 ? (WORD)LsSpace.SingleRowSpace : despara.wiPostAngleBoltSpace;
		if (despara.ciPostAngleBoltNum < 3)	//此处连接主材螺栓数最少为3个
			despara.ciPostAngleBoltNum = 3;
		wiSearchRadius = (WORD)ftoi((despara.ciPostAngleBoltNum + 1)*0.5*wiSingleRowSpace + wiSingleRowSpace);
#ifdef __TMP_LOG_PROC_
		logerr.Log("\tRetrieveExistBolts=0x%X,0x%X", pCurrPostAngle->handle,pPlate->handle);
#endif
		RetrieveExistBolts(boltset, pCurrPostAngle, pPlate->designInfo.origin.xRsltPosition, ciWorkWingX0Y1, wiSearchRadius);
#ifdef __TMP_LOG_PROC_
		logerr.Log("\tRetrieveExistBolts successed");
#endif
		existbolts.Empty();
		for (pBolt = boltset.GetFirst(); pBolt; pBolt = boltset.GetNext())
			existbolts.append(pBolt);
		//更新或添加主材上的螺栓
		layout.ciRows = 1;
		layout.ciLayoutStyle = BOLT_LAYOUT::LAYOUT_DUALSIDE;
		layout.cnCount = despara.ciPostAngleBoltNum;
		layout.basepoint.ciPosCalStyle = 1;	//杆件节点定位
		layout.ciDatumWing = ciWorkWingX0Y1 + 'X';
		listLocations.Empty();
		layout.LayoutBolts(g, &listLocations, LsSpace.EndSpace, wiSingleRowSpace, LsSpace.doubleRowSpace);
		//布置螺栓，填充螺栓设计信息
		for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
			pLocation->TestCalBoltPosition(pCurrPostAngle, despara.pBaseNode);
#ifdef __TMP_LOG_PROC_
		logerr.Log("\t mapping.InitWorkPlaneCS=0x%X,0x%X", pCurrPostAngle->handle, despara.pBaseNode->handle);
#endif
		mapping.InitWorkPlaneCS(pCurrPostAngle, ciWorkWingX0Y1, despara.pBaseNode);
#ifdef __TMP_LOG_PROC_
		logerr.Log("\t mapping.Mapping boltN=%d", existbolts.GetSize());
#endif
		matchcount = mapping.Mapping(existbolts.m_pData, existbolts.GetSize(), &listLocations);
#ifdef __TMP_LOG_PROC_
		logerr.Log("\t mapping.Mapping successed");
#endif
		pConnAnglePara = pPlate->designInfo.partList.Add(pCurrPostAngle->handle);
		pConnAnglePara->m_nClassTypeId = CLS_LINEANGLE;//despara.pBaseHoriAngle->GetClassTypeId();
		pConnAnglePara->angle.bTwoEdge = true;
		pConnAnglePara->iFaceNo = 1;
		pConnAnglePara->start0_end1 = ciTipS0E1;
		pConnAnglePara->angle.cur_wing_x0_y1 = ciWorkWingX0Y1;
#ifdef __TMP_LOG_PROC_
		logerr.Log("\t布置螺栓，填充螺栓设计信息");
#endif
		if (ciTipS0E1 == 0)
		{
			if (pCurrPostAngle->startOdd() < wiSearchRadius)
				pCurrPostAngle->SetStartOdd(wiSearchRadius);
		}
		else if (ciTipS0E1 == 1)
		{
			if (pCurrPostAngle->endOdd() < wiSearchRadius)
				pCurrPostAngle->SetEndOdd(wiSearchRadius);
		}
		for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
		{
			if (pLocation->pRelaBolt)
				pBolt = pLocation->pRelaBolt;
			else
				pBolt = (CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
			pBolt->cfgword = pCurrPostAngle->cfgword;
			pBolt->SetLayer(pCurrPostAngle->layer());
			pBolt->layer(1) = 'P';
			pBolt->d = uiBoltMd;
			pBolt->des_base_pos.hPart = pCurrPostAngle->handle;
			pLocation->WriteParamToBolt(pBolt, pCurrPostAngle, despara.pBaseNode);
			pBolt->AddL0Thick(pCurrPostAngle->handle, TRUE, TRUE);
			pBolt->CalGuigeAuto();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pPlate->AppendLsRef(pBolt->GetLsRef());
			pCurrPostAngle->AppendLsRef(pBolt->GetLsRef());
		}
#ifdef __TMP_LOG_PROC_
		logerr.Log("\tpLocation");
#endif
		if (despara.pDownXieAngle == NULL)
			;// logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "%d号呼高#%C腿主材腿身连接处缺少腿部辅材", despara.ciHeightSerial, '@' + pHeight->xarrSubLegs[j].Id);
		else //if(despara.pDownXieAngle!=NULL)
		{	//下侧接身斜材
			//添加钢板连接杆件信息
#ifdef __TMP_LOG_PROC_
			logerr.Log("\t despara.pDownXieAngle");
#endif
			despara.JudgeConnectTipAndWing(despara.pDownXieAngle, &ciTipS0E1, &ciWorkWingX0Y1);
			pConnAnglePara = pPlate->designInfo.partList.Add(despara.pDownXieAngle->handle);
			pConnAnglePara->m_nClassTypeId = CLS_LINEANGLE;//pVLegAngle->GetClassTypeId();
			pConnAnglePara->angle.bTwoEdge = false;
			pConnAnglePara->iFaceNo = 1;
			despara.JudgeConnectTipAndWing(despara.pDownXieAngle, &ciTipS0E1, &ciWorkWingX0Y1);
			pConnAnglePara->angle.cur_wing_x0_y1 = ciWorkWingX0Y1;
			pConnAnglePara->start0_end1 = ciTipS0E1;
			GEPOINT vWorkNormal = ciWorkWingX0Y1 == 0 ? despara.pDownXieAngle->vxWingNorm : despara.pDownXieAngle->vyWingNorm;
			if (ciTipS0E1 == 0)
			{
				despara.pDownXieAngle->desStartOdd.m_iOddCalStyle = 0;
				despara.pDownXieAngle->desStartOdd.m_hRefPart1 = pCurrPostAngle->handle;
				despara.pDownXieAngle->desStartOdd.m_hRefPart2 = 0;
				if (vWorkNormal*despara.vPlaneNormal < 0)
				{
					if (ciWorkWingX0Y1 == 0)
						despara.pDownXieAngle->desStartPos.wing_x_offset.SetOffset(-pPlate->Thick);
					else
						despara.pDownXieAngle->desStartPos.wing_y_offset.SetOffset(-pPlate->Thick);
				}
				despara.pDownXieAngle->CalStartOddment();
			}
			else if (ciTipS0E1 == 1)
			{
				despara.pDownXieAngle->desEndOdd.m_iOddCalStyle = 0;
				despara.pDownXieAngle->desEndOdd.m_hRefPart1 = pCurrPostAngle->handle;
				despara.pDownXieAngle->desEndOdd.m_hRefPart2 = 0;
				despara.pDownXieAngle->CalEndOddment();
				if (vWorkNormal*despara.vPlaneNormal < 0)
				{
					if (ciWorkWingX0Y1 == 0)
						despara.pDownXieAngle->desEndPos.wing_x_offset.SetOffset(-pPlate->Thick);
					else
						despara.pDownXieAngle->desEndPos.wing_y_offset.SetOffset(-pPlate->Thick);
				}
			}

			jgzj = despara.pDownXieAngle->GetZhunJu(ciWorkWingX0Y1 + 'X');
			g[0] = jgzj.g;
			g[1] = jgzj.g1;
			g[2] = jgzj.g2;
			g[3] = jgzj.g3;
			layout.ciLayoutStyle = 0;
			if (ciTipS0E1 == 0)
				pCurrConnInfo = &despara.pDownXieAngle->connectStart;
			else
				pCurrConnInfo = &despara.pDownXieAngle->connectEnd;
			int nBoltNum = max(2, pCurrConnInfo->wnConnBoltN);
			uiBoltMd = ciTipS0E1 == 0 ? despara.pDownXieAngle->connectStart.d : despara.pDownXieAngle->connectEnd.d;
			if (LsSpace.M_Diameter != uiBoltMd)
				GetLsSpace(LsSpace, uiBoltMd);
			layout.cnCount = max(1, (BYTE)pCurrConnInfo->wnConnBoltN);
			layout.ciRows = 0;
			layout.ciLayoutStyle = BOLT_LAYOUT::LAYOUT_ONESIDE;
			layout.ciFlowDirection = (ciTipS0E1 == 0) ? BOLT_LAYOUT::FLOW_StoE : BOLT_LAYOUT::FLOW_EtoS;
			layout.ciDatumWing = ciWorkWingX0Y1 + 'X';
			layout.basepoint.ciPosCalStyle = 0;
			listLocations.Empty();
			layout.LayoutBolts(g, &listLocations, LsSpace.EndSpace, LsSpace.SingleRowSpace, LsSpace.doubleRowSpace);
#ifdef __TMP_LOG_PROC_
			logerr.Log("\t layout.LayoutBolts despara.pDownXieAngle");
#endif
			//提取相应位置的已布螺栓集合
			wiSearchRadius = (WORD)(nBoltNum*LsSpace.SingleRowSpace);
			boltset.Empty();
			existbolts.SetSize(0,16);
			RetrieveExistBolts(boltset, despara.pDownXieAngle, despara.pBaseNode, ciWorkWingX0Y1, wiSearchRadius);
			for (pBolt = boltset.GetFirst(); pBolt; pBolt = boltset.GetNext())
				existbolts.append(pBolt);

			for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
				pLocation->TestCalBoltPosition(despara.pDownXieAngle, despara.pBaseNode);
			mapping.ClearMapping();
			mapping.InitWorkPlaneCS(despara.pDownXieAngle, ciWorkWingX0Y1, despara.pBaseNode);
			matchcount = mapping.Mapping(existbolts.m_pData, existbolts.GetSize(), &listLocations);

			for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
			{
				if (pLocation->pRelaBolt)
					pBolt = pLocation->pRelaBolt;
				else
				{
					pBolt = (CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
					pBolt->SetLayer(despara.pDownXieAngle->layer());
					pBolt->layer(1) = 'P';
					pBolt->cfgword = despara.pDownXieAngle->cfgword;
				}
				pBolt->d = uiBoltMd;
				pBolt->des_base_pos.hPart = despara.pDownXieAngle->handle;
				pLocation->WriteParamToBolt(pBolt, despara.pDownXieAngle, despara.pBaseNode);
				pBolt->AddL0Thick(despara.pDownXieAngle->handle, TRUE, TRUE);
				pBolt->CalGuigeAuto();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				despara.pDownXieAngle->AppendLsRef(pBolt->GetLsRef());
				pPlate->AppendLsRef(pBolt->GetLsRef());
			}
#ifdef __TMP_LOG_PROC_
			logerr.Log("\t layout.LayoutBolts despara.pDownXieAngle  success");
#endif
		}
		for (pBolt = shareboltset.GetFirst(); pBolt; pBolt = shareboltset.GetNext())
			pPlate->AppendLsRef(pBolt->GetLsRef());
		pPlate->DesignPlate();
#ifdef __TMP_LOG_PROC_
		logerr.Log("\t pPlate->DesignPlate");
#endif
		RELATIVE_OBJECT* pMirRelaObj;
		CLDSPlate* pMirPlate=NULL;
		if(MirTaAtom(pPlate, MIRMSG(1), TRUE))
		{
			for(pMirRelaObj=pPlate->relativeObjs.GetFirst();pMirRelaObj;pMirRelaObj=pPlate->relativeObjs.GetNext())
			{
				if(pMirRelaObj->mirInfo.IsSameMirMsg(MIRMSG(1)))
				{
					pMirPlate=(CLDSPlate*)Ta.Parts.FromHandle(pMirRelaObj->hObj,CLS_PLATE);
					break;
				}
			}
			CDesignLjPartPara* pBaseLj=pMirPlate==NULL?NULL:pMirPlate->designInfo.partList.GetValue(pMirPlate->designInfo.m_hBasePart);
			if(pBaseLj)
			{
				CXhChar50 key=CUnifyModelLayer::PLATE::BuildKey(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,pBaseLj->angle.cur_wing_x0_y1);
				CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,'X'+pBaseLj->angle.cur_wing_x0_y1);
				pRegister->hPlate=pMirPlate->handle;
			}
		}
#ifdef __TMP_LOG_PROC_
		logerr.Log("\t MirTaAtom(1)");
#endif
		if(MirTaAtom(pPlate, MIRMSG(2), TRUE))
		{
			for(pMirRelaObj=pPlate->relativeObjs.GetFirst();pMirRelaObj;pMirRelaObj=pPlate->relativeObjs.GetNext())
			{
				if(pMirRelaObj->mirInfo.IsSameMirMsg(MIRMSG(2)))
				{
					pMirPlate=(CLDSPlate*)Ta.Parts.FromHandle(pMirRelaObj->hObj,CLS_PLATE);
					break;
				}
			}
			CDesignLjPartPara* pBaseLj=pMirPlate==NULL?NULL:pMirPlate->designInfo.partList.GetValue(pMirPlate->designInfo.m_hBasePart);
			if(pBaseLj)
			{
				CXhChar50 key=CUnifyModelLayer::PLATE::BuildKey(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,pBaseLj->angle.cur_wing_x0_y1);
				CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,'X'+pBaseLj->angle.cur_wing_x0_y1);
				pRegister->hPlate=pMirPlate->handle;
			}
		}
#ifdef __TMP_LOG_PROC_
		logerr.Log("\t MirTaAtom(2)");
#endif
		if(MirTaAtom(pPlate, MIRMSG(3), TRUE))
		{
			for(pMirRelaObj=pPlate->relativeObjs.GetFirst();pMirRelaObj;pMirRelaObj=pPlate->relativeObjs.GetNext())
			{
				if(pMirRelaObj->mirInfo.IsSameMirMsg(MIRMSG(3)))
				{
					pMirPlate=(CLDSPlate*)Ta.Parts.FromHandle(pMirRelaObj->hObj,CLS_PLATE);
					break;
				}
			}
			CDesignLjPartPara* pBaseLj=pMirPlate==NULL?NULL:pMirPlate->designInfo.partList.GetValue(pMirPlate->designInfo.m_hBasePart);
			if(pBaseLj)
			{
				CXhChar50 key=CUnifyModelLayer::PLATE::BuildKey(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,pBaseLj->angle.cur_wing_x0_y1);
				CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,'X'+pBaseLj->angle.cur_wing_x0_y1);
				pRegister->hPlate=pMirPlate->handle;
			}
		}
#ifdef __TMP_LOG_PROC_
		logerr.Log("\t MirTaAtom(3)");
#endif
	}
	return true;
}
bool CTowerMultiLegDesigner::CorrectTrunkPostSlopeConnPlate(TRUNK_POST_KJOINT_CONNPLATE& despara, ROD_PTR* xarrDownRods,
					UINT count, bool bMirCreate /*= true*/)
{
	if (despara.pUpperPostAngle == NULL || despara.pPostAngle == NULL || despara.pHoriAngle==NULL)
		return false;
	//计算当前工作面法线
	GEPOINT vLenStdVec = despara.pPostAngle->xPosEnd - despara.pPostAngle->xPosStart;
	normalize(vLenStdVec);
	if (vLenStdVec.z < 0)
		vLenStdVec *= -1.0;
	if (despara.ciFront0OrSide1 == 0)
		despara.vPlaneNormal.Set(0, vLenStdVec.z, -vLenStdVec.y);
	else //if(despara.ciFront0OrSide1==1)
		despara.vPlaneNormal.Set(vLenStdVec.z, 0, -vLenStdVec.x);
	//调整变坡横材的定位参数
	BYTE ciHoriTipS0E1 = 2, ciHoriWingX0Y1 = 0;
	despara.JudgeConnectTipAndWing(despara.pHoriAngle, &ciHoriTipS0E1, &ciHoriWingX0Y1);
	JGZJ jgzj = despara.pHoriAngle->GetZhunJu(ciHoriWingX0Y1 + 'X');
	despara.pHoriAngle->desStartPos.len_offset_type = 1;
	despara.pHoriAngle->desStartPos.len_offset_dist = jgzj.g+10;
	despara.pHoriAngle->desEndPos.len_offset_type = 1;
	despara.pHoriAngle->desEndPos.len_offset_dist = jgzj.g + 10;
	if (ciHoriWingX0Y1 == 0)
	{
		despara.pHoriAngle->desStartPos.wing_x_offset.SetOffset(0);
		despara.pHoriAngle->desStartPos.wing_y_offset.SetOffset(jgzj.g);
		despara.pHoriAngle->desEndPos.wing_x_offset.SetOffset(0);
		despara.pHoriAngle->desEndPos.wing_y_offset.SetOffset(jgzj.g);
	}
	else if(ciHoriWingX0Y1==1)
	{
		despara.pHoriAngle->desStartPos.wing_x_offset.SetOffset(jgzj.g);
		despara.pHoriAngle->desStartPos.wing_y_offset.SetOffset(0);
		despara.pHoriAngle->desEndPos.wing_x_offset.SetOffset(jgzj.g);
		despara.pHoriAngle->desEndPos.wing_y_offset.SetOffset(0);
	}
	despara.pHoriAngle->SetModified();
	despara.pHoriAngle->CalPosition();
	if (ciHoriTipS0E1 == 0)
	{
		despara.pHoriAngle->desStartOdd.m_iOddCalStyle = 0;
		despara.pHoriAngle->desStartOdd.m_hRefPart1 = despara.pPostAngle->handle;
		despara.pHoriAngle->desStartOdd.m_hRefPart2 = 0;
		despara.pHoriAngle->CalStartOddment();
	}
	else
	{
		despara.pHoriAngle->desEndOdd.m_iOddCalStyle = 0;
		despara.pHoriAngle->desEndOdd.m_hRefPart1 = despara.pPostAngle->handle;
		despara.pHoriAngle->desEndOdd.m_hRefPart2 = 0;
		despara.pHoriAngle->CalEndOddment();
	}
	//设置钢板厚度
	int nActualPlateThick = 6;
	if (despara.pUpXieAngle)
		nActualPlateThick = max(nActualPlateThick, (int)despara.pUpXieAngle->size_thick);
	if (despara.pHoriAngle)
		nActualPlateThick = max(nActualPlateThick, (int)despara.pHoriAngle->size_thick);
	if (despara.pDownXieAngle)
		nActualPlateThick = max(nActualPlateThick, (int)despara.pDownXieAngle->size_thick);
	if (nActualPlateThick % 2)
		nActualPlateThick++;
	nActualPlateThick += 2;//塔身主材上K板厚度比射线材大一级
	//布置变坡点上部主材、斜材及横材上的螺栓
	CDesignLjPara designinfo;
	CDesignLjPartPara* pConnAnglePara = NULL;
	BOLTSET boltSet, newBoltSet, shareBoltSet;
	ARRAY_LIST<CLDSBolt*> existBolts;
	CXhSimpleList<BOLT_LOCATION> listLocations;
	CMappingBolts mapping;
	for (int i = 0; i < 3; i++)
	{
		CLDSLineAngle* pAngle = NULL;
		if (i == 0)
			pAngle = despara.pUpperPostAngle;
		else if (i == 1)
		{
			despara.RetrieveCurrHeightKJointUpXieRod(&Ta);
			pAngle = despara.pUpXieAngle;
		}
		else
			pAngle = despara.pHoriAngle;
		if(pAngle==NULL)
			continue;
		//添加钢板连接杆件信息
		BYTE ciTipS0E1 = 2, ciWorkWingX0Y1 = 0;
		despara.JudgeConnectTipAndWing(pAngle, &ciTipS0E1, &ciWorkWingX0Y1);
		CDesignLjPartPara* pConnAnglePara = designinfo.partList.Add(pAngle->handle);
		pConnAnglePara->start0_end1 = ciTipS0E1;
		pConnAnglePara->m_nClassTypeId = CLS_LINEANGLE;
		pConnAnglePara->iFaceNo = (i < 2) ? 2 : 1;
		pConnAnglePara->angle.bTwoEdge = false;
		pConnAnglePara->angle.cur_wing_x0_y1 = ciWorkWingX0Y1;
		if(i==0)
			pConnAnglePara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOBER);
		else
			pConnAnglePara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
		pConnAnglePara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
		//调整杆件正负头及法向偏移
		GEPOINT vWorkNormal = ciWorkWingX0Y1 == 0 ? pAngle->vxWingNorm : pAngle->vyWingNorm;
		if (i == 0)
		{	//上主材指定正负头
			if (ciTipS0E1 == 0)
			{
				pAngle->desStartOdd.m_iOddCalStyle = 2;
				pAngle->SetStartOdd(-15);
			}
			else
			{
				pAngle->desEndOdd.m_iOddCalStyle = 2;
				pAngle->SetEndOdd(-15);
			}
		}
		else if (i == 1)
		{
			if (ciTipS0E1 == 0)
			{
				pAngle->desStartOdd.m_iOddCalStyle = 0;
				pAngle->desStartOdd.m_hRefPart1 = despara.pUpperPostAngle->handle;
				pAngle->desStartOdd.m_hRefPart2 = despara.pHoriAngle->handle;
				pAngle->CalStartOddment();
				if (vWorkNormal*despara.vPlaneNormal < 0)
				{
					if (ciWorkWingX0Y1 == 0)
						pAngle->desStartPos.wing_x_offset.SetOffset(-nActualPlateThick);
					else
						pAngle->desStartPos.wing_y_offset.SetOffset(-nActualPlateThick);
				}
			}
			else
			{
				pAngle->desEndOdd.m_iOddCalStyle = 0;
				pAngle->desEndOdd.m_hRefPart1 = despara.pUpperPostAngle->handle;
				pAngle->desEndOdd.m_hRefPart2 = despara.pHoriAngle->handle;
				pAngle->CalEndOddment();
				if (vWorkNormal*despara.vPlaneNormal < 0)
				{
					if (ciWorkWingX0Y1 == 0)
						pAngle->desEndPos.wing_x_offset.SetOffset(-nActualPlateThick);
					else
						pAngle->desEndPos.wing_y_offset.SetOffset(-nActualPlateThick);
				}
			}
		}
		//布置角钢螺栓
		jgzj = pAngle->GetZhunJu(ciWorkWingX0Y1 + 'X');
		long g[4]={jgzj.g,jgzj.g1,jgzj.g2,jgzj.g3};
		CConnectInfo* pCurrConnInfo = (ciTipS0E1 == 0) ? (&pAngle->connectStart) : (&pAngle->connectEnd);
		UINT uiBoltMd = (ciTipS0E1 == 0) ? pAngle->connectStart.d : pAngle->connectEnd.d;
		int nBoltNum = max(2, pCurrConnInfo->wnConnBoltN);
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace, uiBoltMd);
		BOLT_LAYOUT layout;
		layout.ciLayoutStyle = 0;
		layout.basepoint.ciPosCalStyle = 0;
		layout.cnCount = nBoltNum;
		layout.ciRows = 0;	//自动判断
		layout.cnMaxSingularLineBolts = 4;
		layout.ciFlowDirection = (ciTipS0E1 == 0) ? BOLT_LAYOUT::FLOW_StoE : BOLT_LAYOUT::FLOW_EtoS;
		layout.ciDatumWing = ciWorkWingX0Y1 + 'X';
		listLocations.Empty();
		layout.LayoutBolts(g, &listLocations, LsSpace.EndSpace, LsSpace.SingleRowSpace, LsSpace.doubleRowSpace);
		//提取相应位置的已布螺栓集合
		WORD wiSearchRadius = (WORD)((nBoltNum+1)*LsSpace.SingleRowSpace); 
		existBolts.SetSize(0, 16);
		boltSet.Empty();
		RetrieveExistBolts(boltSet, pAngle, despara.pBaseNode, ciWorkWingX0Y1, wiSearchRadius);
		CLDSBolt* pBolt=NULL;
		for (pBolt = boltSet.GetFirst(); pBolt; pBolt = boltSet.GetNext())
		{
			shareBoltSet.append(pBolt);
			existBolts.append(pBolt);
		}
		//布置螺栓，填充螺栓设计信息
		BOLT_LOCATION* pLocation=NULL;
		for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
			pLocation->TestCalBoltPosition(pAngle, despara.pBaseNode);
		mapping.ClearMapping();
		mapping.InitWorkPlaneCS(pAngle, ciWorkWingX0Y1, despara.pBaseNode);
		mapping.Mapping(existBolts.m_pData, existBolts.GetSize(), &listLocations, 5);
		for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
		{
			if (pLocation->pRelaBolt)
				pBolt = pLocation->pRelaBolt;
			else
			{
				pBolt = (CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
				pBolt->SetLayer("SP*");
				pBolt->layer(2) = pAngle->Layer(2);
				pBolt->cfgword = pAngle->cfgword;
				pBolt->d = uiBoltMd;
				pBolt->des_base_pos.hPart = pAngle->handle;
				pLocation->WriteParamToBolt(pBolt, pAngle, despara.pBaseNode);
				pBolt->AddL0Thick(pAngle->handle, TRUE, TRUE);
				pBolt->CalGuigeAuto();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				//
				shareBoltSet.append(pBolt);
				pAngle->AppendLsRef(pBolt->GetLsRef());
			}
		}
	}
	//设计变坡处双面板
	for (UINT j = 0; j < count; j++)
	{
		SmartRodPtr pDownRod = xarrDownRods[j];
		if (!pDownRod.blValid || pDownRod.pAngle==NULL)
			break;
		CLDSLineAngle* pCurrPostAngle = despara.pPostAngle = pDownRod.pAngle;
		despara.RetrieveCurrHeightKJointDownXieRod(&Ta);
		//设置下侧接身主材正负头
		BYTE ciPostTipS0E1 = 2, ciPostWorkWingX0Y1 = 0,ciXieTipS0E1 = 2, ciXieWorkWingX0Y1 = 0;
		despara.JudgeConnectTipAndWing(pCurrPostAngle, &ciPostTipS0E1, &ciPostWorkWingX0Y1);
		if (ciPostTipS0E1 == 0)
		{
			pCurrPostAngle->desStartOdd.m_iOddCalStyle = 2;
			pCurrPostAngle->SetStartOdd(-15);
		}
		else
		{
			pCurrPostAngle->desEndOdd.m_iOddCalStyle = 2;
			pCurrPostAngle->SetEndOdd(-15);
		}
		//TODO:还未考虑一键生成前就已有板设计，此时再重复布置螺栓，如果布置方案不同容易出现冲突 wjh-2019.8.2
		//布置基准主材上的螺栓，填充螺栓设计信息
		jgzj = pCurrPostAngle->GetZhunJu(ciPostWorkWingX0Y1 + 'X');
		long g[4] = { jgzj.g,jgzj.g1,jgzj.g2,jgzj.g3 };
		CConnectInfo* pCurrConnInfo = (ciPostTipS0E1 == 0) ? (&pCurrPostAngle->connectStart) : (&pCurrPostAngle->connectEnd);
		UINT uiBoltMd = (ciPostTipS0E1 == 0) ? pCurrPostAngle->connectStart.d : pCurrPostAngle->connectEnd.d;
		int nBoltNum = max(3, pCurrConnInfo->wnConnBoltN);
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace, uiBoltMd);
		BOLT_LAYOUT layout;
		layout.ciLayoutStyle = 0;
		layout.basepoint.ciPosCalStyle = 0;
		layout.ciRows = 0;	//自动判断
		layout.cnCount = nBoltNum;
		layout.ciFlowDirection = (ciPostTipS0E1 == 0) ? BOLT_LAYOUT::FLOW_StoE : BOLT_LAYOUT::FLOW_EtoS;
		layout.ciDatumWing = ciPostWorkWingX0Y1 + 'X';
		listLocations.Empty();
		WORD wiSingleRowSpace = (despara.wiPostAngleBoltSpace==0)? (WORD)LsSpace.SingleRowSpace : despara.wiPostAngleBoltSpace;
		layout.LayoutBolts(g, &listLocations, LsSpace.EndSpace, wiSingleRowSpace, LsSpace.doubleRowSpace);
		//提取相应位置的已布螺栓集合
		boltSet.Empty();
		newBoltSet.Empty();
		existBolts.Empty();
		WORD wiSearchRadius = (WORD)ftoi((despara.ciPostAngleBoltNum + 1)*0.5*wiSingleRowSpace + wiSingleRowSpace);
		RetrieveExistBolts(boltSet, pCurrPostAngle, despara.pBaseNode, ciPostWorkWingX0Y1, wiSearchRadius);
		CLDSBolt* pBolt = NULL;
		for (pBolt = boltSet.GetFirst(); pBolt; pBolt = boltSet.GetNext())
			existBolts.append(pBolt);
		//更新或添加主材上的螺栓
		BOLT_LOCATION* pLocation = NULL;
		for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
			pLocation->TestCalBoltPosition(pCurrPostAngle, despara.pBaseNode);
		mapping.ClearMapping();
		mapping.InitWorkPlaneCS(pCurrPostAngle, ciPostWorkWingX0Y1, despara.pBaseNode);
		mapping.Mapping(existBolts.m_pData, existBolts.GetSize(), &listLocations);
		for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
		{
			if (pLocation->pRelaBolt)
				pBolt = pLocation->pRelaBolt;
			else
				pBolt = (CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
			pBolt->cfgword = pCurrPostAngle->cfgword;
			pBolt->SetLayer(pCurrPostAngle->layer());
			pBolt->layer(1) = 'P';
			pBolt->d = uiBoltMd;
			pBolt->des_base_pos.hPart = pCurrPostAngle->handle;
			pLocation->WriteParamToBolt(pBolt, pCurrPostAngle, despara.pBaseNode);
			pBolt->AddL0Thick(pCurrPostAngle->handle, TRUE, TRUE);
			pBolt->CalGuigeAuto();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			//
			newBoltSet.append(pBolt);
			pCurrPostAngle->AppendLsRef(pBolt->GetLsRef());
		}
		//下侧接身斜材
		if(despara.pDownXieAngle!=NULL)
		{	//添加钢板连接杆件信息
			despara.JudgeConnectTipAndWing(despara.pDownXieAngle, &ciXieTipS0E1, &ciXieWorkWingX0Y1);
			GEPOINT vWorkNormal = (ciXieWorkWingX0Y1 == 0) ? despara.pDownXieAngle->vxWingNorm : despara.pDownXieAngle->vyWingNorm;
			if (ciXieTipS0E1 == 0)
			{
				despara.pDownXieAngle->desStartOdd.m_iOddCalStyle = 0;
				despara.pDownXieAngle->desStartOdd.m_hRefPart1 = pCurrPostAngle->handle;
				despara.pDownXieAngle->desStartOdd.m_hRefPart2 = despara.pHoriAngle->handle;
				despara.pDownXieAngle->CalStartOddment();
				//
				if (vWorkNormal*despara.vPlaneNormal < 0)
				{
					if (ciXieWorkWingX0Y1 == 0)
						despara.pDownXieAngle->desStartPos.wing_x_offset.SetOffset(-nActualPlateThick);
					else
						despara.pDownXieAngle->desStartPos.wing_y_offset.SetOffset(-nActualPlateThick);
				}
			}
			else if (ciXieTipS0E1 == 1)
			{
				despara.pDownXieAngle->desEndOdd.m_iOddCalStyle = 0;
				despara.pDownXieAngle->desEndOdd.m_hRefPart1 = pCurrPostAngle->handle;
				despara.pDownXieAngle->desEndOdd.m_hRefPart2 = despara.pHoriAngle->handle;
				despara.pDownXieAngle->CalEndOddment();
				//
				if (vWorkNormal*despara.vPlaneNormal < 0)
				{
					if (ciXieWorkWingX0Y1 == 0)
						despara.pDownXieAngle->desEndPos.wing_x_offset.SetOffset(-nActualPlateThick);
					else
						despara.pDownXieAngle->desEndPos.wing_y_offset.SetOffset(-nActualPlateThick);
				}
			}
			//填充螺栓设计信息
			jgzj = despara.pDownXieAngle->GetZhunJu(ciXieWorkWingX0Y1 + 'X');
			g[0] = jgzj.g; g[1] = jgzj.g1; g[2] = jgzj.g2; g[3] = jgzj.g3;
			CConnectInfo* pCurrConnInfo=(ciXieTipS0E1 == 0)?(&despara.pDownXieAngle->connectStart):(&despara.pDownXieAngle->connectEnd);
			int nBoltNum = max(2, pCurrConnInfo->wnConnBoltN);
			uiBoltMd = (ciXieTipS0E1 ==0) ? despara.pDownXieAngle->connectStart.d : despara.pDownXieAngle->connectEnd.d;
			GetLsSpace(LsSpace, uiBoltMd);
			layout.ciLayoutStyle = 0;
			layout.basepoint.ciPosCalStyle = 0;
			layout.cnCount = nBoltNum;
			layout.ciRows = 0;
			layout.ciFlowDirection = (ciXieTipS0E1==0) ? BOLT_LAYOUT::FLOW_StoE : BOLT_LAYOUT::FLOW_EtoS;
			layout.ciDatumWing = ciXieWorkWingX0Y1 + 'X';
			listLocations.Empty();
			layout.LayoutBolts(g, &listLocations, LsSpace.EndSpace, LsSpace.SingleRowSpace, LsSpace.doubleRowSpace);
			//提取相应位置的已布螺栓集合
			boltSet.Empty();
			existBolts.SetSize(0, 16);
			wiSearchRadius = (WORD)((nBoltNum+1)*LsSpace.SingleRowSpace);
			RetrieveExistBolts(boltSet, despara.pDownXieAngle, despara.pBaseNode, ciXieWorkWingX0Y1, wiSearchRadius);
			for (pBolt = boltSet.GetFirst(); pBolt; pBolt = boltSet.GetNext())
				existBolts.append(pBolt);
			//更新或添加主材上的螺栓
			for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
				pLocation->TestCalBoltPosition(despara.pDownXieAngle, despara.pBaseNode);
			mapping.ClearMapping();
			mapping.InitWorkPlaneCS(despara.pDownXieAngle, ciXieWorkWingX0Y1, despara.pBaseNode);
			mapping.Mapping(existBolts.m_pData, existBolts.GetSize(), &listLocations);
			for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
			{
				if (pLocation->pRelaBolt)
					pBolt = pLocation->pRelaBolt;
				else
					pBolt = (CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
				pBolt->SetLayer(despara.pDownXieAngle->layer());
				pBolt->layer(1) = 'P';
				pBolt->cfgword = despara.pDownXieAngle->cfgword;
				pBolt->d = uiBoltMd;
				pBolt->des_base_pos.hPart = despara.pDownXieAngle->handle;
				pLocation->WriteParamToBolt(pBolt, despara.pDownXieAngle, despara.pBaseNode);
				pBolt->AddL0Thick(despara.pDownXieAngle->handle, TRUE, TRUE);
				pBolt->CalGuigeAuto();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				//
				newBoltSet.append(pBolt);
				despara.pDownXieAngle->AppendLsRef(pBolt->GetLsRef());
			}
		}
		//查找该节点当前面是否已设计连接板
		CLDSPlate* pPlate = NULL;
		for (pPlate = (CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE); pPlate; pPlate = (CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
		{
			if (pPlate->m_hBaseNode != despara.pBaseNode->handle || pPlate->m_hBaseRod != pCurrPostAngle->handle)
				continue;
			if (!pCurrPostAngle->ModuleTogetherWith(pPlate))
				continue;	//不属于当前接腿
			if (pPlate->designInfo.norm.hVicePart != pCurrPostAngle->handle || fabs(pPlate->ucs.axis_z*despara.vPlaneNormal) < 0.9)
				continue;
			if (pPlate->wiType != CLDSPlate::CONNECT_PLATE)
				continue;
			break;
		}
		if (pPlate != NULL)
			continue;
		CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(
			despara.pBaseNode->handle, pCurrPostAngle->handle, 'X' + ciPostWorkWingX0Y1);
		if (pRegister->hPlate > 0x20)
		{
			if((pPlate = (CLDSPlate*)Ta.Parts.FromHandle(pRegister->hPlate, CLS_PLATE))!=NULL)
				continue;
		}
		else
		{
			pPlate = (CLDSPlate*)Ta.Parts.append(CLS_PLATE);
			pRegister->hPlate = pPlate->handle;
		}
		if (despara.ciFront0OrSide1 == 0)
			pPlate->SetLayer("SPQ");
		else if (despara.ciFront0OrSide1 == 1)
			pPlate->SetLayer("SPY");
		pPlate->cfgword = pCurrPostAngle->cfgword;
		pPlate->Thick = nActualPlateThick;
		//设定连接板基点参数
		designinfo.CloneTo(pPlate->designInfo);
		CDesignLjPartPara* pConnAnglePara = pPlate->designInfo.partList.Add(pCurrPostAngle->handle);
		pConnAnglePara->m_nClassTypeId = CLS_LINEANGLE;
		pConnAnglePara->iFaceNo = 1;
		pConnAnglePara->start0_end1 = ciPostTipS0E1;
		pConnAnglePara->angle.bTwoEdge = false;
		pConnAnglePara->angle.cur_wing_x0_y1 = ciPostWorkWingX0Y1;
		pConnAnglePara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOBER);
		pConnAnglePara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
		if (despara.pDownXieAngle)
		{
			pConnAnglePara = pPlate->designInfo.partList.Add(despara.pDownXieAngle->handle);
			pConnAnglePara->m_nClassTypeId = CLS_LINEANGLE;
			pConnAnglePara->iFaceNo = 1;
			pConnAnglePara->start0_end1 = ciXieTipS0E1; 
			pConnAnglePara->angle.cur_wing_x0_y1 = ciXieWorkWingX0Y1;
			pConnAnglePara->angle.bTwoEdge = false;
			pConnAnglePara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
			pConnAnglePara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
		}
		//设定连接板定位参数
		pPlate->designInfo.m_bEnableFlexibleDesign = true;
		pPlate->designInfo.m_hBaseNode = despara.pBaseNode->handle;
		pPlate->designInfo.m_hBasePart = pCurrPostAngle->handle;
		pPlate->designInfo.origin.datum_pos_style = 2;
		pPlate->designInfo.origin.des_para.RODNODE.hNode = despara.pBaseNode->handle;
		pPlate->designInfo.origin.des_para.RODNODE.hRod = pCurrPostAngle->handle;
		pPlate->designInfo.origin.des_para.RODNODE.offset_wing = ciPostWorkWingX0Y1;
		pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style = 0;
		pPlate->designInfo.origin.UpdatePos(this->m_pTower);
		//设定连接板法线参数
		pPlate->designInfo.norm.norm_style = 1;
		pPlate->designInfo.norm.hVicePart = pCurrPostAngle->handle;
		pPlate->designInfo.norm.norm_wing = ciPostWorkWingX0Y1;
		pPlate->designInfo.norm.direction = 0;	//由里朝外
		//钢板火曲信息
		pPlate->face_N = 2;
		pPlate->designInfo.iFaceType = 2;
		pPlate->designInfo.huoqufacenorm[0].norm_style = 2;
		pPlate->designInfo.huoqufacenorm[0].hVicePart = despara.pHoriAngle->handle;
		pPlate->designInfo.huoqufacenorm[0].hCrossPart = despara.pUpperPostAngle->handle;
		pPlate->designInfo.huoqufacenorm[0].nearVector = despara.vPlaneNormal;
		pPlate->designInfo.huoquline_start[0].datum_pos_style = 2;
		pPlate->designInfo.huoquline_start[0].des_para.RODNODE.hRod = despara.pHoriAngle->handle;
		pPlate->designInfo.huoquline_start[0].des_para.RODNODE.hNode = despara.pBaseNode->handle;
		pPlate->designInfo.huoquline_start[0].des_para.RODNODE.direction = ciHoriTipS0E1;
		pPlate->designInfo.huoquline_start[0].des_para.RODNODE.offset_wing = ciHoriWingX0Y1;
		pPlate->designInfo.huoquline_start[0].des_para.RODNODE.wing_offset_style = 4;
		pPlate->designInfo.huoquline_start[0].des_para.RODNODE.wing_offset_dist = -10;
		pPlate->designInfo.huoquline_start[0].des_para.RODNODE.len_offset_dist = -200;
		pPlate->designInfo.huoquline_start[0].CloneParaTo(&pPlate->designInfo.huoquline_end[0]);
		pPlate->designInfo.huoquline_end[0].des_para.RODNODE.len_offset_dist = 200;
		//添加钢板螺栓
		for (pBolt = shareBoltSet.GetFirst(); pBolt; pBolt = shareBoltSet.GetNext())
		{
			pBolt->AddL0Thick(pPlate->handle, TRUE, TRUE);
			pBolt->CalGuigeAuto();
			pPlate->AppendLsRef(pBolt->GetLsRef());
		}
		for (pBolt = newBoltSet.GetFirst(); pBolt; pBolt = newBoltSet.GetNext())
		{
			pBolt->AddL0Thick(pPlate->handle, TRUE, TRUE);
			pBolt->CalGuigeAuto();
			pPlate->AppendLsRef(pBolt->GetLsRef());
		}
		pPlate->DesignPlate();
		//对称生成其他钢板
		RELATIVE_OBJECT* pMirRelaObj=NULL;
		CLDSPlate* pMirPlate = NULL;
		if (MirTaAtom(pPlate, MIRMSG(1), TRUE))
		{
			for (pMirRelaObj = pPlate->relativeObjs.GetFirst(); pMirRelaObj; pMirRelaObj = pPlate->relativeObjs.GetNext())
			{
				if (pMirRelaObj->mirInfo.IsSameMirMsg(MIRMSG(1)))
				{
					pMirPlate = (CLDSPlate*)Ta.Parts.FromHandle(pMirRelaObj->hObj, CLS_PLATE);
					break;
				}
			}
			CDesignLjPartPara* pBaseLj = pMirPlate == NULL ? NULL : pMirPlate->designInfo.partList.GetValue(pMirPlate->designInfo.m_hBasePart);
			if (pBaseLj)
			{
				CXhChar50 key = CUnifyModelLayer::PLATE::BuildKey(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart, pBaseLj->angle.cur_wing_x0_y1);
				CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart, 'X' + pBaseLj->angle.cur_wing_x0_y1);
				pRegister->hPlate = pMirPlate->handle;
			}
		}
		if (MirTaAtom(pPlate, MIRMSG(2), TRUE))
		{
			for (pMirRelaObj = pPlate->relativeObjs.GetFirst(); pMirRelaObj; pMirRelaObj = pPlate->relativeObjs.GetNext())
			{
				if (pMirRelaObj->mirInfo.IsSameMirMsg(MIRMSG(2)))
				{
					pMirPlate = (CLDSPlate*)Ta.Parts.FromHandle(pMirRelaObj->hObj, CLS_PLATE);
					break;
				}
			}
			CDesignLjPartPara* pBaseLj = pMirPlate == NULL ? NULL : pMirPlate->designInfo.partList.GetValue(pMirPlate->designInfo.m_hBasePart);
			if (pBaseLj)
			{
				CXhChar50 key = CUnifyModelLayer::PLATE::BuildKey(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart, pBaseLj->angle.cur_wing_x0_y1);
				CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart, 'X' + pBaseLj->angle.cur_wing_x0_y1);
				pRegister->hPlate = pMirPlate->handle;
			}
		}
		if (MirTaAtom(pPlate, MIRMSG(3), TRUE))
		{
			for (pMirRelaObj = pPlate->relativeObjs.GetFirst(); pMirRelaObj; pMirRelaObj = pPlate->relativeObjs.GetNext())
			{
				if (pMirRelaObj->mirInfo.IsSameMirMsg(MIRMSG(3)))
				{
					pMirPlate = (CLDSPlate*)Ta.Parts.FromHandle(pMirRelaObj->hObj, CLS_PLATE);
					break;
				}
			}
			CDesignLjPartPara* pBaseLj = pMirPlate == NULL ? NULL : pMirPlate->designInfo.partList.GetValue(pMirPlate->designInfo.m_hBasePart);
			if (pBaseLj)
			{
				CXhChar50 key = CUnifyModelLayer::PLATE::BuildKey(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart, pBaseLj->angle.cur_wing_x0_y1);
				CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart, 'X' + pBaseLj->angle.cur_wing_x0_y1);
				pRegister->hPlate = pMirPlate->handle;
			}
		}
	}
	return true;
}
bool CTowerMultiLegDesigner::CorrectTrunkPostJoints(TRUNK_POST_KJOINT_CONNPLATE& despara, ROD_PTR* xarrDownRods, UINT count, bool bMirCreate/* = true*/)
{
	//1.布置塔身K点上斜材及横材上的螺栓
	SmartRodPtr pConnRod;
	SmartRodPtr pDownRod;
	CJoint joint;
	CDesignJoint designJoint;
	joint.cfgword = Ta.GetDefaultCfgPartNo();
	joint.base_node_handle = despara.pBaseNode->handle;
	joint.SetBelongModel(&Ta);
	joint.blInheritExistBoltsInMainAngle = false;
	joint.blInheritExistBoltsInCoupleAngle = true;	//上侧塔身主材继承已布螺栓
	joint.hCoupleAngle = despara.pUpperPostAngle->handle;
	double dfExtendOddment = 0;
	for (UINT j = 0; j < count; j++)
	{
		pDownRod = xarrDownRods[j];
		if (!pDownRod.blValid)
			break;
		joint.SetLayer(pDownRod->szLayer);
		m_pDesignerContext->hashJoints.Add(DUALKEY(despara.pUpperPostAngle->handle, pDownRod->handle, true));
		joint.hMainAngle = pDownRod->handle;
		joint.iSeg = despara.pPostAngle->iSeg;
		if(j == 0)
			joint.blIntelliCalOddment = true;
		else //if (j > 0)
		{
			joint.blIntelliCalOddment = false;
			if (pDownRod->pStart == despara.pBaseNode)
			{
				pDownRod->desStartOdd.m_iOddCalStyle = 2;
				pDownRod->SetStartOdd(dfExtendOddment);
			}
			else
			{
				pDownRod->desEndOdd.m_iOddCalStyle = 2;
				pDownRod->SetEndOdd(dfExtendOddment);
			}
		}
		designJoint.CreateJoint(&joint, TRUE, TRUE);
		if (j == 0)
		{
			if (pDownRod->pStart == despara.pBaseNode)
				dfExtendOddment = pDownRod->dfStartOdd;
			else
				dfExtendOddment = pDownRod->dfEndOdd;
		}
		if (bMirCreate)
		{
			designJoint.MirJoint(&joint, 4);	//四度对称
			for (int mirtype = 1; mirtype < 4; mirtype++)
			{
				CLDSLineAngle* pMirMainJg1 = (CLDSLineAngle*)despara.pUpperPostAngle->GetMirPart(mirtype);
				CLDSLineAngle* pMirMainJg2 = (CLDSLineAngle*)pDownRod->GetMirPart(mirtype);
				m_pDesignerContext->hashJoints.Add(DUALKEY(pMirMainJg1->handle, pMirMainJg2->handle, true));
			}
		}
	}
	return true;
}
bool CTowerMultiLegDesigner::CorrectLegHoriKNodeConnPlate(MULTILEG_HORI_K_CONNPLATE& despara,bool bMirCreate/*=true*/)
{
	CLDSLineAngle* pAngle=NULL;
	//计算当前工作面法线
	GEPOINT vLenStdVec=despara.pLeftLegAngle->xPosEnd-despara.pLeftLegAngle->xPosStart;
	GEPOINT vLineLenStdVec=despara.pLeftLegAngle->pEnd->xOriginalPos-despara.pLeftLegAngle->pStart->xOriginalPos;
	GEPOINT vLinePlaneNormal;	//单线模型的工作面（避免计算误差，判断选取腿部斜材相对更准确)
	if(despara.ciFront0OrSide1==0)
	{
		despara.vPlaneNormal=vLenStdVec^GEPOINT(1,0,0);
		if(despara.vPlaneNormal.y<0)
			despara.vPlaneNormal*=-1;
		vLinePlaneNormal=vLineLenStdVec^GEPOINT(1,0,0);
		if(vLinePlaneNormal.y<0)
			vLinePlaneNormal*=-1;
	}
	else //if(despara.ciFront0OrSide1==1)
	{
		despara.vPlaneNormal=vLenStdVec^GEPOINT(0,1,0);
		if(despara.vPlaneNormal.x<0)
			despara.vPlaneNormal*=-1;
		vLinePlaneNormal=vLineLenStdVec^GEPOINT(0,1,0);
		if(vLinePlaneNormal.x<0)
			vLinePlaneNormal*=-1;
	}
	normalize(despara.vPlaneNormal);
	normalize(vLinePlaneNormal);
	//
	CDesignLjPara designinfo;
	designinfo.m_bEnableFlexibleDesign=true;
	designinfo.m_hBaseNode=despara.pBaseNode->handle;
	designinfo.m_hBasePart=despara.pBaseHoriAngle->handle;
	//设定连接板基点参数
	designinfo.origin.datum_pos_style=2;
	designinfo.origin.des_para.RODNODE.hNode=despara.pBaseNode->handle;
	designinfo.origin.des_para.RODNODE.hRod=despara.pBaseHoriAngle->handle;
	if(fabs(despara.pBaseHoriAngle->vxWingNorm*despara.vPlaneNormal)>fabs(despara.pBaseHoriAngle->vyWingNorm*despara.vPlaneNormal))
		designinfo.origin.des_para.RODNODE.offset_wing=0;
	else
		designinfo.origin.des_para.RODNODE.offset_wing=1;
	designinfo.origin.des_para.RODNODE.wing_offset_style=0;
	designinfo.origin.UpdatePos(this->m_pTower);
	//设定连接板法线参数
	designinfo.norm.norm_style=1;
	designinfo.norm.hVicePart=despara.pBaseHoriAngle->handle;
	designinfo.norm.norm_wing=designinfo.origin.des_para.RODNODE.offset_wing;
	designinfo.norm.direction=0;	//由里朝外
	BYTE ciTipS0E1=2,ciWorkWingX0Y1=(BYTE)designinfo.norm.norm_wing;
	//布置基准横材上的螺栓
	CLDSBolt* pBolt;
	char ciConnWing=designinfo.norm.norm_wing==0?'X':'Y';
	JGZJ jgzj=despara.pBaseHoriAngle->GetZhunJu(ciConnWing);
	long g[4]={jgzj.g,jgzj.g1,jgzj.g2,jgzj.g3};
	UINT uiBoltMd=despara.pBaseHoriAngle->connectStart.d;
	BOLTSET boltset,shareboltset;
	PRESET_ARRAY8<CLDSBolt*> existbolts;
	WORD wiSearchRadius=(WORD)ftoi((despara.ciHoriAngleBoltNum+1)*0.5*despara.wiHoriAngleBoltSpace+despara.wiHoriAngleBoltSpace);
	RetrieveExistBolts(boltset,despara.pBaseHoriAngle,despara.pBaseNode,ciWorkWingX0Y1,wiSearchRadius);
	for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
	{
		shareboltset.append(pBolt);
		existbolts.Append(pBolt);
	}
	//更新或添加横材上的螺栓
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,uiBoltMd);
	WORD wiSingleRowSpace=despara.wiHoriAngleBoltSpace;//nBoltSpace;
	BOLT_LOCATION* pLocation;
	CXhSimpleList<BOLT_LOCATION> listLocations;
	BOLT_LAYOUT layout(BOLT_LAYOUT::LAYOUT_DUALSIDE,despara.ciHoriAngleBoltNum,1);
	layout.basepoint.ciPosCalStyle=1;
	layout.ciDatumWing=ciWorkWingX0Y1+'X';
	layout.LayoutBolts(g,&listLocations,LsSpace.EndSpace,wiSingleRowSpace,LsSpace.doubleRowSpace);
	//布置螺栓，填充螺栓设计信息
	int sideI=0,j=0,matchcount=0;
	for(pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
		pLocation->TestCalBoltPosition(despara.pBaseHoriAngle,despara.pBaseNode);
	CMappingBolts mapping(despara.pBaseHoriAngle,ciWorkWingX0Y1);
	matchcount=mapping.Mapping(existbolts.pPresetObjs,existbolts.Count,&listLocations);
	CDesignLjPartPara* pConnAnglePara=designinfo.partList.Add(despara.pBaseHoriAngle->handle);
	pConnAnglePara->m_nClassTypeId=CLS_LINEANGLE;//despara.pBaseHoriAngle->GetClassTypeId();
	pConnAnglePara->angle.bTwoEdge=true;
	pConnAnglePara->iFaceNo=1;
	pConnAnglePara->start0_end1=2;
	pConnAnglePara->angle.cur_wing_x0_y1=ciWorkWingX0Y1;
	for(pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
	{
		if(pLocation->pRelaBolt)
			pBolt=pLocation->pRelaBolt;
		else
			pBolt=(CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
		pBolt->SetLayer("SP*");
		pBolt->layer(2)=despara.pBaseHoriAngle->Layer(2);
		pBolt->cfgword=despara.pBaseHoriAngle->cfgword;
		pBolt->d=uiBoltMd;
		pBolt->des_base_pos.hPart=despara.pBaseHoriAngle->handle;
		pLocation->WriteParamToBolt(pBolt,despara.pBaseHoriAngle,despara.pBaseNode);
		pBolt->AddL0Thick(despara.pBaseHoriAngle->handle,TRUE,TRUE);
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		shareboltset.append(pBolt);
		despara.pBaseHoriAngle->AppendLsRef(pBolt->GetLsRef());
	}
	CConnectInfo* pCurrConnInfo=NULL;
	GEPOINT vCutPlaneNormal;
	//布置上左(右)斜材上螺栓
	for(sideI=0;sideI<2;sideI++)
	{
		CLDSLineAngle* pUpperAngle=sideI==0?despara.pLeftUpperAngle:despara.pRightUpperAngle;
		if(pUpperAngle==NULL)
			break;
		despara.JudgeConnectTipAndWing(pUpperAngle,&ciTipS0E1,&ciWorkWingX0Y1);
		//添加钢板连接杆件信息
		pConnAnglePara=designinfo.partList.Add(pUpperAngle->handle);
		pConnAnglePara->angle.bTwoEdge=false;
		pConnAnglePara->angle.cur_wing_x0_y1=ciWorkWingX0Y1;
		pConnAnglePara->iFaceNo=1;
		pConnAnglePara->start0_end1=ciTipS0E1;
		pConnAnglePara->m_nClassTypeId=CLS_LINEANGLE;//despara.pLeftUpperAngle->GetClassTypeId();
		if(despara.ciFront0OrSide1==0&&j==0)
			vCutPlaneNormal.Set( 1,0,0);
		else if(despara.ciFront0OrSide1==0&&j==1)
			vCutPlaneNormal.Set(-1,0,0);
		else if(despara.ciFront0OrSide1==1&&j==0)
			vCutPlaneNormal.Set(0,-1,0);
		else if(despara.ciFront0OrSide1==1&&j==1)
			vCutPlaneNormal.Set(0, 1,0);
		if(ciTipS0E1==0)
		{
			pUpperAngle->desStartOdd.m_iOddCalStyle=0;
			pUpperAngle->desStartOdd.m_hRefPart1=despara.pBaseHoriAngle->handle;
			pUpperAngle->desStartOdd.m_hRefPart2=0;
			pUpperAngle->CalStartOddment();
			if(pUpperAngle->CalCutAngleInfo(despara.pBaseNode->xOriginalPos,vCutPlaneNormal,NULL))
				pUpperAngle->m_bUserSpecStartCutAngle=true;
		}
		else if(ciTipS0E1==1)
		{
			pUpperAngle->desEndOdd.m_iOddCalStyle=0;
			pUpperAngle->desEndOdd.m_hRefPart1=despara.pBaseHoriAngle->handle;
			pUpperAngle->desEndOdd.m_hRefPart2=0;
			pUpperAngle->CalEndOddment();
			if(pUpperAngle->CalCutAngleInfo(despara.pBaseNode->xOriginalPos,vCutPlaneNormal,NULL))
				pUpperAngle->m_bUserSpecEndCutAngle=true;
		}

		jgzj=pUpperAngle->GetZhunJu(ciWorkWingX0Y1+'X');
		g[0]=jgzj.g;
		g[1]=jgzj.g1;
		g[2]=jgzj.g2;
		g[3]=jgzj.g3;
		layout.ciLayoutStyle=0;
		if(ciTipS0E1==0)
			pCurrConnInfo=&pUpperAngle->connectStart;
		else
			pCurrConnInfo=&pUpperAngle->connectEnd;
		int nBoltNum=max(2,pCurrConnInfo->wnConnBoltN);
		uiBoltMd=ciTipS0E1==0?pUpperAngle->connectStart.d:pUpperAngle->connectEnd.d;
		if(LsSpace.M_Diameter!=uiBoltMd)
			GetLsSpace(LsSpace,uiBoltMd);
		layout.cnCount=max(2,(BYTE)pCurrConnInfo->wnConnBoltN);
		layout.ciRows=0;
		layout.basepoint.ciPosCalStyle=0;
		layout.ciLayoutStyle=0;
		layout.ciFlowDirection=(ciTipS0E1==0)?BOLT_LAYOUT::FLOW_StoE:BOLT_LAYOUT::FLOW_EtoS;
		layout.ciDatumWing=ciWorkWingX0Y1+'X';
		listLocations.Empty();
		layout.LayoutBolts(g,&listLocations,LsSpace.EndSpace,LsSpace.SingleRowSpace,LsSpace.doubleRowSpace);
		//提取相应位置的已布螺栓集合
		existbolts.ReSize(0);
		wiSearchRadius=(WORD)(nBoltNum*LsSpace.SingleRowSpace);
		boltset.Empty();
		RetrieveExistBolts(boltset,pUpperAngle,despara.pBaseNode,ciWorkWingX0Y1,wiSearchRadius);
		for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
		{
			shareboltset.append(pBolt);
			existbolts.Append(pBolt);
		}
		//布置螺栓，填充螺栓设计信息
		for(pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
			pLocation->TestCalBoltPosition(pUpperAngle,despara.pBaseNode);
		mapping.ClearMapping();
		mapping.InitWorkPlaneCS(pUpperAngle,ciWorkWingX0Y1,despara.pBaseNode);
		matchcount=mapping.Mapping(existbolts.pPresetObjs,existbolts.Count,&listLocations);
		for(pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
		{
			if(pLocation->pRelaBolt)
				pBolt=pLocation->pRelaBolt;
			else
				pBolt=(CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
			pBolt->SetLayer("SP*");
			pBolt->layer(2)=pUpperAngle->Layer(2);
			pBolt->cfgword=pUpperAngle->cfgword;
			pBolt->d=uiBoltMd;
			pBolt->des_base_pos.hPart=pUpperAngle->handle;
			pLocation->WriteParamToBolt(pBolt,pUpperAngle,despara.pBaseNode);
			pBolt->AddL0Thick(pUpperAngle->handle,TRUE,TRUE);
			pBolt->CalGuigeAuto();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			shareboltset.append(pBolt);
			pUpperAngle->AppendLsRef(pBolt->GetLsRef());
		}
	}
	//设计接腿倒K连接板
	SmartRodPtr pConnRod;
	CModelHeightNode* pHeight=modeltree.hashHeights.GetValue(despara.ciHeightSerial);
	for(j=0;j<24;j++)
	{
		if(pHeight->xarrSubLegs[j].Id==0)
			break;
		despara.pLeftLegAngle=despara.pRightLegAngle=NULL;
		for(pConnRod=Ta.EnumRodFirst();pConnRod.IsHasPtr();pConnRod=Ta.EnumRodNext())
		{
			if(!pConnRod->IsLegObj()||!pHeight->xarrSubLegs[j].pPostRod->ModuleTogetherWith(pConnRod))
				continue;
			if((pConnRod->pStart&&pConnRod->pStart==despara.pBaseNode)||(pConnRod->pEnd&&pConnRod->pEnd==despara.pBaseNode))
			{
				vLenStdVec=pConnRod->pEnd->xOriginalPos-pConnRod->pStart->xOriginalPos;
				normalize(vLenStdVec);
				if(fabs(vLinePlaneNormal*vLenStdVec)>EPS2)
					continue;
				CLDSNode* pTipNode=pConnRod->pEnd==despara.pBaseNode?pConnRod->pStart:pConnRod->pEnd;
				if((despara.ciFront0OrSide1==0&&pTipNode->xOriginalPos.x<=0)||(despara.ciFront0OrSide1==1&&pTipNode->xOriginalPos.y>=0))
					despara.pLeftLegAngle=pConnRod.pAngle;
				else
					despara.pRightLegAngle=pConnRod.pAngle;
			}
			else
				continue;
			if(despara.pLeftLegAngle&&despara.pRightLegAngle)
				break;
		}
		if (despara.pLeftLegAngle == NULL || despara.pRightLegAngle == NULL)
		{
			//if(despara.pLeftLegAngle==NULL)
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "呼高{%s}的第%d组接腿缺少腿部斜材",(char*)pHeight->name,j+1);
			continue;
		}
		//查找该节点当前面是否已设计连接板
		CLDSPlate* pPlate=NULL,*pExistPlate=NULL;
		for(pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
		{
			if(pPlate->m_hBaseNode!=despara.pBaseNode->handle||pPlate->m_hBaseRod!=despara.pBaseHoriAngle->handle)
				continue;
			if(!despara.pLeftLegAngle->ModuleTogetherWith(pPlate))
				continue;	//不属于当前接腿
			if(pPlate->designInfo.norm.hVicePart!=despara.pBaseHoriAngle->handle||fabs(pPlate->ucs.axis_z*despara.vPlaneNormal)<0.9)
				continue;
			pExistPlate=pPlate;
			break;
		}
		//搜索是否已存在相应的八字铁连接板
		if(pExistPlate!=NULL)
		{
			pPlate=pExistPlate;
			pPlate->designInfo.m_bEnableFlexibleDesign=true;
			pPlate->designInfo.m_hBaseNode=despara.pBaseNode->handle;
			pPlate->designInfo.m_hBasePart=despara.pBaseHoriAngle->handle;
			//TODO:万一原始板中信息不全怎么办？
		}
		else
		{
			CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(
				despara.pBaseNode->handle, despara.pBaseHoriAngle->handle, 'X' + ciWorkWingX0Y1, 
				pHeight->xarrSubLegs[j].idModelLegSerial);
			if (pRegister->hPlate > 0x20)
				pPlate = (CLDSPlate*)Ta.Parts.FromHandle(pRegister->hPlate, CLS_PLATE);
			else
			{
				pPlate = (CLDSPlate*)Ta.Parts.append(CLS_PLATE);
				pRegister->hPlate = pPlate->handle;
			}
			if(despara.ciFront0OrSide1==0)
				pPlate->SetLayer("LPQ");
			else if(despara.ciFront0OrSide1==1)
				pPlate->SetLayer("LPY");
			pPlate->cfgword=CFGWORD(despara.ciHeightSerial,pHeight->xarrSubLegs[j].Id);
			pPlate->designInfo.m_bEnableFlexibleDesign=true;
			pPlate->designInfo.m_hBaseNode=despara.pBaseNode->handle;
			pPlate->designInfo.m_hBasePart=despara.pBaseHoriAngle->handle;
			designinfo.CloneTo(pPlate->designInfo);
		}
		//八字铁腿部斜材
		for(sideI=0;sideI<2;sideI++)
		{
			CLDSLineAngle* pVLegAngle=sideI==0?despara.pLeftLegAngle:despara.pRightLegAngle;
			if(pVLegAngle==NULL)
				break;
			//添加钢板连接杆件信息
			despara.JudgeConnectTipAndWing(pVLegAngle,&ciTipS0E1,&ciWorkWingX0Y1);
			pConnAnglePara=pPlate->designInfo.partList.Add(pVLegAngle->handle);
			pConnAnglePara->m_nClassTypeId=CLS_LINEANGLE;//pVLegAngle->GetClassTypeId();
			pConnAnglePara->angle.bTwoEdge=false;
			if(despara.ciFront0OrSide1==0&&j==0)
				vCutPlaneNormal.Set( 1,0,0);
			else if(despara.ciFront0OrSide1==0&&j==1)
				vCutPlaneNormal.Set(-1,0,0);
			else if(despara.ciFront0OrSide1==1&&j==0)
				vCutPlaneNormal.Set(0,-1,0);
			else if(despara.ciFront0OrSide1==1&&j==1)
				vCutPlaneNormal.Set(0, 1,0);
			if(ciTipS0E1==0)
			{
				pVLegAngle->desStartOdd.m_iOddCalStyle=0;
				pVLegAngle->desStartOdd.m_hRefPart1=despara.pBaseHoriAngle->handle;
				pVLegAngle->desStartOdd.m_hRefPart2=0;
				pVLegAngle->CalStartOddment();
				if(pVLegAngle->CalCutAngleInfo(despara.pBaseNode->xOriginalPos,vCutPlaneNormal,NULL))
					pVLegAngle->m_bUserSpecStartCutAngle=true;
			}
			else if(ciTipS0E1==1)
			{
				pVLegAngle->desEndOdd.m_iOddCalStyle=0;
				pVLegAngle->desEndOdd.m_hRefPart1=despara.pBaseHoriAngle->handle;
				pVLegAngle->desEndOdd.m_hRefPart2=0;
				pVLegAngle->CalEndOddment();
				if(pVLegAngle->CalCutAngleInfo(despara.pBaseNode->xOriginalPos,vCutPlaneNormal,NULL))
					pVLegAngle->m_bUserSpecEndCutAngle=true;
			}
			pConnAnglePara->iFaceNo=1;
			despara.JudgeConnectTipAndWing(pVLegAngle,&ciTipS0E1,&ciWorkWingX0Y1);
			pConnAnglePara->angle.cur_wing_x0_y1=ciWorkWingX0Y1;
			pConnAnglePara->start0_end1=ciTipS0E1;

			jgzj=pVLegAngle->GetZhunJu(ciWorkWingX0Y1+'X');
			g[0]=jgzj.g;
			g[1]=jgzj.g1;
			g[2]=jgzj.g2;
			g[3]=jgzj.g3;
			layout.ciLayoutStyle=0;
			if(ciTipS0E1==0)
				pCurrConnInfo=&pVLegAngle->connectStart;
			else
				pCurrConnInfo=&pVLegAngle->connectEnd;
			int nBoltNum=max(2,pCurrConnInfo->wnConnBoltN);
			uiBoltMd=ciTipS0E1==0?pVLegAngle->connectStart.d:pVLegAngle->connectEnd.d;
			if(LsSpace.M_Diameter!=uiBoltMd)
				GetLsSpace(LsSpace,uiBoltMd);
			layout.cnCount=max(2,(BYTE)pCurrConnInfo->wnConnBoltN);
			layout.ciRows=0;
			layout.ciLayoutStyle=BOLT_LAYOUT::LAYOUT_ONESIDE;
			layout.ciFlowDirection=(ciTipS0E1==0)?BOLT_LAYOUT::FLOW_StoE:BOLT_LAYOUT::FLOW_EtoS;
			layout.ciDatumWing=ciWorkWingX0Y1+'X';
			layout.basepoint.ciPosCalStyle=0;
			listLocations.Empty();
			layout.LayoutBolts(g,&listLocations,LsSpace.EndSpace,LsSpace.SingleRowSpace,LsSpace.doubleRowSpace);
			//提取相应位置的已布螺栓集合
			wiSearchRadius=(WORD)(nBoltNum*LsSpace.SingleRowSpace);
			boltset.Empty();
			existbolts.ReSize(0);
			RetrieveExistBolts(boltset,pVLegAngle,despara.pBaseNode,ciWorkWingX0Y1,wiSearchRadius);
			for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
				existbolts.Append(pBolt);

			for(pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
				pLocation->TestCalBoltPosition(pVLegAngle,despara.pBaseNode);
			mapping.ClearMapping();
			mapping.InitWorkPlaneCS(pVLegAngle,ciWorkWingX0Y1,despara.pBaseNode);
			matchcount=mapping.Mapping(existbolts.pPresetObjs,existbolts.Count,&listLocations);

			for(pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
			{
				if(pLocation->pRelaBolt)
					pBolt=pLocation->pRelaBolt;
				else
				{
					pBolt=(CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
					pBolt->SetLayer(pVLegAngle->layer());
					pBolt->layer(1)='P';
					pBolt->cfgword=pVLegAngle->cfgword;
				}
				pBolt->d=uiBoltMd;
				pBolt->des_base_pos.hPart=pVLegAngle->handle;
				pLocation->WriteParamToBolt(pBolt,pVLegAngle,despara.pBaseNode);
				pBolt->AddL0Thick(pVLegAngle->handle,TRUE,TRUE);
				pBolt->CalGuigeAuto();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pVLegAngle->AppendLsRef(pBolt->GetLsRef());
				pPlate->AppendLsRef(pBolt->GetLsRef());
			}
		}
		for(pBolt=shareboltset.GetFirst();pBolt;pBolt=shareboltset.GetNext())
			pPlate->AppendLsRef(pBolt->GetLsRef());
		pPlate->DesignPlate();
		MIRMSG mirinfo=(despara.ciFront0OrSide1==0)?MIRMSG(1):MIRMSG(2);
		if(MirTaAtom(pPlate, mirinfo,TRUE))
		{
			RELATIVE_OBJECT* pMirRelaObj;
			CLDSPlate* pMirPlate=NULL;
			if(pMirRelaObj=pPlate->GetMirRelaObj(mirinfo))
				pMirPlate=(CLDSPlate*)Ta.Parts.FromHandle(pMirRelaObj->hObj,CLS_PLATE);
			CDesignLjPartPara* pBaseLj=pMirPlate==NULL?NULL:pMirPlate->designInfo.partList.GetValue(pMirPlate->designInfo.m_hBasePart);
			if(pBaseLj)
			{
				CXhChar50 key=CUnifyModelLayer::PLATE::BuildKey(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,pBaseLj->angle.cur_wing_x0_y1);
				CUnifyModelLayer::PLATE* pRegister = m_pDesignerContext->RegisterPlate(pMirPlate->designInfo.m_hBaseNode,
					pMirPlate->designInfo.m_hBasePart,'X'+pBaseLj->angle.cur_wing_x0_y1);
				pRegister->hPlate=pMirPlate->handle;
			}
		}
	}
	return true;
}
void CTowerMultiLegDesigner::CorrectLegAnglesLayout(QUADLEG* pLeg)
{
	bool reverseStartEnd=false;
	GEPOINT vFrontNormal,vSideNormal,vLegAxisX,vLegAxisY;
	if(pLeg->ciLegQuad==1)
	{
		vFrontNormal.Set(0,1,0);
		vSideNormal.Set( 1,0,0);
		vLegAxisX.Set(1,0,0);
		vLegAxisY.Set(0,1,0);
	}
	else if(pLeg->ciLegQuad==2)
	{
		vFrontNormal.Set(-1,0,0);
		vSideNormal.Set(0,1,0);
		vLegAxisX.Set( 0,1,0);
		vLegAxisY.Set(-1,0,0);
	}
	else if(pLeg->ciLegQuad==3)
	{
		vFrontNormal.Set( 1, 0,0);
		vSideNormal.Set(  0,-1,0);
		vLegAxisX.Set( 0,-1,0);
		vLegAxisY.Set( 1, 0,0);
	}
	else //if(pLeg->ciLegQuad==4)
	{
		vFrontNormal.Set(0,-1,0);
		vSideNormal.Set(-1, 0,0);
		vLegAxisX.Set(-1,0,0);
		vLegAxisY.Set(0,-1,0);
	}
	DESIGN_JGWING_NORM* pCurrWingX,*pCurrWingY;
	//布置正面V斜材朝向
	GEPOINT vxInitWingNorm=pLeg->pVFrontAngle->vxWingNorm;	//记录原始朝向
	GEPOINT vyInitWingNorm=pLeg->pVFrontAngle->vyWingNorm;	//记录原始朝向
	reverseStartEnd=pLeg->pVFrontAngle->pEnd==pLeg->pBaseNode?false:true;
	pCurrWingX=!reverseStartEnd?&pLeg->pVFrontAngle->des_norm_x:&pLeg->pVFrontAngle->des_norm_y;
	pCurrWingY=!reverseStartEnd?&pLeg->pVFrontAngle->des_norm_y:&pLeg->pVFrontAngle->des_norm_x;
	pCurrWingY->bSpecific=pCurrWingY->bByOtherWing=false;
	pCurrWingY->hViceJg=pLeg->pPostAngle->handle;
	pCurrWingY->near_norm=vFrontNormal;
	pCurrWingX->bByOtherWing=true;
	pLeg->pVFrontAngle->cal_x_wing_norm(true);
	pLeg->pVFrontAngle->cal_y_wing_norm(true);
	double cosa_xx=vxInitWingNorm*pLeg->pVFrontAngle->vxWingNorm;
	double cosa_xy=vxInitWingNorm*pLeg->pVFrontAngle->vyWingNorm;
	char ciFromXtoWing='X',ciFromYtoWing='Y';
	if(fabs(cosa_xx)>fabs(cosa_xy))
	{
		if(cosa_xx<0)	//X肢里外朝向反向
			ciFromXtoWing='x';
		if(vyInitWingNorm*pLeg->pVFrontAngle->vyWingNorm<0)
			ciFromYtoWing='y';
	}
	else
	{	//X与Y肢调换
		if(cosa_xy>0)
			ciFromXtoWing='Y';
		else
			ciFromXtoWing='y';
		if(vyInitWingNorm*pLeg->pVFrontAngle->vxWingNorm>0)
			ciFromYtoWing='X';
		else
			ciFromYtoWing='x';
	}
	TurnAngleLayout(pLeg->pVFrontAngle,ciFromXtoWing,ciFromYtoWing);
	pLeg->pVFrontAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
	pLeg->pVSideAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
	//布置侧面V斜材朝向
	vxInitWingNorm=pLeg->pVSideAngle->vxWingNorm;	//记录原始朝向
	vyInitWingNorm=pLeg->pVSideAngle->vyWingNorm;	//记录原始朝向
	reverseStartEnd=pLeg->pVSideAngle->pEnd==pLeg->pBaseNode?false:true;
	pCurrWingX=!reverseStartEnd?&pLeg->pVSideAngle->des_norm_x:&pLeg->pVSideAngle->des_norm_y;
	pCurrWingY=!reverseStartEnd?&pLeg->pVSideAngle->des_norm_y:&pLeg->pVSideAngle->des_norm_x;
	pCurrWingX->bSpecific=pCurrWingX->bByOtherWing=false;
	pCurrWingX->hViceJg=pLeg->pPostAngle->handle;
	pCurrWingX->near_norm=vSideNormal;
	pCurrWingY->bByOtherWing=true;
	pLeg->pVSideAngle->cal_x_wing_norm(true);
	pLeg->pVSideAngle->cal_y_wing_norm(true);
	cosa_xx=vxInitWingNorm*pLeg->pVSideAngle->vxWingNorm;
	cosa_xy=vxInitWingNorm*pLeg->pVSideAngle->vyWingNorm;
	ciFromXtoWing='X',ciFromYtoWing='Y';
	if(fabs(cosa_xx)>fabs(cosa_xy))
	{
		if(cosa_xx<0)	//X肢里外朝向反向
			ciFromXtoWing='x';
		if(vyInitWingNorm*pLeg->pVSideAngle->vyWingNorm<0)
			ciFromYtoWing='y';
	}
	else
	{	//X与Y肢调换
		if(cosa_xy>0)
			ciFromXtoWing='Y';
		else
			ciFromXtoWing='y';
		if(vyInitWingNorm*pLeg->pVSideAngle->vxWingNorm>0)
			ciFromYtoWing='X';
		else
			ciFromYtoWing='x';
	}
	TurnAngleLayout(pLeg->pVSideAngle,ciFromXtoWing,ciFromYtoWing);
	POLYGON polygon,polygonSide;
	f3dPoint VertsOfRect[4];
	LINEPARTSET rodset;
	CLDSNode* pVTopNode=NULL, *pVTopSideNode=NULL;
	CLDSLinePart* pTopRod=NULL;
	//bool bStartTipNode,bEndTipNode;
	pVTopNode=pLeg->pVFrontTopNode;
	pVTopSideNode=pLeg->pVSideTopNode;
	//设置正侧面矩形区域
	VertsOfRect[0]=pVTopNode->xOriginalPos;
	VertsOfRect[2]=pLeg->pBaseNode->xOriginalPos;
	if(pLeg->ciLegQuad==1||pLeg->ciLegQuad==4)
	{
		VertsOfRect[1].Set(VertsOfRect[0].x,VertsOfRect[2].y,VertsOfRect[2].z);
		VertsOfRect[3].Set(VertsOfRect[2].x,VertsOfRect[0].y,VertsOfRect[0].z);
	}
	else if(pLeg->ciLegQuad==2||pLeg->ciLegQuad==3)
	{
		VertsOfRect[1].Set(VertsOfRect[2].x,VertsOfRect[0].y,VertsOfRect[2].z);
		VertsOfRect[3].Set(VertsOfRect[0].x,VertsOfRect[2].y,VertsOfRect[0].z);
	}
	polygon.CreatePolygonRgn(VertsOfRect,4);
	VertsOfRect[3]=pVTopSideNode->xOriginalPos;
	VertsOfRect[1]=pLeg->pBaseNode->xOriginalPos;
	if(pLeg->ciLegQuad==1||pLeg->ciLegQuad==4)
	{
		VertsOfRect[0].Set(VertsOfRect[3].x,VertsOfRect[1].y,VertsOfRect[3].z);
		VertsOfRect[2].Set(VertsOfRect[1].x,VertsOfRect[3].y,VertsOfRect[1].z);
	}
	else if(pLeg->ciLegQuad==2||pLeg->ciLegQuad==3)
	{
		VertsOfRect[0].Set(VertsOfRect[1].x,VertsOfRect[3].y,VertsOfRect[3].z);
		VertsOfRect[2].Set(VertsOfRect[3].x,VertsOfRect[1].y,VertsOfRect[1].z);
	}
	polygonSide.CreatePolygonRgn(VertsOfRect,4);
	//布置接腿正侧面辅材角钢
	CLDSLineAngle* pAngle,*pCrossAngle1,*pCrossAngle2;
	CLDSNode* pNode;
	CHashSet<CLDSLineAngle*>hashRevAngles;
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->m_cPosCalType!=CLDSNode::COORD_INTERSECTION||!pLeg->pBaseNode->ModuleTogetherWith(pNode))
			continue;
		bool onFrontPlane=polygon.PtInRgn2(pNode->xOriginalPos)==1;
		bool onSidePlane =polygonSide.PtInRgn2(pNode->xOriginalPos)==1;
		if(!onFrontPlane&&!onSidePlane)
			continue;
		if((pCrossAngle1=(CLDSLineAngle*)Ta.FromPartHandle(pNode->arrRelationPole[0],CLS_LINEANGLE))==NULL)
		{
			logerr.Log("0x%X{%d}交叉节点所依赖的第一根角钢缺失!",pNode->handle,pNode->pointI);
			continue;
		}
		if((pCrossAngle2=(CLDSLineAngle*)Ta.FromPartHandle(pNode->arrRelationPole[1],CLS_LINEANGLE))==NULL)
		{
			logerr.Log("0x%X{%d}交叉节点所依赖的第一根角钢缺失!",pNode->handle,pNode->pointI);
			continue;
		}
		pCrossAngle1->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
		pCrossAngle2->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
		hashRevAngles.SetValue(pCrossAngle1->handle,pCrossAngle1);
		hashRevAngles.SetValue(pCrossAngle2->handle,pCrossAngle2);
		GEPOINT vUpprTip,vDownTip;
		bool reverseStartEnd=pCrossAngle1->xPosEnd.z<pCrossAngle1->xPosStart.z-200;	//由下至上时与模板要调向
		vUpprTip=!reverseStartEnd?pCrossAngle1->xPosStart:pCrossAngle1->xPosEnd;
		vDownTip= reverseStartEnd?pCrossAngle1->xPosStart:pCrossAngle1->xPosEnd;
		bool bPieRod=false;
		if(onFrontPlane)
			bPieRod=(vDownTip-vUpprTip)*vLegAxisX<0;
		else if(onSidePlane)
			bPieRod=(vDownTip-vUpprTip)*vLegAxisY>0;
		//交叉点处正面辅材撇外捺里原则，其余全为吐水外铁
		if((vDownTip-vUpprTip)*vLegAxisX<0)
		{	//撇材
			//pCrossAngle1->des_norm_x
			DESIGN_JGWING_NORM* pCurrWingX=!reverseStartEnd ? &pCrossAngle1->des_norm_x : &pCrossAngle1->des_norm_y;
			DESIGN_JGWING_NORM* pCurrWingY= reverseStartEnd ? &pCrossAngle1->des_norm_x : &pCrossAngle1->des_norm_y;
			pCurrWingX->bByOtherWing=pCurrWingX->bSpecific=pCurrWingY->bSpecific=false;
			pCurrWingX->near_norm=-vLegAxisY;
			pCurrWingX->hViceJg=pLeg->pPostAngle->handle;
			pCurrWingY->bByOtherWing=true;
			if(onSidePlane)
			{
				pCurrWingX->near_norm=GetDiagMirVector(pLeg->ciLegQuad,pCurrWingX->near_norm);
				DESIGN_JGWING_NORM desnorm=*pCurrWingX;
				*pCurrWingX=*pCurrWingY;
				*pCurrWingY=desnorm;
			}
		}
		else
		{	//捺材
			DESIGN_JGWING_NORM* pCurrWingX=!reverseStartEnd ? &pCrossAngle1->des_norm_x : &pCrossAngle1->des_norm_y;
			DESIGN_JGWING_NORM* pCurrWingY= reverseStartEnd ? &pCrossAngle1->des_norm_x : &pCrossAngle1->des_norm_y;
			pCurrWingX->bByOtherWing=pCurrWingX->bSpecific=pCurrWingY->bSpecific=false;
			pCurrWingX->near_norm= vLegAxisY;
			pCurrWingX->hViceJg=pLeg->pPostAngle->handle;
			pCurrWingY->bByOtherWing=true;
			if(onSidePlane)
			{
				pCurrWingX->near_norm=GetDiagMirVector(pLeg->ciLegQuad,pCurrWingX->near_norm);
				DESIGN_JGWING_NORM desnorm=*pCurrWingX;
				*pCurrWingX=*pCurrWingY;
				*pCurrWingY=desnorm;
			}
		}
		reverseStartEnd=pCrossAngle2->xPosEnd.z<pCrossAngle2->xPosStart.z-200;	//由下至上时与模板要调向
		vUpprTip=!reverseStartEnd?pCrossAngle2->xPosStart:pCrossAngle2->xPosEnd;
		vDownTip= reverseStartEnd?pCrossAngle2->xPosStart:pCrossAngle2->xPosEnd;
		bPieRod=false;
		if(onFrontPlane)
			bPieRod=(vDownTip-vUpprTip)*vLegAxisX<0;
		else if(onSidePlane)
			bPieRod=(vDownTip-vUpprTip)*vLegAxisY>0;
		//交叉点处正面辅材撇外捺里原则，其余全为吐水外铁
		if((bPieRod&&onFrontPlane)||(!bPieRod&&onSidePlane))
		{	//正面撇材，侧面捺材 一般默认为外铁
			DESIGN_JGWING_NORM* pCurrWingX=!reverseStartEnd ? &pCrossAngle2->des_norm_x : &pCrossAngle2->des_norm_y;
			DESIGN_JGWING_NORM* pCurrWingY= reverseStartEnd ? &pCrossAngle2->des_norm_x : &pCrossAngle2->des_norm_y;
			pCurrWingX->bByOtherWing=pCurrWingX->bSpecific=pCurrWingY->bSpecific=false;
			pCurrWingX->near_norm=-vLegAxisY;
			pCurrWingX->hViceJg=pLeg->pPostAngle->handle;
			pCurrWingY->bByOtherWing=true;
			if(onSidePlane)
			{
				pCurrWingX->near_norm=GetDiagMirVector(pLeg->ciLegQuad,pCurrWingX->near_norm);
				DESIGN_JGWING_NORM desnorm=*pCurrWingX;
				*pCurrWingX=*pCurrWingY;
				*pCurrWingY=desnorm;
			}
		}
		else
		{	//捺材
			DESIGN_JGWING_NORM* pCurrWingX=!reverseStartEnd ? &pCrossAngle2->des_norm_x : &pCrossAngle2->des_norm_y;
			DESIGN_JGWING_NORM* pCurrWingY= reverseStartEnd ? &pCrossAngle2->des_norm_x : &pCrossAngle2->des_norm_y;
			pCurrWingX->bByOtherWing=pCurrWingX->bSpecific=pCurrWingY->bSpecific=false;
			pCurrWingX->near_norm= vLegAxisY;
			pCurrWingX->hViceJg=pLeg->pPostAngle->handle;
			pCurrWingY->bByOtherWing=true;
			if(onSidePlane)
			{
				pCurrWingX->near_norm=GetDiagMirVector(pLeg->ciLegQuad,pCurrWingX->near_norm);
				DESIGN_JGWING_NORM desnorm=*pCurrWingX;
				*pCurrWingX=*pCurrWingY;
				*pCurrWingY=desnorm;
			}
		}
		pCrossAngle1->cal_x_wing_norm();
		pCrossAngle1->cal_y_wing_norm();
		pCrossAngle1->CalPosition();
		pCrossAngle2->cal_x_wing_norm();
		pCrossAngle2->cal_y_wing_norm();
		pCrossAngle2->CalPosition();
	}
	for(pAngle=(CLDSLineAngle*)Ta.EnumPartFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)Ta.EnumPartNext(CLS_LINEANGLE))
	{
		if(  pAngle->pStart==NULL||pAngle->pEnd==NULL||!pAngle->IsAuxPole()||
			!pLeg->pBaseNode->ModuleTogetherWith(pAngle))
			continue;
		bool onFrontPlane=polygon.LineInRgn2(pAngle->pStart->xOriginalPos,pAngle->pEnd->xOriginalPos)==1;
		bool onSidePlane =polygonSide.LineInRgn2(pAngle->pStart->xOriginalPos,pAngle->pEnd->xOriginalPos)==1;
		if(!onFrontPlane&&!onSidePlane)
			continue;
		if(hashRevAngles.GetValue(pAngle->handle))
			continue;	//交叉辅材已处理
		pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
		//保证辅材均为吐水
		GEPOINT vCurrPlaneNorm=onFrontPlane?vFrontNormal:vSideNormal;
		DESIGN_JGWING_NORM xTempDesNorm;
		double ddx=pAngle->vxWingNorm*vCurrPlaneNorm;
		double ddy=pAngle->vyWingNorm*vCurrPlaneNorm;
		if(fabs(ddx)>fabs(ddy))
		{
			int sign=ddx>0?1:-1;
			if(pAngle->vyWingNorm.z>0.1)
			{
				xTempDesNorm=pAngle->des_norm_x;
				xTempDesNorm.near_norm=sign*vCurrPlaneNorm;
				pAngle->des_norm_x=pAngle->des_norm_y;
				pAngle->des_norm_y=xTempDesNorm;
				pAngle->cal_y_wing_norm(true);
				pAngle->cal_x_wing_norm(true);
				pAngle->CalPosition(true);
				TurnAngleLayout(pAngle,'Y','x');
			}
		}
		else
		{
			int sign=ddy>0?1:-1;
			if(pAngle->vxWingNorm.z>0.1)
			{
				xTempDesNorm=pAngle->des_norm_y;
				xTempDesNorm.near_norm=sign*vCurrPlaneNorm;
				pAngle->des_norm_y=pAngle->des_norm_x;
				pAngle->des_norm_x=xTempDesNorm;
				pAngle->cal_y_wing_norm(true);
				pAngle->cal_x_wing_norm(true);
				pAngle->CalPosition(true);
				TurnAngleLayout(pAngle,'y','X');
			}
		}
		//通过简单定位自动修正腿部正侧面辅材角钢位置（防止因为腿部斜材变更朝向引起的错误）
		int iStartBerCalStyle=pAngle->desStartPos.jgber_cal_style;
		int iEndBerCalStyle=pAngle->desEndPos.jgber_cal_style;
		pAngle->desStartPos.jgber_cal_style=2;
		pAngle->desEndPos.jgber_cal_style=2;
		pAngle->CalPosition();
		if(iStartBerCalStyle!=2)
			pAngle->desStartPos.jgber_cal_style=iStartBerCalStyle;
		if(iEndBerCalStyle!=2)
			pAngle->desEndPos.jgber_cal_style=iEndBerCalStyle;
		/*
		//交叉点处正面辅材撇外捺里原则，其余全为吐水外铁
		bool reverseStartEnd=pAngle->xPosEnd.z<pAngle->xPosStart.z-200;	//由下至上时与模板要调向
		GEPOINT vUpprTip=!reverseStartEnd?pAngle->xPosStart:pAngle->xPosEnd;
		GEPOINT vDownTip= reverseStartEnd?pAngle->xPosStart:pAngle->xPosEnd;
		bool bPieRod=false;
		if(onFrontPlane)
			bPieRod=(vDownTip-vUpprTip)*vLegAxisX<0;
		else if(onSidePlane)
			bPieRod=(vDownTip-vUpprTip)*vLegAxisY>0;
		//交叉点处正面辅材撇外捺里原则，其余全为吐水外铁
		if(bPieRod)
		{	//撇材
			//pCrossAngle1->des_norm_x
			DESIGN_JGWING_NORM* pCurrWingX=!reverseStartEnd ? &pAngle->des_norm_x : &pAngle->des_norm_y;
			DESIGN_JGWING_NORM* pCurrWingY=!reverseStartEnd ? &pAngle->des_norm_y : &pAngle->des_norm_x;
			pCurrWingX->bByOtherWing=pCurrWingX->bSpecific=pCurrWingY->bSpecific=false;
			pCurrWingX->near_norm=-vLegAxisY;
			pCurrWingX->hViceJg=pLeg->pPostAngle->handle;
			pCurrWingY->bByOtherWing=true;
			if(onSidePlane)
			{
				pCurrWingX->near_norm=GetDiagMirVector(pLeg->ciLegQuad,pCurrWingX->near_norm);
				DESIGN_JGWING_NORM desnorm=*pCurrWingX;
				*pCurrWingX=*pCurrWingY;
				*pCurrWingY=desnorm;
			}
		}
		else
		{	//捺材
			DESIGN_JGWING_NORM* pCurrWingX=!reverseStartEnd ? &pAngle->des_norm_x : &pAngle->des_norm_y;
			DESIGN_JGWING_NORM* pCurrWingY=!reverseStartEnd ? &pAngle->des_norm_y : &pAngle->des_norm_x;
			pCurrWingY->bByOtherWing=pCurrWingX->bSpecific=pCurrWingY->bSpecific=false;
			pCurrWingY->near_norm=-vLegAxisY;
			pCurrWingY->hViceJg=pLeg->pPostAngle->handle;
			pCurrWingX->bByOtherWing=true;
			if(onSidePlane)
			{
				pCurrWingY->near_norm=GetDiagMirVector(pLeg->ciLegQuad,pCurrWingY->near_norm);
				DESIGN_JGWING_NORM desnorm=*pCurrWingX;
				*pCurrWingX=*pCurrWingY;
				*pCurrWingY=desnorm;
			}
		}
		pAngle->cal_x_wing_norm();
		pAngle->cal_y_wing_norm();
		pAngle->CalPosition();*/
	}
}
CTowerMultiLegDesigner::CTowerMultiLegDesigner(CTower* pTower/*=NULL*/)
{
	m_pTower= pTower;
	m_pDesignerContext = NULL;
}
void CTowerMultiLegDesigner::RetrieveMultiLegs()
{
	CModelHeightNode* pHeight;
	QUADLEG* pQuad1Leg,*pQuadLeg;
	for(pHeight=modeltree.hashHeights.GetFirst();pHeight;pHeight=modeltree.hashHeights.GetNext())
	{
		for(int i=0;i<24;i++)
		{
			if(pHeight->xarrSubLegs[i].Id==0||pHeight->xarrSubLegs[i].pPostRod==NULL)
				break;
			if(!pHeight->xarrSubLegs[i].pPostRod->IsAngle())
				continue;
			pQuad1Leg=hashQuadLegs.Add(KEY4C(pHeight->Id,pHeight->xarrSubLegs[i].Id,1));
			pQuad1Leg->pPostAngle=(CLDSLineAngle*)pHeight->xarrSubLegs[i].pPostRod;
			pQuad1Leg->pBaseNode=pQuad1Leg->pPostAngle->xPosEnd.z>pQuad1Leg->pPostAngle->xPosStart.z?pQuad1Leg->pPostAngle->pEnd:pQuad1Leg->pPostAngle->pStart;
			//2象限 
			pQuadLeg=hashQuadLegs.Add(KEY4C(pHeight->Id,pHeight->xarrSubLegs[i].Id,2));
			pQuadLeg->pPostAngle=(CLDSLineAngle*)pQuad1Leg->pPostAngle->GetMirRod(MIRMSG(2),1);
			pQuadLeg->pBaseNode=pQuadLeg->pPostAngle->xPosEnd.z>pQuadLeg->pPostAngle->xPosStart.z?pQuadLeg->pPostAngle->pEnd:pQuadLeg->pPostAngle->pStart;
			//3象限 
			pQuadLeg=hashQuadLegs.Add(KEY4C(pHeight->Id,pHeight->xarrSubLegs[i].Id,3));
			pQuadLeg->pPostAngle=(CLDSLineAngle*)pQuad1Leg->pPostAngle->GetMirRod(MIRMSG(1),1);
			pQuadLeg->pBaseNode=pQuadLeg->pPostAngle->xPosEnd.z>pQuadLeg->pPostAngle->xPosStart.z?pQuadLeg->pPostAngle->pEnd:pQuadLeg->pPostAngle->pStart;
			//4象限 
			pQuadLeg=hashQuadLegs.Add(KEY4C(pHeight->Id,pHeight->xarrSubLegs[i].Id,4));
			pQuadLeg->pPostAngle=(CLDSLineAngle*)pQuad1Leg->pPostAngle->GetMirRod(MIRMSG(3),1);
			pQuadLeg->pBaseNode=pQuadLeg->pPostAngle->xPosEnd.z>pQuadLeg->pPostAngle->xPosStart.z?pQuadLeg->pPostAngle->pEnd:pQuadLeg->pPostAngle->pStart;
		}
	}
	//补全接腿信息
	CLDSLineAngle* pAngle=NULL;
	for(pQuadLeg=hashQuadLegs.GetFirst();pQuadLeg;pQuadLeg=hashQuadLegs.GetNext())
	{	//补全V斜材
		CLDSLinePart* pRod;
		for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->m_bVirtualPart||pRod->pStart==NULL||pRod->pEnd==NULL||!pRod->IsAngle())
				continue;
			pAngle=(CLDSLineAngle*)pRod;
			if(!pQuadLeg->pPostAngle->ModuleTogetherWith(pRod)||pAngle==pQuadLeg->pPostAngle||pAngle->group_father_jg_h==pQuadLeg->pPostAngle->handle)
				continue;
			if(pAngle->pStart==pQuadLeg->pBaseNode||pAngle->pEnd==pQuadLeg->pBaseNode)
			{
				if(fabs(pAngle->xPosEnd.x-pAngle->xPosStart.x)>fabs(pAngle->xPosEnd.y-pAngle->xPosStart.y))
				{	//正面腿部斜材
					if(pQuadLeg->keyid.pairs.ciQuad==1||pQuadLeg->keyid.pairs.ciQuad==4)
						pQuadLeg->pVFrontAngle=pAngle;
					else
						pQuadLeg->pVSideAngle=pAngle;
				}
				else
				{	//侧面腿部斜材
					if(pQuadLeg->keyid.pairs.ciQuad==2||pQuadLeg->keyid.pairs.ciQuad==3)
						pQuadLeg->pVFrontAngle=pAngle;
					else
						pQuadLeg->pVSideAngle=pAngle;
				}
			}
			if(pQuadLeg->pVFrontAngle!=NULL&&pQuadLeg->pVSideAngle!=NULL)
				break;
		}
		if(pQuadLeg->pVFrontAngle==NULL||pQuadLeg->pVSideAngle==NULL)
		{
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"error");
			continue;
		}
		//查找正面隔面顶节点
		CLDSLinePart* pTopRod;
		LINEPARTSET rodset;
		if(Ta.SearchSortedColinearRods(pQuadLeg->pVFrontAngle,rodset,NULL,NULL,5))
		{
			if(pQuadLeg->pVFrontAngle->xPosEnd.z>pQuadLeg->pVFrontAngle->xPosStart.z)
				pTopRod=rodset.GetFirst();
			else
				pTopRod=rodset.GetTail();
			if(pTopRod->xPosEnd.z>pTopRod->xPosStart.z)
				pQuadLeg->pVFrontTopNode=pTopRod->pStart;	
			else
				pQuadLeg->pVFrontTopNode=pTopRod->pEnd;
		}
		else
		{
			if(pQuadLeg->pVFrontAngle->xPosEnd.z>pQuadLeg->pVFrontAngle->xPosStart.z)
				pQuadLeg->pVFrontTopNode=pQuadLeg->pVFrontAngle->pStart;
			else
				pQuadLeg->pVFrontTopNode=pQuadLeg->pVFrontAngle->pEnd;
		}
		//查找侧面隔面顶节点
		rodset.Empty();
		if(Ta.SearchSortedColinearRods(pQuadLeg->pVSideAngle,rodset,NULL,NULL,5))
		{
			if(pQuadLeg->pVSideAngle->xPosEnd.z>pQuadLeg->pVSideAngle->xPosStart.z)
				pTopRod=rodset.GetFirst();
			else
				pTopRod=rodset.GetTail();
			if(pTopRod->xPosEnd.z>pTopRod->xPosStart.z)
				pQuadLeg->pVSideTopNode=pTopRod->pStart;	
			else
				pQuadLeg->pVSideTopNode=pTopRod->pEnd;
		}
		else
		{
			if(pQuadLeg->pVSideAngle->xPosEnd.z>pQuadLeg->pVSideAngle->xPosStart.z)
				pQuadLeg->pVSideTopNode=pQuadLeg->pVSideAngle->pStart;
			else
				pQuadLeg->pVSideTopNode=pQuadLeg->pVSideAngle->pEnd;
		}
		//查找接腿角脊线上腿身过渡平面节点
		if(fabs(pQuadLeg->pPostAngle->pStart->xOriginalPos.z-pQuadLeg->pVFrontTopNode->xOriginalPos.z)<0.1)
			pQuadLeg->pPostTopNode=pQuadLeg->pPostAngle->pStart;
		else if(fabs(pQuadLeg->pPostAngle->pEnd->xOriginalPos.z-pQuadLeg->pVFrontTopNode->xOriginalPos.z)<0.1)
			pQuadLeg->pPostTopNode=pQuadLeg->pPostAngle->pEnd;
		else
		{
			CPtInLineCheck check;
			if(pQuadLeg->pPostAngle->pEnd==pQuadLeg->pBaseNode)
				check.SetLine(pQuadLeg->pPostAngle->pStart->xOriginalPos,pQuadLeg->pPostAngle->pEnd->xOriginalPos);
			else
				check.SetLine(pQuadLeg->pPostAngle->pEnd->xOriginalPos,pQuadLeg->pPostAngle->pStart->xOriginalPos);
			for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{
				if(!pNode->IsLegObj()||pNode->m_bVirtualObj||!pQuadLeg->pBaseNode->ModuleTogetherWith(pNode))
					continue;
				if(fabs(pNode->xOriginalPos.z-pQuadLeg->pVFrontTopNode->xOriginalPos.z)>0.1)
					continue;
				bool bOnLine=false;
				double scaleOfLenPos=check.CheckPointEx(pNode->xOriginalPos,&bOnLine,0.1);
				if(!bOnLine)
					continue;
				pQuadLeg->pPostTopNode=pNode;
				break;
			}
		}
	}
}
bool CTowerMultiLegDesigner::DesignAllLegTransitConnPlates(bool bDesignPostK/*=true*/,bool bDesignHoriK/*=true*/)
{
	CXhChar50 szItemName=bDesignPostK ? "腿身连接板K" : "横材中间八字板K";
	DisplayProcess(0, szItemName);
	int index=0, count=modeltree.hashHeights.GetNodeNum();
	CModelHeightNode* pHeight;
	for(pHeight=modeltree.hashHeights.GetFirst();pHeight;pHeight=modeltree.hashHeights.GetNext(),index++)
	{
		int sideI;	//0.正面;1.侧面
		GEPOINT vNearPlaneNormal,vLenStdVec;
		CLDSLineAngle* pAngle=NULL,*pBaseHoriAngle=NULL;
		QUADLEG* pQuadLeg=hashQuadLegs.GetValue(KEY4C(pHeight->Id,pHeight->xarrSubLegs[0].Id,1));
		if(pQuadLeg==NULL)
		{
			logerr.Log("Quadleg lost!");
			continue;
		}
#ifdef __TMP_LOG_PROC_
		logerr.Log("%d", pHeight->Id);
#endif
		//提取1象限角线正侧面两侧腿身连接板
		if(bDesignPostK)
		{
			MULTILEG_POST_CONNPLATE frontPostPara,sidePostPara;
			frontPostPara.ciHeightSerial=sidePostPara.ciHeightSerial=pHeight->Id;
			frontPostPara.ciFront0OrSide1=0;
			sidePostPara.ciFront0OrSide1=1;
			frontPostPara.pBaseNode=sidePostPara.pBaseNode=pQuadLeg->pPostTopNode;
			if (pQuadLeg->pPostTopNode == 0)
				continue;
			//查找角脊线上与塔身连接的那段主材
			if(pQuadLeg->pPostAngle->pStart==pQuadLeg->pPostTopNode||pQuadLeg->pPostAngle->pEnd==pQuadLeg->pPostTopNode)
				pQuadLeg->pUpPostAngle=pQuadLeg->pPostAngle;	//整根腿部主材是一段
			if( pQuadLeg->pPostAngle->pStart->xOriginalPos.z<pQuadLeg->pPostTopNode->xOriginalPos.z+100||
				pQuadLeg->pPostAngle->pEnd->xOriginalPos.z<pQuadLeg->pPostTopNode->xOriginalPos.z+100)
				pQuadLeg->pUpPostAngle=pQuadLeg->pPostAngle;	//整根腿部主材是一段
			GEPOINT vPostLenStdVec=pQuadLeg->pPostAngle->pEnd->xOriginalPos-pQuadLeg->pPostAngle->pStart->xOriginalPos;
			normalize(vPostLenStdVec);
			//查找与角脊线上腿身过渡节点正侧面相连的两根横材
			for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
			{
				if(pRod->m_bVirtualPart||pRod->pStart==NULL||pRod->pEnd==NULL||!pRod->IsAngle())
					continue;
				pAngle=(CLDSLineAngle*)pRod;
				if(!pQuadLeg->pBaseNode->ModuleTogetherWith(pRod)||pAngle==pQuadLeg->pPostAngle||pAngle->group_father_jg_h==pQuadLeg->pPostAngle->handle)
					continue;
				if(pAngle->group_father_jg_h>0)
					continue;	//组合角钢时，只取虚拟组合角钢
				GEPOINT vRayVec;
				if(pAngle->pStart==pQuadLeg->pPostTopNode)
					vRayVec=pAngle->pEnd->xOriginalPos-pAngle->pStart->xOriginalPos;
				else if(pAngle->pEnd==pQuadLeg->pPostTopNode)
					vRayVec=pAngle->pStart->xOriginalPos-pAngle->pEnd->xOriginalPos;
				if(vRayVec.IsZero())
					continue;
				normalize(vRayVec);
				if(fabs(vRayVec.x)<EPS2)
					sidePostPara.pHoriAngle=pQuadLeg->pHoriRidgeSideAngle=pAngle;
				else if(fabs(vRayVec.y)<EPS2)
					frontPostPara.pHoriAngle=pQuadLeg->pHoriRidgeFrontAngle=pAngle;
				else if(pQuadLeg->pUpPostAngle==NULL&&fabs(vRayVec*vPostLenStdVec)>EPS_COS2)
					frontPostPara.pPostAngle=sidePostPara.pPostAngle=pQuadLeg->pUpPostAngle=pAngle;
				else if(vRayVec.z<0&&fabs(vRayVec.x)  >fabs(vRayVec.y)*4)
					frontPostPara.pUpXieAngle=pQuadLeg->pRidgeFrontXieAngle=pAngle;
				else if(vRayVec.z<0&&fabs(vRayVec.x)*4<fabs(vRayVec.y))
					sidePostPara.pUpXieAngle=pQuadLeg->pRidgeSideXieAngle=pAngle;
				else if(vRayVec.z>0&&fabs(vRayVec.x)  >fabs(vRayVec.y)*4)
					frontPostPara.pLegBraceAngle=pAngle;
				else if(vRayVec.z>0&&fabs(vRayVec.x)*4<fabs(vRayVec.y))
					sidePostPara.pLegBraceAngle =pAngle;
				if( pQuadLeg->pUpPostAngle!=NULL&&
					frontPostPara.pHoriAngle!=NULL&&sidePostPara.pHoriAngle!=NULL&&
					frontPostPara.pUpXieAngle!=NULL&&sidePostPara.pUpXieAngle!=NULL&&
					frontPostPara.pLegBraceAngle!=NULL&&sidePostPara.pLegBraceAngle!=NULL)
					break;
			}
			if (frontPostPara.pHoriAngle == NULL)
			{
				logerr.Log("节点0x%X处缺少正面腿身连接横材", pQuadLeg->pPostTopNode->handle);
				continue;
			}
			if (sidePostPara.pHoriAngle == NULL)
			{
				logerr.Log("节点0x%X处缺少侧面腿身连接横材", pQuadLeg->pPostTopNode->handle);
				continue;
			}
			frontPostPara.pPostAngle=sidePostPara.pPostAngle=pQuadLeg->pUpPostAngle;
			for(sideI=0;sideI<2;sideI++)
			{
				MULTILEG_POST_CONNPLATE* pCurrPara=(sideI==0)?&frontPostPara:&sidePostPara;
#ifdef __TMP_LOG_PROC_
				logerr.Log("K plate side %d", sideI);
#endif
				CorrectLegPostKNodeConnPlate(*pCurrPara,true);
			}
		}
		//提取正侧面八字板设计信息
		if(bDesignHoriK)
		{
			MULTILEG_HORI_K_CONNPLATE frontpara,sidepara;
			frontpara.ciHeightSerial=sidepara.ciHeightSerial=pHeight->Id;
			frontpara.ciFront0OrSide1=0;
			sidepara.ciFront0OrSide1=1;
			frontpara.pRightLegAngle=pQuadLeg->pVFrontAngle;
			frontpara.pLeftLegAngle=(CLDSLineAngle*)pQuadLeg->pVFrontAngle->GetMirRod(MIRMSG(2));
			frontpara.pBaseNode=pQuadLeg->pVFrontTopNode;
			sidepara.pLeftLegAngle=pQuadLeg->pVSideAngle;
			sidepara.pRightLegAngle=(CLDSLineAngle*)pQuadLeg->pVSideAngle->GetMirRod(MIRMSG(1));
			sidepara.pBaseNode=pQuadLeg->pVSideTopNode;
			for(sideI=0;sideI<2;sideI++)
			{
				MULTILEG_HORI_K_CONNPLATE* pCurrPara=(sideI==0)?&frontpara:&sidepara;
				pAngle=pCurrPara->pBaseNode->hFatherPart>0?(CLDSLineAngle*)Ta.Parts.FromHandle(pCurrPara->pBaseNode->hFatherPart,CLS_LINEANGLE):NULL;
				GEPOINT sv,ev;
				if(pAngle&&pAngle->pStart&&pAngle->pEnd)
				{
					sv=pAngle->pStart->xOriginalPos-pCurrPara->pBaseNode->xOriginalPos;
					ev=pAngle->pEnd->xOriginalPos-pCurrPara->pBaseNode->xOriginalPos;
				}
				else
					return false;
				if(sideI==0&&fabs(sv.y)+fabs(sv.z)+fabs(ev.y)+fabs(ev.z)<1)//0.01)
					pCurrPara->pBaseHoriAngle=pAngle;
				else if(sideI==1&&fabs(sv.x)+fabs(sv.z)+fabs(ev.x)+fabs(ev.z)<1)//0.01)
					pCurrPara->pBaseHoriAngle=pAngle;
				else
					return false;
				//计算当前工作面近似法线（单线平面法线）
				vLenStdVec=pCurrPara->pLeftLegAngle->pEnd->xOriginalPos-pCurrPara->pLeftLegAngle->pStart->xOriginalPos;
				GEPOINT vHoriVec=sideI==0?GEPOINT(1,0,0):GEPOINT(0,1,0);
				GEPOINT vNormal=vLenStdVec^vHoriVec;
				normalize(vNormal);
				if(sideI==0)
					vNearPlaneNormal=vNormal.y<0?-vNormal:vNormal;
				else if(sideI==1)
					vNearPlaneNormal=vNormal.x<0?-vNormal:vNormal;

				for(CLDSLinePart* pConnRod=Ta.EnumRodFirst();pConnRod;pConnRod=Ta.EnumRodNext())
				{
					if(pConnRod->IsLegObj()||!pCurrPara->pBaseHoriAngle->ModuleTogetherWith(pConnRod))
						continue;
					if((pConnRod->pStart&&pConnRod->pStart==pCurrPara->pBaseNode)||(pConnRod->pEnd&&pConnRod->pEnd==pCurrPara->pBaseNode))
					{
						vLenStdVec=pConnRod->pEnd->xOriginalPos-pConnRod->pStart->xOriginalPos;
						normalize(vLenStdVec);
						if(fabs(vNearPlaneNormal*vLenStdVec)>EPS2)
							continue;
						CLDSNode* pTopNode=pConnRod->pEnd==pCurrPara->pBaseNode?pConnRod->pStart:pConnRod->pEnd;
						if((sideI==0&&pTopNode->xOriginalPos.x<=0)||(sideI==1&&pTopNode->xOriginalPos.y>=0))
							pCurrPara->pLeftUpperAngle=(CLDSLineAngle*)pConnRod;
						else
							pCurrPara->pRightUpperAngle=(CLDSLineAngle*)pConnRod;
					}
					else
						continue;
					if(pCurrPara->pLeftUpperAngle&&pCurrPara->pRightUpperAngle)
						break;
				}
				CorrectLegHoriKNodeConnPlate(*pCurrPara,true);
			}
		}
		DisplayProcess(ftoi(index*100.0/count), szItemName);
	}
	DisplayProcess(100, szItemName);
	return true;
}
struct VEDGE_NODE{
	CLDSNode* pNode;
	double scaleOfLenPos;
	PRESET_ARRAY4<CLDSLinePart*> rayrods;
	VEDGE_NODE(CLDSNode* _pNode=NULL,double _scaleOfLenPos=0)
	{
		pNode=_pNode;
		scaleOfLenPos=_scaleOfLenPos;
	}
};
typedef CLDSLineAngle* CLDSLineAnglePtr;

int LayoutAngleBolts(CLDSLineAngle* pAngle,CLDSNode* pBaseNode,BYTE ciWorkWingX0Y1,UINT uiBoltMd,BOLT_LAYOUT& layout,BOLTSET& boltset)
{
	CTower* m_pTower=&Ta;
	JGZJ jgzj=pAngle->GetZhunJu(ciWorkWingX0Y1+'X');
	long g[4]={jgzj.g,jgzj.g1,jgzj.g2,jgzj.g3};
	CLDSBolt* pBolt;
	CConnectInfo* pCurrConnInfo=(layout.ciTipS0E1==0)?&pAngle->connectStart:&pAngle->connectEnd;
	if(uiBoltMd==0)
		uiBoltMd=pCurrConnInfo->d;
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,uiBoltMd);
	if(layout.cnCount==0)
		layout.cnCount=max(1,(BYTE)pCurrConnInfo->wnConnBoltN);
	if(pBaseNode==NULL&&(layout.ciTipS0E1==0||layout.ciTipS0E1==1))
	{
		layout.ciLayoutStyle=BOLT_LAYOUT::LAYOUT_ONESIDE;
		layout.ciFlowDirection=layout.ciTipS0E1==0?BOLT_LAYOUT::FLOW_StoE:BOLT_LAYOUT::FLOW_EtoS;
	}
	//layout.ciRows=0;
	//layout.ciLayoutStyle=BOLT_LAYOUT::LAYOUT_ONESIDE;
	//layout.ciDirection=ciTipS0E1;
	layout.ciDatumWing=ciWorkWingX0Y1+'X';
	if(pBaseNode==NULL)
		layout.basepoint.ciPosCalStyle=0;
	else
		layout.basepoint.ciPosCalStyle=1;
	BOLT_LOCATION* pLocation;
	CXhSimpleList<BOLT_LOCATION> listLocations;
	layout.LayoutBolts(g,&listLocations,LsSpace.EndSpace,LsSpace.SingleRowSpace,LsSpace.doubleRowSpace);
	boltset.Empty();
	//提取相应位置的已布螺栓集合
	/*
	WORD wiSearchRadius=(WORD)(nBoltNum*LsSpace.SingleRowSpace);
	existbolts.ReSize(0);
	RetrieveExistBolts(boltset,pVLegAngle,despara.pBaseNode,ciWorkWingX0Y1,wiSearchRadius);
	for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
		existbolts.Append(pBolt);

	for(pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
		pLocation->TestCalBoltPosition(pVLegAngle,despara.pBaseNode);
	mapping.ClearMapping();
	mapping.InitWorkPlaneCS(pVLegAngle,ciWorkWingX0Y1,despara.pBaseNode);
	matchcount=mapping.Mapping(existbolts.pPresetObjs,existbolts.Count,&listLocations);
	*/
	for(pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
	{
		if(pLocation->pRelaBolt)
			pBolt=pLocation->pRelaBolt;
		else
		{
			pBolt=(CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
			pBolt->SetLayer(pAngle->layer());
			pBolt->cfgword=pAngle->cfgword;
		}
		pBolt->d=uiBoltMd;
		pBolt->des_base_pos.hPart=pAngle->handle;
		pLocation->WriteParamToBolt(pBolt,pAngle,pBaseNode);
		pBolt->AddL0Thick(pAngle->handle,TRUE,TRUE);
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		boltset.append(pBolt);
		pAngle->AppendLsRef(pBolt->GetLsRef());
	}
	return boltset.GetNodeNum();
}

CLDSPlate* CTowerMultiLegDesigner::DesignVPlate(CLDSNode* pBaseNode,CLDSLineAngle* pVBaseAngle,char ciWorkWingX0Y1,CLDSLinePartPtr* pConnAngles,int count)
{
	//1、自动计算V面板的厚度及螺栓信息
	WORD wiPlateThick = 0, wiBaseBoltNum = 2, wiBoltMd = 16;
	for (int i = 0; i < count; i++)
	{
		wiPlateThick = max(wiPlateThick, (WORD)pConnAngles[i]->size_thick);
		if (pConnAngles[i]->pStart == pBaseNode)
		{
			if (pConnAngles[i]->connectStart.d > wiBoltMd)
				wiBoltMd = (WORD)pConnAngles[i]->connectStart.d;
			if (pConnAngles[i]->connectStart.wnConnBoltN*1.5 > wiBaseBoltNum)
				wiBaseBoltNum = ftoi(pConnAngles[i]->connectStart.wnConnBoltN*1.5);
		}
		else if (pConnAngles[i]->pEnd == pBaseNode)
		{
			if (pConnAngles[i]->connectEnd.d > wiBoltMd)
				wiBoltMd = (WORD)pConnAngles[i]->connectEnd.d;
			if (pConnAngles[i]->connectEnd.wnConnBoltN*1.5 > wiBaseBoltNum)
				wiBaseBoltNum = ftoi(pConnAngles[i]->connectEnd.wnConnBoltN*1.5);
		}
	}
	wiPlateThick += 2;
	if (wiPlateThick % 2 > 0)
		wiPlateThick += 1;
	if (gxModelGuide.multileg.siSpecThick > 1)	//外部指定板厚
		wiPlateThick = gxModelGuide.multileg.siSpecThick;
	GEPOINT vBaseRodLen = pVBaseAngle->pEnd->xOriginalPos - pVBaseAngle->pStart->xOriginalPos;
	GEPOINT vXieRodLen = pConnAngles[0]->pEnd->xOriginalPos - pConnAngles[0]->pStart->xOriginalPos;
	GEPOINT vPlaneNormal = vBaseRodLen ^ vXieRodLen;
	normalize(vPlaneNormal);
	if (vPlaneNormal.z < 0)
		vPlaneNormal *= -1;
	//2、初始化V面板设计参数
	CDesignLjPara designer;
	designer.m_bEnableFlexibleDesign=true;
	designer.m_hBaseNode=pBaseNode->handle;
	designer.m_hBasePart=pVBaseAngle->handle;
	designer.iFaceType = 2;			//双面板
	designer.iProfileStyle0123 = gxModelGuide.multileg.ciProfileSchema;
	//基点设计信息
	designer.origin.datum_pos_style=2;
	designer.origin.des_para.RODNODE.hNode=pBaseNode->handle;
	designer.origin.des_para.RODNODE.hRod=pVBaseAngle->handle;
	designer.origin.des_para.RODNODE.offset_wing=ciWorkWingX0Y1;
	designer.origin.des_para.RODNODE.wing_offset_style = 0;
	if(gxModelGuide.multileg.blVPlateInside)	//贴于里皮
		CXhString(designer.origin.des_para.RODNODE.norm_offset,51,"-0x%X,-%d",pVBaseAngle->handle,wiPlateThick);
	double fTipNodeLenOffset=0;
	if(pBaseNode==pVBaseAngle->pStart)
	{
		designer.origin.des_para.RODNODE.direction=0;
		designer.origin.des_para.RODNODE.len_offset_dist=-pVBaseAngle->startOdd()+90;
		fTipNodeLenOffset = designer.origin.des_para.RODNODE.len_offset_dist;
	}
	else if(pBaseNode==pVBaseAngle->pEnd)
	{
		designer.origin.des_para.RODNODE.direction=1;
		designer.origin.des_para.RODNODE.len_offset_dist=-pVBaseAngle->endOdd()+90;
		fTipNodeLenOffset=-designer.origin.des_para.RODNODE.len_offset_dist;
	}
	//法向设计信息
	designer.norm.norm_style=1;
	designer.norm.hVicePart=pVBaseAngle->handle;
	designer.norm.norm_wing=ciWorkWingX0Y1;
	//火曲设计信息
	designer.huoqufacenorm[0].norm_style=2;
	designer.huoqufacenorm[0].hVicePart=pVBaseAngle->handle;
	designer.huoqufacenorm[0].hCrossPart=pConnAngles[0]->handle;
	designer.huoqufacenorm[0].nearVector=vPlaneNormal;
	designer.huoquline_start[0].datum_pos_style=2;
	designer.huoquline_start[0].des_para.RODNODE.hNode=pBaseNode->handle;
	designer.huoquline_start[0].des_para.RODNODE.hRod=pVBaseAngle->handle;
	designer.huoquline_start[0].des_para.RODNODE.offset_wing=ciWorkWingX0Y1;
	designer.huoquline_start[0].des_para.RODNODE.wing_offset_style=4;
	designer.huoquline_start[0].des_para.RODNODE.wing_offset_dist = pVBaseAngle->size_wide + gxModelGuide.multileg.siBendLineSpace;
	designer.huoquline_start[0].des_para.RODNODE.len_offset_dist=100;
	designer.huoquline_start[0].CloneParaTo(&designer.huoquline_end[0]);
	designer.huoquline_end[0].des_para.RODNODE.len_offset_dist=-100;
	//连接杆件设计信息
	CDesignLjPartPara* pLjRodPara = designer.partList.Add(pVBaseAngle->handle);
	pLjRodPara->m_nClassTypeId = CLS_LINEANGLE;
	pLjRodPara->pCurLinePart = pVBaseAngle;
	pLjRodPara->iFaceNo = 1;
	pLjRodPara->start0_end1 = 2;
	pLjRodPara->angle.bTwoEdge = true;
	pLjRodPara->angle.cur_wing_x0_y1 = ciWorkWingX0Y1;
	CLDSLineAngle* pPrimaryAngle = NULL;	//用于计算射线杆件正负头
	for (i = 0; i < count; i++)
	{
		if (!pConnAngles[i]->IsAngle())
			continue;
		CLDSLineAngle* pAngle = (CLDSLineAngle*)pConnAngles[i];
		if (pPrimaryAngle == NULL)
			pPrimaryAngle = pAngle;
		else if (fabs(pAngle->pStart->xOriginalPos.z - pAngle->pEnd->xOriginalPos.z) < 0.1)
			pPrimaryAngle = pAngle;
		else if (pAngle->GetLength() > pPrimaryAngle->GetLength())
			pPrimaryAngle = pAngle;
		BYTE ciLinkS0E1 = (pAngle->pStart == pBaseNode) ? 0 : 1;
		double dx = vPlaneNormal * pAngle->vxWingNorm;
		double dy = vPlaneNormal * pAngle->vyWingNorm;
		BYTE ciRayWorkWingX0Y1 = fabs(dx) > fabs(dy) ? 0 : 1;
		//
		pLjRodPara = designer.partList.Add(pConnAngles[i]->handle);
		pLjRodPara->m_nClassTypeId = CLS_LINEANGLE;
		pLjRodPara->pCurLinePart = pAngle;
		pLjRodPara->iFaceNo = 2;
		pLjRodPara->start0_end1 = ciLinkS0E1;
		pLjRodPara->angle.bTwoEdge = false;
		pLjRodPara->angle.cur_wing_x0_y1 = ciRayWorkWingX0Y1;
	}
	//3、布置杆件上螺栓，更新杆件位置
	BOLTSET boltset, sumBoltSet;
	for(pLjRodPara=designer.partList.GetFirst();pLjRodPara;pLjRodPara=designer.partList.GetNext())
	{
		if(pLjRodPara->pCurLinePart==NULL)
			continue;
		if (pLjRodPara->pCurLinePart == pVBaseAngle)
		{	//基准杆件
			BOLT_LAYOUT layout;
			layout.ciLayoutStyle = BOLT_LAYOUT::LAYOUT_DUALSIDE;
			layout.cnCount = (BYTE)wiBaseBoltNum;
			layout.ciTipS0E1 = 2;
			layout.ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
			LayoutAngleBolts(pVBaseAngle, pBaseNode, ciWorkWingX0Y1, wiBoltMd, layout, boltset);
			for (CLDSBolt* pBolt = boltset.GetFirst(); pBolt; pBolt = boltset.GetNext())
			{
				if (pBolt->des_base_pos.datumPoint.datum_pos_style == 2)
					pBolt->des_base_pos.datumPoint.des_para.RODNODE.len_offset_dist = fTipNodeLenOffset;
				sumBoltSet.append(pBolt);
			}
		}
		else   //射线杆件
		{	//调整射线角钢定位参数
			CLDSLineAngle* pRayJg = (CLDSLineAngle*)pLjRodPara->pCurLinePart;
			double dx = vPlaneNormal * pRayJg->vxWingNorm;
			double dy = vPlaneNormal * pRayJg->vyWingNorm;
			BYTE ciRayWorkWingX0Y1 = fabs(dx) > fabs(dy) ? 0 : 1;
			char cRayWorkWingXorY = ciRayWorkWingX0Y1 + 'X';	//先默认为里铁
			if (cRayWorkWingXorY == 'X'&&dx < 0)
				cRayWorkWingXorY = 'x';	//外铁
			else if (cRayWorkWingXorY == 'Y'&&dy < 0)
				cRayWorkWingXorY = 'y';	//外铁
			BYTE ciLinkS0E1 = (pRayJg->pStart == pBaseNode) ? 0 : 1;
			CMultiOffsetPos* pDesPosPara = (ciLinkS0E1 == 0) ? &pRayJg->desStartPos : &pRayJg->desEndPos;
			pDesPosPara->datum_jg_h = pVBaseAngle->handle;
			pDesPosPara->len_offset_dist = fTipNodeLenOffset;
			pDesPosPara->jgber_cal_style = 0;
			if (gxModelGuide.multileg.ciTouchLineType == 0)
				pDesPosPara->spatialOperationStyle = 8 + ciWorkWingX0Y1;
			else if(gxModelGuide.multileg.ciTouchLineType==1)
			{
				pDesPosPara->spatialOperationStyle = 13 + ciWorkWingX0Y1;
				pDesPosPara->offset_zhun = gxModelGuide.multileg.siBendLineG;	
			}
			if (gxModelGuide.multileg.blVPlateInside)
				pDesPosPara->datum_offset_dist = -pVBaseAngle->size_thick - wiPlateThick;
			if (cRayWorkWingXorY == 'x')
				pDesPosPara->wing_x_offset.SetOffset(-wiPlateThick);
			else if (cRayWorkWingXorY == 'y')
				pDesPosPara->wing_y_offset.SetOffset(-wiPlateThick);
			pRayJg->CalPosition(true);
			//更新正负头
			DESIGNODDMENT* pDesOdd = (ciLinkS0E1 == 0) ? &pRayJg->desStartOdd : &pRayJg->desEndOdd;
			pDesOdd->m_iOddCalStyle = 0;
			pDesOdd->m_hRefPart1 = pVBaseAngle->handle;
			if (pRayJg != pPrimaryAngle)
				pDesOdd->m_hRefPart2 = pPrimaryAngle->handle;
			if (ciLinkS0E1 == 0)
				pRayJg->CalStartOddment();
			else
				pRayJg->CalEndOddment();
			//布置射线杆件螺栓
			BOLT_LAYOUT layout;
			layout.ciLayoutStyle = BOLT_LAYOUT::LAYOUT_ONESIDE;
			layout.ciTipS0E1 = ciLinkS0E1;
			layout.cnCount = 0;
			boltset.Empty();
			LayoutAngleBolts(pRayJg, NULL, ciRayWorkWingX0Y1, 0, layout, boltset);
			for (CLDSBolt* pBolt = boltset.GetFirst(); pBolt; pBolt = boltset.GetNext())
				sumBoltSet.append(pBolt);
		}
	}
	//添加钢板
	CLDSPlate* pPlate = NULL;
	CUnifyModelLayer::PLATE* pRegister = NULL;
	if(m_pDesignerContext)
		pRegister = m_pDesignerContext->RegisterPlate(pBaseNode->handle, pVBaseAngle->handle, 'X' + ciWorkWingX0Y1, 0);
	if (pRegister!=NULL&&pRegister->hPlate > 0x20)
		pPlate = (CLDSPlate*)Ta.Parts.FromHandle(pRegister->hPlate, CLS_PLATE);
	else
	{
		pPlate = (CLDSPlate*)Ta.Parts.append(CLS_PLATE);
		if(pRegister!=NULL)
			pRegister->hPlate = pPlate->handle;
	}
	pPlate->SetLayer("LPG");
	pPlate->layer(0) = pVBaseAngle->Layer(0);
	pPlate->cfgword = pVBaseAngle->cfgword;
	pPlate->face_N = 2;
	pPlate->huoqufacenorm[0] = vPlaneNormal;
	designer.CloneTo(pPlate->designInfo);
	for (CLDSBolt* pBolt = sumBoltSet.GetFirst(); pBolt; pBolt = sumBoltSet.GetNext())
	{
		pPlate->AppendLsRef(pBolt->GetLsRef());
		pBolt->CalGuigeAuto();
		pBolt->correct_pos();
	}
	pPlate->DesignPlate();
	return pPlate;
}
bool CTowerMultiLegDesigner::DesignLegVPlates(QUADLEG* pQuadLeg)
{
	f3dPoint vertices[3];
	vertices[0]=pQuadLeg->pBaseNode->xOriginalPos;
	vertices[1]=pQuadLeg->pVSideTopNode->xOriginalPos;
	vertices[2]=pQuadLeg->pVFrontTopNode->xOriginalPos;
	GECS wcs;	//V面工作坐标系
	wcs.origin=pQuadLeg->pBaseNode->xOriginalPos;
	wcs.axis_x=pQuadLeg->pVFrontTopNode->xOriginalPos-pQuadLeg->pVSideTopNode->xOriginalPos;
	wcs.axis_y=pQuadLeg->pVSideTopNode->xOriginalPos-pQuadLeg->pBaseNode->xOriginalPos;
	wcs.axis_z=wcs.axis_x^wcs.axis_y;
	normalize(wcs.axis_x);
	normalize(wcs.axis_z);
	if(wcs.axis_z.z<0)
		wcs.axis_z*=-1;
	wcs.axis_y=wcs.axis_z^wcs.axis_x;
	POLYGON triangle;
	triangle.CreatePolygonRgn(vertices,3);
	CPtInLineCheck leftEdge (pQuadLeg->pBaseNode->xOriginalPos,pQuadLeg->pVSideTopNode->xOriginalPos);
	CPtInLineCheck rightEdge(pQuadLeg->pBaseNode->xOriginalPos,pQuadLeg->pVFrontTopNode->xOriginalPos);
	//GEPOINT vl(pQuadLeg->pVFrontAngle->xPosStart-pQuadLeg->pVFrontAngle->xPosEnd);
	//GEPOINT vr(pQuadLeg->pVSideAngle->xPosStart-pQuadLeg->pVSideAngle->xPosEnd);
	GEPOINT vPlaneNormal=rightEdge.LineStdVec()^leftEdge.LineStdVec();
	normalize(vPlaneNormal);
	if(vPlaneNormal.z<0)
		vPlaneNormal*=-1;
	CLDSNode* pNode;
	SmartRodPtr pRod;
	CHashList<VEDGE_NODE> hashLeftEdgeNodes,hashRightEdgeNodes;	//从V面底面向上看分左右
	hashLeftEdgeNodes.SetValue(pQuadLeg->pVSideTopNode->handle,VEDGE_NODE(pQuadLeg->pVSideTopNode,1));
	hashRightEdgeNodes.SetValue(pQuadLeg->pVFrontTopNode->handle,VEDGE_NODE(pQuadLeg->pVFrontTopNode,1));
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode==pQuadLeg->pBaseNode||!pNode->IsLegObj()||!pQuadLeg->pPostAngle->ModuleTogetherWith(pNode)||pNode->m_bVirtualObj)
			continue;
		bool online=false;
		double scaleOfLenPos=leftEdge.CheckPointEx(pNode->xOriginalPos,&online,1);
		if(online&&scaleOfLenPos>=0&&scaleOfLenPos<=1)
		{
			hashLeftEdgeNodes.SetValue(pNode->handle,VEDGE_NODE(pNode,scaleOfLenPos));
			continue;
		}
		scaleOfLenPos=rightEdge.CheckPointEx(pNode->xOriginalPos,&online,1);
		if(online&&scaleOfLenPos>=0&&scaleOfLenPos<=1)
			hashRightEdgeNodes.SetValue(pNode->handle,VEDGE_NODE(pNode,scaleOfLenPos));
	}
	VEDGE_NODE* pEdgeNode;
	for(pRod=Ta.EnumRodFirst();pRod.IsHasPtr();pRod=Ta.EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->m_bVirtualPart||!pRod->IsLegObj())
			continue;
		if(pRod==pQuadLeg->pVFrontAngle||pRod==pQuadLeg->pVSideAngle)
			continue;	//TODO:未考虑到V斜材分段情况
		if(!pQuadLeg->pPostAngle->ModuleTogetherWith(pRod)||!pRod->IsAngle())
			continue;
		if(triangle.LineInRgn2(pRod->pStart->xOriginalPos,pRod->pEnd->xOriginalPos)!=1)
			continue;
		//修正V面内斜辅材以使其肢朝向尽可能符合吐水及左侧撇外捺里右侧撇里捺外的布置原则
		GEPOINT xTipStart=wcs.TransPToCS(pRod->pStart->xOriginalPos);
		GEPOINT xTipEnd  =wcs.TransPToCS(pRod->pEnd->xOriginalPos);
		GEPOINT vLenVec=xTipEnd - xTipStart;
		normalize(vLenVec);
		char ciSide='L';	//'A'跨越中线,'L'中线左侧'R'中线右侧
		char ciStroke='H';	//'H'横，'P'撇,'N'捺
		if(xTipStart.x<0&&xTipEnd.x<0)
			ciSide='L';
		else if(xTipStart.x>0&&xTipEnd.x>0)
			ciSide='R';
		if(vLenVec.y<EPS2)	//水平材
			ciStroke='H';
		else if((vLenVec.y>0&&vLenVec.x>0)||(vLenVec.y<0&&vLenVec.x<0))
			ciStroke='P';
		else
			ciStroke='N';
		DESIGN_JGWING_NORM desNormX,desNormY;
		WING_OFFSET_STYLE xWingOffset,yWingOffset;
		xWingOffset.SetOffset(0);
		yWingOffset.SetOffset(0,0);
		if(ciStroke=='H')
		{
			desNormX.hViceJg=pQuadLeg->pVSideAngle->handle;
			desNormX.near_norm=wcs.axis_z;
			desNormX.bByOtherWing=desNormX.bSpecific=false;
			desNormY.bByOtherWing=true;
			desNormY.bSpecific=false;
			pRod.pAngle->des_norm_x=(vLenVec.x>0)?desNormX:desNormY;
			pRod.pAngle->des_norm_y=(vLenVec.x>0)?desNormY:desNormX;
			pRod.pAngle->desStartPos.wing_x_offset=(vLenVec.x>0)?xWingOffset:yWingOffset;
			pRod.pAngle->desEndPos.wing_x_offset=(vLenVec.x>0)?xWingOffset:yWingOffset;
			pRod.pAngle->desStartPos.wing_y_offset=(vLenVec.x>0)?yWingOffset:xWingOffset;
			pRod.pAngle->desEndPos.wing_y_offset=(vLenVec.x>0)?yWingOffset:xWingOffset;
		}
		else if((ciSide=='L'&&ciStroke=='P')||(ciSide=='R'&&ciStroke=='N'))
		{	//左侧撇外捺里右侧撇里捺外
			desNormX.hViceJg=pQuadLeg->pVSideAngle->handle;
			desNormX.near_norm=-wcs.axis_z;
			desNormX.bByOtherWing=desNormX.bSpecific=false;
			desNormY.bByOtherWing=true;
			desNormY.bSpecific=false;
			pRod.pAngle->des_norm_x=(vLenVec.y<0)?desNormX:desNormY;
			pRod.pAngle->des_norm_y=(vLenVec.y<0)?desNormY:desNormX;
			pRod.pAngle->desStartPos.wing_x_offset=(vLenVec.y<0)?xWingOffset:yWingOffset;
			pRod.pAngle->desEndPos.wing_x_offset  =(vLenVec.y<0)?xWingOffset:yWingOffset;
			pRod.pAngle->desStartPos.wing_y_offset=(vLenVec.y<0)?yWingOffset:xWingOffset;
			pRod.pAngle->desEndPos.wing_y_offset  =(vLenVec.y<0)?yWingOffset:xWingOffset;
		}
		else if((ciSide=='L'&&ciStroke=='N')||(ciSide=='R'&&ciStroke=='P'))
		{	//左侧撇外捺里右侧撇里捺外
			desNormX.hViceJg=pQuadLeg->pVSideAngle->handle;
			desNormX.near_norm=wcs.axis_z;
			desNormX.bByOtherWing=desNormX.bSpecific=false;
			desNormY.bByOtherWing=true;
			desNormY.bSpecific=false;
			pRod.pAngle->des_norm_x=(vLenVec.y<0)?desNormX:desNormY;
			pRod.pAngle->des_norm_y=(vLenVec.y<0)?desNormY:desNormX;
			pRod.pAngle->desStartPos.wing_x_offset=(vLenVec.y<0)?xWingOffset:yWingOffset;
			pRod.pAngle->desEndPos.wing_x_offset  =(vLenVec.y<0)?xWingOffset:yWingOffset;
			pRod.pAngle->desStartPos.wing_y_offset=(vLenVec.y<0)?yWingOffset:xWingOffset;
			pRod.pAngle->desEndPos.wing_y_offset  =(vLenVec.y<0)?yWingOffset:xWingOffset;
		}
		pRod.pAngle->cal_x_wing_norm(true);
		pRod.pAngle->cal_y_wing_norm(true);
		//V面斜辅材X/Y对称时里外铁翻转
		pRod.pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_INVERT;
		if((pEdgeNode=hashLeftEdgeNodes.GetValue(pRod->pStart->handle))!=NULL)
			pEdgeNode->rayrods.Append(pRod);
		else if((pEdgeNode=hashRightEdgeNodes.GetValue(pRod->pStart->handle))!=NULL)
			pEdgeNode->rayrods.Append(pRod);
		if((pEdgeNode=hashLeftEdgeNodes.GetValue(pRod->pEnd->handle))!=NULL)
			pEdgeNode->rayrods.Append(pRod);
		else if((pEdgeNode=hashRightEdgeNodes.GetValue(pRod->pEnd->handle))!=NULL)
			pEdgeNode->rayrods.Append(pRod);
		//else	//辅材
	}
	int hits=0;
	for(pEdgeNode=hashLeftEdgeNodes.GetFirst();pEdgeNode;pEdgeNode=hashLeftEdgeNodes.GetNext())
	{
		if(pEdgeNode->rayrods.Count==0)
			continue;
		BYTE ciWorkWingX0Y1=(pQuadLeg->pVSideAngle->vxWingNorm*vPlaneNormal>0)?0:1;
		if(DesignVPlate(pEdgeNode->pNode,pQuadLeg->pVSideAngle,ciWorkWingX0Y1,pEdgeNode->rayrods.pPresetObjs,pEdgeNode->rayrods.Count))
			hits++;
	}
	for(pEdgeNode=hashRightEdgeNodes.GetFirst();pEdgeNode;pEdgeNode=hashRightEdgeNodes.GetNext())
	{
		if(pEdgeNode->rayrods.Count==0)
			continue;
		BYTE ciWorkWingX0Y1=(pQuadLeg->pVFrontAngle->vxWingNorm*vPlaneNormal>0)?0:1;
		if(DesignVPlate(pEdgeNode->pNode,pQuadLeg->pVFrontAngle,ciWorkWingX0Y1,pEdgeNode->rayrods.pPresetObjs,pEdgeNode->rayrods.Count))
			hits++;
	}
	return hits>0;
}

#include "MainFrm.h"
#include "CmdLineDlg.h"
//TODO:批量设计多接腿还需要完善以下内容：
//1.设计录入V面板设计参数UI
//2.设计塔脚板设计参数UI
//3.设计向导式整体设计流程命令行UI设计
bool CTowerMultiLegDesigner::DesignAllSubLegs(CTower* pTower, char cSubConnDesignType/* = 0*/, CIntelliModelDesigner* pDesignerContext/* = NULL*/)
{
	m_pTower=pTower;
	m_pDesignerContext = pDesignerContext;
#ifdef __RAPID_ROUGH_3DMODEL_
	CCmdLineDlg* pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	static char cDefaultType = 'L';
	if (cSubConnDesignType != 0)
		cDefaultType = cSubConnDesignType;
	CString cmdStr,cmdNameStr="选择任务，[清除所有腿部连接(&E)/修正斜辅材方向(L)/腿身连接板K(1)/横材中间八字板K(2)/V面板(V)/塔脚板(B)]<L>:";
	if(cSubConnDesignType==0)
		pCmdLine->FillCmdLine(cmdNameStr,"");
	while(cSubConnDesignType==0)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"E|L|1|2|V|B"))
		{
			pCmdLine->m_bGettingStrFromCmdLine=FALSE;
			return false;
		}
		pCmdLine->m_bGettingStrFromCmdLine=FALSE;
		if(cmdStr.CompareNoCase("L")==0)
			cDefaultType='L';
		else if(cmdStr.CompareNoCase("E")==0)
			cDefaultType='E';
		else if(cmdStr.CompareNoCase("1")==0)
			cDefaultType='1';
		else if(cmdStr.CompareNoCase("2")==0)
			cDefaultType='2';
		else if(cmdStr.CompareNoCase("V")==0)
			cDefaultType='V';
		else if(cmdStr.CompareNoCase("B")==0)
			cDefaultType='B';
		else if(cmdStr.GetLength()>0)
		{
			pCmdLine->FillCmdLine(cmdNameStr,"");
			continue;
		}
		cSubConnDesignType = cDefaultType;
		break;
	}

	CUndoOperObject undo(&Ta,true);
	if (modeltree.hashSegments.Count == 0)
	{	//初始化模型树
		CIntelliModelDesigner intelli;
		if (pDesignerContext == NULL)
			pDesignerContext = &intelli;
		COLINEAR_TOLERANCE tolerance(25);//setdlg.m_fTolerance,setdlg.m_fToleranceTangent);
		pDesignerContext->RecognizeSlopes(tolerance.distance);	//必须置于CombineDiscreteRodMembers之后，否则会因合并杆件导致intelli中部分句柄失效 wjh-2015.12.28
		modeltree.SetBelongTower(pTower);
		pDesignerContext->RetrievedTowerModeTree(modeltree);
	}
	//1.提取所有接腿
	QUADLEG* pQuadLeg;
	RetrieveMultiLegs();
	if(cDefaultType=='E')
	{
		console.DispFoundationSet.Empty();
		Ta.Foundations.Empty();
		for(SmartPartPtr pSmartPart=Ta.EnumPartFirst();pSmartPart.blValid;pSmartPart=Ta.EnumPartNext())
		{
			if(!pSmartPart->IsLegObj())
				continue;
			CLsRef* pLsRef;
			CLsRefList* pLsRefList=NULL;
			if(pSmartPart->IsLinePart())
				pLsRefList=pSmartPart.pRod->GetLsRefList();
			else if(pSmartPart->IsPlate())
				pLsRefList=pSmartPart.pBasicPlate->GetLsRefList();
			if(pLsRefList==NULL)
				continue;
			for(pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			{
				if(pLsRef->pBolt->IsLegObj())
					pLsRefList->DeleteCursor();
			}
			pLsRefList->Clean();
		}
		for(SmartPartPtr pSmartPart=Ta.EnumPartFirst();pSmartPart.blValid;pSmartPart=Ta.EnumPartNext())
		{
			if(!pSmartPart->IsLegObj())
				continue;
			if(pSmartPart->IsLinePart()&&pSmartPart.pRod->pStart!=NULL&&pSmartPart.pRod->pEnd!=NULL)
				continue;
			if(pSmartPart->IsPlate()||pSmartPart->IsBolt())
				console.DeletePart(pSmartPart->handle);
		}
		AfxMessageBox("所有腿部连接已清除完成");
		return true;
	}
	if(cDefaultType=='L')
	{	//2.修正各接腿正侧面斜辅材角钢摆放方式
		for(pQuadLeg=hashQuadLegs.GetFirst();pQuadLeg;pQuadLeg=hashQuadLegs.GetNext())
			CorrectLegAnglesLayout(pQuadLeg);	//修正接腿角钢布置方式
	}
	else if(cDefaultType=='1')	//3.设计接腿主材与塔身连接处多接腿换接节点板
		DesignAllLegTransitConnPlates(true,false);
	else if(cDefaultType=='2')	//4.设计接腿与塔身连接处隔面横材中点八字斜材多接腿换接节点板
		DesignAllLegTransitConnPlates(false,true);
	else if(cDefaultType=='V')	//5.接腿V面板设计
	{
		CXhChar50 szItemName="接腿V面板";
		DisplayProcess(0, szItemName);
		int index=0, count=hashQuadLegs.GetNodeNum();
		for(pQuadLeg=hashQuadLegs.GetFirst();pQuadLeg;pQuadLeg=hashQuadLegs.GetNext(),index++)
		{
			if(!DesignLegVPlates(pQuadLeg))
				logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"%d呼高%C接腿第%d象限未能探测到V面板连接设计",pQuadLeg->ciBodySerial,'@'+pQuadLeg->ciLegSerial,pQuadLeg->ciLegQuad);
			szItemName.Printf("%d呼高%C接腿第%d象限V面板设计", pQuadLeg->ciBodySerial, '@'+pQuadLeg->ciLegSerial, pQuadLeg->ciLegQuad);
			DisplayProcess(ftoi(index*100.0/count), szItemName);
		}
		DisplayProcess(100, szItemName);
	}
	else if(cDefaultType=='B')	//6.设计各接腿塔脚
	{
		BASEFOOT basefoot;
		basefoot.CopyFromFoundationParams(&Ta.m_xFoundation);
		AddOrUpdateBaseFoot(&basefoot);	//pFoot
		modeltree.AddOrUpdateFoundations();
	}
#endif
	return true;
}
//1.设计录入V面板设计参数UI
//2.设计塔脚板设计参数UI
#ifdef __RAPID_ROUGH_3DMODEL_
bool TRUNK_POST_KJOINT_CONNPLATE::RetrieveCurrHeightKJointInfo(IModel* pBelongModel,char ciFront0Side1)
{
	if (pBelongModel == NULL)
		pBelongModel = &Ta;
	if (this->pPostAngle == NULL)
		return false;	//基准杆件不合法
	GEPOINT lenStdVec = pPostAngle->pEnd->xOriginalPos- pPostAngle->pStart->xOriginalPos;
	normalize(lenStdVec);
	if (lenStdVec.z < 0)
		lenStdVec *= -1.0;
	this->ciFront0OrSide1 = ciFront0Side1;
	if (ciFront0Side1 == 0)
		this->vPlaneNormal.Set(0, lenStdVec.z, -lenStdVec.y);
	else
		this->vPlaneNormal.Set(lenStdVec.z, 0, -lenStdVec.x);
	normalize(this->vPlaneNormal);
	//CSuperSmartPtr<CLDSLinePart> pDatumRod = (CLDSLinePart*)pPart;
	SmartRodPtr pRod;
	LINEPARTSET linkPartSet;
	//2、遍历全塔把连接在此节点上的杆件放入链表linkPartSet（其中包含父杆件考虑去掉）
	BOOL pushed = pBelongModel->PushPartStack(); //遍历构件列表前应先压栈 wht 12-03-05
	for (pRod = pBelongModel->EnumRodFirst(); pRod.blValid; pRod = pBelongModel->EnumRodNext())
	{	//应使用IsSonPart判断是否为当前模型中的构件，不应使用模型配材号比较 wht 10-08-02
		if (!pRod->IsAngle() && pRod->GetClassTypeId() != CLS_LINETUBE)
			continue;	//仅支持角钢、钢管
		if (pRod->GetClassTypeId() == CLS_LINEANGLE && pRod.LineAnglePointer()->group_father_jg_h > 0x20)
			continue;	//组合角钢的子角钢由组合角钢代表，因此跳过
		if (!this->pPostAngle->ModuleTogetherWith(pRod))
			continue;	//非当前激活呼高分组数据 wht 10-07-23
		if (pRod->pStart == NULL || pRod->pEnd == NULL)
			continue;	//一侧端节点为空视为短角钢，不作处理 wjh-2017.12.18
		if (pRod->pStart == this->pBaseNode || pRod->pEnd == this->pBaseNode)
		{
			if (pRod.IsEqualPtr(this->pPostAngle))
				continue;
			GEPOINT lenRayStdVec = pRod->pEnd->xOriginalPos- pRod->pStart->xOriginalPos;
			normalize(lenRayStdVec);
			if (pRod->pEnd == this->pBaseNode)
				lenRayStdVec *= -1.0;	//保证为射线方向
			if (fabs(lenRayStdVec*vPlaneNormal) > EPS2)
				continue;
			double justify = lenStdVec * lenRayStdVec;
			if (justify < -EPS_COS)	//上侧主材
				this->pUpperPostAngle = pRod.pAngle;
			//else if (justify > EPS_COS2)	//下侧斜材
			//	logerr.Log("error");
			else if (justify > 0)	//下侧斜材
				this->pDownXieAngle = pRod.pAngle;
			else if (fabs(lenRayStdVec.z) < EPS2)
				this->pHoriAngle = pRod.pAngle;
			else if (justify < 0)
				this->pUpXieAngle = pRod.pAngle;
			else
				continue;
			//linkPartSet.append(pRod);
		}
	}
	pBelongModel->PopPartStack(pushed);
	return true;
}
bool TRUNK_POST_KJOINT_CONNPLATE::RetrieveCurrHeightKJointUpXieRod(IModel* pBelongModel)
{
	if (pBelongModel == NULL)
		pBelongModel = &Ta;
	if (this->pUpperPostAngle == NULL)
		return false;
	GEPOINT planeNorm, lenStdVec;
	lenStdVec = pUpperPostAngle->pEnd->xOriginalPos - pUpperPostAngle->pStart->xOriginalPos;
	normalize(lenStdVec);
	if (lenStdVec.z > 0)
		lenStdVec *= -1.0;
	if (ciFront0OrSide1 == 0)
		planeNorm.Set(0, lenStdVec.z, -lenStdVec.y);
	else
		planeNorm.Set(lenStdVec.z, 0, -lenStdVec.x);
	normalize(planeNorm);
	//
	BOOL pushed = pBelongModel->PushPartStack(); //遍历构件列表前应先压栈 wht 12-03-05
	for (SmartRodPtr pRod = pBelongModel->EnumRodFirst(); pRod.blValid; pRod = pBelongModel->EnumRodNext())
	{	
		if (!pRod->IsAngle() && pRod->GetClassTypeId() != CLS_LINETUBE)
			continue;	//仅支持角钢、钢管
		if (pRod->GetClassTypeId() == CLS_LINEANGLE && pRod.LineAnglePointer()->group_father_jg_h > 0x20)
			continue;	//组合角钢的子角钢由组合角钢代表，因此跳过
		if (!this->pPostAngle->ModuleTogetherWith(pRod))
			continue;	//非当前激活呼高分组数据 wht 10-07-23
		if (pRod->pStart == NULL || pRod->pEnd == NULL)
			continue;	//一侧端节点为空视为短角钢，不作处理 wjh-2017.12.18
		if (pRod->pStart == this->pBaseNode || pRod->pEnd == this->pBaseNode)
		{
			if (pRod.IsEqualPtr(this->pPostAngle)||pRod.IsEqualPtr(this->pUpperPostAngle))
				continue;
			GEPOINT lenRayStdVec = pRod->pEnd->xOriginalPos - pRod->pStart->xOriginalPos;
			normalize(lenRayStdVec);
			if (pRod->pEnd == this->pBaseNode)
				lenRayStdVec *= -1.0;	//保证为射线方向
			if (fabs(lenRayStdVec*planeNorm) > EPS2)
				continue;
			if (fabs(lenRayStdVec.z) < EPS2)
				continue;
			double justify = lenStdVec * lenRayStdVec;
			if (justify > 0 && justify < EPS_COS2)	//上侧斜材
				this->pUpXieAngle = pRod.pAngle;
			else
				continue;
		}
	}
	pBelongModel->PopPartStack(pushed);
	return true;
}
bool TRUNK_POST_KJOINT_CONNPLATE::RetrieveCurrHeightKJointDownXieRod(IModel* pBelongModel)
{
	if (pBelongModel == NULL)
		pBelongModel = &Ta;
	if (this->pPostAngle == NULL)
		return false;	//基准杆件不合法
	GEPOINT lenStdVec = pPostAngle->pEnd->xOriginalPos- pPostAngle->pStart->xOriginalPos;
	normalize(lenStdVec);
	if (lenStdVec.z < 0)
		lenStdVec *= -1.0;
	if (ciFront0OrSide1 == 0)
		this->vPlaneNormal.Set(0, lenStdVec.z, -lenStdVec.y);
	else
		this->vPlaneNormal.Set(lenStdVec.z, 0, -lenStdVec.x);
	normalize(this->vPlaneNormal);
	//CSuperSmartPtr<CLDSLinePart> pDatumRod = (CLDSLinePart*)pPart;
	SmartRodPtr pRod;
	LINEPARTSET linkPartSet;
	//2、遍历全塔把连接在此节点上的杆件放入链表linkPartSet（其中包含父杆件考虑去掉）
	BOOL pushed = pBelongModel->PushPartStack(); //遍历构件列表前应先压栈 wht 12-03-05
	for (pRod = pBelongModel->EnumRodFirst(); pRod.blValid; pRod = pBelongModel->EnumRodNext())
	{	//应使用IsSonPart判断是否为当前模型中的构件，不应使用模型配材号比较 wht 10-08-02
		if (!pRod->IsAngle() && pRod->GetClassTypeId() != CLS_LINETUBE)
			continue;	//仅支持角钢、钢管
		if (pRod->GetClassTypeId() == CLS_LINEANGLE && pRod.LineAnglePointer()->group_father_jg_h > 0x20)
			continue;	//组合角钢的子角钢由组合角钢代表，因此跳过
		if (!this->pPostAngle->ModuleTogetherWith(pRod))
			continue;	//非当前激活呼高分组数据 wht 10-07-23
		if (pRod->pStart == NULL || pRod->pEnd == NULL)
			continue;	//一侧端节点为空视为短角钢，不作处理 wjh-2017.12.18
		if (pRod->pStart == this->pBaseNode || pRod->pEnd == this->pBaseNode)
		{
			if (pRod.IsEqualPtr(this->pPostAngle))
				continue;
			GEPOINT lenRayStdVec = pRod->pEnd->xOriginalPos- pRod->pStart->xOriginalPos;
			normalize(lenRayStdVec);
			if (pRod->pEnd == this->pBaseNode)
				lenRayStdVec *= -1.0;	//保证为射线方向
			if (fabs(lenRayStdVec*vPlaneNormal) > EPS2)
				continue;
			if (fabs(lenRayStdVec.z) < EPS2)
				continue;
			double justify = lenStdVec * lenRayStdVec;
			if (justify > 0&&justify < EPS_COS2)	//下侧斜材
				this->pDownXieAngle = pRod.pAngle;
			else
				continue;
		}
	}
	pBelongModel->PopPartStack(pushed);
	return true;
}
static int _LocalCompareNodeCoordZ(const CLDSNodePtr &node1, const CLDSNodePtr &node2)
{
	if (node1->xOriginalPos.z > node2->xOriginalPos.z)
		return 1;
	else if (node1->xOriginalPos.z < node2->xOriginalPos.z)
		return -1;
	else
		return 0;
}
struct NECKPLATE {
	long hBaseNode;	//基准节点
	long hBaseRod;	//基准构件(下主材）
	long hUpPostRod;//变坡瓶口上(外曲臂)主材
	BYTE ciConnectWing;	//0：角钢X肢1：角钢Y肢
	long hHoriAngle;	//火曲板的火曲线基准角钢(既水平横杆)
	LDSNORMAL datumplane,bendplane;
	SIMPLE_ARRAY<CLDSLinePart*,PRESET_OBJS8> xarrDownPlaneRods;	//基面连接杆件(由于后续拆分节点命令用到，不能包含baserod) wjh-2019.10.8
	SIMPLE_ARRAY<CLDSLinePart*,PRESET_OBJS8> xarrUpperPlaneRods;	//1#制弯面连接杆件
	NECKPLATE() {Clear();}
	void Clear(){
		memset(this,0,sizeof(NECKPLATE));
	}
};	//普通节点构造
struct NECKPLATE_ANGLE {
	long hAngle;			//连接角钢句柄
	CLDSLineAngle* pAngle;	//连接角钢
	SIMPLE_ARRAY<CLDSBolt*,PRESET_OBJS64> xarrConnBolts;
public:
	void SetKey(DWORD key) { hAngle=key; }
};
struct CONNECTPLATE{
	CLDSPlate* pPlate;
	SIMPLE_ARRAY<long,PRESET_OBJS64> xarrConnRods;
};
struct RAYANGLE {
	CLDSLineAngle* pAngle;
	GEPOINT vStdRayVec;
	RAYANGLE(CLDSLineAngle* _pRayAngle=NULL,const double* vxStdRayVec=NULL) {
		pAngle=_pRayAngle;
		vStdRayVec=GEPOINT(vxStdRayVec);
	}
};
#include "StdComTempl\StdComModel.h"
bool CTowerMultiLegDesigner::DesignNeckNode(CLDSNode* pNeckNode,CTower* pTower, CIntelliModelDesigner* pDesignerContext/*=NULL*/)
{
	CHashListEx<NECKPLATE_ANGLE> hashFrontNeckAngles,hashSideNeckAngles;
	CLDSModule* pHeight;
	NECKPLATE neckplates[2];//[0]正面；[1]侧面
	neckplates[0].hBaseNode=neckplates[1].hBaseNode=pNeckNode->handle;
	CXhSimpleList<CONNECTPLATE> listFrontPlates,listSidePlates;
	UINT i;
	for (pHeight=pTower->Module.GetFirst();pHeight;pHeight=pTower->Module.GetNext())
	{
		CLdsPartListStack stack(&Ta);
		SmartRodPtr pSmartRod;
		GEPOINT lenStdVec;
		neckplates[0].hBaseRod=neckplates[1].hBaseRod=0;
		neckplates[0].hHoriAngle=neckplates[1].hHoriAngle=0;
		neckplates[0].xarrUpperPlaneRods.Clear();
		neckplates[0].xarrDownPlaneRods.Clear();
		neckplates[1].xarrUpperPlaneRods.Clear();
		neckplates[1].xarrDownPlaneRods.Clear();
		//获取当前呼高下井口正侧面连接点的上下主材及横材
		CHashSet<CLDSGroupLineAngle*> hashGroupAngles;
		GEPOINT lenStdUpVec,lenStdDownVec;
		PRESET_ARRAY32<RAYANGLE> xarrRayAngles;
		CLDSLineAngle* pDownPostAngle=NULL,*pUpPostAngle=NULL;
		double dfUpTipDeltaX=0,dfDownTipDeltaX=0;
		for (pSmartRod=pTower->EnumRodFirst();pSmartRod.blValid;pSmartRod=pTower->EnumRodNext())
		{
			if (pSmartRod->pStart==NULL||pSmartRod->pEnd==NULL||!pSmartRod->IsAngle()||!pHeight->IsSonPart(pSmartRod))
				continue;
			if (pSmartRod->pStart==pNeckNode)
				lenStdVec=pSmartRod->xPosEnd-pSmartRod->xPosStart;
			else if (pSmartRod->pEnd==pNeckNode)
				lenStdVec=pSmartRod->xPosStart-pSmartRod->xPosEnd;
			else
				continue;
			if (hashGroupAngles.GetValue(pSmartRod->handle)!=NULL)
				continue;
			if (pSmartRod.pAngle->blGroupSonAngle)
			{
				if (hashGroupAngles.GetValue(pSmartRod.pAngle->hGroupAngle)!=NULL)
					continue;
				pSmartRod=(CLDSGroupLineAngle*)pTower->FromPartHandle(pSmartRod.pAngle->hGroupAngle,CLS_GROUPLINEANGLE);
			}
			if (pSmartRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
				hashGroupAngles.SetValue(pSmartRod->handle,pSmartRod.pGroupAngle);
			if (pSmartRod->handle==neckplates[0].hBaseRod)
				continue;	//通过之前组合角钢或其它伙伴子角钢已赋值下主材
			normalize(lenStdVec);
			if (pSmartRod->IsPostRod()&&lenStdVec.z>0)	//下主材
			{
				double dfCurrTipDeltaX=pNeckNode->xOriginalPos.x>0?lenStdVec.x:-lenStdVec.x;
				if (pDownPostAngle==NULL||dfDownTipDeltaX<dfCurrTipDeltaX)
				{
					dfDownTipDeltaX=dfCurrTipDeltaX;
					if(pDownPostAngle!=NULL)
						xarrRayAngles.Append(RAYANGLE(pDownPostAngle,lenStdDownVec));
					pDownPostAngle=pSmartRod.pAngle;
					lenStdDownVec=lenStdVec;
					neckplates[0].hBaseRod=neckplates[1].hBaseRod=pSmartRod->handle;
					GEPOINT vFrameLineRodVec=pDownPostAngle->pEnd->xOriginalPos-pDownPostAngle->pStart->xOriginalPos;
					neckplates[0].datumplane.vLine =GEPOINT(1,0,0)^vFrameLineRodVec;
					neckplates[0].datumplane.vSolid=GEPOINT(1,0,0)^lenStdDownVec;
					neckplates[1].datumplane.vLine =GEPOINT(0,1,0)^vFrameLineRodVec;
					neckplates[1].datumplane.vSolid=GEPOINT(0,1,0)^lenStdDownVec;
					if (fabs(pDownPostAngle->vxWingNorm.y)>0.707)
					{
						neckplates[0].ciConnectWing=0;//角钢X肢;
						neckplates[1].ciConnectWing=1;//角钢Y肢
					}
					else if (fabs(pDownPostAngle->vyWingNorm.y)>0.707)
					{
						neckplates[0].ciConnectWing=1;//角钢Y肢
						neckplates[1].ciConnectWing=0;//角钢X肢
					}
					else
						logerr.Log("error");
				}
				else
					xarrRayAngles.Append(RAYANGLE(pSmartRod.pAngle,lenStdVec));
			}
			else if (pSmartRod->IsPostRod()&&lenStdVec.z<0)	//上主材
			{
				double dfCurrTipDeltaX=pNeckNode->xOriginalPos.x>0?lenStdVec.x:-lenStdVec.x;
				if (pUpPostAngle==NULL||dfUpTipDeltaX<dfCurrTipDeltaX)
				{	//区分内曲臂斜材误设为主材时与真实外曲臂如何区分的问题
					dfUpTipDeltaX=dfCurrTipDeltaX;
					if (pUpPostAngle!=NULL)
						xarrRayAngles.Append(RAYANGLE(pUpPostAngle,lenStdUpVec));
					pUpPostAngle=pSmartRod.pAngle;
					lenStdUpVec=lenStdVec;
					neckplates[0].hUpPostRod=neckplates[1].hUpPostRod=pSmartRod->handle;
					GEPOINT vFrameLineRodVec=pUpPostAngle->pEnd->xOriginalPos-pUpPostAngle->pStart->xOriginalPos;
					neckplates[0].bendplane.vLine=GEPOINT(1,0,0)^vFrameLineRodVec;
					neckplates[0].bendplane.vSolid=GEPOINT(1,0,0)^lenStdUpVec;
					neckplates[1].bendplane.vLine=GEPOINT(0,1,0)^vFrameLineRodVec;
					neckplates[1].bendplane.vSolid=GEPOINT(0,1,0)^lenStdUpVec;
				}
				else
					xarrRayAngles.Append(RAYANGLE(pSmartRod.pAngle,lenStdVec));
			}
			else if (fabs(lenStdVec.z)<EPS2&&fabs(lenStdVec.y)<EPS2)//正面连接横材
				neckplates[0].hHoriAngle=pSmartRod->handle;
			else if (fabs(lenStdVec.z)<EPS2&&fabs(lenStdVec.x)<EPS2)	//侧面连接横材
				neckplates[1].hHoriAngle=pSmartRod->handle;
			else
				xarrRayAngles.Append(RAYANGLE(pSmartRod.pAngle,lenStdVec));
		}
		//补充连接点其余射线连接杆件
		for (i=0;i<xarrRayAngles.Count;i++)
		{
			RAYANGLE* pRay=xarrRayAngles.GetAt(i);
			GEPOINT vLiveRodNorm;
			if (pRay->vStdRayVec.z>EPS2)	//下侧杆件
			{
				vLiveRodNorm=pRay->vStdRayVec^lenStdDownVec;
				if (fabs(vLiveRodNorm.x)<EPS2)		//正面杆件
					neckplates[0].xarrDownPlaneRods.Append(pRay->pAngle);
				else if(fabs(vLiveRodNorm.y)<EPS2)	//侧面杆件
					neckplates[1].xarrDownPlaneRods.Append(pRay->pAngle);
			}
			else if (pRay->vStdRayVec.z<-EPS2)	//上侧杆件
			{
				vLiveRodNorm=pRay->vStdRayVec^lenStdUpVec;
				if (fabs(vLiveRodNorm.x)<EPS2)		//正面杆件
					neckplates[0].xarrUpperPlaneRods.Append(pRay->pAngle);
				else if (fabs(vLiveRodNorm.y)<EPS2)	//侧面杆件
					neckplates[1].xarrUpperPlaneRods.Append(pRay->pAngle);
			}
		}
		for(int plane=0;plane<2;plane++)
		{
			normalize(neckplates[plane].datumplane.vLine);
			normalize(neckplates[plane].datumplane.vSolid);
			normalize(neckplates[plane].bendplane.vLine);
			normalize(neckplates[plane].bendplane.vSolid);
			if (neckplates[plane].datumplane.vLine*GEPOINT(pNeckNode->xOriginalPos.x,pNeckNode->xOriginalPos.y)<0)
				neckplates[plane].datumplane.vLine*=-1;
			if (neckplates[plane].datumplane.vSolid*GEPOINT(pNeckNode->xOriginalPos.x,pNeckNode->xOriginalPos.y)<0)
				neckplates[plane].datumplane.vSolid*=-1;
			if (neckplates[plane].bendplane.vLine*GEPOINT(pNeckNode->xOriginalPos.x,pNeckNode->xOriginalPos.y)<0)
				neckplates[plane].bendplane.vLine*=-1;
			if (neckplates[plane].bendplane.vSolid*GEPOINT(pNeckNode->xOriginalPos.x,pNeckNode->xOriginalPos.y)<0)
				neckplates[plane].bendplane.vSolid*=-1;
			CStdComModel modeler;
			CConnectPointInfo connpoint;
			if(plane==0)
			{	//正面
				connpoint.m_bTransitConnect=false;
				connpoint.m_ciBaseRodConnState=3;//衔接段截断点连接
				connpoint.m_hBaseNode=neckplates[0].hBaseNode;
				connpoint.m_hBasePart=neckplates[0].hBaseRod;;
				connpoint.datumplane=neckplates[0].datumplane;
				connpoint.bendplane =neckplates[0].bendplane;
				//connpoint.designType=pConnPlane->designType;
				//connpoint.vDatumDirect=pConnPlane->vDatumDirect;
				connpoint.m_iConnectWing=neckplates[0].ciConnectWing;
				connpoint.m_hBendlineAngle=neckplates[0].hHoriAngle;
				connpoint.linkPartSet.Empty();
				connpoint.bendPlaneRodSet.Empty();
				connpoint.linkPartSet.append(pDownPostAngle);
				connpoint.bendPlaneRodSet.append(pUpPostAngle);
				//补充下侧基面连接杆件
				CLDSLineAngle* pHoriAngle=(CLDSLineAngle*)pTower->FromPartHandle(neckplates[0].hHoriAngle);
				connpoint.linkPartSet.append(pHoriAngle);
				for (i=0;i<neckplates[0].xarrDownPlaneRods.Count;i++)
					connpoint.linkPartSet.append(neckplates[0].xarrDownPlaneRods.At(i));
				//补充上侧1#制弯面连接杆件
				for (i=0;i<neckplates[0].xarrUpperPlaneRods.Count;i++)
					connpoint.bendPlaneRodSet.append(neckplates[0].xarrUpperPlaneRods.At(i));
			}
			else //if(plane==1)
			{	//侧面
				connpoint.m_bTransitConnect=false;
				connpoint.m_ciBaseRodConnState=3;//衔接段截断点连接
				connpoint.m_hBaseNode=neckplates[1].hBaseNode;
				connpoint.m_hBasePart=neckplates[1].hBaseRod;;
				connpoint.datumplane=neckplates[1].datumplane;
				connpoint.bendplane =neckplates[1].bendplane;
				//connpoint.designType=pConnPlane->designType;
				//connpoint.vDatumDirect=pConnPlane->vDatumDirect;
				connpoint.m_iConnectWing=neckplates[0].ciConnectWing;
				connpoint.m_hBendlineAngle=neckplates[1].hHoriAngle;
				connpoint.linkPartSet.Empty();
				connpoint.bendPlaneRodSet.Empty();
				connpoint.linkPartSet.append(pDownPostAngle);
				connpoint.bendPlaneRodSet.append(pUpPostAngle);
				//补充下侧基面连接杆件
				CLDSLineAngle* pHoriAngle=(CLDSLineAngle*)pTower->FromPartHandle(neckplates[1].hHoriAngle);
				connpoint.linkPartSet.append(pHoriAngle);
				for (i=0;i<neckplates[1].xarrDownPlaneRods.Count;i++)
					connpoint.linkPartSet.append(neckplates[1].xarrDownPlaneRods.At(i));
				//补充上侧1#制弯面连接杆件
				for (i=0;i<neckplates[1].xarrUpperPlaneRods.Count;i++)
					connpoint.bendPlaneRodSet.append(neckplates[1].xarrUpperPlaneRods.At(i));
			}
			SIMPLE_ARRAY<long,PRESET_OBJS64> xarrConnRods;
			for (pSmartRod=connpoint.linkPartSet.GetFirst();pSmartRod.blValid;pSmartRod=connpoint.linkPartSet.GetNext())
				xarrConnRods.Append(pSmartRod->handle);
			for (pSmartRod=connpoint.bendPlaneRodSet.GetFirst();pSmartRod.blValid;pSmartRod=connpoint.bendPlaneRodSet.GetNext())
				xarrConnRods.Append(pSmartRod->handle);
			bool blSharedPlate=false;
			CQuickSort<long>::QuickSort(xarrConnRods,xarrConnRods.Count,compare_long);
			CXhSimpleList<CONNECTPLATE>* pListPlates=plane==0?&listFrontPlates:&listSidePlates;
			for(CONNECTPLATE* pConnPlate=pListPlates->EnumObjectFirst();pConnPlate;pConnPlate=pListPlates->EnumObjectNext())
			{
				if (pConnPlate->xarrConnRods.Count!=xarrConnRods.Count)
					continue;
				for (int j=0;j<(int)xarrConnRods.Count;j++)
				{
					if (xarrConnRods[j]!=pConnPlate->xarrConnRods[j])
						break;
				}
				if (j==xarrConnRods.Count)
				{
					blSharedPlate=true;
					pConnPlate->pPlate->cfgword.AddBodyLegs(pHeight->idBodyNo);
					break;
				}
			}
			if (blSharedPlate)
				continue;	//与之前呼高共用的井口连接板
			NECKPLATE_ANGLE* pNeckAngle;
			CHashListEx<NECKPLATE_ANGLE>* pHashNeckAngles=&hashFrontNeckAngles;
			if (plane==1)
				pHashNeckAngles=&hashSideNeckAngles;
			for(pNeckAngle=pHashNeckAngles->GetFirst();pNeckAngle;pNeckAngle=pHashNeckAngles->GetNext())
			{
				CStdComModel::SHAREANGLE* pShareAngle=modeler.hashShareAngles.Add(pNeckAngle->hAngle);
				pShareAngle->pAngle=pNeckAngle->pAngle;
				pShareAngle->xarrConnBolts.CloneFrom(pNeckAngle->xarrConnBolts);
			}
			CLDSPartPtr xarrNewPartsPool[100];
			CNewPartsListen listen(&Ta.Parts,xarrNewPartsPool,100);
			CStackVariant<bool> stackvar(&CStdComModel::m_bEnableMirCreate,false);
			if (modeler.IntelliDesignStdComNodePlate(&connpoint)<= 0)
			{	//根据标准节点构造的模板设计失败（并非用户主动中断），此时依据传统规则继续生成节点板
				CCommonLjSmartDesign design;
				design.DesignConnectPoint(&connpoint,0,TRUE);
			}
			modeler.hashShareAngles.Empty();
			//注册新生成的连接板
			listen.EndListen();
			SmartPartPtr pNewPart;
			for (int i=0;i<listen.xarrNewParts.Count;i++)
			{
				pNewPart = listen.xarrNewParts[i];
				if (pNewPart->IsPlate())
				{
					long hBasePart = pNewPart.pBasicPlate->m_hBaseRod;
					long hBaseNode = pNewPart.pBasicPlate->m_hBaseNode;
					CLDSLineAngle* pBaseAngle = (CLDSLineAngle*)Ta.Parts.FromHandle(hBasePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
					if (pBaseAngle == NULL|| hBaseNode==0)
						continue;
					double jx = pNewPart->ucs.axis_z*pBaseAngle->vxWingNorm;
					double jy = pNewPart->ucs.axis_z*pBaseAngle->vyWingNorm;
					char cWorkWingX0Y1 = fabs(jx) > fabs(jy) ? 'X' : 'Y';
					CUnifyModelLayer::PLATE* pRegPlate=pDesignerContext->RegisterPlate(hBaseNode, hBasePart, cWorkWingX0Y1);
					pRegPlate->hPlate = pNewPart->handle;
					CONNECTPLATE* pConnPlate=pListPlates->Append();
					pConnPlate->pPlate=pNewPart.pPlate;
					pConnPlate->xarrConnRods.CloneFrom(xarrConnRods);
				}
				else if (!pNewPart->IsBolt())
					continue;
				for (pSmartRod=connpoint.linkPartSet.GetFirst();pSmartRod.blValid;pSmartRod=connpoint.linkPartSet.GetNext())
				{
					if (!pSmartRod->IsAngle())
						continue;
					NECKPLATE_ANGLE* pNeckAngle=pHashNeckAngles->Add(pSmartRod->handle);
					pNeckAngle->pAngle=pSmartRod.pAngle;
					if (pNeckAngle->pAngle->FindLsByHandle(pNewPart->handle))
						pNeckAngle->xarrConnBolts.Append(pNewPart.pBolt);
				}
				for (pSmartRod=connpoint.bendPlaneRodSet.GetFirst();pSmartRod.blValid;pSmartRod=connpoint.bendPlaneRodSet.GetNext())
				{
					if (!pSmartRod->IsAngle())
						continue;
					NECKPLATE_ANGLE* pNeckAngle=pHashNeckAngles->Add(pSmartRod->handle);
					pNeckAngle->pAngle=pSmartRod.pAngle;
					if (pNeckAngle->pAngle->FindLsByHandle(pNewPart->handle))
						pNeckAngle->xarrConnBolts.Append(pNewPart.pBolt);
				}
			}
		}
	}
	return true;
}
bool CTowerMultiLegDesigner::DesignTrunkPostNeckShiftPlates(CTower* pTower,CIntelliModelDesigner* pDesignerContext/* = NULL*/)
{
	CModelSlopeNode* pBtmSlope=modeltree.listSlopes.EnumObjectFirst();
	if(pBtmSlope==NULL||pBtmSlope->hashRootHeights.Count==0)
		return false;
	//设计最下层变处段瓶口处直连塔身呼高情况下的瓶口板
	CLDSModule* pHighestModule=NULL;
	for (pHighestModule=pTower->Module.GetFirst();pHighestModule;pHighestModule=pTower->Module.GetNext())
	{
		if (pHighestModule->idBodyNo==pBtmSlope->pMainHeight->Id)
			break;
	}
	if (pHighestModule==NULL)
		return NULL;
	GEPOINT xNeckPoint(pBtmSlope->fFrontWidth*0.5,pBtmSlope->fSideWidth*0.5,pBtmSlope->fTopZ);
	CLDSNode* pNode=NULL,*pNeckNode=NULL;
	PRESET_ARRAY8<CLDSNode*> xarrNeckNodes;
	for (pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
	{
		if (pNode->IsLegObj())
			continue;
		GEPOINT xUniPoint(fabs(pNode->xOriginalPos.x),fabs(pNode->xOriginalPos.y),pNode->xOriginalPos.z);
		if (xUniPoint.IsEqual(xNeckPoint))
			xarrNeckNodes.Append(pNode);
	}
	if (xarrNeckNodes.Count!=4)
	{
		logerr.Log("最下层塔身坡段瓶口节点数目非4");
		return false;
	}
	int hits=0;
	for (UINT i=0;i<xarrNeckNodes.Count;i++)
	{
		if (DesignNeckNode(xarrNeckNodes.At(i),pTower,pDesignerContext))
			hits++;
	}
	return hits>0;
}
bool CTowerMultiLegDesigner::DesignTrunkPostShiftJoints(CTower* pTower, CIntelliModelDesigner* pDesignerContext/* = NULL*/)
{
	m_pTower = pTower;
	m_pDesignerContext = pDesignerContext;
	CXhChar50 szProcessTitle = "塔身共用段接身轮换点设计";
	Ta.DisplayProcess(0, szProcessTitle);
	if (modeltree.hashSegments.Count == 0)
	{	//初始化模型树
		CIntelliModelDesigner intelli;
		if (pDesignerContext == NULL)
			pDesignerContext = &intelli;
		COLINEAR_TOLERANCE tolerance(25);//setdlg.m_fTolerance,setdlg.m_fToleranceTangent);
		pDesignerContext->RecognizeSlopes(tolerance.distance);	//必须置于CombineDiscreteRodMembers之后，否则会因合并杆件导致intelli中部分句柄失效 wjh-2015.12.28
		modeltree.SetBelongTower(pTower);
		pDesignerContext->RetrievedTowerModeTree(modeltree);
	}
	DesignTrunkPostNeckShiftPlates(pTower,pDesignerContext);
	CModelSegmentNode* pSegment=NULL;
	CModelHeightNode* pHeight=NULL;
	CModelSubLegNode* pSubLeg=NULL;
	LINEPARTSET postrods;
	CHashSet<CLDSNode*> hashJointNodes;
	for(pSegment=modeltree.hashSegments.GetFirst();pSegment;pSegment=modeltree.hashSegments.GetNext())
	{
		postrods.append(pSegment->pPostRod);
		hashJointNodes.SetValue(pSegment->pPostRod->pStart->handle,pSegment->pPostRod->pStart);
		hashJointNodes.SetValue(pSegment->pPostRod->pEnd->handle,pSegment->pPostRod->pEnd);
	}
	for (pHeight = modeltree.hashHeights.GetFirst(); pHeight; pHeight = modeltree.hashHeights.GetNext())
	{
		for (UINT i = 0; i < pHeight->xarrSubLegs.Count; i++)
			postrods.append(pHeight->xarrSubLegs.At(i).pPostRod);
	}
	DYN_ARRAY<CLDSNode*> xarrPostNodes(hashJointNodes.GetNodeNum());
	UINT i=0;
	CLDSNode* pNode;
	SmartRodPtr pSmartRod,pUpRod,pDownRod;
	ARRAY_LIST<ROD_PTR> xarrDownRods(0,8);
	for (pNode = hashJointNodes.GetFirst(); pNode; pNode = hashJointNodes.GetNext(), i++)
		xarrPostNodes[i] = pNode;
	CQuickSort<CLDSNode*>::QuickSort(xarrPostNodes, xarrPostNodes.Size(), _LocalCompareNodeCoordZ);
	UINT count = xarrPostNodes.Size();
	for(i=0;i< xarrPostNodes.Size();i++)
	{
		pNode = xarrPostNodes[i];
		xarrDownRods.SetSize(0);
#ifdef __TMP_LOG_PROC_
		logerr.Log("0x%X塔身换接身节点在设计%d@%d", pNode->handle,i,count);
#endif
		pUpRod = NULL;
		bool hasOnlyDownLegPostRods=true;
		for(pSmartRod=postrods.GetFirst();pSmartRod.blValid;pSmartRod=postrods.GetNext())
		{
			if (!pSmartRod->IsAngle())
				continue;
			if(pSmartRod->pStart==pNode)
			{
				if(pSmartRod->pEnd->xOriginalPos.z<pNode->xOriginalPos.z)
				{
					if(pUpRod.blValid)
						logerr.Log("error");
					pUpRod=pSmartRod;
				}
				else
				{
					xarrDownRods.append(pSmartRod);
					hasOnlyDownLegPostRods &= pSmartRod->IsLegObj();
				}
			}
			else if(pSmartRod->pEnd==pNode)
			{
				if(pSmartRod->pStart->xOriginalPos.z<pNode->xOriginalPos.z)
				{
					if (pUpRod.blValid)
						logerr.Log("error");
					pUpRod=pSmartRod;
				}
				else
				{
					xarrDownRods.append(pSmartRod);
					hasOnlyDownLegPostRods &= pSmartRod->IsLegObj();
				}
			}
		}
		if (xarrDownRods.GetSize() == 0)
			continue;	//可能为腿身过渡节点，已单独在多接腿设计中处理，此处跳过
		if (xarrDownRods.GetSize() == 1)
			continue;	//可能为全塔共用段或上下均同属某一呼高，跳过留给批量节点智能设计
		if (!pUpRod.blValid)
			continue;	//可能是塔身顶部节点
		TRUNK_POST_KJOINT_CONNPLATE kjoint;
		kjoint.pUpperPostAngle = pUpRod.pAngle;
		kjoint.pPostAngle = (CLDSLineAngle*)xarrDownRods[0];
		kjoint.pBaseNode = pNode;
		GEPOINT lenStdUpVec = kjoint.pUpperPostAngle->xPosStart - kjoint.pUpperPostAngle->xPosEnd;
		normalize(lenStdUpVec);
		GEPOINT lenStdDownVec = kjoint.pPostAngle->xPosEnd - kjoint.pPostAngle->xPosStart;
		normalize(lenStdDownVec);
		kjoint.blColinearJoint = fabs(lenStdUpVec*lenStdDownVec) > EPS_COS;
		bool blSharedWithMultiHeights = xarrDownRods.Size() > 1;
		if (kjoint.blColinearJoint)
		{	//进行K点连接设计
			if (!hasOnlyDownLegPostRods)
			{
				//提取正面K点连接信息
#ifdef __TMP_LOG_PROC_
				logerr.Log("prepare");
#endif
				if (!kjoint.RetrieveCurrHeightKJointInfo(&Ta, 0))
					continue;
#ifdef __TMP_LOG_PROC_
				logerr.Log("KJoint retrieve on front");
#endif
				CorrectTrunkPostKNodeConnPlate(kjoint, xarrDownRods.m_pData, xarrDownRods.Size(), true);
#ifdef __TMP_LOG_PROC_
				logerr.Log("KJoint on front");
#endif
				//提取侧面K点连接信息
				if (!kjoint.RetrieveCurrHeightKJointInfo(&Ta, 1))
					continue;
#ifdef __TMP_LOG_PROC_
				logerr.Log("KJoint retrieve on side");
#endif
				CorrectTrunkPostKNodeConnPlate(kjoint, xarrDownRods.m_pData, xarrDownRods.Size(), true);
#ifdef __TMP_LOG_PROC_
				logerr.Log("KJoint on side");
#endif
			}
			//设计接身换接点的接头
			CorrectTrunkPostJoints(kjoint, xarrDownRods.m_pData, xarrDownRods.Size(), true);
		}
		else if (!kjoint.blColinearJoint && blSharedWithMultiHeights)
		{	//进行多呼高过渡的变坡点连接设计
			//提取正面变坡点连接信息
			if (!kjoint.RetrieveCurrHeightKJointInfo(&Ta, 0))
				continue;
			CorrectTrunkPostSlopeConnPlate(kjoint, xarrDownRods.m_pData, xarrDownRods.Size(), true);
			//提取侧面变坡点连接信息
			if (!kjoint.RetrieveCurrHeightKJointInfo(&Ta, 1))
				continue;
			CorrectTrunkPostSlopeConnPlate(kjoint, xarrDownRods.m_pData, xarrDownRods.Size(), true);
		}
#ifdef __TMP_LOG_PROC_
		logerr.Log("KJoints");
#endif
		int percent = 100 * i / count;
		Ta.DisplayProcess(percent, szProcessTitle);
	}
	Ta.DisplayProcess(100, szProcessTitle);
	return true;
}
#endif
void Test_CorrectTowerLegs()
{
	CLogErrorLife loglife(&logerr);
	CTowerMultiLegDesigner designer;
	designer.DesignAllSubLegs(&Ta);
}
#endif
