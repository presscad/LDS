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
	WORD wFrontAngleG,wSideAngleG;	//1�������ĽǸ�������֫��λ��׼�ľࣨ��ϽǸּ��ֹ�Ϊ0��
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
	__declspec(property(get=get_fTopZ)) double fTopZ;		//�ֶζ���Z����
	__declspec(property(get=get_fSegHeight)) double fHeight;//�ֶδ�ֱ�߶�
	WORD get_wPreferAngleG();
	__declspec(property(get=get_wPreferAngleG)) WORD wPreferAngleG;
	WORD get_wActualFrontAngleG();
	__declspec(property(get=get_wActualFrontAngleG)) WORD wActualFrontAngleG;
	WORD get_wActualSideAngleG();
	__declspec(property(get=get_wActualSideAngleG)) WORD wActualSideAngleG;
	double get_fTopSizeX();
	__declspec(property(get=get_fTopSizeX))  double fTopSizeX;	//�ֶ������Ͽ���
	double get_fTopSizeY();
	__declspec(property(get=get_fTopSizeY))  double fTopSizeY;	//�ֶβ����Ͽ���
	double get_fBtmSizeX();
	__declspec(property(get=get_fBtmSizeX))  double fBtmSizeX;	//�ֶ������¿���
	double get_fBtmSizeY();
	__declspec(property(get=get_fBtmSizeY))  double fBtmSizeY;	//�ֶβ����¿���
};
typedef CModelSegmentNode* CModelSegmentNodePtr;
class CModelSubLegNode{
protected:
	UINT idLegSerial;
	CModelHeightNode* m_pBelongHeight;
public:
	void SetKey(DWORD key){idLegSerial=key;}
	UINT get_idModelLegSerial();	//������ȫ��ģ���еĽ��Ⱥ�
	__declspec(property(get = get_idModelLegSerial)) UINT idModelLegSerial;
	POSTRODFLAG syncflag;
	struct PANTSLEG {
		CLDSLineAngle* pLeftHoriAngle;	//����ýṹ���ȶ������м�ض�С���(��)
		CLDSLineAngle* pLeftUpLegAngle;	//����ýṹ����������ϲ����б�ĽǸ�(�����ӵ��������е�)
		CLDSLineAngle* pLeftDwLegAngle;	//����ýṹ����������²����б�ĽǸ�(��)
		CLDSLineAngle* pRightHoriAngle;	//����ýṹ���ȶ������м�ض�С���(��)
		CLDSLineAngle* pRightUpLegAngle;	//����ýṹ����������ϲ����б�ĽǸ�(�ң����ӵ��������е�)
		CLDSLineAngle* pRightDwLegAngle;	//����ýṹ����������²����б�ĽǸ�(��)
		CLDSNode* pVLeftTopNode;		//����ýṹ���Ȱ���б�Ķ����ڵ�
		CLDSNode* pVRightTopNode;		//����ýṹ���Ȱ���б�Ķ����ڵ�
		PANTSLEG() { 
			pLeftHoriAngle=pRightHoriAngle=NULL;
			pLeftDwLegAngle=pLeftUpLegAngle=pRightDwLegAngle=pRightUpLegAngle=NULL;
			pVLeftTopNode=pVRightTopNode=NULL; 
		}
	}pants;
	long iSeg;
	double minz,maxz;
	double fLegHeight;	//���ȸ߶�
	WORD wFrontAngleG,wSideAngleG;	//1�������ĽǸ�������֫��λ��׼�ľࣨ��ϽǸּ��ֹ�Ϊ0��
	double fHalfFrontBaseWidth,fHalfSideBaseWidth;	//�����ڵ㴦��������������
	double zFoundationTopPlane;	//��������Z����
	CLDSLinePartPtr m_arrPostRods[4];
	long m_hRelaFoundation;
	CModelSubLegNode(UINT iLegSerial=0);
	void SetBelongHeight(CModelHeightNode* _pBelongHeight){m_pBelongHeight=_pBelongHeight;}
	bool IdentifyPantsLeg(bool blClearLegacyState=true);//ʶ��ǰ�����Ƿ�Ϊ����ýṹ�Ļ�����
	bool CorrectPantsKneeJointPosition();
	bool UpdateFoundationPosition();	//���½��ȵĻ�������Ĭ��ֵ
	double get_fHalfFrontWidth();
	__declspec(property(get=get_fHalfFrontWidth)) double fHalfFrontWidth;	//�����ڵ㴦�������������
	double get_fHalfSideWidth();
	__declspec(property(get=get_fHalfSideWidth))  double fHalfSideWidth;	//�����ڵ㴦�������������
	WORD get_wRodCount();
	__declspec(property(get=get_wRodCount)) WORD wRodCount;
	UINT get_Id(){return idLegSerial;}
	__declspec(property(get=get_Id)) UINT Id;
	int AppendLegPostRod(CLDSLinePart* pRod);
	CLDSLinePart* get_pPostRod();
	__declspec(property(get=get_pPostRod)) CLDSLinePart* pPostRod;	//������ڵ����������²��������ĸ˼�
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
	__declspec(property(get=get_fTopSizeX))  double fTopSizeX;	//�ֶ������Ͽ���
	double get_fTopSizeY();
	__declspec(property(get=get_fTopSizeY))  double fTopSizeY;	//�ֶβ����Ͽ���
	double get_fBtmSizeX();
	__declspec(property(get=get_fBtmSizeX))  double fBtmSizeX;	//�����������
	double get_fBtmSizeY();
	__declspec(property(get=get_fBtmSizeY))  double fBtmSizeY;	//�����������
	double get_fBaseSizeX();
	__declspec(property(get=get_fBaseSizeX))  double fBaseSizeX;//�����������
	double get_fBaseSizeY();
	__declspec(property(get=get_fBaseSizeY))  double fBaseSizeY;//�����������
};
class CModelHeightNode{
	UINT idBody;
	CTowerModelTree* m_pTreeModel;
public:
	CXhChar16 name;
	double zExclusiveBodyTop;	//��ǰ����ר������Ķ���Z���꣬��Ҫ�������ӱ��¿�ֱ���������ĺ��� wjh-2020.1.5
	double zLongestLeg;	//���Z����
	struct PRESET_OBJS24 {static const int COUNT=  24;};
	PRESET_ARRAY<CModelSubLegNode,PRESET_OBJS24> xarrSubLegs;	//ÿ��������ദ��24������
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
	CHashSet<CModelHeightNode*>  hashRootHeights;	//ֱ�Ӵ��¶ζ�(��)���ӵ��������
	double fTopZ;	//�¶ζ���Z����
	double fFrontWidth,fSideWidth;	//�¶ζ��������濪��
	double fBtmZ;	//�¶εײ�Z����
	double fBtmFrontWidth,fBtmSideWidth;	//�¶εײ������濪��
	CModelHeightNode* pMainHeight;
	CModelSlopeNode();
	void SetTreeModel(CTowerModelTree* pModel){m_pTreeModel=pModel;}
	void SortSegments();
	double get_fSlopeRatio();	//�����¶�
	__declspec(property(get = get_fSlopeRatio)) double fSlopeRatio;
	CModelSegmentNode* AppendSegmentRod(CLDSLinePart* pRod);
};
class CTower;
class CTowerModelTree{
protected:
	CTower* m_pTower;
public:
	struct SORTSEG : public ICompareClass{
		long minz,maxz;	//�������ã���ȷ����������
		void *pRelaTreeNode;	//���������ڵ�����
		CLDSLinePart* pRelaRod;
		SORTSEG(CLDSLinePart* pRod=NULL);
		void InitByRod(CLDSLinePart* pRod);
		virtual int Compare(const ICompareClass *pCompareObj)const;
	};
	double m_fNamedHeightZeroZ;	//�������ֵ����ʼƽ���׼Z����
	CXhSimpleList<CModelSlopeNode> listSlopes;	//�ɸߵ�������
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
	long AllocBodySegmentNumber(int iInitBodySegNumber,int* xarrExcludeSegNumber=NULL,UINT uiEcludeNumberCount=0);	//�Զ������������ȵķֶκ�
	void Empty();
};
extern CTowerModelTree gxModelTree;
