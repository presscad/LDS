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
	static const BYTE PROP_START_NODEPOS= 1;	//始端节点坐标
	static const BYTE PROP_START_TIPPOS = 2;	//始端端面顶点坐标(角钢楞点，钢管轴线端点）
	static const BYTE PROP_END_NODEPOS	= 3;	//终端节点坐标
	static const BYTE PROP_END_TIPPOS	= 4;	//终端端面顶点坐标(角钢楞点，钢管轴线端点）
	static const BYTE PROP_WING_NORM	= 5;	//角钢两肢法线
	BYTEFLAG cbValidProp;
public:
	long hRod;
	UINT idClsType;
	GEPOINT vxWingNorm,vyWingNorm;	//仅用于组合角钢子角钢的识别
	GEPOINT xStartPosition;		//该杆件在部件模型中的始端坐标，用于根据坐标在宿主环境中自动识别相关节点
	GEPOINT xEndPosition;		//该杆件在部件模型中的末端坐标，用于根据坐标在宿主环境中自动识别相关节点
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
	WORD d,wL;	//螺栓直径及有效长度
	GEPOINT vWorkNorm;		//螺栓穿插工作法线方向
	GEPOINT xBasePosition;	//装配面基点坐标
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
	UCS_STRU lcs;			//装配定位坐标系(Locate Coordinate System)
	BOOL m_bDataModified;	//数据修改标识
	BOOL m_bEmbeded;		//内嵌式部件块（子构件存在当前塔模型中）
public:	//装配时图块随动固接坐标系定义
	CParaCS des_lcs;//装配坐标系设计参数
	CHashList<CParaCS> Acs;			//以坐标系id为键值存储坐标系
	static const int COMMON_BLOCK = 0;
	static const int PARAMETRIC_BLOCK = 1;
public:
	THANDLE handle;					//部件句柄
	THANDLE m_hMinIdleHandle;
	char name[51];					//部件的名称描述
	DWORD dwPermission;			//权限字，用于标识谁对此象拥有修改权限
	int feature;	//定义用户自己的信息(临时备用,不可持久存储，也不一定都用)
	CHashListEx<CONSTRAINT>hashConstraints;
	CLdsList<CDisplayView> View;	//显示视图
	CLdsList<CLDSPoint> Point;			//基准点
	CLdsList<CLDSLine>  Line;			//基准线
	CLdsList<CLDSPlane> Plane;			//基准平面
    CLdsList<CLDSNode> Node;			//节点
	CLdsPartList Parts;					//各种型钢制成的铁塔构件，如角钢、钢板、钢管、槽钢、螺栓等
	//不属于当前部件，但在部件内的杆件节点的关联对象中涉及的杆件与节点，仅将简单信息导出，以便在宿主模型中进行自动匹配
	CHashListEx<EXTERNAL_BLKNODE> hashExterNodes;
	CHashListEx<EXTERNAL_BLKROD>  hashExterRods;
	CHashListEx<EXTERNAL_BLKBOLT> hashExterBolts;
	//CLdsList<CLDSArcLift> ArcLift;	//圆弧预拱项
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLdsList<CLDSPartGroup> PartGroup;
#endif
	long (*GetNewHandleHookFunc)();
	//还没处理好通过勾子函数添加数据对象后的查询问题，暂不启用以下两函数 wjh-2014.5.26
	//CLDSNode (*AppendNodeHookFunc)(BOOL bGetNewHandle);
	//CLDSPart (*AppendPartHookFunc)(int CLASS_TYPEID,BOOL bGetNewHandle);
public:
	CBlockModel(BOOL embeded=TRUE);
	virtual ~CBlockModel(void);
	BOOL IsEmbeded(){return m_bEmbeded;}
	void SetEmbeded(BOOL bEmbeded){m_bEmbeded=bEmbeded;}
	long GetMinValidObjHandle(bool bIncViewObj=false);//获得最小在用的对象句柄;
	virtual long GetNewHandle();
	virtual DWORD ShrinkObjHandle(DWORD hStartShrink=0x20);	//收缩句柄号
	virtual bool IsTowerModel(){return false;}
	//获取杆塔模型空间中直接管理并实际存在的构件数组，filterShadowPart=true表示只计入母本构件，影射构件将被怱略
	//pPolyPartList为CLdsList<CLDSPolyPart>*类型
	virtual bool GetRealityPartArr(DYN_ARRAY<CLDSBolt*>& arrBoltPtr,DYN_ARRAY<CLDSPart*>& arrPartPtr,
								   void *pPolyPartList,bool filterShadowPart=false);
	int RetrievePolyPartList(POLYPARTLIST &list);
	BOOL IsModified(){return m_bDataModified;}
	void UpdateLCS();	//更新定位坐标系
	void UpdatePos();	//更新原点位置
	//从与部件装配定位坐标系相对应的装配坐标系转换到与部件原始坐标相对应的装配坐标系
	UCS_STRU TransToACS(const UCS_STRU& cs);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer& buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	bool ExportToBlkFile(const char* blk_file,long version,IModel *pTowerModel=NULL,BOOL bSaveTaAsModel=FALSE);
	bool ImportFromBlkFile(char* blk_file);
	bool ImportFromXMLFile(const char* xml_file);
	BOOL SelectDisplaySet(NODESET& DispNodeSet,PARTSET& DispPartSet,CDisplayView *pView=NULL);
	UCS_STRU GetLCS(){return lcs;}
	void (*DisplayProcess)(int percent,const char *sTitle);	//进度显示回调函数
	BOOL CheckNodeIntegrality(IModel *pModel,NODESET& nodeSet);		//检查部件节点完整性
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
	{		return Parts.FromHandle(h,CLASS_TYPEID1,CLASS_TYPEID2);	}//根据句柄获得节点
    virtual CLDSPart* FromPartHandle(THANDLE h,int CLASS_TYPEID1,int CLASS_TYPEID2,int CLASS_TYPEID3)
	{		return Parts.FromHandle(h,CLASS_TYPEID1,CLASS_TYPEID2,CLASS_TYPEID3);	}//根据句柄获得节点
	virtual CLDSLinePart *FromRodHandle(long hRod);
	virtual CLDSArcLift *FromArcLiftHandle(long h){return NULL;}
public:	//约束相关的函数
	virtual CONSTRAINT* AppendConstraint(CONSTRAINT* pCopyFrom=NULL);
	virtual CONSTRAINT* GetConstraint(long constraint_id);
	virtual bool RemoveConstraint(long constraint_id,long hFromMasterObj=0);
	virtual CONSTRAINT* EnumConstraintFirst(BOOL bIterDelete=FALSE);
	virtual CONSTRAINT* EnumConstraintNext(BOOL bIterDelete=FALSE);
	virtual void ClearConstraintSyncStates();
public:	//遍历函数
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
public:	//增加数据对象
	virtual CParaCS *AppendAcs();	//增加装配坐标系
	virtual CDisplayView *AppendView(BOOL bGetNewHandle=TRUE){return View.append(bGetNewHandle);}
	virtual CLDSPoint *AppendPoint(BOOL bGetNewHandle=TRUE){return Point.append(bGetNewHandle);}
	virtual CLDSLine *AppendLine(BOOL bGetNewHandle=TRUE){return Line.append(bGetNewHandle);}
	virtual CLDSPlane *AppendPlane(BOOL bGetNewHandle=TRUE){return Plane.append(bGetNewHandle);}
	virtual CLDSNode *AppendNode(BOOL bGetNewHandle=TRUE){return Node.append(bGetNewHandle);}
	virtual CLDSPart *AppendPart(int CLASS_TYPEID,BOOL bGetNewHandle=TRUE){return Parts.append(CLASS_TYPEID,bGetNewHandle);}
public:	//删除数据对象
	virtual BOOL DeleteAcs(long hAcs);//删除装配坐标系
	virtual BOOL DeleteView(long h,BOOL bIncPerm=TRUE){return View.DeleteNode(h,bIncPerm);}
	virtual BOOL DeleteNode(long h,BOOL bIncPerm=TRUE){return Node.DeleteNode(h,bIncPerm);}
	virtual BOOL DeletePart(long h,BOOL bIncPerm=TRUE){return Parts.DeleteNode(h,bIncPerm);}
	//
	bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents){return true;}
	//属性
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CXhChar100 HELPLINK_URL;
	UPDATE_PROPLISTITEM_FUNC(CBlockModel,propHashtable)
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	int GetPropValueStr(long id,char* valueStr);	//根据属性ID得到属性值字符串
	static void InitBlockModelPropHashtable();
	static long GetPropID(char* propStr);			//得到属性ID
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
};
class CParaBlockModel;
/* 该类代码已移至lds_part.h中
#include "Expression.h"
union KEY4C{
private:
	char label[4];
public:
	DWORD dwItem;
	struct{
		WORD wItem;	//key中前半部，2字节一般作为循环参数的主标识
		WORD index;	//key中后半部，2字节一般作为循环参数的索引标识
	}pair;
public:
	operator DWORD(){return dwItem;}
	KEY4C(DWORD dwKey=0){dwItem=dwKey;}
	KEY4C(WORD wItem,WORD wIndex){pair.wItem=wItem;pair.index=wIndex;}
	KEY4C(char* keyStr);
	char* Label();//返回指针是临时字符串指针, 到下一次调用KEY4C::Label()前有效
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
	char sPreferLayer[3];		//由模板指定的优化图层名
	KEY4C keyLabel;				//节点的模板标识
	KEY4C keyLabelFatherRod;	//归属父杆件的模板标识
	char posx_expression[50];	//
	char posy_expression[50];
	char posz_expression[50];
	char scale_expression[50];
	char offset_expression[50];
	char exist_logicexpr[20];	//该节点对象是否存在的逻辑表达式，目前该表达式中只能出现一次'<'、'>'、'='、'<='、'>='、'!='逻辑操作符
	//bool m_bInternodeRepeat;	//节间中的重复节点，重复参数为"i"
	KEY4C keyRepeatParam;		//>0时表示代表该节点重复次数的CBlockParaModel::listParams参数键值，重复参数为"i"；其值为重复节点所归属的节点重复轴线的标识
	/*
		0-坐标无任何依赖的节点;
		1-杆件上X坐标值不变点;
		2-杆件上Y坐标值不变点;
		3-杆件上Z坐标值不变点;
		4-两杆件交叉交点;
		5-杆件上的比例等分点;
	*/
	short attach_type;	
	char ciOffsetType;	//0:长度;'X':X坐标分量;'Y':Y坐标分量;'Z':Z坐标分量
	double attach_scale;	//正值表示小数形式的比例位置值,负值表示分子.分母
	WORD wiOffsetByPrevNode;	//i>ciOffsetByPrevNode时:表示自前一偏移节点为基准进行偏移
	KEY4C attachNodeArr[4];
public:	//坐标计算类型，attach_type
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
	KEY4C keyLabel;	//标签（字符形式的标识可用于参数化建模时代替Id索引，有别于构件编号）
	char sPreferLayer[3];		//由模板指定的优化图层名
	long m_idPartClsType;	//该杆件的构件类型，如角钢、钢管、钢板等, CLS_LINEANGLE,CLS_LINETUBE
	char cMaterial;	//'S':Q235;'H':Q345;'G':Q390;'T':Q420;'U':Q460
	bool m_bInternodeRepeat;	//节间中的重复杆件，重复参数为"i"
	char exist_logicexpr[20];	//该节点对象是否存在的逻辑表达式，目前该表达式中只能出现一次'<'、'>'、'='、'<='、'>='、'!='逻辑操作符
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
	BYTE  nIterStep;	//重复杆件时节点索引的增长步长, 默认为1
	CXhChar16 indexParaStart,indexParaEnd;	//对于可重复节点的索引参数，如"i"、"i+1"、"i+N"等
	CXhChar16 m_sGuige;
	BYTE start_joint_type,end_joint_type;
	BYTE start_force_type,end_force_type;
	short callen_type;
	short layout_style;		//角钢的布置方式（里外铁，口朝上口朝下），取值0,1,2,3
	short m_iElemType;		//0表示自动判断;1表示二力杆桁架;2表示经典梁单元;3表示柔索单元
	union{
		COORD3D eyeSideNorm;	//对于角钢人眼观看侧的法线方向
		COORD3D pickWorkPlane;	//用于确定钢管基准边所在工作面的拾取点 wjh-2015.9.1
	};
	BOOL m_bMajorMat;		//主材
public:
	//JOINT_TYPE 类型的枚举值
	static const BYTE JOINT_HINGE	= 0;	//铰接
	static const BYTE JOINT_RIGID	= 1;	//刚接
	static const BYTE JOINT_AUTOJUSTIFY = 2;//自动判断
	//杆件两端的受力类型 FORCE_TYPE
	static const BYTE CENTRIC_FORCE		= 0;//中心受力
	static const BYTE ECCENTRIC_FORCE	= 1;//偏心受力
	//杆件的有限单元承力类型 ELEM_TYPE
	static const BYTE FEMELEM_TRUSS	= 1;//二力杆桁架单元
	static const BYTE FEMELEM_BEAM	= 2;//经典梁单元
	static const BYTE FEMELEM_GUYS	= 3;//柔索单元
};
class CParaModelAngle : public CParaModelRod{
public:
	struct POS{
	BYTE cTouchLine;			//0.X肢心线;1.Y肢心线;2.楞线
	BYTE cTouchLineType;		//0.心线;1.火曲线;2.偏移准线;3.控制点
	BYTE cFaceOffsetDatumLine;  //平推模式下的自身平推基准线
	GEPOINT face_offset_norm;	//平推面法线
	KEY4C labelHuoquRod;
	BYTE coordCtrlType;	//0:X-Z主控;1:Y-Z主控;2.X-Y主控(仅在spatialOperationStyle=15时有效)
	BYTE cMainCoordCtrlWing;//角钢主控肢：'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
	WING_OFFSET_STYLE XOrCtrlWing;	//角钢主要控制肢(控制两个坐标分量的肢)
	WING_OFFSET_STYLE YOrOtherWing;//角钢辅助控制肢(控制一个坐标分量的肢)
	}desStart,desEnd;
public:
	CParaModelAngle();
	virtual bool IsAngleObject(){return true;}
};
class CParaModelGeneralPlate : public CParaModelPart{
public:
	int m_iParamType;	//0.表示普通钢板;>0表示参数化板的类型
	CDesignLjPara designInfo;
};
class CParaModelParamPlate : public CParaModelPart{
protected:
	TAG_PLATE_INFO* pTagInfo;
public:
	static const int TYPE_D_TOPPLATE	=1;	//端撑顶板
	static const int TYPE_D_ENDPLATE	=2;	//端撑端板
	static const int TYPE_D_CHAPLATE	=3;	//端撑插板
	static const int TYPE_D_LINKPLATE	=4;	//端撑连板
	static const int TYPE_L_NODEPLATE	=5;	//立柱连板
	static const int TYPE_L_TOPPLATE	=6;	//立柱顶板
	static const int TYPE_L_JIAPLATE	=7;	//立柱夹板
	static const int TYPE_Q_TOPPLATE	=8;	//曲梁顶板
	static const int TYPE_Q_BASEPLATE	=9;	//曲梁基板
	static const int TYPE_Q_RIBPLATE	=10;//曲梁肋板
public:
	int m_iParamType;	//0.表示普通钢板;>0表示参数化板的类型
	CDesignLjPara designInfo;
	long _thick;
	BOOL _bStdPart;	//TRUE:标准构件 FALSE:非标准构件
	KEY4C keyDatumNode;
	KEY4C keyDatumPart,keyAssistPart,keyAssistPart2;
	ATOM_LIST<ATTACHPART_ITEM> AttachPart;
	CLDSVector heteroFlConnVec[3];	//变坡处异型法兰的三根杆件连接方向
	ATOM_LIST<CONNECT_DIRECTION> ConnDirectList;
	ATOM_LIST<DESIGN_PLATE_ITEM>designItemList;
	CLDSVector workNorm;	//工作法线的设计参数
	char off_expression[50];	
	int feature;			//临时使用
	//
	CParaModelParamPlate();
	~CParaModelParamPlate();
	void SetTagInfo(void* pTag){pTagInfo=(TAG_PLATE_INFO*)pTag;}
	TAG_PLATE_INFO* TagInfo(){return pTagInfo;}
};
class CParaBlockModelTemplate
{
protected:
	ATOM_LIST<TAG_NODE_INFO>listTagNodeInfo;		//临时挂接的附加到构件项{PART_ITEM}上的数据对象链表
	ATOM_LIST<TAG_ROD_INFO>listTagRodInfo;			//临时挂接的附加到构件项{PART_ITEM}上的数据对象链表
	ATOM_LIST<TAG_PLATE_INFO>listTagPlateInfo;		//临时挂接的附加到构件项{PART_ITEM}上的数据对象链表
	CHashList<CParaModelNode>listRepeatedParaNodes;	//临时存储重复生成的序列节点
	CSuperHashList<CParaModelRod>listRepeatedParaRods;	//临时存储重复生成的序列杆件
public:
	static const int SUBSTATION_TEMPLATE	= 0;		//变电构架模版
	static const int CROSSARM_TEMPLATE		= 1;		//典型横担模版
	//static const int TYPE_PLATE			= 5;		//CLS_PLATE
	//static const int TYPE_PARAMPLATE		= 6;		//CLS_PARAMPLATE
	//static const int TYPE_LINEANGLE		=13;		//CLS_LINEANGLE
	//static const int TYPE_LINETUBE		=17;		//CLS_LINETUBE
public:
	int m_iTempType;	//模版类型：0:变电构架模板  1：典型横担模版 
	/*
	变电构架模版
	0.三角梁构架   1.四方梁构架
	典型横担模版的不同样式序号：
	0.下三角形横担 1.上三角横担
	*/
	int m_iTempIndex;	//同一模版类型的不同模版序号
	CHashList<DESIGN_PARAM_ITEM>listParams;
	CHashList<CParaModelNode>listParaNodes;
	CSuperHashList<CParaModelRod>listParaRods;
	CHashList<CParaModelParamPlate> listParaPlates;

	//virtual DESIGN_PARAM_ITEM* AddParaItem(DWORD key,bool glbalControlled=false);
	//virtual CParaModelNode* AddParaNode(DWORD key,bool glbalControlled=false);
	//virtual CParaModelRod* AddParaRod(DWORD key,int idClsPartType=0);
	//virtual CParaModelParamPlate* AddParaPlate(DWORD key);
	//virtual DESIGN_PARAM_ITEM* GetParamItem(DWORD key);
	virtual CParaModelNode* GetParaNode(DWORD key);	//从listParaNodes及listRepeatedParaNodes中查找参数化节点模板
	virtual CParaModelRod* GetParaRod(DWORD key);	//从listParaRods及listRepeatedParaRods中查找参数化节点模板
	virtual CParaModelParamPlate* GetParaPlate(DWORD key);
	//virtual bool LoadTemplate(UINT uidTemplType);
	//virtual void LogError(char* error);
public:
	CParaBlockModelTemplate();
	virtual ~CParaBlockModelTemplate();
	//当CParaModelNode已经有挂接对象时执行TagInfo(...), 否则创建新挂接数据对象后返回
	TAG_NODE_INFO* LoadTagInfo(CParaModelNode* pItem);
	TAG_ROD_INFO*  LoadTagInfo(CParaModelRod*  pItem);
	TAG_PLATE_INFO* LoadTagInfo(CParaModelParamPlate* pItem);
	CLDSNode* InsertNewNode(CParaBlockModel* pBlockModel,CParaModelNode* pParaNode,f3dPoint pos);
	CLDSLinePart* InsertNewPole(CParaBlockModel* pBlockModel,CParaModelRod* pParaRod);
	CLDSParamPlate* InsertNewPlate(CParaBlockModel* pBlockModel,CParaModelParamPlate* pParaPlate,CExpression& expression);
	void FillMirInfo(CParaBlockModel* pBlockModel);			//匹配节点及杆件的对称信息
	virtual void RebuildModel(CParaBlockModel* pBlockModel);		//创建几何模型, 已实现
	//virtual void RebuildModelEx(CParaBlockModel* pBlockModel);		//创建几何模型, 已实现
	void ResetPosition(CParaBlockModel* pBlockModel);
	bool MechanicsDesign(CParaBlockModel* pBlockModel);
	bool StructuralDesign(CParaBlockModel* pBlockModel);
};
/* 该类代码已移至IModel.h中
struct DESIGN_PARAM_ITEM
{
	KEY4C key;			//该参数的标识键值（或不超过4个长的字符串）
	CXhChar50 sLabel;	//属性栏中显示的短名称标签
	CXhChar100 sNotes;	//界面中对该项属性的解释性描述
	union{double fValue;THANDLE hPart;long iType;}value;
	BYTE dataType;	//0.double数值 1.句柄 2. 整数
	BOOL bModify;
	DESIGN_PARAM_ITEM(){dataType=0; bModify=FALSE;}
	EXPRESSION_VAR ToExprVar();
public:	//dataType的数据类型
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
	//bool m_bInternodeRepeat;	//允许多节间重复杆件
	long m_nKeyNode;
	long m_nInternodeNum;		//默认节间数
	long m_nRepeatLessNum;		//重复连接杆件次数相比节间数的差额
	GECS acs;				//装配坐标系
public:
	NODESET keyNodeSet;		//装配主控节点
	PARTSET keyPartSet;		//装配主控构件
	CXhPtrList<ARRAY_LIST<double> >listArrayParamValues;
	CHashSet<ARRAY_LIST<double>*>hashArrayParamValues;
	CHashList<DESIGN_PARAM_ITEM>listParams;
	CXhSimpleList<KEY4C> listGlobalParams;	//全局模型参数 wjh-2017.10.26
	CXhSimpleList<KEY4C> listControlNodes;	//由全局模型参数控制的节点模板标识 wjh-2017.10.26
	CParaBlockModel();
	virtual ~CParaBlockModel();
	void BuildAcs();
	void RebuildModel();
	void ResetPosition();
	bool MechanicsDesign(CParaBlockModel* pBlockModel);
	bool StructuralDesign(CParaBlockModel* pBlockModel);
	bool AssertParamsValid();//调用各模板中应支持的部件参数合法性校验
	ARRAY_LIST<double>* AppendDoubleArray(DWORD key);
	void InitTemplateParams();	//初始化各类模版的参数
public:	//IParaBlockContext的接口实现
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
	long hBlockNode;	//装配部件环境内对应的寄生节点/杆件
	long hHostModelNode;//寄主模型环境内对应的节点/杆件
	void SetKey(DWORD key){hBlockNode=key;}
};
struct OUTofBOUNDARY_OBJ{
	long hBlockObj,hHostModelObj;
	void SetKey(DWORD key){hBlockObj=key;}
	OUTofBOUNDARY_OBJ(){hBlockObj=hHostModelObj=0;}
};
struct OUTofBOUNDARY_NODE : public OUTofBOUNDARY_OBJ{
	GEPOINT xBlkPosition;			//该节点在部件模型中的坐标，用于根据坐标在宿主环境中自动识别相关节点
public:	//派生属性
	PROPERTY(long,hBlockNode);		//装配部件环境内对应的寄生节点
	GET(hBlockNode){return hBlockObj;}
	SET(hBlockNode){hBlockObj=value;}
	PROPERTY(long,hHostModelNode);	//寄主模型环境内对应的节点
	GET(hHostModelNode){return hBlockObj;}
	SET(hHostModelNode){hBlockObj=value;}
};
struct OUTofBOUNDARY_ROD : public OUTofBOUNDARY_OBJ{
	GEPOINT xBlkStartPosition;		//该杆件在部件模型中的始端坐标，用于根据坐标在宿主环境中自动识别相关节点
	GEPOINT xBlkEndPosition;		//该杆件在部件模型中的末端坐标，用于根据坐标在宿主环境中自动识别相关节点
public:	//派生属性
	PROPERTY(long,hBlockRod);		//装配部件环境内对应的寄生杆件
	GET(hBlockRod){return hBlockObj;}
	SET(hBlockRod){hBlockObj=value;}
	PROPERTY(long,hHostModelRod);	//寄主模型环境内对应的杆件
	GET(hHostModelRod){return hBlockObj;}
	SET(hHostModelRod){hBlockObj=value;}
};
struct DYNA_RELA_OBJECT{
protected:
	char _name[64];
public:
	UINT id;
	UINT hBlkRef;				//关联映射的部件引用
	DYNA_RELA_OBJECT(){memset(_name,0,64);id=hBlkRef=0;}
	void SetKey(DWORD key){id=key;}
public:	//派生属性
	PROPERTY(BYTE,ciMappingDirection);	//0.从主模型向部件影射；1.从部件向主模型影射
	GET(ciMappingDirection){return _name[63];}
	SET(ciMappingDirection){_name[63]=value;}
	PROPERTY(CXhString,name);
	GET(name){return CXhString(_name,63);}
	SET(name){StrCopy(_name,value,63);}
};
struct DYNA_RELA_POINT : public DYNA_RELA_OBJECT{
	CLDSPoint m_xPoint;	//ciMappingDirection=0:在主模型定义;ciMappingDirection=1:在部件中定义
	GEPOINT GetBlockPoint();	//获取装配部件中对应坐标点
	GEPOINT GetModelPoint();	//获取主模型坐标点
};
struct DYNA_RELA_VECTOR : public DYNA_RELA_OBJECT{
	CLDSVector m_xVector;	//ciMappingDirection=0:在主模型定义;ciMappingDirection=1:在部件中定义
	GEPOINT GetBlockVector();	//获取装配部件中对应坐标点
	GEPOINT GetModelVector();	//获取主模型坐标点
};
class CBlockReference : public CLDSDbObject
{
	BOOL is_data_modified;
	UCS_STRU acs;		//装配时在总装模型整体坐标下的装配坐标系定义
	BOOL m_bExploded;	//已被炸碎为零散构件
	CBlockModel* m_pInternalBlock;	//避免频烦根据部件句柄查找部件对象，临时存储对象
public:
	char descript[51];	//部件引用的名称描述
	long m_hBlock;
	CParaCS des_acs;//装配坐标系设计参数
	CHashListEx<BOUNDARY_NODE>boundary;
	CHashListEx<OUTofBOUNDARY_NODE>hashExterRelaNodes;
	CHashListEx<OUTofBOUNDARY_ROD> hashExterRelaRods;
	CHashListEx<DYNA_RELA_POINT>  hashMappingPoints;
	CHashListEx<DYNA_RELA_VECTOR> hashMappingVectors;

	bool m_bDisplayLineFrame;
	CSolidBody *pSolidBody;
	//聚合时保留下来的零散螺栓（一般是边界处与主体连接的螺栓，用于打孔）
	CHashList<CLDSBolt*>hashReserveBolts;
public:
	CBlockReference();
	virtual ~CBlockReference();
	void UpdateAcs();	//更新装配坐标系
	void UpdatePos();	//更新原点位置
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
	void UpdateBlockSegNoAndCfgWord();	//根据部件引用段号及配材号更新部件内构件的段号及配材号
	//
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	//属性
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	UPDATE_PROPLISTITEM_FUNC(CBlockReference,propHashtable)
	int GetPropValueStr(long id,char* valueStr);	//根据属性ID得到属性值字符串
	static void InitBlockReferencePropHashtable();
	static long GetPropID(char* propStr);			//得到属性ID
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
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
	CBlockModel* append(BOOL bGetNewHandle=TRUE)//在节点链表的末尾添加节点
	{
		CBlockModel* pBlock=new CBlockModel();
		CBlockModel **ppBlock = ATOM_LIST<CBlockModel*>::append(pBlock);
		if(bGetNewHandle)	//不用自动获得句柄
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
	CBlockModel* FromHandle(long handle)	//根据句柄获得节点
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
