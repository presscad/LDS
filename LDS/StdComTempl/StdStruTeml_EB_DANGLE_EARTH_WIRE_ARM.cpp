#include "stdafx.h"
#include "..\LDS.h"
#include "NodeStdComTempl.h"
#include "WireStruComTempl.h"
#include "Fitting.h"

//��ʼ��ģ�����
//IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1
void CStdComTemplWireStructure::InitComTempl_EB_DANGLE_EARTH_WIRE_ARM1()
{
	ClearTmplParams();
	name.Copy("EB������ֱ��������֧��");
	szSketchImgFile.Copy("EB1.bmp");
	this->m_bTmplInitialized=true;
	//�趨���߼ܵ�ʶ��ؼ��ڵ��˼�
	this->xarrKeyRodNodes.ReSize(1);
	xarrKeyRodNodes[0].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod = 1;	//ͨ�����߶˽ڵ�ʶ��
	//0.�ҵ���鶨��,�趨�ҿ׽����Ϣ
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
	m_cnWireGroupCount = 2;
	xarrWireGroups[0].ciGroupSerial=1;
	xarrWireGroups[0].cnWirePointCount = 1;
	xarrWireGroups[0].wiSpaceL = 0;
	xarrWireGroups[0].ciFittingType=CLDSFitting::TYPE_EB;
	xarrWireGroups[0].cnShaftD = ftoi(pEBInfo->dfTrunnionD);
	xarrWireGroups[0].wnFittingWidth = ftoi(pEBInfo->dfBoardL);
	StrCopy(xarrWireGroups[0].szFittingSizeCode, pEBInfo->szSizeCode, 17);
	StrCopy(xarrWireGroups[0].szUringSizeCode, pURInfo->szSizeCode, 17);
	xarrWireGroups[1].ciGroupSerial = 2;
	xarrWireGroups[1].cnWirePointCount = 2;
	xarrWireGroups[1].cnHoleD = 26;
	xarrWireGroups[1].wiSpaceL = 400;
	xarrWireGroups[1].ciFittingType=CLDSFitting::TYPE_EB;
	xarrWireGroups[1].cnShaftD = ftoi(pEBInfo->dfTrunnionD);
	xarrWireGroups[1].wnFittingWidth = ftoi(pEBInfo->dfBoardL);
	StrCopy(xarrWireGroups[1].szFittingSizeCode, pEBInfo->szSizeCode, 17);
	StrCopy(xarrWireGroups[1].szUringSizeCode, pURInfo->szSizeCode, 17);
	//1.��Ʋ���������
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	pParam=listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("T�͹��߽Ǹּ�϶, EB�ҵ��߿��+��϶��mm");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=xarrWireGroups[0].wnReservedWidth;	//���ܸ��̶�ֵ84������������߳�ͻ
	//2.ģ�ͽڵ�˼�����
	CStdTemplParaNode* pParaNode = NULL;
	pParaNode=listParaNodes.Add(N10_ID);
	pParaNode=listParaNodes.Add(N20_ID);
	pParaNode=listParaNodes.Add(N30_ID);
	pParaNode=listParaNodes.Add(N40_ID);
	pParaNode=listParaNodes.Add(N50_ID);	//T�ͽǸ��м�ҵ�ڵ�
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::SCALE;
	pParaNode->attach_scale=0.5;
	pParaNode->attachNodeArr[0]= N20_ID;
	pParaNode->attachNodeArr[1]=TMPLOBJID(N20_ID,'X');
	pParaNode->keyLabelFatherRod=R_W_T;
	//�˼�����
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode=N30_ID;
	pParaAngle->idEndNode=N20_ID;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode=N10_ID;
	pParaAngle->idEndNode=N20_ID;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode=N30_ID;
	pParaAngle->idEndNode=N40_ID;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(5, CLS_LINEANGLE);
	pParaAngle->idStartNode = N30_ID;
	pParaAngle->idEndNode = N10_ID;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(R_W_T, CLS_LINEANGLE);	//T�͹�����ϽǸ�
	pParaAngle->idStartNode= N20_ID;
	pParaAngle->idEndNode  = TMPLOBJID(N20_ID,'X');
	pParaAngle->ciSectStyle='T';
	pParaAngle->ciMasterIndex=1;
	pParaAngle->ciAngleLayout=3;
	pParaAngle->m_bNeedUpdateNormal = false;	//�ݲ�����T����ϽǸ�֫����
	pParaAngle->normal.ciNormalDirection='Z';
	pParaAngle->normal.uidRefRodOfWorkWing=2;
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->desStart.uidTouchLineRod=2;
	pParaAngle->desStart.szXorCtrlWingOffset="-$2#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szYorOthrWingOffset[0]='0';
	pParaAngle->desEnd.uidTouchLineRod=TMPLOBJID(2,'X');
	pParaAngle->desEnd.szXorCtrlWingOffset="-$2#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
	
	//3.��άģ������
	//3.1�ҿ�����(�Ѹ���Translate_XXX()���Զ����� wjh-2019.9.6)
	//3.2���Ӱ�����
	//3.3��׼���ӵ�����
	//N10_ID�������Ӱ�DX3����DX3A
	//DX3�ڵ���DX3A���Ӹ˼��������ӵ����Ͳ�ͬ�ǻ���ģ�ʵ��ִ��ʱ����ͬʱ���DX3��DX3A wjh-2019.11.03
	MATCH_CONNECT_POINT* pConnPoint = listStdConnPoints.Append();
	pConnPoint->idNode = N10_ID;
	pConnPoint->idBaseRod = 5;
	pConnPoint->idRayRodOnAxisY = 2;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DX3;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DX3;
	//DX3A(����֧�����Ҹ˹��߶Ͽ�)
	pConnPoint = this->listStdConnPoints.Append();
	pConnPoint->idNode = N10_ID;
	pConnPoint->idBaseRod = 2;
	pConnPoint->idRayRodOnAxisY = 5;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DX3A;//TMPLID(0x0201,1);
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DX3A;//TMPLID(0x0301,1);
	//N20�������Ӱ�GX2
	pConnPoint=this->listStdConnPoints.Append();
	pConnPoint->idNode=N20_ID;
	pConnPoint->idBaseRod=2;
	pConnPoint->idRayRodOnAxisY=1;
	pConnPoint->cbMirCreateFlag=MIR_CREATE_X;
	pConnPoint->idStdConnPoint=STDTEMPL_CONNPOINT_XH_GX2;
	pConnPoint->idStdNodePlate=STDTEMPL_NODEPLATE_XH_GX2;
	//N20�������Ӱ�DM1
	pConnPoint = this->listStdConnPoints.Append();
	pConnPoint->idNode = N20_ID;
	pConnPoint->idBaseRod = 2;
	pConnPoint->idRayRodOnAxisY = R_W_T;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DM1;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DM1;
	//N30�������Ӱ�DX1
	pConnPoint = listStdConnPoints.Append();
	pConnPoint->idNode = N30_ID;
	pConnPoint->idBaseRod = 5;
	pConnPoint->idRayRodOnAxisY = 3;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DX1;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DX1;
	//N40�������Ӱ�DX2
	pConnPoint = listStdConnPoints.Append();
	pConnPoint->idNode=N40_ID;
	pConnPoint->idBaseRod=2;
	pConnPoint->idRayRodOnAxisY=3;
	pConnPoint->cbMirCreateFlag=MIR_CREATE_X;
	pConnPoint->idStdConnPoint=STDTEMPL_CONNPOINT_XH_DX2;
	pConnPoint->idStdNodePlate=STDTEMPL_NODEPLATE_XH_DX2;
}
//����ģ����Ʋ���
bool CStdComTemplWireStructure::TransTemlInstance_EB_DANGLE_EARTH_WIRE_ARM1()
{
	//����T����ϽǸּ�϶ֵ
	CStdTemplParaAngle* pParaWireAngle = (CStdTemplParaAngle*)listParaRods.GetValue(R_W_T);
	if (pParaWireAngle == NULL)
		return false;
	STDTEMPL_PARAM_ITEM* pParam = listParams.GetValue(2);
	if (pParam)
		pParaWireAngle->wiGroupAngleSpace = (WORD)pParam->value.liSafeVal;
	//��ʼ���ҿ׶�λ����
	short siWireHoleSpaceL = 400;
	for(BYTE ciGroup=0;ciGroup<2;ciGroup++)
	{
		short snCount = xarrWireGroups[ciGroup].cnWirePointCount;
		short siCurrLenOffsetToStart=0,siCurrLenOffsetToEnd=0;
		siWireHoleSpaceL=xarrWireGroups[ciGroup].wiSpaceL;
		short siHalfSpaceL=siWireHoleSpaceL/2;
		if (ciGroup==1&&siWireHoleSpaceL==0)
			continue;	//�ڶ��鵵��Ϊ�㣬��ʾ���� wjh-2019.11.02
		if (xarrWireGroups[ciGroup].cnWirePointCount%2 == 1)
		{	//�м�ҿ�
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
			pParaBolt->org.siLenOffset=0;
			pParaBolt->normal.ciNormalCalStyle = 1;
			pParaBolt->normal.widBasePart= R_W_T;
			//
			snCount -= 1;
		}
		else
			siCurrLenOffsetToStart=siCurrLenOffsetToEnd=-siWireHoleSpaceL/2;
		for (int jI = 0; jI < snCount; jI++)
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
			if(jI%2==0)
			{	//���ǰ��(ʼ�˲�)�ҿ�
				siCurrLenOffsetToStart+=siWireHoleSpaceL;
				pParaBolt->org.siLenOffset=-siCurrLenOffsetToStart;
			}
			else
			{	//��Ӻ��(�ն˲�)�ҿ�
				siCurrLenOffsetToEnd+=siWireHoleSpaceL;
				pParaBolt->org.siLenOffset=siCurrLenOffsetToEnd;
			}
			pParaBolt->normal.ciNormalCalStyle = 1;
			pParaBolt->normal.widBasePart = R_W_T;
		}
	}
	return true;
}
//IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM2
void CStdComTemplWireStructure::InitComTempl_EB_DANGLE_EARTH_WIRE_ARM2()
{
	ClearTmplParams();
	name.Copy("EB����������ֱ��������֧��");
	szSketchImgFile.Copy("ENZ-EB2.bmp");
	this->m_bTmplInitialized=true;
	//�趨���߼ܵ�ʶ��ؼ��ڵ��˼�
	this->xarrKeyRodNodes.ReSize(1);
	xarrKeyRodNodes[0].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod = 1;	//ͨ�����߶˽ڵ�ʶ��
	//0.�ҵ���鶨��
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
	xarrWireGroups[0].cnWirePointCount = 2;
	xarrWireGroups[0].wiSpaceL = 400;
	xarrWireGroups[0].ciFittingType = CLDSFitting::TYPE_EB;
	xarrWireGroups[0].cnShaftD = ftoi(pEBInfo->dfTrunnionD);
	xarrWireGroups[0].wnFittingWidth = ftoi(pEBInfo->dfBoardL);
	StrCopy(xarrWireGroups[0].szFittingSizeCode, pEBInfo->szSizeCode, 17);
	StrCopy(xarrWireGroups[0].szUringSizeCode, pURInfo->szSizeCode, 17);
	//��Ʋ���������
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	pParam=listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("T�͹��߽Ǹּ�϶, EB�ҵ��߿��+��϶��mm");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue= xarrWireGroups[0].wnReservedWidth;	//���ܸ��̶�ֵ84������������߳�ͻ
	//��������
	pParam = listParams.Add(KEY4C("L"));
	pParam->sLabel.Copy("SpaceL");
	pParam->sNotes.Copy("ͬ��ҿ׼�Ŀ�������");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//���߶̽ǸֵĹ��
	pParam = listParams.Add(KEY4C("WAS"));
	pParam->sLabel.Copy("WIREANGLE_SIZE");
	pParam->sNotes.Copy("T�͹��߽Ǹֹ��mm");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_STRING;
	StrCopy(pParam->value.szStrBuf, "125x10", 96);
	//���߶̽ǸֵĹ̽���˨
	pParam = listParams.Add(KEY4C("FBN"));
	pParam->sLabel.Copy("FIX_CONN_BOLTS_N");
	pParam->sNotes.Copy("���߶̽ǸֵĹ̽���˨��");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_LONG;
	pParam->value.liValue = this->m_cnFixWireAngleConnBoltsN = 2;
	//2.ģ�ͽڵ�˼�����
	CStdTemplParaNode* pParaNode=NULL;
	pParaNode=listParaNodes.Add(N10_ID);
	pParaNode=listParaNodes.Add(N20_ID);
	pParaNode=listParaNodes.Add(N30_ID);
	pParaNode=listParaNodes.Add(N40_ID);
	pParaNode=listParaNodes.Add(N50_ID);
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::SCALE;
	pParaNode->attach_scale=0.5;
	pParaNode->attachNodeArr[0]= N20_ID;//2;
	pParaNode->attachNodeArr[1]=TMPLOBJID(N20_ID,'X');
	pParaNode->keyLabelFatherRod=R_W_T;//3;
	pParaNode=listParaNodes.Add(N60_ID);	//ǰ��EB�ҵ�λ��
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=N50_ID;
	pParaNode->attachNodeArr[1]=N20_ID;
	pParaNode->attachNodeArr[2]=N50_ID;
	strcpy(pParaNode->offset_expression, "L/2");
	pParaNode->keyLabelFatherRod=R_W_T;
	pParaNode=listParaNodes.Add(N70_ID);	//���EB�ҵ�λ��
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=N50_ID;
	pParaNode->attachNodeArr[1]=N50_ID;
	pParaNode->attachNodeArr[2]=N20_ID;
	strcpy(pParaNode->offset_expression, "L/2");
	pParaNode->keyLabelFatherRod = R_W_T;
	//ģ��˼�
	CStdTemplParaAngle* pParaAngle = NULL; 
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode=N30_ID;
	pParaAngle->idEndNode=N20_ID;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode=N10_ID;
	pParaAngle->idEndNode=N20_ID;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode=N30_ID;
	pParaAngle->idEndNode=N40_ID;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(5, CLS_LINEANGLE);
	pParaAngle->idStartNode = N30_ID;
	pParaAngle->idEndNode = N10_ID;
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(R_W_T,CLS_LINEANGLE);	//T����ϽǸ�
	pParaAngle->idStartNode= N20_ID;//2;
	pParaAngle->idEndNode  = TMPLOBJID(N20_ID,'X');//-2;
	pParaAngle->ciSectStyle='T';
	pParaAngle->ciMasterIndex=1;
	pParaAngle->ciAngleLayout=3;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->normal.ciNormalDirection='Z';
	pParaAngle->normal.uidRefRodOfWorkWing=2;
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->desStart.uidTouchLineRod=2;
	pParaAngle->desStart.szXorCtrlWingOffset="-$2#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szYorOthrWingOffset[0]='0';
	pParaAngle->desEnd.uidTouchLineRod=TMPLOBJID(2,'X');
	pParaAngle->desEnd.szXorCtrlWingOffset="-$2#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//������˨��������ͷ
	//6��7��Ϊǰ��ҵ�����������̽Ǹ�
	const int ANG_FL1 = 6;
	const int ANG_FL2 = 7;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_FL1, CLS_LINEANGLE);
	pParaAngle->idStartNode = N60_ID;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->attachangle.idBaseAngle = R_W_T;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode = N60_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'X';
	pParaAngle->attachangle.ciNormalOffset = 0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout = 2;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection = 'Z';	//ʼ->����'X'�����
	pParaAngle->m_bNeedUpdatePosition = true;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->blShortAngle = true;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//������˨λ�ü���
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->desStart.szYorOthrWingOffset = "-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szXorCtrlWingOffset[0] = '0';
	pParaAngle->desEnd.szYorOthrWingOffset = "-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szXorCtrlWingOffset[0] = '0';
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_FL2, CLS_LINEANGLE);
	pParaAngle->idStartNode = N60_ID;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->attachangle.idBaseAngle = R_W_T;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode = N60_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'X';
	pParaAngle->attachangle.ciNormalOffset = 0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout = 3;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection = 'Z';	//ʼ->����'X'�����
	pParaAngle->m_bNeedUpdatePosition = true;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//������˨λ�ü���
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->desStart.szXorCtrlWingOffset = "-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.szXorCtrlWingOffset = "-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	//8��9��Ϊ���ҵ�����������̽Ǹ�
	const int ANG_BL1 = 8;
	const int ANG_BL2 = 9;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_BL1, CLS_LINEANGLE);
	pParaAngle->idStartNode = N70_ID;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->attachangle.idBaseAngle = R_W_T;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode = N70_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'X';
	pParaAngle->attachangle.ciNormalOffset = 0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout = 2;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection = 'Z';	//ʼ->����'X'�����
	pParaAngle->m_bNeedUpdatePosition = true;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->blShortAngle = true;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//������˨λ�ü���
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->desStart.szYorOthrWingOffset = "-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szXorCtrlWingOffset[0] = '0';
	pParaAngle->desEnd.szYorOthrWingOffset = "-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szXorCtrlWingOffset[0] = '0';
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_BL2, CLS_LINEANGLE);
	pParaAngle->idStartNode = N70_ID;
	pParaAngle->ciSectStyle = 'L';
	pParaAngle->attachangle.idBaseAngle = R_W_T;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode = N70_ID;
	pParaAngle->attachangle.ciDatumAngleWing = 'X';
	pParaAngle->attachangle.ciNormalOffset = 0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout = 3;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection = 'Z';	//ʼ->����'X'�����
	pParaAngle->m_bNeedUpdatePosition = true;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//������˨λ�ü���
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->desStart.szXorCtrlWingOffset = "-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.szXorCtrlWingOffset = "-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	//3.��άģ������
	//3.1�ҿ�����
	//3.2���Ӱ�����
	//3.3��׼���ӵ�����
	//N10�������Ӱ�DX3,DX3�ڵ���DX3A���Ӹ˼��������ӵ����Ͳ�ͬ�ǻ���ģ�ʵ��ִ��ʱ����ͬʱ���DX3��DX3A wjh-2019.11.03
	MATCH_CONNECT_POINT* pConnPoint=this->listStdConnPoints.Append();
	pConnPoint=this->listStdConnPoints.Append();
	pConnPoint->idNode=N10_ID;
	pConnPoint->idBaseRod=5;
	pConnPoint->idRayRodOnAxisY=2;
	pConnPoint->cbMirCreateFlag=MIR_CREATE_X;
	pConnPoint->idStdConnPoint=STDTEMPL_CONNPOINT_XH_DX3;//TMPLID(0x0201,1);
	pConnPoint->idStdNodePlate=STDTEMPL_NODEPLATE_XH_DX3;//TMPLID(0x0301,1);
	//DX3A(����֧�����Ҹ˹��߶Ͽ�)
	pConnPoint=this->listStdConnPoints.Append();
	pConnPoint->idNode=N10_ID;
	pConnPoint->idBaseRod=5;
	pConnPoint->idRayRodOnAxisY=2;
	pConnPoint->cbMirCreateFlag=MIR_CREATE_X;
	pConnPoint->idStdConnPoint=STDTEMPL_CONNPOINT_XH_DX3A;//TMPLID(0x0201,1);
	pConnPoint->idStdNodePlate=STDTEMPL_NODEPLATE_XH_DX3A;//TMPLID(0x0301,1);
	//N20�������Ӱ�GX2
	pConnPoint->idNode = N20_ID;
	pConnPoint->idBaseRod = 2;
	pConnPoint->idRayRodOnAxisY = 1;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_GX2;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_GX2;
	//N20�������Ӱ�DM1
	pConnPoint = this->listStdConnPoints.Append();
	pConnPoint->idNode = N20_ID;
	pConnPoint->idBaseRod = 2;
	pConnPoint->idRayRodOnAxisY = R_W_T;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DM1;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DM1;
	//N30�������Ӱ�DX1
	pConnPoint = this->listStdConnPoints.Append();
	pConnPoint->idNode = N30_ID;
	pConnPoint->idBaseRod = 5;
	pConnPoint->idRayRodOnAxisY = 3;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DX1;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DX1;
	//N40�������Ӱ�DX2
	pConnPoint=this->listStdConnPoints.Append();
	pConnPoint->idNode=N40_ID;
	pConnPoint->idBaseRod=2;
	pConnPoint->idRayRodOnAxisY=3;
	pConnPoint->cbMirCreateFlag=MIR_CREATE_X;
	pConnPoint->idStdConnPoint=STDTEMPL_CONNPOINT_XH_DX2;
	pConnPoint->idStdNodePlate=STDTEMPL_NODEPLATE_XH_DX2;
}

//����ģ����Ʋ���
bool CStdComTemplWireStructure::TransTemlInstance_EB_DANGLE_EARTH_WIRE_ARM2()
{
	PRESET_ARRAY4<CStdTemplParaAngle*> xParaAngleArr;
	xParaAngleArr[0] = (CStdTemplParaAngle*)this->listParaRods.GetValue(6);
	xParaAngleArr[1] = (CStdTemplParaAngle*)this->listParaRods.GetValue(7);
	xParaAngleArr[2] = (CStdTemplParaAngle*)this->listParaRods.GetValue(8);
	xParaAngleArr[3] = (CStdTemplParaAngle*)this->listParaRods.GetValue(9);
	if (xParaAngleArr[0] == NULL || xParaAngleArr[1] == NULL ||
		xParaAngleArr[2] == NULL || xParaAngleArr[3] == NULL)
		return false;
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	//��ʼ���Ǹֹ��
	if (pParam = listParams.GetValue(KEY4C("WAS")))
	{
		for (int i = 0; i < 4; i++)
			xParaAngleArr[i]->m_sGuige.Copy(pParam->value.szStrBuf);
	}
	//����T����ϽǸּ�϶ֵ
	if (pParam = listParams.Add(2))
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
	//����������ֵ��¼��listParams��
	if (pParam = listParams.GetValue(KEY4C("L")))
		pParam->value.fValue = (double)xarrWireGroups[0].wiSpaceL;
	//���ݹ̽���˨����ʼ����˨��λ����
	if (pParam = this->listParams.GetValue(KEY4C("FBN")))
	{
		CStdTemplParaBolt* pParaBolt = NULL;
		for (int i = 0; i < 4; i++)
		{
			if (pParam->value.liValue == 2)
			{
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = R_W_T;
				pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g", 2);
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = R_W_T;
				pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g", 2);
			}
			else if (pParam->value.liValue == 4)
			{	//�̽���˨��,����T�ͽǸ����ӽǸָ�������
				//g1���߽���λ����˨
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = R_W_T;
				pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g1", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = R_W_T;
				pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g1", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g1", 2);
				//g2���߽���λ����˨
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = R_W_T;
				pParaBolt->org.idWorkSonJg = 0;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
				//
				pParaBolt = listParaBolts.Add(0);
				pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_CONNECT_BOLT;
				pParaBolt->org.ciDatumPointCalStyle = 3;
				pParaBolt->org.idBasePart = xParaAngleArr[i]->Id;
				pParaBolt->org.idCrossRod = R_W_T;
				pParaBolt->org.idWorkSonJg = 1;
				pParaBolt->org.ciDatumWing = xParaAngleArr[i]->ciAngleWorkWing;
				memcpy(pParaBolt->org.szWingG, "g2", 2);
				memcpy(pParaBolt->org.szCrossWingG, "g2", 2);
			}
		}
	}
	//���ǰ��ҿ�
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
	pParaBolt->org.idBaseNode = N60_ID;
	pParaBolt->org.ciDatumWing = ciHoleWing;
	//��Ӻ��ҿ�
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
	pParaBolt->org.idBaseNode = N70_ID;
	pParaBolt->org.ciDatumWing = ciHoleWing;
	return true;
}
void CStdComTemplWireStructure::InitComTempl_EB_DANGLE_EARTH_WIRE_ARM3()
{
	ClearTmplParams();
	name.Copy("�����ǺᵣEB�����͵���֧��");
	szSketchImgFile.Copy("ENZ-EB3.bmp");
	this->m_bTmplInitialized = true;
	//�趨���߼ܵ�ʶ��ؼ��ڵ��˼�
	this->xarrKeyRodNodes.ReSize(1);
	xarrKeyRodNodes[0].ciNode0Rod1 = MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod = 1;	//ͨ�����߶˽ڵ�ʶ��
	//0.�ҵ���鶨��,�趨�ҿ׽����Ϣ
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
	xarrWireGroups[0].cnWirePointCount = 2;
	xarrWireGroups[0].cnHoleD = 26;
	xarrWireGroups[0].wiSpaceL = 400;
	xarrWireGroups[0].ciFittingType = CLDSFitting::TYPE_EB;
	xarrWireGroups[0].cnShaftD = ftoi(pEBInfo->dfTrunnionD);
	xarrWireGroups[0].wnFittingWidth = ftoi(pEBInfo->dfBoardL);
	xarrWireGroups[0].sfMasterSwingAngle=-90;
	StrCopy(xarrWireGroups[0].szFittingSizeCode, pEBInfo->szSizeCode, 17);
	StrCopy(xarrWireGroups[0].szUringSizeCode, pURInfo->szSizeCode, 17);
	//��Ʋ���������
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	pParam = listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("T�͹��߽Ǹּ�϶, EB�ҵ��߿��+��϶��mm");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue = xarrWireGroups[0].wnReservedWidth;
	//ģ�ͽڵ�˼�����
	CStdTemplParaNode* pParaNode = NULL;
	pParaNode = listParaNodes.Add(N10_ID);
	pParaNode = listParaNodes.Add(N20_ID);
	pParaNode = listParaNodes.Add(N30_ID);
	pParaNode = listParaNodes.Add(N40_ID);	//T�ͽǸ��м�ҵ�ڵ�
	pParaNode->m_bNecessaryNode = false;
	pParaNode->ciPosCalType = CStdTemplParaNode::SCALE;
	pParaNode->attach_scale = 0.5;
	pParaNode->attachNodeArr[0] = N10_ID;
	pParaNode->attachNodeArr[1] = TMPLOBJID(N10_ID, 'X');
	pParaNode->keyLabelFatherRod = R_W_T;
	//
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->idEndNode = N20_ID;
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->idEndNode = N30_ID;
	//4��ΪT�͹�����ϽǸ�
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(R_W_T, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->idEndNode = TMPLOBJID(N10_ID, 'X');
	pParaAngle->ciSectStyle = 'T';
	pParaAngle->ciMasterIndex = 1;
	pParaAngle->ciAngleLayout = 3;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->normal.ciNormalDirection = 'Z';
	pParaAngle->normal.uidRefRodOfWorkWing = 1;
	pParaAngle->m_bNeedUpdatePosition = true;
	pParaAngle->desStart.uidTouchLineRod = 1;
	pParaAngle->desStart.szXorCtrlWingOffset = "-$1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.uidTouchLineRod = TMPLOBJID(1, 'X');
	pParaAngle->desEnd.szXorCtrlWingOffset = "-$1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//������˨��������ͷ
	//3.��άģ������
	//3.1�ҿ�����(�Ѹ���Translate_XXX()���Զ����� wjh-2019.9.6)
	//3.2���Ӱ�����
	//3.3��׼���ӵ�����
	//N10����ְ�
	MATCH_CONNECT_POINT* pConnPoint = this->listStdConnPoints.Append();
	pConnPoint->idNode = N10_ID;
	pConnPoint->idBaseRod = 1;
	pConnPoint->idRayRodOnAxisY = 2;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_GX1;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_GX1;
	//N10����ְ�
	pConnPoint = this->listStdConnPoints.Append();
	pConnPoint->idNode = N10_ID;
	pConnPoint->idBaseRod = 1;
	pConnPoint->idRayRodOnAxisY = R_W_T;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DM1;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DM1A;
}
bool CStdComTemplWireStructure::TransTemlInstance_EB_DANGLE_EARTH_WIRE_ARM3()
{
	CStdTemplParaAngle* pParaWireAngle = (CStdTemplParaAngle*)listParaRods.GetValue(R_W_T);
	if (pParaWireAngle == NULL)
		return false;
	STDTEMPL_PARAM_ITEM* pParam = NULL;
	//������ϽǸּ�϶
	if (pParam = listParams.GetValue(2))
		pParaWireAngle->wiGroupAngleSpace = (WORD)pParam->value.liSafeVal;
	//
	short snCount = xarrWireGroups[0].cnWirePointCount;
	short siCurrLenOffsetToStart = 0, siCurrLenOffsetToEnd = 0;
	short siWireHoleSpaceL = xarrWireGroups[0].wiSpaceL;
	short siHalfSpaceL = siWireHoleSpaceL / 2;
	if (siWireHoleSpaceL == 0)
		return true;	
	if (xarrWireGroups[0].cnWirePointCount % 2 == 1)
	{	//�м�ҿ�
		CStdTemplParaBolt* pParaBolt = listParaBolts.Add(0);
		pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_EARTH_WIRE;
		pParaBolt->m_iWireGroup = 0;
		pParaBolt->dfHoleDia = xarrWireGroups[0].cnHoleD;
		pParaBolt->through[0].cItemType = 1;
		pParaBolt->through[0].val = R_W_T;
		pParaBolt->org.ciDatumPointCalStyle = 2;
		pParaBolt->org.idBasePart = R_W_T;
		pParaBolt->org.idBaseNode = N40_ID;
		pParaBolt->org.ciDatumWing = 1;
		pParaBolt->org.siLenOffset = 0;
		pParaBolt->normal.ciNormalCalStyle = 1;
		pParaBolt->normal.widBasePart = R_W_T;
		//
		snCount -= 1;
	}
	else
		siCurrLenOffsetToStart = siCurrLenOffsetToEnd = -siWireHoleSpaceL / 2;
	for (int jI = 0; jI < snCount; jI++)
	{	//ǰ��Գƹҿ�
		CStdTemplParaBolt* pParaBolt = listParaBolts.Add(0);
		pParaBolt->m_ciHoleType = CStdTemplParaBolt::HOLE_EARTH_WIRE;
		pParaBolt->m_iWireGroup = 0;
		pParaBolt->dfHoleDia = xarrWireGroups[0].cnHoleD;
		pParaBolt->through[0].cItemType = 1;
		pParaBolt->through[0].val = R_W_T;
		pParaBolt->org.ciDatumPointCalStyle = 2;
		pParaBolt->org.idBasePart = R_W_T;
		pParaBolt->org.idBaseNode = N40_ID;
		pParaBolt->org.ciDatumWing = 1;
		if (jI % 2 == 0)
		{	//���ǰ��(ʼ�˲�)�ҿ�
			siCurrLenOffsetToStart += siWireHoleSpaceL;
			pParaBolt->org.siLenOffset = -siCurrLenOffsetToStart;
		}
		else
		{	//��Ӻ��(�ն˲�)�ҿ�
			siCurrLenOffsetToEnd += siWireHoleSpaceL;
			pParaBolt->org.siLenOffset = siCurrLenOffsetToEnd;
		}
		pParaBolt->normal.ciNormalCalStyle = 1;
		pParaBolt->normal.widBasePart = R_W_T;
	}
	return true;
}