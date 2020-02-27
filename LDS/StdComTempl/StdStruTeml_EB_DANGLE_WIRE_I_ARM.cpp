#include "stdafx.h"
#include "..\LDS.h"
#include "NodeStdComTempl.h"
#include "WireStruComTempl.h"
#include "Fitting.h"

//��ʼ��ģ�����
//IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM1
void CStdComTemplWireStructure::InitComTempl_EB_DANGLE_WIRE_I_ARM1()
{
	ClearTmplParams();
	name.Copy("�����Ǻᵣ����EB����I��");
	szSketchImgFile.Copy("DXC-EB3.bmp");
	this->m_bTmplInitialized = true;
	//�趨���߼ܵ�ʶ��ؼ��ڵ��˼�
	this->xarrKeyRodNodes.ReSize(1);
	xarrKeyRodNodes[0].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod = 1;	//ͨ�����߶˽ڵ�ʶ��
	//0.�ҵ���鶨��
	//�趨�ҿ׽����Ϣ
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
	xarrWireGroups[0].ciGroupSerial = 1;
	xarrWireGroups[0].cnWirePointCount = 1;
	xarrWireGroups[0].wiSpaceL = 0;
	xarrWireGroups[0].ciFittingType = CLDSFitting::TYPE_EB;
	xarrWireGroups[0].cnShaftD = ftoi(pEBInfo->dfTrunnionD);
	xarrWireGroups[0].wnFittingWidth = ftoi(pEBInfo->dfBoardL);
	StrCopy(xarrWireGroups[0].szFittingSizeCode, pEBInfo->szSizeCode, 17);
	StrCopy(xarrWireGroups[0].szUringSizeCode, pURInfo->szSizeCode, 17);
	//1.��׼�����Ʋ���������
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	pParam = listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("T�͹��߽Ǹּ�϶, EB�ҵ��߿��+��϶��mm");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue = xarrWireGroups[0].wnReservedWidth;	//���ܸ��̶�ֵ84������������߳�ͻ
	//2.ģ�ͽڵ�˼�����
	CStdTemplParaNode* pParaNode=NULL;
	pParaNode = listParaNodes.Add(N10_ID);
	pParaNode = listParaNodes.Add(N20_ID);
	pParaNode = listParaNodes.Add(N30_ID);
	pParaNode = listParaNodes.Add(N40_ID);
	pParaNode->m_bNecessaryNode = false;
	pParaNode->ciPosCalType = CStdTemplParaNode::SCALE;
	pParaNode->attach_scale = 0.5;
	pParaNode->attachNodeArr[0] = N10_ID;//1;
	pParaNode->attachNodeArr[1] = TMPLOBJID(N10_ID, 'X');
	pParaNode->keyLabelFatherRod = R_W_T;
	//
	CStdTemplParaAngle* pParaAngle=NULL;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = N20_ID;
	pParaAngle->idEndNode = N10_ID;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = N30_ID;
	pParaAngle->idEndNode = N10_ID;
	//3��ΪT�͹�����ϽǸ�
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(R_W_T, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->idEndNode = TMPLOBJID(N10_ID, 'X');
	pParaAngle->ciSectStyle = 'T';
	pParaAngle->ciMasterIndex = 1;
	pParaAngle->ciAngleLayout = 3;
	pParaAngle->normal.ciNormalDirection = 'Z';
	pParaAngle->normal.uidRefRodOfWorkWing = 1;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->m_bNeedUpdatePosition = true;
	pParaAngle->desStart.uidTouchLineRod = 1;
	pParaAngle->desStart.szXorCtrlWingOffset = "-$1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.uidTouchLineRod = TMPLOBJID(1, 'X');
	pParaAngle->desEnd.szXorCtrlWingOffset = "-$1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	//3.��άģ������
	MATCH_CONNECT_POINT* pConnPoint = this->listStdConnPoints.Append();
	pConnPoint->idNode = N10_ID;
	pConnPoint->idBaseRod = 1;
	pConnPoint->idRayRodOnAxisY = R_W_T;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DM1;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DM1;
	pConnPoint = this->listStdConnPoints.Append();
	pConnPoint->idNode = N10_ID;
	pConnPoint->idBaseRod = 1;
	pConnPoint->idRayRodOnAxisY = 2;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_GX2;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_GX2;
}
bool CStdComTemplWireStructure::TransTemlInstance_EB_DANGLE_WIRE_I_ARM1()
{
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	//����T����ϽǸּ�϶ֵ
	if (pParam = listParams.Add(2))
	{
		CStdTemplParaAngle* pParaAngle = (CStdTemplParaAngle*)listParaRods.GetValue(R_W_T);
		pParaAngle->wiGroupAngleSpace = (WORD)pParam->value.liSafeVal;
	}
	//��ʼ���ҿ׶�λ����
	for (BYTE ciGroup = 0; ciGroup < m_cnWireGroupCount; ciGroup++)
	{
		short snCount = xarrWireGroups[ciGroup].cnWirePointCount;
		short siHalfSpaceL = (short)(xarrWireGroups[ciGroup].wiSpaceL*0.5);
		short siCurrLenOffsetToStart = 0, siCurrLenOffsetToEnd = 0;
		if (xarrWireGroups[ciGroup].cnWirePointCount % 2 == 1)
		{	//�м�ҿ�
			CStdTemplParaBolt* pParaBolt = listParaBolts.Add(0);
			pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONDUCT_WIRE;
			pParaBolt->m_iWireGroup = ciGroup;
			pParaBolt->dfHoleDia = xarrWireGroups[ciGroup].cnHoleD;
			pParaBolt->through[0].cItemType = 1;
			pParaBolt->through[0].val = R_W_T;
			pParaBolt->org.ciDatumPointCalStyle = 2;
			pParaBolt->org.idBasePart = R_W_T;
			pParaBolt->org.idBaseNode = N40_ID;
			pParaBolt->org.ciDatumWing = 1;
			pParaBolt->org.siLenOffset = 0;
			pParaBolt->normal.ciNormalCalStyle = 1;
			pParaBolt->normal.widBasePart = R_W_T;
			pParaBolt->normal.ciNormalDirect = 0;
			//
			snCount -= 1;
		}
		for (int jI = 1; jI <= snCount; jI++)
		{	//ǰ��Գƹҿ�
			CStdTemplParaBolt* pParaBolt = listParaBolts.Add(0);
			pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_EARTH_WIRE;
			pParaBolt->m_iWireGroup = ciGroup;
			pParaBolt->dfHoleDia = xarrWireGroups[ciGroup].cnHoleD;
			pParaBolt->through[0].cItemType = 1;
			pParaBolt->through[0].val = R_W_T;
			pParaBolt->org.ciDatumPointCalStyle = 2;
			pParaBolt->org.idBasePart = R_W_T;
			pParaBolt->org.idBaseNode = N50_ID;
			pParaBolt->org.ciDatumWing = 1;
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
			pParaBolt->normal.widBasePart = R_W_T;
		}
	}
	return true;
}
//IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM2
void CStdComTemplWireStructure::InitComTempl_EB_DANGLE_WIRE_I_ARM2()
{
	ClearTmplParams();
	name.Copy("�ᵣV��EB�����ҵ�");
	szSketchImgFile.Copy("EB6.bmp");
	this->m_bTmplInitialized = true;
	//�趨���߼ܵ�ʶ��ؼ��ڵ��˼�
	this->xarrKeyRodNodes.ReSize(2);
	xarrKeyRodNodes[0].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod = N10_ID;
	xarrKeyRodNodes[1].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[1].idNodeOrRod = N20_ID;
	//�趨�ҿ׽����Ϣ
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
	xarrWireGroups[0].ciGroupSerial = 1;
	xarrWireGroups[0].cnWirePointCount = 1;
	xarrWireGroups[0].wiSpaceL = 0;
	xarrWireGroups[0].ciFittingType = CLDSFitting::TYPE_EB;
	xarrWireGroups[0].cnShaftD = ftoi(pEBInfo->dfTrunnionD);
	xarrWireGroups[0].wnFittingWidth = ftoi(pEBInfo->dfBoardL);
	StrCopy(xarrWireGroups[0].szFittingSizeCode, pEBInfo->szSizeCode, 17);
	StrCopy(xarrWireGroups[0].szUringSizeCode, pURInfo->szSizeCode, 17);
	//��Ʋ���������
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	pParam = listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("T�͹��߽Ǹּ�϶, EB�ҵ��߿��+��϶��mm");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue = xarrWireGroups[0].wnReservedWidth;
	//���߽Ǹֹ��
	pParam = listParams.Add(KEY4C("WAS"));
	pParam->sLabel.Copy("WireAngleSizeSpec");
	pParam->sNotes.Copy("���߽Ǹֹ��");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_STRING;
	StrCopy(pParam->value.szStrBuf, "125x10", 96);
	//�̽���˨��
	pParam = listParams.Add(KEY4C("FBN"));
	pParam->sLabel.Copy("FIX_CONN_BOLTS_N");
	pParam->sNotes.Copy("���߶̽ǸֵĹ̽���˨��");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_LONG;//VALUETYPE_LONG;
	pParam->value.liValue = this->m_cnFixWireAngleConnBoltsN = 2;
	//�ڵ�˼�����
	CStdTemplParaNode* pParaNode=NULL;
	pParaNode = listParaNodes.Add(N10_ID);
	pParaNode = listParaNodes.Add(N20_ID);
	pParaNode = listParaNodes.Add(N30_ID);
	CStdTemplParaAngle* pParaAngle=NULL;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = N20_ID;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->idEndNode = N20_ID;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(5, CLS_LINEANGLE);
	pParaAngle->idStartNode = N20_ID;
	pParaAngle->idEndNode = N30_ID;
	//�ᵣT�ͽǸּ�������˨
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(R_W_T, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->idEndNode = TMPLOBJID(N10_ID, 'X');
	pParaAngle->ciSectStyle = 'T';
	pParaAngle->ciMasterIndex = 1;	//T_Y
	pParaAngle->ciAngleLayout = 3;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->normal.ciNormalDirection = 'Z';
	pParaAngle->normal.uidRefRodOfWorkWing = 1;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.uidTouchLineRod = 1;
	pParaAngle->desStart.szXorCtrlWingOffset = "-$1#T";	
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->bUpdateEndPos = true;
	pParaAngle->desEnd.uidTouchLineRod = TMPLOBJID(1, 'X');
	pParaAngle->desEnd.szXorCtrlWingOffset = "-$1#T";
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	//T�ͽǸ���ᵣ����ͨ����˨����
	CStdTemplParaBolt* pParaBolt = NULL;
	for (int i = 0; i < 2; i++)
	{
		pParaBolt = listParaBolts.Add(0);
		pParaBolt->cbMirCreateFlag = 0X01;
		pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
		pParaBolt->org.ciDatumPointCalStyle = 3;
		pParaBolt->org.idBasePart = 1;
		pParaBolt->org.idCrossRod = R_W_T;
		pParaBolt->org.idWorkSonJg = i;
		pParaBolt->org.ciDatumWing = 'Y';
		memcpy(pParaBolt->org.szWingG, "g1", 2);
		memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
		//
		pParaBolt = listParaBolts.Add(0);
		pParaBolt->cbMirCreateFlag = 0X01;
		pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
		pParaBolt->org.ciDatumPointCalStyle = 3;
		pParaBolt->org.idBasePart = 1;
		pParaBolt->org.idCrossRod = R_W_T;
		pParaBolt->org.idWorkSonJg = i;
		pParaBolt->org.ciDatumWing = 'Y';
		memcpy(pParaBolt->org.szWingG, "g2", 2);
		memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
	}
	//���߶̽Ǹ�
	const int ANG_FL = 6;
	const int ANG_BL = 7;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_FL, CLS_LINEANGLE);
	pParaAngle->idStartNode = N30_ID;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->blShortAngle = true;
	pParaAngle->attachangle.idBaseAngle = R_W_T;
	pParaAngle->attachangle.idBaseNode = N30_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'X';
	pParaAngle->attachangle.ciNormalOffset = 0;
	pParaAngle->attachangle.ciAngleLayout = 3;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = 0;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//������˨λ�ü���
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->desStart.szXorCtrlWingOffset = "-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.szXorCtrlWingOffset = "-P1#T";
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	//
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_BL, CLS_LINEANGLE);
	pParaAngle->idStartNode = N20_ID;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->blShortAngle = true;
	pParaAngle->attachangle.idBaseAngle = R_W_T;
	pParaAngle->attachangle.idBaseNode = N30_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'X';
	pParaAngle->attachangle.ciNormalOffset = 0;
	pParaAngle->attachangle.ciAngleLayout = 2;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//������˨λ�ü���
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->desStart.szYorOthrWingOffset = "-P1#T";
	pParaAngle->desStart.szXorCtrlWingOffset[0] = '0';
	pParaAngle->desEnd.szYorOthrWingOffset = "-P1#T";
	pParaAngle->desEnd.szXorCtrlWingOffset[0] = '0';
	//���Ӱ�
	//N10_ID�������Ӱ�
	MATCH_CONNECT_POINT* pConnPoint = listStdConnPoints.Append();
	pConnPoint->idNode = N10_ID;
	pConnPoint->idBaseRod = 1;
	pConnPoint->idRayRodOnAxisY = 3;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_GX3;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_GX3;
	//N20_ID�������Ӱ�
	pConnPoint = listStdConnPoints.Append();
	pConnPoint->idNode = N20_ID;
	pConnPoint->idBaseRod = 2;
	pConnPoint->idRayRodOnAxisY = 3;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_GX3;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_GX3;
	//N30_ID�������Ӱ�
	pConnPoint = listStdConnPoints.Append();
	pConnPoint->idNode = N30_ID;
	pConnPoint->idBaseRod = R_W_T;
	pConnPoint->idRayRodOnAxisY = 5;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_GX4;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_GX4;
	//N30�������Ӱ�(���й��߶̽Ǹ�)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(8, CLS_LINEANGLE);
	pParaAngle->idStartNode = N30_ID;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(9, CLS_LINEANGLE);
	pParaAngle->idStartNode = N30_ID;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(10, CLS_LINEANGLE);
	pParaAngle->idStartNode = N30_ID;
	pParaAngle->m_bNecessaryRod = false;
	//
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Add(1);
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
	pParaPlate->_thick = 10;
	pParaPlate->design.idBaseNode = N30_ID;
	pParaPlate->design.idBaseRod = R_W_T;
	pParaPlate->design.ciProfileStyle = 1;	//��������
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	pParaPlate->connrods[0].InitBasicInfo(CMPSTID(R_W_T, 1), true, 1, 2, true);
	pParaPlate->connrods[0].InitTipInfo(2, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = N30_ID;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 2;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 2;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	pParaPlate->connrods[1].InitBasicInfo(CMPSTID(R_W_T, 2), true, 1, 2, true);
	pParaPlate->connrods[1].InitTipInfo(2, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = N30_ID;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 2;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 2;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	pParaPlate->connrods[2].InitBasicInfo(8, true, 1, 3, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, R_W_T);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = N30_ID;
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	pParaPlate->connrods[3].InitBasicInfo(9, true, 1, 3, false);
	pParaPlate->connrods[3].InitTipInfo(0, 0, R_W_T);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt = N30_ID;
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	pParaPlate->connrods[4].InitBasicInfo(10, false, 1, 2, false);
	pParaPlate->connrods[4].InitTipInfo(0, 0, R_W_T);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt = N30_ID;
	pParaPlate->connrods[4].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}
extern CTower Ta;
bool CStdComTemplWireStructure::TransTemlInstance_EB_DANGLE_WIRE_I_ARM2()
{
	//5��6��Ϊ����ǰ�����������������߶̽Ǹ�
	const int ANG_FL = 6;
	const int ANG_BL = 7;
	CStdTemplParaAngle* pParaAngleF = (CStdTemplParaAngle*)this->listParaRods.GetValue(ANG_FL);
	CStdTemplParaAngle* pParaAngleB = (CStdTemplParaAngle*)this->listParaRods.GetValue(ANG_BL);
	if (pParaAngleF == NULL || pParaAngleB == NULL)
		return false;
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	//��ʼ���Ǹֹ��
	if (pParam = listParams.GetValue(KEY4C("WAS")))
	{
		pParaAngleF->m_sGuige.Copy(pParam->value.szStrBuf);
		pParaAngleB->m_sGuige.Copy(pParam->value.szStrBuf);
	}
	//���¹��߽Ǹּ�϶ֵ
	if (pParam = listParams.Add(2))
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
	//���ݹ̽���˨����ʼ����˨��λ����
	if (pParam = this->listParams.GetValue(KEY4C("FBN")))
	{
		CStdTemplParaBolt* pParaBolt = NULL;
		for (int i = 0; i < 2; i++)
		{
			CStdTemplParaAngle* pParaAngle = (i == 0) ? pParaAngleF : pParaAngleB;
			if (pParam->value.liValue == 2)
			{	//�̽���˨��Ϊ2,����T�ͽǸ����ӽǸָ���1��
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = pParaAngle->Id;
				pParaBolt->org.idCrossRod = R_W_T;
				pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = pParaAngle->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 1);
				memcpy(pParaBolt->org.szCrossWingG, "g", 1);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//�����ְ�
				pParaBolt->through[0].val = 1;
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = pParaAngle->Id;
				pParaBolt->org.idCrossRod = R_W_T;
				pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = pParaAngle->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 1);
				memcpy(pParaBolt->org.szCrossWingG, "g", 1);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//�����ְ�
				pParaBolt->through[0].val = 1;
			}
			else if (pParam->value.liValue == 4)
			{	//�̽���˨��Ϊ4,����T�ͽǸ����ӽǸָ�������
				//g1���߽���λ����˨
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = pParaAngle->Id;
				pParaBolt->org.idCrossRod = R_W_T;
				pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = pParaAngle->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g1", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//�����ְ�
				pParaBolt->through[0].val = 1;
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = pParaAngle->Id;
				pParaBolt->org.idCrossRod = R_W_T;
				pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = pParaAngle->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g1", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//�����ְ�
				pParaBolt->through[0].val = 1;
				//g2���߽���λ����˨
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = pParaAngle->Id;
				pParaBolt->org.idCrossRod = R_W_T;
				pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = pParaAngle->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//�����ְ�
				pParaBolt->through[0].val = 1;
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = pParaAngle->Id;
				pParaBolt->org.idCrossRod = R_W_T;
				pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = pParaAngle->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
				pParaBolt->through[0].cItemType = 1;
				pParaBolt->through[0].cPartType = 2;	//�����ְ�
				pParaBolt->through[0].val = 1;
			}
		}
	}
	//���ݹҿײ�����ʼ���ҿ׶�λ����
	if (xarrWireGroups[0].cnWirePointCount == 1)
	{
		char ciHoleWing = (pParaAngleF->ciAngleWorkWing == 'X') ? 'Y' : 'X';
		//��ӿ�
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
	//ƥ��N30���ĸְ����Ӹ˼�
	long* pHandle=hashMatchRodHid.GetValue(R_W_T);
	CLDSLinePart* pDatumRod = Ta.FromRodHandle(*pHandle);
	CStdTemplParaNode* pParaNode = listParaNodes.GetValue(N30_ID);
	if (pParaNode && pParaNode->pRelaNode && pDatumRod)
	{
		LINEPARTSET linkPartSet;	//�����ڵ�ǰ�ڵ��ϵĸ˼�����
		CLDSNode* pKeyNode = pParaNode->pRelaNode;
		GEPOINT vStdPlaneNormal(0, 0, 1), vLine, vDatumLine;
		vDatumLine = pDatumRod->pEnd->xOriginalPos - pDatumRod->pStart->xOriginalPos;
		normalize(vDatumLine);
		BOOL pushed = Ta.PushPartStack();
		for (CLDSLinePart* pRod = Ta.EnumRodFirst(); pRod; pRod = Ta.EnumRodNext())
		{
			if (pRod->pStart == NULL || pRod->pEnd == NULL)
				continue;
			if (pRod->pStart != pKeyNode && pRod->pEnd != pKeyNode)
				continue;
			linkPartSet.append(pRod);
		}
		Ta.PopPartStack(pushed);
		for (CLDSLinePart* pRod = linkPartSet.GetFirst(); pRod; pRod = linkPartSet.GetNext())
		{
			if (pRod->pStart->handle == pKeyNode->handle)
				vLine = pRod->pEnd->xOriginalPos - pRod->pStart->xOriginalPos;
			else
				vLine = pRod->pStart->xOriginalPos - pRod->pEnd->xOriginalPos;	
			GEPOINT vNormal = vLine ^ vDatumLine;
			normalize(vNormal);
			if (fabs(vNormal*vStdPlaneNormal) < 0.82)
				continue;	//
			normalize(vLine);
			if (fabs(vLine*vDatumLine) < EPS2)
				hashMatchRodHid.SetValue(10, pRod->handle);
			else
			{
				if (hashMatchRodHid.GetValue(8) == NULL)
					hashMatchRodHid.SetValue(8, pRod->handle);
				else
					hashMatchRodHid.SetValue(9, pRod->handle);
			}
		}
	}
	return true;
}
//IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM3
void CStdComTemplWireStructure::InitComTempl_EB_DANGLE_WIRE_I_ARM3()
{
	ClearTmplParams();
	name.Copy("EB������ֱ��������I������(ˮƽ�ᵣ)");
	szSketchImgFile.Copy("EB7.bmp");
	//2.ģ�ͽڵ�˼�����
	CStdTemplParaNode* pParaNode;
	CStdTemplParaRod* pParaRod;
	CStdTemplParaAngle* pParaAngle;
	pParaNode = listParaNodes.Add(N10_ID);
	pParaNode = listParaNodes.Add(N20_ID);
	pParaNode = listParaNodes.Add(N30_ID);
	pParaNode->m_bNecessaryNode = false;
	pParaNode->ciPosCalType = CStdTemplParaNode::SCALE;
	pParaNode->attach_scale = 0.5;
	pParaNode->attachNodeArr[0] = N20_ID;//2;
	pParaNode->attachNodeArr[1] = TMPLOBJID(N20_ID, 'X');
	pParaNode->keyLabelFatherRod = R_W_T;

	pParaRod = listParaRods.Add(1, CLS_LINEANGLE);
	pParaRod->idStartNode = N10_ID;//3;
	pParaRod->idEndNode = N20_ID;
	pParaRod = listParaRods.Add(2);
	pParaRod->idStartNode = N20_ID;
	pParaRod->idEndNode = TMPLOBJID(N20_ID, 'X');
	pParaRod = listParaRods.Add(3);
	pParaRod->idStartNode = N20_ID;
	pParaRod->idEndNode = N30_ID;
	//3��ΪT�͹�����ϽǸ�
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(R_W_T, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;//2;
	pParaAngle->idEndNode = TMPLOBJID(N10_ID, 'X');//-2;
	pParaAngle->ciSectStyle = 'T';
	pParaAngle->ciMasterIndex = 1;
	pParaAngle->ciAngleLayout = 1;
	pParaAngle->normal.ciNormalDirection = 'Z';
	pParaAngle->normal.uidRefRodOfWorkWing = 2;
	pParaAngle->m_bNeedUpdatePosition = pParaAngle->m_bNeedUpdateNormal = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "-$2#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.szXorCtrlWingOffset = "-$2#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	pParaRod = listParaRods.Add(5);
	pParaRod->idStartNode = N10_ID;
	pParaRod->siMinRayAngle = -5;
	pParaRod->siMaxRayAngle = 5;

	//3.��άģ������
	//3.1�ҿ�����
	CStdTemplParaBolt* pParaBolt = listParaBolts.Add(1);
	pParaBolt->m_wiDiameter = 20;
	pParaBolt->m_fHoleIncrement = 26 - 20;
	pParaBolt->m_hBoltFamily = 1;
	pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_EARTH_WIRE;
	pParaBolt->through[0].cItemType = 1;
	pParaBolt->through[0].val = R_W_T;
	pParaBolt->normal.ciNormalCalStyle = 1;
	pParaBolt->normal.widBasePart = R_W_T;
	pParaBolt->normal.ciNormalDirect = 0;
	pParaBolt->org.ciDatumPointCalStyle = 2;
	pParaBolt->org.ciDatumWing = 1;
	pParaBolt->org.idBaseNode = N20_ID;
	pParaBolt->org.idBasePart = R_W_T;
	memcpy(pParaBolt->org.szWingG, "50", 2);
	pParaBolt->org.siLenOffset = 0;
	MATCH_CONNECT_POINT* pConnPoint = this->listStdConnPoints.Append();
	pConnPoint->idNode = N10_ID;
	pConnPoint->idBaseRod = 5;
	pConnPoint->idRayRodOnAxisY = R_W_T;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DM1;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DM1;
}
