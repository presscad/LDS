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
	GECS wcs;	//��˨ƥ��ʱ�Ĺ���ƽ������ϵ
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
	WORD wAnchorD,wAnchorHoleD;		//�ؽ���˨ֱ�����װ�׾�
	WORD S,W;						//��˨��༰�װ���
	BYTE cnMainAngleBoltNum,cnRows;	//����������˨������������
	WORD wRibWidth,wRibOuterHeight,wRibInnerHeight;		//�߰���(�װ庸�ӱ�),�߶ȣ�����࣬�ڲࣩ
	BYTE ciBaseThick,ciShoeThick,ciRibThick,ciRibMargin;//�װ��ȡ�ѥ���ȣ��߰��ȡ��߾�
	void CopyFromFoundationParams(FOUNDATION_PARA* pFoundationPara);
};
struct QUADLEG{
	union KEY{
		struct{
			char ciBodySerial;	//�����
			char ciLegSerial;	//���Ⱥ�
			char ciQuad;		//���޺�
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
	//������ɸ������������м���ְ��������
	CLDSNode* pBaseNode,*pVFrontTopNode,*pVSideTopNode;
	CLDSLineAngle* pPostAngle;	//�����ϵ��Ȳ����ģ�������ڵ�������һ�����ģ�
	CLDSLineAngle* pVFrontAngle;//��1��������Vб����ͬռλ��Vб�ĽǸ�
	CLDSLineAngle* pVSideAngle; //��1���޲���Vб����ͬռλ��Vб�ĽǸ�
	CLDSLineAngle* pHoriFrontAngle;	//�м������ְ����ڵ��Ƕκ��
	CLDSLineAngle* pHoriSideAngle;	//�м������ְ����ڵ��Ƕκ��
	//�Ǽ��������������Ӱ��������
	CLDSNode* pPostTopNode;
	CLDSLineAngle* pUpPostAngle;//�����ϵ��ϲ��Ȳ����ģ������������������һ�����ģ������pPostAngle��ͬ��
	CLDSLineAngle* pHoriRidgeFrontAngle;//��Ǽ����������Ƕ������ģ������pHoriFrontAngle��ͬ��
	CLDSLineAngle* pHoriRidgeSideAngle;	//��Ǽ����������Ƕβ����ģ������pHoripSideAngle��ͬ��
	CLDSLineAngle* pRidgeFrontXieAngle;	//������Ǽ�������������б��
	CLDSLineAngle* pRidgeSideXieAngle;	//������Ǽ�������������б��
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
	char ciHeightSerial;		//���߱����
	char ciFront0OrSide1;		//0.ǰ��;1.����
	BYTE ciHoriAngleBoltNum;	//�������˨��
	WORD wiHoriAngleBoltSpace;	//��ģ���׼�ģ�����˨���
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
	CLDSLineAngle *pPostAngle;		//�Ȳ�����
	CLDSLineAngle *pUpXieAngle;		//��������б��;
	CLDSLineAngle *pHoriAngle;		//������
	CLDSLineAngle *pLegBraceAngle;	//�Ȳ����Ӹ��Ļ������ӵ��²������б��
	char ciHeightSerial;		//���߱����
	char ciFront0OrSide1;		//0.�������������;1.�����������Ҳ�ciOnLeft0OrRight1
	BYTE ciPostAngleBoltNum;	//�������˨��
	WORD wiPostAngleBoltSpace;	//��ģ���׼�ģ�����˨���
	MULTILEG_POST_CONNPLATE(){
		memset(this,0,sizeof(MULTILEG_POST_CONNPLATE));
		ciPostAngleBoltNum=3;
		wiPostAngleBoltSpace=80;
	}
	bool JudgeConnectTipAndWing(CLDSLineAngle* pAngle,BYTE* piStart0End1,BYTE* piWingX0Y1);
};
struct TRUNK_POST_KJOINT_CONNPLATE : public MULTILEG_POST_CONNPLATE {
	bool blColinearJoint;	//�������Ĺ��ߣ���Ҫ�����ؽ�ͷ
	CLDSLineAngle *pUpperPostAngle;		//�����ӵ��ϲ�����
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
