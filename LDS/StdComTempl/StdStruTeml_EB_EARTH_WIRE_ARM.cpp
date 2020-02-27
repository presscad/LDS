#include "stdafx.h"
#include "..\LDS.h"
#include "NodeStdComTempl.h"
#include "WireStruComTempl.h"

//初始化模板参数-IStdComTempl::STDTMPL_STRU_EB_EARTH_WIRE_ARM
void CStdComTemplWireStructure::InitComTempl_EB_EARTH_WIRE_ARM()
{
	ClearTmplParams();
	name.Copy("EB型耐张塔地线挂点支架)");
	szSketchImgFile.Copy("EB5.bmp");
	//2.模型节点杆件描述
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
	//3号为T型挂线组合角钢
	pParaAngle = (CStdTemplParaAngle*)listParaRods.Add(R_W_T, CLS_LINEANGLE);
	pParaAngle->idStartNode = N20_ID;//2;
	pParaAngle->idEndNode = TMPLOBJID(N20_ID, 'X');//-2;
	pParaAngle->ciSectStyle = 'T';
	pParaAngle->ciMasterIndex = 1;
	pParaAngle->ciAngleLayout = 1;
	pParaAngle->normal.ciNormalDirection = 'Z';
	pParaAngle->normal.uidRefRodOfWorkWing = 2;
	pParaAngle->m_bNeedUpdatePosition = pParaAngle->m_bNeedUpdateNormal = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "-$2#T";	//表示负角钢2的肢厚
	pParaAngle->desStart.szYorOthrWingOffset[0] = '0';
	pParaAngle->desEnd.szXorCtrlWingOffset = "-$2#T";	//表示负角钢2的肢厚
	pParaAngle->desEnd.szYorOthrWingOffset[0] = '0';
	//3.三维模型描述
	//3.1挂孔描述
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
	pConnPoint->idBaseRod = 2;
	pConnPoint->idRayRodOnAxisY = R_W_T;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_DM1;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_DM1;
	pConnPoint = this->listStdConnPoints.Append();
	pConnPoint->idNode = N20_ID;
	pConnPoint->idBaseRod = 2;
	pConnPoint->idRayRodOnAxisY = 1;
	pConnPoint->cbMirCreateFlag = MIR_CREATE_X;
	pConnPoint->idStdConnPoint = STDTEMPL_CONNPOINT_XH_GX2;
	pConnPoint->idStdNodePlate = STDTEMPL_NODEPLATE_XH_GX2;
}