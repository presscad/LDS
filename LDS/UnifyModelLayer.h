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
	BYTE ciSectPolyN;	//塔身横截面边数,0:复合结构;3:三角塔;4:四角塔;N:N角塔
public:
	bool m_bKeepMechanicalLayer;	//图层修正应保持力学计算设计时的各杆件内含不变
	bool m_bCorrectNodeFatherRod;	//同时修正节点父杆件
	struct WORKPLANE{
		char cType;	//0.塔身坡面;1.侧横担工作平面;2.顶部塔头工作平面
		char cThirdLayer;
		UINT id;
		GEPOINT org,norm;
		WORKPLANE();
		void SetKey(DWORD key){id=(UINT)key;}
		bool IsPtOnPlane(const double* poscoords);
	};
	struct PLATE{
		char cWorkWing;		//钢板定位基准角钢的工作肢（连接肢）
		short siBodyOrLegNo;//siBodyOrLegNo>0表示接腿号;<0表示本体号
		long hAttachNode;	//钢板所附属的节点
		long hLocationAngle;//钢板定位基准角钢
		long hBendlineAngle;//钢板火曲线基准角钢（一般与火曲线平行），0时表示默认取值hLocationAngle;
		GEPOINT bendPlaneNorm;	//火曲面法线（据此拾取选择火曲面杆件）
		UINT idBasePlane;	//钢板基面的工作平面Id标识
		UINT idBendPlane;	//钢板火曲面的工作平面Id标识，0时表示单面板
		union{
			long hBolt;	//无板连接时生成的连接螺栓句柄（多颗螺栓时，为其中之一）
			long hPlate;//生成钢板对象的句柄
		};
	public:
		PLATE();
		PLATE(long hNode,long hDatumAngle,char cAngleWorkWing,short siBodyOrLegNo=0);//siBodyOrLegNo>0表示接腿号;<0表示本体号
		static CXhChar50 BuildKey(long hNode, long hDatumAngle, char cAngleWorkWing, short siBodyOrLegNo = 0);
		CXhChar50 StrKey();	//hLocationAngle-hAttachNode-cWorkWing
		__declspec(property(get = StrKey)) CXhChar50 szKey;
	};
	typedef struct SLOPE{
		TAPER_SCOPE taper;	//梯形台
		WORKPLANE planes[4];
		//以下特指第１象限角棱线
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
		static const DWORD LAYER		=0x00000001;	//修订数据对象图层名
		static const DWORD POS_START	=0x00000002;	//修订杆件始端位置
		static const DWORD POS_END		=0x00000004;	//修订杆件终端位置
		static const DWORD LAYOUT		=0x00000008;	//角钢摆放方式
		static const DWORD PARENT_ROD	=0x00000002;	//节点父杆件
	};
	CHashList<BITFLAG>hashObjRevision;	//杆件或节点
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
	void CorrectPostLineNodes(COLINEAR_TOLERANCE* pTolerance=NULL);	//修正位于塔身角线上的无任何依赖节点
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
	bool m_bEnablePromptUI;	//智能设计过程中允许弹出提示交互框
	CIntelliModelDesigner(CIntelliAgent *pIntelliAgent=NULL);
	BOOL DesignPlate(PLATE& plate);
	static long DesignConnectPointStructure(CConnectPointInfo *pConnectPoint,CIntelliModelDesigner* pIntelliDesigner=NULL);
#if defined(__RAPID_ROUGH_3DMODEL_)&&defined(__LDS_CONTEXT_)
private:
	CXhSimpleList<STDTEMPL> listArms;
	//内部调用函数
	CLDSPlate* InitWirePlateBaseInfo(CLDSNode* pBaseNode,CLDSLineAngle* pBaseJg,BYTE ciWorkWing,LINEPARTSET& rayJgSet);
	void CreateWirePlate(CLDSPlate* pWirePlate,STDTEMPL* pLiveArm,STDTEMPL::PLATE* pWireHolePara);
	void CorrectGeneralArmRodPosition();	//一般性处理横担上下弦杆与塔身主材间的连接位置
public:
	void DesignAllNodeConnections(bool bRetrieveConnPoints=true);
	void SetIntelliExperts(CIntelliAgent *pIntelliAgent){m_pIntelliAgent=pIntelliAgent;}
	void CorrectHeadArmRodPosition();
	bool DesignWirePlatePoint(STDTEMPL::PLATE* pArmPlate,STDTEMPL* pLiveArm);
	void ResetSpecialAngleG(bool displayUI=false);
#endif
};
#endif
