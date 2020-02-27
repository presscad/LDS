// Bolt.cpp: implementation of the C3dLs class.
//wbt translation
//////////////////////////////////////////////////////////////////////
#pragma once

#include "SortFunc.h"
#include "lds_part.h"
#include "FoundationParam.h"

class CModelHeightNode;
class CModelSlopeNode;
class CTowerModelTree;
class CModelSegmentNode{
protected:
	UINT id;
	CTowerModelTree* m_pTreeModel;
	CModelHeightNode* m_pBelongHeight;
public:
	long iSeg;
	POSTRODFLAG syncflag;
	WORD wFrontAngleG,wSideAngleG;	//1象限主材角钢正侧面肢定位基准心距（组合角钢及钢管为0）
	CLDSLinePart* pPostRod;
	CHashSet<CModelSegmentNode*> hashMutexSubSegments;
	CHashSet<CModelHeightNode*> hashHeightSet;
public:
	void SetKey(DWORD key){id=key;}
	void SetTreeModel(CTowerModelTree* pModel){m_pTreeModel=pModel;}
	void SetBelongHeight(CModelHeightNode* pHeight){m_pBelongHeight=pHeight;}
	CModelSegmentNode();
	bool AppendSubTreeNode(LINEPARTSET& rodset,UINT idSiblingBodyNo);
	UINT get_Id(){return id;}
	__declspec(property(get=get_Id)) UINT Id;
	CModelHeightNode* get_pBelongHeight(){return m_pBelongHeight;}
	__declspec(property(get=get_pBelongHeight)) CModelHeightNode* pBelongHeight;
	CTowerModelTree* get_pTreeModel(){return m_pTreeModel;}
	__declspec(property(get=get_pTreeModel)) CTowerModelTree* pTreeModel;
	double get_fTopZ();
	double get_fSegHeight();
	__declspec(property(get=get_fTopZ)) double fTopZ;		//分段顶部Z坐标
	__declspec(property(get=get_fSegHeight)) double fHeight;//分段垂直高度
	WORD get_wPreferAngleG();
	__declspec(property(get=get_wPreferAngleG)) WORD wPreferAngleG;
	WORD get_wActualFrontAngleG();
	__declspec(property(get=get_wActualFrontAngleG)) WORD wActualFrontAngleG;
	WORD get_wActualSideAngleG();
	__declspec(property(get=get_wActualSideAngleG)) WORD wActualSideAngleG;
	double get_fTopSizeX();
	__declspec(property(get=get_fTopSizeX))  double fTopSizeX;	//分段正面上开口
	double get_fTopSizeY();
	__declspec(property(get=get_fTopSizeY))  double fTopSizeY;	//分段侧面上开口
	double get_fBtmSizeX();
	__declspec(property(get=get_fBtmSizeX))  double fBtmSizeX;	//分段正面下开口
	double get_fBtmSizeY();
	__declspec(property(get=get_fBtmSizeY))  double fBtmSizeY;	//分段侧面下开口
};
typedef CModelSegmentNode* CModelSegmentNodePtr;
class CModelSubLegNode{
protected:
	UINT idLegSerial;
	CModelHeightNode* m_pBelongHeight;
public:
	void SetKey(DWORD key){idLegSerial=key;}
	UINT get_idModelLegSerial();	//接腿在全局模型中的接腿号
	__declspec(property(get = get_idModelLegSerial)) UINT idModelLegSerial;
	POSTRODFLAG syncflag;
	struct PANTSLEG {
		CLDSLineAngle* pLeftHoriAngle;	//大裤衩结构接腿顶部的中间截断小横材(左)
		CLDSLineAngle* pLeftUpLegAngle;	//大裤衩结构腿身过渡面上侧接腿斜材角钢(左，连接到隔面横材中点)
		CLDSLineAngle* pLeftDwLegAngle;	//大裤衩结构腿身过渡面下侧接腿斜材角钢(左)
		CLDSLineAngle* pRightHoriAngle;	//大裤衩结构接腿顶部的中间截断小横材(右)
		CLDSLineAngle* pRightUpLegAngle;	//大裤衩结构腿身过渡面上侧接腿斜材角钢(右，连接到隔面横材中点)
		CLDSLineAngle* pRightDwLegAngle;	//大裤衩结构腿身过渡面下侧接腿斜材角钢(右)
		CLDSNode* pVLeftTopNode;		//大裤衩结构接腿八字斜材顶部节点
		CLDSNode* pVRightTopNode;		//大裤衩结构接腿八字斜材顶部节点
		PANTSLEG() { 
			pLeftHoriAngle=pRightHoriAngle=NULL;
			pLeftDwLegAngle=pLeftUpLegAngle=pRightDwLegAngle=pRightUpLegAngle=NULL;
			pVLeftTopNode=pVRightTopNode=NULL; 
		}
	}pants;
	long iSeg;
	double minz,maxz;
	double fLegHeight;	//接腿高度
	WORD wFrontAngleG,wSideAngleG;	//1象限主材角钢正侧面肢定位基准心距（组合角钢及钢管为0）
	double fHalfFrontBaseWidth,fHalfSideBaseWidth;	//基础节点处正侧面基础半根开
	double zFoundationTopPlane;	//基础顶面Z坐标
	CLDSLinePartPtr m_arrPostRods[4];
	long m_hRelaFoundation;
	CModelSubLegNode(UINT iLegSerial=0);
	void SetBelongHeight(CModelHeightNode* _pBelongHeight){m_pBelongHeight=_pBelongHeight;}
	bool IdentifyPantsLeg(bool blClearLegacyState=true);//识别当前接腿是否为大裤衩结构的换接腿
	bool CorrectPantsKneeJointPosition();
	bool UpdateFoundationPosition();	//更新接腿的基础根开默认值
	double get_fHalfFrontWidth();
	__declspec(property(get=get_fHalfFrontWidth)) double fHalfFrontWidth;	//基础节点处正面铁塔半根开
	double get_fHalfSideWidth();
	__declspec(property(get=get_fHalfSideWidth))  double fHalfSideWidth;	//基础节点处侧面铁塔半根开
	WORD get_wRodCount();
	__declspec(property(get=get_wRodCount)) WORD wRodCount;
	UINT get_Id(){return idLegSerial;}
	__declspec(property(get=get_Id)) UINT Id;
	int AppendLegPostRod(CLDSLinePart* pRod);
	CLDSLinePart* get_pPostRod();
	__declspec(property(get=get_pPostRod)) CLDSLinePart* pPostRod;	//与基础节点相连的最下侧塔腿主材杆件
	CModelHeightNode* get_pBelongHeight(){return m_pBelongHeight;}
	__declspec(property(get=get_pBelongHeight)) CModelHeightNode* pBelongHeight;
	CTowerModelTree* get_pTreeModel();
	__declspec(property(get=get_pTreeModel)) CTowerModelTree* pTreeModel;
	WORD get_wPreferAngleG();
	__declspec(property(get=get_wPreferAngleG)) WORD wPreferAngleG;
	WORD get_wActualFrontAngleG();
	__declspec(property(get=get_wActualFrontAngleG)) WORD wActualFrontAngleG;
	WORD get_wActualSideAngleG();
	__declspec(property(get=get_wActualSideAngleG)) WORD wActualSideAngleG;
	double get_fTopSizeX();
	__declspec(property(get=get_fTopSizeX))  double fTopSizeX;	//分段正面上开口
	double get_fTopSizeY();
	__declspec(property(get=get_fTopSizeY))  double fTopSizeY;	//分段侧面上开口
	double get_fBtmSizeX();
	__declspec(property(get=get_fBtmSizeX))  double fBtmSizeX;	//铁塔正面根开
	double get_fBtmSizeY();
	__declspec(property(get=get_fBtmSizeY))  double fBtmSizeY;	//铁塔侧面根开
	double get_fBaseSizeX();
	__declspec(property(get=get_fBaseSizeX))  double fBaseSizeX;//基础正面根开
	double get_fBaseSizeY();
	__declspec(property(get=get_fBaseSizeY))  double fBaseSizeY;//基础侧面根开
};
class CModelHeightNode{
	UINT idBody;
	CTowerModelTree* m_pTreeModel;
public:
	CXhChar16 name;
	double zExclusiveBodyTop;	//当前呼高专属塔身的顶部Z坐标，主要用于甄别从变坡口直接延下来的呼高 wjh-2020.1.5
	double zLongestLeg;	//最长腿Z坐标
	struct PRESET_OBJS24 {static const int COUNT=  24;};
	PRESET_ARRAY<CModelSubLegNode,PRESET_OBJS24> xarrSubLegs;	//每个呼高最多处理24个减腿
	CHashSet<CModelSegmentNode*> hashSegments;
	CModelHeightNode();
	void SetTreeModel(CTowerModelTree* pModel){m_pTreeModel=pModel;}
	void SetKey(DWORD key){idBody=key;}
	UINT get_Id(){return idBody;}
	__declspec(property(get=get_Id)) UINT Id;
	WORD get_wLegCount();
	__declspec(property(get=get_wLegCount)) WORD wLegCount;
	double get_zTransitHoriPlane();
	__declspec(property(get=get_zTransitHoriPlane)) double zTransitHoriPlane;
	double get_fNamedHeight();
	__declspec(property(get = get_fNamedHeight)) double fNamedHeight;
	double get_fLinkBodyHeight();
	__declspec(property(get = get_fLinkBodyHeight)) double fLinkBodyHeight;
	CTowerModelTree* get_pTreeModel(){return m_pTreeModel;}
	__declspec(property(get=get_pTreeModel)) CTowerModelTree* pTreeModel;
	CModelSubLegNode* AppendSubLeg(CModelSubLegNode &subleg);
	CModelSegmentNode* AppendSegmentRod(CLDSLinePart* pRod);
};
class CModelSlopeNode{
protected:
	CTowerModelTree* m_pTreeModel;
public:
	CHashSet<CModelSegmentNode*> hashSegments;
	CHashSet<CModelHeightNode*>  hashRootHeights;	//直接从坡段顶(根)部接的塔身呼高
	double fTopZ;	//坡段顶部Z坐标
	double fFrontWidth,fSideWidth;	//坡段顶部正侧面开口
	double fBtmZ;	//坡段底部Z坐标
	double fBtmFrontWidth,fBtmSideWidth;	//坡段底部正侧面开口
	CModelHeightNode* pMainHeight;
	CModelSlopeNode();
	void SetTreeModel(CTowerModelTree* pModel){m_pTreeModel=pModel;}
	void SortSegments();
	double get_fSlopeRatio();	//计算坡度
	__declspec(property(get = get_fSlopeRatio)) double fSlopeRatio;
	CModelSegmentNode* AppendSegmentRod(CLDSLinePart* pRod);
};
class CTower;
class CTowerModelTree{
protected:
	CTower* m_pTower;
public:
	struct SORTSEG : public ICompareClass{
		long minz,maxz;	//仅排序用，精确到整数即可
		void *pRelaTreeNode;	//关联的树节点类型
		CLDSLinePart* pRelaRod;
		SORTSEG(CLDSLinePart* pRod=NULL);
		void InitByRod(CLDSLinePart* pRod);
		virtual int Compare(const ICompareClass *pCompareObj)const;
	};
	double m_fNamedHeightZeroZ;	//计算呼高值的起始平面基准Z坐标
	CXhSimpleList<CModelSlopeNode> listSlopes;	//由高到低排序
	CHashListEx<CModelSegmentNode> hashSegments;
	CHashListEx<CModelHeightNode>  hashHeights;
public:
	CTowerModelTree(CTower* pTower=NULL);
	void SetBelongTower(CTower* pTower){m_pTower=pTower;}
	CTower* get_pTower(){return m_pTower;}
	__declspec(property(get=get_pTower)) CTower* pTower;
	bool AddOrUpdateFoundations();
	GEPOINT GetFoundationPosition(CModelSubLegNode* pSubLeg,CModelHeightNode* pBlongHeight=NULL);
	CModelSlopeNode* AppendSlopeNode();
	CModelSegmentNode* AppendSegmentRod(CLDSLinePart* pRod);
	CModelHeightNode* AppendHeightNode(UINT idHeightNo);
	long AllocBodySegmentNumber(int iInitBodySegNumber,int* xarrExcludeSegNumber=NULL,UINT uiEcludeNumberCount=0);	//自动分配塔身及接腿的分段号
	void Empty();
};
extern CTowerModelTree gxModelTree;
