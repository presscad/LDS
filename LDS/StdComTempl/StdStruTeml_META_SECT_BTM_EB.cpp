#include "stdafx.h"
#include "..\LDS.h"
#include "NodeStdComTempl.h"
#include "WireStruComTempl.h"
#include "Fitting.h"

//��ʼ��ģ�����
//IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_EB
void CStdComTemplWireStructure::InitComTempl_META_SECT_BTM_1_EB()
{
	ClearTmplParams();
	name.Copy("ֱ��������ĵ�EB�ҵ�");
	szSketchImgFile.Copy("CZC-EB1.bmp");
	this->m_bTmplInitialized=true;
	//�趨���߼ܵ�ʶ��ؼ��ڵ��˼�
	xarrKeyRodNodes.ReSize(1);
	xarrKeyRodNodes[0].ciNode0Rod1=MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod=1;	//ͨ�����߶˽ڵ�ʶ��
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
	xarrWireGroups[0].cnWirePointCount = 1;
	xarrWireGroups[0].wiSpaceL = 0;
	xarrWireGroups[0].ciFittingType = CLDSFitting::TYPE_EB;
	xarrWireGroups[0].cnShaftD = ftoi(pEBInfo->dfTrunnionD);
	xarrWireGroups[0].wnFittingWidth = ftoi(pEBInfo->dfBoardL);
	StrCopy(xarrWireGroups[0].szFittingSizeCode, pEBInfo->szSizeCode, 17);
	StrCopy(xarrWireGroups[0].szUringSizeCode, pURInfo->szSizeCode, 17);
	//1.��׼�����Ʋ���������
	STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"WorkPlaneCS(1,1,'Z')");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//��߿��
	pParam=listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("T�͹��߽Ǹּ�϶, EB�ҵ��߿��+��϶��mm");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=xarrWireGroups[0].wnReservedWidth;	//���ܸ��̶�ֵ84������������߳�ͻ
	//���߶̽ǸֵĹ��
	pParam=listParams.Add(KEY4C("WAS"));
	pParam->sLabel.Copy("WIREANGLE_SIZE");
	pParam->sNotes.Copy("T�͹��߽Ǹֹ��mm");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_STRING;
	StrCopy(pParam->value.szStrBuf,"125x10",96);
	//���߶̽ǸֵĹ̽���˨
	pParam=listParams.Add(KEY4C("FBN"));
	pParam->sLabel.Copy("FIX_CONN_BOLTS_N");
	pParam->sNotes.Copy("���߶̽ǸֵĹ̽���˨��");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;
	pParam->value.liValue=this->m_cnFixWireAngleConnBoltsN=2;
	//2.ģ�ͽڵ�˼�����
	CStdTemplParaNode* pParaNode=listParaNodes.Add(N10_ID);
	pParaNode->m_bNecessaryNode = true;
	pParaNode=listParaNodes.Add(N20_ID);
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::SCALE;
	pParaNode->attach_scale=0.5;
	pParaNode->attachNodeArr[0]= N10_ID;//2;
	pParaNode->attachNodeArr[1]=TMPLOBJID(N10_ID,'X');
	pParaNode->keyLabelFatherRod=1;
	//�˼�����
	const int ANG_ARMPOST = 2;
	//1��Ϊ���߽Ǹֵ����ӻ�׼T����ϽǸ�
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
	pParaAngle->desStart.szXorCtrlWingOffset="-$2#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szYorOthrWingOffset[0]='0';
	pParaAngle->desEnd.uidTouchLineRod = TMPLOBJID(ANG_ARMPOST, 'X');
	pParaAngle->desEnd.szXorCtrlWingOffset="-$2#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
	//�ᵣ�Ҹ����ģ���Ϊ�����������ھ���1��T����ϽǸֵ�֫���� wjh-2019.9.10
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_ARMPOST, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->m_bNecessaryRod = false;
	//3��4��Ϊ����ǰ�����������������߶̽Ǹ�
	const int ANG_FL=3;
	const int ANG_BL=4;
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(ANG_FL,CLS_LINEANGLE);
	pParaAngle->idStartNode= N20_ID;//2;
	pParaAngle->ciSectStyle='L';
	pParaAngle->blShortAngle=true;
	pParaAngle->attachangle.idBaseAngle=1;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode=N20_ID;
	pParaAngle->attachangle.ciDatumAngleWing='X';
	pParaAngle->attachangle.ciNormalOffset=0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout=2;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection='Z';	//ʼ->����'X'�����
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->m_bNeedUpdateNormal=true;
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//������˨λ�ü���
	//pParaAngle->normal.ciNormalDirection='Z';
	//pParaAngle->normal.uidRefRodOfWorkWing=2;
	pParaAngle->desStart.szYorOthrWingOffset="-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szXorCtrlWingOffset[0]='0';
	pParaAngle->desEnd.szYorOthrWingOffset="-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szXorCtrlWingOffset[0]='0';
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(ANG_BL,CLS_LINEANGLE);
	pParaAngle->idStartNode= N20_ID;
	//pParaAngle->idEndNode  = TMPLOBJID(N20_ID,'X');//-2;
	pParaAngle->ciSectStyle='L';
	pParaAngle->blShortAngle = true;
	pParaAngle->attachangle.idBaseAngle=1;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode=N20_ID;
	pParaAngle->attachangle.ciDatumAngleWing='X';
	pParaAngle->attachangle.ciNormalOffset=0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout=3;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection='Z';	//ʼ->����'X'�����
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->m_bNeedUpdateNormal=true;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//������˨λ�ü���
	//pParaAngle->normal.ciNormalDirection='Z';
	//pParaAngle->normal.uidRefRodOfWorkWing=2;
	pParaAngle->desStart.szXorCtrlWingOffset="-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szYorOthrWingOffset[0]='0';
	pParaAngle->desEnd.szXorCtrlWingOffset="-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
	//3.��άģ������
	//���Ӱ�����
	MATCH_CONNECT_POINT* pConnPoint=this->listStdConnPoints.Append();
	pConnPoint->idNode = N10_ID;
	pConnPoint->idBaseRod= ANG_ARMPOST;
	pConnPoint->idRayRodOnAxisY = 1;
	pConnPoint->cbMirCreateFlag=MIR_CREATE_X;
	pConnPoint->idStdConnPoint=STDTEMPL_CONNPOINT_XH_DM1;
	pConnPoint->idStdNodePlate=STDTEMPL_NODEPLATE_XH_DM1;
}
//����ģ����Ʋ���
bool CStdComTemplWireStructure::TransTemlInstance_SECT_BTM_1_EB()
{
	//3��4��Ϊ����ǰ�����������������߶̽Ǹ�
	const int ANG_FL=3;
	const int ANG_BL=4;
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
	//���ݽ�߿�����ö̽Ǹ�֫��ƫ����
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
			{	//�̽���˨��Ϊ4,����T�ͽǸ����ӽǸָ�������
				//g1���߽���λ����˨
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
				//g2���߽���λ����˨
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
	return  true;
}
//////////////////////////////////////////////////////////////////////////
//IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB
void CStdComTemplWireStructure::InitComTempl_META_SECT_BTM_2_EB()
{
	ClearTmplParams();
	name.Copy("ֱ���������˫EB�ҵ�");
	szSketchImgFile.Copy("CZC-S-EB.bmp");
	this->m_bTmplInitialized=true;
	//�趨���߼ܵ�ʶ��ؼ��ڵ��˼�
	this->xarrKeyRodNodes.ReSize(1);
	xarrKeyRodNodes[0].ciNode0Rod1=MAP_RODnNODE::TYPE_NODE;
	xarrKeyRodNodes[0].idNodeOrRod=1;	//ͨ�����߶˽ڵ�ʶ��
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
	this->m_cnWireGroupCount=1;
	xarrWireGroups[0].cnWirePointCount = 2;
	xarrWireGroups[0].wiSpaceL = 400;
	xarrWireGroups[0].ciFittingType = CLDSFitting::TYPE_EB;	
	xarrWireGroups[0].cnShaftD = ftoi(pEBInfo->dfTrunnionD);
	xarrWireGroups[0].wnFittingWidth = ftoi(pEBInfo->dfBoardL);
	StrCopy(xarrWireGroups[0].szFittingSizeCode, pEBInfo->szSizeCode, 17);
	StrCopy(xarrWireGroups[0].szUringSizeCode, pURInfo->szSizeCode, 17);
	//1.��׼�����Ʋ���������
	STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"WorkPlaneCS(1,1,'Y','Z')");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//��߿��
	pParam=listParams.Add(2);
	pParam->sLabel.Copy("EBW");
	pParam->sNotes.Copy("T�͹��߽Ǹּ�϶, EB�ҵ��߿��+��϶��mm");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=xarrWireGroups[0].wnReservedWidth;	//���ܸ��̶�ֵ84������������߳�ͻ
	//��������(���ڼ���ƫ�ƽڵ�λ�ã�ֵ��Դ��xarrWireGroups[0].wiSpaceL)
	pParam = listParams.Add(KEY4C("L"));
	pParam->sLabel.Copy("SpaceL");
	pParam->sNotes.Copy("ͬ��ҿ׼�Ŀ�������");
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//���߶̽ǸֵĹ��
	pParam=listParams.Add(KEY4C("WAS"));
	pParam->sLabel.Copy("WIREANGLE_SIZE");
	pParam->sNotes.Copy("T�͹��߽Ǹֹ��mm");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_STRING;
	StrCopy(pParam->value.szStrBuf,"125x10",96);
	//���߶̽ǸֵĹ̽���˨
	pParam=listParams.Add(KEY4C("FBN"));
	pParam->sLabel.Copy("FIX_CONN_BOLTS_N");
	pParam->sNotes.Copy("���߶̽ǸֵĹ̽���˨��");
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;//VALUETYPE_LONG;
	pParam->value.liValue=this->m_cnFixWireAngleConnBoltsN=2;
	//2.ģ�ͽڵ�˼�����
	CStdTemplParaNode* pParaNode=listParaNodes.Add(N10_ID);
	pParaNode->m_bNecessaryNode = true;
	//��׼�Ǹ��е�
	pParaNode=listParaNodes.Add(N20_ID);
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::SCALE;
	pParaNode->attach_scale=0.5;
	pParaNode->attachNodeArr[0]= N10_ID;
	pParaNode->attachNodeArr[1]=TMPLOBJID(N10_ID,'X');
	pParaNode->keyLabelFatherRod=1;
	//ǰ��EB�ҵ�λ��
	pParaNode=listParaNodes.Add(N30_ID);
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType = CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0] = N20_ID;
	pParaNode->attachNodeArr[1] = N10_ID;
	pParaNode->attachNodeArr[2] = N20_ID;
	strcpy(pParaNode->offset_expression, "L/2");
	pParaNode->keyLabelFatherRod=1;
	//���EB�ҵ�λ��
	pParaNode = listParaNodes.Add(N40_ID);
	pParaNode->m_bNecessaryNode = false;
	pParaNode->ciPosCalType = CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0] = N20_ID;
	pParaNode->attachNodeArr[1] = N20_ID;
	pParaNode->attachNodeArr[2] = N10_ID;
	strcpy(pParaNode->offset_expression, "L/2");
	pParaNode->keyLabelFatherRod = 1;
	//�˼�����
	const int ANG_ARMPOST = 2;
	//1��Ϊ���߽Ǹֵ����ӻ�׼T����ϽǸ�
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode=N10_ID;//1;
	pParaAngle->idEndNode=TMPLOBJID(N10_ID,'X');
	pParaAngle->ciSectStyle='T';
	pParaAngle->ciMasterIndex=1;
	pParaAngle->ciAngleLayout=3;	//ciAngleWorkWing='x'��X֫����
	pParaAngle->normal.ciNormalDirection='Z';
	pParaAngle->normal.uidRefRodOfWorkWing=ANG_ARMPOST;
	pParaAngle->m_bNeedUpdateNormal = false;
	pParaAngle->m_bNeedUpdatePosition=false;
	pParaAngle->desStart.uidTouchLineRod = ANG_ARMPOST;
	pParaAngle->desStart.szXorCtrlWingOffset="-$2#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szYorOthrWingOffset[0]='0';
	pParaAngle->desStart.uidTouchLineRod = TMPLOBJID(ANG_ARMPOST, 'X');
	pParaAngle->desEnd.szXorCtrlWingOffset="-$2#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
	//�ᵣ�Ҹ����ģ���Ϊ�����������ھ���1��T����ϽǸֵ�֫���� wjh-2019.9.10
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(ANG_ARMPOST, CLS_LINEANGLE);
	pParaAngle->idStartNode = N10_ID;
	pParaAngle->m_bNecessaryRod = false;
	//3��4��Ϊǰ��ҵ�����������̽Ǹ�
	const int ANG_FL1=3;
	const int ANG_FL2=4;
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(ANG_FL1,CLS_LINEANGLE);
	pParaAngle->idStartNode= N30_ID;
	pParaAngle->ciSectStyle='L';
	pParaAngle->attachangle.idBaseAngle=1;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode=N30_ID;
	pParaAngle->attachangle.ciDatumAngleWing='X';
	pParaAngle->attachangle.ciNormalOffset=0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout=2;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection='Z';	//ʼ->����'X'�����
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->m_bNeedUpdateNormal=false;
	pParaAngle->blShortAngle=true;
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//������˨λ�ü���
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->desStart.szYorOthrWingOffset="-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szXorCtrlWingOffset[0]='0';
	pParaAngle->desEnd.szYorOthrWingOffset="-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szXorCtrlWingOffset[0]='0';
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(ANG_FL2,CLS_LINEANGLE);
	pParaAngle->idStartNode= N30_ID;
	pParaAngle->ciSectStyle='L';
	pParaAngle->attachangle.idBaseAngle=1;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode=N30_ID;
	pParaAngle->attachangle.ciDatumAngleWing='X';
	pParaAngle->attachangle.ciNormalOffset=0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout=3;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection='Z';	//ʼ->����'X'�����
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->m_bNeedUpdateNormal=false;
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//������˨λ�ü���
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->desStart.szXorCtrlWingOffset="-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szYorOthrWingOffset[0]='0';
	pParaAngle->desEnd.szXorCtrlWingOffset="-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
	//5��6��Ϊ���ҵ�����������̽Ǹ�
	const int ANG_BL1=5;
	const int ANG_BL2=6;
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(ANG_BL1,CLS_LINEANGLE);
	pParaAngle->idStartNode= N40_ID;
	pParaAngle->ciSectStyle='L';
	pParaAngle->attachangle.idBaseAngle=1;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode=N40_ID;
	pParaAngle->attachangle.ciDatumAngleWing='X';
	pParaAngle->attachangle.ciNormalOffset=0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout=2;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection='Z';	//ʼ->����'X'�����
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->m_bNeedUpdateNormal=false;
	pParaAngle->blShortAngle=true;
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//������˨λ�ü���
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->desStart.szYorOthrWingOffset="-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szXorCtrlWingOffset[0]='0';
	pParaAngle->desEnd.szYorOthrWingOffset="-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szXorCtrlWingOffset[0]='0';
	pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(ANG_BL2,CLS_LINEANGLE);
	pParaAngle->idStartNode= N40_ID;
	pParaAngle->ciSectStyle='L';
	pParaAngle->attachangle.idBaseAngle=1;		//ǰ��ᵣ���ĽǸ�
	pParaAngle->attachangle.idBaseNode=N40_ID;
	pParaAngle->attachangle.ciDatumAngleWing='X';
	pParaAngle->attachangle.ciNormalOffset=0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout=3;	//��������(1�Ż�׼�Ǹ�ʼ->��Ϊ����)
	pParaAngle->attachangle.ciLenDirection='Z';	//ʼ->����'X'�����
	pParaAngle->m_bNeedUpdatePosition=true;
	pParaAngle->m_bNeedUpdateNormal=false;
	pParaAngle->wiStartOddCalStyle=pParaAngle->wiEndOddCalStyle=1;	//������˨λ�ü���
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->desStart.szXorCtrlWingOffset="-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desStart.szYorOthrWingOffset[0]='0';
	pParaAngle->desEnd.szXorCtrlWingOffset="-P1#T";	//��ʾ���Ǹ�2��֫��
	pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
	//3.��άģ������
	//���Ӱ�����
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
	//��ʼ���Ǹֹ��
	if (pParam = listParams.GetValue(KEY4C("WAS")))
	{
		for(int i=0;i<4;i++)
			xParaAngleArr[i]->m_sGuige.Copy(pParam->value.szStrBuf);
	}
	//���ݽ�߿�����ö̽Ǹ�֫��ƫ����
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
			{	//�̽���˨��Ϊ2,����T�ͽǸ����ӽǸָ���1��
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
			{	//�̽���˨��Ϊ4,����T�ͽǸ����ӽǸָ�������
				//g1���߽���λ����˨
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
				//g2���߽���λ����˨
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
	//���ݹҿײ�����ʼ���ҿ׶�λ����
	//if (xarrWireGroups[0].cnWirePointCount == 2)
	{
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
		pParaBolt->org.idBaseNode = N30_ID;
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
		pParaBolt->org.idBaseNode = N40_ID;
		pParaBolt->org.ciDatumWing = ciHoleWing;
	}
	return  true;
}