#include "stdafx.h"
#include "..\LDS.h"
#include "NodeStdComTempl.h"
#include "WireStruComTempl.h"
#include "Fitting.h"

//��ʼ��ģ�����
//IStdComTempl::STDTMPL_STRU_META_SECT_FRT_1_GD
void CStdComTemplWireStructure::InitComTempl_META_SECT_FRT_1_GD()
{
	ClearTmplParams();
	name.Copy("����������������浥GD�ҵ�");
	szSketchImgFile.Copy("JC-META-GD1.bmp");
	m_bTmplInitialized = true;
	//m_bEnableMirCreate = true;
	//�趨���߼ܵ�ʶ��ؼ��ڵ��˼�
	xarrKeyRodNodes.ReSize(1);
	xarrKeyRodNodes[0].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod = 1;	//ͨ�����߶˽ڵ�ʶ��
	//�趨�ҿ׽����Ϣ
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
	//1.��׼�����Ʋ���������
	//���߽Ǹּ�϶
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	pParam = listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("���߽Ǹּ�϶, GD�ҵ��߿��+��϶��mm");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue = xarrWireGroups[0].wnReservedWidth;	//���ܸ��̶�ֵ84������������߳�ͻ;
	//���߽Ǹֹ��
	pParam = listParams.Add(KEY4C("WAS"));
	pParam->sLabel.Copy("WireAngleSizeSpec");
	pParam->sNotes.Copy("���߽Ǹֹ��");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_STRING;
	StrCopy(pParam->value.szStrBuf, "125x10", 96);
	//�����Ǹֹ��
	pParam = listParams.Add(KEY4C("BAS"));
	pParam->sLabel.Copy("BaseAngleSizeSpec");
	pParam->sNotes.Copy("�����Ǹֹ��mm");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_STRING;
	StrCopy(pParam->value.szStrBuf, "125x10", 96);
	//�̽���˨��
	pParam = listParams.Add(KEY4C("FBN"));
	pParam->sLabel.Copy("FIX_CONN_BOLTS_N");
	pParam->sNotes.Copy("���߶̽ǸֵĹ̽���˨��");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_LONG;//VALUETYPE_LONG;
	pParam->value.liValue = this->m_cnFixWireAngleConnBoltsN = 2;
	//2.ģ�ͽڵ�˼�����
	CStdTemplParaNode* pParaNode = NULL;
	pParaNode = listParaNodes.Add(N10_ID);
	pParaNode->m_bNecessaryNode = true;
	//1��Ϊ�ᵣ����
	CStdTemplParaAngle* pParaAngle=NULL;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->vLocalNearVecS2E.Set(-1, 0, 0);
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->normal.ciNormalDirection = 'Y';
	//2��3��Ϊǰ������ᵣ�����ϵ����������̽Ǹ�
	const int ANG_FRT_L = 2;
	const int ANG_FRT_R = 3;
	const int ANG_BTM = 4;	//�ײ������̽Ǹ�
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_FRT_L, CLS_LINEANGLE);
	pParaAngle->cbMirCreateFlag = 0x01;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->blShortAngle = true;
	pParaAngle->attachangle.idBaseAngle = 1;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode = N10_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'X';
	pParaAngle->attachangle.ciNormalOffset = 0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout = 2;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection = 'Z';	//ʼ->����'X'�����
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = 0;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//������˨λ�ü���
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
	pParaAngle->attachangle.idBaseAngle = 1;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode = N10_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'X';
	pParaAngle->attachangle.ciNormalOffset = 0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout = 3;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection = 'Z';	//ʼ->����'X'�����
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = 0;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//������˨λ�ü���
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
	pParaAngle->attachangle.idBaseAngle = 1;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode = N10_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'Y';
	pParaAngle->attachangle.ciAssistWing = 'X';
	pParaAngle->attachangle.ciNormalOffset = 0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout = 0;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection = 0;	//ʼ->����'X'�����
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//������˨λ�ü���
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->desStart.szXorCtrlWingOffset = "-P2#T";
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.szXorCtrlWingOffset = "-P2#T";
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	//3.��άģ������
	//���Ӱ����Ӹ˼�
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
	//���Ӱ�����
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Add(1);
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
	pParaPlate->cbMirCreateFlag = 0x01;
	pParaPlate->design.idBaseNode = N10_ID;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//��������
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

//����ģ����Ʋ���
extern CTower Ta;
bool CStdComTemplWireStructure::TransTemlInstance_SECT_FRT_1_GD()
{
	PRESET_ARRAY4<CStdTemplParaAngle*> xParaAngleArr;
	xParaAngleArr[0] = (CStdTemplParaAngle*)this->listParaRods.GetValue(2);
	xParaAngleArr[1] = (CStdTemplParaAngle*)this->listParaRods.GetValue(3);
	xParaAngleArr[2] = (CStdTemplParaAngle*)this->listParaRods.GetValue(4);
	if (xParaAngleArr[0] == NULL || xParaAngleArr[1] == NULL|| xParaAngleArr[2]==NULL)
		return false;
	//1�ŽǸ�Ϊ��ϽǸ�ʱ��������ӹ��ߵ����Ǹ�
	BOOL bBaseGroupJg = FALSE;
	long *phRod = hashMatchRodHid.GetValue(1);
	CLDSLineAngle* pBaseJg = (CLDSLineAngle*)Ta.FromPartHandle(*phRod, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
	if (pBaseJg && pBaseJg->GetClassTypeId() == CLS_GROUPLINEANGLE)
	{
		xParaAngleArr[2]->m_bNecessaryRod = false;
		bBaseGroupJg = TRUE;
	}
	//��׼�˼�Ϊ��ϽǸ�ʱ�����ְ����Ӹ˼�ϸ�����ӽǸ�
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
	//��ʼ���Ǹֹ��
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	if (pParam = listParams.GetValue(KEY4C("WAS")))
	{
		for (int i = 0; i < 2; i++)
			xParaAngleArr[i]->m_sGuige.Copy(pParam->value.szStrBuf);
	}
	if (pParam = listParams.GetValue(KEY4C("BAS")))
		xParaAngleArr[2]->m_sGuige.Copy(pParam->value.szStrBuf);
	//���ݽ�߿�����ö̽Ǹ�֫��ƫ����
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
	//���ݹ̽���˨����ʼ����˨��λ����
	if (pParam= listParams.GetValue(KEY4C("FBN")))
	{
		CStdTemplParaBolt* pParaBolt = NULL;
		for (int i = 0; i < 2; i++)
		{
			if (pParam->value.liValue == 2)
			{	//�̽���˨��Ϊ2,����T�ͽǸ����ӽǸָ���1��
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
				pParaBolt->through[0].cPartType = 2;	//�����ְ�
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
				pParaBolt->through[0].cPartType = 2;	//�����ְ�
				pParaBolt->through[0].val = 1;
			}
			else if (pParam->value.liValue == 4)
			{	//�̽���˨��Ϊ4,����T�ͽǸ����ӽǸָ���2��
				//g1���߽���λ����˨
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
				pParaBolt->through[0].cPartType = 2;	//�����ְ�
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
				pParaBolt->through[0].cPartType = 2;	//�����ְ�
				pParaBolt->through[0].val = 1;
				//g2���߽���λ����˨
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
				pParaBolt->through[0].cPartType = 2;	//�����ְ�
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
				pParaBolt->through[0].cPartType = 2;	//�����ְ�
				pParaBolt->through[0].val = 1;
			}
		}
	}
	//��ʼ���ҿ׶�λ����
	char cWireJgWorkWing = xParaAngleArr[0]->ciAngleWorkWing;
	for (BYTE ciGroup = 0; ciGroup < m_cnWireGroupCount; ciGroup++)
	{
		short snCount = xarrWireGroups[ciGroup].cnWirePointCount;
		short siHalfSpaceL = xarrWireGroups[ciGroup].wiSpaceL/2;
		short siCurrLenOffsetToStart = 0, siCurrLenOffsetToEnd = 0;
		if (xarrWireGroups[ciGroup].cnWirePointCount % 2 == 1)
		{	//�м�ҿ�
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
		{	//ǰ��Գƹҿ�
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
			{	//���ǰ��(ʼ�˲�)�ҿ�
				siCurrLenOffsetToStart += siHalfSpaceL;
				pParaBolt->org.siLenOffset = -siCurrLenOffsetToStart;
			}
			else
			{	//��Ӻ��(�ն˲�)�ҿ�
				siCurrLenOffsetToEnd += siHalfSpaceL;
				pParaBolt->org.siLenOffset = siCurrLenOffsetToEnd;
			}
			pParaBolt->normal.ciNormalCalStyle = 1;
			pParaBolt->normal.widBasePart = xParaAngleArr[0]->Id;
		}
	}
	return true;
}