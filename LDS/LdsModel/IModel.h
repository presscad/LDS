#pragma once
class CParaCS;
class CLDSObject;
class CLDSPoint;
class CLDSLine;
class CLDSPlane;
class CLDSNode;
class CLDSPart;
class CLDSBolt;
class CLDSLinePart;
class CDisplayView;
class CLDSArcLift;
class CLDSPartGroup;
struct CONSTRAINT;
#include "ArrayList.h"
#include "ldsptr_list.h"
/*#ifndef __PARTSET_DEF_
#define __PARTSET_DEF_
typedef CTmaPtrList<CLDSPart,CLDSPart*> PARTSET;
#endif*/
//#include "lds_list.h" 
/*template <class TYPE> class LIST_ENUM
{
public:
	virtual int push_stack()=0;
	virtual bool pop_stack(int pos=-1)=0;
	virtual long GetNodeNum()const=0;
	virtual TYPE* GetCursor()=0;
	virtual TYPE* GetFirst(BOOL bIterDelete=FALSE)=0;
	virtual TYPE* GetNext(BOOL bIterDelete=FALSE)=0;
	virtual TYPE* GetPrev(BOOL bIterDelete=FALSE)=0;
    virtual TYPE* GetTail(BOOL bIterDelete=FALSE)=0;
};
template<class TYPE> class LIST_MGR
{
public:
	LIST_ENUM<TYPE>* Enum(){return (LIST_ENUM<TYPE>*)this;}
	virtual bool IsPartList(){return false;}
	virtual TYPE* FromHandle(long h)=0;
	virtual TYPE* append(BOOL bGetNewHandle=TRUE)=0;
	virtual BOOL  DeleteNode(long h,BOOL bIncPerm=TRUE)=0;
};
class LIST_PART : public LIST_ENUM<CLDSPart>,public LIST_MGR<CLDSPart>
{
public:
	virtual bool IsPartList(){return true;}
	virtual CLDSLinePart* EnumRodFirst()=0;
	virtual CLDSLinePart* EnumRodNext()=0;
	virtual CLDSPart* FromHandle(long h){return FromHandle(h,0);}
	virtual CLDSPart* FromHandle(long h,int CLASS_TYPEID)=0;
	virtual CLDSPart* FromHandle(long h,int CLASS_TYPEID1,int CLASS_TYPEID2)=0;	//根据句柄获得节点
    virtual CLDSPart* FromHandle(long h,int CLASS_TYPEID1,int CLASS_TYPEID2,int CLASS_TYPEID3)=0;	//根据句柄获得节点
	virtual CLDSPart* append(BOOL bGetNewHandle=TRUE){return NULL;}
	virtual CLDSPart *append(int CLASS_TYPEID,BOOL bGetNewHandle=TRUE)=0;
	virtual BOOL  DeleteNode(long h,BOOL bIncPerm=TRUE)=0;
};*/
struct NODERELAINFO{
	long handle;
	UINT uiVisits;	//遍历时的访问次数
	CLDSNode* pNode;
	PRESET_ARRAY8<NODERELAINFO*> relationNodes;
	PRESET_ARRAY8<CLDSLinePart*> relationRods;
public:
	NODERELAINFO(){handle=uiVisits=0;pNode=NULL;}
	~NODERELAINFO();
	bool Visit(void (*taskfunc)(NODERELAINFO*pSubRelaNode,void* pContext),void* pContext=NULL,UINT maxVisits=2,UINT maxSubVisits=1);
	void SetKey(long key){handle = key;}
};
struct PROGRESS_CONTEXT{
	const char* title;
	BYTE ciStartProgressNumber,ciToProgressNumber;
	int Mapping(double percent);	//percent为百分数,将0~100的百分比映射转换至ciStartProgressNumber~ciToProgressNumber区间
	PROGRESS_CONTEXT();
};
class IModel
{
/*
public:
	CLdsList<CDisplayView> View;	//显示视图
	CLdsList<CLDSPoint> Point;			//基准点
	CLdsList<CLDSLine>  Line;			//基准线
	CLdsList<CLDSPlane> Plane;			//基准平面
    CLdsList<CLDSNode> Node;			//节点
	CLdsPartList Parts;					//各种型钢制成的铁塔构件，如角钢、钢板、钢管、槽钢、螺栓等
	CLdsList<CLDSArcLift> ArcLift;	//圆弧预拱项
*/
protected:
	virtual void* Get_hashNodeRelations(){return NULL;}//CHashListEx<NODERELAINFO>;
public:
	IModel(){;}
	virtual ~IModel(){;}
	virtual void InitializeNodeRelations();
	virtual NODERELAINFO* GetNodeRelaInfo(long hNode);
	virtual void ValidateObjRefH(THANDLE* phObjRef,bool neglectHighBit=false)	//phObjRef无效时，进行*phObjRef=0操作
	{
		long hObj=*phObjRef;
		if(neglectHighBit)
			hObj&=0x7fffffff;	//高位为0x80000000可能代表基准平面中的参照节点句柄;
		if(phObjRef!=NULL && FromHandle(hObj)==NULL)
			*phObjRef=0;
	}
	virtual long GetNewHandle()=0;
	virtual BYTE GetRollProcessType(){return 0;}	//卷边处理方式默认值为0
	virtual BYTE GetRollOffsetType(){return 0;}		//卷边偏移方式默认为内侧
	virtual BYTE GetDeformType(){return 0;}			//火曲变形处理方式
	virtual double GetDeformCoef(){return 0.35;}	//火曲变形系数
	virtual double GetHuoquR(){return 0;}			//火曲半径
	virtual DWORD ShrinkObjHandle(DWORD hStartShrink=0x20){return 0x20;}	//收缩句柄号
	void AddMirRelativeObjInfo(PROGRESS_CONTEXT* pDisplayProcessContext=NULL);
	virtual long GetMinAvailableHandle();//初始化最小可能的图元句柄;
	virtual bool IsTowerModel()=0;
	virtual bool EvolveCfgword(CFGWORD& cfgword,int version,long doc_type){return false;}
	virtual BYTE RepositionAlgor(){return false;}
	//获取杆塔模型空间中直接管理并实际存在的构件数组，filterShadowPart=true表示只计入母本构件，影射构件将被怱略
	//pPolyPartList为CLdsList<CLDSPolyPart>*类型
	virtual bool GetRealityPartArr(DYN_ARRAY<CLDSBolt*>& arrBoltPtr,DYN_ARRAY<CLDSPart*>& arrPartPtr,
		void *pPolyPartList,bool filterShadowPart=false)=0;
public:	//根据句柄查找对象
	virtual CLDSObject *FromHandle(long h,BOOL bDbObjOnly=FALSE)=0;
	virtual CDisplayView *FromViewHandle(long h)=0;
	virtual CParaCS *FromAcsHandle(long hAcs)=0;
	virtual CLDSPoint *FromPointHandle(long h)=0;
	virtual CLDSLine *FromLineHandle(long h)=0;
	virtual CLDSPlane *FromPlaneHandle(long h)=0;
	virtual CLDSNode *FromNodeHandle(long h)=0;
	virtual CLDSPart *FromPartHandle(long h,int CLASS_TYPEID=0)=0;
	virtual CLDSPart* FromPartHandle(long h,int CLASS_TYPEID1,int CLASS_TYPEID2)=0;	//根据句柄获得节点
    virtual CLDSPart* FromPartHandle(long h,int CLASS_TYPEID1,int CLASS_TYPEID2,int CLASS_TYPEID3)=0;	//根据句柄获得节点
	virtual CLDSLinePart *FromRodHandle(long hRod)=0;
	virtual CLDSArcLift *FromArcLiftHandle(long h)=0;
public:	//约束相关的函数
	virtual CONSTRAINT* AppendConstraint(CONSTRAINT* pCopyFrom=NULL)=0;
	virtual CONSTRAINT* GetConstraint(long constraint_id)=0;
	virtual bool RemoveConstraint(long constraint_id,long hFromMasterObj=0)=0;
	virtual CONSTRAINT* EnumConstraintFirst(BOOL bIterDelete=FALSE)=0;
	virtual CONSTRAINT* EnumConstraintNext(BOOL bIterDelete=FALSE)=0;
	virtual void ClearConstraintSyncStates()=0;
public:	//遍历函数
	virtual long GetObjNum(int CLASS_TYPEID)=0;
	virtual CDisplayView* EnumViewFirst(BOOL bIterDelete=FALSE)=0;
	virtual CDisplayView* EnumViewNext(BOOL bIterDelete=FALSE)=0;
	virtual CLDSNode* EnumNodeFirst(BOOL bIterDelete=FALSE)=0;
	virtual CLDSNode* EnumNodeNext(BOOL bIterDelete=FALSE)=0;
	virtual CLDSPart* EnumPartFirst(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE)=0;
	virtual CLDSPart* EnumPartNext(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE)=0;
	virtual CLDSNode* MoveToTailNode(BOOL bIterDelete=FALSE)=0;
	virtual CLDSPart* MoveToTailPart(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE)=0;
	virtual CLDSLinePart* EnumRodFirst()=0;
	virtual CLDSLinePart* EnumRodNext()=0;
	virtual CLDSPoint* EnumPointFirst(BOOL bIterDelete=FALSE)=0;
	virtual CLDSPoint* EnumPointNext(BOOL bIterDelete=FALSE)=0;
	virtual CLDSLine* EnumLineFirst(BOOL bIterDelete=FALSE)=0;
	virtual CLDSLine* EnumLineNext(BOOL bIterDelete=FALSE)=0;
	virtual CLDSPlane* EnumPlaneFirst(BOOL bIterDelete=FALSE)=0;
	virtual CLDSPlane* EnumPlaneNext(BOOL bIterDelete=FALSE)=0;
	virtual int PushNodeStack()=0;
	virtual bool PopNodeStack(int pos=-1)=0;
	virtual int PushPartStack()=0;
	virtual bool PopPartStack(int pos=-1)=0;
	virtual bool SearchSortedColinearRods(CLDSLinePart* pOriginalRod,CTmaPtrList<CLDSLinePart,CLDSLinePart*>& rodset,bool* bStartTipIsStartNode,bool* bEndTipIsStartNode,double tolerance=0.1,double tolerance_tangent=0.001,bool (*pfnVerifyIterRod)(CLDSLinePart* pRod)=NULL);//double minColinearCosA=0.99
#ifdef __COMMON_PART_INC_
	virtual CLDSPartGroup* EnumPartGroupFirst()=0;
	virtual CLDSPartGroup* EnumPartGroupNext()=0;
	virtual int GetPartGroupPartSet(long hPartGroup,CTmaPtrList<CLDSPart,CLDSPart*> &partset,int CLASS_TYPEID=0,bool bClearLock=true,long* phExcludePartBranchArr=0,int count=1)=0;
	virtual long GetPartGroupNum()=0;
	//移自IModel类中 wjh-2014.7.01
	virtual void SmartCalAngleWingNorm(CLDSLinePart *pRod);
#endif
public:	//增加数据对象
	virtual CParaCS *AppendAcs()=0;	//增加装配坐标系
	virtual CDisplayView *AppendView(BOOL bGetNewHandle=TRUE)=0;
	virtual CLDSPoint *AppendPoint(BOOL bGetNewHandle=TRUE)=0;
	virtual CLDSLine *AppendLine(BOOL bGetNewHandle=TRUE)=0;
	virtual CLDSPlane *AppendPlane(BOOL bGetNewHandle=TRUE)=0;
	virtual CLDSNode *AppendNode(BOOL bGetNewHandle=TRUE)=0;
	virtual CLDSPart *AppendPart(int CLASS_TYPEID,BOOL bGetNewHandle=TRUE)=0;
	virtual CLDSLine * MatchDatumLine(const double* pxLineOrg,const double* pvLineVec);
	virtual CLDSPlane* MatchDatumPlane(const double* pxPlaneOrg,const double* pvPlaneStdNormal);
public:	//删除数据对象
	virtual BOOL DeleteAcs(long hAcs)=0;//删除装配坐标系
	virtual BOOL DeleteView(long h,BOOL bIncPerm=TRUE)=0;
	virtual BOOL DeleteNode(long h,BOOL bIncPerm=TRUE)=0;
	virtual BOOL DeletePart(long h,BOOL bIncPerm=TRUE)=0;
};
