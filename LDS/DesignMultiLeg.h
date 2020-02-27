#pragma once 

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "./StdComTempl/IStdComTempl.h"
#include "UnifyModelLayer.h"
#include "ModelTree.h"

class CMappingBolts
{
	CHashSet<BOLT_LOCATION*> hashMapBolts;
	struct MATCHSTATE {
		bool matched;
		GEPOINT pos;
		CLDSBolt* pBolt;
		MATCHSTATE(bool _matched = false, const double* pos3d = NULL, CLDSBolt* _pBolt = NULL)
		{
			matched = _matched;
			pBolt = _pBolt;
			if (pos3d != NULL)
				pos.Set(pos3d[0], pos3d[1], pos3d[2]);
		}
	};
	CXhSimpleList < MATCHSTATE>listBoltStates;
	GECS wcs;	//螺栓匹配时的工作平面坐标系
public:
	CMappingBolts(CLDSLineAngle* pAngle = NULL, char ciWorkWingX0Y1 = 0, CLDSNode* pTipNode = NULL);
	void ClearMapping();
	void InitWorkPlaneCS(CLDSLineAngle* pAngle, char ciWorkWingX0Y1, CLDSNode* pTipNode = NULL);
	bool InitExistBoltsInfo(CLDSLineAngle* pAngle, char ciWorkWingX0Y1, CLDSNode* pTipNode = NULL);
	UINT Mapping(CLDSBoltPtr* ppBolts, UINT uiBoltCount, IXhList<BOLT_LOCATION> *pListLocations, double tolerance = 10);
	UINT Mapping(BOLT_LOCATION *pxarrLocations, int count = 1, double tolerance = 10);
};
#ifdef __RAPID_ROUGH_3DMODEL_
struct BASEFOOT{
	WORD wAnchorD,wAnchorHoleD;		//地脚螺栓直径，底板孔径
	WORD S,W;						//螺栓间距及底板宽度
	BYTE cnMainAngleBoltNum,cnRows;	//主材连接螺栓数及布置排数
	WORD wRibWidth,wRibOuterHeight,wRibInnerHeight;		//肋板宽度(底板焊接边),高度（分外侧，内侧）
	BYTE ciBaseThick,ciShoeThick,ciRibThick,ciRibMargin;//底板厚度、靴板厚度，肋板厚度、边距
	void CopyFromFoundationParams(FOUNDATION_PARA* pFoundationPara);
};
struct QUADLEG{
	union KEY{
		struct{
			char ciBodySerial;	//主体号
			char ciLegSerial;	//接腿号
			char ciQuad;		//象限号
		}pairs;
		DWORD id;
	}keyid;
	char set_ciBodySerial(char ciSerial){return keyid.pairs.ciBodySerial=ciSerial;}
	char get_ciBodySerial(){return keyid.pairs.ciBodySerial;}
	__declspec(property(put=set_ciBodySerial,get=get_ciBodySerial)) char ciBodySerial;
	char set_ciLegSerial(char ciSerial){return keyid.pairs.ciLegSerial=ciSerial;}
	char get_ciLegSerial(){return keyid.pairs.ciLegSerial;}
	__declspec(property(put=set_ciLegSerial,get=get_ciLegSerial)) char ciLegSerial;
	char set_ciLegQuad(char ciQuad){return keyid.pairs.ciQuad=ciQuad;}
	char get_ciLegQuad(){return keyid.pairs.ciQuad;}
	__declspec(property(put=set_ciLegQuad,get=get_ciLegQuad)) char ciLegQuad;
	//腿身过渡隔面正侧面横材中间八字板所需参数
	CLDSNode* pBaseNode,*pVFrontTopNode,*pVSideTopNode;
	CLDSLineAngle* pPostAngle;	//角线上的腿部主材（与基础节点相连的一段主材）
	CLDSLineAngle* pVFrontAngle;//与1象限正面V斜材相同占位的V斜材角钢
	CLDSLineAngle* pVSideAngle; //与1象限侧面V斜材相同占位的V斜材角钢
	CLDSLineAngle* pHoriFrontAngle;	//中间隔面八字板所在的那段横材
	CLDSLineAngle* pHoriSideAngle;	//中间隔面八字板所在的那段横材
	//角脊线两侧腿身连接板所需参数
	CLDSNode* pPostTopNode;
	CLDSLineAngle* pUpPostAngle;//角线上的上部腿部主材（与腿身过渡面相连的一段主材，大多与pPostAngle相同）
	CLDSLineAngle* pHoriRidgeFrontAngle;//与角脊线相连的那段正面横材（大多与pHoriFrontAngle相同）
	CLDSLineAngle* pHoriRidgeSideAngle;	//与角脊线相连的那段侧面横材（大多与pHoripSideAngle相同）
	CLDSLineAngle* pRidgeFrontXieAngle;	//正面与角脊线相连的塔身斜材
	CLDSLineAngle* pRidgeSideXieAngle;	//侧面与角脊线相连的塔身斜材
	QUADLEG(){
		keyid.id=0;
		pBaseNode=pVFrontTopNode=pVSideTopNode=pPostTopNode=NULL;
		pPostAngle=pUpPostAngle=pVFrontAngle=pVSideAngle=NULL;
		pHoriFrontAngle=pHoriSideAngle=pHoriRidgeFrontAngle=pHoriRidgeSideAngle=NULL;
	}
	void SetKey(DWORD key){keyid.id=key;}
};
int RetrieveExistBolts(BOLTSET& boltset,CLDSLineAngle* pAngle,CLDSNode* pDatumNode,char ciWingX0Y1,WORD wiSearchRadius);
struct MULTILEG_HORI_K_CONNPLATE{
	GEPOINT vPlaneNormal;
	CLDSNode* pBaseNode;
	CLDSLineAngle *pBaseHoriAngle;
	CLDSLineAngle *pLeftLegAngle, *pRightLegAngle;
	CLDSLineAngle *pLeftUpperAngle, *pRightUpperAngle;
	char ciHeightSerial;		//呼高本体号
	char ciFront0OrSide1;		//0.前面;1.侧面
	BYTE ciHoriAngleBoltNum;	//横材上螺栓数
	WORD wiHoriAngleBoltSpace;	//横材（基准材）上螺栓间距
	MULTILEG_HORI_K_CONNPLATE(){
		memset(this,0,sizeof(MULTILEG_HORI_K_CONNPLATE));
		ciHoriAngleBoltNum=4;
		wiHoriAngleBoltSpace=80;
	}
	bool JudgeConnectTipAndWing(CLDSLineAngle* pAngle,BYTE* piStart0End1,BYTE* piWingX0Y1);
};
struct MULTILEG_POST_CONNPLATE{
	GEPOINT vPlaneNormal;
	CLDSNode* pBaseNode;
	CLDSLineAngle *pPostAngle;		//腿部主材
	CLDSLineAngle *pUpXieAngle;		//塔身连接斜材;
	CLDSLineAngle *pHoriAngle;		//横隔面材
	CLDSLineAngle *pLegBraceAngle;	//腿部连接辅材或塔身换接点下侧的塔身斜材
	char ciHeightSerial;		//呼高本体号
	char ciFront0OrSide1;		//0.连接在主材左侧;1.连接在主材右侧ciOnLeft0OrRight1
	BYTE ciPostAngleBoltNum;	//横材上螺栓数
	WORD wiPostAngleBoltSpace;	//横材（基准材）上螺栓间距
	MULTILEG_POST_CONNPLATE(){
		memset(this,0,sizeof(MULTILEG_POST_CONNPLATE));
		ciPostAngleBoltNum=3;
		wiPostAngleBoltSpace=80;
	}
	bool JudgeConnectTipAndWing(CLDSLineAngle* pAngle,BYTE* piStart0End1,BYTE* piWingX0Y1);
};
struct TRUNK_POST_KJOINT_CONNPLATE : public MULTILEG_POST_CONNPLATE {
	bool blColinearJoint;	//上下主材共线，需要设计相关接头
	CLDSLineAngle *pUpperPostAngle;		//塔身换接点上侧主材
	CLDSLineAngle* get_pDownXieAngle() { return pLegBraceAngle; }
	CLDSLineAngle* set_pDownXieAngle(CLDSLineAngle* pXieAngle) { return pLegBraceAngle= pXieAngle; }
	__declspec(property(put = set_pDownXieAngle, get = get_pDownXieAngle)) CLDSLineAngle* pDownXieAngle;
	bool RetrieveCurrHeightKJointInfo(IModel* pBelongModel, char ciFront0Side1);
	bool RetrieveCurrHeightKJointDownXieRod(IModel* pBelongModel);
	bool RetrieveCurrHeightKJointUpXieRod(IModel* pBelongModel);
	TRUNK_POST_KJOINT_CONNPLATE() { blColinearJoint = false; pUpperPostAngle = NULL; }
};
class CTowerMultiLegDesigner{
	CTower* m_pTower;
	CTowerModelTree modeltree;
	CHashListEx<QUADLEG>hashQuadLegs;
	CIntelliModelDesigner* m_pDesignerContext;
	CLDSPlate* DesignVPlate(CLDSNode* pBaseNode, CLDSLineAngle* pVBaseAngle, char ciWorkWingX0Y1, CLDSLinePartPtr* pConnAngles, int count);
protected:
	void RetrieveMultiLegs();
	bool AddBaseFootPlates(CLDSLinePart* pBasePostRod,BASEFOOT* pFoot);
	bool CorrectLegPostKNodeConnPlate(MULTILEG_POST_CONNPLATE& despara,bool bMirCreate=true);
	bool CorrectLegHoriKNodeConnPlate(MULTILEG_HORI_K_CONNPLATE& despara,bool bMirCreate=true);
	bool DesignAllLegTransitConnPlates(bool bDesignPostK=true,bool bDesignHoriK=true);
	bool DesignLegVPlates(QUADLEG* pQuadLeg);
	static void CorrectLegAnglesLayout(QUADLEG* pLeg);
	bool CorrectTrunkPostKNodeConnPlate(TRUNK_POST_KJOINT_CONNPLATE& despara, ROD_PTR* xarrDownRods, UINT count, bool bMirCreate = true);
	bool CorrectTrunkPostJoints(TRUNK_POST_KJOINT_CONNPLATE& despara, ROD_PTR* xarrDownRods, UINT count, bool bMirCreate = true);
	bool CorrectTrunkPostSlopeConnPlate(TRUNK_POST_KJOINT_CONNPLATE& despara, ROD_PTR* xarrDownRods, UINT count, bool bMirCreate = true);
#ifdef __RAPID_ROUGH_3DMODEL_
	bool DesignNeckNode(CLDSNode* pNeckNode,CTower* pTower, CIntelliModelDesigner* pDesignerContext = NULL);
	bool DesignTrunkPostNeckShiftPlates(CTower* pTower, CIntelliModelDesigner* pDesignerContext = NULL);
#endif
public:
	CTowerMultiLegDesigner(CTower* pTower=NULL);
	void AddOrUpdateBaseFoot(BASEFOOT* pFoot, bool blOnlyUpdateBaseFoot = false);
	bool DesignAllSubLegs(CTower* pTower, char cSubConnDesignType = 0, CIntelliModelDesigner* pDesignerContext = NULL);
#ifdef __RAPID_ROUGH_3DMODEL_
	bool DesignTrunkPostShiftJoints(CTower* pTower, CIntelliModelDesigner* pDesignerContext = NULL);
#endif
};
#endif
