#pragma once
#include "Tower.h"
#include "HashCmplexKeyTable.h"
#if defined(__RAPID_ROUGH_3DMODEL_)&&defined(__LDS_CONTEXT_)
#include "ModelTree.h"
#include ".\IntelliAgent\IntelliModel.h"
#endif

class CUnifyModelLayer
{
protected:
	BYTE ciSectPolyN;	//�����������,0:���Ͻṹ;3:������;4:�Ľ���;N:N����
public:
	bool m_bKeepMechanicalLayer;	//ͼ������Ӧ������ѧ�������ʱ�ĸ��˼��ں�����
	bool m_bCorrectNodeFatherRod;	//ͬʱ�����ڵ㸸�˼�
	struct WORKPLANE{
		char cType;	//0.��������;1.��ᵣ����ƽ��;2.������ͷ����ƽ��
		char cThirdLayer;
		UINT id;
		GEPOINT org,norm;
		WORKPLANE();
		void SetKey(DWORD key){id=(UINT)key;}
		bool IsPtOnPlane(const double* poscoords);
	};
	struct PLATE{
		char cWorkWing;		//�ְ嶨λ��׼�ǸֵĹ���֫������֫��
		short siBodyOrLegNo;//siBodyOrLegNo>0��ʾ���Ⱥ�;<0��ʾ�����
		long hAttachNode;	//�ְ��������Ľڵ�
		long hLocationAngle;//�ְ嶨λ��׼�Ǹ�
		long hBendlineAngle;//�ְ�����߻�׼�Ǹ֣�һ���������ƽ�У���0ʱ��ʾĬ��ȡֵhLocationAngle;
		GEPOINT bendPlaneNorm;	//�����淨�ߣ��ݴ�ʰȡѡ�������˼���
		UINT idBasePlane;	//�ְ����Ĺ���ƽ��Id��ʶ
		UINT idBendPlane;	//�ְ������Ĺ���ƽ��Id��ʶ��0ʱ��ʾ�����
		union{
			long hBolt;	//�ް�����ʱ���ɵ�������˨����������˨ʱ��Ϊ����֮һ��
			long hPlate;//���ɸְ����ľ��
		};
	public:
		PLATE();
		PLATE(long hNode,long hDatumAngle,char cAngleWorkWing,short siBodyOrLegNo=0);//siBodyOrLegNo>0��ʾ���Ⱥ�;<0��ʾ�����
		static CXhChar50 BuildKey(long hNode, long hDatumAngle, char cAngleWorkWing, short siBodyOrLegNo = 0);
		CXhChar50 StrKey();	//hLocationAngle-hAttachNode-cWorkWing
		__declspec(property(get = StrKey)) CXhChar50 szKey;
	};
	typedef struct SLOPE{
		TAPER_SCOPE taper;	//����̨
		WORKPLANE planes[4];
		//������ָ�ڣ����޽�����
		bool bTopOnStart,bBtmOnStart;
		CLDSLinePart* pTopRod,*pBtmRod;
		CLDSLine* pDatumLine;
		double length;
		GEPOINT top,bottom,lenStdVec;
		SLOPE();
		SLOPE(GEPOINT toppt,GEPOINT btmpt);
		void InitTaperSlopePlanes();
		char IdentifyLineTaperSlopeLayer(CLDSLinePart* pRod,double tolerance=EPS);
		char IdentifyPointTaperSlopeLayer(const GEPOINT& point,double tolerance=EPS);
		bool TestInSlopeLine(const double* point,double* dist2top=NULL,double tolerance=EPS);
		bool MergeWithUpperSlope(SLOPE* pUpperSlope);
	}*SLOPEPTR;
	COLINEAR_TOLERANCE xTolerance;
	ATOM_LIST<SLOPE> listSlopes;
	CHashListEx<WORKPLANE> hashPlanes;
	struct REVISION{
		static const DWORD LAYER		=0x00000001;	//�޶����ݶ���ͼ����
		static const DWORD POS_START	=0x00000002;	//�޶��˼�ʼ��λ��
		static const DWORD POS_END		=0x00000004;	//�޶��˼��ն�λ��
		static const DWORD LAYOUT		=0x00000008;	//�Ǹְڷŷ�ʽ
		static const DWORD PARENT_ROD	=0x00000002;	//�ڵ㸸�˼�
	};
	CHashList<BITFLAG>hashObjRevision;	//�˼���ڵ�
	struct JOINTINFO {
		long hMainAngle, hCoupleAngle;
		void SetKey(XHUINT64 key) {
			DUALKEY dualkey(key);
			hMainAngle = dualkey.lowId;
			hCoupleAngle = dualkey.highId;
		}
		JOINTINFO(long hAngle1=0, long hAngle2 = 0) { hMainAngle = hAngle1, hCoupleAngle = hAngle2; }
	};
	CHashStrListEx<PLATE> hashPlates;
	CHash64ListEx<JOINTINFO> hashJoints;
	void RetrieveModelLegacyConnections(IModel* pModel);
protected:
	int CheckColinearJointAngles(CLDSPlate* pPlate, CUnifyModelLayer::JOINTINFO* xarrJoints, int nMaxCount);
	virtual bool IsDesigner(){return false;}
	bool UnifySlopeObjLayer(CLDSNode* pSlopeBtmQuadNode,CLDSLinePart* pSlopeBtmQuadRod,double* pBodyTopZ,bool bCorrectObjLayer=true);
	void UnifyTrunkObjPosLayer();
	bool CorrectHeadArmObjPosLayerByWorkPlane();
	void UnifyHeadArmObjPosLayer();
	WORKPLANE* AppendUniqueWorkPlane(WORKPLANE plane,WORKPLANE** ppMatchPlane=NULL);
public:
	bool SetRodRevision(long hRod,DWORD dwFlagType);
	bool IsRodRevision(long hRod,DWORD dwFlagType);
	bool SetNodeRevision(long hNode,DWORD dwFlagType);
	bool IsNodeRevision(long hNode,DWORD dwFlagType);
public:
	CUnifyModelLayer(void);
	virtual ~CUnifyModelLayer(void);
	PLATE* RegisterPlate(long hBaseNode, long hBasePart, char cWorkWing, short siBodyOrLegNo=0);
	static double GetRodGradient2(CLDSLinePart *pLinePart);
	static CLDSLinePart *FindSlopeRodByBaseNode(CLDSNode *pBaseNode);
	static bool FindQuad1LowestBaseNode(CLDSNode** ppLowestBaseNode, CLDSLinePart** ppLowestLegRod);
	static void GetMirRotateHashTable(CHashSet<CLDSLinePart*>& hashRods);
	static void GetMirHashTable(CHashSet<CLDSLinePart*>& hashRods);
	static void GetTopNodeByDirection(CHashSet<CLDSLinePart*>&hashSlopePart,f3dPoint stdXieLenVec,CLDSNode* pSlopeBtmQuadNode,CLDSNode**ppTopNode,CLDSLinePart** ppTopRod=NULL,COLINEAR_TOLERANCE* pTolerance=NULL);
	static CLDSNode* FindLegTopNodeByLegTopZ(double fLegTopZ,CHashSet<CLDSLinePart*>&hashLegPostRods,CLDSNode* pSlopeBtmQuadNode,CLDSLinePart* pSlopeBtmQuadRod,CLDSLinePart** ppTopRod=NULL);
	static double FindLegTopZ(CHashSet<CLDSLinePart*>&hashLegXiePart,CLDSNode* pSlopeBtmQuadNode,CLDSLinePart* pSlopeBtmQuadRod);
	static void UpdateSlopeRodMirLayer(CLDSLinePart* pSlopeRod);
	static void GetSlopeTopNode(CHashSet<CLDSLinePart*>&hashSlopePart,CLDSNode *pSlopeBtmQuadNode,CLDSLinePart* pSlopeBtmQuadRod,CLDSNode **ppSlopeTopQuadNode,CLDSLinePart** ppTopRod=NULL,COLINEAR_TOLERANCE* pTolerance=NULL);
	static void InitHashSlopeNode(CHashSet<CLDSNode*>&hashSlopeNode,CLDSNode * pTopNode,CLDSNode *pDownNode);
	static void InitTaperSlopePlanes(const TAPER_SCOPE& taper,WORKPLANE planes[4]);
public:
	int CorrectNodeFatherRodInfo(NODESET* pNodeSet=NULL,COLINEAR_TOLERANCE* pTolerance=NULL,bool bCorrectColinear=true);
	void CorrectColinearJoints(COLINEAR_TOLERANCE* pTolerance=NULL,int liInitProcessPercent=0);
	void CorrectPostLineNodes(COLINEAR_TOLERANCE* pTolerance=NULL);	//����λ����������ϵ����κ������ڵ�
	void CorrectTrunkBodyPosition();
	void CorrectTGroupAnglePosition();
	UINT MergeModelOverlapNodes();
	UINT CorrectDependRodsOfIntersNode();
	bool UnifyObjsLayer(COLINEAR_TOLERANCE tolerance=COLINEAR_TOLERANCE(1.0));
	bool RecognizeSlopes(COLINEAR_TOLERANCE* pTolerance);
	bool RecognizeSlopes(double distTolerance = 5.0, double tangentTolerance = 0.001);
	bool TestRodPosition(CLDSLinePart* pRod,int* piPosTypeToTrunk);
#if defined(__RAPID_ROUGH_3DMODEL_)&&defined(__LDS_CONTEXT_)
	void RetrievedTowerModeTree(CTowerModelTree &modeltree);
#endif
};
#ifdef __ANGLE_PART_INC_
class CIntelliAgent;
class CIntelliModelDesigner : public CUnifyModelLayer
{
	CIntelliAgent *m_pIntelliAgent;
	virtual bool IsDesigner(){return true;}
	long DesignConnectionPoint(CConnectPointInfo* pConn);
public:
	bool m_bEnablePromptUI;	//������ƹ�������������ʾ������
	CIntelliModelDesigner(CIntelliAgent *pIntelliAgent=NULL);
	BOOL DesignPlate(PLATE& plate);
	static long DesignConnectPointStructure(CConnectPointInfo *pConnectPoint,CIntelliModelDesigner* pIntelliDesigner=NULL);
#if defined(__RAPID_ROUGH_3DMODEL_)&&defined(__LDS_CONTEXT_)
private:
	CXhSimpleList<STDTEMPL> listArms;
	//�ڲ����ú���
	CLDSPlate* InitWirePlateBaseInfo(CLDSNode* pBaseNode,CLDSLineAngle* pBaseJg,BYTE ciWorkWing,LINEPARTSET& rayJgSet);
	void CreateWirePlate(CLDSPlate* pWirePlate,STDTEMPL* pLiveArm,STDTEMPL::PLATE* pWireHolePara);
	void CorrectGeneralArmRodPosition();	//һ���Դ���ᵣ�����Ҹ����������ļ������λ��
public:
	void DesignAllNodeConnections(bool bRetrieveConnPoints=true);
	void SetIntelliExperts(CIntelliAgent *pIntelliAgent){m_pIntelliAgent=pIntelliAgent;}
	void CorrectHeadArmRodPosition();
	bool DesignWirePlatePoint(STDTEMPL::PLATE* pArmPlate,STDTEMPL* pLiveArm);
	void ResetSpecialAngleG(bool displayUI=false);
#endif
};
#endif
