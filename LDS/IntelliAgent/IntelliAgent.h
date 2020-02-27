#pragma once
#include "Tower.h"
#include "IntelliModel.h"
#include "SortFunc.h"

#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
struct MAIN_ANGLE : public ICompareClass{
	CLDSLineAngle *pAngle;	//主材角钢
	GEPOINT vertex;	//角钢与截面交点
	double fAngle;		//vertex与X轴夹角
	bool disabled;	//true:表示仅参照不修订
	f3dPoint Start(){return (pAngle!=NULL)?pAngle->pStart->xActivePos:f3dPoint();}
	f3dPoint End(){return (pAngle!=NULL)?pAngle->pEnd->xActivePos:f3dPoint();}
	MAIN_ANGLE(){pAngle=NULL;fAngle=0;disabled=false;}
	virtual int Compare(const ICompareClass *pCompareObj)const{
		MAIN_ANGLE* pMainAngle2=(MAIN_ANGLE*)pCompareObj;
		if(fAngle>pMainAngle2->fAngle)
			return 1;
		else if(fAngle<pMainAngle2->fAngle)
			return -1;
		else
			return 0;
	}
};
#endif

#ifdef __RAPID_ROUGH_3DMODEL_
class CIntelliAgent : public ITowerModel
{
	IModel* m_pModel;
public:
	struct CModelNode: public IModelNode{
	protected:
		bool _bLayerModified;
	public:
		CLDSNode* pMappingNode;	//对就原原始节点对象指针CLDSNode*
		void SetKey(DWORD key){handle=key;}
		CModelNode(CLDSNode* pNode=NULL);
		virtual void* MappingNode(){return pMappingNode;}
		virtual bool SetLayerRevState(bool modified=true);
		bool get_bLayerModified() { return _bLayerModified; }
		__declspec(property(put=SetLayerRevState,get = get_bLayerModified)) bool m_bLayerModified;
	};
	struct CModelRod: public IModelRod{
		bool m_bLayerModified;
		CLDSLinePart* pMappingRod;
		void SetKey(DWORD key){handle=key;}
		CModelRod(CLDSLinePart* pRod=NULL);
		virtual bool IsAngle(){return pMappingRod->IsAngle();}
		virtual int GetClassTypeId(){return pMappingRod->GetClassTypeId();}
		virtual void* MappingRod(){return pMappingRod;}
		virtual bool SetLayerRevState(bool modified = true);
	};
protected:
	CHashListEx<CModelNode> hashModelNodes;
	CHashListEx<CModelRod> hashModelRods;
	UINT m_uiMinKeyLength;
public:
	CIntelliAgent(IModel* pModel=NULL,UINT uiMinKeyLength=300);
	virtual ~CIntelliAgent(void);
	static bool ApplyLiveArmTempl(STDTEMPL* pLiveArm,long* arrayPlanes=NULL,int nPlaneCount=0,CHashList<BITFLAG> *pHashObjRevision=NULL);
	void ClearObjects();
	virtual void SyncModel(IModel* pModel);
	virtual IModelNode* AppendNode(CLDSNode* pNode);
	virtual IModelRod* AppendRod(CLDSLinePart* pRod);
	virtual IModelNode* FromNodeHandle(long hNode);
	virtual IModelRod* FromRodHandle(long hRod);
	virtual IModelNode* EnumNodeFirst();
	virtual IModelNode* EnumNodeNext();
	virtual IModelRod* EnumRodFirst();
	virtual IModelRod* EnumRodNext();
	virtual UINT GetNodeCount(){return hashModelNodes.GetNodeNum();}
	virtual UINT GetRodCount(){return hashModelRods.GetNodeNum();}
	int RecognizeAllStdTempl(const double* vxSlopeTop3d,const double* vxSlopeBtm3d,IXhList<STDTEMPL>* pListLiveArms,COLINEAR_TOLERANCE* pxTolerance=NULL);
	bool RecognizeConnPointSchema(CLDSNode* pBaseNode,STDTEMPL::CONNPOINT* pConnPoint,GECS* pDCS=NULL);
};
#endif
