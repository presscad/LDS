#pragma once
#include "IModel.h"
#include "f_ent_list.h"
#include "Buffer.h"
#include "ArrayList.h"
#include "list.h"
#include "Expression.h"

struct EXTERNAL_BLKNODE{
	long hNode;
	GEPOINT xPosition;
	void SetKey(DWORD key){hNode=key;}
	EXTERNAL_BLKNODE(CLDSNode* pNode=NULL){
		hNode=pNode!=NULL?pNode->handle:0;
		if(pNode)
			xPosition=pNode->xPreliftPos;
	}
};
struct EXTERNAL_BLKROD{
	static const BYTE PROP_START_NODEPOS= 1;	//ʼ�˽ڵ�����
	static const BYTE PROP_START_TIPPOS = 2;	//ʼ�˶��涥������(�Ǹ���㣬�ֹ����߶˵㣩
	static const BYTE PROP_END_NODEPOS	= 3;	//�ն˽ڵ�����
	static const BYTE PROP_END_TIPPOS	= 4;	//�ն˶��涥������(�Ǹ���㣬�ֹ����߶˵㣩
	static const BYTE PROP_WING_NORM	= 5;	//�Ǹ���֫����
	BYTEFLAG cbValidProp;
public:
	long hRod;
	UINT idClsType;
	GEPOINT vxWingNorm,vyWingNorm;	//��������ϽǸ��ӽǸֵ�ʶ��
	GEPOINT xStartPosition;		//�ø˼��ڲ���ģ���е�ʼ�����꣬���ڸ��������������������Զ�ʶ����ؽڵ�
	GEPOINT xEndPosition;		//�ø˼��ڲ���ģ���е�ĩ�����꣬���ڸ��������������������Զ�ʶ����ؽڵ�
	void SetKey(DWORD key){hRod=key;}
	EXTERNAL_BLKROD(CLDSLinePart* pRod=NULL){
		hRod=pRod!=NULL?pRod->handle:0;
		idClsType=cbValidProp=0;
		if(pRod)
		{
			idClsType=pRod->GetClassTypeId();
			xStartPosition=pRod->pStart!=NULL?pRod->pStart->xPreliftPos:pRod->Start();
			xEndPosition=pRod->pEnd!=NULL?pRod->pEnd->xPreliftPos:pRod->End();
			if(pRod->pStart)
				cbValidProp.SetBitState(PROP_START_NODEPOS);
			else
				cbValidProp.SetBitState(PROP_START_TIPPOS);
			if(pRod->pEnd)
				cbValidProp.SetBitState(PROP_END_NODEPOS);
			else
				cbValidProp.SetBitState(PROP_END_TIPPOS);
			if(pRod->IsAngle())
			{
				cbValidProp.SetBitState(PROP_WING_NORM);
				vxWingNorm=((CLDSLineAngle*)pRod)->vxWingNorm;
				vyWingNorm=((CLDSLineAngle*)pRod)->vyWingNorm;
			}
		}
	}
};
struct EXTERNAL_BLKBOLT{
public:
	long hBolt;
	WORD d,wL;	//��˨ֱ������Ч����
	GEPOINT vWorkNorm;		//��˨���幤�����߷���
	GEPOINT xBasePosition;	//װ�����������
	void SetKey(DWORD key){hBolt=key;}
	EXTERNAL_BLKBOLT(CLDSBolt* pBolt=NULL){
		hBolt=pBolt!=NULL?pBolt->handle:0;
		if(pBolt)
		{
			d=pBolt->d;
			wL=(WORD)pBolt->get_L();
			vWorkNorm=pBolt->ucs.axis_z;
			xBasePosition=pBolt->ucs.origin;
		}
		else
		{
			d=16;
			wL=45;
		}
	}
};
class CBlockModel : public IModel//CLDSObject
{
protected:
	UCS_STRU lcs;			//װ�䶨λ����ϵ(Locate Coordinate System)
	BOOL m_bDataModified;	//�����޸ı�ʶ
	BOOL m_bEmbeded;		//��Ƕʽ�����飨�ӹ������ڵ�ǰ��ģ���У�
public:	//װ��ʱͼ���涯�̽�����ϵ����
	CParaCS des_lcs;//װ������ϵ��Ʋ���
	CHashList<CParaCS> Acs;			//������ϵidΪ��ֵ�洢����ϵ
	static const int COMMON_BLOCK = 0;
	static const int PARAMETRIC_BLOCK = 1;
public:
	THANDLE handle;					//�������
	THANDLE m_hMinIdleHandle;
	char name[51];					//��������������
	DWORD dwPermission;			//Ȩ���֣����ڱ�ʶ˭�Դ���ӵ���޸�Ȩ��
	int feature;	//�����û��Լ�����Ϣ(��ʱ����,���ɳ־ô洢��Ҳ��һ������)
	CHashListEx<CONSTRAINT>hashConstraints;
	CLdsList<CDisplayView> View;	//��ʾ��ͼ
	CLdsList<CLDSPoint> Point;			//��׼��
	CLdsList<CLDSLine>  Line;			//��׼��
	CLdsList<CLDSPlane> Plane;			//��׼ƽ��
    CLdsList<CLDSNode> Node;			//�ڵ�
	CLdsPartList Parts;					//�����͸��Ƴɵ�������������Ǹ֡��ְ塢�ֹܡ��۸֡���˨��
	//�����ڵ�ǰ���������ڲ����ڵĸ˼��ڵ�Ĺ����������漰�ĸ˼���ڵ㣬��������Ϣ�������Ա�������ģ���н����Զ�ƥ��
	CHashListEx<EXTERNAL_BLKNODE> hashExterNodes;
	CHashListEx<EXTERNAL_BLKROD>  hashExterRods;
	CHashListEx<EXTERNAL_BLKBOLT> hashExterBolts;
	//CLdsList<CLDSArcLift> ArcLift;	//Բ��Ԥ����
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLdsList<CLDSPartGroup> PartGroup;
#endif
	long (*GetNewHandleHookFunc)();
	//��û�����ͨ�����Ӻ���������ݶ����Ĳ�ѯ���⣬�ݲ��������������� wjh-2014.5.26
	//CLDSNode (*AppendNodeHookFunc)(BOOL bGetNewHandle);
	//CLDSPart (*AppendPartHookFunc)(int CLASS_TYPEID,BOOL bGetNewHandle);
public:
	CBlockModel(BOOL embeded=TRUE);
	virtual ~CBlockModel(void);
	BOOL IsEmbeded(){return m_bEmbeded;}
	void SetEmbeded(BOOL bEmbeded){m_bEmbeded=bEmbeded;}
	long GetMinValidObjHandle(bool bIncViewObj=false);//�����С���õĶ�����;
	virtual long GetNewHandle();
	virtual DWORD ShrinkObjHandle(DWORD hStartShrink=0x20);	//���������
	virtual bool IsTowerModel(){return false;}
	//��ȡ����ģ�Ϳռ���ֱ�ӹ���ʵ�ʴ��ڵĹ������飬filterShadowPart=true��ʾֻ����ĸ��������Ӱ�乹�������S��
	//pPolyPartListΪCLdsList<CLDSPolyPart>*����
	virtual bool GetRealityPartArr(DYN_ARRAY<CLDSBolt*>& arrBoltPtr,DYN_ARRAY<CLDSPart*>& arrPartPtr,
								   void *pPolyPartList,bool filterShadowPart=false);
	int RetrievePolyPartList(POLYPARTLIST &list);
	BOOL IsModified(){return m_bDataModified;}
	void UpdateLCS();	//���¶�λ����ϵ
	void UpdatePos();	//����ԭ��λ��
	//���벿��װ�䶨λ����ϵ���Ӧ��װ������ϵת�����벿��ԭʼ�������Ӧ��װ������ϵ
	UCS_STRU TransToACS(const UCS_STRU& cs);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer& buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	bool ExportToBlkFile(const char* blk_file,long version,IModel *pTowerModel=NULL,BOOL bSaveTaAsModel=FALSE);
	bool ImportFromBlkFile(char* blk_file);
	bool ImportFromXMLFile(const char* xml_file);
	BOOL SelectDisplaySet(NODESET& DispNodeSet,PARTSET& DispPartSet,CDisplayView *pView=NULL);
	UCS_STRU GetLCS(){return lcs;}
	void (*DisplayProcess)(int percent,const char *sTitle);	//������ʾ�ص�����
	BOOL CheckNodeIntegrality(IModel *pModel,NODESET& nodeSet);		//��鲿���ڵ�������
	void Empty();
public:
	virtual CDisplayView *FromViewHandle(long h){return View.FromHandle(h);}
	virtual CLDSObject *FromHandle(long h,BOOL bDbObjOnly=FALSE);
	virtual CParaCS *FromAcsHandle(long hAcs);
	virtual CLDSPoint *FromPointHandle(long h){return Point.FromHandle(h);}
	virtual CLDSLine *FromLineHandle(long h){return Line.FromHandle(h);}
	virtual CLDSPlane *FromPlaneHandle(long h){return Plane.FromHandle(h);}
	virtual CLDSNode *FromNodeHandle(long h){return Node.FromHandle(h);}
	virtual CLDSPart *FromPartHandle(long h,int CLASS_TYPEID=0){return Parts.FromHandle(h,CLASS_TYPEID);}
	virtual CLDSPart* FromPartHandle(THANDLE h,int CLASS_TYPEID1,int CLASS_TYPEID2)
	{		return Parts.FromHandle(h,CLASS_TYPEID1,CLASS_TYPEID2);	}//���ݾ����ýڵ�
    virtual CLDSPart* FromPartHandle(THANDLE h,int CLASS_TYPEID1,int CLASS_TYPEID2,int CLASS_TYPEID3)
	{		return Parts.FromHandle(h,CLASS_TYPEID1,CLASS_TYPEID2,CLASS_TYPEID3);	}//���ݾ����ýڵ�
	virtual CLDSLinePart *FromRodHandle(long hRod);
	virtual CLDSArcLift *FromArcLiftHandle(long h){return NULL;}
public:	//Լ����صĺ���
	virtual CONSTRAINT* AppendConstraint(CONSTRAINT* pCopyFrom=NULL);
	virtual CONSTRAINT* GetConstraint(long constraint_id);
	virtual bool RemoveConstraint(long constraint_id,long hFromMasterObj=0);
	virtual CONSTRAINT* EnumConstraintFirst(BOOL bIterDelete=FALSE);
	virtual CONSTRAINT* EnumConstraintNext(BOOL bIterDelete=FALSE);
	virtual void ClearConstraintSyncStates();
public:	//��������
	virtual long GetObjNum(int CLASS_TYPEID);
	virtual CDisplayView* EnumViewFirst(BOOL bIterDelete=FALSE){return View.GetFirst(bIterDelete);}
	virtual CDisplayView* EnumViewNext(BOOL bIterDelete=FALSE){return View.GetNext(bIterDelete);}
	virtual CLDSNode* EnumNodeFirst(BOOL bIterDelete=FALSE){return Node.GetFirst(bIterDelete);}
	virtual CLDSNode* EnumNodeNext(BOOL bIterDelete=FALSE){return Node.GetNext(bIterDelete);}
	virtual CLDSPart* EnumPartFirst(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE){return Parts.GetFirst(CLASS_TYPEID,bIterDelete);}
	virtual CLDSPart* EnumPartNext(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE){return Parts.GetNext(CLASS_TYPEID,bIterDelete);}
	virtual CLDSNode* MoveToTailNode(BOOL bIterDelete = FALSE) { return Node.GetTail(bIterDelete); }
	virtual CLDSPart* MoveToTailPart(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE){return Parts.GetTail(CLASS_TYPEID,bIterDelete);}
	virtual CLDSLinePart* EnumRodFirst(){return Parts.GetFirstLinePart();}
	virtual CLDSLinePart* EnumRodNext(){return Parts.GetNextLinePart();}
	virtual CLDSPoint* EnumPointFirst(BOOL bIterDelete=FALSE){return Point.GetFirst(bIterDelete);}
	virtual CLDSPoint* EnumPointNext(BOOL bIterDelete=FALSE){return Point.GetNext(bIterDelete);}
	virtual CLDSLine* EnumLineFirst(BOOL bIterDelete=FALSE){return Line.GetFirst(bIterDelete);}
	virtual CLDSLine* EnumLineNext(BOOL bIterDelete=FALSE){return Line.GetNext(bIterDelete);}
	virtual CLDSPlane* EnumPlaneFirst(BOOL bIterDelete=FALSE){return Plane.GetFirst(bIterDelete);}
	virtual CLDSPlane* EnumPlaneNext(BOOL bIterDelete=FALSE){return Plane.GetNext(bIterDelete);}
	virtual int PushNodeStack(){return Node.push_stack();}
	virtual bool PopNodeStack(int pos=-1){return Node.pop_stack(pos);}
	virtual int PushPartStack(){return Parts.push_stack();}
	virtual bool PopPartStack(int pos=-1){return Parts.pop_stack(pos);}
#ifdef __COMMON_PART_INC_
	virtual CLDSPartGroup* EnumPartGroupFirst(){return PartGroup.GetFirst();}
	virtual CLDSPartGroup* EnumPartGroupNext(){return PartGroup.GetNext();}
	virtual int GetPartGroupPartSet(long hPartGroup,PARTSET &partset,int CLASS_TYPEID=0,bool bClearLock=true,long* phExcludePartBranchArr=0,int count=1);
	virtual long GetPartGroupNum(){return PartGroup.GetNodeNum();}
#endif
public:	//�������ݶ���
	virtual CParaCS *AppendAcs();	//����װ������ϵ
	virtual CDisplayView *AppendView(BOOL bGetNewHandle=TRUE){return View.append(bGetNewHandle);}
	virtual CLDSPoint *AppendPoint(BOOL bGetNewHandle=TRUE){return Point.append(bGetNewHandle);}
	virtual CLDSLine *AppendLine(BOOL bGetNewHandle=TRUE){return Line.append(bGetNewHandle);}
	virtual CLDSPlane *AppendPlane(BOOL bGetNewHandle=TRUE){return Plane.append(bGetNewHandle);}
	virtual CLDSNode *AppendNode(BOOL bGetNewHandle=TRUE){return Node.append(bGetNewHandle);}
	virtual CLDSPart *AppendPart(int CLASS_TYPEID,BOOL bGetNewHandle=TRUE){return Parts.append(CLASS_TYPEID,bGetNewHandle);}
public:	//ɾ�����ݶ���
	virtual BOOL DeleteAcs(long hAcs);//ɾ��װ������ϵ
	virtual BOOL DeleteView(long h,BOOL bIncPerm=TRUE){return View.DeleteNode(h,bIncPerm);}
	virtual BOOL DeleteNode(long h,BOOL bIncPerm=TRUE){return Node.DeleteNode(h,bIncPerm);}
	virtual BOOL DeletePart(long h,BOOL bIncPerm=TRUE){return Parts.DeleteNode(h,bIncPerm);}
	//
	bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents){return true;}
	//����
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CXhChar100 HELPLINK_URL;
	UPDATE_PROPLISTITEM_FUNC(CBlockModel,propHashtable)
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	int GetPropValueStr(long id,char* valueStr);	//��������ID�õ�����ֵ�ַ���
	static void InitBlockModelPropHashtable();
	static long GetPropID(char* propStr);			//�õ�����ID
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
};
class CParaBlockModel;
/* �������������lds_part.h��
#include "Expression.h"
union KEY4C{
private:
	char label[4];
public:
	DWORD dwItem;
	struct{
		WORD wItem;	//key��ǰ�벿��2�ֽ�һ����Ϊѭ������������ʶ
		WORD index;	//key�к�벿��2�ֽ�һ����Ϊѭ��������������ʶ
	}pair;
public:
	operator DWORD(){return dwItem;}
	KEY4C(DWORD dwKey=0){dwItem=dwKey;}
	KEY4C(WORD wItem,WORD wIndex){pair.wItem=wItem;pair.index=wIndex;}
	KEY4C(char* keyStr);
	char* Label();//����ָ������ʱ�ַ���ָ��, ����һ�ε���KEY4C::Label()ǰ��Ч
	bool IsEqual(KEY4C key){return dwItem==key.dwItem;}
};
*/
#include "LifeObj.h"
class CParaModelNode;
class CParaModelRod;
class CParaModelParamPlate;
struct TAG_NODE_INFO : public TAG_INFO<CParaModelNode>
{
	CLDSNode* pModelNode;
	TAG_NODE_INFO(){pModelNode=NULL;}
};
struct TAG_ROD_INFO : public TAG_INFO<CParaModelRod>
{
	CLDSLinePart* pModelRod;
	TAG_ROD_INFO(){pModelRod=NULL;}
};
struct TAG_PLATE_INFO : public TAG_INFO<CParaModelParamPlate>
{
	CLDSParamPlate* pModelPlate;
	TAG_PLATE_INFO(){pModelPlate=NULL;}
};
class CParaModelNode{
protected:
	TAG_NODE_INFO* pTagInfo;
public:
	CParaModelNode();
	virtual ~CParaModelNode();
	void SetTagInfo(void* pTag){pTagInfo=(TAG_NODE_INFO*)pTag;}
	TAG_NODE_INFO* TagInfo(){return pTagInfo;}
	//void ToStream(IStream *pStream);
	//void FromStream(IStream *pStream);
	char sPreferLayer[3];		//��ģ��ָ�����Ż�ͼ����
	KEY4C keyLabel;				//�ڵ��ģ���ʶ
	KEY4C keyLabelFatherRod;	//�������˼���ģ���ʶ
	char posx_expression[50];	//
	char posy_expression[50];
	char posz_expression[50];
	char scale_expression[50];
	char offset_expression[50];
	char exist_logicexpr[20];	//�ýڵ�����Ƿ���ڵ��߼����ʽ��Ŀǰ�ñ��ʽ��ֻ�ܳ���һ��'<'��'>'��'='��'<='��'>='��'!='�߼�������
	//bool m_bInternodeRepeat;	//�ڼ��е��ظ��ڵ㣬�ظ�����Ϊ"i"
	KEY4C keyRepeatParam;		//>0ʱ��ʾ����ýڵ��ظ�������CBlockParaModel::listParams������ֵ���ظ�����Ϊ"i"����ֵΪ�ظ��ڵ��������Ľڵ��ظ����ߵı�ʶ
	/*
		0-�������κ������Ľڵ�;
		1-�˼���X����ֵ�����;
		2-�˼���Y����ֵ�����;
		3-�˼���Z����ֵ�����;
		4-���˼����潻��;
		5-�˼��ϵı����ȷֵ�;
	*/
	short attach_type;	
	char ciOffsetType;	//0:����;'X':X�������;'Y':Y�������;'Z':Z�������
	double attach_scale;	//��ֵ��ʾС����ʽ�ı���λ��ֵ,��ֵ��ʾ����.��ĸ
	WORD wiOffsetByPrevNode;	//i>ciOffsetByPrevNodeʱ:��ʾ��ǰһƫ�ƽڵ�Ϊ��׼����ƫ��
	KEY4C attachNodeArr[4];
public:	//����������ͣ�attach_type
	static const BYTE INDEPENDENT		 = 0;
	static const BYTE COORD_X_SPECIFIED  = 1;
	static const BYTE COORD_Y_SPECIFIED  = 2;
	static const BYTE COORD_Z_SPECIFIED  = 3;
	static const BYTE INTERSECTION		 = 4;
	static const BYTE SCALE				 = 5;
	static const BYTE COORD_OFFSET		 = 7;
};
class CParaModelPart{
public:
	CParaModelPart();
	KEY4C keyLabel;	//��ǩ���ַ���ʽ�ı�ʶ�����ڲ�������ģʱ����Id�������б��ڹ�����ţ�
	char sPreferLayer[3];		//��ģ��ָ�����Ż�ͼ����
	long m_idPartClsType;	//�ø˼��Ĺ������ͣ���Ǹ֡��ֹܡ��ְ��, CLS_LINEANGLE,CLS_LINETUBE
	char cMaterial;	//'S':Q235;'H':Q345;'G':Q390;'T':Q420;'U':Q460
	bool m_bInternodeRepeat;	//�ڼ��е��ظ��˼����ظ�����Ϊ"i"
	char exist_logicexpr[20];	//�ýڵ�����Ƿ���ڵ��߼����ʽ��Ŀǰ�ñ��ʽ��ֻ�ܳ���һ��'<'��'>'��'='��'<='��'>='��'!='�߼�������
};
class CParaModelRod : public CParaModelPart{
protected:
	TAG_ROD_INFO* pTagInfo;
public:
	CParaModelRod();
	virtual ~CParaModelRod();
	virtual bool IsAngleObject(){return false;}
	void SetTagInfo(void* pTag){pTagInfo=(TAG_ROD_INFO*)pTag;}
	TAG_ROD_INFO* TagInfo(){return pTagInfo;}
	//void ToStream(IStream *pStream);
	//void FromStream(IStream *pStream);
	KEY4C keyRefPole,keyRefPole2,keyLamdaRefPole;
	KEY4C keyStartNode,keyEndNode;
	BYTE  nIterStep;	//�ظ��˼�ʱ�ڵ���������������, Ĭ��Ϊ1
	CXhChar16 indexParaStart,indexParaEnd;	//���ڿ��ظ��ڵ��������������"i"��"i+1"��"i+N"��
	CXhChar16 m_sGuige;
	BYTE start_joint_type,end_joint_type;
	BYTE start_force_type,end_force_type;
	short callen_type;
	short layout_style;		//�ǸֵĲ��÷�ʽ�����������ڳ��Ͽڳ��£���ȡֵ0,1,2,3
	short m_iElemType;		//0��ʾ�Զ��ж�;1��ʾ���������;2��ʾ��������Ԫ;3��ʾ������Ԫ
	union{
		COORD3D eyeSideNorm;	//���ڽǸ����۹ۿ���ķ��߷���
		COORD3D pickWorkPlane;	//����ȷ���ֹܻ�׼�����ڹ������ʰȡ�� wjh-2015.9.1
	};
	BOOL m_bMajorMat;		//����
public:
	//JOINT_TYPE ���͵�ö��ֵ
	static const BYTE JOINT_HINGE	= 0;	//�½�
	static const BYTE JOINT_RIGID	= 1;	//�ս�
	static const BYTE JOINT_AUTOJUSTIFY = 2;//�Զ��ж�
	//�˼����˵��������� FORCE_TYPE
	static const BYTE CENTRIC_FORCE		= 0;//��������
	static const BYTE ECCENTRIC_FORCE	= 1;//ƫ������
	//�˼������޵�Ԫ�������� ELEM_TYPE
	static const BYTE FEMELEM_TRUSS	= 1;//��������ܵ�Ԫ
	static const BYTE FEMELEM_BEAM	= 2;//��������Ԫ
	static const BYTE FEMELEM_GUYS	= 3;//������Ԫ
};
class CParaModelAngle : public CParaModelRod{
public:
	struct POS{
	BYTE cTouchLine;			//0.X֫����;1.Y֫����;2.����
	BYTE cTouchLineType;		//0.����;1.������;2.ƫ��׼��;3.���Ƶ�
	BYTE cFaceOffsetDatumLine;  //ƽ��ģʽ�µ�����ƽ�ƻ�׼��
	GEPOINT face_offset_norm;	//ƽ���淨��
	KEY4C labelHuoquRod;
	BYTE coordCtrlType;	//0:X-Z����;1:Y-Z����;2.X-Y����(����spatialOperationStyle=15ʱ��Ч)
	BYTE cMainCoordCtrlWing;//�Ǹ�����֫��'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	WING_OFFSET_STYLE XOrCtrlWing;	//�Ǹ���Ҫ����֫(�����������������֫)
	WING_OFFSET_STYLE YOrOtherWing;//�Ǹָ�������֫(����һ�����������֫)
	}desStart,desEnd;
public:
	CParaModelAngle();
	virtual bool IsAngleObject(){return true;}
};
class CParaModelGeneralPlate : public CParaModelPart{
public:
	int m_iParamType;	//0.��ʾ��ͨ�ְ�;>0��ʾ�������������
	CDesignLjPara designInfo;
};
class CParaModelParamPlate : public CParaModelPart{
protected:
	TAG_PLATE_INFO* pTagInfo;
public:
	static const int TYPE_D_TOPPLATE	=1;	//�˳Ŷ���
	static const int TYPE_D_ENDPLATE	=2;	//�˳Ŷ˰�
	static const int TYPE_D_CHAPLATE	=3;	//�˳Ų��
	static const int TYPE_D_LINKPLATE	=4;	//�˳�����
	static const int TYPE_L_NODEPLATE	=5;	//��������
	static const int TYPE_L_TOPPLATE	=6;	//��������
	static const int TYPE_L_JIAPLATE	=7;	//�����а�
	static const int TYPE_Q_TOPPLATE	=8;	//��������
	static const int TYPE_Q_BASEPLATE	=9;	//��������
	static const int TYPE_Q_RIBPLATE	=10;//�����߰�
public:
	int m_iParamType;	//0.��ʾ��ͨ�ְ�;>0��ʾ�������������
	CDesignLjPara designInfo;
	long _thick;
	BOOL _bStdPart;	//TRUE:��׼���� FALSE:�Ǳ�׼����
	KEY4C keyDatumNode;
	KEY4C keyDatumPart,keyAssistPart,keyAssistPart2;
	ATOM_LIST<ATTACHPART_ITEM> AttachPart;
	CLDSVector heteroFlConnVec[3];	//���´����ͷ����������˼����ӷ���
	ATOM_LIST<CONNECT_DIRECTION> ConnDirectList;
	ATOM_LIST<DESIGN_PLATE_ITEM>designItemList;
	CLDSVector workNorm;	//�������ߵ���Ʋ���
	char off_expression[50];	
	int feature;			//��ʱʹ��
	//
	CParaModelParamPlate();
	~CParaModelParamPlate();
	void SetTagInfo(void* pTag){pTagInfo=(TAG_PLATE_INFO*)pTag;}
	TAG_PLATE_INFO* TagInfo(){return pTagInfo;}
};
class CParaBlockModelTemplate
{
protected:
	ATOM_LIST<TAG_NODE_INFO>listTagNodeInfo;		//��ʱ�ҽӵĸ��ӵ�������{PART_ITEM}�ϵ����ݶ�������
	ATOM_LIST<TAG_ROD_INFO>listTagRodInfo;			//��ʱ�ҽӵĸ��ӵ�������{PART_ITEM}�ϵ����ݶ�������
	ATOM_LIST<TAG_PLATE_INFO>listTagPlateInfo;		//��ʱ�ҽӵĸ��ӵ�������{PART_ITEM}�ϵ����ݶ�������
	CHashList<CParaModelNode>listRepeatedParaNodes;	//��ʱ�洢�ظ����ɵ����нڵ�
	CSuperHashList<CParaModelRod>listRepeatedParaRods;	//��ʱ�洢�ظ����ɵ����и˼�
public:
	static const int SUBSTATION_TEMPLATE	= 0;		//��繹��ģ��
	static const int CROSSARM_TEMPLATE		= 1;		//���ͺᵣģ��
	//static const int TYPE_PLATE			= 5;		//CLS_PLATE
	//static const int TYPE_PARAMPLATE		= 6;		//CLS_PARAMPLATE
	//static const int TYPE_LINEANGLE		=13;		//CLS_LINEANGLE
	//static const int TYPE_LINETUBE		=17;		//CLS_LINETUBE
public:
	int m_iTempType;	//ģ�����ͣ�0:��繹��ģ��  1�����ͺᵣģ�� 
	/*
	��繹��ģ��
	0.����������   1.�ķ�������
	���ͺᵣģ��Ĳ�ͬ��ʽ��ţ�
	0.�������κᵣ 1.�����Ǻᵣ
	*/
	int m_iTempIndex;	//ͬһģ�����͵Ĳ�ͬģ�����
	CHashList<DESIGN_PARAM_ITEM>listParams;
	CHashList<CParaModelNode>listParaNodes;
	CSuperHashList<CParaModelRod>listParaRods;
	CHashList<CParaModelParamPlate> listParaPlates;

	//virtual DESIGN_PARAM_ITEM* AddParaItem(DWORD key,bool glbalControlled=false);
	//virtual CParaModelNode* AddParaNode(DWORD key,bool glbalControlled=false);
	//virtual CParaModelRod* AddParaRod(DWORD key,int idClsPartType=0);
	//virtual CParaModelParamPlate* AddParaPlate(DWORD key);
	//virtual DESIGN_PARAM_ITEM* GetParamItem(DWORD key);
	virtual CParaModelNode* GetParaNode(DWORD key);	//��listParaNodes��listRepeatedParaNodes�в��Ҳ������ڵ�ģ��
	virtual CParaModelRod* GetParaRod(DWORD key);	//��listParaRods��listRepeatedParaRods�в��Ҳ������ڵ�ģ��
	virtual CParaModelParamPlate* GetParaPlate(DWORD key);
	//virtual bool LoadTemplate(UINT uidTemplType);
	//virtual void LogError(char* error);
public:
	CParaBlockModelTemplate();
	virtual ~CParaBlockModelTemplate();
	//��CParaModelNode�Ѿ��йҽӶ���ʱִ��TagInfo(...), ���򴴽��¹ҽ����ݶ���󷵻�
	TAG_NODE_INFO* LoadTagInfo(CParaModelNode* pItem);
	TAG_ROD_INFO*  LoadTagInfo(CParaModelRod*  pItem);
	TAG_PLATE_INFO* LoadTagInfo(CParaModelParamPlate* pItem);
	CLDSNode* InsertNewNode(CParaBlockModel* pBlockModel,CParaModelNode* pParaNode,f3dPoint pos);
	CLDSLinePart* InsertNewPole(CParaBlockModel* pBlockModel,CParaModelRod* pParaRod);
	CLDSParamPlate* InsertNewPlate(CParaBlockModel* pBlockModel,CParaModelParamPlate* pParaPlate,CExpression& expression);
	void FillMirInfo(CParaBlockModel* pBlockModel);			//ƥ��ڵ㼰�˼��ĶԳ���Ϣ
	virtual void RebuildModel(CParaBlockModel* pBlockModel);		//��������ģ��, ��ʵ��
	//virtual void RebuildModelEx(CParaBlockModel* pBlockModel);		//��������ģ��, ��ʵ��
	void ResetPosition(CParaBlockModel* pBlockModel);
	bool MechanicsDesign(CParaBlockModel* pBlockModel);
	bool StructuralDesign(CParaBlockModel* pBlockModel);
};
/* �������������IModel.h��
struct DESIGN_PARAM_ITEM
{
	KEY4C key;			//�ò����ı�ʶ��ֵ���򲻳���4�������ַ�����
	CXhChar50 sLabel;	//����������ʾ�Ķ����Ʊ�ǩ
	CXhChar100 sNotes;	//�����жԸ������ԵĽ���������
	union{double fValue;THANDLE hPart;long iType;}value;
	BYTE dataType;	//0.double��ֵ 1.��� 2. ����
	BOOL bModify;
	DESIGN_PARAM_ITEM(){dataType=0; bModify=FALSE;}
	EXPRESSION_VAR ToExprVar();
public:	//dataType����������
	static const BYTE TYPE_DOUBLE	= 0;
	static const BYTE TYPE_HANDLE	= 1;
	static const BYTE TYPE_INTEGER	= 2;
};
*/
struct IParaBlockContext{
	virtual CLDSNode* GetKeyNode(DWORD index)=0;
	virtual DESIGN_PARAM_ITEM* AddParaItem(DWORD key,bool glbalControlled=false)=0;
	virtual DESIGN_PARAM_ITEM* GetParamItem(DWORD key)=0;
	virtual CParaModelNode* AddParaNode(DWORD key,bool glbalControlled=false)=0;
	virtual CParaModelRod* AddParaRod(DWORD key,int idClsPartType=0)=0;
	virtual CParaModelParamPlate* AddParaPlate(DWORD key)=0;
	virtual CParaModelNode* GetParaNode(DWORD key)=0;
	virtual CParaModelRod* GetParaRod(DWORD key)=0;
	virtual CParaModelParamPlate* GetParaPlate(DWORD key)=0;
	virtual void LogError(char* error)=0;
};
class CParaBlockModel : public CBlockModel,public IParaBlockContext
{
public:
	CParaBlockModelTemplate *m_pTemplate;
	//bool m_bInternodeRepeat;	//�����ڼ��ظ��˼�
	long m_nKeyNode;
	long m_nInternodeNum;		//Ĭ�Ͻڼ���
	long m_nRepeatLessNum;		//�ظ����Ӹ˼�������Ƚڼ����Ĳ��
	GECS acs;				//װ������ϵ
public:
	NODESET keyNodeSet;		//װ�����ؽڵ�
	PARTSET keyPartSet;		//װ�����ع���
	CXhPtrList<ARRAY_LIST<double> >listArrayParamValues;
	CHashSet<ARRAY_LIST<double>*>hashArrayParamValues;
	CHashList<DESIGN_PARAM_ITEM>listParams;
	CXhSimpleList<KEY4C> listGlobalParams;	//ȫ��ģ�Ͳ��� wjh-2017.10.26
	CXhSimpleList<KEY4C> listControlNodes;	//��ȫ��ģ�Ͳ������ƵĽڵ�ģ���ʶ wjh-2017.10.26
	CParaBlockModel();
	virtual ~CParaBlockModel();
	void BuildAcs();
	void RebuildModel();
	void ResetPosition();
	bool MechanicsDesign(CParaBlockModel* pBlockModel);
	bool StructuralDesign(CParaBlockModel* pBlockModel);
	bool AssertParamsValid();//���ø�ģ����Ӧ֧�ֵĲ��������Ϸ���У��
	ARRAY_LIST<double>* AppendDoubleArray(DWORD key);
	void InitTemplateParams();	//��ʼ������ģ��Ĳ���
public:	//IParaBlockContext�Ľӿ�ʵ��
	virtual CLDSNode* GetKeyNode(DWORD index);
	virtual DESIGN_PARAM_ITEM* AddParaItem(DWORD key,bool glbalControlled=false);
	virtual DESIGN_PARAM_ITEM* GetParamItem(DWORD key);
	virtual CParaModelNode* AddParaNode(DWORD key,bool glbalControlled=false);
	virtual CParaModelRod* AddParaRod(DWORD key,int idClsPartType=0);
	virtual CParaModelParamPlate* AddParaPlate(DWORD key);
	virtual CParaModelNode* GetParaNode(DWORD key);
	virtual CParaModelRod* GetParaRod(DWORD key);
	virtual CParaModelParamPlate* GetParaPlate(DWORD key);
	virtual void LogError(char* error);
};
typedef CBlockModel* CBlockModelPtr;

struct BOUNDARY_NODE{
	long hBlockNode;	//װ�䲿�������ڶ�Ӧ�ļ����ڵ�/�˼�
	long hHostModelNode;//����ģ�ͻ����ڶ�Ӧ�Ľڵ�/�˼�
	void SetKey(DWORD key){hBlockNode=key;}
};
struct OUTofBOUNDARY_OBJ{
	long hBlockObj,hHostModelObj;
	void SetKey(DWORD key){hBlockObj=key;}
	OUTofBOUNDARY_OBJ(){hBlockObj=hHostModelObj=0;}
};
struct OUTofBOUNDARY_NODE : public OUTofBOUNDARY_OBJ{
	GEPOINT xBlkPosition;			//�ýڵ��ڲ���ģ���е����꣬���ڸ��������������������Զ�ʶ����ؽڵ�
public:	//��������
	PROPERTY(long,hBlockNode);		//װ�䲿�������ڶ�Ӧ�ļ����ڵ�
	GET(hBlockNode){return hBlockObj;}
	SET(hBlockNode){hBlockObj=value;}
	PROPERTY(long,hHostModelNode);	//����ģ�ͻ����ڶ�Ӧ�Ľڵ�
	GET(hHostModelNode){return hBlockObj;}
	SET(hHostModelNode){hBlockObj=value;}
};
struct OUTofBOUNDARY_ROD : public OUTofBOUNDARY_OBJ{
	GEPOINT xBlkStartPosition;		//�ø˼��ڲ���ģ���е�ʼ�����꣬���ڸ��������������������Զ�ʶ����ؽڵ�
	GEPOINT xBlkEndPosition;		//�ø˼��ڲ���ģ���е�ĩ�����꣬���ڸ��������������������Զ�ʶ����ؽڵ�
public:	//��������
	PROPERTY(long,hBlockRod);		//װ�䲿�������ڶ�Ӧ�ļ����˼�
	GET(hBlockRod){return hBlockObj;}
	SET(hBlockRod){hBlockObj=value;}
	PROPERTY(long,hHostModelRod);	//����ģ�ͻ����ڶ�Ӧ�ĸ˼�
	GET(hHostModelRod){return hBlockObj;}
	SET(hHostModelRod){hBlockObj=value;}
};
struct DYNA_RELA_OBJECT{
protected:
	char _name[64];
public:
	UINT id;
	UINT hBlkRef;				//����ӳ��Ĳ�������
	DYNA_RELA_OBJECT(){memset(_name,0,64);id=hBlkRef=0;}
	void SetKey(DWORD key){id=key;}
public:	//��������
	PROPERTY(BYTE,ciMappingDirection);	//0.����ģ���򲿼�Ӱ�䣻1.�Ӳ�������ģ��Ӱ��
	GET(ciMappingDirection){return _name[63];}
	SET(ciMappingDirection){_name[63]=value;}
	PROPERTY(CXhString,name);
	GET(name){return CXhString(_name,63);}
	SET(name){StrCopy(_name,value,63);}
};
struct DYNA_RELA_POINT : public DYNA_RELA_OBJECT{
	CLDSPoint m_xPoint;	//ciMappingDirection=0:����ģ�Ͷ���;ciMappingDirection=1:�ڲ����ж���
	GEPOINT GetBlockPoint();	//��ȡװ�䲿���ж�Ӧ�����
	GEPOINT GetModelPoint();	//��ȡ��ģ�������
};
struct DYNA_RELA_VECTOR : public DYNA_RELA_OBJECT{
	CLDSVector m_xVector;	//ciMappingDirection=0:����ģ�Ͷ���;ciMappingDirection=1:�ڲ����ж���
	GEPOINT GetBlockVector();	//��ȡװ�䲿���ж�Ӧ�����
	GEPOINT GetModelVector();	//��ȡ��ģ�������
};
class CBlockReference : public CLDSDbObject
{
	BOOL is_data_modified;
	UCS_STRU acs;		//װ��ʱ����װģ�����������µ�װ������ϵ����
	BOOL m_bExploded;	//�ѱ�ը��Ϊ��ɢ����
	CBlockModel* m_pInternalBlock;	//����Ƶ�����ݲ���������Ҳ���������ʱ�洢����
public:
	char descript[51];	//�������õ���������
	long m_hBlock;
	CParaCS des_acs;//װ������ϵ��Ʋ���
	CHashListEx<BOUNDARY_NODE>boundary;
	CHashListEx<OUTofBOUNDARY_NODE>hashExterRelaNodes;
	CHashListEx<OUTofBOUNDARY_ROD> hashExterRelaRods;
	CHashListEx<DYNA_RELA_POINT>  hashMappingPoints;
	CHashListEx<DYNA_RELA_VECTOR> hashMappingVectors;

	bool m_bDisplayLineFrame;
	CSolidBody *pSolidBody;
	//�ۺ�ʱ������������ɢ��˨��һ���Ǳ߽紦���������ӵ���˨�����ڴ�ף�
	CHashList<CLDSBolt*>hashReserveBolts;
public:
	CBlockReference();
	virtual ~CBlockReference();
	void UpdateAcs();	//����װ������ϵ
	void UpdatePos();	//����ԭ��λ��
	GEPOINT ShadowPositionBlkToModel(const double* blk_position);
	GEPOINT ShadowPositionModelToBlk(const double* model_position);
	GEPOINT ShadowVectorBlkToModel(const double* blk_vector);
	GEPOINT ShadowVectorModelToBlk(const double* model_vector);
	void SetModified(BOOL bTrue=TRUE){if(bTrue)is_undo_modified=TRUE;is_data_modified=bTrue;SetDocModifiedFlag();}
	BOOL IsModified(){return is_data_modified;}
	BOOL Create3dSolidModel(double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	CSolidPart GetSolidPartObject();
	UCS_STRU GetACS(){return acs;}
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	BOOL Exploded(BOOL bExploded);
	BOOL IntegrateToTowerModel();
	BOOL IsExploded(){return m_bExploded;}
	void UpdateBlockSegNoAndCfgWord();	//���ݲ������öκż���ĺŸ��²����ڹ����Ķκż���ĺ�
	//
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	//����
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	UPDATE_PROPLISTITEM_FUNC(CBlockReference,propHashtable)
	int GetPropValueStr(long id,char* valueStr);	//��������ID�õ�����ֵ�ַ���
	static void InitBlockReferencePropHashtable();
	static long GetPropID(char* propStr);			//�õ�����ID
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
};

class CBlockList : public ATOM_LIST<CBlockModel*>
{
	BOOL *m_pModified;
	IModel *m_pModel;
public:
	CBlockList(){m_pModel=NULL;m_pModified=NULL;}
	~CBlockList(){Empty();}
	void SetBelongModel(IModel *pModel){m_pModel = pModel;}
public:
	CBlockModel* append(BOOL bGetNewHandle=TRUE)//�ڽڵ������ĩβ��ӽڵ�
	{
		CBlockModel* pBlock=new CBlockModel();
		CBlockModel **ppBlock = ATOM_LIST<CBlockModel*>::append(pBlock);
		if(bGetNewHandle)	//�����Զ���þ��
			pBlock->handle = m_pModel->GetNewHandle();
		return *ppBlock;
	}
    CBlockModel* GetNext()
	{
		CBlockModel **ppBlock=ATOM_LIST<CBlockModel*>::GetNext();
		if(ppBlock)
			return *ppBlock;
		else
			return NULL;
	}
    CBlockModel* GetPrev()
	{
		CBlockModel **ppBlock=ATOM_LIST<CBlockModel*>::GetPrev();
		if(ppBlock)
			return *ppBlock;
		else
			return NULL;
	}
    CBlockModel* GetFirst()
	{
		CBlockModel **ppType=ATOM_LIST<CBlockModel*>::GetFirst();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
    CBlockModel* GetTail()
	{
		CBlockModel **ppType=ATOM_LIST<CBlockModel*>::GetTail();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
    CBlockModel* GetCursor()
	{
		CBlockModel **ppType=ATOM_LIST<CBlockModel*>::GetCursor();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
	CBlockModel* FromHandle(long handle)	//���ݾ����ýڵ�
	{
		CBlockModel* pType;
		BOOL bPush=push_stack();
		for(pType=GetFirst();pType!=NULL;pType=GetNext())
		{
			if(pType->handle == handle)
			{
				if(bPush)
					pop_stack();
				return pType;
			}
		}
		if(bPush)
			pop_stack();
		return NULL;
	}
	BOOL DeleteAt(long ii)
	{
		CBlockModel** ppBlock=GetByIndex(ii);
		if(ppBlock!=NULL&&*ppBlock!=NULL)
		{
			delete *ppBlock;
			return ATOM_LIST<CBlockModel*>::DeleteAt(ii);
		}
		else
			return FALSE;
	}
	BOOL DeleteNode(long handle)
	{
		int hits=0;
		for(CBlockModel* pType=GetFirst();pType;pType=GetNext())
		{
			if(!IsCursorDeleted()&&pType->handle== handle)
			{
				delete GetCursor();
				ATOM_LIST<CBlockModel*>::DeleteCursor();
				hits++;
			}
		}
		if(hits>0)
			return TRUE;
		else
			return FALSE;
	}
	void Empty(){
		for(CBlockModel* pBlock=GetFirst();pBlock;pBlock=GetNext())
			delete pBlock;
		ATOM_LIST<CBlockModel*>::Empty(); 
	}
};
