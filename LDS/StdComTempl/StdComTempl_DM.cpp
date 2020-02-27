#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"

void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//
void CStdComTemplConnPoint::InitConnPointTempl_DM1()
{
	name = "XH_DM1";
	szSketchImgFile = "DM1.bmp";
	description = "����֧�ܶ˵���";
	widBaseRod = 1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod = 1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	this->xView.ciViewPlaneNormal = 'Z';
	xView.siMinBaseRodAngle = 0;
	xView.siMaxBaseRodAngle = 20;
	this->m_bEnableIntelliConnPointDesign = true;
	rodarr.ZeroPresetMemory();

	//1.�ᵣ
	InitSchemaRod(rodarr[0], 1, -1, 1, 1, 'X', 'R', true);
	//2.T�͹��߽Ǹ�
	InitSchemaRod(rodarr[1], 2, 70, 95, 1, 2, 'R');
	//3.���Ͻ���б��
	InitSchemaRod(rodarr[2], 3, 110, 150, 1, 'X', 'R');
	rodarr.ReSize(3);

	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DM1);
	pMatchPlate->xarrMapConnRods.Set(0, 1);
	pMatchPlate->xarrMapConnRods.Set(1, CMPSTID(2, 1));
	pMatchPlate->xarrMapConnRods.Set(2, CMPSTID(2, 2));
	pMatchPlate->xarrMapConnRods.Set(3, 3);
	pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DM1A);
	pMatchPlate->xarrMapConnRods.Set(0, 1);
	pMatchPlate->xarrMapConnRods.Set(1, CMPSTID(2, 1));
	pMatchPlate->xarrMapConnRods.Set(2, CMPSTID(2, 2));
	pMatchPlate->xarrMapConnRods.Set(3, 3);
}
void CStdComTempl::InitNodePlateTemp_DM1()
{
	szSketchImgFile = "DM1.bmp";
	this->m_bEnableIntelliConnPointDesign = true;
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//�ڵ����
	CStdTemplParaNode* pParaNode = NULL;
	pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//�˼�����
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->cbGroupAngle.bAsynAnglesOdd = true;	//���Ǹֵ��ӽǸ�����ͷ��������ϽǸ��첽
	pParaAngle->cbGroupAngle.ciMasterIndex = 0;		//Ԥ��T-X�ͣ����ջ���ʵ������ƥ��ͬ���������ԣ�
	pParaAngle->ciAngleWorkWing = 'y';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "0";
	pParaAngle->desStart.szYorOthrWingOffset = "-R1#T";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T|]=-R1#T,-R2#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//��ͨ���Ӱ�
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Add(1);
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
	//pParaPlate->cbMirCreateFlag = 0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//��������
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.�ᵣ
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 3, true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].ciConnectWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 2;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPlaneType = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciStyleG = 1;	//g1�ģ���g1=0,��ȡg
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciStyleCrossG = 0;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2�ڲ�����ӽǸ�
	pParaPlate->connrods[1].InitBasicInfo(CMPSTID(2, 1), true, 1, 3, false);	//
	pParaPlate->connrods[1].InitTipInfo(0, 1, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseAngle = 1;	//��ǰ�Ǹ�
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciStyleG = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciStyleCrossG = 1;	//g1�ģ���g1=0,��ȡg
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.widBaseAngle = 1;	//��ǰ�Ǹ�
	pParaPlate->connrods[1].layoutbolts[1].basepoint.ciStyleG = 0;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.ciStyleCrossG = 2;	//g2�ģ���g2=0,��ȡg
	pParaPlate->connrods[1].layoutbolts[1].cnCount = 1;
	pParaPlate->connrods[1].layoutbolts[1].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[1].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[1].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[1].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(2);
	//�������ӽǸ�
	pParaPlate->connrods[2].InitBasicInfo(CMPSTID(2, 2), true, 1, 3, false);
	pParaPlate->connrods[2].InitTipInfo(0, 1, 1);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciStyleG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciStyleCrossG = 1;	//g1�ģ���g1=0,��ȡg
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.ciStyleG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.ciStyleCrossG = 2;	//g2�ģ���g2=0,��ȡg
	pParaPlate->connrods[2].layoutbolts[1].cnCount = 1;
	pParaPlate->connrods[2].layoutbolts[1].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[1].ciLayoutStyle = 1;
	pParaPlate->connrods[2].layoutbolts[1].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[2].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.ciStyleG = BOLT_ORIGIN::POS_WaS;	//W+S
	pParaPlate->connrods[2].layoutbolts[2].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[2].layoutbolts[2].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[2].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[2].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[2].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[2].layoutbolts[2].szLenOffsetOf1stBolt[0] = 0;
	pParaPlate->connrods[2].layoutbolts[2].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(3);
	//3.����֧�Ÿ�
	pParaPlate->connrods[3].InitBasicInfo(3, true, 1, 2, false);
	pParaPlate->connrods[3].InitTipInfo(0, 1, 1, CMPSTID(2, 1));
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].ciConnectWing = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widCrossAngle = CMPSTID(2, 1);
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts[1].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[3].layoutbolts[1].basepoint.widCrossAngle = CMPSTID(2, 1);
	pParaPlate->connrods[3].layoutbolts[1].basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_WaS;
	pParaPlate->connrods[3].layoutbolts[1].cnCount = 0;
	pParaPlate->connrods[3].layoutbolts[1].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[1].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[1].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[3].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(2);
	pParaPlate->connrods.ReSize(4);
}
void CStdComTempl::InitNodePlateTemp_DM1A()
{
	szSketchImgFile = "DM1A.bmp";
	this->m_bEnableIntelliConnPointDesign = true;
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//�ڵ����
	CStdTemplParaNode* pParaNode = NULL;
	pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//�˼�����
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->cbGroupAngle.bAsynAnglesOdd = true;	//���Ǹֵ��ӽǸ�����ͷ��������ϽǸ��첽
	pParaAngle->cbGroupAngle.ciMasterIndex = 1;		//Ԥ��T-Y�ͣ����ջ���ʵ������ƥ��ͬ���������ԣ�
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType = 0;
	pParaAngle->desStart.cTouchLine = 0xff;//'X';�Զ��жϴ�ӹ���֫
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "0";//"-P1#T";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->ciAngleWorkWing = 'x';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType = 0;
	pParaAngle->desStart.cTouchLine = 0xff;//'X';�Զ��жϴ�ӹ���֫
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//��ͨ���Ӱ�
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Add(1);
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
	//pParaPlate->cbMirCreateFlag = 0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//��������
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.�ᵣ
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 3, true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].ciConnectWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 2;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPlaneType = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciStyleG = 1;	//g1�ģ���g1=0,��ȡg
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciStyleCrossG = 0;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2�ڲ�����ӽǸ�
	pParaPlate->connrods[1].InitBasicInfo(CMPSTID(2, 2), true, 1, 3, false);	//
	pParaPlate->connrods[1].InitTipInfo(0, 1, 1);
	pParaPlate->connrods[1].ciConnectWing = 0;
	pParaPlate->connrods[1].cnMaxSingularLineBolts = 4;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseAngle  = 1;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciStyleG = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widCrossAngle	= 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciStyleCrossG = 1;	//g1�ģ���g1=0,��ȡg
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 'y';//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.ciStyleG = 0;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[1].layoutbolts[1].basepoint.ciStyleCrossG = 2;	//g2�ģ���g2=0,��ȡg
	pParaPlate->connrods[1].layoutbolts[1].cnCount = 1;
	pParaPlate->connrods[1].layoutbolts[1].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[1].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[1].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[1].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts[2].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[1].layoutbolts[2].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[1].layoutbolts[2].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[1].layoutbolts[2].basepoint.ciStyleG = BOLT_ORIGIN::POS_WaS;	//W+S
	pParaPlate->connrods[1].layoutbolts[2].basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[1].layoutbolts[2].cnCount = 1;
	pParaPlate->connrods[1].layoutbolts[2].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[2].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[2].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[1].layoutbolts[2].szLenOffsetOf1stBolt[0] = 0;
	pParaPlate->connrods[1].layoutbolts[2].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(3);
	//�������ӽǸ�
	pParaPlate->connrods[2].InitBasicInfo(CMPSTID(2, 1), true, 1, 3, false);
	pParaPlate->connrods[2].InitTipInfo(0, 1, 1);
	pParaPlate->connrods[2].ciConnectWing = 0;
	pParaPlate->connrods[2].cnMaxSingularLineBolts = 4;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widCrossAngle= 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciStyleG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciStyleCrossG = 1;	//g1�ģ���g1=0,��ȡg
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 'x';//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.ciStyleG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[2].layoutbolts[1].basepoint.ciStyleCrossG = 2;	//g2�ģ���g2=0,��ȡg
	pParaPlate->connrods[2].layoutbolts[1].cnCount = 1;
	pParaPlate->connrods[2].layoutbolts[1].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[1].ciLayoutStyle = 1;
	pParaPlate->connrods[2].layoutbolts[1].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[2].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.ciPosCalStyle = 3;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.widBaseAngle = 1;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.widCrossAngle = 0;
	pParaPlate->connrods[2].layoutbolts[2].basepoint.ciStyleG = BOLT_ORIGIN::POS_WaS;	//W+S
	pParaPlate->connrods[2].layoutbolts[2].basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_G;
	pParaPlate->connrods[2].layoutbolts[2].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[2].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[2].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[2].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[2].layoutbolts[2].szLenOffsetOf1stBolt[0] = 0;
	pParaPlate->connrods[2].layoutbolts[2].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(3);
	//3.����֧�Ÿ�
	pParaPlate->connrods[3].InitBasicInfo(3, true, 1, 2, false);
	pParaPlate->connrods[3].InitTipInfo(0, 0, 1, CMPSTID(2, 1));
	pParaPlate->connrods[3].ciConnectWing = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(4);
}
//
void CStdComTemplConnPoint::InitConnPointTempl_DM2()
{
	name = "XH_DM2";
	szSketchImgFile = "DM2.bmp";
	description = "����֧�ܶ˵���";
	widBaseRod = 1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod = 1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	xView.ciViewPlaneNormal = 'Z';
	xView.siMinBaseRodAngle = 0;
	xView.siMaxBaseRodAngle = 20;
	this->m_bEnableIntelliConnPointDesign = true;
	rodarr.ZeroPresetMemory();

	//1.ˮƽ�ᵣ
	InitSchemaRod(rodarr[0], 1, 0, 5, 1, 2, 'A');
	//2.��ֱ���߽Ǹ�
	InitSchemaRod(rodarr[3], 2, 85, 95, 1, 2, 'R');
	//3.��б����
	InitSchemaRod(rodarr[1], 3, 105, 165, 0, 2, 'R');
	//4.��б����
	InitSchemaRod(rodarr[2], 4, 15, 75, 0, 2, 'R');
	rodarr.ReSize(4);
	//
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DM2);
	pMatchPlate->InitDefaultMatchRods(4);
}
void CStdComTempl::InitNodePlateTemp_DM2()
{
	szSketchImgFile = "DM2.bmp";
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->vLocalNearVecS2E.Set(1, 0, 0);
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "g";
	pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "g";
	pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//���Ӱ�
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//��������
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//ˮƽ�ᵣ�Ǹ�
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 5, true);
	pParaPlate->connrods[0].InitTipInfo(2, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.������
	pParaPlate->connrods[1].InitBasicInfo(4, false, 1, 2, false);
	pParaPlate->connrods[1].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.��б����
	pParaPlate->connrods[2].InitBasicInfo(2, true, 1, 3, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1, 2);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.��б����
	pParaPlate->connrods[3].InitBasicInfo(3, true, 1, 3, false);
	pParaPlate->connrods[3].InitTipInfo(0, 0, 1, 2);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(4);
}
//
void CStdComTemplConnPoint::InitConnPointTempl_DM3()
{
	name = "XH_DM3";
	szSketchImgFile = "DM3.bmp";
	description = "����֧�ܶ˵���";
	widBaseRod = 1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod = 1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	ciBaseRodConnState = 1;		//1.�м����ӵ�
	xView.ciViewPlaneNormal = 'Z';
	xView.ciBaseAxisXMode = 1;
	xView.siMinBaseRodAngle = 85;
	xView.siMaxBaseRodAngle = 95;
	this->m_bEnableIntelliConnPointDesign = true;
	rodarr.ZeroPresetMemory();
	//1.T�͹��߽Ǹ�
	InitSchemaRod(rodarr[0], 1, 0, 5, 1, 2, 'A');
	//2.��б����
	InitSchemaRod(rodarr[1], 2, 110, 165, 1, 2, 'R');
	//3.��б����
	InitSchemaRod(rodarr[2], 3, 15, 70, 1, 2, 'R');
	//4.����
	InitSchemaRod(rodarr[3], 4, 85, 95, 0, 2, 'R');
	rodarr.ReSize(4);
	//
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DM3);
	pMatchPlate->InitDefaultMatchRods(4);
}
void CStdComTempl::InitNodePlateTemp_DM3()
{
	szSketchImgFile = "DM3.bmp";
	this->m_bEnableIntelliConnPointDesign = true;
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");
	//
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "g";
	pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "g";
	pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//���Ӱ�
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//��������
	pParaPlate->design.ciDatumLenAxis = 'Y';
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//T�ͽǸ�1�ŽǸ�
	pParaPlate->connrods[0].InitBasicInfo(CMPSTID(1, 1), true, 1, 4, true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//T�ͽǸ�2�ŽǸ�
	pParaPlate->connrods[1].InitBasicInfo(CMPSTID(1, 2), true, 1, 4, true);
	pParaPlate->connrods[1].InitTipInfo(0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//2.��б����
	pParaPlate->connrods[2].InitBasicInfo(2, true, 1, 3, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//3.��б����
	pParaPlate->connrods[3].InitBasicInfo(3, true, 1, 3, false);
	pParaPlate->connrods[3].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//4.������
	pParaPlate->connrods[4].InitBasicInfo(4, false, 1, 2, false);
	pParaPlate->connrods[4].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[4].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}