#pragma once
#include "Tower.h"
#include "IStdComTempl.h"
#include "SortFunc.h"
#include "Deg.h"
class CLDSBolt;
struct MATCHCONNPOINT_INFO;
class CStdComModel
{
protected:
	IModel* m_pModel;
	CStdComTempl* m_pStdTempl;
	struct BOLTREF{
		CLDSBolt* pBolt;
		GEPOINT location;
		BOLTREF(CLDSBolt* _pBolt=NULL,const double* location_coords=NULL);
	};
	f3dPoint m_xNearAxisZ;
	CStdComTemplExpression m_xExpression;
	virtual bool BuildAcsOfAssmComponent();
public:
	GECS acs;	//标准组件的装配坐标系
	PARTSET m_xMapPartSet;
	NODESET m_xMapNodeSet;
	struct SHAREANGLE {
		long hAngle;			//连接角钢句柄
		CLDSLineAngle* pAngle;	//连接角钢
		SIMPLE_ARRAY<CLDSBolt*,PRESET_OBJS64> xarrConnBolts;
	public:
		void SetKey(DWORD key) { hAngle=key; }
	};
	static CHashListEx<SHAREANGLE> hashShareAngles;
	static bool BuildAcsOfAssmComponent(CStdComTempl* pStdTempl,GECS* pAcs);
	bool VerifyMatchRods(CStdComTempl* pStdTempl,UINT* parrRodIds=NULL,UINT count=1);
	static bool MatchWithStdConnPoint(CConnectPointInfo *pConnectPoint,
		CStdComTemplConnPoint* pStdConnPoint, MATCHCONNPOINT_INFO* pMatchInfo);
	static int DesignStdConnPoint(CStdComTemplConnPoint* pStdConnPoint, CConnectPointInfo *pConnectPoint, MATCHCONNPOINT_INFO* pMatchInfo,
		UINT* pidSpecNodePlateTmpl = NULL, bool bDisplayMultiTmplUI = false,
		BYTE cbMirCreateFlag = 0, bool blNeglectNodePlateTmplCheckState = true);
protected:
	double GetNormOffset(CStdTemplParaRod* pCurrParaRod,char cAngleTowardIn1Out2,const char* szNormalOffset,BYTE* pStyleG,CParaThick* pParaThick=NULL);
	CLDSNode* AddOrUpdateParaModelNode(CStdTemplParaNode* pParaNode);
	CLDSLinePart* AddOrUpdateParaModelRod(CStdTemplParaRod* pParaRod);
	CLDSGeneralPlate* AppendParaModelPlate(CStdTemplParaPlate* pParaPlate,GECS* pAcs=NULL,BYTE ciMirMode=0);
	CLDSBolt* AppendParaModelBolt(CStdTemplParaBolt* pParaBolt);
	bool UpdateAngleWingDirection(CStdTemplParaAngle* pParaAngle);
	void UpdateTemlRodReverseInfo(CStdTemplParaRod* pParaRod, BYTE* pInsideWingX = 0, BYTE* pInsideWingY = 0);
	void LayoutConnRodBolts(CStdTemplParaPlate* pParaPlate, GEPOINT* pWorkPlaneNormal);
	UINT LayoutAngleBolts(CONNECT_ROD* pConnRod, BOLT_LAYOUT& layout, CXhSimpleList<BOLT_LOCATION>* pListLocations);
	virtual bool MirrorNewModelParts(CStdComTempl* pStdTempl);
public:
	CStdComModel(IModel* pModel=NULL);
	virtual ~CStdComModel(void);
	//
	virtual bool RetrieveConnPoint(CConnectPointInfo* pConnPoint,CLDSNode* pNode,CLDSLinePart* pBaseRod,const double* vzStdWorkPlaneNormal,bool bIncBendConnect=true);
	virtual bool BuildModel(CStdComTempl* pStdTempl,BYTE ciMirMode=0);
	virtual CLDSPart* GetLdsPartFromId(long idPart,BYTE ciPartType=0);
	virtual CLDSLinePart* GetLdsRodFromId(long idRod);
	virtual CLDSNode* GetLdsNodeFromId(TMPLOBJID idNode);
	virtual CLDSGeneralPlate* GetLdsPlateFromId(long idPlate);
public:
	static bool m_bEnableMirCreate;	//智能设计过程中允许对称生成构件
	static bool m_bEnablePromptUI;	//智能设计过程中允许弹出提示交互框
	//-1表示用户中断智能生成连接板;0.软件未在标准节点库中找到合适的标准模板;1.软件已根据系统中最佳匹配标准模板进行了节点连接设计 wjh-2019.5.26
	static int IntelliDesignStdComNodePlate(CConnectPointInfo *pConnectPoint,UINT* parrSpecifyStdTmpl=NULL,UINT count=1,BYTE cbMirCreateFlag=0,bool blNeglectNodePlateTmplCheckState=true);
};

struct MATCHROD_INFO {
	bool matched;
	char ciConnTipS0E1Els2;
	char ciPlaneNo;
public:	//属性
	short siRayDegAngle;
	DEG get_xRayDegAngle() { return DEG(siRayDegAngle); }
	__declspec(property(get = get_xRayDegAngle)) DEG xRayDegAngle;
	char get_ciRodType()		//'A':Across，杆件跨越基准节点;'R':Radiation杆件从基准节点射线发出
	{
		if (ciConnTipS0E1Els2 == 0 || ciConnTipS0E1Els2 == 1)
			return 'R';
		else
			return 'A';
	}
	__declspec(property(get = get_ciRodType)) char ciRodType;
public:
	CLDSLinePart* pRod;
	MATCHROD_INFO(CLDSLinePart* _pRod = NULL) {
		matched = false;
		pRod = _pRod;
		ciPlaneNo = 0;
		ciConnTipS0E1Els2 = 0;
	}
};
struct MATCHCONNPOINT_INFO {
public:	//连接点基本识别信息
	//增加'StoE'主要是为了严格澄清变量含义与赋值规范，因为后续要用到必须严格始->终 wjh-2019.9.16
	GEPOINT lenBaseRodStdVecStoE;
	PRESET_ARRAY8<MATCHROD_INFO> connrods;
public:	//连接点匹配信息
	long hBaseNode;
	CLDSLinePart* pBaseRod;
	char ciMirrorMatchMode;	//是否为镜像对称
	GECS lcs;
	PRESET_ARRAY8<long> xarrMatchRodHid;	//预留8根杆件，遇到第一根widRod=0杆件结束 
	bool m_bConnPointMatched;	//true表明已经在DesignStdConnPoint调用前进行了匹配;false需要在DesignStdConnPoint内匹配 wjh-2019.12.17
public:	//属性
	long set_hBaseRod(long hRod) {
		xarrMatchRodHid.Set(0, hRod);
		return hRod;
	}
	long get_hBaseRod() { return xarrMatchRodHid[0]; }
	__declspec(property(put = set_hBaseRod, get = get_hBaseRod)) long hBaseRod;
	long set_hRayRodOnAxisY(long hRod) {
		xarrMatchRodHid.Set(1, hRod);
		return hRod;
	}
	long get_hRayRodOnAxisY() { return xarrMatchRodHid[1]; }
	__declspec(property(put = set_hRayRodOnAxisY, get = get_hRayRodOnAxisY)) long hRayRodOnAxisY;
	MATCHCONNPOINT_INFO() {
		m_bConnPointMatched=false;
		ciMirrorMatchMode = 0;
		hBaseNode = 0;
		pBaseRod = NULL;
		xarrMatchRodHid.ZeroPresetMemory();
	}
};
bool MapInfoToStdConnPoint(MATCHCONNPOINT_INFO* pMatchInfo, CStdComTemplConnPoint* pStdConnPoint);
bool RetrieveBasicMatchInfo(CConnectPointInfo *pConnectPoint, MATCHCONNPOINT_INFO* pMatchInfo);
