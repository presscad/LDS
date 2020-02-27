#include "stdafx.h"
#include "..\LDS.h"
#include "NodeStdComTempl.h"
#include "WireStruComTempl.h"

//��ʼ��ģ�����
//IStdComTempl::STDTMPL_STRU_EB_WIRE_V_ARM1
void CStdComTemplWireStructure::InitComTempl_EB_WIRE_V_ARM1()
{
	ClearTmplParams();
	name.Copy("EB�͵��ҵ�ֱ��������V������");
	szSketchImgFile.Copy("EB8_1.bmp");
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
}
//IStdComTempl::STDTMPL_STRU_EB_WIRE_V_ARM2
void CStdComTemplWireStructure::InitComTempl_EB_WIRE_V_ARM2()
{
	ClearTmplParams();
	name.Copy("EB��˫�ҵ�ֱ��������V������");
	szSketchImgFile.Copy("EB8_2.bmp");
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
}
//IStdComTempl::STDTMPL_STRU_EB_WIRE_V_ARM3
void CStdComTemplWireStructure::InitComTempl_EB_WIRE_V_ARM3()
{
	ClearTmplParams();
	name.Copy("EB�͵��ҵ�ֱ��������V������(���κᵣ)");
	szSketchImgFile.Copy("EB9.bmp");
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
	pParaNode->attachNodeArr[0] = N10_ID;//2;
	pParaNode->attachNodeArr[1] = TMPLOBJID(N10_ID, 'X');
	pParaNode->keyLabelFatherRod = R_W_T;

	pParaRod = listParaRods.Add(1, CLS_LINEANGLE);
	pParaRod->idStartNode = N10_ID;//3;
	pParaRod->siMinRayAngle = 170;
	pParaRod->siMaxRayAngle = 190;
	pParaRod = listParaRods.Add(2);
	pParaRod->idStartNode = N10_ID;
	pParaRod->idEndNode = N20_ID;
	pParaRod = listParaRods.Add(3);
	pParaRod->idStartNode = N20_ID;	//1;
	pParaRod->siMinRayAngle = 170;
	pParaRod->siMaxRayAngle = 190;
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
	pParaRod = listParaRods.Add(5);//�ᵣ�����ϸ˼�
	pParaRod->idStartNode = N20_ID;	//1;
	pParaRod->idEndNode = TMPLOBJID(N20_ID, 'X');		//2;
	pParaRod = listParaRods.Add(6);
	pParaRod->idStartNode = N30_ID;	//1;
	pParaRod->siMinRayAngle = 130;
	pParaRod->siMaxRayAngle = 145;
	pParaRod = listParaRods.Add(7);
	pParaRod->idStartNode = N30_ID;	//1;
	pParaRod->idEndNode = TMPLOBJID(N20_ID, 'X');
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
	pConnPoint->idNode = N30_ID;
	pConnPoint->idBaseRod = R_W_T;
	pConnPoint->idRayRodOnAxisY = 6;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DM2;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DM2;
}
//IStdComTempl::STDTMPL_STRU_EB_WIRE_V_ARM4
void CStdComTemplWireStructure::InitComTempl_EB_WIRE_V_ARM4()
{
	ClearTmplParams();
	name.Copy("EB�͵��ҵ�ֱ��������V������");
	szSketchImgFile.Copy("EB10.bmp");
	//2.ģ�ͽڵ�˼�����
	CStdTemplParaNode* pParaNode;
	CStdTemplParaRod* pParaRod;
	CStdTemplParaAngle* pParaAngle;
	pParaNode = listParaNodes.Add(N10_ID);
	pParaNode = listParaNodes.Add(N20_ID);
	pParaNode = listParaNodes.Add(N30_ID);
	pParaNode = listParaNodes.Add(N40_ID);
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
	pParaRod->idEndNode = N30_ID;
	//3��ΪT�͹�����ϽǸ�
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(R_W_T, CLS_LINEANGLE);
	pParaAngle->idStartNode = N20_ID;//2;
	pParaAngle->idEndNode = TMPLOBJID(N20_ID, 'X');//-2;
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
	pConnPoint->idNode = N20_ID;
	pConnPoint->idBaseRod = 1;
	pConnPoint->idRayRodOnAxisY = R_W_T;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DM1;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DM1;
	pConnPoint = this->listStdConnPoints.Append();
	pConnPoint->idNode = N20_ID;
	pConnPoint->idBaseRod = 1;
	pConnPoint->idRayRodOnAxisY = 2;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_GX2;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_GX2;
}
//IStdComTempl::STDTMPL_STRU_EB_WIRE_V_ARM5
void CStdComTemplWireStructure::InitComTempl_EB_WIRE_V_ARM5()
{
	ClearTmplParams();
	name.Copy("EB�͵��ҵ�ֱ��������V������(����)");
	szSketchImgFile.Copy("EB11.bmp");
}
//IStdComTempl::STDTMPL_STRU_EB_WIRE_V_ARM6
void CStdComTemplWireStructure::InitComTempl_EB_WIRE_V_ARM6()
{
	ClearTmplParams();
	name.Copy("EB�͵��ҵ�ֱ��������V������(���ർ��V���ڲ�ҵ�)");
	szSketchImgFile.Copy("EB12.bmp");
}
