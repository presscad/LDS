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
	virtual CLDSPart* FromHandle(long h,int CLASS_TYPEID1,int CLASS_TYPEID2)=0;	//���ݾ����ýڵ�
    virtual CLDSPart* FromHandle(long h,int CLASS_TYPEID1,int CLASS_TYPEID2,int CLASS_TYPEID3)=0;	//���ݾ����ýڵ�
	virtual CLDSPart* append(BOOL bGetNewHandle=TRUE){return NULL;}
	virtual CLDSPart *append(int CLASS_TYPEID,BOOL bGetNewHandle=TRUE)=0;
	virtual BOOL  DeleteNode(long h,BOOL bIncPerm=TRUE)=0;
};*/
struct NODERELAINFO{
	long handle;
	UINT uiVisits;	//����ʱ�ķ��ʴ���
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
	int Mapping(double percent);	//percentΪ�ٷ���,��0~100�İٷֱ�ӳ��ת����ciStartProgressNumber~ciToProgressNumber����
	PROGRESS_CONTEXT();
};
class IModel
{
/*
public:
	CLdsList<CDisplayView> View;	//��ʾ��ͼ
	CLdsList<CLDSPoint> Point;			//��׼��
	CLdsList<CLDSLine>  Line;			//��׼��
	CLdsList<CLDSPlane> Plane;			//��׼ƽ��
    CLdsList<CLDSNode> Node;			//�ڵ�
	CLdsPartList Parts;					//�����͸��Ƴɵ�������������Ǹ֡��ְ塢�ֹܡ��۸֡���˨��
	CLdsList<CLDSArcLift> ArcLift;	//Բ��Ԥ����
*/
protected:
	virtual void* Get_hashNodeRelations(){return NULL;}//CHashListEx<NODERELAINFO>;
public:
	IModel(){;}
	virtual ~IModel(){;}
	virtual void InitializeNodeRelations();
	virtual NODERELAINFO* GetNodeRelaInfo(long hNode);
	virtual void ValidateObjRefH(THANDLE* phObjRef,bool neglectHighBit=false)	//phObjRef��Чʱ������*phObjRef=0����
	{
		long hObj=*phObjRef;
		if(neglectHighBit)
			hObj&=0x7fffffff;	//��λΪ0x80000000���ܴ����׼ƽ���еĲ��սڵ���;
		if(phObjRef!=NULL && FromHandle(hObj)==NULL)
			*phObjRef=0;
	}
	virtual long GetNewHandle()=0;
	virtual BYTE GetRollProcessType(){return 0;}	//��ߴ���ʽĬ��ֵΪ0
	virtual BYTE GetRollOffsetType(){return 0;}		//���ƫ�Ʒ�ʽĬ��Ϊ�ڲ�
	virtual BYTE GetDeformType(){return 0;}			//�������δ���ʽ
	virtual double GetDeformCoef(){return 0.35;}	//��������ϵ��
	virtual double GetHuoquR(){return 0;}			//�����뾶
	virtual DWORD ShrinkObjHandle(DWORD hStartShrink=0x20){return 0x20;}	//���������
	void AddMirRelativeObjInfo(PROGRESS_CONTEXT* pDisplayProcessContext=NULL);
	virtual long GetMinAvailableHandle();//��ʼ����С���ܵ�ͼԪ���;
	virtual bool IsTowerModel()=0;
	virtual bool EvolveCfgword(CFGWORD& cfgword,int version,long doc_type){return false;}
	virtual BYTE RepositionAlgor(){return false;}
	//��ȡ����ģ�Ϳռ���ֱ�ӹ���ʵ�ʴ��ڵĹ������飬filterShadowPart=true��ʾֻ����ĸ��������Ӱ�乹�������S��
	//pPolyPartListΪCLdsList<CLDSPolyPart>*����
	virtual bool GetRealityPartArr(DYN_ARRAY<CLDSBolt*>& arrBoltPtr,DYN_ARRAY<CLDSPart*>& arrPartPtr,
		void *pPolyPartList,bool filterShadowPart=false)=0;
public:	//���ݾ�����Ҷ���
	virtual CLDSObject *FromHandle(long h,BOOL bDbObjOnly=FALSE)=0;
	virtual CDisplayView *FromViewHandle(long h)=0;
	virtual CParaCS *FromAcsHandle(long hAcs)=0;
	virtual CLDSPoint *FromPointHandle(long h)=0;
	virtual CLDSLine *FromLineHandle(long h)=0;
	virtual CLDSPlane *FromPlaneHandle(long h)=0;
	virtual CLDSNode *FromNodeHandle(long h)=0;
	virtual CLDSPart *FromPartHandle(long h,int CLASS_TYPEID=0)=0;
	virtual CLDSPart* FromPartHandle(long h,int CLASS_TYPEID1,int CLASS_TYPEID2)=0;	//���ݾ����ýڵ�
    virtual CLDSPart* FromPartHandle(long h,int CLASS_TYPEID1,int CLASS_TYPEID2,int CLASS_TYPEID3)=0;	//���ݾ����ýڵ�
	virtual CLDSLinePart *FromRodHandle(long hRod)=0;
	virtual CLDSArcLift *FromArcLiftHandle(long h)=0;
public:	//Լ����صĺ���
	virtual CONSTRAINT* AppendConstraint(CONSTRAINT* pCopyFrom=NULL)=0;
	virtual CONSTRAINT* GetConstraint(long constraint_id)=0;
	virtual bool RemoveConstraint(long constraint_id,long hFromMasterObj=0)=0;
	virtual CONSTRAINT* EnumConstraintFirst(BOOL bIterDelete=FALSE)=0;
	virtual CONSTRAINT* EnumConstraintNext(BOOL bIterDelete=FALSE)=0;
	virtual void ClearConstraintSyncStates()=0;
public:	//��������
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
	//����IModel���� wjh-2014.7.01
	virtual void SmartCalAngleWingNorm(CLDSLinePart *pRod);
#endif
public:	//�������ݶ���
	virtual CParaCS *AppendAcs()=0;	//����װ������ϵ
	virtual CDisplayView *AppendView(BOOL bGetNewHandle=TRUE)=0;
	virtual CLDSPoint *AppendPoint(BOOL bGetNewHandle=TRUE)=0;
	virtual CLDSLine *AppendLine(BOOL bGetNewHandle=TRUE)=0;
	virtual CLDSPlane *AppendPlane(BOOL bGetNewHandle=TRUE)=0;
	virtual CLDSNode *AppendNode(BOOL bGetNewHandle=TRUE)=0;
	virtual CLDSPart *AppendPart(int CLASS_TYPEID,BOOL bGetNewHandle=TRUE)=0;
	virtual CLDSLine * MatchDatumLine(const double* pxLineOrg,const double* pvLineVec);
	virtual CLDSPlane* MatchDatumPlane(const double* pxPlaneOrg,const double* pvPlaneStdNormal);
public:	//ɾ�����ݶ���
	virtual BOOL DeleteAcs(long hAcs)=0;//ɾ��װ������ϵ
	virtual BOOL DeleteView(long h,BOOL bIncPerm=TRUE)=0;
	virtual BOOL DeleteNode(long h,BOOL bIncPerm=TRUE)=0;
	virtual BOOL DeletePart(long h,BOOL bIncPerm=TRUE)=0;
};
