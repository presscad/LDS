//<LOCALE_TRANSLATE BY hxr /> 2014-12-8
#ifndef __LDS_PART_H_
#define __LDS_PART_H_

#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "PartLib.h"
#include "XhLdsLm.h"
#include "HashTable.h"
#include "Hash64Table.h"
#include "LogFile.h"
#include "SmartPtr.h"
#include "Buffer.h"
#include "macro.h"
#include "ldsptr_list.h"
#include "IModel.h"
#include "IPartLibrary.h"
#include "ComplexId.h"
#include "LifeObj.h"
#ifdef __PART_DESIGN_INC_
#include "WirePlaceCode.h"
#include "Resolver.h"
#endif
#include "XhCharString.h"
#include "SolidBody.h"
#include "MemberProperty.h"
#include "SegI.h"
#include "PropListItem.h"
#ifdef __LDS_CONTEXT_
#include "..\BOM\BOM.h"
#endif
#include "SortFunc.h"
//#include "ConnectPointInfo.h"

/*
			版权所有2001-2007 北京信狐天诚软件科技有限公司

文件描述:
	LDS中描述塔构件几何信息的类
*/
#define SELECTINDEX_NODE		1
#define SELECTINDEX_LINE		2	//0x02
#define SELECTINDEX_CIRCLE		3	//0x04
#define SELECTINDEX_LINEANGLE	4	//0x08
#define SELECTINDEX_LINETUBE	5	//0x10
#define SELECTINDEX_LINEFLAT	6	//0x20
#define SELECTINDEX_LINESLOT	7	//0x40
#define SELECTINDEX_PLATE		8
#define SELECTINDEX_PARAMPLATE	9
#define SELECTINDEX_BOLT		10
#define SELECTINDEX_SPHERE		11
#define SELECTINDEX_ARCANGLE	12
#define SELECTINDEX_ARCSLOT		13
#define SELECTINDEX_ARCFLAT		14
#define SELECTINDEX_ARCTUBE		15
#define SELECTINDEX_STDPART		16
#define SELECTINDEX_GROUPLINEANGLE 17
#define SELECTINDEX_BLOCKREF	18
#define SELECTINDEX_HOLEWALL	19
#define SELECTINDEX_FITTINGS	20
#define SELECT_LINEPART			0x00000078
#define SELECT_ARCPART			0x00007800
#define	SELECT_PART				0x0001FFF8
#define SELECT_ALL				0xFFFFFFFF

#ifndef X_RESTRICT
	#define	X_RESTRICT		0x0001		//X方向约束
#endif
#ifndef Y_RESTRICT
	#define	Y_RESTRICT		0x0002		//Y方向约束
#endif
#ifndef	Z_RESTRICT
	#define	Z_RESTRICT		0x0004		//Z方向约束
#endif
#ifndef X_ROT_RESTRICT
	#define	X_ROT_RESTRICT	0x0008		//X轴转动约束
#endif
#ifndef Y_ROT_RESTRICT
	#define	Y_ROT_RESTRICT	0x0010		//Y轴转动约束
#endif
#ifndef	Z_ROT_RESTRICT
	#define	Z_ROT_RESTRICT	0x0020		//Z轴转动约束
#endif
#ifndef ALL_RESTRICT
	#define	ALL_RESTRICT	0x003F		//各方向约束
#endif

#define MAX_CHAR_GUIGE_16		16
#define MAX_TEMP_CHAR_50		50
#define MAX_TEMP_CHAR_100		100
#define MAX_TEMP_CHAR_200		200
#define MAX_TEMP_CHAR_500		500

//构件号检查类型 wht 12-04-25
#define CHKPARTNO_NULLPARTNO			0x0001	//空编号构件
#define CHKPARTNO_PARTTYPE_DIFFER		0x0002	//构件类型不同
#define CHKPARTNO_RODINFO				0x0004	//同编号杆件长度及正负头、规格、材质等基本信息
#define CHKPARTNO_RODBOLTINFO			0x0008	//同编号杆件上螺栓数量、规格及杆件上螺栓位置信息（含颠倒对比）
#define CHKPARTNO_ANGLECUT				0x0010	//角钢上切角信息（含颠倒对比）
#define CHK_PLATE_BASICINFO				0x0020	//同编号钢板厚度、材质、火曲面数等基本信息
#define CHK_PLATE_PROFILEANDBOLTINFO	0x0040	//钢板螺栓数数量及规格及相对孔位、卷边及外形，颠倒互换性,
//校核对称杆件时的子类型
#define CHK_MIRROD_BASICINFO			0x0080	//对称杆件规格、材质、基准长度等基本信息
#define CHK_MIRROD_LENGTHINFO			0x0100	//对称杆件的实际长度信息

//////////////////////////////////////////////////////////////////////////////

enum LDSPART_TYPE{
	TA_NODE				= 0,		// 节点
	STEELTYPE_ANGLE		= 1,		// 角钢
	STEELTYPE_BOLT		= 2,		// 螺栓
	STEELTYPE_PLATE		= 3,		// 钢板
	STEELTYPE_TUBE		= 4,		// 钢管
	STEELTYPE_FLAT		= 5,		// 扁铁
	STEELTYPE_SLOT		= 6,		// 槽钢
	TA_OTHER	= 100,
};
enum LDSCLASS_TYPEID{
	CLS_OBJECT = 1,
	CLS_DBOBJECT,
	CLS_NODE,
	CLS_PART,
	CLS_PLATE,
	CLS_PARAMPLATE,
	CLS_BOLT,
	CLS_LINEPART,
	CLS_GEOBJECT,
	CLS_GEPOINT,
	CLS_GELINE,
	CLS_GEPLANE,
	CLS_LINEANGLE,
	CLS_POLYJOINT,
	CLS_GROUPLINEANGLE,
	CLS_GROUPPOLYANGLE,
	CLS_LINETUBE,
	CLS_LINEFLAT,
	CLS_POLYFLAT,
	CLS_LINESLOT,
	CLS_POLYPART,
	CLS_FOOT,
	CLS_JOINT,
	CLS_TUBEJOINT,
	CLS_WELDLINE,
	CLS_DISPLAYVIEW,
	CLS_WELDPART,
	CLS_SPHERE,
	CLS_ARCPART,
	CLS_ARCANGLE,
	CLS_ARCSLOT,
	CLS_ARCFLAT,
	CLS_ARCTUBE,	//2009年10月10日添加，如果用以前版本的LMA做标准件，文件格式可能不对
	CLS_STDPARTMODEL,
	CLS_CONETUBE,
	CLS_CONETUBEFL,
	CLS_CABLE,		//线缆
	CLS_FEMELEMENT,	//有限单元
	CLS_ARCLIFT,	//圆弧预拱
	CLS_SLOTARM,    //槽型横担
	CLS_BLOCK,		//部件
	CLS_BLOCKREF,	//部件引用
	CLS_PROFILEVERTEX,//外形顶点
	CLS_DELEGATEROD,
	CLS_EXTERNALPART,	//外部构件
	CLS_FITTINGS,		//金具
	CLS_INSULATOR,		//绝缘子
	CLS_BOLTU,			//U型螺栓
	CLS_WORKCASE,		//荷载工况组合
	CLS_SUPPLEMENTPART,	//手动录入附加构件
	CLS_LINEHSHAPE,
	CLS_ANCHORBOLT,		//地脚螺栓
	CLS_FOUNDATION,		//基础
	CLS_HOLEWALL,		//构件上的孔壁实体
	CLS_WIREARROW,		//挂点方向示意箭头

	CLS_HEIGHTMODULE=100,
};
//辅材验算依据，AUX_AUTODESIGN:全自动设计;AUX_BY_START:依据始端支撑杆件;AUX_BY_END:依据终端支撑杆件;
//AUX_ONLY_PRIMARY:仅考虑对斜材的支撑力验算　wjh-2017.2.6
enum AUX_POLE_SEL_METHOD{AUX_AUTODESIGN=0,AUX_BY_START=1,AUX_BY_END=2,AUX_ONLY_PRIMARY=3};

#ifndef __CLS_CSOLIDPART_DEFINED_
#define __CLS_CSOLIDPART_DEFINED_
#include "HiberarchyId.h"
class CSolidPart{
public:
	HIBERID hiberid;
	long get_m_hPart(){return (long)hiberid.masterId;}
	long set_m_hPart(long hPartId){
		hiberid.masterId=(DWORD)hPartId;
		return hPartId;
	}
	// 构件的句柄
	__declspec(property(put=set_m_hPart,get=get_m_hPart)) long m_hPart;
	int  m_idClassType;	// 构件的类型
	GECS acs;			// 构件的装配坐标系
	BOOL m_bDisplaySingle;	//线型构件是否需要在单线图中显示
	BOOL m_bSelected;		//构件是否当前被选中(此属性只由程序控制)
	int m_iLineType;		//0:构件为实体类型；1:构件为直线型构件；2:构件为弧线型构件; 3:箭头
	f3dLine line;		// 单线状态下的简化线及	//箭头长度
	f3dArcLine arc;			// 单线状态下的弧线简化线
	double dfSphereRadius;
	CSolidBody *pBody;		// 显示构件的实体显示数据
public:
	CSolidPart(){
		m_iLineType=1;
		m_bDisplaySingle=TRUE;
		m_bSelected=FALSE;pBody=NULL;
		m_hPart = m_idClassType=0;
		dfSphereRadius=0;
	}
};
#endif
enum TREEITEM_TYPE{
	NONE_INFO,
	//视图
	VIEW_GROUP,
	VIEW_LAYERSET,
	VIEW_DEFINEDSET,
	VIEW_DEFINEDFOLDER,
	VIEW_CASE,
	//部件
	BLOCK_REF,
	BLOCK_GROUP,
	BLOCKREF_GROUP,
	BLOCK_CASE,
	BLOCKREF_CASE,
	BLOCK_VIEW_CASE,
	BLOCKREF_VIEW_CASE,
	//计算模型组
	DATA_SPACE,
	MODEL_GROUP,
	MODEL_LANDSCAPE,
	MODEL_SAFEGRADE,
	MODEL_VOLTAGE,
	MODEL_ICE_DENSITY,
	MODEL_CASE,
	MODEL_LAND_HIGH,
	MODEL_QUAD1LEG_CFGNO,
	MODEL_QUAD2LEG_CFGNO,
	MODEL_QUAD3LEG_CFGNO,
	MODEL_QUAD4LEG_CFGNO,
	MODEL_BODY_LEGNO,
	MODEL_BURGLAR_LS_SCOPE,
	MODEL_DATUM_POINT_GROUP,
	MODEL_DATUM_POINT_CASE,
	MODEL_DATUM_LINE_GROUP,
	MODEL_DATUM_LINE_CASE,
	MODEL_DATUM_PLANE_GROUP,
	MODEL_DATUM_PLANE_CASE,
	MODEL_ARCLIFT_GROUP,
	MODEL_ARCLIFT_CASE,
	MODEL_RESTRICT,
	MODEL_RESTRICT_NODE,
	//架线方案
	WIRELAYOUT_GROUP,
	WIRELAYOUT_CASE,
	//线条荷载计算模型
	WIRELOAD_MODEL_GROUP,
	WIRELOAD_MODEL_CASE,
	//计算工况
	WORKSTATUS_GROUP,
	WORKSTATUS_CASE,
	WORKSTATUS_SCOPE,
	WORKSTATUS_ICE_THICK,
	WORKSTATUS_POINTLOAD,
	WORKSTATUS_POINTLOAD_CASE,
	WORKSTATUS_WEIGHTLOAD,
	WORKSTATUS_WINDLOAD,
	WORKSTATUS_EARTHQUAKE,
	//附加风荷载
	WORKSTATUS_ATTACH_WINDLOAD,
	WORKSTATUS_ATTACH_WINDLOADITEM,
	WORKSTATUS_ATTACH_WINDLOADREF,
	WORKSTATUS_ATTACH_WINDLOADREFITEM,
	//风压段
	WINDSEG_GROUP,
	WINDSEG_CASE,
	WINDSEG_MIRMSG,
	WINDSEG_TYPENO,
	WINDSEG_STYLE,
	WINDSEG_FRONTCOEF,
	WINDSEG_SIDECOEF,
	//材质
	TREE_ITEM_MAT,
	MATERIAL_GROUP,
	MATERIAL_RECORD,
	MATERIAL_E_MODULUS,
	MATERIAL_F_INTENSITY,	//抗拉压弯强度设计值
	MATERIAL_FC_INTENSITY,	//孔臂承压强度设计值
	BOLT_GROUP,
	BOLT_TYPE_RECORD,
	//构件类型
	TREE_ITEM_PARTTYPE,
	TYPE_ANGLE_IRON,
	TYPE_COMPOSITE_ANGLE_IRON,
	TYPE_STEEL_TUBE,
	TYPE_SLOT_IRON,
	TYPE_FLAT_IRON,
	TYPE_STEEL_GUY,
	//优化选材
	FL_OPTIMIZOR_GROUP,
	//装配坐标系
	PARAMETRIC_CS_GROUP,
};

struct ANGLEREPLACER_TAG{
	struct ANGLE_STATE{
		bool replaceStartDependRod;
		bool replaceEndDependRod;
		ANGLE_STATE(){
			replaceStartDependRod=replaceEndDependRod=false;
		}
	};
	CHashList<ANGLE_STATE> hashAngles;
};
struct HANDLEEX{
	long handle;
	long set_hPoint(long _hPoint);
	long get_hPoint();
	__declspec(property(put=set_hPoint,get=get_hPoint)) long hNodePoint;
	bool set_bSimpleNodePoint(bool _bSimpleNodePoint);
	bool get_bSimpleNodePoint();
	__declspec(property(put=set_bSimpleNodePoint,get=get_bSimpleNodePoint)) bool bSimpleNodePoint;
	operator long(){return handle;}
	long operator =(long _hPoint){
		handle=_hPoint&0x7fffffff;
		return handle;
	}
	HANDLEEX(long _hPoint=0,bool isSimpleNodePoint=false){
		handle=_hPoint&0x7fffffff;
		if(isSimpleNodePoint)
			handle|=0x80000000;
	}
};
struct HANDLEPOINT{
//protected:
	char ciHidType;	//handle含义0.CLDSPoint.handle;'1':CLDSNode.handle;'S':CLDSLinePart.handle->PosStart;'E':CLDSLinePart.handle->PosEnd
public:
	long handle;
	bool set_bSimpleNodePoint(bool _bSimpleNodePoint);
	bool get_bSimpleNodePoint();
	__declspec(property(put=set_bSimpleNodePoint,get=get_bSimpleNodePoint)) bool bSimpleNodePoint;
	bool set_bRodTipStart(bool _bRodTipPoint);
	bool get_bRodTipStart();
	__declspec(property(put=set_bRodTipStart,get=get_bRodTipStart)) bool bRodTipStart;
	bool set_bRodTipEnd(bool _bRodTipPoint);
	bool get_bRodTipEnd();
	__declspec(property(put=set_bRodTipEnd,get=get_bRodTipEnd)) bool bRodTipEnd;
	operator long(){return handle;}
	long operator =(long _hPoint){
		handle=_hPoint&0x7fffffff;
		return handle;
	}
	HANDLEPOINT(long _hPoint=0,char ciHidType=0);
};
struct HANDLE_ADDR {
	BYTE ciAddrType;		//0.表示指向句柄值变量的地址指针;1.表示指向构件句柄值及厚度常量的字符串变量地址指针
	WORD wnMaxStrBuffLen;	//ciAddrType=1时，pszThickPartStr指向的字符串缓存长度
	union{
		long* pHandlVarAddr;	//存储句柄值的内存属性变量
		char* pszThickPartStr;	//存储构件句柄值及厚度常量的字符串
	};
	HANDLE_ADDR(long* pVarAddr = NULL) {
		wnMaxStrBuffLen = ciAddrType = 0;
		pHandlVarAddr = pVarAddr;
	}
	HANDLE_ADDR(char* pStrVarAddr,WORD wnStrBuffLen) {
		ciAddrType = 1;
		wnMaxStrBuffLen = wnStrBuffLen;
		pszThickPartStr = pStrVarAddr;
	}
	HANDLE_ADDR(void* pVarAddr, BYTE ciVarAddrType,WORD wnStrBuffLen=0) {
		ciAddrType = ciVarAddrType;
		wnMaxStrBuffLen = wnStrBuffLen;
		pHandlVarAddr = (long*)pVarAddr;
	}
	long get_HandleId() const;
	long set_HandleId(long hHandlValue);
	__declspec(property(put = set_HandleId, get = get_HandleId)) long HandleId;
};
class CHandleReplacer{
	struct SECTION{
		GEPOINT lineFilterStart,lineFilterEnd,lineStdVec;	//进行线段区间过滤
		double sect_length;
		long hSectRod;
		CLDSLinePart* pSectRod;
		long priorSerial;	//优先度(值越大优先度越高)，用于判断节点为两线段共同端点情况下判断节点的归属区间
		SECTION();
	};
	bool m_bReplaceObjSelfHandle;	//true：同时替换对象自身的标识句柄，false：只替换数据中的关联对象句柄 wjh-2015.5.19
	bool m_bExcludeObjPointer;		//true:替换句柄时排除直接对数据对象的指针引用，如杆件的端节点
	bool m_bVerifyGroupAngleReplaced;	//默认为true，当确认当前替换对象非组合角钢时，跳过相关子角钢替换代码以提升效率 wjh-2016.11.10
	ATOM_LIST<SECTION> sections;
	long _hOld,_hOldSonAngles[4];
	bool m_bCollectiveTransition;
	bool m_bReverseStartAndEnd;
	bool m_bReverseWingXAndWingY;
public:
	long hNew;
	CLDSLinePart *pOldRod, *pNewRod;
	ANGLEREPLACER_TAG *pTag;
public:
	PROPERTY(BOOL,hOld);
	GET(hOld){return _hOld;}
	bool IsReplaceObjSelfHandle(){return m_bReplaceObjSelfHandle;}
	void SetReplaceObjSelfState(bool bReplaceSelf=true){m_bReplaceObjSelfHandle=bReplaceSelf;}
	bool IsExcludeReplaceObjPointer(){return m_bExcludeObjPointer;}
	void SetExcludeReplaceObjPointer(bool bExludeObjPointer=true){m_bExcludeObjPointer=bExludeObjPointer;}
	CHandleReplacer(long handleOld,long handleNew,f3dLine* line_filter=NULL,bool bCollectiveTransition=false,
		bool bReverseSAndE=false,bool bReverseXAndY=false);
	bool IsCollectiveTransition(){return m_bCollectiveTransition;}
	bool IsReverseStartAndEnd(){return m_bReverseStartAndEnd;}
	bool IsReverseWingXAndWingY(){return m_bReverseWingXAndWingY;}
	bool VerifyAndReplace(HANDLEEX* hCurrent,bool bObjSelfHandle=false);
	bool VerifyAndReplace(long* hCurrent,bool bObjSelfHandle=false);
	bool AppendSectRod(const double* lineStartCoord,const double* lineEndCoord,long hSectRod,long priorSerial=0);
	//不区分点不在区段沿线外情况，仅按在沿线上投影点判断，如要求在区段沿线上，则应在AppendSectRod时保证　wjh-2014.6.25
	long SelectPerfectPartHandle(const double* pos_coord);
};
//CMaxDouble及CMinDouble转移到XhMath.h中定义 wjh-2019.2.21
#include "XhMath.h"
//CStdVector
//0.自定义 1.X+ 2.X- 3.Y+ 4.Y- 5.Z+ 6.Z- 
class CStdVector
{
public:
	static f3dPoint GetVector(int nVecType);		//根据方向类型得到标准方向
	static int GetVectorType(const double* vec_coord);			//根据方向得到标准方向类型
	static CString MakeVectorSetStr(bool bHasEditPara=false);				//标准方向字符串
	static CString GetVecTypeString(int nVecType);	//得到输入类型对应的字符串
	static CString GetVecTypeString(const double* vec_coord);	//得到输入方向对应的字符串
};

class CLDSObject;
class CLDSDbObject;
class CLDSNode;
class CLDSPart;
class CLDSLinePart;
class CBlockModel;
class CBlockReference;
struct SUB_OBJ;
typedef CTmaPtrList<SUB_OBJ,SUB_OBJ*> SUBOBJSET;
typedef CTmaPtrList<CLDSObject,CLDSObject*> OBJSET;
typedef CHashSet<CLDSObject*> HASHOBJSET;
typedef CTmaPtrList<CLDSDbObject,CLDSDbObject*> DBOBJSET;
typedef CTmaPtrList<CLDSNode,CLDSNode*> NODESET;
#ifndef __PARTSET_DEF_
#define __PARTSET_DEF_
typedef CTmaPtrList<CLDSPart,CLDSPart*> PARTSET;
#endif
typedef CTmaPtrList<CLDSLinePart,CLDSLinePart*> LINEPARTSET;

/* 该部分代码已移至ComplexId.h,ComplexId.cpp中 wjh-2014.6.14
struct SUBID{
protected:
	long wholeId;
public:
	static const BYTE GROUP_DEFAULT	= 0;//默认模式
	static const BYTE GROUP_BOUNDARY= 1;//构件的外形边框
public:
	SUBID(long id=0,BYTE cGroup=0);	//将组号与标识合并为组标识
	virtual bool Set(long id=0,long group=0);
	virtual long Group() const;				//从组标识中提取组号部分，组标识信息部分占5个比特位
	virtual long Id() const;				//从组标识中提取标识部分
	long FromString(const char* subidStr);	//组标识与子标识间通过'-'分隔
	CXhChar16 ToString(char cJunction='-',bool bIdOnAfter=true,int alignSize=0) const;
	operator DWORD(){return (DWORD)wholeId;}
	operator long(){return wholeId;}
	bool operator !=(long Id){return wholeId!=Id;}
};
*/
typedef struct tagLDSOBJNO
{
	int iNo;
	BOOL bInUse;
}LDSOBJNO;

class CObjNoManager;
class CObjNoGroup
{
	friend class CObjNoManager;
	long m_idInit;	//起始id(即最小的id值),默认为1
	long m_idGroup;
	long m_nMaxNo;
	ATOM_LIST<LDSOBJNO>list;
public:
	CObjNoGroup(int nMaxNo=0);
	void InitMaxNo(int nMaxNo);
	int EnumIdleNo();
	void SetNoState(int iNo,BOOL bInUse=TRUE);
	long GetUsedNoCount();
	long GetFirstUsedNo();
	long GetNextUsedNo();
public:
	void EmptyNo();
};
class CObjNoManager
{
	ATOM_LIST<CObjNoGroup>NoGroupList;
public:
	CObjNoManager();
	CObjNoGroup* AppendGroup(long ID,long nMaxNo);
	CObjNoGroup* FromGroupID(long ID);
	void EmptyGroupNo();
	void EmptyGroup();
};
struct IConsole;
struct ISysPara
{
public:
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	BOOL m_bAuxPoleAnalysisInc;				//辅材参预有限元力学分析计算
	//virtual int GetExecuteLoadCode()=0;
	//virtual double gamaG()=0;				//永久荷载（重力荷载）分项系数
	//virtual double Beta_z_Design()=0;		//优化选材时风荷载调整系数（风振系数）
	//virtual double Beta_z_Foundation()=0;	//基础设计时风荷载调整系数（风振系数）
	//virtual bool IsReadOnly(int coefType)=0;
#endif
	virtual int GetBendClearance()=0;
	virtual BOOL IsPreferAngleSimplePosMode()=0;
	virtual BOOL IsDisplayAllHole()=0;
	virtual BYTE HoleWallDisplayMode()=0;
	virtual BOOL IsDelBoltsResideOnPlate()=0;
};
class CBlockParameter{
public:
	CBlockModel *m_pBlock;
	CBlockReference *m_pBlockRef;
	long m_hMotherObj;
	CBlockParameter(){
		m_pBlockRef=NULL;
		m_pBlock=NULL;
		m_hMotherObj=0;
	}
};
struct ILog
{
	virtual bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents){return true;}
};
class COldVerDataFile;
long HexStrToLong(const char* idstr);
class CLDSObject : public ILog{
protected:
	BYTE m_bLocked;
public:
	static IConsole* _console;
	static ISysPara* sysPara;	//当前所遵循的荷载计算规范
	static ILog2File* log2file;
	static const BYTE SHADOW_NONE	              = 0;	//非影射构件
	static const BYTE SHADOW_ISOLATED             = 1;	//普通构件生成影射构件时的参数(记录母本构件句柄、定位坐标系
	static const BYTE SHADOW_EMBEDDED             = 2;	//嵌入式部件构件生成影射构件时的参数(记录母本构件句柄、构件从属的块引用)
	static const BYTE SHADOW_EXTERBLK             = 3;	//导入式部件构件生成影射构件时的参数(记录母本构件句柄(块内构件句柄、构件从属的块引用
	static const BYTE SHADOW_ISOLATED_RELA_LOCATION = 4;//依附其他影射构件生成影射构件时的参数(记录母本构件句柄、构件从属的影射构件句柄)
	//影射构件分类
	static const BYTE SHADOWMODE_NONE	= 0;	//非影射构件
	static const BYTE SHADOWMODE_COMMON = 1;	//普通影射构件,对应SHADOW_ISOLATED、SHADOW_ISOLATED_RELA_LOCATION
	static const BYTE SHADOWMODE_BLOCK	= 2;	//部件装配影射构件,对应SHADOW_EMBEDDED、SHADOW_EXTERBLK
	static ILog2File* Log2File();	//永远会返回一个有效指针,但只有log2file!=NULL时,才会真正记录错误日志 wjh-2015.2.7
protected:
	int m_nClassTypeId;
	//char m_sClassName[MAX_TEMP_CHAR_50+1];
	BOOL *m_pModified;
	bool is_undo_modified;	//仅用于Undo操作实现过程中记录数据是否发生了修改
	IModel *m_pModel;
	//////////////////////////////////////////////////////////////////////////////////////
	//影射构件的相关属性
	CLDSObject *m_pMotherObj;
	/*
		m_iShadowObjType=SHADOW_ISOLATED, 普通影射构件
		m_iShadowObjType=SHADOW_EMBEDDED, 嵌入式部件装配引用打碎后的子影射构件
		m_iShadowObjType=SHADOW_EXTERBLK, 外生式部件装配引用
		m_iShadowObjType=SHADOW_ISOLATED_RELA_LOCATION, 普通影射构件中相对定位构件(常用于螺栓定位)
	*/
	BYTE m_iShadowObjType;		
	union SHADOW_PARA
	{
		struct{	//普通影射构件, m_iShadowObjType=SHADOW_ISOLATED
			THANDLE m_hMotherObj;	//影射源对象句柄(m_pMotherObj,注意不一定与m_hAcs装配坐标系中的源对象相同)
			THANDLE m_hAcs;			//影射后的参数化装配坐标系句柄
		}COMMON_OBJ;
		struct{
			//m_iShadowObjType=SHADOW_EMBEDDED时指杆塔内影射源对象句柄，m_iShadowObjType=SHADOW_EXTERBLK时指部件内影射源对象句柄
			THANDLE m_hMotherObj;
			THANDLE m_hBlockRef;	//装配引用对象的句柄（用以确定子影射构件的装配坐标系）
		}BLOCK_OBJ;
	}shadow;
public:
	THANDLE handle;
	long m_hBlock;				//归属的嵌入式内生部件块句柄
	DWORD dwPermission;			//权限字，用于标识谁对此象拥有修改权限
	long feature;				//定义用户自己的信息(临时备用,不可持久存储，也不一定都用)
	virtual void SetTagInfo(void* pTag){memcpy(&feature,&pTag,4);}
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsGeObject() const{return false;}
	virtual bool IsDbObject() const{return false;}
	virtual void CleanInvalidObjRef(){;}
	virtual void TransToCS(GECS& cs){;}
	virtual void Offset(const double* offset){;}
	virtual bool OffsetPosParam(const double* offset){return false;}
	CXhChar50 GetBlockIdName();
	CLDSObject();
	~CLDSObject();
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0){;}
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0){;}
	virtual void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	virtual void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	DWORD SetPermission(DWORD dwPerm);
	void GetClassType(char class_type[MAX_TEMP_CHAR_50+1]);
	virtual bool IsCompositeSonAngle(){return false;}	//是否为组合角钢的子角钢
	virtual int GetClassTypeId()const{return m_nClassTypeId;}
	virtual bool IsBelongTo(int iClassTypeId)const{return m_nClassTypeId==iClassTypeId;}
	void SetDocModifiedFlagAddress(BOOL *pModified)
	{	m_pModified=pModified;	}
	virtual void SetModified(BOOL bTrue=TRUE,BOOL bRenderSolid=TRUE){
		if(bTrue)
		{
			is_undo_modified=TRUE;
			SetDocModifiedFlag();
		}
	}
	virtual void ResetUndoModifiedFlag(){is_undo_modified=FALSE;}
	BOOL IsUndoModified(){return is_undo_modified;}
	void SetDocModifiedFlag()	//只是通知文档发生了修改，不修改构件本身任何属性
	{	if(m_pModified) *m_pModified=TRUE; }
	void SetBelongModel(IModel *pModel){m_pModel=pModel;}
	IModel* BelongModel() const;
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true){;}
	bool Lock(){
		if(m_bLocked)
			return false;
		else
			m_bLocked = true;
		return true;
	}
	bool IsLocked(){return m_bLocked!=0;}
	void Unlock(){m_bLocked = false;}
	//影子构件的相关方法
	void SetShadowPara(CLDSObject *pMotherObj,long hObj,int iShadowObjType=SHADOW_ISOLATED);
	void SetShadowPara(CLDSObject *pMotherObj,CBlockReference *pBlockRef);
	virtual bool IsAggregated(){return false;}
	virtual void SetAggregateState(bool aggregated=true){;}	//设置聚合状态
	virtual BOOL GetObjPropReadOnlyDefaultState(long prop_id){return FALSE;}
	//由于同一对象以不同类别调用结果不同，GetPropValueStr函数不能设为virtual 函数 wjh-2017.5.17
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100){return 0;}	//根据属性ID得到属性值字符串
	void EmptyShadowPara();
	CLDSObject* UpdateMotherObject();
	//ancestor=true时返回始祖级对象，否则返回直接父对象（如对称时）
	CLDSObject* MotherObject(bool ancestor=true);
	bool IsBlockRefShadowObject();	//是否为装配部件打碎时产生的影射构件
	BYTE ShadowMode();
	bool IsShadowObject();
	bool IsMotherObject();
	int GetMotherClassTypeId()const;
	bool IsMemberOfBlockRef(CBlockReference *pBlockRef);
	bool IsBlockRefPart();
	bool GetBlockParameter(CBlockParameter &blockPara);
	int GetShadowObjType(){return m_iShadowObjType;}
	CParaCS* GetParaCS();
	//
	bool ShadowParaAndBlockIdToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	bool ShadowParaAndBlockIdFromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
public://公共属性
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType){return false;}
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType){return false;}
	virtual long get_Id();
	virtual long set_Id(long hId);
	_declspec(property(put=set_Id,get=get_Id)) long Id;	//对象的句柄值
};
class COldVerLdsObjLife{
private:
	bool m_bRestore;
	long m_iVersion;
	long m_iDocType;
	CBuffer m_xBuffer;
	CLDSObject* m_pSrcObj;
	COldVerDataFile* m_pOldVerInfo;
public:
	COldVerLdsObjLife();
	COldVerLdsObjLife(COldVerDataFile* pOldVerInfo,CLDSObject* pObj,long version=0,long doc_type=0);
	~COldVerLdsObjLife();
};
class RELATIVE_OBJECT{
public:
	RELATIVE_OBJECT(){;}
	RELATIVE_OBJECT(long h,MIRMSG mirmsg){hObj=h;mirInfo=mirmsg;}
	long hObj;
	MIRMSG mirInfo;
	void SetKey(DWORD key){hObj=(long)key;}
};
class CFGINFO{
	CFGWORD _cfgword;	//配材号信息
	bool m_bLegObj;		//是否为接腿对象
public:
	CFGINFO(){m_bLegObj=true;}
	CFGINFO(const CFGWORD& cfgword,bool bLegObj){Set(cfgword,bLegObj);}
	void Set(const CFGWORD& cfgword,bool bLegObj)
	{
		m_bLegObj=bLegObj;
		if(!bLegObj)	//对本体对象进行配材号统一化处理
			_cfgword=cfgword.ToBodyWord();
	}
	bool IsLegObj() const{return m_bLegObj;}
	CFGWORD Cfgword() const{return _cfgword;}
};
class MEMORY_BLOCK{
	WORD  wSize;
	union{
		UINT u4Bits;
		char* block;
	};
public:
	MEMORY_BLOCK(){wSize=4;u4Bits=0;}
	MEMORY_BLOCK(MEMORY_BLOCK& memblock);
	~MEMORY_BLOCK(){ReleaseBlockBuffer();}
	void ReleaseBlockBuffer();
	WORD Size() const{return wSize;}
	bool operator==(const MEMORY_BLOCK& memblock);
	MEMORY_BLOCK& operator=(const MEMORY_BLOCK& memblock);
	const char* Data(WORD wiByteIndex=0) const;
	bool IsZero() const{return (wSize<=4&&u4Bits==0);}
	bool GetBitState(WORD bitpos);	//bitpos以0为基数
	void SetBitState(WORD bitpos, bool state=true);
	void CopyState(WORD addr_dest, const char* fromStr,WORD size);
	void FromBuffer(BUFFER_IO* buffer_io,long version=NULL,long doc_type=0);
	void ToBuffer(BUFFER_IO* buffer_io,long version=NULL,long doc_type=0);
};
class CLDSDbObject : public CLDSObject
{
protected:
	char layer_tag[4];		//所在图层
	SEGI _iSeg;				//分段号
	bool _bSyncShadowPartNo;
//protected:
	char m_ciLegQuad;		//接腿部分节点、杆件或其它构件的归属接腿像限1、2、3、4，0表示根据位置智能判断
	virtual char get_ciLegQuad();
	virtual char set_ciLegQuad(char ciBelongLegQuad);
public:
	PROPERTY(char,ciLegQuad);
	GET(ciLegQuad){ return get_ciLegQuad();}
	SET(ciLegQuad){ set_ciLegQuad(value); }
public:
	struct PROPVAL{
		static const BYTE TYPE_NA		=0;
		static const BYTE TYPE_BOOL		=1;
		static const BYTE TYPE_BYTE		=2;
		static const BYTE TYPE_WORD		=3;
		static const BYTE TYPE_DWORD	=4;
		static const BYTE TYPE_LONG		=5;
		static const BYTE TYPE_DOUBLE	=6;
		static const BYTE TYPE_COORD3D	=7;
		BYTE ciType;	//0.undefined;1.bool;2.BYTE;3.WORD;4.DWORD;5.long;6.double;7.COORD3D
		union{
			bool blVal;
			BYTE cbVal;
			WORD wiVal;
			DWORD dwVal;
			long liVal;
			double fVal;
			COORD3D vPoint;
		};
		BYTE ToByte();
		WORD ToWord();
		DWORD ToDword();
		long ToLong();
		double ToDouble();
	};
	static const UINT PROP_LOCATION	;//=1;		//装配位置
	static const UINT PROP_SIZESPEC	;//=2;		//构件宽度
	static const UINT PROP_WIDTH	;//=3;		//构件宽度
	static const UINT PROP_THICK	;//=4;		//构件宽度
	static const UINT PROP_LENGTH	;//=5;		//构件长度
	static const UINT PROP_MATERIAL	;//=6;		//构件材质
	static const UINT PROP_NORMAL	;//=7;		//构件法线
	static const UINT PROP_WINGX_NORMAL	;//=8;	//角钢X肢法线
	static const UINT PROP_WINGY_NORMAL	;//=9;	//角钢Y肢法线
	static const UINT PROP_POS_START	;//=10;	//杆件始端位置
	static const UINT PROP_POS_END		;//=11;	//杆件终端位置
	static const UINT PROP_PROFILE		;//=12;	//钢板外形
	virtual bool GetPropParam(long idObjProp,PROPVAL* pVal=NULL){return false;}
	virtual bool SetPropParam(long idObjProp,PROPVAL& propval){return false;}
	virtual void IncrementConstraintRef();
	CONSTRAINT* AppendConstraint(CONSTRAINT* pCopyFrom=NULL);
	CONSTRAINT* GetConstraintByPropID(WORD idDbProp);
	bool RemoveConstraint(long constraint_id);
	IXhTemplEnumerator<CONSTRAINT>* GetConstraintEnumerator();
	virtual bool ProcessConstraintFromMasterObj(CLDSDbObject* pMasterObj,CONSTRAINT* pConstraint);
	virtual long ReDesignConstraintSonObjs(WORD idMasterObjProp=0,CHashSet<CLDSDbObject*>* pHashUpdateObjSet=NULL);
protected:
	long m_nHasConstraintsCounts;
public:
	READONLY_PROPERTY(bool,bHasConstraints);
	GET(bHasConstraints){return m_nHasConstraintsCounts>0;}
public:	//公用属性
	PROPERTY(SEGI,iSeg);
	GET(iSeg){
		if(!_bSyncShadowPartNo)
			return _iSeg;
		CLDSDbObject* pMotherObj=(CLDSDbObject*)MotherObject();
		if(pMotherObj==this)
			return _iSeg;
		else
			return pMotherObj->iSeg;
	}
	SET(iSeg){
		if(!_bSyncShadowPartNo)
		{
			_iSeg=value;
			SetModified(TRUE,FALSE);
			return;
		}
		CLDSDbObject* pMotherObj=(CLDSDbObject*)MotherObject();
		if(_iSeg.iSeg!=value.iSeg)
		{
			_iSeg=value;
			UpdatePartNoBySegI();
		}
		if(pMotherObj!=this)
			pMotherObj->iSeg=value;
	}
	PROPERTY(bool,m_bSyncShadowPartNo);
	GET(m_bSyncShadowPartNo){ return _bSyncShadowPartNo;}
	SET(m_bSyncShadowPartNo){ _bSyncShadowPartNo=value; }
public:
	BOOL is_highlight;		//是否高亮显示
	BOOL is_visible;		//是否处于显示状态
	char sNotes[MAX_TEMP_CHAR_50+1];
	CFGWORD cfgword;		//配材号
	CHashListEx<RELATIVE_OBJECT> relativeObjs;
	RELATIVE_OBJECT* GetMirRelaObj(MIRMSG& mirmsg){
		for(RELATIVE_OBJECT* pMirRelaObj=relativeObjs.GetFirst();pMirRelaObj;pMirRelaObj=relativeObjs.GetNext())
		{
			if(pMirRelaObj->mirInfo.IsSameMirMsg(mirmsg))
				return pMirRelaObj;
		}
		return NULL;
	}
	MEMORY_BLOCK mpcl;		//针对当前对象的特定属性对称拷贝
	virtual PROPLIST_TYPE* GetSyncPropList(){return NULL;}
	virtual MEMORY_BLOCK GetDefaultMPCL(){return mpcl;}
public:
	static void MirObjLayer(const char* sOrgLayer,char* sNewLayer,MIRMSG mirmsg);
	virtual CFGWORD UnifiedCfgword();//若为本体对象，则返回补齐全部接腿占位后的配材号
	virtual CFGWORD GetUnifiedCfgword() const;//若为本体对象，则返回补齐全部接腿占位后的配材号
	virtual CFGINFO Cfginfo(){return CFGINFO(cfgword,IsLegObj());}	//获取该对象的配材信息
	virtual bool ModuleTogetherWith(const CFGINFO& cfginfo) const;	//判断两对象是否共存于同一呼高实例
	virtual bool ModuleTogetherWith(CLDSDbObject* pDbObj) const;		//判断两对象是否共存于同一呼高实例
	virtual void CopyModuleInstanceInfo(CLDSDbObject* pDbObj);	//拷贝对象的塔模型实例配材信息（配材号及图层首字母）
	virtual bool IsLegObj()const{return toupper(layer_tag[0])=='L';}	//是否为接腿对象
	virtual bool IsBodyObj(){return toupper(layer_tag[0])!='L';}//是否为主体(塔头及身)对象
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsGeObject() const{return false;}
	virtual bool IsDbObject() const{return true;}
	virtual bool IsPart() const{return false;}
	virtual bool IsLinePart() const{return false;}
	virtual bool IsNode() const{return false;}
	virtual void ResetRelativeObjs(){;}
	virtual long ArcLiftHandle(){return 0;}
	virtual long SetArcLift(long hArcLift){return 0;}
	virtual void UpdatePartNoBySegI(){;}
	virtual void CalPosition(bool bIgnoreRepeatFlag=false){;}
	virtual void UpdateRelaObjArcLiftProp();
	//同步属性
	static const DWORD PROP_DISCONNECT	= 0;	//断开
	static const DWORD PROP_SYNC_COPY	= 1;	//同步对称
	static const BYTE MIR_X = 1;	//X轴对称
	static const BYTE MIR_Y = 2;	//Y轴对称
	static const BYTE MIR_Z = 3;	//Z轴对称
	virtual bool IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr);
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部位置对称无关的属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,long hMirAcs,short wRotAngle,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff){return false;}
	RELATIVE_OBJECT *AppendRelativeObj(RELATIVE_OBJECT &relaObj);
	virtual BOOL IsFatherObject();
	RELATIVE_OBJECT* GetRelativeObjInfo(long h,bool bIncRotateMir=true);
	//bIncSelf 是否包括自对称情况
	virtual long GetRelativeMirObj(MIRMSG msg,bool bIncSelf=TRUE);
	virtual BOOL GetFatherMirObj(RELATIVE_OBJECT *pRelaObj);
	virtual void CopyProperty(CLDSDbObject* pObj){;}
	CLDSDbObject();
	char& layer(int i);
	char  Layer(int i) const;
	char* layer();
	char get_chLayer1st();
	char set_chLayer1st(char chValue);
	__declspec(property(put=set_chLayer1st, get=get_chLayer1st)) char chLayer1st;
	char get_chLayer2nd();
	char set_chLayer2nd(char chValue);
	__declspec(property(put=set_chLayer2nd, get=get_chLayer2nd)) char chLayer2nd;
	char get_chLayer3rd();
	char set_chLayer3rd(char chValue);
	__declspec(property(put=set_chLayer3rd, get=get_chLayer3rd)) char chLayer3rd;
	BOOL SetLayer(const char *layer);
	const char* get_szLayer() { return this->layer_tag; }
	__declspec(property(get = get_szLayer)) const char* szLayer;
	virtual void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	virtual void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	//属性管理
	static int m_iCurDisplayPropGroup;
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;		//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSDbObject,propHashtable)
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	static void InitDbObjectPropHashtable();
	static int GetPropStr(long id, char *propStr);		//得到属性字符串
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
};
class CLDSAnchorBolt;
typedef CLDSAnchorBolt* CLDSAnchorBoltPtr;
class CLDSFoundation : public CLDSDbObject
{
	BOOL is_data_modified;
	BOOL is_solid_modified;
public:
	BYTE ciHeightSerial;//基础对应的呼高号
	BYTE ciLegSerial;	//基础在归属呼高中的减腿号（一般最高腿为1，高->你依次逐级加1）
	BYTE ciAnchorBoltD;	//基础螺栓直径
	WORD wiWidth;		//基础顶面矩形显示宽度（按混凝土灰色显示区域）
	WORD wiHeight;		//基础深(高)度
	COLORREF crColor;	//预览显示颜色
	//struct LCS{		//基础工作定位坐标系
	GEPOINT location;	//基础顶面中心点位置
	GEPOINT vWorkNormal;//基础顶平面法线（地面朝上）
	GEPOINT vProjAxisX;	//局部(工作平面)坐标系的X轴水平投影方向
	//};
	CSolidBody *pSolidBody;
	//BYTE ciQuad;		//已在父类CLDSDbObject中定义
	struct LOCATION{
		long hAnchorBolt;	//关联的地脚螺栓句柄
		short siPosX,siPosY;
		LOCATION(short posx=0,short posy=0,long _hRelaBolt=0){
			hAnchorBolt=_hRelaBolt;
			siPosX=posx;
			siPosY=posy;
		}
		void SetLocation(short posx,short posy)
		{
			siPosX=posx;
			siPosY=posy;
		}
	};
	PRESET_ARRAY8<LOCATION> anchors;
public:
	CLDSFoundation(IModel *pModel=NULL);
	~CLDSFoundation();
	//
	virtual char get_ciLegQuad();
	virtual void SetModified(BOOL bTrue=TRUE,BOOL bRenderSolid=TRUE);
	virtual CSolidPart GetSolidPartObject();
	BOOL Create3dSolidModel();
	BOOL IsModified(){return is_data_modified;}
	BOOL IsSolidModified(){return is_solid_modified;}
	GEPOINT GetAnchorLocalPosition(CLDSBolt* pAnchorBolt);
	GEPOINT TransToModelCS(const double* xAnchorBoltLocalLocation);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void LayoutAnchorBolts(int schema=0,short S=200);	//schema=0表示四螺栓布置;schema=1表示八螺栓布置
	GECS GetAcs();	//获取基础的装配坐标系
	UINT AddOrUpdateAnchorBolts(WORD wiBasePlateThick=0,bool blMatchDetachedAnchor=true,bool blAppendLostAnchorBolts=true);
	//属性栏
	DECLARE_PROP_FUNC(CLDSFoundation);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值
};

class CLDSGeObject : public CLDSObject
{
public:
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsGeObject() const{return true;}
	virtual bool IsDbObject() const{return false;}
	CLDSGeObject();
};
//厚度设计
#pragma pack(push,1)
struct THICK_ITEM{
public:
	static const int PART_THROUGH	 =1;	//穿过构件（在存在任一ItemType=3时，不计通厚）
	static const int THICK_THROUGH	 =2;	//单纯穿过指定厚度
	static const int PART_THROUGHnT  =3;	//穿过构件（优先排它性计入通厚构件）
	char cItemType;	//0.FinalKey;1.穿孔构件(不计通厚）句柄值;2.厚度值;3.不受呼高限制的穿孔（优先排它性计入通厚构件）构件句柄
public:
	int  val;
	THICK_ITEM(const char* data=NULL);
	//主要用于GetNextItem()函数中用于指定是否已经结束
	bool IsFinalKey();	//{return (cItemType<=0||cItemType>3);}
	bool IsHandle();	//{return cItemType==1||cItemType==3;}
};
struct THICK_ITEMTAG : public THICK_ITEM
{
	CLDSPart *pPart;//关联的构件
	int through;	//穿过的通厚
	double zOffset;	//穿过的起始位置Z偏移量
public:
	THICK_ITEMTAG(const THICK_ITEM& item){
		cItemType=item.cItemType;
		val=item.val;
		pPart=NULL;
		zOffset=through=0;
	};
	THICK_ITEMTAG(){pPart=NULL;zOffset=through=0;}
};
#pragma pack(pop)
class CDesignThickPara
{
	CXhChar50 keystr;	//临时(transient)存储用,不对外公开,主要用于支持GetFirstItem,GetNextItem
public:
	static IModel* m_pTower;
	bool ReplaceReferenceHandle(CHandleReplacer& replacer);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	void CleanInvalidObjRef(IModel *pModel=NULL);
	bool AddThick(int val,BOOL bHandle=FALSE,BOOL bSamePartFilter=FALSE);
	CDesignThickPara(){memset(this,0,sizeof(CDesignThickPara));}
	CDesignThickPara(const char *str){key_str.Copy(str);}
	CDesignThickPara(double dblThick);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	CXhChar50 key_str;
	long Thick(IModel* pTower,bool bEnableModuleFilter=false);	
	CDesignThickPara MirThickPara(IModel* pModel,MIRMSG msg);
	void ClearRedundant(IModel* pTower=NULL);
	void EmptyThick();
	THICK_ITEM GetFirstItem();
	THICK_ITEM GetNextItem();
	//void Serialize(CArchive &ar);
};
class IHeapMemory;
class CParaThick
{
	//临时将MAX_BUFF_LEN加到220，未来应考虑通过IHeapMemory来解决这种极端情况 wjh-2019.8.12
#ifndef __LDS_FILE_
	static const int MAX_BUFF_LEN	= 160;	//100->160多用60字节，通过CLDSBolt中减少属性占用字节节省80个字节 wjh-2019.7.16
	static const int MAX_ITEM_COUNT =32;	//100->160多用60字节，通过CLDSBolt中减少属性占用字节节省80个字节 wjh-2019.7.16
#else
	static const int MAX_BUFF_LEN	= 220;	//100->160多用60字节，通过CLDSBolt中减少属性占用字节节省80个字节 wjh-2019.7.16
	static const int MAX_ITEM_COUNT =44;	//100->160多用60字节，通过CLDSBolt中减少属性占用字节节省80个字节 wjh-2019.7.16
#endif
	char ciIndex,datapool[MAX_BUFF_LEN];
	char* m_pAllocData;	//m_pExterAllocData!=NULL时，以m_pExterAllocData为基准
	WORD m_wnAllocDataLength;
	long get_hHeapMemory();
	long set_hHeapMemory(long hMemory);
	__declspec(property(put=set_hHeapMemory,get=get_hHeapMemory)) long hHeapMemory;
	char* get_pData();
	__declspec(property(get=get_pData)) char* data;
public:
	static IModel* m_pTower;
	static IHeapMemory* m_pExterHeapMemory;
	//THICK_ITEM::cItemType;	//0.FinalKey;1.穿孔构件(不计通厚）句柄值;2.厚度值;3.不受呼高限制的穿孔（计入通厚）构件句柄
	void AttachExterData(char* pExterData,UINT uiDataLength);
	void CopyFromData(char* pExterData,UINT uiDataLength);
	void DetachAllocData();
	UINT MaxBuffLen() const;
	__declspec(property(get=MaxBuffLen)) UINT uiMaxBuffLength;
	bool ReplaceReferenceHandle(CHandleReplacer& replacer);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	//void CleanInvalidObjRef();
	bool AddThick(int val,BOOL bHandle=FALSE,BOOL bSamePartFilter=FALSE);
	void BuildFromStr(const char *str);	//以*代替'X',表示不受当前呼高限制的穿孔构件wjh-2015.4.10
	void FromBuffStr(CBuffer &buffer);
	CParaThick(){memset(this,0,sizeof(CParaThick));}
	CParaThick(const char *str){BuildFromStr(str);}
	CParaThick(double dblThick);
	bool SetItem(int i,THICK_ITEM item);	//i取值0~19,表示子项索引而不是内存data的索引　wjh-2015.4.10
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//腿多时腿身衔接板上的螺栓可能穿过10~20块板，此时字符串最长可达200左右, 
	//故此返回字符串设成CXhChar500 wjh-2019.7.11
	CXhChar500 ToThickStr(bool toLegacyMode=false) const;
	long Thick(IModel* pTower,bool bEnableModuleFilter=false,bool* pHasPriorPartThick=NULL);	
	CParaThick MirThickPara(IModel* pModel,MIRMSG msg);
	void ClearRedundant(IModel* pTower=NULL);
	void EmptyThick();
	THICK_ITEM GetFirstItem();
	THICK_ITEM GetNextItem();
};
class CLDSArcLift : public CLDSGeObject
{
public:
	long iNo;				//预拱序号
	char name[MAX_TEMP_CHAR_50+1];
	char m_ciLiftType;		//0.圆弧预拱;1.垂直预拱(Z轴预拱)
	char m_cRotAxis;		//预拱时旋转轴'X':X轴;'Y':Y轴
	GEPOINT m_xRotOrg;		//预拱旋转基点
	GEPOINT m_xDatumLiftPos;//基准预拱点初始位置
	double m_fLiftHeight;	//基准预拱点的预拱值,mm
public:
	CLDSArcLift();
	// initPos:初始点位置；bInRightSide:初始点在旋转轴右侧(左侧反向旋转预拱)
	f3dPoint LiftPosTrans(const f3dPoint& initPos);
	//进行逆向预拱还原运算（主要用于更改预拱后，指定角钢预拱前楞点坐标）wjh-2015.5.19
	f3dPoint ReverseLiftPosTrans(const f3dPoint& arcPos);
	bool IsInRightSide(const f3dPoint& pickPos);
	// initPos:初始方向；bInRightSide:初始点在旋转轴右侧(左侧反向旋转预拱)
	f3dPoint LiftVecTrans(const f3dPoint& initVec,bool bInRightSide=true);
	f3dPoint LiftVecTrans(const f3dPoint& initVec,const f3dPoint& pickPos);
	f3dPoint ReverseLiftVecTrans(const f3dPoint& initVec,bool bInRightSide=true);
	f3dPoint ReverseLiftVecTrans(const f3dPoint& initVec,const f3dPoint& pickPos);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
};
struct VECTOR_PARA{
	//0.指定;1.角钢肢法线;2.杆件交叉面法线3.构件工作法线4.基准构件上的相对方向5.基准杆件上的特征方向
	short norm_style;
	//norm_style==1、3:	0.朝外;1.朝里 
	//norm_style==5:	0.始->终      1.终->始 
	//					2.+角钢norm_x(自基准边平面法向绕钢管逆时针旋转rot_ang角度后的方向、
	//					  扁钢工作法线方向+
	//					  槽钢肢法线方向(X轴方向)+)
	//					3.-角钢norm_x(自焊道线平面法向绕钢管逆时针旋转rot_ang角度后的方向、
	//					  扁钢工作法线方向-
	//					  槽钢肢法线方向(X轴方向)-)
	//					4.+角钢norm_y (钢管始端切割平面法线、槽钢侧面法线方向(Y轴方向)+)
	//					其他.-角钢norm_y(钢管终端切割平面法线、槽钢侧面法线方向(Y轴方向)-)
	short direction;
	THANDLE hVicePart;			//工作法线基准杆件
	THANDLE hCrossPart;			//交叉杆件
	union{
		short norm_wing;	//0.X肢;1.Y肢（仅对基准构件是角钢时有效）
		BYTE reside_plane;	//0.所在钢板面号(0表示基面;1.表示第一个火曲面;2.表示第二个火曲面) wjh-2015.2.7
	};
	double fRotAngAroundTube;	//绕钢管逆时针旋转指定的角度°(因一般指定时都是角度制，故此处存储角度单位制)
	COORD3D vNearVector;
public:
	void ZeroClear();
	void ToBuffer(BUFFER_IO* pIO);
	void FromBuffer(BUFFER_IO* pIO);
};
struct POINT_PARA{
	/*  0.无位置设计信息,直接指定1.角钢两端楞点为基准;2.杆件上节点为基准;
		3.角钢心线交点;4.基准线交点;5.基准线上轴向坐标不变点;
		6.基准面上轴向坐标不变点,7.沿角钢两肢法向偏移的包铁楞点,8.与节点同位,
		9.指定构件上的相对坐标,10.两节点间比例等分点(该类型仅用于填板定位,不在基准点对话框中显示)
		11.柱面定位点
	*/
	BYTE ciLocationType;	//datum_pos_style
	COORD3D xDatumPos;
	union
	{
		THANDLE hNode;	//与节点同位
		THANDLE hPart;	//指定构件上的相对坐标
		struct{
			THANDLE hDatum1;
			THANDLE hDatum2;
			short wing_offset_style1,wing_offset_style2;	//0.g;1.g1;2.g2;3.g3;4.自定义
			long wing_offset_dist1,wing_offset_dist2;
			double near_vector[3];	//仅在法向偏移量有值时有效
			char cProjectPlane;	//投影面类型0.交叉面（叉积法线为面法线）;1.X-Z平面;2.Y-Z平面;3.X-Y平面
			char norm_offset[MAX_TEMP_CHAR_50+1];//法向偏移量;
		}AXIS_INTERS;
		struct tagRODEND{
			THANDLE hRod;		//杆件句柄
			short direction;	//0.基准杆件方向:始->终;1.终->始
			short offset_wing;  //0.X肢;1.Y肢
			short wing_offset_style;	//0.g;1.g1;2.g2;3.g3;4.自定义
			double wing_offset_dist,len_offset_dist;//角钢肢向偏移值;纵向偏移值;
			BOOL bIncOddEffect;			//纵向偏移值计入正负头影响
			char norm_offset[MAX_TEMP_CHAR_50+1];			//法向偏移量
			void SetWingOffset(double wing_offset=0,short offset_g_style=4);
		}RODEND;	//以后为避免误解应统一更名为RODEND wjh-2013.11.16
		struct{
			THANDLE hRod;	//角钢句柄
			THANDLE hNode;	//基准节点句柄
			WORD bFaceOffset;
			short direction;	//0.基准角钢方向:始->终;1.终->始
			short offset_wing;//0.X肢;1.Y肢
			short wing_offset_style;	//0.g;1.g1;2.g2;3.g3;4.自定义
			double wing_offset_dist,len_offset_dist;//肢向偏移值;纵向偏移值;
			char norm_offset[MAX_TEMP_CHAR_50+1];//法向偏移量;
			COORD3D vPlaneNormal;//face_norm_offset[3];	平推面法线
		}RODNODE;
		struct{
			THANDLE hDatumLine;
			char cMirTransType;	//0,'X','Y','Z'
			short fix_axis_val_style_x0_y1_z2;
		}DATUMLINE;
		struct{
			THANDLE hDatumPlane;
			char cMirTransType;	//0,'X','Y','Z'
			short fix_axis_style_x0_y1_z2;
		}DATUMPLANE;
		struct{
			THANDLE hLinePart;	//基准杆件句柄
			THANDLE hStartNode;	//始端节点句柄
			THANDLE hEndNode;	//终端节点句柄
			double start_offset_dist;	//始端偏移量
			double end_offset_dist;		//终端偏移量
			double offset_scale;//偏移比例
			double offset_dist;	//在等分基础上的偏移值
		}SCALE_NODE; //两节点间的比例等分点,该类型目前仅用于填板定位 wht 11-01-23
		struct tagCOLUMN_INTERS{
			THANDLE hLineTube;		//>0时以该钢管定义柱面｛axis_vec,axis_pick,column_r｝
			//自低到高指定位为1表示对应矢量或点为参数化点或方向。
			//第1位:axis_vec;
			//第2位:sect_center;
			//第3位:sect_norm;
			BYTE cPointAndVectorFlag;
			union{
				COORD3D axis_vec;		//柱面的轴线正方向
				long hVector;
			}AxisVector;
			union{
				COORD3D sect_center;	//柱面轴线与截面的交点，即圆截面（圆或椭圆）的圆心点
				long hPoint;
			}SectCenter;
			struct{
				long hNode;
				double len_offset;	//自hNode节点沿hLineTube始->终(hNode>0,或hNode<0时终->始)方向的轴向偏移量
			}SectCenterParam;
			union{
				COORD3D sect_norm;	//截面法向：为空时表示为柱面的垂直截面
				long hVector;
			}SectNorm;
			//0.ray_vec为直接定义的射线绝对方向;
			//1.ray_vec为用户指定的环向旋转角度起始边
			//2.hLineTube>0时，钢管基准边为柱面起始边
			//3.hLineTube>0时，钢管焊道线为柱面起始边
			//4.hLineTube>0时，钢管截面上的水平射线方向
			BYTE ray_vec_style;		//0.ray_vec为直接定义的射线绝对方向;>0:指定柱面起始边，并根据ray_angle确定射线方向
			union{
				COORD3D ray_vec;		//射线方向
				long hVector;
			}RayVector;
			double ray_angle;		//ray_vec_style==1时射线方向自柱面起始边的逆时针旋转角度,°
			double column_r;		//柱面半径
			double ray_para_offset;	//射线平行移动距离（垂直射线方向进行平行线移动,以右手螺旋正向侧移动为正，另一侧为负）
			bool bSpecRo;			//true:辅射半径以距柱心投影绝对距离定；false:辅射半径以柱面射线交点向外的延伸偏量确定
			double ray_offset_r;	//bSpecRo=true:辅射半径; bSpecRo=false:射线方向延伸偏移量
		public:
			//void SetAxisVector(double vx,double vy,double vz);
			//void SetSectNorm(double vx,double vy,double vz);
			//void SetSectCenter(double cx,double cy,double cz);
			//void SetRayVector(double vx,double vy,double vz);
		}COLUMN_INTERS;	// added by wjh-2013.10.13
	};
public:
	void ZeroClear();
	void ToBuffer(BUFFER_IO* pIO);
	void FromBuffer(BUFFER_IO* pIO);
};
class CParaPointLifeObj{
	bool m_bEnabled;
	static long CallAngleIds[2];	//快速记录的已调用角钢的句柄值
	static bool m_bLivingState;
	static CHashList<bool> hashAngleLockStates;
public:
	static bool LockCall(long hAngle);	//指定角钢未锁定时，锁定返回true,否则返回false wjh-2018.1.5
	CParaPointLifeObj();
	~CParaPointLifeObj();
};
class CLDSPoint: public CLDSGeObject {
	GEPOINT pos;	//最终返回位置坐标
public:
	static const BYTE COLUMN_INTERS_AXIS_VEC	= 1;
	static const BYTE COLUMN_INTERS_SECT_CENTER = 2;
	static const BYTE COLUMN_INTERS_SECT_NORM	= 3;
	static const BYTE COLUMN_INTERS_RAY_VEC		= 4;
	static IModel* m_pTower;
	short iNo;
	char description[MAX_TEMP_CHAR_50+1];
	/*  0.无位置设计信息,直接指定1.角钢两端楞点为基准;2.杆件上节点为基准;
		3.角钢心线交点;4.基准线交点;5.基准线上轴向坐标不变点;
		6.基准面上轴向坐标不变点,7.沿角钢两肢法向偏移的包铁楞点,8.与节点同位,
		9.指定构件上的相对坐标,10.两节点间比例等分点(该类型仅用于填板定位,不在基准点对话框中显示)
		11.柱面定位点
	*/
	BYTE datum_pos_style;
	long hAgentBlockRef;	//将自定义点从装配模型空间转换到部件空间中的关联媒介（部件引用）
	union DES_PARA
	{
		THANDLE hNode;	//与节点同位
		THANDLE hPart;	//指定构件上的相对坐标
		struct{
			THANDLE hDatum1;
			THANDLE hDatum2;
			short wing_offset_style1,wing_offset_style2;	//0.g;1.g1;2.g2;3.g3;4.自定义
			long wing_offset_dist1,wing_offset_dist2;
			double near_vector[3];	//仅在法向偏移量有值时有效
			char cProjectPlane;	//投影面类型0.交叉面（叉积法线为面法线）;1.X-Z平面;2.Y-Z平面;3.X-Y平面
			char norm_offset[MAX_TEMP_CHAR_50+1];//法向偏移量;
		}AXIS_INTERS;
		struct tagRODEND{
			THANDLE hRod;		//杆件句柄
			short direction;	//0.基准杆件方向:始->终;1.终->始
			short offset_wing;  //0.X肢;1.Y肢
			short wing_offset_style;	//0.g;1.g1;2.g2;3.g3;4.自定义
			double wing_offset_dist,len_offset_dist;//角钢肢向偏移值;纵向偏移值;
			BOOL bIncOddEffect;			//纵向偏移值计入正负头影响
			char norm_offset[MAX_TEMP_CHAR_50+1];			//法向偏移量
			void SetWingOffset(double wing_offset=0,short offset_g_style=4);
		}RODEND;	//以后为避免误解应统一更名为RODEND wjh-2013.11.16
		struct{
			THANDLE hRod;	//角钢句柄
			THANDLE hNode;	//基准节点句柄
			WORD bFaceOffset;
			short direction;	//0.基准角钢方向:始->终;1.终->始
			short offset_wing;//0.X肢;1.Y肢
			short wing_offset_style;	//0.g;1.g1;2.g2;3.g3;4.自定义
			double wing_offset_dist,len_offset_dist;//肢向偏移值;纵向偏移值;
			char norm_offset[MAX_TEMP_CHAR_50+1];//法向偏移量;
			COORD3D vPlaneNormal;//face_norm_offset[3];	平推面法线
		}RODNODE;
		struct{
			THANDLE hDatumLine;
			char cMirTransType;	//0,'X','Y','Z'
			short fix_axis_val_style_x0_y1_z2;
		}DATUMLINE;
		struct{
			THANDLE hDatumPlane;
			char cMirTransType;	//0,'X','Y','Z'
			short fix_axis_style_x0_y1_z2;
		}DATUMPLANE;
		struct{
			THANDLE hLinePart;	//基准杆件句柄
			THANDLE hStartNode;	//始端节点句柄
			THANDLE hEndNode;	//终端节点句柄
			double start_offset_dist;	//始端偏移量
			double end_offset_dist;		//终端偏移量
			double offset_scale;//偏移比例
			double offset_dist;	//在等分基础上的偏移值
		}SCALE_NODE; //两节点间的比例等分点,该类型目前仅用于填板定位 wht 11-01-23
		struct tagCOLUMN_INTERS{
			THANDLE hLineTube;		//>0时以该钢管定义柱面｛axis_vec,axis_pick,column_r｝
			//自低到高指定位为1表示对应矢量或点为参数化点或方向。
			//第1位:axis_vec;
			//第2位:sect_center;
			//第3位:sect_norm;
			BYTE cPointAndVectorFlag;
			union{
				COORD3D axis_vec;		//柱面的轴线正方向
				long hVector;
			}AxisVector;
			union{
				COORD3D sect_center;	//柱面轴线与截面的交点，即圆截面（圆或椭圆）的圆心点
				long hPoint;
			}SectCenter;
			struct{
				long hNode;
				double len_offset;	//自hNode节点沿hLineTube始->终(hNode>0,或hNode<0时终->始)方向的轴向偏移量
			}SectCenterParam;
			union{
				COORD3D sect_norm;	//截面法向：为空时表示为柱面的垂直截面
				long hVector;
			}SectNorm;
			//0.ray_vec为直接定义的射线绝对方向;
			//1.ray_vec为用户指定的环向旋转角度起始边
			//2.hLineTube>0时，钢管基准边为柱面起始边
			//3.hLineTube>0时，钢管焊道线为柱面起始边
			//4.hLineTube>0时，钢管截面上的水平射线方向
			BYTE ray_vec_style;		//0.ray_vec为直接定义的射线绝对方向;>0:指定柱面起始边，并根据ray_angle确定射线方向
			union{
				COORD3D ray_vec;		//射线方向
				long hVector;
			}RayVector;
			double ray_angle;		//ray_vec_style==1时射线方向自柱面起始边的逆时针旋转角度,°
			double column_r;		//柱面半径
			double ray_para_offset;	//射线平行移动距离（垂直射线方向进行平行线移动,以右手螺旋正向侧移动为正，另一侧为负）
			bool bSpecRo;			//true:辅射半径以距柱心投影绝对距离定；false:辅射半径以柱面射线交点向外的延伸偏量确定
			double ray_offset_r;	//bSpecRo=true:辅射半径; bSpecRo=false:射线方向延伸偏移量
		public:
			void SetAxisVector(double vx,double vy,double vz);
			void SetSectNorm(double vx,double vy,double vz);
			void SetSectCenter(double cx,double cy,double cz);
			void SetRayVector(double vx,double vy,double vz);
		}COLUMN_INTERS;	// added by wjh-2013.10.13
	}des_para;
public:
	CLDSPoint();
	//0.指定坐标点5.基准线6.基准面上点时，代表预拱前坐标位置
	//9.在基准构件相对坐标系下的位置坐标
	GEPOINT datum_pos;
	f3dPoint Position(){return pos;}
	f3dPoint SetRsltPositionOnly(const double* pxRsltPos);
	f3dPoint SetPosition(const f3dPoint& position);
	f3dPoint SetPosition(double x,double y,double z);
	f3dPoint SetPositionX(double x);
	f3dPoint SetPositionY(double y);
	f3dPoint SetPositionZ(double z);
	bool ReplaceRodEndNode(long hRod,long hOldEndNode,long hNewEndNode);
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,double* pickpos_coord,int idClassType=0);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	virtual bool OffsetPosParam(double offset,const double* vOffsetDirection);
	DECLARE_READONLY_PROPERTY(long,m_hNode);
	DECLARE_READONLY_PROPERTY(long,m_hDatumRod);	//仅在角钢(含包铁)楞点定位及杆件节点定位时返回对应的定位基准杆件 wjh-2016.11.8
	DECLARE_READONLY_PROPERTY(GEPOINT,xRsltPosition){return pos;}	//基准点最终的参数化计算结果位置
	BOOL UpdatePos(IModel *pModel=NULL,CLDSArcLift* pLift=NULL,bool bReverseLift=false);
	int MirToPoint(IModel *pModel,CLDSPoint &mirPoint,MIRMSG& mirinfo);	//0.表示对称成功，<0表示对称错误类型
	BOOL AssertValid(IModel *pModel,char *error_info=NULL);
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	void ToParaBuffer(BUFFER_IO* pIO);	//仅把参数化点位中的几何参数信息提取为POINT_PARA形式写入缓存
	void FromParaBuffer(BUFFER_IO* pIO);//从POINT_PARA导出缓存中读入参数化点位中的几何参数信息
	void CloneParaTo(CLDSPoint* pPoint);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//快速设定由角钢楞点(含包铁楞点型式)的基准点参数,其中offsetWing=0:X肢偏移;1:Y肢偏移;其余为包铁楞点双肢法向偏移
	void SetRapidAngleBer(THANDLE hJg,BYTE direction,BYTE wing_x0_y1_both2=0,double offsetDist=0.0,BYTE gStyle=4);
	//void Serialize(CArchive &ar);

	static void InitPropHashtable();						//初始化属性哈希表
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSPoint,propHashtable)
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	static long GetPropID(char* propStr);			//得到属性ID
};
class CLDSVector
{	//普通法线设计参数
public:
	static IModel* m_pTower;
	CLDSVector();
	bool ReplaceReferenceHandle(CHandleReplacer& replacer);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	void CleanInvalidObjRef(IModel *pModel=NULL);
	BOOL UpdateVector(IModel *pTower);
	BOOL AssertValid(IModel *pTower,char *error_info=NULL);
	long hAgentBlockRef;	//将自定义方向从装配模型空间转换到部件空间中的关联媒介（部件引用）
	GEPOINT vector,nearVector;	//实际所代表的方向矢量
	THANDLE hVicePart;			//工作法线基准杆件
	THANDLE hCrossPart;			//交叉杆件
	//0.指定;1.角钢肢法线;2.杆件交叉面法线3.构件工作法线4.基准构件上的相对方向5.基准杆件上的特征方向
	short norm_style;
	union{
		short norm_wing;	//0.X肢;1.Y肢（仅对基准构件是角钢时有效）
		BYTE reside_plane;	//0.所在钢板面号(0表示基面;1.表示第一个火曲面;2.表示第二个火曲面) wjh-2015.2.7
	};
	//norm_style==1、3:	0.朝外;1.朝里 
	//norm_style==5:	0.始->终      1.终->始 
	//					2.+角钢norm_x(自基准边平面法向绕钢管逆时针旋转rot_ang角度后的方向、
	//					  扁钢工作法线方向+
	//					  槽钢肢法线方向(X轴方向)+)
	//					3.-角钢norm_x(自焊道线平面法向绕钢管逆时针旋转rot_ang角度后的方向、
	//					  扁钢工作法线方向-
	//					  槽钢肢法线方向(X轴方向)-)
	//					4.+角钢norm_y (钢管始端切割平面法线、槽钢侧面法线方向(Y轴方向)+)
	//					其他.-角钢norm_y(钢管终端切割平面法线、槽钢侧面法线方向(Y轴方向)-)
	short direction;
	double rot_ang;		//绕钢管逆时针旋转指定的角度°(因一般指定时都是角度制，故此处存储角度单位制)
	MIRMSG mirmsg;
	void ToParaBuffer(BUFFER_IO* pIO);	//仅把参数化矢量中的几何参数信息提取为VECTOR_PARA形式写入缓存
	void FromParaBuffer(BUFFER_IO* pIO);//从VECTOR_PARA导出缓存中读入参数化矢量中的几何参数信息
	bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	//绝不能将函数CLDSVector::AddToConsoleObjBuf改为virtual function 否则其余地方一旦直接用'='进行CLDSVector赋值，将会导致混乱和异常　wjh-2017.12.11
	void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//void Serialize(CArchive &ar);
	bool ReverseDirection(bool bUpdateVector=true);	//调转里外方向,对于钢管上基准旋转方向时返回false
	void MirToVector(IModel *pModel,CLDSVector &mirVector,MIRMSG& mirinfo);
	char *GetDirectionDes();
	char *GetDirectionCmbItems();
	char *GetDirectionHelpStr();
	char *GetNormStyleDes();
	static char *GetNormStyleCmbItems();

	static void InitPropHashtable();						//初始化属性哈希表
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	UPDATE_PROPLISTITEM_FUNC(CLDSVector,propHashtable)		//定义属性项更新函数
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	static long GetPropID(char* propStr);			//得到属性ID
};
//钢结构中的表示实际工件（型钢，板材及螺栓）或用于附挂实际工件的节点
struct SOLIDMODEL_DISPLAY_PARA{
	BOOL bDisplayHole;
	double scale_of_user2scr;
	double sample_len;
	int smoothness;
};
struct HOLE_WALL{
	bool is_visible;
	DWORD hBolt;
	CLDSBolt* pBolt;	//句柄为hBolt的螺栓对象指针
	GEPOINT vStart,vEnd;
	CSolidBody *pSolidBody;
	struct HOLEITEM{
		GEPOINT ptS,ptE;
		long hRelaPart;
	}items[4];
	void SetKey(XHUINT64 key);
	HOLE_WALL();
	~HOLE_WALL();
public:
	BOOL UpdateSolid();
	BOOL Create3dSolidModel(double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	CSolidPart GetSolidPartObject();
};
struct IPartLibrary;
class CLsRef;
class CLsRefList;
//控制约束
struct CONSTRAINT{
protected:
	BYTE synchronize;	//同步状态
	UINT id;			//关联约束的标识Id
	//-1:表示因主控构件相应参数发生变化被动受控构件应进行自适应设计;0表示A=M;
	//>0:其余正值表示控制表达式字符串的标识Id
	long idExpression;
public:
	WORD wiAcceptPropId;//被动受控的属性或参数
	WORD wiMasterPropId;//主控构件的控制属性或参数
	long hAcceptObj;	//被动受控构件对象
	long hMasterObj;	//主控构件对象
	READONLY_PROPERTY(UINT,Id);	//已从Master->Accept进行了同步，避免死循环
	GET(Id){return id;}
	DECLARE_PROPERTY(CXhChar50,expression);
	void ClearSynchronize(){synchronize&=0x01;}
	DECLARE_PROPERTY(bool,dual);	//双向同步
	DECLARE_PROPERTY(bool,gone);	//已从Master->Accept进行了同步，避免死循环
	DECLARE_PROPERTY(bool,came);	//双向同步已从Accept->Master进行了同步，避免死循环
	bool IsOriginator(long hObj);	//hObj是为为当前约束对象的发起方
	void CopyProperty(CONSTRAINT* pCopyFrom);
public:
	static CHashList<CXhChar50> hashExpressions;
	void SetKey(DWORD keyid){id=keyid;}
	long DetachExprStr();
	CONSTRAINT();
	~CONSTRAINT();
	struct ACCEPTOR{ WORD wiAcceptPropId;CLDSDbObject* pAcceptObj;};
	bool Execute(IModel* pModel=NULL,CLDSDbObject* pMasterObj=NULL,CLDSDbObject* pAcceptObj=NULL,ACCEPTOR* lpAcceptor=NULL);
};
class CLDSPart : public CLDSDbObject
{
protected:
	bool is_data_modified;
	bool is_solid_modified;
	char *m_pMotherBodySolidBuf;	//用于判断当前影子构件实体数据是否与母构件实体数据是否版本匹配一致
	bool _bVirtualPart;
	bool _bLiveNeedSupplyPart;	//根据现场需要下料的构件（不受构件编号校审控制，如一些钢筋拉花，需现场按需下料） wjh-2016.9.4
protected:	//与公用属性对应的内存存储变量
	char _material;
	char _cQualityLevel;
	CXhChar16 sPartNo;//构件编号
public:
	struct HOLE{
		BYTE cFlag;				//描述螺栓孔加工工艺等特殊信息的标识细节，如低->高第1位为0x00表示冲孔，为0x01表示钻孔
		BYTE ciResidePlane;		//角钢:0.X肢;1.Y肢; 钢板0.基面;1.第一火曲面;2.第二火曲面
		short wWaistLen;		//腰圆孔腰长，腰长方向与装配坐标系的X轴同向
		short wBoltD;			//穿插螺栓直径
		float fHoleMargin;		//孔与螺栓间的总间隙，即孔径增大值
		CLDSBolt* pBolt;		//关联螺栓对象
		GEPOINT xLocalPosition;	//在宿主构件局部坐标系下的孔位置
		GEPOINT xMcsPosition;	//在全局框架型坐标系下的孔位置（模型坐标系）
		GEPOINT vHoleAxisVec;	//孔法线，一般为孔所在构件的正法线方向（模型坐标系）
		HOLE(CLsRef* pLsRef=NULL);
	public:	//孔径属性
		DECLARE_PROPERTY(float,fHoleD);
	};
protected:
	//通过CLDSPart::HOLE来汇总螺栓孔（含虚拟螺栓）及其余杂项孔（直接靠参数生成的工艺孔）集合　wjh-2017.11.2
	virtual UINT AddMiscellaneousHoles(IXhList<HOLE>* pListHoles);	//在GetHoleSet内调用,以添加除典型螺栓孔外的杂项孔
	virtual char get_ciLegQuad();
public:
	virtual UINT GetHoleSet(IXhList<HOLE>* pListHoles,bool bOnlyBoltHole=false);	//返回值为孔总数
public:	//公用属性
	char get_cMaterial();
	char set_cMaterial(char cBriefMaterialMark);
	char get_cQualityLevel();
	char set_cQualityLevel(char qualitylevel);
	bool get_blDefaultQualityLevel();
	bool set_blDefaultQualityLevel(bool blSetTrue);
	BYTE get_cbVirginQualityLevel();
	BYTE set_cbVirginQualityLevel(BYTE qualitylevel);
	__declspec(property(put=set_cMaterial,get=get_cMaterial)) char cMaterial;
	__declspec(property(put=set_cQualityLevel,get=get_cQualityLevel)) char cQualityLevel;
	//为了少改继承海涛旧代码，完全与cQualityLevel相同
	__declspec(property(put=set_cQualityLevel,get=get_cQualityLevel)) char m_cQualityLevel;
	//true,表示随材料表中相应材料的默认质量等级而定，false表示有指定的默认质量等级
	__declspec(property(put=set_blDefaultQualityLevel,get=get_blDefaultQualityLevel)) bool blDefaultQualityLevel;
	//包含材料原始质量等级设定的单字节属性（含是否随材料表默认质量等级更新变化）
	__declspec(property(put=set_cbVirginQualityLevel,get=get_cbVirginQualityLevel)) BYTE cbVirginQualityLevel;
	PROPERTY(double,Thick); 
	virtual GET(Thick){
		return 0;
	}
	virtual SET(Thick){
		return;
	}
	PROPERTY(bool,m_bVirtualPart);
	GET(m_bVirtualPart){
		if(IsAggregated())
			return true;
		else
			return _bVirtualPart;
	}
	SET(m_bVirtualPart){
		_bVirtualPart=value;
	}
	PROPERTY(bool,m_bLiveNeedSupplyPart);
	VIRTUAL_GET(m_bLiveNeedSupplyPart);
	VIRTUAL_SET(m_bLiveNeedSupplyPart);
	PROPERTY(long,m_hPartWeldParent);
	VIRTUAL_GET(m_hPartWeldParent);
	VIRTUAL_SET(m_hPartWeldParent);
	//属性sPartNo
	const char* GetPartNo();
	__declspec( property(get=GetPartNo)) const char* Label;
	virtual void SetPartNo(const char *part_no,bool bIncStdPart=false);
	bool ModifyPartNoSegI(SEGI newseg);	//更新构件编号中的段号部分
	virtual bool GetPropParam(long idObjProp,PROPVAL* pVal=NULL);
	virtual bool SetPropParam(long idObjProp,PROPVAL& propval);
public:
	static IPartLibrary* library;
	static int CustomerSerial;	//客户定制标识序号
	static bool INTERNAL_TEST_VERSION;
	static BOOL UPDATE_PARTNO_AND_SEG;
	static bool IsLaboratoryFuncEnabled();
	CLDSPart();
	~CLDSPart();
	bool m_bDisplayHole;		//显示孔及切割面
	UCS_STRU ucs;				//相对于塔绝对坐标系的相对坐标系(用于装配塔构件)
	unsigned long  m_uStatMatCoef;		//统材系数
	CSolidBody *pSolidBody;
	long _hPartWeldParent;		//焊接父构件，变为m_hPartWeldParent属性是为了调试方便 wjh-2017.11.16
	bool m_bUserSpecColor;		//用户指定构件颜色
	COLORREF crMaterial;		//构件颜色
public:	//影子构件的相关属性和方法
	//ancestor=true时返回始祖级构件，否则返回直接父构件（如对称时）
	CLDSPart* MotherPart(bool ancestor=true);
	virtual bool IsBelongTo(int iClassTypeId)const;
	virtual CLDSPart* NewCopy(bool bByShadowMode=false);
	virtual bool ShadowDesign();	//根据m_hAcs对应的装配坐标系更新ucs
	virtual void Offset(const double* offset);
	virtual bool IsRelaUsed(){return false;}
	virtual bool IsExternalPart() {return false;}
	//获取螺栓穿过角钢外皮的交点位置 wjh-2015.4.8
	static const int DATUM_PLANE	= 0;	//构件基准平面
	static const int BOLTBTM_PLANE	= 1;	//相对于螺栓Z(朝)向的通厚底面
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* cReserved=NULL,int iIntersPlane=0){return false;}
	//GetBoltIntersPos参数顺序不合理(作为可选输出参数应置于最后)，出于代码维护安全考虑以后应逐步用CalBoltIntersPos取代 wjh-2016.5.7
	virtual bool CalBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int iIntersPlane=0,int* cReserved=NULL){
		return GetBoltIntersPos(pBolt,inters_coords,cReserved,iIntersPlane);
	}
	//virtual void AddToConsoleObjBuf();
protected:	//与公用属性对应的内存存储变量
	BYTE _cFuncType;
public:
	static const BYTE FUNC_COMMON		= 0;	//普通构件
	static const BYTE FUNC_FOOTNAIL		= 1;	//脚钉，槽钢、角钢表示脚钉底座，钢板表示脚钉用途的钢板
	static const BYTE FUNC_REINFORCE	= 2;	//加(强)固，角钢表示加劲短角钢，钢板表示加强肋板
	static const BYTE FUNC_WIREPLATE	= 3;	//挂线板
	static const BYTE FUNC_STEPPLATE	= 4;	//爬梯上的踏板
	static const BYTE FUNC_SETUPPLATE	= 5;	//辅助安装挂板
	static const BYTE FUNC_LEGANGLE		= 6;	//塔腿角钢
	static const BYTE FUNC_FOOTPLATE	= 7;	//塔脚板
	static const BYTE FUNC_GASKET		= 8;	//垫板
	static const BYTE FUNC_BODYANGLE	= 9;	//塔身主材角钢
	static const BYTE FUNC_HEADANGLE	= 10;	//头部主材角钢
	PROPERTY(BYTE,m_cFuncType);
	VIRTUAL_GET(m_cFuncType){return _cFuncType;}
	VIRTUAL_SET(m_cFuncType){_cFuncType=value;}
	virtual BYTE FuncType(){return m_cFuncType;}
	virtual BYTE FilterFuncType(){return m_cFuncType;}	//过滤代孔脚钉为脚钉
public:
	void CloneBodyFrom(CSolidBody *pBody);//从指定实体克隆实体数据
	virtual UCS_STRU GetUnifiedACS(){return ucs;}
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart);
#endif
	virtual CSolidPart GetSolidPartObject();
	//itemType--辅助参数 -1表示不启用该参数 cMaterial--材质
	static COLORREF GetColor(int idClassType, long itemType,BYTE cMaterial);
	static void GetArcSimuPolyVertex(f3dArcLine* pArcLine, f3dPoint vertex_arr[], int slices);
	static void GetCircleSimuPolyVertex(double radius, f3dPoint vertex_arr[],int slices);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	virtual BOOL IsHasFootNail();
	virtual int GetLsCount(short d=0);
	virtual CLsRef* FindLsByHandle(THANDLE h){return NULL;}
	virtual CLsRefList *GetLsRefList(){return NULL;}
	virtual bool UpdateHoleWall(HOLE_WALL* pHoleWall){return false;}
	virtual int GatherHoleWalls(CHash64ListEx<HOLE_WALL>& hashHoles){return 0;}
	//根据构件上螺栓孔的先后顺序生螺栓句柄->孔索引标识(以1为基数)映射，返回螺栓孔数量 wjh-2016.1.05
	virtual long DispatchHoleIndexId(CHashList<long>* hashHoleIndexIdByBoltHandle);
	virtual CLsRef* AppendLsRef(CLsRef &ls_ref,BOOL bNearLsCheck=TRUE);
	virtual BOOL DeleteLsRef(THANDLE hBolt);
	virtual BOOL ResetSonBoltRayNo();
	virtual bool IsEqual(CLDSPart *pPart, BOOL bCompareBoltInfo,double epsilon=1.0,char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);
	struct PEER_BOLT{
		BYTE ciState;	//0.孔位与大小相同;1.孔位不同;2.孔径不同
		long hBaseBolt;	//基准螺栓句柄
		long hPeerBolt;	//对等螺栓句柄
		double difference;	//两螺栓差别值，ciState=2时，表示对等螺栓孔径与基准螺栓孔径间的差值
		void SetKey(DWORD key){hBaseBolt=key;}
		PEER_BOLT(long hKeyBolt=0){
			hBaseBolt=hKeyBolt;
			difference=hPeerBolt=ciState=0;
		}
	};
	class PEER_PART{
	public:
		BYTE ciEqualState;	//0.完全一致可工艺互换;1.
		BYTE cbDetailFlag;	//0.不输出任何细节;0x01:输出hashBoltsByBase; 0x02:输出hashBoltsByPeer;
		CLDSPart* pPart;
		long hPeerPart;		//伙伴构件的句柄
		DWORD m_hBlock;
		UCS_STRU csBase;	//基准构件对比一致或近似时的局部（装配）坐标系
		UCS_STRU csPeer;	//伙伴构件对比一致或近似时的局部（装配）坐标系
		CHashListEx<PEER_BOLT> hashBoltsByBase,hashBoltsByPeer;
	public:
		void SetKey(DWORD key){hPeerPart=key;}
		PEER_PART(){hPeerPart=cbDetailFlag=ciEqualState=0;m_hBlock=0;pPart=NULL;}
		void CopyFrom(PEER_PART& peerpart); 
	};
	virtual bool CompareLabelPart(CLDSPart *pPeerPart,MIRMSG* pMirmsg=NULL,BOOL bCmpls=TRUE,PEER_PART *pPeerInfo=NULL,double epsilon=1.0){return false;}
#endif
	virtual char* GetPartTypeName();
	virtual int GetSpecification(char *spec,BOOL bIncMat=TRUE){return 0;}
	//bAnalysisMode=true时根据size.idClassType判断，否则根据GetClassTypeId判断
	bool IsAngle(BOOL bAnalysisMode=false);
	bool IsTube(BOOL bAnalysisMode=false);	//简化构件类型判断 wht 11-05-19
	virtual bool IsBolt(){return false;}
	virtual double GetWidth(){return 0;}
	virtual double GetThick(){return 0;}
	virtual double GetThick2(){return 0;}
	virtual double GetHeight(){return 0;}
	virtual double GetLength(){return 0;}
	virtual double GetWeight(BOOL bSolidLength=TRUE){return 0;}
	virtual BOOL Create3dSolidModel(SOLIDMODEL_DISPLAY_PARA displayPara);	//此函数目的是减少以下函数的参数过长问题，暂未启用 wjh-2011.7.18
	virtual BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	//当前构件是否可以在指定绘图坐标系(负Z轴为视线方向)下遮掩住另一构件
	virtual bool IsCanHide(CLDSPart* pPart,const double* dcs_norm){return false;}
	BOOL CreateShadowPart3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0){;}
	virtual void ToXmlFile(FILE* fp,DWORD schema){;}
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0){;}
	virtual bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	virtual void UpdatePartNoBySegI();
	virtual void SetSolidModified(BOOL bRenderSolid=TRUE){is_solid_modified=(bRenderSolid!=0);}
	virtual void SetModified(BOOL bTrue=TRUE,BOOL bRenderSolid=TRUE);
	BOOL IsModified(){return is_data_modified;}
	BOOL IsSolidModified(){return is_solid_modified;}
	//对于杆件0.完全对称一致;1.除实体坐标外对称一致;2.拾取点处射线同向对称;3.拾取点处射线异向对称
	virtual BYTE MirSimilarGradeWith(CLDSPart* pPart,const MIRMSG& msg,f3dPoint *pPickPos=NULL);
	//pPickPos为指定对称拾取点,主要用于杆件拾取点部分对称,有始终端节点的杆件应输入单线模型坐标,否则应输入实体坐标 wjh-2011.8.9
	//bCheckProfile在获取对称钢板时用来控制是否进行外形比对 wht 11-07-22
	virtual CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsPart() const{return true;}
	virtual bool IsNode() const{return false;}
	virtual bool IsLinePart() const{return false;}
	virtual bool IsArcPart() const{return false;}
	virtual bool IsPolyPart() const{return false;}
	virtual bool IsPlate() const{return false;}
	virtual bool IsExternalPart() const{return false;}
	virtual bool IntelliRecogWeldLine(){return false;}
	virtual bool IsWeldWith(double* lineStartCoord,double* lineEndCoord){return false;}
	virtual bool IsWeldWith(f3dArcLine& arcline){return false;}
	virtual int ToPPIBuffer(CBuffer &buffer,DWORD dwInheritFlag=0);	//得到构件工艺信息中性文件buffer
	virtual int WriteProcessBuffer(CBuffer &buffer,bool bSaveSubSerial=false,long version=0){ return 0;}		//得到构件工艺信息buffer
	virtual void ResetRelativeObjs();	//重新设置关联对象
	//属性管理
	/*static UINT crShortAngle,crAngleEdge,crPole,crAngleWingX,crAngleWingY,crBolt12,crBolt16,crBolt20,
		crBolt24,crBolt27,crBolt30,crBolt36,crBolt39,crBolt42,crBolt48,
		crOtherBolt,crPlate,crCutFaceOrPolyJoint,crStdPart;*/
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	static void InitPartPropHashtable();
	UPDATE_PROPLISTITEM_FUNC(CLDSPart,propHashtable) //定义属性项更新函数
	static int GetPropStr(long id,char *propStr);		//得到属性字符串
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
};
typedef CLDSPart* CLDSPartPtr;
class CPartTempUcsLife{
	UCS_STRU oldUCS;
	CLDSPart *m_pPart;
public:
	CPartTempUcsLife(CLDSPart *pPart,UCS_STRU temp_ucs)
	{
		m_pPart=pPart;
		if(m_pPart)
		{
			oldUCS=m_pPart->ucs;
			m_pPart->ucs=temp_ucs;
		}
	}
	~CPartTempUcsLife()
	{
		if(m_pPart)
			m_pPart->ucs=oldUCS;
	}
};
//附加构件材料,只用于材料统计
class CSupplementPart : public CLDSPart {
public:
	CXhChar16 guige;	//规格
	double length;		//长度
	double piece_weight;//单件重
	int nQuantity;		//单基数量
	int part_type;		//构件类型:0.角钢;1.钢板2.其它
	BOOL bLegPart;		//腿部构件（统材时具有排它性，只在一个本体中进行统计）;
	CSupplementPart();
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//void Serialize(CArchive &ar);
};
// 螺栓
struct DIANQUAN_STRU{
	short _wholethick;//垫圈厚(含是否自动匹配垫圈厚度的信息)
public:
	static const int DIANBAN  = 0;
	static const int DIANQUAN = 1;
	DECLARE_PROPERTY(bool,AutoMatchThick);	//重新设计螺栓时是否自动根据螺栓直径匹配垫圈厚度与数量
	short get_thick();
	short set_thick(short _thickvalue);
	//垫圈厚度获取与设置,thick<256;
	__declspec(property(put=set_thick,get=get_thick)) short thick;
	short N;	//垫圈数量
	double offset;	//从螺栓基点沿轴向的偏移量
};
struct DIANBAN_STRU{
	BOOL bOriginal;		//是否为正本
	long hDianBan;		//垫板
	short feature;		//备用
};
class CLDSLine : public CLDSGeObject {
public:
	CLDSLine();
	/*BYTE m_cDefineType;	//0.指定基准线端点;1.指定杆件特征线(钢管轴线或角钢楞线与offset_g配合使用)2.角钢X肢心线3.角钢Y肢心线
	union LINE_PARA{
		THANDLE hPointArr[2];
		CLDSPoint arrPoint[2];
		struct KEY_LINE{
			long m_hRod;
			long offset_nx;	//m_cDefineType＝1且指定杆件为角钢时，表示沿X肢法向偏移量
			long offset_ny;	//m_cDefineType＝1且指定杆件为角钢时，表示沿Y肢法向偏移量
		}keyline;
		struct WING_LINE{
			long m_hAngle;
			long offset_n;	//m_cDefineType＝2且指定杆件为角钢时，表示沿Y肢偏移量
			long offset_g;	//m_cDefineType＝2且指定杆件为角钢时，表示沿X肢偏移量
		}wingline;
	}des_para;*/
	HANDLEPOINT hPointArr[2];
	long  iNo;
	char description[MAX_TEMP_CHAR_100+1];	//对基准线的文字描述
	GEPOINT start_pos,end_pos;
	BOOL UpdateLine(IModel *pTower);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	bool ReplaceReferenceHandle(CHandleReplacer& replacer);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	void CleanInvalidObjRef(IModel *pModel=NULL);
	bool IsSameWith(const double* pxLineOrg,const double* pvLineVec);
	//void Serialize(CArchive &ar);
};
class CLDSPlane : public CLDSGeObject {
	GEPOINT base_pos,work_norm;
public:
	CLDSPlane();
	long  iNo;
	char description[MAX_TEMP_CHAR_100+1];	//对基准面的文字描述
	HANDLEPOINT  hPointArr[3];	//最高位为0x80000000时表示直接输入的是节点句柄
	BOOL UpdatePlane(IModel *pTower,CLDSArcLift* pLift=NULL,bool bReverseLift=false);
	BOOL IsValidPlane();
	f3dPoint GetWorkNorm();
	f3dPoint GetBasePos();
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	bool IsSameWith(const double* pxPlaneOrg,const double* pvPlaneStdNormal);
	//工作平面定位基点
	GEPOINT get_xPickPoint(){return base_pos;}
	__declspec(property(get=get_xPickPoint)) GEPOINT xPickPoint;
	//工作平面法线
	GEPOINT get_vStdNormal(){return work_norm;}
	__declspec(property(get=get_vStdNormal)) GEPOINT vStdNormal;
	//void Serialize(CArchive &ar);
};
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	//螺栓位置设计参数类型
enum LSPOS_STYLE{LS_POS_NO_PARA,LS_POS_AXIS_INTERS,LS_POS_JG_BER,LS_POS_NODE_PRJ,LS_POS_LS,LS_POS_NODE_FACEOFFSET,LS_POS_UCS};
LSPOS_STYLE TranslateLsPosStyleFromI(int i);
	//螺栓位置设计参数
class DESIGN_LS_POS
{
public:
	CLDSPoint datumPoint;
	THANDLE hPart;	//定位基准构件句柄，用以决定自定位基点的两个偏移方向（纵向和横向或X/Y轴）
	union{
		BYTE offset_wing;	//0.X肢;1.Y肢（仅对基准构件是角钢时有效）
		BYTE reside_plane;	//0.所在钢板面号(0表示基面;1.表示第一个火曲面;2.表示第二个火曲面) wjh-2015.2.7
	};
	static const BYTE GLOBAL_COORD = 0;
	static const BYTE LOCAL_COORD  = 1;
	BYTE cLocationStyle;	//0:GLOBAL_COORD基点坐标为绝对坐标;1:LOCAL_COORD基点坐标为基准构件的相对坐标
	//union{
		//struct{
			BYTE direction;	//0.基准角钢方向:始->终;1.终->始（仅对基准构件是角钢时有效）
			BYTE m_biWingOffsetStyle;	//0.g;1.g1;2.g2;3.g3;4.自定义
			double wing_offset_dist,len_offset_dist;//肢向偏移值（Y）;纵向偏移值（X）;
		//}angle;
		//struct{
			BYTE cStartEdge;		//环钢管定位时的,角度起始边 0:基准边;1:焊道线
			double offset_angle;	//螺栓若为环行布置 则该变量记录螺栓基于（基准构件所依附的）基准钢管的偏移角度（以钢管焊道线或基准边为基准）
			double R;	//R<=0表示螺栓非环形布置 螺栓为环形布置时 该变量记录螺栓所在圆的半径
		//}tube;
	//}location;
	CDesignThickPara norm_offset;	//法向偏移量字符串
	DESIGN_LS_POS();
	//void Serialize(CArchive &ar);
	void FromBuffer(CBuffer &buffer,long version,long doc_type);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
};
class CLDSBolt;
class CLDSLineAngle;
//螺栓设计参数
class CDesignLsPara
{
public:
	long iNo;
  	long d;	     			// 螺栓直径.
	SEGI iSeg;				// 螺栓段号
	char grade[5];			// 螺栓级别
  	CParaThick L0;	// 螺栓通过厚度.
	double hole_d_increment;//螺栓孔直径增量
	bool m_bVirtualPart;	//仅孔无螺栓
	DWORD dwRayNo;			// 所在的射线索引号，出图时用
	CLDSVector des_work_norm;	//螺栓工作法线设计数据
	DESIGN_LS_POS des_base_pos;	//螺栓基准点初始坐标(不考虑纵向移动)设计数据
	CDesignThickPara pad_thick;	//螺栓垫(垫圈或垫板)
	CDesignThickPara pad_offset;//螺栓垫自基点偏移量
	CLDSBolt *pMapLs;			// 临时用,不存储
	int nLjJg;				// 临时用,不存储,用以标明，此螺栓连接了几根角钢
	CDesignLsPara();
	bool DesignLs(CLDSBolt *pLs);
	void FromBuffer(CBuffer &buffer);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void CloneTo(CDesignLsPara &ls_para);
};
//螺栓垫板设计参数
class CDesignPadPlankPara{
public:
	long iNo;
	char sPartNo[MAX_CHAR_GUIGE_16+1];// 垫板构件编号
  	char ls_str[MAX_TEMP_CHAR_200+1];// 螺栓范围字符串
	long hBaseJg;			// 所贴基准角钢
	long hCutRefJg;			// 切角时的基准角钢,一般是连接基准角钢
  	CDesignThickPara thick;	// 垫板厚度设计
	CDesignPadPlankPara(){memset(this,0,sizeof(CDesignPadPlankPara));}
	void FromBuffer(CBuffer &buffer,long version=NULL);
	void ToBuffer(CBuffer &buffer);
	void ToXmlFile(FILE* fp,DWORD schema);
	//void Serialize(CArchive &ar);
};
struct ANGLE_SPACE_FLAG{
	BYTE cbFlag;
	static const BYTE SPACE_UDF	   =0;	//指定数值边距
	static const BYTE SPACE_BOLTEND=1;	//标准螺栓端距
	static const BYTE SPACE_TOEDGE =2;	//延伸到边缘
	static const BYTE SPACE_TOBER  =3;	//延伸到楞线边缘-肢厚度

	//static const BYTE SPACE_UDF	  =0;	//指定数值边距
	//static const BYTE SPACE_BOLTEND =1;	//标准螺栓端距
	static const BYTE SPACE_ALIGN	  =2;	//标准螺栓端距+半个螺栓间距（用于两肢连板对齐)
public:
	ANGLE_SPACE_FLAG(BYTE flag=0x05){cbFlag=flag;}	//Ber::SPACE_BOLTEND|Wing::SPACE_BOLTEND
	operator BYTE(){return cbFlag;}
	BYTE StyleOfBerSpace();
	BYTE StyleOfWingSpace();
	BYTE StyleOfEndSpace();
	BYTE SetBerSpaceStyle(BYTE cbSpaceStyle);
	BYTE SetWingSpaceStyle(BYTE cbSpaceStyle);
	BYTE SetEndSpaceStyle(BYTE cbSpaceStyle);
};
class CDesignLjPartPara{
	BYTE m_ciAsynDualEdgeEndSpace;		//0.同步;1.异步 是否已经设定了双侧螺栓端距时的始端端距,未设定时按end_space考虑
	//end_space相当于currEndSpace,表示当前连接端的螺栓端距或双侧螺栓端距时的杆件终端侧螺栓端距 wjh-2017.11.23
	double dualEdgeStartSideEndSpace;	//双侧螺栓时的杆件始端侧螺栓端距
	BYTE m_ciAsynDualEdgeBerSpace;		//0.同步;1.异步 是否已经设定了双侧螺栓端距时的始端楞线侧边距,未设定时按ber_space考虑
	//ber_space相当于currBerSpace,表示当前连接端的楞线侧螺栓边距或双侧螺栓端距时的杆件终端楞线侧螺栓边距 wjh-2017.11.23
	double dualEdgeStartSideBerSpace;	//双侧螺栓时的杆件始端楞线侧螺栓边距
	BYTE m_ciAsynDualEdgeWingSpace;		//0.同步;1.异步 是否已经设定了双侧螺栓端距时的始端肢翼侧边距,未设定时按wing_space考虑
	//wing_space相当于currWingSpace,表示当前连接端的螺栓端距或双侧螺栓端距时的杆件终端肢翼侧螺栓边距 wjh-2017.11.23
	double dualEdgeStartSideWingSpace;	//双侧螺栓时的杆件始端肢翼侧螺栓边距
public:
	CDesignLjPartPara();
	void AddLsNo(int iLsNo);
	double CalOddment();
	//void Serialize(CArchive &ar);
public:
	struct DESIGNLJ_ANGLEPARA
	{
		BYTE cur_wing_x0_y1;	//当前连接肢
		ANGLE_SPACE_FLAG cbSpaceFlag;		//
		BOOL bEndLjJg;			//端连接角钢
		BOOL bTwoEdge;			//是否螺栓线两侧都有外轮廓点
	}angle;
	BOOL m_bAdjustLjEndPos;		//对称钢板时是否需要调整射线杆件连接端摆放位置，主要在对称共用板时使用 wht 10-02-26 
	long m_nClassTypeId;
	//int m_iAffectTubeEndPlateStyle;
	long hPart;//,hRefPart1,hRefPart2;	//当前钢管句柄,正负头校验参照构件
	short start0_end1,iFaceNo;			//设计始端0，终端1;	所在连接面
	double end_space,wing_space,ber_space;	//定义板外形时的参数
	DECLARE_PROPERTY(double,startSideEndSpace);
	DECLARE_PROPERTY(double,endSideEndSpace);
	DECLARE_PROPERTY(double,startSideBerSpace);
	DECLARE_PROPERTY(double,endSideBerSpace);
	DECLARE_PROPERTY(double,startSideWingSpace);
	DECLARE_PROPERTY(double,endSideWingSpace);
	DECLARE_PROPERTY(BYTE,ciAsynDualEdgeEndSpace);
	DECLARE_PROPERTY(BYTE,ciAsynDualEdgeBerSpace);
	DECLARE_PROPERTY(BYTE,ciAsynDualEdgeWingSpace);
	CLDSLinePart* pCurLinePart;		//临时用,不存储
	void SetKey(DWORD key){hPart=key;}
	//BOOL bBasePart;				//连接基准角钢
	//CDesignThickPara norm_offset,datum_norm_offset;	//法向偏移量
	//long nCollideSpace;			//楞点间隙设计参数
	//char ls_str[MAX_TEMP_CHAR_100+1];	//螺栓字符串
	//short odd_cal_style;		//0.表示根据碰撞参照杆件计算;1.根据螺栓位置计算;2.表示不计算正负头
	//管理连接杆件属性
	DECLARE_PROP_FUNC(CDesignLjPartPara);
	int GetPropValueStr(long id, char *valueStr, UINT nMaxStrBufLen = 100);	//根据属性ID得到属性值
	static void InitViewPropHashtable();
};

class CLDSPlate;
class CObjNoGroup;
class CDesignLjPara{
public:
	BOOL m_bEnableFlexibleDesign;	//启用参数化柔性设计（主要对于老版本文件无设计信息用）
	long m_hBaseNode,m_hBasePart;	//基准节点,基准杆件,双基准第二基准杆件
	short iFaceType;			//面数类型，1-3，表示相应的面数的普通板，4表示折叠板，5表示螺栓垫板
	short iProfileStyle0123;	//外轮廓定义方式,0:标准外形1:定制外形2:包络外形3:自定义外形
	long  iWeldLineStyle;		//0.自由计算；1.指定焊缝线；2:对齐焊缝线
	double weld_start_high,weld_end_high,weld_width;	//焊接连接时的焊线起始高终止高及焊区宽
	UINT uiStyleFlag;
	DECLARE_PROPERTY(bool,m_bWeldToDatumJg);		//单面板是否焊接到基准角钢 wht 10-06-06
	//---VVV---以下临时只为读LDS文件设定,以后可能要去掉wjh-08-11-11)--
	//CDesignThickPara thick;		//厚度(临时只为读LDS文件设定,以后可能要去掉wjh-08-11-11)
	//char ls_str[MAX_TEMP_CHAR_200+1];	//螺栓字符串(临时只为读LDS文件设定,以后可能要去掉wjh-08-11-11)
	//ATOM_LIST<CDesignLsPara>  ls_list;
	//ATOM_LIST<CDesignPadPlankPara> padplank_list;
	//CObjNoGroup lsnogroup,padplanknogroup;
	//---^^^-------------------------------------------
	bool m_bSelfCorrectBendline;	//自动正位制弯线，以保证制弯线为两面交线 wjh-2016.5.14
	CLDSPoint origin,huoquline_start[2],huoquline_end[2];
	CLDSVector norm,huoqufacenorm[2];
	CHashListEx<CDesignLjPartPara>partList;
	fPtList additional_vertex;
	CDesignLjPara();
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//void Serialize(CArchive &ar);
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	//克隆设计参数 wht 09-12-30
	void CloneTo(CDesignLjPara &designInfo);
	//根据输入的杆件句柄查找相应的连接杆件设计参数 wht 10-01-05
	CDesignLjPartPara *FromHandle(long hPart);
};
class CLDSSphere : public CLDSPart{
public:
	double D;	//球外直径(mm)
	double d;	//球内直径(mm)
	double thick;	//球厚度 该变量不存储
	long hDatumNode; //基准节点句柄
	CLDSSphere();
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
	CSolidPart GetSolidPartObject();
	BOOL IsModified(){return is_data_modified;}
	void CopyProperty(CLDSDbObject *pObj);
	bool IsEqual(CLDSSphere* pMirSphere,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	//属性管理
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSSphere,propHashtable)   //定义属性项更新函数
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	static void InitSpherePropHashtable();
	static int GetPropStr(long id,char *propStr);		//得到属性字符串
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态

	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
};
//////////////////////////////////////////////////////////////////////////////////
class CLsRef;
class CLDSBolt : public CLDSPart {
protected:
  	short _d;	     	// 螺栓直径.int->short(19.07.16)       省2 Bytes
  	short _L;           // 螺栓有效长度.double->short(19.07.16)省6 Bytes
	short _L0;			// 螺栓通过厚度.double->short(19.07.16)省6 Bytes
	short _siInitStdL;      //恢复变为脚钉的螺栓有效长度.double->short(19.07.16)省6 Bytes
	CParaThick designL0;	//螺栓通厚的设计参数
	bool m_bNeedUpdateL0;	//是否需要更新L0.BOOL->bool(19.07.16)省3 Bytes
	bool m_bCalL0IncGapByInters;	//根据新求交算法计算螺栓通厚（包含中间的缺失垫板间隙值） wjh-2019.7.11
	short _hFamily;		//从属螺栓规格系列.long->short(19.07.16)省2 Bytes
	char grade[5];		//螺栓级别
	bool m_bAggregated;	//true:此螺栓为装配部件影射螺栓，同时已被聚合到部件引用中，实体显示时无须显示
	float m_sfHoleDIncrement;	//hole_d_increment螺栓孔直径增量double->float(19.07.16)省4 Bytes
public:	//公共属性
	static bool m_bEnableActiveHeightFilterOnL0InternalCal;	//get_L0()内部调用通厚计算时是否启用当前激活呼高验证，默认为true
	//L0计算模式:0.表示按穿过构件厚度累加计算;1.表示考虑穿过点间距离值复核;
	//2.视螺栓本身m_bCalL0IncGapByInters属性计算
	static const char L0CAL_BY_SUMM_THICK = 0;
	static const char L0CAL_INC_GAP_THICK = 1;
	static const char L0CAL_BY_BOLTSELF	= 2;
	static char BOLTL0_PREFER_MODE;	//螺栓默认的通厚计算规则类型，只能取值0或1
	static char BOLTL0_CALMODE;		//螺栓当前通厚实际计算规则，取值0~2 wjh-2019.7.23
	//根据新求交算法计算螺栓通厚（包含中间的缺失垫板间隙值） wjh-2019.7.11
	bool get_blCalL0IncGapByInters();
	bool set_blCalL0IncGapByInters(bool blVal);
	__declspec(property(put = set_blCalL0IncGapByInters, get = get_blCalL0IncGapByInters)) bool blCalL0IncGapByInters;
	bool m_bDisableSimpleLocateMode;	//是否阻止优先启用简单定位显示模式
	DECLARE_READONLY_PROPERTY(bool,m_bCanSimpleLocateMode);
	long get_m_hFamily();
	long set_m_hFamily(long value);
	__declspec(property(put = set_m_hFamily, get = get_m_hFamily)) long m_hFamily;
	char* Grade(){
		CLDSBolt* pMotherObj=(CLDSBolt*)MotherObject();
		if(pMotherObj==this)
			return grade;
		else
			return pMotherObj->Grade();
	}
	char* SetGrade(char* boltgrade)
	{
		CLDSBolt* pMotherObj=(CLDSBolt*)MotherObject();
		grade[4]=0;
		strncpy(grade,boltgrade,4);
		if(pMotherObj!=this)
			pMotherObj->SetGrade(grade);
		return grade;
	}
public:
	static const DWORD TWO_CAP;		//双帽螺栓标识位
	static const DWORD ANTI_THEFT;	//防盗螺栓标识位
	static const DWORD ANTI_LOOSE;	//含防松垫圈标识位
	static const DWORD FOOT_NAIL;	//脚钉螺栓标识位
	static const DWORD REVERSE_NORM;//调转螺栓朝向标识位
	char statSegStr[24];	// 统计螺栓时的归属段号字符串（螺栓可能存在多段累计统计的情况） wjh-2011.9.28
	double get_hole_d_increment()const;
	double set_hole_d_increment(double value);
	//螺栓孔直径增量
	__declspec(property(put = set_hole_d_increment,get= get_hole_d_increment)) double hole_d_increment;
	DIANQUAN_STRU DianQuan;	// 螺栓垫(垫圈或垫板)
	DIANBAN_STRU DianBan;	// 目前仅为读取LDS文件用
	BITFLAG m_dwFlag;		// 有特殊要求的标识位，如ANTITHEFT_BOLT(1),ANTILOOSE_BOLT(2),FOOTNAIL_BOLT(4)
	DWORD dwRayNo;			// 所在的射线索引号，出图时用
	CLDSVector des_work_norm;	//螺栓工作法线设计数据
	DESIGN_LS_POS des_base_pos;	//螺栓基准点初始坐标(不考虑纵向移动)设计数据
#ifdef __DRAWING_CONTEXT_
	GEPOINT baseRayVec;	//与des_base_pos.hPart对应的射线方向
	struct RELA_RAYPART{
		THANDLE hRelaPart;//关联构件句柄
		DWORD dwRayNo;
		GEPOINT rayVec;
	}relaRayArr[4];	//相应的射线号
	BOOL SetRelaPartRayNo(long hPart,DWORD dwRayNo,const double* rayVec);
#endif
public:
	double GetWeight(BOOL bSolidLength=TRUE);
	int ValidCheck();
	CLDSBolt(IModel *pTower=NULL);
	~CLDSBolt();
	BITFLAG get_dwSafeFlag();
	__declspec(property(get=get_dwSafeFlag)) BITFLAG dwSafeFlag;	//相对m_dwFlag考虑了IsAntiTheft和IsAntiLoose属性 wjh-2019.3.28
	virtual bool IsBolt(){return true;}
	static const BYTE FUNC_COMMON	= 0;//0.连接螺栓
	static const BYTE FUNC_FOOTNAIL	= 1;//1.脚钉，槽钢、角钢表示脚钉底座，钢板表示脚钉用途的钢板
	static const BYTE FUNC_WIREHOLE	= 2;//2.挂线孔;
	static const BYTE FUNC_EARTHHOLE= 3;//3.接地孔;
	static const BYTE FUNC_SETUPHOLE= 4;//4.装配孔;
	static const BYTE FUNC_BRANDHOLE= 5;//5.挂牌孔;
	static const BYTE FUNC_WATERHOLE= 6;//6.引流孔
	static const BYTE FUNC_FOOTNAIL2= 7;//7.代孔脚钉
	static const BYTE FUNC_BASEHOLE = 8;//8.地脚螺栓孔

	virtual BYTE FuncType();
	PROPERTY(BYTE,m_cFuncType);
	VIRTUAL_GET(m_cFuncType){return FuncType();}
	VIRTUAL_SET(m_cFuncType);
	virtual BYTE FilterFuncType();	//过滤代孔脚钉为脚钉
	virtual bool IsRelaUsed();
	virtual bool IsAggregated();
	virtual void SetAggregateState(bool aggregated=true){m_bAggregated=aggregated;}	//设置聚合状态
	virtual bool IsBelongTo(int iClassTypeId)const;
	BOOL IsStatSeg(SEGI iSeg);
	int GetStatSegArr(SEGI seg_arr[],int maxSegN);
	//ULONG AddRef();		//增加一个引用
	//ULONG Release();	//删除一个引用
	//ULONG GetRefCounts();
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	f3dPoint set_norm(const double* norm);
	f3dPoint get_norm() const;
	__declspec( property(put=set_norm,get=get_norm)) f3dPoint Normal;
	BOOL correct_pos() ;
	void correct_worknorm();
	BOOL IsLocalCoordLocation(CLDSPart* pDatumPart=NULL) const;	//螺栓是否以构件上相对坐标为基准定位
	BOOL IsCanLocalCoordLocation(CLDSPart* pDatumPart=NULL) const;	//螺栓是否以构件上相对坐标为基准定位
	virtual bool ShadowDesign();			//根据m_xDesOrgInfo、m_xMirAxisInfo及normOffset自动更新ucs
	short CalL0();
	short get_L0(BOOL bNeedUpdateL0=FALSE);
	CParaThick *GetL0ThickPara();	
	THICK_ITEM GetFirstL0ThickItem(){return designL0.GetFirstItem();}
	THICK_ITEM GetNextL0ThickItem(){return designL0.GetNextItem();}
	//double set_L0(double ls_L0);	//清空原有螺栓通厚参数,直接设置螺栓通厚
	void EmptyL0DesignPara();	//清空原有螺栓通厚参数
	void GetL0KeyStr(char *key_str,UINT nMaxStrBuffLen=200);//得到通厚字符串
	void SetL0KeyStr(const char *key_str);//设置通厚字符串
	void SetL0ThickPara(CParaThick thick);//设置通厚字符串
	void AddL0Thick(long val,BOOL bHandle=FALSE,BOOL bSamePartFilter=FALSE);//增加螺栓通厚
	int set_d(int d);
	int get_d();
	__declspec( property(put=set_d,get=get_d))int d;
	double add_L(double addL);	//通厚增加值
	double set_L(double L);
	double get_L();
	__declspec( property(put=set_L,get=get_L))double L;
	double get_CurrL0() { return get_L0(false); }
	__declspec(property(get = get_CurrL0))double L0;
	double get_dfLe();
	__declspec(property(get = get_dfLe))double dfLe;	//实际从贴合面节点起算的出露长
	long get_oldL() const;
	long set_oldL(long ls_L);
	__declspec(property(put=set_oldL,get=get_oldL)) long liInitL;	//代孔为脚钉前的普通螺栓的有效长，mm
	bool restore_oldL();
	bool PushInitialStdBoltL();	//由普通螺栓升级为代孔脚钉时，存档当前初始标准螺栓有效长度 wjh-2019.8.21
	bool PopInitialStdBoltL();
	bool CalGuigeAuto(BOOL bCalShadowBoltGuiGe=FALSE,BOOL bIncGrade=FALSE);
	bool IsFootNail();	//是否属于脚钉
	bool IsTwoCap();	//是否为双帽螺栓
	bool IsAntiTheft();	//是否为防盗螺栓
	bool IsAntiLoose();	//是否有防松垫圈
	UCS_STRU GetUCS();
	CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
protected:
	BOOL CreateClimbNailSolidModel(fBody* pBody,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
public:
    BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void CalGasketOffset(PARTSET& partset);	//计算垫圈偏移量
	BOOL IsNeedGasket(PARTSET& partset);	//判断螺栓是否需要垫圈
	int CalPadPara(double dist);
	int CorrectBoltZnL0Para();
	static int CalPadPara(int d,double dist,DIANQUAN_STRU* pDianQuanInfo=NULL);	//0.垫板 1.垫圈
	static int CorrectBoltZnL0Para(CLDSBolt** ppBolts,UINT count=1);	//自动修正螺栓Z向穿插位置及通厚
	//blPushLAsInitStdBoltL=true转换螺栓为脚钉之前，初始化当前长度为螺栓的标准螺栓初始有效长度 wjh-2019.8.21
	void CopyProperty(CLDSBolt *pBolt,bool blPushLAsInitStdBoltL=true);
	CLsRef GetLsRef();
	void CloneTo(CLDSBolt &bolt);
	virtual bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	static CXhChar50 EmendZOffsetStr(CLDSBolt* pBolt,PARTSET& partset);
	static void CreateBoltSolidBody(fBody* pBody,int d,double L,double D,double H);
	static void CreateBoltCapSolidBody(fBody* pBody,int d,double D,double H2);
	//属性管理
	DECLARE_PROP_FUNC(CLDSBolt);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值
	static void InitBoltPropHashtable();
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//得到构件属性的默认只读状态
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//得到构件属性的默认只读状态
	virtual void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	virtual void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
public:
	//同步属性
	static MEMORY_BLOCK MPCL;	//Mirror Property Control List
	static PROPLIST_TYPE propMirSync;
	static void InitPropMirBitsMap();
	virtual PROPLIST_TYPE* GetSyncPropList(){return &propMirSync;}
	virtual MEMORY_BLOCK GetDefaultMPCL(){return MPCL;}
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部位置对称无关的属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
public:	//记录螺栓变动引起的其余构件的变动 wjh-2017.11.22
	static const BYTE CLEAR_BUFF_OF_RELA_PARTS	=0x01;
	static const BYTE DISABLE_RECORD_RELA_PARTS =0x02;
	static BYTE GetMovePartsUpdateFlag();
	static BYTE SetMovePartsUpdateFlag(BYTE cbFlag=0x01);
	static void ClearUpdateMovePartsBuff();
	static CLDSPart* EnumFirstRelaMovePart();
	static CLDSPart* EnumNextRelaMovePart();
};
typedef CLDSBolt* CLDSBoltPtr;
class CLDSGeneralPlate;
class CLDSAnchorBolt : public CLDSBolt
{
public:
	long m_hRelaVirtualBolt;	//记录原始底脚板上的关联虚拟螺栓句柄
	struct WASHER{
		WORD wiWidth,wiThick;
		float fHoleIncrement;	//垫板孔径相对地脚螺栓直径的增大值
		long hWasherPlate;		//关联垫板的模型句柄
	}washer;
	struct BASEPLATE{
		WORD wiBasePlateThick;	//塔脚底座板厚度
		long hBasePlate;		//关联塔脚底座板句柄(有值时优先于wiBasePlateThick计算厚度)
	}baseplate;
	struct FOUNDATION{
		BYTE ciHeightSerial,ciLegSerial,ciLegQuad,ciBoltSerial;
		FOUNDATION();
	}Foundation;	//与归属基础的关系
	WORD get_wBasePlateThick();
	__declspec(property(get=get_wBasePlateThick)) WORD wBasePlateThick;
	BYTE m_ciAnchorType;//地脚螺栓类型;0.表示普通双头型地脚螺栓;1.L型;2.J型;3.棘爪型;4.T型
	float m_fNutH;		//螺母高度
	float m_fNutTaper;	//螺母端面过渡圆锥面的坡度h/w;
	float m_fNutDe;	//螺母顶点到顶点距离
	float m_fNutDs;	//螺母边到边距离
	float m_fLe;	//出露长
	float m_fLr;	//基础顶面之下的埋深(root)
	//ciQuad定义:	B(4)  C(3)	ciBoltSerial定义(俯视):	2  1
	//				A(2)  D(1)							3  4
public:
	CLDSAnchorBolt(IModel *pModel=NULL);
	CLDSAnchorBolt(const ANCHOR_BOLT& anchorbolt);
	~CLDSAnchorBolt();
	virtual bool IsBolt(){return true;}
	void SetAnchorInfo(const ANCHOR_BOLT* anchorbolt);
	void InitDefaultAnchorInfo();	//为了显示效果对于一些非标准地脚螺栓进行参数初始化 wjh-2019.5.9
	void UpdateByBasePlate(CLDSGeneralPlate* pBasePlate);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	//属性栏
	DECLARE_PROP_FUNC(CLDSAnchorBolt);
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//检测属性值是否相等
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值
public:
	static BOOL CreateAnchorBoltSolidBody(fBody* pBody,const ANCHOR_BOLT* pAnchorBoltPara);
	static BOOL CreateAnchorBoltCapSolidBody(fBody* pBody,const ANCHOR_BOLT* pAnchorBoltPara);
    BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
};
class CLDSBoltU : public CLDSPart{
public:
	bool m_bExternalFittingPart;
	long d;		//螺栓单肢直径
	long m;		//U型直径
	long b;		//螺痕的长度
	long a;		//单肢的长度
	long R;		//U型半径
	long m_uDisplaySlices;	//U型螺栓实体显示时的切片数，默认为6
	CLDSBoltU();
	virtual bool IsExternalPart() { return m_bExternalFittingPart; }
	virtual BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
}; 
typedef CLDSBolt* BOLT_PTR;
class CLsRef;
struct BOLTHOLE_INFO
{
	CLDSBolt* pBolt;
	double bolt_d;
	double hole_d;
	float hole_d_increment;		//螺栓孔直径增量
	short waistLen;
	GEPOINT centre,norm,axis_x;
	BYTE cFuncType;		//螺栓孔功用类型
	DWORD dwRayNo;		//所在的射线索引号，出图时用
public:
	PROPERTY(double,posX); 
	virtual GET(posX){return centre.x;}
	virtual SET(posX){centre.x=value;}
	PROPERTY(double,posY); 
	virtual GET(posY){return centre.y;}
	virtual SET(posY){centre.y=value;}
public:
	BOLTHOLE_INFO();
	BOLTHOLE_INFO(const COORD3D& center,CLsRef* pLsRef=NULL);
	BOLTHOLE_INFO(const COORD3D& center, short waistLen = 0, CLDSBolt* pBolt = NULL);
	long BoltHandle() const;
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
};
class CLsRef{
	CLDSBolt* pLs;
	long hAgentBlockRef;	//螺栓与装配部件中孔的关联部件引用
public:
	DWORD dwRayNo;			// 所在的射线索引号，出图时用
#ifdef __DRAWING_CONTEXT_
	GEPOINT rayVecArr[32];	//绘图环境下默认固定开辟32个射线方向(归属构件相对坐标）的空间,以便存储与dwRayNo相匹配的射线方向 wjh-2015.2.27
#endif
	// 1表示位于角钢始端的螺栓，2表示位于角钢终端的螺栓，0表示位于角钢中间（或其它构件上）的螺栓
	//十字插板上的螺栓引用	11表示X-Y平面：Y正方向连接插板和包板的螺栓；12表示连接包板和其他连板的螺栓；
	//						21表示X-Y平面：Y负方向连接插板和包板的螺栓；22表示连接包板和其他连板的螺栓；
	//						31表示X-Z平面：Z正方向连接插板和包板的螺栓；32表示连接包板和其他连板的螺栓；
	//						41表示X-Z平面：Z负方向连接插板和包板的螺栓；42表示连接包板和其他连板的螺栓；
	BYTE start1_end2_mid0;		
	static const BYTE PUNCH_HOLE	=0x00;
	static const BYTE DRILL_HOLE	=0x01;	//钻孔
	static const BYTE ENLARGE_HOLE	=0x02;	//扩孔
	BYTE cFlag;				//描述螺栓孔加工工艺等特殊信息的标识细节，如低->高第1位为0x00表示冲孔，为0x01表示钻孔
	short waistLen;			// 腰圆孔腰长，腰长方向与装配坐标系的X轴同向
	long feature;			// 备用临时变量，不一定用
	CLsRef();
	CLDSBolt *GetLsPtr(){return pLs;}
	__declspec( property(get=GetLsPtr)) CLDSBolt* pBolt;
	CLsRef& operator = (CLDSBolt* rPtr)
	{
		pLs = rPtr;
		return *this;
	}
	CLDSBolt* operator->()const
	{
		if(!pLs)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Encounter an pointer error";
#else
			throw "指针遭遇失败";
#endif
		return pLs;
	}
	void FromBuffer(CBuffer &buffer,IModel* pTower,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer);
	void ToXmlFile(FILE* fp,DWORD schema);
};
#endif
#include "lslist.h"
// 节点
class NODE_POS_TO_JG_STRU
{
public:
	NODE_POS_TO_JG_STRU(){memset(this,0,sizeof(NODE_POS_TO_JG_STRU));}
	long offset_X_dist_style;	// X肢0表示偏移量为标准准距,1为g1,2为g2,3为g3,4为自定义
	long offset_Y_dist_style;	// Y肢0表示偏移量为标准准距,1为g1,2为g2,3为g3,4为自定义
	double offset_X_dist,offset_Y_dist;		// 自定义偏移量时自基准点的偏移量
};
enum REASON_TYPE
{
	REASON_DEL		= 0,
	REASON_MODIFIED	= 1,
	REVERSE_OFFSET  = 2,
	REASON_UNEXPECTED	= 100,
};
struct DESIGN_JGWING_NORM
{	//角钢肢方向设计参数
	THANDLE hViceJg;
	GEPOINT near_norm;
	BOOL bSpecific;		//指定肢方向
	BOOL bByOtherWing;
	CLDSVector spec_norm;
public:
	void ReverseDirection();
	BOOL IsSpecNormActual(){return bSpecific||(!bByOtherWing&&hViceJg==0);}
	BOOL IsByOtherWingActual(){return !bSpecific&&bByOtherWing;}
	BOOL IsByViceRodActual(){return !IsByOtherWingActual()&&!IsSpecNormActual()&&hViceJg>0x20;}
};
class CDesignPlankPara;
typedef CLDSLinePart* ROD_PTR;
typedef CLDSLinePart* CLDSLinePartPtr;

typedef struct tagDESIGN_TYPE
{
	BOOL bIntersBolt;//单螺栓
	BOOL bAngleJoint;//角钢接头
	BOOL bFlJoint;//对接法兰
	BOOL bSlopeTransition;	//变坡或双变单节点
	BOOL bBaseFoot;			//基础塔脚节点
	BOOL bL_1LB1;
	BOOL bL_1LB2;
	BOOL bL_1LE1;
	BOOL bL_1LE2;
	BOOL bL_2LB1;
	BOOL bL_2LB2;
	BOOL bL_2LB3;
	BOOL bL_2LB4;
	BOOL bL_2LE1;
	BOOL bL_2LE2;
	BOOL bL_2LC1;
	BOOL bL_2LC2;
	tagDESIGN_TYPE(){memset(this,0,sizeof(tagDESIGN_TYPE));};
}DESIGN_TYPE; 
//连接点结构构造信息
struct SLOPE_TRANSITION
{	//塔身变坡处角点关键构件
	bool bBaseFoot;
	CLDSNode* pNode;
	CLDSLinePart* pUpRod;
	CLDSLinePartPtr xarrDownRods[16];	//个别情况存在变坡点下侧主材杆件存在多组情况
	CLDSLinePart* get_pDownRod() { return xarrDownRods[0]; }
	__declspec(property(get = get_pDownRod)) CLDSLinePart* pDownRod;
	CLDSLinePart* pHoriRodX;
	CLDSLinePart* pHoriRodY;
	void ClearDownRods(){memset(xarrDownRods,0,sizeof(CLDSLinePartPtr)*16);}
	SLOPE_TRANSITION(){
		bBaseFoot=false;
		pNode = NULL;
		pUpRod = pHoriRodX = pHoriRodY = NULL;
		//xarrDownRods.ZeroPresetMemory();	//含有虚函数不能对整个SLOPE对象memset wjh-2019.5.26
		//memset(this,0,sizeof(SLOPE_TRANSITION));
	}
};
struct LDSNORMAL {	GEPOINT vLine,vSolid; };
struct CONNECT_NODEINFO;
class CConnectPointInfo
{
public:
	CConnectPointInfo();
	~CConnectPointInfo(){;}
	void Clear();
public:
	CFGWORD cfgword;
	struct SOURCE_CONN_NODEINFO{
		CONNECT_NODEINFO* pSrcNodeInfo;
		char ciConnPlaneIndex;
		SOURCE_CONN_NODEINFO(){pSrcNodeInfo=NULL;ciConnPlaneIndex=0;}
	}source;
	SLOPE_TRANSITION transit;
	bool m_bTransitConnect;	//塔身变坡处双面板
	long m_hBaseNode;	//基准节点
	long m_hBasePart;	//基准构件
	long m_hBendlineAngle;	//1号火曲板的火曲线基准角钢
	long m_hBendlineAngle2;	//2号火曲板的火曲线基准角钢
	long m_hJointPart;  //如果设计接头存储接头杆件句柄
	int m_nMaxRayLsNum;	//射线材上最多的螺栓数；
	BYTE m_iConnectWing;//0：角钢X肢1：角钢Y肢
	BYTE m_ciBaseRodConnState;//0.初始未识别;1.中间点连接;2.衔接段末端连接;3.衔接段截断点连接;4.共线截断点连接
	DESIGN_TYPE designType;
	LDSNORMAL datumplane,bendplane,bendplane2;
	GEPOINT vDatumDirect;	//垂直于火曲线基准杆件朝向基准面的方向矢量(仅当有火曲面时有效)
	LINEPARTSET linkPartSet;
	LINEPARTSET bendPlaneRodSet;	//火曲面射线杆件集合
	LINEPARTSET bendPlaneRodSet2;	//2号火曲面射线杆件集合
	int BuildRodSet(CHashSet<CLDSLinePart*>* pHashRayRods);
};
struct CONNECT_PLANE {
	BYTE ciConnectWing;	//0：角钢X肢1：角钢Y肢
	WORD wnMaxRayLsNum;	//射线材上最多的螺栓数；
	long hBendlineAngle;	//1号火曲板的火曲线基准角钢
	long hBendlineAngle2;	//2号火曲板的火曲线基准角钢
	DESIGN_TYPE designType;
	GEPOINT vDatumDirect;	//垂直于火曲线基准杆件朝向基准面的方向矢量(仅当有火曲面时有效)
	LDSNORMAL datumplane,bendplane,bendplane2;
	SIMPLE_ARRAY<CLDSLinePart*,PRESET_OBJS16> xarrBasePlaneRods;	//基面连接杆件(由于后续拆分节点命令用到，不能包含baserod) wjh-2019.10.8
	SIMPLE_ARRAY<CLDSLinePart*,PRESET_OBJS16> xarrBendPlaneRods;	//1#制弯面连接杆件
	SIMPLE_ARRAY<CLDSLinePart*,PRESET_OBJS16> xarrElsePlaneRods;	//2#制弯(其它)面连接杆件
	bool get_blValid() { return xarrBasePlaneRods.Count>0; }
	__declspec(property(get=get_blValid)) bool blValid;
public:
	int BuildRodSet(CHashSet<CLDSLinePart*>* pHashRayRods,long hExcludeBaseRod=0);
};
struct CONNECT_NODEINFO
{
protected:
	short m_siIterSerial;	//遍历指针序号
public:
	BYTE cbDisableFlag;	//某些情况下需要对某些特别连接进行抑制（可能由于已经连并处理过了） wjh-2019.10.5
	CONNECT_NODEINFO();
	void Clear();
public:
	//0.普通连接板;1.变坡或双变单节点;2.基础塔脚节点
	char ciTransitConnType;	//
	SLOPE_TRANSITION transit;	//m_bTransitConnect=true时有效
	struct COMMON_NODE_STRUCTURE {
		BYTE ciBaseRodConnState;//0.初始未识别;1.中间点连接;2.衔接段末端连接;3.衔接段截断点连接;4.共线截断点连接
		long hBaseNode;	//基准节点
		long hBaseRod;	//基准构件
		long hJointRod;	//共线时设计接头，存储对接杆件的句柄
		bool blFlJoint;		//true:接头为钢管法兰连接;false:接头为角钢接头
		SIMPLE_ARRAY<CONNECT_PLANE,PRESET_OBJS4> planes;
	}xCommNode;	//普通节点构造
	CONNECT_PLANE* AddPlane(int *lpRsltPlaneIndex=NULL);
	CONNECT_PLANE& get_xXlegPlane();
	__declspec(property(get=get_xXlegPlane)) CONNECT_PLANE& xXlegPlane;
	CONNECT_PLANE& get_xYlegPlane();
	__declspec(property(get=get_xYlegPlane)) CONNECT_PLANE& xYlegPlane;
	CONNECT_PLANE& get_xAcrossPlane();
	__declspec(property(get=get_xAcrossPlane)) CONNECT_PLANE& xAcrossPlane;
	static const short CONNPLANE_LEGX	=  0;
	static const short CONNPLANE_LEGY	=  1;
	static const short CONNPLANE_JOINT  = -1;
	static const short CONNPLANE_ACROSS = -2;
	bool GetConnectPointInfo(short siConnPlaneIndex,CConnectPointInfo* pConnPointInfo);
	bool EnumConnPointFirst(CConnectPointInfo* pConnPointInfo);
	bool EnumConnPointNext(CConnectPointInfo* pConnPointInfo);
	bool InitFromConnectPointInfo(CConnectPointInfo* pConnPointInfo,int liToConnPlaneIndex);
};

class CNodeRestrict{
	WORD m_wRestrict;	//百十个位分别表示是否有X/Y/Z方向的约束，零表无此向约束
public:
	THANDLE m_hWorkModule;
	CNodeRestrict(){m_wRestrict=0;m_hWorkModule=0;}
	WORD GetRestrictWord();
	void SetRestrict(WORD wFlag,BOOL bEnable=TRUE);
	void SetRestrictWord(WORD wFlag);
	BOOL IsHasRestrict(WORD wFlag);
	operator WORD(){return m_wRestrict;}
	CXhChar16 ToString(int style=0);	//style=0:用六位数字表示;style=1:用字母表示
};
class CNodeLoad{
public:
	THANDLE m_hWorkStatus;	//荷载所对应的工况句柄
	f3dPoint windFxy,wireFxy,setupF,wirebreakF;//三个坐标轴方向的集中可变外荷载值(N)
	double iceFz,permanentFz;		//覆冰荷载及重力方向的准永久外荷载值
	CNodeLoad(){m_hWorkStatus=0;iceFz=permanentFz=0.0;}	//效应系数
};

/* 转至Tower.h文件　wjh-2014.4.27
//节点集中荷载
class CExternalNodeLoad
{
public:
	double permanentFz;	//重力方向的准永久外荷载值(N)
	f3dPoint Fxyz;		//三个坐标轴方向的集中可变外荷载值(N)
	CExternalNodeLoad(){permanentFz=0.0;}
};
*/
typedef ATOM_LIST<CNodeRestrict> CNodeRestList;

class CAttachWindLoad : public CLDSDbObject
{
	ATOM_LIST<long>* nodeList;
protected:
	BOOL *m_pModified;
public:
	CAttachWindLoad();
	virtual ~CAttachWindLoad();
	void SetModifiedFlagAddress(BOOL *pModified)
	{	m_pModified=pModified;	}
	void SetModifiedFlag()
	{	if(m_pModified) *m_pModified=TRUE; }

	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);	
	void ToXmlFile(FILE* fp,DWORD schema);
	long handle;
	long iNo;					//临时保证唯一的编号
	ATOM_LIST<long>* GetNodeList(){return nodeList;}
	char name[MAX_TEMP_CHAR_50+1];
	double m_fShieldArea;		//挡风面积
	double m_fShieldCoef;		//挡风系数
	double m_fProfileCoef;		//体型系数	
	double m_fAttachWeight;		//附加重量(kg)
	double m_fBetaZ;			//风振系数
	double m_fBetaZForFoundation;	//风振系数
};

struct XIANLU_STRU
{
	double horizon_space;	//水平档距
	double vertical_space;	//垂直档距
	double min_vert_space;	//最小垂直档距
	double horizon_space_allotcoef;	//水平档距分配系数
	double vertical_space_allotcoef;//垂直档距分配系数
	double represent_space;	//代表档距
	double HtoL;			//高差系数
	double xianlu_angle;	//单位度(°)
};
struct DIANXIAN_STRU
{
	double m_fDiameter;		// 电线外径,mm
	double m_fArea;			// 电线计算截面积,mm2
	double m_fUnitWeight;	// 电线单位长重量,kg/km
	double m_fForcePermit;	// 电线额定抗拉力,N
	double m_fSafeCoef;		// 电线安全系数
	double m_fMaxUseTension;// 电线最大使用张力,N
	double m_fElasticCoef;	// 电线弹性系数,MPa
	double m_fExpansionCoef;// 电线温度线膨胀系数,1/1/°C
	char specWireType[50];	// 电线的类型代码
	char specWire[50];		// 电线的规格型号描述
};
class CWireInfo
{
public:
	DIANXIAN_STRU wire;
	XIANLU_STRU xianlu;
	BYTE m_cXianType;	//'E'地线Earth Wire,'C'导线Conducting Wire,'O'为跳线等辅助线挂点，其余为非挂线点
	BYTE m_cFormula;    //张力弧垂计算公式0悬链挂线1斜抛物线 
	BYTE m_nDaoXianFengLieNum;	//导线分裂数
	bool m_bNaiZhang;	//true耐张挂线点 ,false悬垂直线挂线点
	int  m_nVoltGrade;	//电压等级(kv)
	double land_high;	//(m)
	char specAttachment[50];			//金具及绝缘子等附属件明细
	double m_fAttachmentWeight;			//金具及绝缘子等附属件重量(kg)
	double m_fAttachmentDiameter;		//绝缘子等附属件宽度或外径(mm)
	double m_fAttachmentLength;			//绝缘子等附属件长度(mm)
	double m_fAchorLineAngle;			//锚线对地夹角
	double m_fTractionLineAngle;		//牵引线对地夹角
	CWireInfo();
};

struct WEATHER{
	//依次为最低气温、平均气温、大风、覆冰气温、最高气温、安装气温
	long m_nHighestT,m_nLowestT,m_nAverageT,m_nMaxWindT,m_nMaxIceT,m_nSetupT;
	double m_fMaxIceThick;		//最大覆冰厚度(mm)
	double m_fMaxIceThickCheck;	//验算覆冰厚度(mm)
	double m_fMaxIceWindV;
	double m_fMaxWindV;			//大风速度(m/s)
	double m_fMaxWindVCheck;	//验算风速(m/s)
	double m_fSetupWindV;
	void CopyWeather(WEATHER* pWeather)
	{
		m_fMaxIceThick=pWeather->m_fMaxIceThick;		//最大覆冰厚度(mm)
		m_fMaxIceThickCheck=pWeather->m_fMaxIceThick;	//验算覆冰厚度(mm)
		m_fMaxIceWindV=pWeather->m_fMaxIceWindV;
		m_fMaxWindV=pWeather->m_fMaxWindV;		//大风速度(m/s)
		m_fMaxWindVCheck=pWeather->m_fMaxWindV;	//验算风速(m/s)
		m_fSetupWindV=pWeather->m_fSetupWindV;
		m_nAverageT=pWeather->m_nAverageT;
		m_nHighestT=pWeather->m_nHighestT;
		m_nLowestT=pWeather->m_nLowestT;
		m_nMaxIceT=pWeather->m_nMaxIceT;
		m_nMaxWindT=pWeather->m_nMaxWindT;
		m_nSetupT=pWeather->m_nSetupT;
	}
	void FromBuffer(CBuffer &buffer,long version=NULL);
	void ToBuffer(CBuffer &buffer);	
	void ToXmlFile(FILE* fp,DWORD schema);
};
struct WEATHER_REGION : public WEATHER
{
	char weatherCode[51];
	WEATHER_REGION(){memset(this,0,sizeof(WEATHER_REGION));}
	void FromBuffer(CBuffer &buffer,long version=NULL);
	void ToBuffer(CBuffer &buffer);	
	void ToXmlFile(FILE* fp,DWORD schema);
};
/* 已移至WireLayoutsEvolvement.h,cpp中　wjh-2014.5.2
class CWorkEnvLoad
{
public:
	char name[41];					//对应工况名称
	double m_fLoadX;				//电气提供横向风力荷载总和(N)
	double m_fLoadY;				//电气提供纵向荷载总和(N)
	double m_fLoadZ;				//电气提供垂直荷载总和(N)
	double m_fMinVertSpaceLoadX;	//电气提供横向荷载(最小垂直档距)总和(N)
	double m_fMinVertSpaceLoadY;	//电气提供纵向荷载(最小垂直档距)总和(N)
	double m_fMinVertSpaceLoadZ;	//电气提供垂直荷载(最小垂直档距)总和(N)
	double m_fWireLoadX;			//电线的风荷载(N)
	double m_fWireLoadZ;			//电线的垂直荷载(N)
	double m_fMinVertSpaceWireLoadZ;//最小垂直档距时电线的垂直荷载(N)
	double m_fAttachmentLoadX;		//附属件风荷载(含覆冰影响)(N)
	double m_fAttachmentLoadZ;		//附属件垂直荷载(含覆冰影响)(N)
	double m_fSetupGa;				//安装时所需的附加垂直荷载(N)
	double huchui;					//m,弧垂
	CWorkEnvLoad();
	void SumCal();	//将各种分荷载汇总成总荷载
};					//电气提供的原始线条荷载
enum WORKENVLOAD_STATUS{
	WORKENVLOAD_WIND=1,WORKENVLOAD_ICE,WORKENVLOAD_LOWTEMP,WORKENVLOAD_ICEDIFF,
	WORKENVLOAD_ICELESS,WORKENVLOAD_ICEMORE,WORKENVLOAD_DUANXIANRUNNING,WORKENVLOAD_DUANXIANBREAK,
	WORKENVLOAD_SETUPSUSPEND,WORKENVLOAD_SETUPSUSPENDING,WORKENVLOAD_SETUPJINXIAN_OTHER_FINISH,WORKENVLOAD_SETUPJINXIAN_OTHER_EMPTY,
	WORKENVLOAD_ANCHORFINISHED,WORKENVLOAD_SETUPANCHORING,WORKENVLOAD_EARTHQUAKE};
class CWireLoadModel
{
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	CXuanLianResolver* m_pResolver;
public:
	WEATHER weather;
	char name[51];	//线条荷载计算模型名称
	long iNo;		//标识编号
	CWireInfo wireInfo;
	CHashList<CWorkEnvLoad>hashLoad;	//电气提供荷载
	CWireLoadModel(){memset(name,0,51);iNo=0;}
	//void InitWeather(CTower *pTower);	//为使包含头文件更合理，已移至CTower类中
	void ResetWorkEnvLoadSet();
	void CopyWireLoadModel(CWireLoadModel *pWireLoadModel);
	void FromBuffer(CBuffer &buffer,int nVersion=0,long doc_type=3);
	void ToBuffer(CBuffer &buffer);
	void ToXmlFile(FILE *fp,DWORD schema);
	//判断导地线的控制气象条件并初始化电线状态方程
	void InitDianXianEquation(
		WEATHER &weather,double gama_g,double gama_ice_w,double gama_max_w,double gama_ice);
	//weather是气象条件
	//cal_style=0：使用悬链线方程计算cal_style=1:使用斜抛物线方程计算
	void CalWorkEnvLoad(char cLandscape,char cLineRoadType);
	//属性管理
	static CHashStrTable<long> propHashtable;
	static CHashTable<DWORD> propStatusHashtable;
	int GetPropValueStr(long id,char *valueStr);				//得到属性值字符串
	static void InitModelPropHashtable();
	static int GetPropStr(long id,char *propStr);		//得到属性字符串
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
#endif
};
*/
//分配到节点上的体荷载（自重荷载及塔身覆冰荷载）
class CProcessBodyLoad
{
public:
	CFGWORD cfgword;
	double iceWeightLoadCoef;	//覆冰荷载的计算系数，此系数×覆冰厚度(mm)＝覆冰荷载(N)
	double iceWeightLoadCoef2;	//覆冰荷载的计算的二次项系数，此系数×覆冰厚度×覆冰厚度(mm)＝覆冰荷载(N)
	double selfWeightLoad;		//折算到此承力节点上的自重荷载(N)
	CProcessBodyLoad(){iceWeightLoadCoef=iceWeightLoadCoef2=selfWeightLoad=0;}
	CProcessBodyLoad& operator+=(const CProcessBodyLoad& other);
	friend CProcessBodyLoad operator+(const CProcessBodyLoad& v1,const CProcessBodyLoad& v2);
	CProcessBodyLoad& operator*=(double coef);
	CProcessBodyLoad operator*(double coef);
public:
	double IceLoadCoef();
	double IceLoadCoef2();
	double SelfWeight();
};
enum NODE_FORCE_TYPE{AUTO_NODE=0,FORCE_NODE=1,SWING_NODE=2,};	//自动节点是否传力据情况而定，而传力节点则一定是传力的,摆动节点则可以沿无约束方向进行摆动
enum FORCE_TYPE{CENTRIC_FORCE=0,ECCENTRIC_FORCE=1};	//端部受力连接的两种情况：中心受力；偏心受力
//<DEVELOP_PROCESS_MARK nameId="CLDSNode">
struct WIREPLACE
{
	SUBID16 idNodePlace;//挂位编号,格式为"架线方案号-挂位编号",方案号与挂位编号各占2个字节
	double fLoadAllocCoefX,fLoadAllocCoefY,fLoadAllocCoefZ;//各荷载分量的分配系数
	WIREPLACE();
};
//节点坐标的受控信息
struct SLAVE_NODE
{
protected:
	long _hRelaNode;
public:
	BYTE cFlag;	//0
	BYTE ciCtrlType;	//受控类型:0.总控参数直接控制坐标;1.指定坐标系下局部坐标;2.标准组件
	BYTE cbMirFlag;		//0x01:同时控制X轴对称节点;0x02:同时控制Y轴对称节点;0x03:同时控制Z轴对称节点
	long hAcs;			//在指定坐标系
	char posx_expr[20];
	char posy_expr[20];
	char posz_expr[20];
public:
	SLAVE_NODE();
	void SetKey(DWORD keyid){_hRelaNode=keyid;}
	long get_hRelaNode(){return _hRelaNode;}
	__declspec(property(get=get_hRelaNode)) long hRelaNode;
	void SetControlState(char cCoordType='X',bool controlled=true);
	bool IsControlCoordX();//{return cFlag&0x01;}
	bool IsControlCoordY();//{return cFlag&0x02;}
	bool IsControlCoordZ();//{return cFlag&0x04;}
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);	
};
class CLDSModule;
class CLDSNode : public CLDSDbObject{
	bool m_bInitArcLiftPos;	//预搭后位置为计算值，需要在SetPosition中根据节点初始位置赋初值
	bool _bVirtualObj;		//虚拟辅助节点
	CONNECT_NODEINFO* m_pxConnNodeInfo;
	long m_hArcLift;
	GEPOINT pos;	//原始节点位置
	GEPOINT m_xArcLiftPos;		//预拱后坐标
	GEPOINT m_xPreliftPosition;	//个别情况pos输入的是预拱后坐标，此时m_xPreliftPosition表示临时反算出来的预拱前坐标，不持物理存储 wjh-2018.5.15

	// ------ vvvvvvv -------- 有关节点坐标计算方式的变量 -----------
	/*
	节点坐标计算类型(去掉原类型"8.沿杆件自某节点的垂直偏移点")
	0.无任何依赖
	1.杆件上X坐标值不变点
	2.杆件上Y坐标值不变点
	3.杆件上Z坐标值不变点
	4.两杆件交叉交点，_cPosCalViceType此时表示投影面类型, 0:表示交叉面为投影面,1:X-Z平面;2:Y-Z平面;3:X-Y平面
	5.杆件上的比例等分点
	6.沿杆件与某节点等坐标分量的节点_cPosCalViceType=0,'Z'表示等Z分量,'X'表示等X分量,'Y'表示等Y分量
	7.沿杆件自某节点的偏移点_cPosCalViceType=0表示沿杆件长度偏移,'X'表示等X分量,'Y'表示等Y分量,'Z'表示等Z分量
	8.基准面上XY坐标值不变点，对应原类型9，_cPosCalViceType=0表示无对称参照,'X'表示X轴对称,'Y'表示Y轴对称,'Z'表示Z轴对称,'I'表示通过两节点指定线面求交的线
	9.基准面上YZ坐标值不变点，对应原类型10，_cPosCalViceType同上
	10.基准面上XZ坐标值不变点，对应原类型11，_cPosCalViceType同上
	11.指定基准点
	12.原始节点相对X/Y/Z对称生成的节点，对称轴为_cPosCalViceType='X','Y','Z'
	13.等坐标分量点，分别对应：
	   pos.x=arrRelationNode[0].pos.x
	   pos.y=arrRelationNode[1].pos.y
	   pos.z=arrRelationNode[2].pos.z
	*/
	BYTE _cPosCalType;
	BYTE _cPosCalViceType;	// 个别节点坐标计算类型的坐标分量类型属性(仅对计算类型4,6～10有效)
	double _attach_scale;	// 比例点所占比例(自角钢起始点相对全角钢长)
	double _attach_offset;	// 偏移距离
	long _hFatherPart;			// 节点所从属角钢句柄
	bool MigrateFatherRod(long hNewRod);
protected:
	virtual char get_ciLegQuad();
public:	//坐标计算类型m_cPosCalType的枚举值
	static const BYTE COORD_INDEPENDENT	 = 0;
	static const BYTE COORD_X_SPECIFIED  = 1;
	static const BYTE COORD_Y_SPECIFIED  = 2;
	static const BYTE COORD_Z_SPECIFIED  = 3;
	static const BYTE COORD_INTERSECTION = 4;
	static const BYTE COORD_SCALE		 = 5;
	static const BYTE COORD_OFFSET		 = 7;
	static const BYTE PLANE_XY_SPECIFIED = 8;
	static const BYTE PLANE_YZ_SPECIFIED = 9;
	static const BYTE PLANE_XZ_SPECIFIED =10;

	static bool INTERNAL_TEST_VERSION;	//目前用以启动关联节点随动功能
	//类对象属性拷贝及设计参数智能对称更新
	static const DWORD PROP_BASIC_COPY			= 0;	//基本信息
	static const DWORD PROP_POS_COPLANAR_COPY	= 1;	//节点基准面定位信息

	static const DWORD PROP_DISCONNECT	= 0;	//断开
	static const DWORD PROP_SYNC_COPY	= 1;	//同步对称
	static MEMORY_BLOCK MPCL;	//Mirror Property Control List
	static PROPLIST_TYPE propMirSync;
	static void InitPropMirBitsMap();
	virtual PROPLIST_TYPE* GetSyncPropList(){return &propMirSync;}
	virtual MEMORY_BLOCK GetDefaultMPCL(){return MPCL;}
	virtual bool IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr);
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部位置对称无关的属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
	//
	static const BYTE CLEAR_BUFF_OF_RELA_PARTS	=0x01;
	static const BYTE DISABLE_RECORD_RELA_PARTS =0x02;
	static BYTE GetMovePartsUpdateFlag();
	static BYTE SetMovePartsUpdateFlag(BYTE cbFlag=0x01);
	static void ClearUpdateMovePartsBuff();
	static CLDSLinePart* EnumFirstRelaRod();
	static CLDSLinePart* EnumNextRelaRod();
	static CLDSNode* EnumFirstRelaNode();
	static CLDSNode* EnumNextRelaNode();
public:
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
public:
	GEPOINT Feq;					//地震荷载未乘任何系数的标准值
	GEPOINT vSwingLoadFxyz;		//由摆动节点处外荷载通过计算等效过来的外荷载,临时数据不存储(N)
	CProcessBodyLoad bodyLoad;		//分配到节点上的等效节点体荷载,临时数据不存储
	long m_hRelaWireLoad;
	CNodeRestrict restrict;			//节点约束
	//CHashList<CNodeLoad>hashLoad;	//存储各节点在各工况下的各种荷载力
	// ------ VVVVVVV -------- 有关有限元应力计算的参数------
	NODE_FORCE_TYPE force_type;	//传力类型
	GEPOINT AutoCalLoad;			//x/y/z三个分量分别表示这三个方向的自动计算所得荷载(自荷载)力（牛）主要用于输出时临时统计计算，不作存储(N)
	GEPOINT WindLoad;				//x/y/z三个分量分别表示这三个方向的自动计算的风荷载(N)
	GEPOINT m_offset,m_rotoffset;	//节点的位移和旋转位移
	GEPOINT external_force;		//该节点上的外加等效荷载力(N)
	GEPOINT internal_force;		//各单元汇集到该节点上的内力(N)
	GEPOINT internal_moment;		//各单元汇集到该节点上的内力矩
	GEPOINT unbalance_force;		//各节点的不平衡力(N)
	GEPOINT PermanentLoad;			//此节点上的永久荷载,输入的值应考虑永久荷载分项系数及效应系数(N)
	GEPOINT m_offsetAmplitude;		//某一振型下该节点的振幅
	double attachMass;			//附加设施质量重力荷载(kg)
	double dfAddedStruMass;		//模态分析时扣除受力材本身之外需要增加的结构重理（辅材，节点板等）,临时计算变量不永久存储 wjh-2019.7.5
	BYTE m_ciWireFractions;		//线缆分几个挂点挂到塔架上
	WIREPLACE_CODE wireplace;	//挂位编码如"前导1-S1"，"前导1-V2"，"跳1-2"等
	//BYTE m_cHangWireType;
	PROPERTY(char,m_cHangWireType);	//'E'地线Earth Wire,'C'导线Conducting Wire,'J'为跳线等辅助线挂点，其余为非挂线点
	SETEX(m_cHangWireType){return wireplace.ciWireType=value;}
	GET(m_cHangWireType){return wireplace.ciWireType;}
	CXhChar50 m_sHangWireDesc;	//挂线描述
	//CHashList<WIREPLACE> wires;		//该挂点所接电线位置编号，可用于一点多挂情况
public:
	virtual BOOL IsFatherObject();
	virtual BOOL GetFatherMirObj(RELATIVE_OBJECT *pRelaObj);
	//int GetForceFaceNum(f3dPoint axis_x,f3dPoint *force_face_norm);	//返回该点存在受力面数(超过2时返回2，forece_face_norm存储主受力面法线（垂直于杆单元轴线方向）
	//f3dPoint GetBodyLoad(double ice_thick=0.0);	//返回等效节点体积力,单位N
	void UpdateWireDesc();
	bool IsHangWire();	//是否挂有各种类型的电线
	BOOL IsHasRestrict(WORD wRestrict);
	BOOL IsHasRestrict(THANDLE hModule,WORD wRestrict);	//指定模型中是否有指定约束
	WORD GetRestrictWord(THANDLE hModule);		//获得指定模型中的约束字
	void SetRestrict(WORD wRestrict,BOOL bEnable=TRUE);
	void SetRestrict(THANDLE hModule,WORD wRestrict,BOOL bEnable=TRUE);//设定指定模型中的指定约束
	void SetRestrictWord(THANDLE hModule,WORD wRestrict);//设定指定模型中的指定约束
	//CHashList<CExternalNodeLoad>hashNodeLoad;		//存储荷载节点,key值为节点句柄
#endif
	UINT point_i;					//节点编号(用于满应力接口)
	DECLARE_READONLY_PROPERTY(UINT,pointI);
	DECLARE_READONLY_PROPERTY(GEPOINT,xPreliftPos);				//预拱前初始点坐标
	DECLARE_READONLY_PROPERTY(GEPOINT,xOriginalPos){return pos;}			//不考虑正(反)向预拱始初始输入点坐标
	DECLARE_READONLY_PROPERTY(GEPOINT,xArcliftPos);	//预拱后点坐标
	DECLARE_READONLY_PROPERTY(GEPOINT,xActivePos);	//当前显示的点坐标
public:
	NODE_POS_TO_JG_STRU xFatherAngleZhunJu;

	PROPERTY(long,hFatherPart);
	GET(hFatherPart){return _hFatherPart;}
	SET(hFatherPart){
		if(MigrateFatherRod(value))
			SetModified();
	}
	PROPERTY(bool, m_bVirtualObj);		//虚拟辅助节点
	GET(m_bVirtualObj){return _bVirtualObj;}
	SET(m_bVirtualObj){
		_bVirtualObj= value;
		SetModified();
	}
	PROPERTY(BYTE,m_cPosCalType);
	GET(m_cPosCalType){
		CLDSNode* pMotherNode=MotherNode();
		if(pMotherNode==this)
			return _cPosCalType;
		else
			return pMotherNode->m_cPosCalType;
	}
	SET(m_cPosCalType){
		if (value>=0)
		{
			CLDSNode* pMotherNode=MotherNode();
			_cPosCalType=value;
			if(pMotherNode!=this)
				pMotherNode->m_cPosCalType=value;
		}
	}
	PROPERTY(BYTE,m_cPosCalViceType);
	GET(m_cPosCalViceType){
		CLDSNode* pMotherNode=MotherNode();
		if(pMotherNode==this)
			return _cPosCalViceType;
		else
			return pMotherNode->m_cPosCalViceType;
	}
	SET(m_cPosCalViceType){
		if(value>=0)
		{
			CLDSNode* pMotherNode=MotherNode();
			_cPosCalViceType=value;
			if(pMotherNode!=this)
				pMotherNode->m_cPosCalViceType=value;
		}
	}
	PROPERTY(double,attach_scale);
	GET(attach_scale)
	{
		CLDSNode* pMotherNode=MotherNode();
		if(pMotherNode==this)
			return _attach_scale;
		else
			return pMotherNode->attach_scale;
	}
	SET(attach_scale)
	{
		CLDSNode* pMotherNode=MotherNode();
		_attach_scale=value;
		if(pMotherNode!=this)
			pMotherNode->attach_scale=value;
	}
	PROPERTY(double,attach_offset);
	GET(attach_offset)
	{
		CLDSNode* pMotherNode=MotherNode();
		if(pMotherNode==this)
			return _attach_offset;
		else 
			return pMotherNode->attach_offset;
	}
	SET(attach_offset)
	{
		CLDSNode* pMotherNode=MotherNode();
		_attach_offset=value;
		if(pMotherNode!=this)
			pMotherNode->attach_offset=value;
	}
	// 描述与本节点有关的其它数据.
	THANDLE hDatumPoint;	// 定位基准点	wht 13-10-17
	THANDLE hRelationPlane;	// 相关基准面句柄
    THANDLE arrRelationNode[4];	// 根据两节点求比例等分点
    THANDLE arrRelationPole[2];	// 根据两角钢求交叉交点
//4.公有成员函数定义
public:
	f3dPoint ActivePosition(){return Position(!m_bDisplayBeforeArcLift);}
	f3dPoint Position(bool bArcLiftPos=true);
	double Position(char cCoordType,bool bArcLiftPos=true);
	f3dPoint SetPosition(const f3dPoint& position);
	f3dPoint SetPosition(double x,double y,double z);
	f3dPoint SetPositionX(double coordx);
	f3dPoint SetPositionY(double coordy);
	f3dPoint SetPositionZ(double coordz);
	bool IsUdfCoordX() const;	//X坐标分量是否由用户定义
	bool IsUdfCoordY() const;	//Y坐标分量是否由用户定义
	bool IsUdfCoordZ() const;	//Z坐标分量是否由用户定义
	BOOL Set(bool bIgnoreRepeatFlag=false);				// 三种赋值方法：
	bool IsIntegrityParams();	//是否坐标计算参数是完整的
	bool ReverseUpdateParams();	//根据坐标反向计算参数
	bool SearchRelaDependNodeRods(COLINEAR_TOLERANCE* pTolerance=NULL);	//自动搜索派生节点的坐标依赖节点及杆件
	CLDSNode *MotherNode();
	virtual long ArcLiftHandle(){return m_hArcLift;}
	virtual long SetArcLift(long hArcLift){m_hArcLift=hArcLift;SetModified();return m_hArcLift;}
	f3dPoint ArcLiftPos(){return m_xArcLiftPos;}
	virtual void CalPosition(bool bIgnoreRepeatFlag=false){Set(bIgnoreRepeatFlag);}
//4.公有成员函数定义
public:
	long RootFatherPartHandle();
	CLDSPart* RootFatherPart();
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsPart() const{return false;}
	virtual bool IsNode() const{return true;}
	virtual void TransToCS(GECS& cs);
	virtual void Offset(const double* offset);
	virtual bool OffsetPosParam(const double* offset);
	virtual void ResetRelativeObjs();
	void ClearErrorRelativeObjs();
	int ValidCheck();
	CLDSNode();
	~CLDSNode();
	CLDSNode& operator=(const CLDSNode &node);
	void CopyProperty(CLDSDbObject* pObj);
	THANDLE GetHandle(){return handle;}
	long GetRelativeMirObj(MIRMSG msg,bool bIncSelf=TRUE);
	CLDSNode * GetMirNode(MIRMSG mirmsg);
	BOOL AssertValid() const;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	bool RetrieveSlopeTransitionNode(SLOPE_TRANSITION* pTransition);
	bool RetrieveFootTransitionNode(SLOPE_TRANSITION* pTransition);
	CONNECT_NODEINFO* GetConnectNodeInfo();
	bool RetrieveConnectNodeInfo(CONNECT_NODEINFO* pConnNodeInfo,BOOL bDesignBendConnect=TRUE,CLDSModule *pWorkHeight=NULL);	//智能识别连接点信息
	BOOL GetIntersZhun(COORD3D* inters_zhun,double *pDist=NULL);
	void GetAttachNodeFatherJg(CLDSNode *pNode1,CLDSNode *pNode2,CLDSLineAngle **ppJg1,CLDSLineAngle **ppJg2);
	f3dPoint GetInverseNodePos(CLDSLineAngle *pJg=NULL);
	bool RetrieveConnectRods(const double* vzWorkNorm, LINEPARTSET& rodSet, bool bIncBendConnect = true);
#endif
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);	
	//bReadForCheck表示是否读取为了进行模型比对检查(仅读取最少量校审相关信息)
	void FromCdfBuffer(CBuffer &buffer,long version=0,bool bReadForCheck=false);
	void ToCdfBuffer(CBuffer &buffer,long version=0);	
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	//update_type 为更新类型 0-更新节点父杆件，1-更新节点依附杆件1，2-更新节点依附杆件2
	void UpdateMirNodeFatherPart(MIRMSG mirmsg, CLDSPart *pPart, int update_type=0);
	//update_type 0-更新依附节点1 1-更新依附节点2 2-更新依附节点3 3-更新依附节点4
	void UpdateMirNodeAttachNode(MIRMSG mirmsg,CLDSNode *pAttachNode,int update_type=0);
	//属性管理
private:
	static long m_idPropUdf;						//用户当前可定义使用的属性Id
	static long m_idMinPropUdf;						//可使用的最小用户自定义属性Id
public:
	static BOOL m_bDisplayBeforeArcLift;			//显示预拱前位置
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	static long RegisterUdfPropId(char *propStr);		//添加用户自定义属性Id
	DWORD RetrieveWirePlaceKey(char* propKeyStr);	//得到属性值
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);				//得到属性值字符串
	static void InitNodePropHashtable();
	UPDATE_PROPLISTITEM_FUNC(CLDSNode,propHashtable)		//定义属性项更新函数
	static BOOL GetPropStr(long id,char *propStr,int maxStrLen=100);		//得到属性字符串
	static BOOL FindUdfPropKeyStrByPropId(long id,char* propStr=NULL,int maxStrLen=100);		//判断属性Id是否是用户自定义的Id
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//得到构件属性的默认只读状态
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//得到构件属性的默认只读状态
	//版本控制日志相关方法
#if defined(__PART_DESIGN_INC_)&&!defined(__DRAWING_CONTEXT_)&&!defined(__IGNORE_VERSION_COMPARE_)
	//virtual bool IsIdentical(void* pOldObj);
	virtual bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents);
#endif
	//以下为旧函数用于与新代码对比测试
#ifdef __ALFA_TEST_VERSION_
	GEPOINT TestAutoCalLoad;			//x/y/z三个分量分别表示这三个方向的自动计算所得荷载(自荷载)力（牛）主要用于输出时临时统计计算，不作存储(N)
	GEPOINT TestWindLoad;				//x/y/z三个分量分别表示这三个方向的自动计算的风荷载(N)
#endif
	bool HasRelativeNode(long hNode);
};
class CNodeMoveDetector{
	CLDSNode* m_pNode;
	GEPOINT m_xOldPos;
	bool m_bEnableDetector;
	static long m_nDepth;	//探测深度
protected:
	bool AppendPosSlaveNodes(CLDSNode* pMasterNode);
public:
	static bool m_bGlobalEnabled;	//是否全局启用/禁用节点移动探测
	CNodeMoveDetector(CLDSNode* pNode,bool enabled=true);
	bool Enable(bool enabled=true){return m_bEnableDetector=enabled;}
	~CNodeMoveDetector();
};
//</DEVELOP_PROCESS_MARK>
#ifdef __PART_DESIGN_INC_
struct HANG_NODE_INFO : public ICompareClass
{
	char m_ciType;
	char m_sName[50];
	GEPOINT pos;
	GEPOINT relaHolePt[2];
	WIREPLACE_CODE m_xWirePlace; 
	HANG_NODE_INFO(){m_ciType=0;strcpy(m_sName,"");}
	virtual int Compare(const ICompareClass *pCompareObj)const;
};
#endif

typedef CLDSNode* NODE_PTR;
typedef CLDSNode* CLDSNodePtr;
#include "TaperPolygon.h"
class CWindSegElemPicker;
class CWindSegment : public ILog{
	int m_nValidNodeCount;		//临时记录以便提高代码执行效率
public:
	CWindSegment();
	void ToBuffer(CBuffer& buffer, long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromBuffer(CBuffer& buffer, long version = NULL,long doc_type=3);
public:	//风振计算属性及函数
	double dfMuSa, dfMuSb;		//风压段正侧面风吹的加权平均体型系数(拟临时存储给风振系数计算用，不存储) wjh-2019.7.6
	CWindSegment* pMirArmWindSeg;	//风振系数计算时指向临时的对称风压段对象 wjh-2019.7.6
	TAPER_POLYGON xTaperScope;		//风振系数计算时用于识别杆件单元是否归属当前风压段的多面体 
	bool MirToWindSeg(CWindSegment* pMirWindSeg, int ciMirY1X2, IModel* pModel);
	bool IsIncludeElem(IModel* pTower, const double* pxElemStart, const double* pxElemEnd, double tolerance = 1.0);
	bool IsIncludePoint(IModel* pTower, const double* pxPoint, double tolerance = 1.0);
public:
	CWindSegElemPicker* pSideElemPicker;
	CWindSegElemPicker* pFrontElemPicker;
	GEPOINT xProjPlaneOrg;	//投影面基点,一般为原点(0,0,0)
	int GetMaxNodeCount(){return 8;}//sizeof(nodeArr)/4;}
	int StatValidNodeCount(IModel *pTower);
	int ValidNodeCount(){return m_nValidNodeCount;}
	void InitNodePtrArr(IModel* pTower);
	//bFront只针对８点塔身段时表示统计正面还是侧面轮廓点用
	bool StatProfileNodeSet(IModel* pTower,NODESET &nodeset,BOOL bFront=TRUE);
	bool IsHasLegObj(IModel* pTower);
	CFGWORD StatCfgword(IModel* pTower);
	long iNo;
	long nodeArr[8];			//八个轮廓节点的句柄
	CLDSNode* nodePtrArr[8];	//八个轮廓节点的指针（临时存储不作为持久化依据）
	BYTE cType;					//标准段号0.八点分配塔身段;1.自定义多节点轮廓;2.六点分配三角塔身
	BYTE cMirType;				//对称信息0:无对称;1:Y轴对称;2:X轴对称
	BYTE cCalStyle;				//计算方式0:按塔身方式计算风压;1:按X方向水平横担方式计算风压;2:按Y方向水平横担方式计算风压
	BOOL m_bAutoCalProfileCoef;	//是否自动加权平均计算K值，否时由用户自己输入
	double CqiFront, CqiSide;	//正/侧面增大系数
	double K;					//塔身风荷载断面形状系数，组合角钢时为1.1其余为1.0
	double m_fProfileModeCoef;	//体型系数的加权平均附加系数
	double AfFront;				//m2构件正面承受风压净投影面积
	double AfSide;				//m2构件侧面承受风压净投影面积
	double FaiFront;			//正面挡风系数
	double FaiSide;				//侧面挡风系数
	BOOL   m_bSpecB_A;			//由用户指定迎风面与背风面之间距离与迎风面宽度之比值
	struct B_A {
		int a, b;	//迎风面形心处宽度（尺度较窄的方向），风压段的前后面间深度，mm
		B_A() {	a = b = 0;	}
	}b2aFR,b2aSD;
	double B_AFront;			//正面塔架迎风面与背风面之间距离/塔架迎风面宽度
	double B_ASide;				//侧面塔架迎风面与背风面之间距离/塔架迎风面宽度
	double BetaZ;				//风振系数或称风荷载调整系数（βz）
	double BetaZForFoundation;	//用于基础设计的风振系数或称风荷载调整系数（βz）
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	void CleanInvalidObjRef(IModel* pBelongModel);
	//属性栏
	DECLARE_PROP_FUNC(CWindSegment);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);
};
class WINDSEG_FILTER
{
	GECS cs,backcs;	//前侧挡风面局部坐标系（模型坐标系下定义，法线为挡风面法线）
	GECS prjucs;	//风压段轮廓顶点投影面相对坐标系，用于计算构件单元的投影挡风面积
	double minZ,maxZ;
	POLYGON frontrgn;	//前挡风面多边形区域
	POLYGON region;		//投影多边形区域
	CFGWORD bodyword,legword;
	bool ModuleTogetherWith(CLDSDbObject* pDbObj) const;
public:
	WINDSEG_FILTER(){minZ=maxZ=0;}
	bool InitFilter(CWindSegment* pWindSeg);
	bool InitFilter(CLDSNodePtr* nodes,int count,f3dPoint prjOrg);
	bool IsIncludePoint(const double* point3d);
	bool IsIncludeLine(const double* lineStart,const double* lineEnd);
	bool IsIncludeRod(CLDSLinePart* pRod);
	bool IsLineInFrontWindBox(CLDSLinePart* pRod);
};
struct Lo_CALINFO
{
	BOOL bCalByElemLen;
	double coef;	//Lo=coef*DIST(pLoStart,pLoEnd)
	long hStartNode,hEndNode;
	Lo_CALINFO(){memset(this,0,sizeof(Lo_CALINFO));}
};
struct RODEND_NODES{
	long hStart,hEnd;
	RODEND_NODES(){hStart=hEnd=0;}
};
struct SMC_ITEM{
	//同时受压枚举类型
	static const int SMC_NONE	= 0;	//不考虑无同时受压
	static const int SMC_REFER_MIRX	= 1;//考虑X轴对称单元同时受压
	static const int SMC_REFER_MIRY	= 2;//考虑Y轴对称单元同时受压
	static const int SMC_REFER_START= 4;//考虑与始端节点同端点同时受压
	static const int SMC_REFER_END	= 8;//考虑与终端节点同端点同时受压
	static const int SMC_REFER_2ENDS=16;//考虑与两侧端节点同端点同时受压
	static const int SMC_REFER_UDF	=32;//考虑与指定单元的同时受压
public:
	BYTE cSMCType;		//同时受压类型,首位(高位0x80)为0表示平行轴，为1表示最小轴，具体类型见上面的枚举值
	bool enabled;		//默认为True,个别情况（如参照杆件非受压时）此值为false,
	double ratioL2toL3;	//>0且小于1时表示为交叉斜材,较长分隔与斜材整长间比值,否则不作交叉斜材处理
	Lo_CALINFO LoRc;
	RODEND_NODES relaElem;	//关联参照单元的始末端节点句柄
	BYTE RetrieveSmcType(long hElemStart,long hElemEnd,IModel* pModel);
	SMC_ITEM(){cSMCType=0;enabled=true;ratioL2toL3=0;}
};
#ifndef _ENUM_JOINT_TYPE_
#define _ENUM_JOINT_TYPE_
enum JOINT_TYPE{JOINT_HINGE=0,JOINT_RIGID=1,JOINT_AUTOJUSTIFY=2};		//连接形式
#endif
class CElemInfo;
struct ELEMINFO_KEY
{
	long hStartNode,hEndNode;
	CElemInfo *pElemInfo;
	void Clear(){hStartNode=hEndNode=0;pElemInfo=NULL;}
	ELEMINFO_KEY(){Clear();}
};
class CElemInfo : public CLDSObject
{
	static long globalId;
public:	//不需要存储的属性
	//long Id;	//仅用于图形显示及属性栏的临时标识，所以只要保证同一时间点全局唯一即可
	long tag;
public://只读属性
	long m_hParentRod;
	long m_hStartNode,m_hEndNode;
	BYTE ciFreedomStart,ciFreedomEnd;	//临时存储始末端实际有限元计算时的自由度　wjh-2017.7.14
	//int start_joint_state,end_joint_state;	//0.铰接;1.刚接;2.强制铰接
	//BOOL m_bValidStartRestStiff,m_bValidEndRestStiff;	//是否有承受部分转动约束的能力
	struct RELATIVE_ELEM
	{
		bool bFatherElem;	//此单元是否为对称关联单元中的父单元，即节点号综合较小的单元
		ELEMINFO_KEY xFatherElem,xElemMirX,xElemMirY,xElemMirZ;
		//CElemInfo *pFatherElem,*pElemMirX,*pElemMirY,*pElemMirZ;
	}relaElem;
public:	//可更改属性
	char cCalLenType;	//0.智能判断;1~4.塔腿构造;5.指定平行轴和最小轴验算长度
	RODEND_NODES horiElem;			//横连杆件
	Lo_CALINFO LoRyo,LoRx;	//最小轴、平行轴及同时受压时的计算长度信息
	SMC_ITEM smcItems[3];	//最多考虑三根同时受压参照杆件单元（0号交叉斜材1-2号同端节点材及含）
	CElemInfo();
	bool IsSpecLoInfo(){return cCalLenType==5;}
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	short ElemType(CLDSLinePart* pParentRod=NULL);		//0:二力杆或经典梁；1:柔索
	int StartJointState(CLDSLinePart* pParentRod=NULL);	//0.铰接;1.刚接;2.强制铰接
	int EndJointState(CLDSLinePart* pParentRod=NULL);	//0.铰接;1.刚接;2.强制铰接
	int StartForceType(CLDSLinePart* pParentRod=NULL);
	int EndForceType(CLDSLinePart* pParentRod=NULL);
	int LoRyoCalType();	//0.单元长度;1.指定节间;2.不考虑
	int LoRxCalType();	//0.单元长度;1.指定节间;2.不考虑
	int LoCalType(Lo_CALINFO& LoInfo);
	int GetSmcItemsNum(int *scmItemsTag=NULL);	//得到同时受压考虑的项数并对受压项进行标记
	static long GetNewId();
	static void ClearInitialGlobalId(){globalId=0;}
	//属性管理
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CElemInfo,propHashtable)		//定义属性项更新函数
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值
	static void InitElemInfoPropHashtable();
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
};
typedef CElemInfo* ELEMINFO_PTR;
typedef struct tagDESIGNODDMENT{
	long m_hRefPart1,m_hRefPart2;
	double m_fCollideDist;
	int m_iOddCalStyle;		//0.表示根据碰撞参照杆件计算;1.根据螺栓位置计算;2.表示不计算正负头
	tagDESIGNODDMENT(){m_hRefPart1=m_hRefPart2=0;m_iOddCalStyle=0;m_fCollideDist=0;}
}DESIGNODDMENT;
struct POLE_CALLEN_COEF{
	long hRefPole,hHorizPole;	//第一个是同时受压参照杆件，第二个是横连杆
	struct SPEC_LEN{
		long hStartNode,hEndNode;
		double coef;	//长度计算系数
	}minR,paraR;
	//double spec_callenMinR,spec_callenParaR;//指定杆件计算长度，此系数只是在iTypeNo=5时有用，其余情况临时判断
	long iTypeNo;	//0.智能判断;1~4.塔腿构造;5.指定平行轴和最小轴验算长度;6.指定交叉材计算长度
	long iSelRFlag;	//回转半径选择标识（对角钢来说0:表示平行轴；1:表示最小轴)，此参数只在iTypeNo=0时有用，其余时候临时判断
	//版本控制日志相关方法
#if defined(__PART_DESIGN_INC_)&&!defined(__DRAWING_CONTEXT_)&&!defined(__IGNORE_VERSION_COMPARE_)
	//virtual bool IsIdentical(void* pOldObj);
	bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents);
#endif
};

//杆件端头与其它构件之间，含螺栓信息在内的连接信息
class CConnectInfo{
public:
	CConnectInfo();
	~CConnectInfo();
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void CopyBasicBoltInfo(CConnectInfo &src);	//含螺栓级别、数量、规格、排数等信息
	BYTE m_iConnectWing;	//角钢连接肢（如0:角钢双肢连接;1:X肢连接;2:Y肢连接）
	BYTE m_iConnectType;	//对于钢管0．表示法兰连接;1.单剪(槽型)插板连接;2.双剪(U型或十字)插板连接
	char grade[5];	//螺栓级别4.8;5.8;6.8;8.8;10.9
	long d;					//螺栓直径（规格）
	//-------VVVV---螺栓数量----VVVV--------------
	union{
		long uiLegacyN;	//long N;
		WORD wnConnBoltN;	//杆件基准连接螺栓数(角钢单肢连接数）
	};
	WORD get_wnSummBoltN();
	WORD set_wnSummBoltN(WORD cnBoltCount);
	//力学计算时的单剪螺栓计算总数，0表示取同wnConnCount
	__declspec(property(put=set_wnSummBoltN,get=get_wnSummBoltN)) WORD wnSummBoltN;
	WORD get_wnDualShearConnBoltN();
	//接头处双剪螺栓数，一般为单剪螺栓数的一半 wjh-2019.6.24
	__declspec(property(get = get_wnDualShearConnBoltN)) WORD wnDualShearConnBoltN;
	//-------^^^^---螺栓数量----^^^^--------------
	long rows;				//螺栓布置排数
	double bearloadcoef;	//螺栓承载系数
	union{
		char flNo[16];		//连接法兰时的法兰系列号
		long idAngleJoint;	//当主杆件为角钢时有效，表示关联的接头标识(CTower::hashAngleJoint中的键值）
	};
	long m_hCoupleRod;      //连接法兰时记录接头连接的杆件句柄
	//仅用于临时记录特殊的螺栓间距及端距，不存储临时使用 wht 11-04-26
	struct tagCHIEF_LSSPACE{
		long SingleRowSpace;	//单排间距
		long DoubleRowSpace;	//双排间距
		long EndSpace;			//端距
		tagCHIEF_LSSPACE(){memset(this,0,sizeof(tagCHIEF_LSSPACE));}
	}LsSpace;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	double maxTension;		//杆件端头的最大连接轴向拉力（拉为正，压为负）,N
	double maxCompression;	//杆件端头的最大连接轴向压力（拉为正，压为负）,N
	double maxMoment;		//杆件端头的最大连接弯矩,N*mm;
#endif
};
struct SIZE_WTM{
	char cMaterial;
	BYTE idClsType;		//构件类型标识
	char cSubClassType;	//角钢的子类型,'L':单角钢; 'D'
	double width,thick;	//角钢或组合角钢肢宽肢厚，槽钢宽高，钢管直径及壁厚，扁钢宽厚，柔索直径
	void Set(char material='S',double size_wide=0,double size_thick=0,int idPartClsType=CLS_LINEANGLE,int subClassType='L')
	{
		cMaterial=material;
		idClsType=(BYTE)idPartClsType;
		cSubClassType=subClassType;
		width=size_wide;
		thick=size_thick;
	}
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer);
};
struct SIZESPEC{
	char spec[MAX_TEMP_CHAR_50],wireType[MAX_TEMP_CHAR_50];
	double wide,thick,height,wideUpSide,thick2;
	long idClassType;
	char cSubClassType;
	SIZESPEC(){strcpy(spec,"");strcpy(wireType,"");wide=thick=thick2=height=wideUpSide=0;idClassType=CLS_LINEANGLE;cSubClassType='L';}
#if defined(__PART_DESIGN_INC_)&&!defined(__DRAWING_CONTEXT_)&&!defined(__IGNORE_VERSION_COMPARE_)
	bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents);
#endif
	bool IsEqual(const SIZESPEC& size);
};
struct ANGLEWINGNORM{
	WORD wLayoutStyle;	//肢摆放方式1、4为外铁，2、3为里铁
	GEPOINT viewNorm;	//工作面法向
	long hRefPoleArr[2];
};
class CLDSLineTube;
class CRodPosPara;
class CLDSGeneralPlate;
class CLDSLinePart : public CLDSPart{
protected:
	static const BYTE CLEAR_BUFF_OF_RELA_PARTS	=0x01;
	static const BYTE DISABLE_RECORD_RELA_PARTS =0x02;
	static BYTE _localUpdateMovePartsFlag;	//0x01:清除关联构件记录缓存;0x02:临时阻止记录角钢位置变动关联构件
	static CHashSet<CLDSBolt*> _localHashModifiedBolts;
	static CHashSet<CLDSGeneralPlate*> _localHashModifiedPlates;
	static CHashSet<CLDSLineAngle*> _localHashModifiedAngles;
	static CHashSet<CLDSLineTube*>	_localHashModifiedTubes;
public:
	static double ODDMENT_MINIMAL_MODIFIED;		//正负头最小变动值，mm
	static double RODPOSITION_MINIMAL_MODIFIED;	//杆件端点位置移动最小监测值，mm
	class MOVEROD_STACK{
		bool m_bTerminateStack;
		BYTE m_cbOldUpdateMovePartsState;
	public:
		MOVEROD_STACK();
		MOVEROD_STACK(bool enable);
		~MOVEROD_STACK();
		bool Enable();	//返回旧状态
		bool Disable();	//返回旧状态
		void RestoreStack();
	};
	static BYTE GetMovePartsUpdateFlag();
	static BYTE SetMovePartsUpdateFlag(BYTE cbFlag=0x01);
	static void ClearModifiedPartsBuff();
	static void ClearUpdateMovePartsBuff();
	static CLDSBolt* EnumFirstRelaMoveBolt();	//遍历第一颗因最近一次CLDSLineAngle::CalPosition影响到的位移螺栓
	static CLDSBolt* EnumNextRelaMoveBolt();	//遍历下一颗因最近一次CLDSLineAngle::CalPosition影响到的位移螺栓
	static CLDSGeneralPlate* EnumFirstRelaMovePlate();	//遍历第一颗因最近一次CLDSLineAngle::CalPosition影响到的位移钢板
	static CLDSGeneralPlate* EnumNextRelaMovePlate();	//遍历下一颗因最近一次CLDSLineAngle::CalPosition影响到的位移钢板
	static bool RemoveRelaModifiedPart(long hPart);		//移除从构件变更表中移除指定构件（如在Undo/Redo中删构件时需此操作）
	static bool IsHasModifiedRods();
	static bool IsHasRodRelaModifiedParts();	//是否存在因杆件发生变动而需要更新实体的构件 wjh-2018.2.22
protected:
	CLDSPart* m_pOriginalPart;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLsRefList *pLsRefList;
	double start_oddment,end_oddment;
#endif
	BOOL m_bLockStart,m_bLockEnd,m_bLockStartPos,m_bLockEndPos;
	long m_hArcLift;
	f3dArcLine baseline;
	GEPOINT m_xStartBeforeLift,m_xEndBeforeLift;
	virtual bool ArcLift(){return false;}	//进行预拱计算
	DWORD _uStatMatNo;				//统材组号
	SIZESPEC size;
	struct NODE_REF{
		bool bActiveHandle;	//false:pNode值有效;true:hNode有效　wjh-2017.8.21
		union{
			long hNode;
			CLDSNode *pNode;
		};
		NODE_REF(){hNode=0;bActiveHandle=false;}
	}xStartNode,xEndNode;
	virtual char get_ciLegQuad();
public:	//公用属性
	bool get_blGroupSonAngle();
	__declspec(property(get = get_blGroupSonAngle)) bool blGroupSonAngle;
	virtual f3dPoint WorkPlaneNorm(){return ucs.axis_z;}
	READONLY_PROPERTY(CLDSPart*,pDelegatePart);
	VIRTUAL_GET(pDelegatePart){return m_pOriginalPart;}
	PROPERTY(double,Thick); 
	virtual GET(Thick){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.thick;
		else
			return pMotherObj->Thick;
	}
	virtual SET(Thick){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.thick = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->Thick=value;
		}
	}
	char* GetSpec(){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.spec;
		else
			return pMotherObj->GetSpec();
	}
	char* SetSpec(char* spec)
	{
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		size.spec[MAX_TEMP_CHAR_50-1]=0;
		strncpy(size.spec,spec,MAX_TEMP_CHAR_50-1);
		if(pMotherObj!=this)
			pMotherObj->SetSpec(size.spec);
		return size.spec;
	}
	char* GetWireType(){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.wireType;
		else
			return pMotherObj->GetWireType();
	}
	char* SetWireType(char* wireType)
	{
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		size.wireType[MAX_TEMP_CHAR_50-1]=0;
		strncpy(size.wireType,wireType,MAX_TEMP_CHAR_50-1);
		if(pMotherObj!=this)
			pMotherObj->SetWireType(size.wireType);
		return size.wireType;
	}
	PROPERTY(double,size_wide); 
	virtual GET(size_wide){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.wide;
		else
			return pMotherObj->size_wide;
	}
	virtual SET(size_wide){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.wide = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->size_wide=value;
		}
	}
	PROPERTY(double,size_thick); 
	virtual GET(size_thick){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.thick;
		else
			return pMotherObj->size_thick;
	}
	virtual SET(size_thick){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.thick = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->size_thick=value;
		}
	}
	PROPERTY(double,size_thick2); 
	virtual GET(size_thick2){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.thick2;
		else
			return pMotherObj->size_thick2;
	}
	virtual SET(size_thick2){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.thick2 = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->size_thick2=value;
		}
	}
	PROPERTY(double,size_height); 
	virtual GET(size_height){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.height;
		else
			return pMotherObj->size_height;
	}
	virtual SET(size_height){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.height = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->size_height=value;
		}
	}
	PROPERTY(double,size_wideUpSide); 
	virtual GET(size_wideUpSide){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.wideUpSide;
		else
			return pMotherObj->size_wideUpSide;
	}
	virtual SET(size_wideUpSide){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.wideUpSide = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->size_wideUpSide=value;
		}
	}
	PROPERTY(long,size_idClassType); 
	virtual GET(size_idClassType){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.idClassType;
		else
			return pMotherObj->size_idClassType;
	}
	virtual SET(size_idClassType){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.idClassType = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->size_idClassType=value;
		}
	}
	PROPERTY(char,size_cSubClassType);
	GET(size_cSubClassType);
	/*{ //由于cSubClassType主要用于组合角钢子类型定义，该属性转移至 lds_co_part.cpp中定义 wjh-2019.12.31
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.cSubClassType;
		else
			return pMotherObj->size_cSubClassType;
	}*/
	SET(size_cSubClassType);
	/*{ //由于cSubClassType主要用于组合角钢子类型定义，该属性转移至 lds_co_part.cpp中定义 wjh-2018.7.5
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		size.cSubClassType=value;
		if(pMotherObj!=this)
			pMotherObj->size_cSubClassType=value;
	}*/
	PROPERTY(DWORD,m_uStatMatNo);
	GET(m_uStatMatNo){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return _uStatMatNo;
		else
			return pMotherObj->m_uStatMatNo;
	}
	SET(m_uStatMatNo){
		if (value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			_uStatMatNo=value;
			if(pMotherObj!=this)
				pMotherObj->m_uStatMatNo=value;
		}
	}
	PROPERTY(CLDSNode*,pStart);
	GET(pStart){
		return xStartNode.bActiveHandle?NULL:xStartNode.pNode;
	}
	SET(pStart){
		if (value!=xStartNode.pNode)
		{
			xStartNode.pNode=value;
			xStartNode.bActiveHandle=false;
			SetModified();
		}
	}
	PROPERTY(CLDSNode*,pEnd);
	GET(pEnd){
		return xEndNode.bActiveHandle?NULL:xEndNode.pNode;
	}
	SET(pEnd){
		if (value!=xEndNode.pNode)
		{
			xEndNode.pNode=value;
			xEndNode.bActiveHandle=false;
			SetModified();
		}
	}
	PROPERTY(long,hStartNode);
	GET(hStartNode){
		return xStartNode.bActiveHandle?xStartNode.hNode:(xStartNode.pNode!=NULL?xStartNode.pNode->handle:0);
	}
	SET(hStartNode){
		if (value!=xStartNode.hNode)
		{
			xStartNode.hNode=value;
			xStartNode.bActiveHandle=true;
			SetModified();
		}
	}
	PROPERTY(long,hEndNode);
	GET(hEndNode){
		return xEndNode.bActiveHandle?xEndNode.hNode:(xEndNode.pNode!=NULL?xEndNode.pNode->handle:0);
	}
	SET(hEndNode){
		if (value!=xEndNode.hNode)
		{
			xEndNode.hNode=value;
			xEndNode.bActiveHandle=true;
			SetModified();
		}
	}
public:
	struct USERDEF_DATA{
		double E;		//MPa
		double area;	//mm2
		double ft;		//抗拉强度设计值(MPa)
		double fc;		//抗压强度设计值(MPa)
		double unit_weight;	//kg/m
		double pre_stress;	//预应力Pa
	}userdef;
	bool m_bSpecAngleWingNorm;
	ANGLEWINGNORM des_wing_norm;
	// --------- 1.1. 螺栓 -----------
public:	//连接螺栓信息
	CConnectInfo connectStart,connectMid,connectEnd;
	struct ENDBOLTS{
		bool dualConnect;
		char grade[5];
		BYTE n16,n20,n24,nX;//M16螺栓数,M20螺栓数,M24螺栓数
		double Avf,Ahf;		//螺栓有效承剪面积,孔壁承压面积
		ENDBOLTS();
		ENDBOLTS(WORD d,WORD N,const char* grade);
		void Init(WORD d,WORD N,const char* grade);
		void Clear(){Avf=Ahf=n16=n20=n24=nX=0;}
		DECLARE_READONLY_VIRTUAL_PROPERTY(CXhChar50,szBoltStr);
	};
	virtual bool GetEffectiveEndBolts(ENDBOLTS* pEndBolts,bool bRetrieveStart=true){return false;}
	virtual bool IntelliUpdateConnBoltsBySummBolts();	//根据螺栓计算数智能更新(单肢)连接数
	WORD BoltsNumOn1Wing(BOOL bStart);
	WORD SetBoltsNumOn1Wing(int n,BOOL bStart);
	WORD get_nStartBoltNumOn1Wing(){return connectStart.wnConnBoltN;}
	WORD set_nStartBoltNumOn1Wing(WORD nNum){return connectStart.wnConnBoltN=nNum;}
	__declspec(property(put=set_nStartBoltNumOn1Wing,get=get_nStartBoltNumOn1Wing)) WORD wnStartConnBoltN;
	WORD get_nEndBoltNumOn1Wing(){return connectEnd.wnConnBoltN;}
	WORD set_nEndBoltNumOn1Wing(WORD nNum){return connectEnd.wnConnBoltN=nNum;}
	__declspec(property(put=set_nEndBoltNumOn1Wing,get=get_nEndBoltNumOn1Wing)) WORD wnEndConnBoltN;
public:
	int start_force_type,end_force_type;	//角钢两端受力状况CENTRIC_FORCE,ECCENTRIC_FORCE
	//0：完全铰接JOINT_HINGE；1：部分刚接JOINT_RIGID（限制除扭转外的转动约束）,2：自动判断JOINT_AUTOJUSTIFY
	int start_joint_type,end_joint_type;
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	BOOL m_bActivePole;	// 对于承拉杆承拉时被激活,承压时被移除
	BOOL m_bAutoCalHoleOutCoef;	//自动计算螺栓减孔数，否则为指定螺栓减孔数
	POLE_CALLEN_COEF CalLenCoef;
	/*
		控制规则0:未知1:表示长细比控制2:受拉强度控制
		3:受压强度控制4:表示稳定5:统材号控制6:螺栓数量
	*/
	void* pCtrlElem;			//控制杆件规格的单元，为NULL时表示是统材号控制
	short m_iElemType;			//0表示自动判断;1表示二力杆桁架;2表示经典梁单元;3表示柔索单元
	union{
		struct{	//当规格库中缺当当前规格的物料时,临时存储原物料规格明细
			BYTE _cSubClsType;
			BYTE _ciThick;
			WORD _wiWidth;
		}xInheritSize;
		int iInitialSizeIndex;	//用于单向继承选材时存储，选材前杆件的初始规格排序号（临时存储）
	};
	PROPERTY(char,cInheritSubClsType);
	GET(cInheritSubClsType){return xInheritSize._cSubClsType&0x7f;}
	SET(cInheritSubClsType){xInheritSize._cSubClsType=value;}
	PROPERTY(BYTE,ciInheritThick);
	GET(ciInheritThick){return xInheritSize._ciThick;}
	SET(ciInheritThick){xInheritSize._ciThick=value;}
	PROPERTY(WORD,wiInheritWidth);
	GET(wiInheritWidth){return xInheritSize._wiWidth;}
	SET(wiInheritWidth){xInheritSize._wiWidth=value;}
	PROPERTY(bool,bSpecInheritSizeDetails);
	GET(bSpecInheritSizeDetails){return (xInheritSize._cSubClsType&0x80)>0;}
	SET(bSpecInheritSizeDetails){
		if(value)
			xInheritSize._cSubClsType|=0x80;
		else
			xInheritSize._cSubClsType&=0x7f;
	}
	int iSizeIndex;			//用于按照规格排序比较规格大小（临时存储）
	AUX_POLE_SEL_METHOD sel_mat_method;	//塔材选材方式
	double bear_load_coef;		//辅助材支撑所支撑杆件内力的
	BOOL m_bNeedSelMat;			//需要选择材质
	BOOL m_bNeedSelSize;		//需要选择规格
	double m_fWeightRaiseCoef;	//重力放大系数，考虑联板、螺栓等对重力的放大作用
	double hole_out;			//杆件上最多螺栓减孔数
	//第1位：-1.不按腿部V斜材计算;0.根据图层名判断;1.强制性按腿部V斜材计算;根据《技术规定》腿部斜材计算长度应乘1.2增大系数 wjh-2017.4.1
	//第2位：true，受力材（参预有限元分析），但需要根据辅材进行验算 wjh-2019.7.9
	char m_cbRodFlag;	//0x01
	bool get_bLegVRod();
	bool set_bLegVRod(bool blLegVRod);
	__declspec(property(put=set_bLegVRod,get=get_bLegVRod)) bool m_bLegVRod;
	bool get_blCheckByBrace();
	bool set_blCheckByBrace(bool blBraceRod);
	__declspec(property(put=set_blCheckByBrace,get=get_blCheckByBrace)) bool m_bCheckByBrace;

	double GetElasticityModulus(double para=1.0);	//para用于计算材料非线性弹性模量(如应力等),MPa
	virtual double GetWorkI(int para=0);	//返回杆件指定（角钢时para=0平行轴或para=1最小轴）惯性矩(mm4)
	virtual double GetWorkR(int para=0);	//返回杆件指定（角钢时para=0平行轴或para=1最小轴）回转半径(mm)
	virtual double GetWorkW(int para=0);	//返回杆件指定（角钢时para=0平行轴或para=1最小轴）回转半径(mm3)
	double CalSpecLamdaLenMinR();
	double CalSpecLamdaLenParaR();
	virtual BOOL IsFatherObject();
	virtual BOOL GetFatherMirObj(RELATIVE_OBJECT *pRelaObj);
	//线缆专用查询函数
	double m_fPreTension;			//拉线锚固端安装预张力,N
#if defined(__TSA_)||defined(__LDS_)//因用到了GetSysPath函数，不适宜于在MapLma.arx中调用
	double GetWireArea();			//线缆截面积,mm2
	double GetWireUnitWeight();		//单位长重量,kg/km
	double GetWireForcePermit();	//额定抗拉力,N
	double GetWireElasticCoef();	//弹性系数(模量),MPa
	double GetWireExpansionCoef();	//温度线膨胀系数,
#endif
#endif
	BOOL IsAuxPole();	//是否是辅助材
	BOOL IsPostRod();	//是否为柱杆(即主材)
	BOOL IsPrimaryRod(bool bOnlyBevelRod=false);	//是否为柱普通受力杆(即斜材横材等)
	DESIGNODDMENT desStartOdd,desEndOdd;
	BOOL bHuoQuStart,bHuoQuEnd;	//始/终端点是否为火曲端
	THANDLE huoqu_handle;		//若是火曲杆件段时此项才有效，表示其所归属的火曲杆件
	DWORD dwStartPermission,dwEndPermission;	//始终端的修改权限
	static BOOL bDisplayLineVecMark;
public:	//影射构件相关函数
	static const BYTE LINKNODE_NULL		= 0;	//无连接节点
	static const BYTE LINKNODE_ACTUAL	= 1;	//实际存在的节点
	static const BYTE LINKNODE_SHADOW	= 2;	//影射源杆件存在连接节点,但本身不存在连接节点信息

	virtual bool IsBelongTo(int iClassTypeId)const;
	virtual int StartLinkNodeType();
	virtual int EndLinkNodeType();
	virtual f3dPoint GetDatumPos(CLDSNode* pNode);
	//起始连接节点的模型(影射)坐标
	virtual f3dPoint StartLinkNodePos(bool bArcLiftPos=true);
	//终止连接节点的模型(影射)坐标
	virtual f3dPoint EndLinkNodePos(bool bArcLiftPos=true);
	virtual bool CalShadowRodPos();//BOOL bStart,CLDSArcLift *pLift);
	virtual bool ShadowDesign();
public:
	f3dPoint GetMirPickPos(int start0_end1);
	//对于杆件0.完全对称一致;1.除实体坐标外对称一致;2.拾取点处射线同向对称;3.拾取点处射线异向对称
	virtual BYTE MirSimilarGradeWith(CLDSPart *pPart,const MIRMSG& msg,const double *pPickPos=NULL);
	virtual void TransToCS(GECS& cs);
	virtual void Offset(const double* offset);
	virtual void CopyProperty(CLDSDbObject* pObj);
	SIZESPEC GetSizeSpec();
	void SetSizeSpec(SIZESPEC size_info);
	virtual CRodPosPara* DesStartPos(){return NULL;}
	virtual CRodPosPara* DesEndPos(){return NULL;}
	void UpdateMirLinePartEndNode(MIRMSG mirmsg,CLDSNode *pNewStart,CLDSNode *pNewEnd);
	void UpdateRodEndNodeRelation(CLDSNode *pOldNode,CLDSNode *pNewNode);
	//更新杆件连接信息 wht 09-12-24
	virtual void UpdateConnectInfo(CLDSNode *pNode){return;}
	virtual void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	virtual void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	//bReadForCheck表示是否读取为了进行模型比对检查(仅读取最少量校审相关信息)
	virtual void FromCdfBuffer(CBuffer &buffer,long version=0,bool bReadForCheck=false);
	virtual void ToCdfBuffer(CBuffer &buffer,long version=0);	
	//自协同中心通过管道读取数据
	virtual void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	//通过管道向协同中心写入数据
	virtual void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	virtual UCS_STRU GetUnifiedACS();	//返回统一位置的构件装配(局部)坐标系
	virtual UCS_STRU BuildUCS(){return ucs;}
	virtual double GetArea();	//单位为mm2
	//bSolidLength表示计算长度时按单线计算还是按实体计算,主要是力学分析时统计重量用
	virtual double GetWeight(BOOL bSolidLength=TRUE);	//单位为kg
	virtual char* GetBriefPartTypeMark();
	virtual int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	virtual double GetWidth();//{return size_wide;}	//单位为mm
	virtual double GetThick();//{return size_thick;}	//单位为mm
	virtual double GetHeight();//{return size_height;}	//单位为mm
	virtual double GetLength();	//单位为mm
	virtual double GetThick2();
	virtual void ClearFlag();
	virtual long ArcLiftHandle(){return m_hArcLift;}
	virtual long SetArcLift(long hArcLift){m_hArcLift=hArcLift;SetModified();return m_hArcLift;}
	virtual void CalPosition(bool bIgnoreRepeatFlag=false);
	virtual bool IterCalPosition(double tolerance=0.5,UINT nMaxIterCount=2);
	CLDSLinePart* MotherLinePart();
	CLDSLinePart();
	~CLDSLinePart();
	f3dPoint StartBeforeLift(){return m_xStartBeforeLift;}
	f3dPoint EndBeforeLift(){return m_xEndBeforeLift;}
	f3dPoint Start(bool bArcLift=true) const;
	void SetStart(f3dPoint start,bool bArcLift=true);
	f3dPoint End(bool bArcLift=true) const;
	void SetEnd(f3dPoint end,bool bArcLift=true);

	READONLY_PROPERTY(GEPOINT,xPosStart);
	GET(xPosStart){return Start();}
	READONLY_PROPERTY(GEPOINT,xPosEnd);
	GET(xPosEnd){return End();}

	BOOL LockStartPos(){	//用户锁定角钢端点楞线位置使其不变
		if(m_bLockStartPos)
			return FALSE;
		else
			m_bLockStartPos = TRUE;
		return TRUE;
	}
	BOOL IsStartPosLocked(){return m_bLockStartPos;}
	void UnlockStartPos(){m_bLockStartPos = FALSE;}
	BOOL LockEndPos(){		//用户锁定角钢端点楞线位置使其不变
		if(m_bLockEndPos)
			return FALSE;
		else
			m_bLockEndPos = TRUE;
		return TRUE;
	}
	BOOL IsEndPosLocked(){return m_bLockEndPos;}
	void UnlockEndPos(){m_bLockEndPos = FALSE;}
	BOOL LockStart(){	//用于免除重复迭代计算端点楞线位置
		if(m_bLockStart)
			return FALSE;
		else
			m_bLockStart = TRUE;
		return TRUE;
	}
	BOOL IsStartLocked(){return m_bLockStart;}
	void UnlockStart(){m_bLockStart = FALSE;}
	BOOL LockEnd(){	//用于免除重复迭代计算端点楞线位置
		if(m_bLockEnd)
			return FALSE;
		else
			m_bLockEnd = TRUE;
		return TRUE;
	}
	BOOL IsEndLocked(){return m_bLockEnd;}
	void UnlockEnd(){m_bLockEnd = FALSE;}
	BOOL IsCfgWordCompatible(CFGWORD cfgword);	//判断端节点与杆件本身配材号是否一致
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsLinePart() const{return true;}
	virtual bool IsPolyPart() const{return false;}
	virtual CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
	//力学分析时的对称杆件(只考虑单线端节点坐标对称)
	CLDSLinePart *GetMirRod(MIRMSG mirmsg,double epsilong=EPS);
	bool IsEqualWith(CLDSLinePart *pLinePart,double epsilon=1.0,char *error_info=NULL,int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	double GetDatumLength();
	void GetMirLinePartsByPos(MIRMSG mirmsg,LINEPARTSET &mirRodSet);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	virtual void CalStartOddment(char ciAutoCalMode = 0,int liInitOddment=0);	//0.普通碰撞规则算;1.强制性把指定负头情况按碰撞规则算;2.按允许切角时保证切角边距情况的碰撞规则算
	virtual void CalEndOddment(char ciAutoCalMode = 0,int liInitOddment=0);		//0.普通碰撞规则算;1.强制性把指定负头情况按碰撞规则算;2.按允许切角时保证切角边距情况的碰撞规则算
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart){return false;}
#endif
	virtual double startOdd(bool bToFL=false);// const{return start_oddment;}
	virtual double endOdd(bool bToFL=false);// const{return end_oddment;}
	double get_StartOddment(){return startOdd(false);}
	double get_StartOddment2FL(){return startOdd(true);}
	__declspec(property(get=get_StartOddment)) double dfStartOdd;
	__declspec(property(get=get_StartOddment2FL)) double dfStartOdd2FL;
	double get_EndOddment(){return endOdd(false);}
	double get_EndOddment2FL(){return endOdd(true);}
	__declspec(property(get=get_EndOddment)) double dfEndOdd;
	__declspec(property(get=get_EndOddment2FL)) double dfEndOdd2FL;
	BOOL SetStartOdd(double start_odd,bool bToFL=false);//BOOL bCorrectLsPosPara=FALSE,BOOL bCorrectPlateProfile=FALSE,bool bToFL=false);
	BOOL SetEndOdd(double end_odd,bool bToFL=false);//BOOL bCorrectLsPosPara=FALSE,BOOL bCorrectPlateProfile=FALSE,bool bToFL=false);
	virtual long get_hStartDatumRod();
	virtual long get_hEndDatumRod();
	long get_StartDatumRod(){return get_hStartDatumRod();}
	long get_EndDatumRod(){return get_hEndDatumRod();}
	__declspec(property(get=get_StartDatumRod)) long hStartDatumRod;
	__declspec(property(get=get_EndDatumRod)) long hEndDatumRod;
	virtual CLsRefList *GetLsRefList();
	CLsRef* GetFirstLsRef(){return pLsRefList->GetFirst();}
	CLsRef* GetNextLsRef(){return pLsRefList->GetNext();}
	virtual CLsRef* AppendLsRef(CLsRef &ls_ref,BOOL bNearLsCheck=TRUE);
	CLsRef* AppendStartLsRef(CLsRef ls_ref,BOOL bNearLsCheck=TRUE);
	CLsRef* AppendMidLsRef(CLsRef ls_ref,BOOL bNearLsCheck=TRUE);
	CLsRef* AppendEndLsRef(CLsRef ls_ref,BOOL bNearLsCheck=TRUE);
	int GetLocalLsCount(short start1_end2_mid0=0,double length_scope=0);
	virtual int GetLsCount(short d=0);
	void EmptyLsRef(){ pLsRefList->Empty();}
	CLsRef* FindLsByUcsPos(f3dPoint base,UCS_STRU ucs,int iNormX0Y1Z2,double epsilon=EPS2)
	{return pLsRefList->FindLsByUcsPos(base,ucs,iNormX0Y1Z2,epsilon);}
	CLsRef* FindLsByPos(f3dPoint base,f3dPoint work_norm,double epsilon=EPS2)
	{return pLsRefList->FindLsByPos(base,work_norm,epsilon);}
	virtual CLsRef* FindLsByHandle(THANDLE h);
	friend BOOL IsLsRefListEqual(CLsRefList *pLsRefList,const UCS_STRU &ucs,
					   CLsRefList *pViceLsRefList, const UCS_STRU &vice_ucs,
					   double epsilon,char *error_info,long *pH);
#endif
	//同步属性
	virtual bool IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr);
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部位置对称无关的属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
	//属性管理
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSLinePart,propHashtable)  //定义属性项更新函数
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值
	static void InitLinePartPropHashtable();
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	//版本控制日志相关方法
#if defined(__PART_DESIGN_INC_)&&!defined(__DRAWING_CONTEXT_)&&!defined(__IGNORE_VERSION_COMPARE_)
	//virtual bool IsIdentical(void* pOldObj);
	virtual bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents);
#endif
};
class CLDSCable : public CLDSLinePart{
public:
	CLDSCable();
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
};
//仅在受力时作为杆件传力使用的构件，实际可能表示一组构件如连接基座
class CLDSDelegateRod : public CLDSLinePart{
	CLDSDelegateRod();
	virtual ~CLDSDelegateRod(){;}
};
/*struct OBJECT_CS
{
	GECS cs;		//对指定坐标系类型用于存储坐标系几何数据，对于参数化坐标系只是临时储存坐标系计算结果
	long m_idParaCS;	//如为参数化坐标系，此属性代表对应的参数化坐标系的标识ID
	OBJECT_CS(){m_idParaCS=0;}
};
enum DATUM_PLANE_TYPE{
	PLANE_TYPE_XY=0,	//X为基准轴，Y为辅助轴
	PLANE_TYPE_YX,		//Y为基准轴，X为辅助轴
	PLANE_TYPE_YZ,		//Y为基准轴，Z为辅助轴
	PLANE_TYPE_ZY,		//Z为基准轴，Y为辅助轴
	PLANE_TYPE_ZX,		//Z为基准轴，X为辅助轴
	PLANE_TYPE_XZ,		//X为基准轴，Z为辅助轴
};
*/
class CParaCS
{
public:	//静态参数及相关结构体定义
	//坐标右手正交化基准平面(轴)顺序
	static const BYTE DP_XY=0;	//X为基准轴，Y为辅助轴
	static const BYTE DP_YX=1;	//Y为基准轴，X为辅助轴
	static const BYTE DP_YZ=2;	//Y为基准轴，Z为辅助轴
	static const BYTE DP_ZY=3;	//Z为基准轴，Y为辅助轴
	static const BYTE DP_ZX=4;	//Z为基准轴，X为辅助轴
	static const BYTE DP_XZ=5;	//X为基准轴，Z为辅助轴
	//坐标系定义类型
	static const BYTE CS_PARAM		=0;	//通过参数化定义确定坐标系
	static const BYTE CS_MIRTRANS	=1;	//通过对原始坐标系进行空间对称(旋转)变换获取
	static const BYTE CS_TRANSLATE	=2;	//通过对原始坐标系进行空间平移变换
	//典型对称样式
	static const BYTE CS_MIRX =1;	//基于杆塔模型坐标系X-Z平面对称
	static const BYTE CS_MIRY =2;	//基于杆塔模型坐标系Y-Z平面对称
	static const BYTE CS_MIRZ =3;	//基于杆塔模型坐标系Z轴对称
	struct PARAMETRIC_INFO{	//参数化坐标系定义的定义参数
		//0.参数化定义基点及装配平面;1.对称;2.刚体平移(以desAxisX为偏移方向m_xMirInfo.origin.x为刚体偏移量)
		BYTE m_cDefStyle;
		//0.用户自定义参数化坐标系
		BYTE m_eDatumPlane;	//0.X-Y;1.Y-X;2.Y-Z;3.Z-Y;4.Z-X;5.X-Z 
		CLDSPoint desOrg;	//0.参数化定义基点及装配平面
		CLDSVector desAxisX,desAxisY,desAxisZ;
		//1.对称---相对于母构件的对称信息
		//long m_hMotherPart;	//基准母构件,即原始坐标系
		MIRMSG m_xMirInfo;	//无对称和旋转变时，m_xMirInfo.origin值代表刚体平移量
		PARAMETRIC_INFO(){m_cDefStyle=m_eDatumPlane=0;}
	};
private:
	GECS cs;				//只是临时储存坐标系计算结果
	bool m_bNeedUpdateCS;	//是否需要通过柔性设计参数[param]更新坐标系计算结果[cs]
	PARAMETRIC_INFO param;	//坐标系柔性设计参数
	bool RightHandRuled();
public:
	long id;			//标识id
	CParaCS();
	void SetKey(DWORD key){id=(long)key;}
	/////////////////////////////////////////////////////
	BYTE GetDefStyle(){return param.m_cDefStyle;}
	PARAMETRIC_INFO GetParamInfo(){return param;};
	void SetParamInfo(PARAMETRIC_INFO para_info){param=para_info;m_bNeedUpdateCS=true;}
	void SetModifiedState(/*bool modified=true*/){m_bNeedUpdateCS=true;}
	//指定坐标系时，要求确保输入坐标系的右手螺旋正交及单位化性
	bool SetACS(const GECS& ucs,bool bModifyParam=true);
	bool SetACS(const UCS_STRU& ucs,bool bModifyParam=true);
	GECS GetACS(){	return cs;}
	static BOOL CalMirAcs(GECS& cs,CLDSPart *pMotherPart,const MIRMSG& mirInfo);
	BOOL UpdateACS(CLDSPart *pMotherPart);
	BOOL UpdateACS(IModel* pModel);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	//
	f3dPoint CoordTrans(f3dPoint pos);
	f3dPoint VectorTrans(f3dPoint vec);
	BOOL IsDesAxisX();
	BOOL IsDesAxisY();
	BOOL IsDesAxisZ();
	char *GetAxisDescription(char cAxis);
	static char *GetDatumPlaneCmbItems();
};

typedef CLDSLinePart* POLE_PTR;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
class CLDSArcPart : public CLDSPart{
protected:
	CLsRefList *pLsRefList;
public:
	UINT m_uDisplaySlices;
	double m_fZoomLenCoef;//制造时相对于圆弧基线的长度缩放系数
	double m_fBaseLineRadius;		//弯曲圆弧基线半径
	short m_siLayoutStyle;		//构件截面相对圆弧的摆放方式
	BYTE flag;
	CLDSNode *pStart,*pEnd,*_pCenterPickNode,*_pPlanePickNode;
	f3dPoint arc_start;
	double start_rot_angle;		//圆弧起始角度
	double sector_angle;		//扇形角
	double offset_h,offset_v;	//基准母线距离截面形状特征(标定)原点的横向及竖向位移
	f3dArcLine baseline;
	double start_oddment,end_oddment;
	CLDSPoint des_arc_center,des_arc_start;	//环形构件圆心及起始点
	CLDSVector des_arc_norm;				//环形构件法线方向
	//圆弧构建类型, 0:指定圆弧始末端点及半径1:指定圆弧夹角2:指定圆弧半径3:椭圆弧
	PROPERTY(BYTE,type);
	GET(type);
	SET(type);
	//圆弧半径
	PROPERTY(double,radius);
	GET(radius){return m_fBaseLineRadius;}
	SET(radius){m_fBaseLineRadius=value;}
	//圆弧圆心点或圆心侧拾取点
	PROPERTY(CLDSNode*,pCenterPickNode);
	GET(pCenterPickNode);
	SET(pCenterPickNode);
	PROPERTY(GEPOINT,center);
	GET(center);
	SET(center);
	//圆弧工作平面拾取点
	PROPERTY(CLDSNode*,pPlanePickNode);
	GET(pPlanePickNode);
	SET(pPlanePickNode);
	PROPERTY(GEPOINT,worknorm);
	GET(worknorm);
	SET(worknorm);
public:
	CLDSArcPart();
	~CLDSArcPart();
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	BOOL DesignArcBaseLine();
	BOOL DesignArcBaseLine(CLDSPoint &center,CLDSPoint &start,CLDSVector &workNorm,double rotAngle,double sectorAngle);
	f3dPoint Center(){return baseline.Center();}
	f3dPoint Start(){return baseline.Start();}
	f3dPoint End(){return baseline.End();}
	double Radius(){return baseline.Radius();}
	f3dPoint WorkNorm(){return baseline.WorkNorm();}
	virtual f3dPoint GetDatumPos(f3dPoint pickPos){return pickPos;}
	f3dPoint PositionInAngle(double rad_ang){ return baseline.PositionInAngle(rad_ang); }
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsArcPart() const{return true;}
	virtual double GetWidth(){return 0;}
	virtual double GetThick(){return 0;}
	virtual double GetLength(){return 0;}
	virtual double GetWeight(BOOL bSolidLength=TRUE){return 0;}
	virtual void getUCS(UCS_STRU& ucs);
	virtual GEPOINT GetArcRayNorm(const double* point);		//弧形杆件在pNode节点处的射线方向(圆弧切面法线)
	virtual GEPOINT GetArcTangentVec(const double* point);	//弧形杆件在pNode节点处的切线方向(长度方向切矢量)
	virtual CLsRefList *GetLsRefList(){return pLsRefList;}
	CLsRef* GetFirstLsRef(){return pLsRefList->GetFirst();}
	CLsRef* GetNextLsRef(){return pLsRefList->GetNext();}
	virtual CLsRef* AppendLsRef(CLsRef &ls_ref,BOOL bNearLsCheck=TRUE);
	void EmptyLsRef(){ pLsRefList->Empty();}
	virtual CLsRef* FindLsByHandle(THANDLE h){return pLsRefList->FromHandle(h);}
	friend BOOL IsLsRefListEqual(CLsRefList *pLsRefList,const UCS_STRU &ucs,
					   CLsRefList *pViceLsRefList, const UCS_STRU &vice_ucs,
					   double epsilon,char *error_info,long *pH);
	virtual CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//属性管理
	static int m_iCurDisplayPropGroup;
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;		//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSArcPart,propHashtable)   //定义属性项更新函数
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	static void InitArcPartPropHashtable();
	static int GetPropStr(long id,char* propStr);		//得到属性字符串
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
};
class CLDSArcAngle : public CLDSArcPart{
public:
	double wing_wide,wing_thick;
	CLDSArcAngle();
	~CLDSArcAngle(){;}
public:
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	bool IsEqual(CLDSArcAngle* pMirArcAngle,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	double GetWidth(){return wing_wide;}
	double GetThick(){return wing_thick;}
	double GetLength();
	double GetWeight(BOOL bSolidLength=TRUE);
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	//属性管理
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSArcAngle,propHashtable)  //定义属性项更新函数
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	static void InitArcAnglePropHashtable();
	static int GetPropStr(long id,char* propStr);		//得到属性字符串
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
};

class CLDSArcFlat : public CLDSArcPart
{
public:
	double wide,thick;
	CLDSArcFlat();
	~CLDSArcFlat(){;}
public:
	f3dPoint GetDatumPos(f3dPoint pickPos);
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	bool IsEqual(CLDSArcFlat* pMirArcFlat,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	double GetWidth(){return wide;}
	double GetThick(){return thick;}
	double GetLength();
	double GetWeight(BOOL bSolidLength=TRUE);
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	//属性管理
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSArcFlat,propHashtable)  //定义属性项更新函数
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	static void InitArcFlatPropHashtable();
	static int GetPropStr(long id,char* propStr);		//得到属性字符串
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
};

class CLDSArcSlot : public CLDSArcPart
{
public:
	char spec[MAX_CHAR_GUIGE_16+1];
	double wide,thick,height; //宽度 厚度 高度
	CLDSArcSlot();
	~CLDSArcSlot(){;}
public:
	f3dPoint GetDatumPos(f3dPoint pickPos);
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	bool IsEqual(CLDSArcSlot* pMirArcSlot,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	double GetWidth(){return wide;}
	double GetThick(){return thick;}
	double GetHeight(){return height;}
	double GetLength();
	double GetWeight(BOOL bSolidLength=TRUE);
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	//属性管理
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;		//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSArcSlot,propHashtable)  //定义属性项更新函数
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	static void InitArcSlotPropHashtable();
	static int GetPropStr(long id,char* propStr);		//得到属性字符串
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
};
class CLDSPolyPart : public CLDSPart{
public:
	CLDSPolyPart();
	~CLDSPolyPart();
	CTmaPtrList<CLDSLinePart,CLDSLinePart*> segset;
public:
	double GetWidth();
	double GetThick();
	double GetLength();
	void CopyProperty(CLDSDbObject* pObj);
	virtual double GetWeight(BOOL bSolidLength=TRUE);
	virtual int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	virtual bool IsEqual(CLDSPolyPart *pPart,double epsilon=1.0,char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);
	bool IsEqualPolyAngle(CLDSPolyPart *pPolyJg,double epsilon=1.0,char *error_info=NULL,int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);
	bool IsEqualSegAngle(CLDSLineAngle *pSegJg,CLDSLineAngle *pMirSegJg,BOOL bReverse=FALSE,double epsilon=1.0,char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);
	virtual bool CompareLabelPart(CLDSPart *pPeerPart,MIRMSG* pMirmsg=NULL,BOOL bCmpls=TRUE,PEER_PART *pPeerInfo=NULL,double epsilon=1.0);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0){;}
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0){;}
	bool IsLinePart() const{return false;}
	bool IsPolyPart() const{return true;}
	virtual int WriteProcessBuffer(CBuffer &buffer,bool bSaveSubSerial=false,long version=0); //得到制弯角钢工艺扩展信息buffer
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	UCS_STRU GetUCS(bool bIncOddment=false,bool bReverse=false);
	virtual UCS_STRU GetUnifiedACS(){return GetUCS(true);}
	double startOdd(bool bToFL=false);
	double endOdd(bool bToFL=false);
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart);
#endif
	//根据构件上螺栓孔的先后顺序生螺栓句柄->孔索引标识(以1为基数)映射，返回螺栓孔数量 wjh-2016.1.05
	virtual long DispatchHoleIndexId(CHashList<long>* hashHoleIndexIdByBoltHandle);
	static ATOM_LIST<BOLTHOLE_INFO>boltListX,boltListY;
	static BOOL AnaylyzeAngleBolts(CLDSPart *pPolyAnglePart);
};
struct WING_OFFSET_STYLE{
	BYTE gStyle;		//角钢肢偏移量:0.标准准距;1:g1;2:g2;3:g3;4:自定义
	BYTE operStyle;		//操作类型0.垂向投影;1.X-Y平面平推;2.Y-Z平面平推;3.X-Z平面平推;4.自定义平推面
	double offsetDist;	//仅在gStyle=4时有效
	WING_OFFSET_STYLE(){gStyle=operStyle=0;offsetDist=0.0;}
	void SetOffset(double offset_dist=0,BYTE g_style=4,BYTE oper_style=0);
	void FromBuffer(CBuffer &buffer);
	void ToBuffer(CBuffer &buffer);
	void ToXmlFile(FILE* fp,DWORD schema);
};
class CTubeEndPosPara;
class CJgBerPosPara;
class CMultiOffsetPos;
class CRodPosPara{
protected:
	int m_iPosParaType;	//0:角钢(旧);1:钢管;2:角钢(新)
public:
	CRodPosPara(){m_iPosParaType=0;}
	virtual BOOL IsInDatumJgBer()=0;		//基点是否在基准角钢楞线上
	virtual BOOL IsInDatumJgWingX()=0;	//基点是否在基准角钢X肢上
	virtual BOOL IsInDatumJgWingY()=0;	//基点是否在基准角钢Y肢上
	CJgBerPosPara* AnglePosParaOld(){return (CJgBerPosPara*)this;}
	CTubeEndPosPara* TubePosPara(){return (CTubeEndPosPara*)this;}
	CMultiOffsetPos* AnglePosPara(){return (CMultiOffsetPos*)this;}
};
class CJgBerPosPara : public CRodPosPara{
public:
	/*long datum_pos;			//只在角钢不为节点父角钢时有效，0为父角钢楞线,1为父角钢X肢,2为父角钢Y肢
	  long joint_style;			//0为基准角钢预定准线,1火曲线,2为偏移准线,3为指定基点位置,4节点位置
	  long offset_style;*/		//从基准点偏移方式:0表示投影;1表示沿指定平面平推;2表示指定坐标;3点向直线的投影
	//角钢楞点的计算方式:0:表示两步偏移;1:表示点向线投影;
	//2:表示点向线平推;3:表示直接指定坐标
	long jgber_cal_style;
	//基点的计算方式:0:节点即基点
	// 1:向楞线投影      2:X肢心线投影       3:Y肢心线投影
	// 4:向楞线平推      5:X肢心线平推       6:Y肢心线平推
	// 7:投影楞线火曲线  8:投影X肢心线火曲线 9:投影Y肢心线火曲线
	//10:平推楞线火曲线 11:平推X肢心线火曲线12:平推Y肢心线火曲线
	//13:X肢偏移准线    14:Y肢偏移准线      15:直接指定基点坐标
	long datum_pos_style;
	long datum_to_ber_style;	//根据基点计算楞点的方式:0:表示投影;1:表示平推
	short work_wing_auto0_x1_y2_both3;	//直接指定基点时，自基点偏移时的当前工作肢
	short huoqu_pos_auto_0_x_wing1_y_wing2;	//火曲线位置的判断
	long prj_type;				//0.垂直于肢方向投影 1.平行于肢方向投影
	long offset_X_dist_style;	//X肢0表示偏移量为标准准距,1为g1,2为g2,3为g3,4为自定义
	long offset_Y_dist_style;	//Y肢0表示偏移量为标准准距,1为g1,2为g2,3为g3,4为自定义
	double len_offset_dist;		//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	double datum_offset_dist;	//搭接角钢投影方式时,搭接点自基准肢沿法线的偏量
	double offset_X_dist,offset_Y_dist;	//自定义偏移量时自基准点的偏移量
	double fStartJgLengOffsetX;	//点线投影时起始角钢基准楞点法向偏移量
	double fStartJgLengOffsetY;	//点线投影时起始角钢基准楞点法向偏移量
	double fEndJgLengOffsetX;	//点线投影时终止角钢基准楞点法向偏移量
	double fEndJgLengOffsetY;	//点线投影时终止角钢基准楞点法向偏移量
	double fEccentricDist;		//偏心距==>当角钢搭接在钢管上时才需要此变量 wht 09-10-17
	THANDLE datum_jg_h;			//搭接位置基准角钢
	THANDLE start_jg_h;			//起始角钢句柄
	THANDLE end_jg_h;			//终止角钢句柄
	THANDLE huoqu_jg_h;			//火曲基准角钢句柄
	CLDSPoint datum_point;		//基准点句柄
	f3dPoint face_offset_norm;	//平推面法线
	BYTE cFaceOffsetDatumLine;	//0:楞线;'X':X肢心线;'Y':Y肢心线
	double huoqu_dist;			//火曲线间隙
	double offset_zhun;			//偏移准线的偏移量
	long iStartJgBerStyle,iEndJgBerStyle;	//指定基准角钢的基准端点0:始楞点;1:终楞点
	//平推处理
	BOOL bByOtherEndOffset;		//根据另一端进行平推计算
public:
	CJgBerPosPara();
	BOOL IsFaceOffset();		//是否为平推	wht 10-07-30		
	BOOL IsInDatumJgBer();		//基点是否在基准角钢楞线上
	BOOL IsInDatumJgWingX();	//基点是否在基准角钢X肢上
	BOOL IsInDatumJgWingY();	//基点是否在基准角钢Y肢上
	BOOL IsInHuoQuLine();		//基点是否在火曲线上
	BOOL IsInOffsetZhunLine();	//基点是否在偏移准线上
	int GetDatumType();			//获得基点定位类型
	BOOL ChangeToDatumType(int prj1_faceoffset2_huoqu3and4_offsetzhun5);	//将基点转换为另一类型
	bool ReplaceReferenceHandle(CHandleReplacer& replacer);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
};
//杆件(主要是角钢)楞点定位信息类
class CMultiOffsetPos  : public CRodPosPara
{
public:
	BYTE jgber_cal_style;	//0.参数化定位;1.直接指定楞点坐标;2.简单定位模式
	//A. 初始位置定位方式
	THANDLE datum_jg_h;		//搭接位置基准角钢
	BYTE origin_pos_style;	//0.节点位置;1.基准点;2.基准角钢关键点;3.另一端节点坐标;4.另一端楞点坐标;5.智能识别点
	BYTE cDatumAngleWing;	//角钢'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点(仅在origin_pos_style=2时有效)
	WING_OFFSET_STYLE datumWingOffset;
	CLDSPoint datumPoint;
	//B. 空间复杂运算操作(初始点->基点也可称基点定位)类型: 0.基准点
	// 1:向楞线投影      2:X肢心线投影       3:Y肢心线投影
	// 4:向楞线平推      5:X肢心线平推       6:Y肢心线平推
	// 7:投影楞线火曲线  8:投影X肢心线火曲线 9:投影Y肢心线火曲线
	//10:平推楞线火曲线 11:平推X肢心线火曲线12:平推Y肢心线火曲线
	//13:X肢偏移准线    14:Y肢偏移准线      15:两肢坐标控制
	//16:指定线投影		17:指定线平推		18:旧式控制点投影 19:旧式控制点平推
	BYTE spatialOperationStyle;
	BYTE huoqu_pos_auto_0_x_wing1_y_wing2;	//火曲线位置的判断
	double len_offset_dist;		//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	BYTE len_offset_type;		//len_offset_dist的偏移类型0.基准杆件长度沿向,1.工作面内沿当前杆件垂直方向偏移,'X','Y','Z'
	double datum_offset_dist;	//搭接角钢投影方式时,搭接点自基准肢沿法线的偏量
	BYTE cFaceOffsetDatumLine;	//0:楞线;'X':X肢心线;'Y':Y肢心线，仅spatialOperationStyle==17时有效
	GEPOINT face_offset_norm;	//平推面法线
	THANDLE huoqu_jg_h;		//火曲基准角钢句柄
	double huoqu_dist;		//火曲线间隙
	double offset_zhun;		//偏移准线的偏移量
	union{
		double fEccentricDist;	//偏心距==>当角钢搭接在钢管上时才需要此变量 wht 09-10-17
		double fLenOffsetOfCoordWing;	//两肢坐标控制时，垂向投影肢相对平推肢的向外轴向偏移量 wjh-2016.4.27
	};
	BYTE coordCtrlType;	//0:X-Z主控;1:Y-Z主控;2.X-Y主控(仅在spatialOperationStyle=15时有效)
	BYTE cMainCoordCtrlWing;//角钢主控肢：'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
	WING_OFFSET_STYLE ctrlWing;	//角钢主要控制肢(控制两个坐标分量的肢)
	WING_OFFSET_STYLE otherWing;//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSPoint datumLineStart,datumLineEnd;	//仅在spatialOperationStyle=16,17时有效
	THANDLE hDatumLine;		//基准线，仅在spatiaOperationStyle=16,17时有效，hDatumLine=0时表示定义基准点
	BYTE cMirTransType;		//0,'X','Y','Z',当hDatumLine>0时有效
	//C. 偏移操作(基点->楞点的矢量偏移)，仅适用于搭接杆件端(楞)点的位置计算
	BYTE cWorkWingStyle;//自基点进行偏移时当前角钢的基准肢0:自动判断;1:X肢;2:Y肢;3:双肢;其余自动判断
	WING_OFFSET_STYLE wing_x_offset;	//X肢法向偏移量及方式
	WING_OFFSET_STYLE wing_y_offset;	//Y肢法向偏移量及方式(此时忽略WING_OFFSET_STYLE::operStyle属性)
	//////////////////////////////////////////////////////////
	WING_OFFSET_STYLE& get_xMasterWing();
	WING_OFFSET_STYLE& set_xMasterWing(WING_OFFSET_STYLE xOffsetStyle);
	__declspec(property(put = set_xMasterWing, get = get_xMasterWing)) WING_OFFSET_STYLE& xMasterWing;
	WING_OFFSET_STYLE& get_xSlaveWing();
	WING_OFFSET_STYLE& set_xSlaveWing(WING_OFFSET_STYLE xOffsetStyle);
	__declspec(property(put = set_xSlaveWing, get = get_xSlaveWing)) WING_OFFSET_STYLE& xSlaveWing;
	bool get_blLegayDualFairWingCtrlPoint();	//塔身控制点投影或以楞线为基准的塔身控制点平推 wjh-2020.1.1
	__declspec(property(get = get_blLegayDualFairWingCtrlPoint)) bool blLegayDualFairWingCtrlPoint;
	//////////////////////////////////////////////////////////
	//jgber_cal_style=2简单定位模式时
private:
	int  GetSimpleDatumPoint();
	void SetSimpleDatumPoint(int iSimpleDatumPointType);
public:
	BYTE cbSimpleUdfFlag;	//仅当jgber_cal_style==2时有效,用于表示用户人工干预的控制信息 wjh-2016.3.12
	//基点定位类型(占第1位), 0.智能判断;1.人工指定基点定位类型
	PROPERTY(BOOL,bUdfDatumPointMode);	//依赖角钢的当前搭接肢
	GET(bUdfDatumPointMode){return cbSimpleUdfFlag&0x01;}
	SET(bUdfDatumPointMode){
		if(value)
			cbSimpleUdfFlag|=0x01;	//0000 0001
		else
			cbSimpleUdfFlag&=0xFE;	//1111 1110
	}
	//搭接时搭接于依赖角钢的哪个肢(占第2位), 0.智能判断;1.人工指定搭接依赖肢
	DECLARE_PROPERTY(BOOL,bUdfLappedDatumWing);	//依赖角钢的当前搭接肢
	//目标交线模式(占第3~4位), 0.自动判断;1.楞线;2.(肢)心线;3.火曲线;4.偏移准线 wjh-2016.11.26
	static const BYTE TO_BERLINE	= 0x01;	//连接到楞线	 (对应cbSimpleUdfFlag=0x01)
	static const BYTE TO_AXISLINE	= 0x02;	//连接到X/Y肢心线(对应cbSimpleUdfFlag=0x01)
	static const BYTE TO_BENDLINE	= 0x03;	//连接到火曲线	 (对应cbSimpleUdfFlag=0x02)
	static const BYTE TO_OFFSETZHUN	= 0x04;	//连接到偏移准线 (对应cbSimpleUdfFlag=0x03)
	DECLARE_PROPERTY(BYTE,ciDestLineMode)	//依赖角钢的当前搭接肢
	//各肢控制坐标模式(占第5位), 0.自动判断;1.人工指定各肢控制坐标分量
	PROPERTY(BOOL,bUdfCtrlCoordsMode);
	GET(bUdfCtrlCoordsMode){return (cbSimpleUdfFlag&0x10)>>4;}
	SET(bUdfCtrlCoordsMode){
		if(value)
			cbSimpleUdfFlag|=0x10;	//0001 0000
		else 
			cbSimpleUdfFlag&=0xEF;	//1110 1111
	}
	//搭接依赖角钢的当前连接基准肢
	PROPERTY(BOOL,cSimpleDatumAngleWing);	//依赖角钢的当前搭接肢
	GET(cSimpleDatumAngleWing){return cDatumAngleWing;}
	SET(cSimpleDatumAngleWing){cDatumAngleWing=value;}
	//当前角钢自身的工作肢
	PROPERTY(BOOL,cSimpleSelfWorkWing);	//依赖角钢的当前搭接肢
	GET(cSimpleSelfWorkWing){return cWorkWingStyle;}
	SET(cSimpleSelfWorkWing){cWorkWingStyle=value;}
	PROPERTY(int,m_iSimpleDatumPoint);	//依赖角钢的当前搭接肢
	GET(m_iSimpleDatumPoint){return GetSimpleDatumPoint();}
	SET(m_iSimpleDatumPoint){SetSimpleDatumPoint(value);}
public:
	CMultiOffsetPos();
	void ClearContents();	//清零属性并赋默认值
	void FromJgBerPosPara(CJgBerPosPara *pPosPara);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	BOOL IsFaceOffset();		//是否为平推	wht 10-07-30		
	BOOL IsInDatumJgBer();		//基点是否在基准角钢楞线上
	BOOL IsInDatumJgWingX();	//基点是否在基准角钢X肢上
	BOOL IsInDatumJgWingY();	//基点是否在基准角钢Y肢上
	BOOL IsInHuoQuLine(bool blIncOffsetGLine=false);	//基点是否在火曲线上,blIncOffsetGLine=true包含偏移准线情况
	BOOL IsInOffsetZhunLine();	//基点是否在偏移准线上
	int GetDatumType();			//获得基点定位类型
	void SetToDatumPointStyle(){spatialOperationStyle=0;origin_pos_style=1;}
	BOOL ChangeToDatumType(int prj1_faceoffset2_huoqu3and4_offsetzhun5);	//将基点转换为另一类型
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,double* pickpos_coord);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
};
struct EXTRA_ODDINFO
{
	BYTE style;	//0:表示指定为odd；1:extraStartPoint-extraEndPoint的间距值;2:根据角钢自动判定odd值
	double odd;	//style=0时有效
	//extraStartPoint->extraEndPoint矢量的正头方向 positiveDirect，仅在style==1时有效
	CLDSPoint extraDatumPoint;	//style=1时有效
	EXTRA_ODDINFO(){style=0;odd=0;}
};
//角钢端额外(固定)正负头信息
struct EXTRA_ANGLEODD{
	EXTRA_ODDINFO wing_x,wing_y,ber;	//分别表示X肢、Y肢及楞线为基准时的额外正负头
	EXTRA_ANGLEODD(){wing_x.style=wing_y.style=2;}
};
class CKaoMeAngle
{
public:
	SEGI iSeg;
	struct BOLTHOLE{
		short d;
		union POS{
			DWORD key;
			struct{short x,y;};
		}pos;
	};
	CXhChar16 sLabel;
	CXhChar16 sKaiHeAngle;
	short wide,thick,length;
	CHashList<BOLTHOLE> listHoles;
	CKaoMeAngle(){wide=thick=length=0;}
};
struct KAI_HE_JIAO_TEMP{
public:
	float decWingAngle;										//两肢夹角
	short Position(){return position;}						//标定位置
	short StartLength(){return position-startPosLength;}	//始端开合长度
	short EndLength(){return endPosLength-position;}		//终端开合长度
	//
	short StartPosLength()const {return startPosLength;}	//始端开合位置长度
	short EndPosLength()const {return endPosLength;}		//终端开合位置长度
	void SetStartPosLength(short startPosLen){startPosLength=startPosLen; UpdatePos();}
	void SetEndPosLength(short endPosLen){endPosLength=endPosLen; UpdatePos();}
private:
	void UpdatePos(){position=(short)((startPosLength+endPosLength)*0.5);}
	short position;			//标定位置
	short startPosLength;	//始端开合位置长度
	short endPosLength;		//终端开合位置长度	
};
class CLDSGeneralPlate;
class CLDSLineAngle : public CLDSLinePart{
	CLDSLineAngle *GetStartNodeOffsetDatumAngle();
	CLDSLineAngle *GetEndNodeOffsetDatumAngle();
	BOOL CreateSpecialHeJiaoSolidModel(fBody* pBody,BOOL bDisplayHole=FALSE);
protected:
	PRESET_ARRAY4<long> arrDatumBerLocks;	//long arrDatumBerLocks[4] 防止GetDatumBerPos死锁调用 wjh-2016.3.26
	bool TestVisitLockLoop(long hNode);
protected:
	GEPOINT norm_x_wing,norm_y_wing;	//X肢Y肢的法线
	BYTE  _cnCalNormHits;				//防止肢法线方向递归计算进入死循环,计算过程控制临时变量
	/*	#被刷新的标志：
		节点进行初步到位计算时，该节点只要计算一次就能够达到初步到位。
		当第二次计算时，如果发现该标志已经有了，就不重复计算了。
		(如果重复计算了，计算结果也肯定相同，只是增加了工作量。)
	*/
	bool _bFootNail;	//脚钉角钢(TRUE) 角钢(FALSE)
	bool _bCutBer,_bCutRoot;
	BYTE _start_push_flat_x1_y2;			//始端X/Y肢压扁
	double _start_push_flat_length;		//始端压扁长度
	BYTE _end_push_flat_x1_y2;			//终端X/Y肢压扁
	double _end_push_flat_length;		//终端压扁长度
	BYTE _mid_push_flat_x1_y2;			//中间X/Y肢压扁
	double _mid_push_flat_length;		//中间压扁长度
	double _mid_push_flat_up_offset;	//上偏移量
	double _fCutAngleCoefR;	//切角间隙内圆弧半径占比系数
	short _nCutAngleSpaceOdd;//角钢切角时，切去的内圆弧半径系数之外的零头间隙
	//角钢四个角的倒角圆弧半径，0表示不需倒角
	double _dfFilletROfSX,_dfFilletROfSY,_dfFilletROfEX,_dfFilletROfEY;
	//依据螺栓检测计算负头时，负头允许跨过中位断面,此属性主要目的是为了平滑过渡老放样人员习惯 wjh-2019.8.13
	bool _blOddByBoltCanAcrossMiddleSect;	
	bool _blScatterBoltLocaTolerance;	//对于横担与塔身横材间的水平外贴角钢上非标间距形成累积误差较多时，须采用分散误差的方式（不能保证间距） wjh-2019.8.14
public:
	bool get_blOddByBoltCanAcrossMiddleSect() { return _blOddByBoltCanAcrossMiddleSect; }
	bool set_blOddByBoltCanAcrossMiddleSect(bool blValue);
	__declspec(property(put=set_blOddByBoltCanAcrossMiddleSect,get=get_blOddByBoltCanAcrossMiddleSect)) bool m_blOddByBoltCanAcrossMiddleSect;
	bool get_blScatterBoltLocaTolerance() { return _blScatterBoltLocaTolerance; }
	bool set_blScatterBoltLocaTolerance(bool blValue);
	__declspec(property(put=set_blScatterBoltLocaTolerance,get=get_blScatterBoltLocaTolerance)) bool m_blScatterBoltLocaTolerance;
	double get_dfFilletROfSX();
	double set_dfFilletRofSX(double dfFilletR);
	__declspec(property(put=set_dfFilletRofSX,get=get_dfFilletROfSX)) double dfFilletROfSX;
	double get_dfFilletROfSY();
	double set_dfFilletRofSY(double dfFilletR);
	__declspec(property(put=set_dfFilletRofSY,get=get_dfFilletROfSY)) double dfFilletROfSY;
	double get_dfFilletROfEX();
	double set_dfFilletRofEX(double dfFilletR);
	__declspec(property(put=set_dfFilletRofEX,get=get_dfFilletROfEX)) double dfFilletROfEX;
	double get_dfFilletROfEY();
	double set_dfFilletRofEY(double dfFilletR);
	__declspec(property(put=set_dfFilletRofEY,get=get_dfFilletROfEY)) double dfFilletROfEY;
	//角钢某肢上的开口
	struct OPENING_CUT{
		short siCornerFilletR;		//割口内角过渡弧
		double dfLateralBtmXorY;	//肢上中间开口底部的横向位置，>0表示Y肢开口，<0表示X肢开口
		double dfLenPosFrom,dfLenPosTo;			//开口底部的起止纵向位置，以角钢起始端面（不含正负头）为起点
		double dfTipDeltaS,dfTipDeltaE;	//起止点开口顶部（肢边缘处）相对底部的偏移量（始->终为正）
		OPENING_CUT();
	};
	PRESET_ARRAY4<OPENING_CUT> xarrOpenCuts;	//角钢上的开口参数
	//角钢局部开合角区间
	struct KAIHE_SECTION{
		char ciOperWing;		//开合角的操作肢，0.X; 1.Y; 'X'; 'Y'
		double dfFromX,dfToX;	//开口底部的起止纵向位置，以角钢起始端面（不含正负头）为起点
		double dfReservedSizeY;	//开合角时的横向保留宽度
		double dfOpenAngle;		//>0：开角度数；<0：合角度数；0.不需开合角
		KAIHE_SECTION();
	};
	PRESET_ARRAY4<KAIHE_SECTION> xarrKaiheSects;	//角钢上局部开合角的区间描述参数
protected:
	//通过CLDSPart::HOLE来汇总螺栓孔（含虚拟螺栓）及其余杂项孔（直接靠参数生成的工艺孔）集合　wjh-2017.11.2
	virtual UINT AddMiscellaneousHoles(IXhList<HOLE>* pListHoles);	//在GetHoleSet内调用,以添加除典型螺栓孔外的杂项孔
public:
	virtual UINT GetHoleSet(IXhList<HOLE>* pListHoles,bool bOnlyBoltHole=false);	//返回值为孔总数
public:	//公用属性
	static double KAIHEJIAO_ANGLE_THRESHOLD;
	static double KAIHEJIAO_CLEARANCE_THRESHOLD;
	static CLDSLineAngle* AppendModifiedAngles(CLDSLineAngle* pModifiedAngle);	//记录需要更新实体的角钢
	static CLDSLineAngle* EnumFirstModifiedAngles();	//遍历第一根发生变化后需要更新实体的角钢
	static CLDSLineAngle* EnumNextModifiedAngles();		//遍历下一根发生变化后需要更新实体的角钢
	virtual BYTE get_ciStartAnchorWing();
	virtual BYTE get_ciEndAnchorWing();
	_declspec(property(get=get_ciStartAnchorWing)) BYTE ciStartAnchorWing;
	_declspec(property(get=get_ciEndAnchorWing))   BYTE ciEndAnchorWing;
	PROPERTY(BOOL,m_bFootNail);
	GET(m_bFootNail){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _bFootNail;
		else
			return pMotherObj->m_bFootNail;
	}
	SET(m_bFootNail){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		_bFootNail=(value!=0);
		if(pMotherObj!=this)
			pMotherObj->m_bFootNail=value;
	}
	PROPERTY(BOOL,m_bCutBer);
	GET(m_bCutBer){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _bCutBer;
		else
			return pMotherObj->m_bCutBer;
	}
	SET(m_bCutBer){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		_bCutBer=(value!=0);
		if(pMotherObj!=this)
			pMotherObj->m_bCutBer=value;
	}
	PROPERTY(BOOL,m_bCutRoot);
	GET(m_bCutRoot){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _bCutRoot;
		else
			return pMotherObj->m_bCutRoot;
	}
	SET(m_bCutRoot){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		_bCutRoot=(value!=0);
		if(pMotherObj!=this)
			pMotherObj->m_bCutRoot=value;
	}
	PROPERTY(int,start_push_flat_x1_y2);
	GET(start_push_flat_x1_y2){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _start_push_flat_x1_y2;
		else
			return pMotherObj->start_push_flat_x1_y2;
	}
	SET(start_push_flat_x1_y2){
		if (value==0||value==1||value==2)
		{
			_start_push_flat_x1_y2=(BYTE)value;
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			if(pMotherObj!=this)
				pMotherObj->start_push_flat_x1_y2=value;
		}
	}
	PROPERTY(double,start_push_flat_length);
	GET(start_push_flat_length){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _start_push_flat_length;
		else
			return pMotherObj->start_push_flat_length;
	}
	SET(start_push_flat_length){
		if (value>=0)
		{
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			_start_push_flat_length=value;
			if(pMotherObj!=this)
				pMotherObj->start_push_flat_length=value;
		}
	}
	PROPERTY(int,end_push_flat_x1_y2);
	GET(end_push_flat_x1_y2){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _end_push_flat_x1_y2;
		else
			return pMotherObj->end_push_flat_x1_y2;
	}
	SET(end_push_flat_x1_y2){
		if (value==0||value==1||value==2)
		{
			_end_push_flat_x1_y2=(BYTE)value;
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			if(pMotherObj!=this)
				pMotherObj->end_push_flat_x1_y2=value;
		}
	}
	PROPERTY(double,end_push_flat_length);
	GET(end_push_flat_length){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _end_push_flat_length;
		else
			return pMotherObj->end_push_flat_length;
	}
	SET(end_push_flat_length){
		if (value>=0)
		{
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			_end_push_flat_length=value;
			if(pMotherObj!=this)
				pMotherObj->end_push_flat_length=value;
		}
	}
	PROPERTY(int,mid_push_flat_x1_y2);
	GET(mid_push_flat_x1_y2){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _mid_push_flat_x1_y2;
		else
			return pMotherObj->mid_push_flat_x1_y2;
	}
	SET(mid_push_flat_x1_y2){
		if (value==0||value==1||value==2)
		{
			_mid_push_flat_x1_y2=(BYTE)value;
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			if(pMotherObj!=this)
				pMotherObj->mid_push_flat_x1_y2=value;
		}
	}
	PROPERTY(double,mid_push_flat_length);
	GET(mid_push_flat_length){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _mid_push_flat_length;
		else
			return pMotherObj->mid_push_flat_length;
	}
	PROPERTY(double, mid_push_flat_start_offset);
	GET(mid_push_flat_start_offset){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _mid_push_flat_length/2-_mid_push_flat_up_offset;
		else
			return pMotherObj->mid_push_flat_start_offset;
	}
	SET(mid_push_flat_start_offset)
	{
		if(value>=0)
		{
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			_mid_push_flat_length=mid_push_flat_end_offset+value;
			_mid_push_flat_up_offset=_mid_push_flat_length/2-value;
			if(pMotherObj!=this)
				pMotherObj->mid_push_flat_start_offset=value;
		}
	}
	PROPERTY(double, mid_push_flat_end_offset);
	GET(mid_push_flat_end_offset){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _mid_push_flat_length/2+_mid_push_flat_up_offset;
		else
			return pMotherObj->mid_push_flat_end_offset;
	}
	SET(mid_push_flat_end_offset)
	{
		if(value>=0)
		{
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			_mid_push_flat_length=mid_push_flat_start_offset+value;
			_mid_push_flat_up_offset=-(_mid_push_flat_length/2-value);
			if(pMotherObj!=this)
				pMotherObj->mid_push_flat_start_offset=value;
		}
	}
	//切角间隙内圆弧半径占比系数
	PROPERTY(double,m_fCutAngleCoefR);
	GET(m_fCutAngleCoefR){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _fCutAngleCoefR;
		else
			return pMotherObj->m_fCutAngleCoefR;
	}
	SET(m_fCutAngleCoefR){
		if (value>=0)
		{
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			_fCutAngleCoefR=value;
			if(pMotherObj!=this)
				pMotherObj->m_fCutAngleCoefR=value;
			SetModified();
		}
	}
	//角钢切角时，切去的内圆弧半径系数之外的零头间隙
	PROPERTY(short,m_nCutAngleSpaceOdd);
	GET(m_nCutAngleSpaceOdd){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _nCutAngleSpaceOdd;
		else
			return pMotherObj->m_nCutAngleSpaceOdd;
	}
	SET(m_nCutAngleSpaceOdd){
		if (value>=0)
		{
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			_nCutAngleSpaceOdd=value;
			if(pMotherObj!=this)
				pMotherObj->m_nCutAngleSpaceOdd=value;
			SetModified();
		}
	}
	//获取X肢法线，只读
	READONLY_PROPERTY(GEPOINT,vxWingNorm);
	GET(vxWingNorm){
		return this->norm_x_wing;
	}
	//将来可以在避免错误赋值肢法向情况下考虑用PROPERTY取代BriefSetWingNormX/Y() wjh-2016.8.9
	void SetBriefWingNormX(const double* wingnorm,bool bToSpecNorm=true);
	void SetBriefWingNormY(const double* wingnorm,bool bToSpecNorm=true);
	bool UpdateParentGroupAngleByWingNormX();
	bool UpdateParentGroupAngleByWingNormY();
	//获取Y肢法线，只读
	READONLY_PROPERTY(GEPOINT,vyWingNorm);
	GET(vyWingNorm){
		return this->norm_y_wing;
	}
public:	
	static BOOL m_bDisplaySolidAxisLine;	//显示角钢心线 wht 10-12-27
	static const DWORD PROP_DISCONNECT	= 0;	//断开
	static const DWORD PROP_SYNC_COPY	= 1;	//同步对称
	static MEMORY_BLOCK MPCL;	//Mirror Property Control List
	static PROPLIST_TYPE propMirSync;
	static void InitPropMirBitsMap();
	virtual PROPLIST_TYPE* GetSyncPropList(){return &propMirSync;}
	virtual MEMORY_BLOCK GetDefaultMPCL(){return MPCL;}
	virtual bool IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr);
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部位置对称无关的属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
	virtual bool IsBelongTo(int iClassTypeId)const;
	virtual CRodPosPara* DesStartPos(){return &desStartPos;}
	virtual CRodPosPara* DesEndPos(){return &desStartPos;}
	double  GetWeigthCenterG();	//返回底座板设计时的基础中心（一般位于角钢重心线）对应的心距值，一般为W/4 wjh-2019.3.31
	static 	JGZJ GetAngleZJ(double wide);		//查询角钢准距
	struct ANGLEPOINT{
		BYTE direction;	//0.基准角钢方向:始->终;1.终->始
		BYTE offset_wing;//0.X肢;1.Y肢
		short wing_offset_style;//0.g;1.g1;2.g2;3.g3;4.自定义
		long  hResideNode;		//主要用于识别节点处设有特殊准距的情况
		double wing_offset_dist,len_offset_dist;//肢向偏移值;纵向偏移值;
		char norm_offset[51];//法向偏移量;将来应视情况看看能否用CParaThick取代 wjh-2017.6.5
		ANGLEPOINT();
	};
	//提取两根具有关联对称关系的角钢间的对称匹配状态（始末是否调个，X/Y肢法线是否调换) wjh-2017.11.13
	bool GetMirMatchInfo(CLDSLineAngle* pToMirAngle,MIRMSG mirmsg,bool* pbReverseStartEnd=NULL,bool* pbReverseWingXnY=NULL);
	bool GetMirAnglePoint(const ANGLEPOINT from,ANGLEPOINT &to, CLDSLineAngle* pToMirAngle,MIRMSG mirmsg);
public:
	CLDSLineAngle* Fabricate(CLDSPart* pOrgPart,const double* xArcTagentPt=NULL);	//根据一个原始构件伪造一根角钢，以进行一些仅角钢可进行的操作 wjh-2016.9.3
public:
	BOOL m_bWeldPart;
	BOOL m_bReplPart;	//代用规格构件
	char org_material;	//原材质
	BYTE m_cHeJiaoReservedWidthType;	//'I'肢内侧拐点到内侧角点宽度；'O'肢外侧拐点到外侧楞点宽度
	double m_fHeJiaoReservedWidth;			//合角较大时（如三角塔身主材），由于内圆弧原因，必须保留楞线附近一定直角宽度
	double org_wing_wide,org_wing_thick;	//原肢宽肢厚
	WORD m_nCutBerR;
	BOOL m_bEnableTeG;					//启用角钢特殊准距
	BYTE m_cPosCtrlDatumLineType;		//'X':X肢标准心线控制;'Y':Y肢标准心线控制;其余楞线控制
	BYTE m_cPosCtrlType;				//位置控制类型:0.垂向投影;1.X-Y平面平推;2.Y-Z平面平推;3.X-Z平面平推;4.自定义平推面
	GEPOINT pre_huoqu_start_xyz,pre_huoqu_end_xyz;	//火曲前起止点坐标
	JGZJ xWingXZhunJu,xWingYZhunJu;		//角钢特殊准距
	THANDLE mid_push_flat_node_handle;	//中间压扁节点句柄 wht 2012-10-10
	DWORD dwLayStyle;						//摆放方式1、4为外铁，2、3为里铁
	union{
		long hGroupAngle;	//仅为了敲键方便，及视觉可读性添加 wjh-2019.9.30
		THANDLE group_father_jg_h;			//若是组合子角钢存储其组合父角钢句柄
	};
	double huoqu_r;		//若是火曲角钢段时，此项才有效，表示火曲半径
	double nominal_length;	//名义长度
	long kaihe_base_wing_x0_y1;		//开合角钢基准肢
	long huoqu_base_wing_x0_y1_both2;//火曲线基准肢
	long huoqu_start1_end2;			//相关火曲线位置（1:始端火曲线2:终端火曲线),现在已无用已被bHuoQuStart,bHuoQuEnd取代，以后有时间要删除掉
	BOOL m_bUserSpecStartCutAngle;		//由操作人员手动维护始端切角切肢信息
	BOOL m_bUserSpecEndCutAngle;		//由操作人员手动维护终端切角切肢信息
	THANDLE son_jg_h[4];//子角钢句柄
	// --------- 1.2. 切肢 -----------
	/*cut_wing_para[0]表示始端背切肢参数cut_wing_para[1]表示终端背切肢参数
	...[][0]表示角钢楞线切取长度
	...[][1]表示另一肢切取宽度
	...[][2]表示肢棱线切取长度
	*/
	double cut_wing_para[2][3];
	//标识所切角钢肢,0表示无切肢,1表示X肢,2表示Y肢
	long cut_wing[2];
    // --------- 1.3. 切角 -----------
	/*
	cut_angle[0]表示起点X肢
	cut_angle[1]表示起点Y肢
	cut_angle[2]表示终点X肢
	cut_angle[3]表示终点Y肢
	...[][0]表示平行楞线方向切取量  ...[][1]表示垂直楞线方向切取量
	*/
	double cut_angle[4][2];
public:
	//顶点间隙: vertex_dist		射线角钢位于主角钢外侧时使用顶点间隙值	通过函数library->GetVertexDist();获得 wht 10-07-02
	//切角间隙: cut_angle_space 切角间隙默认取2/3*r(角钢内圆弧半径) 当用户指定切角间隙且大于2/3*r时使用用户输入值  
	void SetWeldPart(bool bWeldPart){m_bWeldPart=bWeldPart;}
	bool IsWeldPart();
	virtual bool IsCompositeSonAngle(){return group_father_jg_h>0x20;}	//是否为组合角钢的子角钢
	void SetHeJiaoReservedWidth(double fHeJiaoReservedWidth){m_fHeJiaoReservedWidth = fHeJiaoReservedWidth;}
	double GetHeJiaoReservedWidth(){return m_fHeJiaoReservedWidth;}
	void SetKaiHeBaseWing(long kaihe_base_wing){kaihe_base_wing_x0_y1 = kaihe_base_wing;}
	long GetKaiHeBaseWing(){return kaihe_base_wing_x0_y1;}
	void SetHuoQuBaseWing(long huoqu_base_wing){huoqu_base_wing_x0_y1_both2 = huoqu_base_wing;}
	long GetHuoQuBaseWing(){return huoqu_base_wing_x0_y1_both2;}
	static bool CoplanarConnectDistinguish(CLDSLineAngle* pAngle1,CLDSLineAngle* pAngle2,int* angle1_wing,int* angle2_wing,BOOL* same_direct=NULL,double justify=0.5);
	bool CalCutAngleInfo(CLDSLineAngle *pCutAngle1,CLDSLineAngle *pCutAngle2,bool bStart,int (*InputCollideDist)(int dist),int cut_angle_space=0);
	BOOL CalCutAngleInfo(f3dPoint face_pick,f3dPoint face_norm,int (*InputCollideDist)(int dist),int cut_angle_space=0);
	double GetCutAngleCoefR();//获取切角间隙内圆弧半径占比系数
	CXhChar16 GetCutAngleSpaceExpr();//获取角钢肢内侧切角时的间隙值描述表达式（内圆弧半径*系数+零头）
	JGZJ GetZhunJu(char cWorkWing='X');
	int GetWingYG(int styleG0123);
	int GetWingXG(int styleG0123);
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	virtual double GetArea();	//单位为mm2
	//bSolidLength表示计算长度时按单线计算还是按实体计算,主要是力学分析时统计重量用
	virtual double GetWeight(BOOL bSolidLength=TRUE);	//单位为kg
	double GetWidthWingY(){return IsUnequalAngle()?size_height:size_wide;}
	//double GetWidth(){return size_wide;}
	//double GetThick(){return size.thick;}
	double GetEndWingNormOffset(int x_wing0_y_wing1);
	double GetStartWingNormOffset(int x_wing0_y_wing1);
	virtual BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	double set_wing_wide(double wing_wide);
	double set_wing_thick(double wing_thick);
	double set_y_wing_width(double y_wing_wide);
	BOOL IsUnequalAngle(); //判断当前角钢是否为不等边角钢 wht 12-04-26
	static BOOL IsUnequalAngle(double x_wing_wide,double y_wing_wide,double thick);
	//得到简化规格字符串不包括构件类型、材质等信息仅包括肢宽及肢厚 wht12-04-27
	void GetAngleSimpleSpec(char *simple_spec);
	static void GetAngleSimpleSpec(char *simple_spec,double wing_wide,double y_wing_wide,double wide_thick);

	void RetrieveKaoMeAngle(CKaoMeAngle* pKM,long* hBoltArr,int nBolts,CLDSLineAngle *pCoupleAngle=NULL);
	void GetProcessStr(char *notes,size_t max_len,bool bByEnglish=false,bool bNumericKaiHeInfo=false);
	void RemoveKaiHeJiao();
	int ValidCheck();
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//更新角钢当前连接肢信息(m_iConnectWing)
	void UpdateConnectInfo(CLDSNode *pNode);
	//自协同中心通过管道读取数据
	virtual void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	//通过管道向协同中心写入数据
	virtual void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	virtual void CalStartOddment(char ciAutoCalMode = 0,int liInitOddment=0);	//0.普通碰撞规则算;1.强制性把指定负头情况按碰撞规则算;2.按允许切角时保证切角边距情况的碰撞规则算
	virtual void CalEndOddment(char ciAutoCalMode = 0,int liInitOddment=0);		//0.普通碰撞规则算;1.强制性把指定负头情况按碰撞规则算;2.按允许切角时保证切角边距情况的碰撞规则算
	long get_hStartDatumRod();
	long get_hEndDatumRod();
	CLDSLinePart* GetStartDatumPart();
	CLDSLinePart* GetEndDatumPart();
	double GetStartOffsetByWingX();
	double GetStartOffsetByWingY();
	double GetEndOffsetByWingX();
	double GetEndOffsetByWingY();
	bool IsCutAngleOrWing();
	bool IsStartCutAngleOrWing();
	bool IsEndCutAngleOrWing();
	//-2:在直线上,但位于线段起点外侧 
	//-1:在直线上,但位于线段终点外侧
	//0:不在直线上 1:在线段端点上 2:在线段内
	int IsNodeInJg(CLDSNode *pNode);
	DWORD SetPermission(DWORD dwPerm);
	double GetNodeWingYG(CLDSNode* pNode);
	double GetNodeWingXG(CLDSNode* pNode);
	//0:X肢为当前端工作肢;1:Y肢为当前端工作肢;2:双肢均为工作肢如塔身主材及其它双肢连接构件
	int GetStartWorkWing();
	//0:X肢为当前端工作肢;1:Y肢为当前端工作肢;2:双肢均为工作肢如塔身主材及其它双肢连接构件
	int GetEndWorkWing();
	double GetStartExtraOdd(int wing_x0_y1_ber2);
	double GetEndExtraOdd(int wing_x0_y1_ber2);
	int AutoCorrectSonNode();
	void SetWingYLsPos(CLsRef *pLsRef,double z,double g,double depth);
	void SetWingXLsPos(CLsRef *pLsRef,double z,double g,double depth);
	double GetLsParamG(CLDSBolt *pBolt);	//获取螺栓对于当前角钢的心距参数，不同于GetLsG返加的是实际空间计算值 wjh-2016.1.8
	double GetLsG(CLsRef *pLsRef,double *depth=NULL,bool bInitUCS=true);
	double GetLsG(CLDSBolt *pLs,double *depth=NULL,bool bInitUCS=true);
	double GetLsG(f3dPoint ls_norm,f3dPoint ls_pos,double *depth=NULL);
	//返回0表示失败，1:表示正常；-1表示螺栓垂直与肢面，但穿插位置有误（未插到角钢肢面上）
	bool GetBoltLocalPos(CLDSBolt *pBolt,double *local_pos,bool bIncOddment,int* ciWingX0Y1=NULL);	//输出ciWingX0Y1,0:螺栓在X肢;1:螺栓在Y肢
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* wingx0_y1=NULL,int iIntersPlane=0);	//获取螺栓穿过角钢外皮的交点位置 wjh-2015.4.8
	virtual bool UpdateHoleWall(HOLE_WALL* pHoleWall);	//当螺栓或角钢发生变化时，实时更新孔壁
	virtual int GatherHoleWalls(CHash64ListEx<HOLE_WALL>& hashHoles);
	virtual bool GetEffectiveEndBolts(ENDBOLTS* pEndBolts,bool bRetrieveStart=true);
	/*
	返回值:
	 0表示合法;
	-1螺栓端距过小;
	-2螺栓轧制边距过小;
	-3螺栓切角边距太小 
	*/
	int ValidLsList(char sError[MAX_TEMP_CHAR_200+1],
		BOOL bRollingSpace=TRUE,BOOL bCutAngleSpace=TRUE,BOOL bLsEndSpace=TRUE,double epsilon=2);
	BOOL ValidLsPushFlat(char sError[MAX_TEMP_CHAR_200+1],double epsilon=2);
	f3dPoint GetStartInitPos();	//获取始端定位初始位置(无论数据是否合理都返回值)
	f3dPoint GetEndInitPos();	//获取终端定位初始位置(无论数据是否合理都返回值)
	bool GetStartInitPos(f3dPoint &init_pos);	//获取始端定位初始位置(数据不合理返回false)
	bool GetEndInitPos(f3dPoint &init_pos);		//获取终端定位初始位置(数据不合理返回false)
	f3dPoint GetStartDatumPos();
	f3dPoint GetEndDatumPos();
	f3dPoint GetEndDatumPosEx();	//如果是火曲线为基准，则返回火曲线位置
	f3dPoint GetStartDatumPosEx();
	f3dPoint get_end_in_ber();		//获得终端的内侧楞点位置
	f3dPoint get_start_in_ber();	//获得始端的内侧楞点位置
	BOOL IsLsInWing(CLDSBolt *pLs,int *x_wing0_y_wing1=NULL,bool bNeglectLenLimit=false,double* pRsltIntersPoint=NULL);
	virtual void SetModified(BOOL bTrue=TRUE,BOOL bRenderSolid=TRUE);

	void ClearFlag();
	CLDSLineAngle &operator=(const CLDSLineAngle &jg);
	bool IsKaiJiao(double tolerance=0,double style_cosa0_clearance_1=0);
	bool IsHeJiao(double tolerance=0,double style_cosa0_clearance_1=0);
	bool IsReservedHeJiao();
	double GetRadWingAngle();
	double GetDecWingAngle();
	double ConnectClearance();	//有开合角时，其上连接的射线角钢最大间隙
	void CopyProperty(CLDSDbObject* pObj);
	CLDSLineAngle();
	~CLDSLineAngle();
	f3dPoint get_norm_x_wing();
	f3dPoint set_norm_x_wing(f3dPoint norm,BOOL bSpecific=TRUE);
	f3dPoint get_norm_y_wing();
	f3dPoint set_norm_y_wing(f3dPoint norm,BOOL bSpecific=TRUE);
	f3dPoint CalWingOffsetVec(char cOffsetWing,double spec_g);
	f3dPoint GetWingVecX();
	f3dPoint GetWingVecY();
	bool IsLenOffsetNode(CLDSNode *pNode);
	virtual f3dPoint GetDatumPos(CLDSNode* pNode);
	//cSpecDatumWing='X'表示X肢为基准;'Y'表示Y肢为基准，其余为楞线，该参数仅在基准角钢为有保留肢宽的特殊合角时有效wjh-2011.10.21
	f3dPoint GetDatumPosBer(CLDSNode *pNode,char cSpecDatumWing=0,bool bClearLocks=true);//,BOOL bFatherJgPos=TRUE);
	f3dPoint GetDatumPosWingX(CLDSNode *pNode,double spec_g=0);//,BOOL bFatherJgPos=TRUE);
	f3dPoint GetDatumPosWingY(CLDSNode *pNode,double spec_g=0);//,BOOL bFatherJgPos=TRUE);
	//cSpecDatumWing='X'表示X肢为基准;'Y'表示Y肢为基准，其余为楞线，该参数仅在基准角钢为有保留肢宽的特殊合角时有效wjh-2011.10.21
	f3dPoint GetDatumPosBerByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick=NULL,char cSpecDatumWing=0);
	f3dPoint GetDatumPosWingXByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick=NULL,double spec_g=0);
	f3dPoint GetDatumPosWingYByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick=NULL,double spec_g=0);
	f3dPoint GetSelfDatumPosWingXByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick=NULL);
	f3dPoint GetSelfDatumPosWingYByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick=NULL);
	f3dPoint GetStartWingDatumPosByFaceOffset(char cWing,f3dPoint face_norm);
	f3dPoint GetEndWingDatumPosByFaceOffset(char cWing,f3dPoint face_norm);
	virtual bool SetDirectMasterAngle();	//如果是组合角钢子角钢时，设定自身为组合角钢肢法线的计算基准角钢 wjh-2016.9.19
	bool cal_x_wing_norm(bool bIgnoreRepeatFlag=false);
	bool cal_y_wing_norm(bool bIgnoreRepeatFlag=false);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	virtual void ResetRelativeObjs();
	void UpdateRelateRayJgPos();	//更新相关角钢摆放方式
	BYTE FuncType();
	char GetSelfWorkWing(BOOL bStart,const double* worknorm);//当前角钢始末端自身工作肢
	char GetLappedWorkWing(BOOL bStart,const double* worknorm);//始端基准角钢的搭接肢
public:
	//智能更新简单定位时角钢始端的位置设计参数　wjh-2016.3.11
	bool IntelliUpdateSimplePosParam(BOOL bStart,CLDSLinePart* pFatherRod=NULL);
private:
	//新更改角钢楞点位置参数设计原理后的始端位置计算函数 wjh-2012-4-28
	void CalStartPosition(CLDSArcLift *pLift=NULL);
	//新更改角钢楞点位置参数设计原理后的终端位置计算函数 wjh-2012-4-28
	void CalEndPosition(CLDSArcLift *pLift=NULL);
	//过时的楞点位置计算参数及函数
	void cal_start_ber_pos();
	void cal_end_ber_pos();
public:
	virtual bool ShadowDesign();
	virtual void TransToCS(GECS& cs);
	virtual void Offset(const double* offset);
	virtual int GatherOddRelaParts(BYTE start1end2all3=3);
	virtual bool RecordRelaPartsAfterModified();
	virtual void CalPosition(bool bIgnoreRepeatFlag=false);
	virtual bool IsPosSelfDependent(bool bDetectStartTip);	//判断角钢指定端楞点位置参数是否出现了自依赖死循环
	//ciTurnStyle0or1
	//0.里外铁翻转	立肢换为另外一肢   朝向不变
	//				工作肢换为另外一肢 朝向反向
	//1.左右面翻转  立肢换为另外一肢   朝向反向
	//				工作肢换为另外一肢 朝向不变
	virtual bool TurnLineAngle(int work_wing_x0_y1,char ciTurnStyle0or1);
	virtual UCS_STRU BuildUCS();
	void getUCS(UCS_STRU &ucs,int base_x0_y1_mid2=0);
	BOOL GetStartEndBoltD(int *start_d,int *end_d);
	int GetKaiHeWingX0Y1(long hPreventSideRod=0);	//hPreventSideRod指共线杆件时调用该函数的发起侧杆件句柄 wjh-2019.8.7
	int GetWorkWingX0Y1();
	bool IsStronglyNoneKaiHeAngle();	//是否为强制不开合角的情况，如塔身主材，内外包铁等
	bool IsEqual(CLDSLineAngle *pJg,double epsilon=1.0,char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	virtual bool CompareLabelPart(CLDSPart *pPeerPart,MIRMSG* pMirmsg=NULL,BOOL bCmpls=TRUE,PEER_PART *pPeerInfo=NULL,double epsilon=1.0);
	virtual void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	virtual void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	//bReadForCheck表示是否读取为了进行模型比对检查(仅读取最少量校审相关信息)
	virtual void FromCdfBuffer(CBuffer &buffer,long version=0,bool bReadForCheck=false);
	virtual void ToCdfBuffer(CBuffer &buffer,long version=0);	
	BOOL UpdateInnerCircleSlopeFace(fBody* pBody,f3dPoint wing_x_vec,f3dPoint wing_y_vec,f3dPoint norm_Y);
	BOOL UpdateWingFilletArcFace(fBody* pBody,f3dPoint wing_x_vec,f3dPoint wing_y_vec,f3dPoint norm_Y);
	virtual int WriteProcessBuffer(CBuffer &buffer,bool bSaveSubSerial=false,long version=0);			//得到角钢工艺扩展信息的buffer
	void EmendShortJgLsPosY(CLDSBolt* pBolt,double &fLsPosY);
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart);
#endif
	/*
	CLsRefList *GetLsRefList(){return pLsRefList;}
	CLsRef* GetFirstLsRef(){return pLsRefList->GetFirst();}
	CLsRef* GetNextLsRef(){return pLsRefList->GetNext();}
	int GetLsCount(short d=0){return pLsRefList->GetLsCount(d);}
	void EmptyLsRef(){ pLsRefList->Empty();}
	CLsRef* FindLsByUcsPos(f3dPoint base,UCS_STRU ucs,int iNormX0Y1Z2,double epsilon=EPS2)
	{return pLsRefList->FindLsByUcsPos(base,ucs,iNormX0Y1Z2,epsilon);}
	CLsRef* FindLsByPos(f3dPoint base,f3dPoint work_norm,double epsilon=EPS2)
	{return pLsRefList->FindLsByPos(base,work_norm,epsilon);}
	CLsRef* FindLsByHandle(THANDLE h){return pLsRefList->FromHandle(h);}
	friend BOOL IsLsRefListEqual(CLsRefList *pLsRefList,const UCS_STRU &ucs,
					   CLsRefList *pViceLsRefList, const UCS_STRU &vice_ucs,double epsilon);
	*/
	//设计依据
	DESIGN_JGWING_NORM des_norm_x,des_norm_y;
	CMultiOffsetPos desStartPos,desEndPos;
	CJgBerPosPara *pOldDesStartPos,*pOldDesEndPos;
	EXTRA_ANGLEODD extraStartOdd,extraEndOdd;	//始末端的额外(固定)正负头
	bool IsLappedAnchorStart();	//始端是否为搭接定位
	bool IsLappedAnchorEnd();	//终端是否为搭接定位
	bool IsValidCombinationDesPara(bool bStartEnd);//判断始(true)终(false)端位置设计参数组合是否合法
	static BOOL IsInsideAngle(CLDSLineAngle* pAngle,f3dPoint norm,int *x_wing0_y_wing1);
	//属性
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSLineAngle,propHashtable)		//定义属性项更新函数
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	static int GetPropValueStr(CLDSLineAngle* pLineAngle,long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	static int InitLineAnglePropHashtable();
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//得到构件属性的默认只读状态
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//得到构件属性的默认只读状态
	//以下为旧函数用于与新代码对比测试
#ifdef __ALFA_TEST_VERSION_
public:
	void Test_CalPosition();
	int Test_GetStartWorkWing();
	int Test_GetEndWorkWing();
	double Test_GetStartExtraOdd(int wing_x0_y1_ber2);
	double Test_GetEndExtraOdd(int wing_x0_y1_ber2);
	f3dPoint Test_GetStartDatumPos();
	f3dPoint Test_GetEndDatumPos();
	f3dPoint Test_GetStartDatumPosEx();
	f3dPoint Test_GetEndDatumPosEx();
	double Test_GetStartOffsetByWingX();
	double Test_GetEndOffsetByWingX();
	double Test_GetStartOffsetByWingY();
	double Test_GetEndOffsetByWingY();
	void Test_UpdateRelateRayJgPos();
	void Test_UpdateConnectInfo(CLDSNode *pNode);
	void Test_CopyProperty(CLDSDbObject* pObj);
#endif
	PROPERTY(int,LayStyle);	//摆放方式1、4为外铁，2、3为里铁
	GET(LayStyle){
		return dwLayStyle&0x000000ff;
	}
	SET(LayStyle){
		dwLayStyle=dwLayStyle&0x00000f00;	//原0x00000100存在潜在问题，只是之前恰巧未有合适调用　wjh-2016.5.29
		dwLayStyle+=(value&0xff);
	}
	//MirLayStyle的类型
	static const DWORD MIRXorY_UNCHAHNGE	= 0;	//X或Y轴对称时,保留原有里外铁
	static const DWORD MIRXorY_SAMEDIRECT	= 1;	//X或Y轴对称时,工作肢里外同向
	static const DWORD MIRXorY_INVERT		= 2;	//X或Y轴对称时,工作肢里外反向
	PROPERTY(DWORD,MirLayStyle);
	GET(MirLayStyle){
		return (dwLayStyle&0x00000f00)>>8;
	}
	SET(MirLayStyle){
		dwLayStyle&=0x000000ff;
		dwLayStyle|=(value&0x0000000f)<<8;
	}
};
class CDesignProfileVertex{
public:
	f3dPoint ber_pos,edge_pos;
	THANDLE hJg;				//角钢句柄
	THANDLE hNode;				//基准节点句柄
	double g,ber_space,edge_space;	//心距;心线向楞线一侧的偏移量;心线向肢边一侧的偏移量
	short direction;				//0.基准角钢方向:始->终;1.终->始
	short offset_wing;			//0.X肢;1.Y肢
	double len_offset_dist;		//纵向偏移值
	short face_i;				//1表示基准面,2表示第一火曲面,3表示第二火曲面
public:
	CDesignProfileVertex();
	BOOL UpdateVertex(IModel* pTower);
};

//设计参数类型
enum DESIGN_PARAM_TYPE{
	PARAM_POS = 0,		//定位参数
	PARAM_SHAPE = 1,	//外形参数
};
/* 该部分代码已移至ComplexId.h,ComplexId.cpp中 wjh-2014.6.14
union KEY2C{
private:
	static char pack_str[3];
	char label[2];
public:
	WORD wItem;
	operator WORD(){return wItem;}
	KEY2C(WORD wKey=0){wItem=wKey;}
	KEY2C(char* keyStr){
		wItem=0;
		if(keyStr!=NULL)
		{
			for(size_t i=0;i<strlen(keyStr)&&i<2;i++)
				label[i]=keyStr[i];
		}
	}
	char* Label();//返回指针是临时字符串指针
	bool IsEqual(KEY2C key){return wItem==key.wItem;}
};
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
	KEY4C(WORD wItem,WORD wIndex);
	KEY4C(char* keyStr);
	char* Label();//返回指针是临时字符串指针, 到下一次调用KEY4C::Label()前有效
	KEY4C ClonePrior(WORD index) const;	//克隆前半部(wItem)，后半部取值输入index值
	bool IsEqual(KEY4C key){return dwItem==key.dwItem;}
};
*/
/*
struct DESIGN_PARAM_ITEM
{
	KEY4C key;			//该参数的标识键值（或不超过4个长的字符串）
	CXhChar50 sLabel;	//属性栏中显示的短名称标签
	CXhChar100 sNotes;	//界面中对该项属性的解释性描述
	union{double fValue;THANDLE hPart;long iType;}value;
	BYTE dataType;	//0.double数值 1.句柄 2. 整数
	BOOL bModify;
	DESIGN_PARAM_ITEM(){dataType=0; bModify=FALSE;}
public:	//dataType的数据类型
	static const BYTE TYPE_DOUBLE	= 0;
	static const BYTE TYPE_HANDLE	= 1;
	static const BYTE TYPE_INTEGER	= 2;
};
*/
#include "DesignParamItem.h"
struct DESIGN_PLATE_ITEM
{
	KEY2C key;
	CXhChar200 sNotes;
	struct DUAL_VALUE{THANDLE hObj;DWORD uiAttribute;};
	union{
		double fValue;
		struct {THANDLE hObj;UINT uiAttribute;};
		THANDLE hPart;
		long iType;
	}value;
	BYTE dataType;	//0.double数值 1.句柄 2. 整数 3.句柄+附加属性
	BOOL bModify;
	BYTE paramType;	//0.定位参数 1.外形参数
	void SetKey(DWORD dwKey){key=(WORD)dwKey;}
	double ToDouble();
	long   ToLong();
	double SetItemValue(double val);
	DECLARE_READONLY_PROPERTY(double,fValue){return ToDouble();}
	DECLARE_READONLY_PROPERTY(long,hValue)	{return ToLong();}
	DECLARE_READONLY_PROPERTY(long,iValue)	{return ToLong();}
	DECLARE_READONLY_PROPERTY(UINT,uiAttribute){
		if(dataType==3)
			return value.uiAttribute;
		else
			return 0;
	}
	DESIGN_PLATE_ITEM(){dataType=0; bModify=FALSE; paramType=PARAM_SHAPE;}
};
class CDesignPlateBoltPara{
public:
	CDesignPlateBoltPara(){;}
	~CDesignPlateBoltPara(){;}
	int d;				//螺栓直径
	int iSeg;			//螺栓段号
	double x,y,z;		//基准点坐标值
	int array_num;		//环形陈列螺栓数量
	double scope_angle;	//环形陈列填充角度,单位:°。角度小于等于0时，逐个定制;大于0时，环形陈列
};

struct WELDLINE{
	static const char SMART_SLOPECUT = 0x01;	//智能判断坡口
	static const char SPECIFY_HEIGHT = 0x02;	//指定焊缝高（否则根据母构件自行计算）
	char cCalModeFlag;	//0.坡口；2.焊缝高
public:
	static const char STATE_NEED_SLOPECUT  = 0x01;	//需要进行坡口
	static const char STATE_FSQUARE_WELD   = 0x02;	//正面平焊（无突出焊缝），否则默认为满焊
	static const char STATE_BSQUARE_WELD   = 0x04;	//背面平焊（无突出焊缝），否则默认为满焊
	char cStateFlag;	//0.坡口；2.正面平(满)焊；4.背面平(满)焊
public:
	short height;			//焊缝高度,mm
	short length;			//焊缝长度,mm
	short lenpos;		//焊缝起点位置(自边始端点的偏移量), mm
	long hWeldMotherPart;	//焊接母构件（焊缝所在的母构件）
public:
	WELDLINE();
	//用于设定含有排它性选项的标识字,排它选项掩码标识位在低位，排它选项标识位自高位排列
	char SetStateFlagDword(char cFlag,bool bSet=true);
	char SetCalModeFlagDword(char cFlag,bool bSet=true);
	//是否具有某种标识设定
	bool IsHasStateFlag(char cFlag);
	bool IsHasCalModeFlag(char cFlag);
	void FromBuffer(CBuffer &buffer);
	void ToBuffer(CBuffer &buffer);
};
class CLDSGeneralPlate;
struct PROFILE_EDGEINFO{
	bool m_bWeldEdge; 
	bool m_bRollEdge;				//此边需卷边
	WELDLINE weldline;				//焊缝参数
	short manu_space;				//焊缝加工预留间隙或卷边高度(mm)
	short roll_edge_offset_dist;	//卷边偏移距离 wht 11-04-02
	double radius;					//椭圆弧所需要的参数
	double sector_angle;			//标准圆弧所需要的参数
	//NewPlate.cpp中OnCutPlateByColumnFace中用到了center.feature故暂时还不能改为GEPOINT wjh-2014.12.29
	f3dPoint center,column_norm;	//椭圆弧所需要的参数（永远输入绝对坐标),center如果表示圆弧圆心(拾取点)时，则输入相对坐标
	double local_point_y;			//对活点位置，在该点两侧标注
	int local_point_vec;			//0.表示无对活点，1.表示对活点两侧标注50-->100的方向与焊接父杆件的start-->end方向相同，2.表示相反
};
struct SIMPLE_VERTEX{
	BYTE type;					//0:普通直边1:指定圆弧夹角2:指定圆弧半径3:椭圆弧
	BYTE ciPlaneNo;				//节点归属的面号1.表示基准面
	double radius;				//椭圆弧所需要的参数
	double sector_angle;		//标准圆弧所需要的参数
	GEPOINT vertex;				//顶点的位置坐标
	GEPOINT center,vColNormal;	//椭圆弧所需要的参数（永远输入绝对坐标),center如果表示圆弧圆心(拾取点)时，则输入相对坐标
	SIMPLE_VERTEX(){type=0;ciPlaneNo=1;radius=sector_angle=0;}
};
class PROFILE_VERTEX{
public:
	static const BYTE DT_NONE		= 0;	//用户自定义
	static const BYTE DT_BER		= 1;	//根据杆件计算出的楞线侧顶点	
	static const BYTE DT_WING		= 2;	//根据杆件计算出的肢翼侧顶点
	static const BYTE DT_BER2		= 3;	//两端轮廓时根据杆件计算出的另一端楞线侧顶点
	static const BYTE DT_WING2		= 4;	//两端轮廓时根据杆件计算出的另一端肢翼侧顶点
	static const BYTE DT_ARCTANGENT	= 5;	//公切点（要求邻边一边为直线，另一边为圆弧，否则保留不变）顶点
	static const BYTE DT_INNERBEND	= 6;	//直线边内的制弯点（制弯点始终位于前一顶点与后一顶点的连线与制弯线交点上）
	static const BYTE DT_WELDSTART		= 7;	//焊缝边起始顶点（基准杆件始端侧）
	static const BYTE DT_WELDEND		= 8;	//焊缝边终止顶点（基准杆件始端侧）
	static const BYTE DT_WELDSTART_TIP	= 9;	//焊缝边起始顶点（基准杆件始端侧）
	static const BYTE DT_WELDEND_TIP	= 10;	//焊缝边终止顶点（基准杆件始端侧）*/
	static const BYTE DT_PARAMETRIC	= 11;	//参数化板设计所得的顶点*/
	static const BYTE DT_BEND_DATUM_POINT	= 12;	//火曲线共线三面板，制弯基准点	wht 18-12-17
	static const BYTE DT_BEND_EXTEND_POINT	= 13;	//火曲线共线三面板，制弯延伸点

	BYTE type;	//0:普通直边1:指定圆弧夹角2:指定圆弧半径3:椭圆弧
	f3dPoint vertex;				//顶点的位置坐标
	BYTE get_ciPlaneNo() const{ return vertex.feature; }
	BYTE set_ciPlaneNo(BYTE _ciPlaneNo);
	//归属平面,1:表示基准面;2.表示第一个火曲制弯面;3.表示第二个火曲制弯面 wjh-2019.9.4
	__declspec(property(put=set_ciPlaneNo,get=get_ciPlaneNo)) BYTE ciPlaneNo;
	bool m_bCanCut;//判断凹点是否去除的依据
	bool m_bUpVertex;		//
	double rayVecX,rayVecY;	//
	bool m_bWeldEdge; 
	bool m_bRollEdge;				//此边需卷边
	WELDLINE weldline;				//焊缝参数
	short manu_space;				//焊缝加工预留间隙或卷边高度(mm)
	short roll_edge_offset_dist;	//卷边偏移距离 wht 11-04-02
	double radius;					//椭圆弧所需要的参数
	double sector_angle;			//标准圆弧所需要的参数
	GEPOINT center,column_norm;	//椭圆弧所需要的参数（永远输入绝对坐标),center如果表示圆弧圆心(拾取点)时，则输入相对坐标
	//对接点相对于初始零点沿当前焊接边的偏移量（以钢管始->终方向为偏移量正值方向）
	double local_point_y;			//对活点位置，在该点两侧标注
	long get_liLocationAlongOffset() const;
	long set_liLocationAlongOffset(long liAlongOffset);
	//对接点相对于初始零点沿当前焊接边的偏移量（以钢管始->终方向为偏移量正值方向） wjh-2019.11.02
	__declspec(property(put=set_liLocationAlongOffset,get=get_liLocationAlongOffset)) long liLocationAlongOffset;
	//连接板与基准钢管间的对接走向标识模式
	int local_point_vec;			//0.表示无对活点，1.表示对活点两侧标注50-->100的方向与焊接父杆件的start-->end方向相同，2.表示相反
	char get_ciWeldFlowDirection() const;
	char set_ciWeldFlowDirection(char ciFlowDirection);
	//等同local_point_vec，表示焊缝延基准钢管始->终的对接走向模式,0.表示无对活点;1.对接走向标识为始->终;2.对接走向标识为终->始
	__declspec(property(put=set_ciWeldFlowDirection,get=get_ciWeldFlowDirection)) char ciWeldFlowDirection;
	//钢板重新设计时,顶点特殊属性保留，只需更新顶点坐标
	BYTE m_cPosDesignType;			//0:无依赖杆件顶点 1:楞线侧顶点 2:肢翼侧顶点 3:两端轮廓时另一个楞线侧顶点 4:两端轮廓时另一个肢翼侧顶点  
	//在参数化基础上，在钢板局部坐标系下的位置偏移 wjh-2017.12.1
	struct LOCALOFFSET{
		BYTE ciOffMode;	//0.板坐标系下偏移|1.向前顶点偏移|2.向后顶点偏移
		float x,y;		//杆件延伸方向为X值，横向心距为Y值，使板切角变小为正，否则为负 wjh-2017.12.8
	public:
		bool Enabled(){return x!=0||y!=0;}
		__declspec(property(get=Enabled)) bool enabled;
		LOCALOFFSET(){ciOffMode=0;x=y=0;}
	}xOffset;
	long m_hRod;					//顶点计算句柄
	DECLARE_PROPERTY(double,fLengCutWidth);
	DECLARE_PROPERTY(double,fWingCutWidth);
	//bool b_AutoCalHuoqu;			//是否自动计算共线火曲
	CLDSGeneralPlate* m_pBelongPlate;	//临时存储，用于查看顶点属性栏信息
	UCS_STRU* m_pTransCS;				//临时存储，用于查看工作坐标系下的顶点坐标
	union TAG {
		UINT  uiTagValue;
		long  liTagValue;
		void* pTagObj;
	}xTag;	//便于数据处理过程中临时赋予一些含义，存储一些变量信息 wjh-2019.9.4
public:
	PROFILE_VERTEX();
	PROFILE_VERTEX(COORD3D vertice,BYTE arctype=0);
	PROFILE_VERTEX(f3dPoint vertice,BYTE arctype=0);
	PROFILE_VERTEX(double x,double y,double z,BYTE arctype=0);
	void Initialize(double x,double y,double z,BYTE arctype=0);
	void CopyEdgeInfoTo(PROFILE_EDGEINFO* pEdgeInfo);
	void CopyEdgeInfoFrom(const PROFILE_EDGEINFO& edgeinfo);
	//ucs只是在生成椭圆弧时标定椭圆弧工作面用
	void RetrieveArcLine(f3dArcLine &arcline,f3dPoint endPoint,UCS_STRU *ucs);
	//计算钢板组焊时的定位标识点位置（在焊缝线上）所以位置及方向矢量均应转换到同一相对坐标系下方可进行
	GEPOINT RetrieveWeldLocation(double* endPoint,double* tube_line_vec,double* datum_org=NULL);
	//修正钢板组焊线上的定位标志点位置，避免组焊标志点出现在组焊线段外
	void AmendWeldLocationY(double* endPoint,double* tube_line_vec,double* datum_org=NULL);
	//pNextVertex用于检测圆弧一致性
	bool IsEqualWith(PROFILE_VERTEX* pCompVertex,PROFILE_VERTEX* pNextVertex=NULL,PROFILE_VERTEX* pCompNextVertex=NULL);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0,bool bParamPlateVertex=false,UCS_STRU* pPlateCS=NULL);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0,bool bParamPlateVertex=false);
	void SetBelongPlate(CLDSGeneralPlate* pBelongPlate,UCS_STRU* pWcs=NULL){m_pBelongPlate=pBelongPlate;m_pTransCS=pWcs;}
	void SetPosDesignType(BYTE cPosDesType,long hKey){m_cPosDesignType=cPosDesType;m_hRod=hKey;}
	//属性
	DECLARE_PROP_FUNC(PROFILE_VERTEX)
	static void InitProfileVertexPropHashtable();
	static BOOL IsPropEqual(SUBOBJSET &selectObjs, long idProp);	//检测属性值是否相等
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	//int GetPropValueStr(long id,char* valueStr,UCS_STRU* pWcs=NULL,CLDSGeneralPlate* pBelongPlate=NULL);	//根据属性ID得到属性值字符串
};

//附件链表		wht 08-01-19
struct ATTACHPART_ITEM{
	long h,idClsType;
	char sPartNo[MAX_CHAR_GUIGE_16+1];
	ATTACHPART_ITEM(){strcpy(sPartNo,"");}
	void SetKey(DWORD key){h=key;}
};

//连接方向定义:
typedef struct CONNECT_DIRECTION{
	long feature;	//不存储
	BYTE style;		//0:direct表示焊缝线走向;1:表示焊接面工作法向
	BYTE serial;	//用于标识此连接方向与哪一块关联板对应,一般取值1或2,serial>=10时用来记录异形法兰对应的连接杆件
	CLDSVector direct;
	short end_space;	//螺栓到连板边的端距
	short high_space;	//螺栓向连板边角焊缝线投影点的高度
	short tip_space;	//螺栓向连板边肢翼侧的延伸宽度（如角钢肢翼侧宽）
	//根据基准点、Y轴方向以及当前连接方向获取连接方向关联螺栓
	int GetRelaLsRefList(CLsRefList &boltList,IModel *pTower,f3dPoint axis_y,f3dPoint origin);
	CONNECT_DIRECTION(){
		feature=0;
		style=serial=0;
		end_space=high_space=tip_space=30;
	};
	CONNECT_DIRECTION(BYTE cStyle,CLDSVector vec,short nEndSpace,short nHighSpace){
		feature=0;
		style=cStyle;
		direct=vec;
		end_space=nEndSpace;
		high_space=tip_space=nHighSpace;
	};
	void SetKey(DWORD key){serial=(BYTE)key;}
	BOOL IsProfilePole(){return serial>=10;}
}*CONNECT_DIRECTION_PTR;

//参数化板类型
enum PARAM_PLATE_TYPE{
	TYPE_FL					= 2,	//法兰盘
	TYPE_UEND				= 3,	//U型插板
	TYPE_ROLLEND			= 4,	//卷边插板
	TYPE_ELBOWPLATE			= 6,	//肘板
	TYPE_VERTPLATE			= 8,	//垂向加劲板(已废弃不用)
	TYPE_ENDPLATE			= 9,	//封头板(已废弃不用)
	TYPE_CIRRIBPLATE		= 10,	//环向肋板
	TYPE_CIRCOVERPLATE		= 11,	//环向封板 
	TYPE_TUBERIBPLATE		= 12,	//钢管肋板(跨越连板，逐渐被TYPE_CIRRIBPLATE取代)
	TYPE_LINEANGLERIBPLATE	= 13,	//角钢肋板
	TYPE_XEND				= 14,	//十字插板
	TYPE_FLD				= 15,	//对焊法兰
	TYPE_FLP				= 16,	//平焊法兰
	TYPE_WATERSHIELDEDPLATE	= 17,	//遮水板
	TYPE_RIBPLATE			= 18,	//肋板(角肋板，槽肋板，角钢肋板，钢板肋板，钢管肋板，钢管夹板) wht 11-03-21
	TYPE_FLG				= 19,	//刚性法兰
	TYPE_FLR				= 20,	//柔性法兰
	TYPE_BASEPLATE			= 21,	//塔脚底板
	TYPE_SHOEPLATE			= 22,	//塔脚靴板
	TYPE_STATIONCONNPLATE	= 23,	//变电构架端撑柱顶部连接板
	TYPE_STATIONSHOEPLATE	= 24,	//变电构架人字柱间靴板（带端撑时与靴板极相似）
	TYPE_STATIONCROSSPLATE	= 25,	//变电构架端撑柱顶部十字插板
	TYPE_STATIONLINKPLATE	= 26,	//变电构架端撑中间连接板
	TYPE_STATIONNODEPLATE	= 27,	//变电构架人字柱中间连接板
	TYPE_TUBESTAYWIREPLATE	= 28,	//钢管拉线板
	TYPE_WALKWAYCIRPLATE	= 29,	//走道的加劲环板
	TYPE_PEDALPLATE			= 30,	//脚踏板
};

//此结构中的所有坐标都是展开到基面后的绝对坐标
struct CIRCULAR_PLATE_VERTEXSET{
	f3dPoint inner_arc_start,inner_arc_end;			//内弧关键顶点
	f3dPoint inner_arc_midhuoqu,weld_outer_huoqu;	//内圆弧及外部自由边上的制弯点
	f3dPoint weld_outer_w,weld_outer_n;				//两关联焊缝的外端顶点
	f3dPoint weld_outer_w_h,weld_outer_n_h;			//两关联焊缝的外端顶点偏移倒角高后顶点
	f3dPoint weld_outer_w_arc,weld_outer_n_arc;		//两关联焊缝的外端顶部圆弧端顶点
};
struct PLATEEDIT_STATE{
	BYTE ciPickedDisplayMode;
	double m_fLocalWcsOrgX,m_fLocalWcsOrgY;
	double m_fLocalWcsAxisX_x,m_fLocalWcsAxisX_y;
	double m_fLocalWcsAxisY_x,m_fLocalWcsAxisY_y;
	PLATEEDIT_STATE(BYTE _ciPickedDisplayMode=1)
	{
		ciPickedDisplayMode=_ciPickedDisplayMode;
		m_fLocalWcsOrgX=m_fLocalWcsOrgY=0;
		m_fLocalWcsAxisX_x=1;m_fLocalWcsAxisX_y=0;
		m_fLocalWcsAxisY_x=0;m_fLocalWcsAxisY_y=1;
	}
};
class CLDSGeneralPlate : public CLDSPart{
protected:
	long _thick;			//板厚
	BYTE m_cDatumAxis;		//工作平面上的坐标系定义基准轴只能是 'X'或'Y'
	BYTE _cRollProcessType;	// 0.展开后倒角处为两条直线;1.最少刀数保证卷边外侧边长度为L-N;2.最少刀数保证卷边内侧边长度为L-N
	BYTE _ciRollOffsetType;	//暂不存储，取文件属性相应的值，如果钢板卷边有特殊设定，需进行存储
	BYTE _ciDeformType;		//暂不存储，取文件属性响应的值，
	double _fDeformCoef;	//中性层系数
	double _fHuoquR;		//火曲半径
	union{
		bool m_bClockwiseCalProfile;	//去凹点算法，true:按顺时针方向;false:按逆时针方向计算板外形
		BYTE m_cReverseAxisVec;			//反转指定轴 0.不反转  'X'.反转X轴  'Y'.反转Y轴
	};
	//旧0.表示普通板 1.交叉板 2.共用板 3.折叠板 4.单螺栓设计5.多孔螺栓垫板
	//新0.表示普通板 1.折叠板 2.环向板 3.钢管夹板 4.螺栓垫板 5.相对于基准构件位置不变的附加板 6.共用板
	union UNION_TYPE{
		UINT iLegacyStyle;		//由旧_jdb_style更名而来 wjh-2017.1.21
		struct{
			WORD wiLegacyStyle;	//对应TMA V5.1.5之前的_jdb_style
			WORD wiType;		//对应与CLDSParamPlate::m_iParamType整合后的钢板设计参数类型
		};
	}style;
	CLsRefList *pLsRefList;
	UINT m_uiInnerLoopBuffSize;
	char* m_pxInnerLoopBuff;	//loops.N(4)+loop(i).N+SIMPLE_VERTEX存储格式 wjh-2019.9.7
	//通过CLDSPart::HOLE来汇总螺栓孔（含虚拟螺栓）及其余杂项孔（直接靠参数生成的工艺孔）集合　wjh-2017.11.2
	virtual UINT AddMiscellaneousHoles(IXhList<HOLE>* pListHoles);	//在GetHoleSet内调用,以添加除典型螺栓孔外的杂项孔
	void CalEquidistantShape(double minDistance,ATOM_LIST<PROFILE_VERTEX> *pList);
public:
	static BYTE m_ciDisplayVertexType;	//显示钢板轮廓点类型 0.不显示|1.显示轮廓点及文字|2.只显示轮廓点(不显示文字)
	static CHashList<PLATEEDIT_STATE> m_hashDisplayPlateStates;
	ATOM_LIST<PROFILE_VERTEX> vertex_list;				//板底面的顶点表坐标（板相对坐标系下）
	ATOM_LIST<BOLTHOLE_INFO> hole_list;					//板底面的螺栓孔坐标（板相对坐标系下）
	struct GASKETEDGESPACE{
		bool initialized;
		short endSpaceS,endSpaceE,berSpace,wingSpace;
		GASKETEDGESPACE(){endSpaceS=endSpaceE=berSpace=wingSpace=0;initialized=false;}
	}gasketEdgeSpace;
	bool IntelliRetrieveGasketEdgeSpace(GASKETEDGESPACE* pEdgeSpace=NULL,BOOL bInitPosDesType=TRUE);
	bool RetrieveGasketVertexCalType();
	CHashListEx<DESIGN_PLATE_ITEM>params;			
	char m_cPickedDisplayMode;	//临时不存储属性, 被选中构件的显示模式,0.半透明高亮显示;1.单线轮廓显示
	struct DESIGN_PARAM
	{
		bool bExist;	//是否在参数表中找到该参数
		BYTE cValueType;
		BYTE cParamType;
		WORD wItemKey;
		struct DUAL_VALUE{THANDLE hPart;UINT uiAttribute;};
		union{
			double fValue;
			struct {THANDLE hPart;UINT uiAttribute;};
			THANDLE hValue;
			long iValue;
		}value;
		void SetKey(DWORD dwKey){wItemKey=(WORD)dwKey;}
		READONLY_PROPERTY(double,fValue);
		GET(fValue){return ToDouble();}
		READONLY_PROPERTY(long,hValue);
		GET(hValue){return ToLong();}
		READONLY_PROPERTY(long,iValue);
		GET(iValue){return ToLong();}
		READONLY_PROPERTY(UINT,uiAttribute);
		GET(uiAttribute){
			if(cValueType==3)
				return value.uiAttribute;
			else
				return 0;
		}
	public:
		void Set(double fValue,bool exist=true);
		void Set(long iValue,bool exist=true);
		double ToDouble();
		long   ToLong();
		void CopyValue(const DESIGN_PARAM& item){bExist=item.bExist;value.fValue=item.value.fValue;}
		void InitBasicInfo(WORD wKey,BYTE cValType,BYTE cParamType=PARAM_SHAPE);
	};
	DESIGN_PLATE_ITEM *GetDesignItem(WORD wKey);
	virtual BOOL GetDesignItemValue(WORD wKey, double *pfValue,CHashTable<DESIGN_PLATE_ITEM*>*pTbl=NULL);
	virtual BOOL GetDesignItemValue(WORD wKey, long *piVal,CHashTable<DESIGN_PLATE_ITEM*>*pTbl=NULL);
	virtual BOOL GetDesignItemValue(WORD wKey, DESIGN_PARAM *pParamItem,CHashTable<DESIGN_PLATE_ITEM*>*pTbl=NULL);
public:
	CLDSGeneralPlate();
	~CLDSGeneralPlate();
	virtual bool GetPropParam(long idObjProp,PROPVAL* pVal=NULL);
	virtual bool SetPropParam(long idObjProp,PROPVAL& propval);
	virtual bool ProcessConstraintFromMasterObj(CLDSDbObject* pMasterObj,CONSTRAINT* pConstraint);
	virtual bool HasPreferShadowDirection(double* vShadowDirection){return false;}
	virtual CLDSGeneralPlate* CloneCopy(double offset,double* vUdfDirection);
	virtual CLDSGeneralPlate* ShadowCopy(double offset,double* vUdfDirection);
	virtual CLDSGeneralPlate* CloneCopy(GEPOINT vOffset,bool localAcsCoord=true);
	virtual CLDSGeneralPlate* ShadowCopy(double offset,bool localAcsCoord=true);
	virtual bool IsGeomMir(CLDSGeneralPlate *pPlate,MIRMSG mirmsg,BOOL bCheckProfile=TRUE,const double *srcGeomCenterVec=NULL);
	virtual bool IsEqualMirWith(CLDSGeneralPlate* pMirPlate,MIRMSG mirmsg);
	virtual bool OptimizeStructure();
	virtual bool StableCheck(ILog2File* pRptLog,DWORD dwCheckFlag=0xffff,
		double (*CallBackDialogUIFunc)(CLDSLineAngle* pAngle,double dfAxisNByBolts,
				DWORD *pdwCheckFlag)=NULL);
	struct EDGEBOLT{
		BOOL bToHuoquLine;		//
		long hBolt;				//靠近钢板边界的螺栓句柄
		double actualEndSpace;	//实际的螺栓边距
		long needEndSpace;		//按规定需要的螺栓边距
		EDGEBOLT(){hBolt=0;actualEndSpace=0;needEndSpace=0;bToHuoquLine=FALSE;}
	};
protected://获取钢板的子分类类型
	virtual WORD InternalGetDesignType();		//该函数纯粹是因为解决不能直接get=派生虚函数导致 wjh-2017.1.20
	virtual void InternalSetDesignType(WORD wiDesignType);
	virtual PROFILE_VERTEX* InternalGet3PlanePlatePeakVertex(){return NULL;}
public:
	//1) 继承原TMA的钢板样式CLDSPlate::jdb_style
	virtual WORD GetLegacyStyle();
	virtual void SetLegacyStyle(WORD wiLegacyStyle);
	PROPERTY(WORD,wiLegacyStyle); //对应TMA V5.1.5之前的CLDSPlate::jdb_style
	VIRTUAL_SET(wiLegacyStyle){SetLegacyStyle(value);}
	VIRTUAL_GET(wiLegacyStyle){return GetLegacyStyle();}
	//2) 拓展整合参数化板参数类型后的，设计类型
	static const WORD CONNECT_PLATE = 100;	//连接板
	static const WORD WASHER_PLATE  = 101;	//螺栓垫板
	static const WORD WRAPPER_PLATE	= 102;	//包钢板
	static const WORD RIB_PLATE		= 103;	//加强劲肋板
	static const WORD ELSE_PLATE	=1000;	//其余板
	PROPERTY(WORD,wiType);	//整合CLDSParamPlate::m_iParamType后的钢板设计(参数)类型
	VIRTUAL_SET(wiType){InternalSetDesignType(value);}
	VIRTUAL_GET(wiType){return InternalGetDesignType();}

	static const BYTE VALIDATE_PROFILE_BASIC	 = 0x01;
	static const BYTE VALIDATE_PROFILE_EDGESPACE = 0x02;
	virtual BOOL ValidateProfile(double epsilon=1.0,EDGEBOLT *endBoltInfoArr=NULL,int *count=NULL,BYTE cbValidateFlag=0xff);
	virtual int IsEqualProfile(CLDSGeneralPlate* pMirPlate,BOOL bCompareLs=TRUE,double epsilon=1.0){return 1;}
	virtual bool CompareLabelPart(CLDSPart *pPeerPart,MIRMSG* pMirmsg=NULL,BOOL bCmpls=TRUE,PEER_PART *pPeerInfo=NULL,double epsilon=1.0);
	virtual void GetPeerPlateUcs(CLDSGeneralPlate *pPeerPlate,int base_i,BOOL bReverse,UCS_STRU& acs);
	virtual CLDSNode *GetBaseNode(bool bClearLocks=true){return NULL;}	//返回连接板的依赖节点
	virtual long get_BaseNode(){return 0;}	//返回连接板的依赖节点
	virtual long get_BaseRod() {return 0;}	//返回钢板基准节点  wht 09-12-12
	__declspec(property(get=get_BaseNode)) long m_hBaseNode;
	__declspec(property(get=get_BaseRod))  long m_hBaseRod;
	virtual bool IsPtInPlate(double* pos_coord);
	virtual bool GetBendlineVertexies(PROFILE_VERTEX** ppBendLine1Start=NULL,PROFILE_VERTEX** ppBendLine1End=NULL,
		PROFILE_VERTEX** ppBendLine2Start=NULL,PROFILE_VERTEX** ppBendLine2End=NULL);
	virtual bool IntelliRecogWeldLine(PARTSET* pSearchSet=NULL);
	virtual bool IsWeldWith(double* lineStartCoord,double* lineEndCoord);
	virtual bool IsWeldWith(f3dArcLine& arcline);
	virtual bool IsConnectPlate();	//钢板是否为有杆件及螺栓连接的连接板
	virtual bool IsPeakPointPlate(){return false;}	//是否为带顶角的三面板　wjh-2016.1.9
	virtual bool IsFoldPlate(){return false;}		//是否为三面折叠板　wjh-2016.1.9
	virtual bool IsTubeFoldPlate();	//变电构架曲梁上折叠板（双火曲面为连接面，基面反而仅是过渡不是连接面） wjh-2017.11.8
	virtual bool IsSplayFoldPlate();	//变电构件三角梁顶梁上八字板（基准面并非沿钢管轴向） wjh-2017.11.8
	virtual bool IsMaybeBasePlate();	//是否为塔脚底座板（用于添加地脚螺栓）
	virtual void CalRollLineAndHuoquLine(PROFILE_VERTEX *pStartVertex,PROFILE_VERTEX *pEndVertex,PROFILE_VERTEX *pPreVertex,PROFILE_VERTEX *pNextVertex,
		f3dLine &huoqu_line,f3dLine &roll_line,BOOL *pbRetainRollLineStart=NULL,BOOL *pbRetainRollLineEnd=NULL);
	virtual bool ProcessRollEdgeVertex(ATOM_LIST<PROFILE_VERTEX> &vertex_list);
	virtual bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	//输出最小包络矩形范围及面积
	virtual f2dRect GetMinWrapRect(UCS_STRU *Rectucs=NULL,double offset=0,BOOL bExportDeformedProfile=FALSE);
	virtual double CalAreaOfMinWrapRect(SCOPE_STRU *pScope=NULL);
	virtual double CalAreaOfNetWrap();
	virtual double CalPerimeter();		//计算钢板周长,mm
	virtual double CalWeldEdgeLength();	//计算钢板焊缝边长,mm
	virtual double CalWeldLineLength();	//计算钢板实际焊缝线长
	virtual BOOL IsAngleRibPlate();		//是否为角钢带劲板
	virtual BOOL IsBushWeldPlate();		//是否为套管焊接板
	virtual long   GetEdgeNum(double lenGreatThan=0);	//计算钢板切割长度大于指定值(mm)的边数
	virtual double GetNormOffset(){return 0;}	//获取钢板底面相对于基准原点的法向偏移量
	virtual UCS_STRU GetUnifiedACS();
	virtual int GetFaceN(){ return 1;}							//钢板面数
	virtual f3dLine GetHuoquLine(int index){return f3dLine();}	//获取指火曲线(0.第1、2面的火曲线 1.第2、3面的获取线)，为板坐标系下的相对坐标
	virtual f3dPoint GetHuoquFaceNorm(int faceI){return f3dPoint();}//获取指火曲面的法线，为绝对坐标
	virtual int GetNormFaceI(CLDSBolt* pBolt);//输入一个法线向量，返回此向量所在的板平面号0,1,2
	virtual int GetNormFaceI(const double* vCurrNormal);	//输入一个法线向量，返回此向量所在的板平面号0,1,2
	virtual int GetFaceNoByPos(GEPOINT pos);	//输入一个点坐标(展开到钢板坐标系)，返回此坐标所在的板平面号
	virtual int WriteProcessBuffer(CBuffer &buffer,bool bSaveSubSerial=false,long version=0); //得到制弯角钢工艺扩展信息buffer
	virtual BOOL GetProfileByBoltHoles(ATOM_LIST<f3dPoint>& pt_list);
	virtual int CompareProfileVertex(ATOM_LIST<f3dPoint>& ptList1,ATOM_LIST<f3dPoint>& ptList2,BOOL bReverseOrder,int base_i,double epsilon);
	virtual int CompareVertexOnLine(ATOM_LIST<f3dPoint>& ptList1,ATOM_LIST<f3dPoint>& ptList2,BOOL bReverseOrder,double epsilon);
	virtual bool IsEqualProfile(ATOM_LIST<f3dPoint>& ptList1,ATOM_LIST<f3dPoint>& ptList2,double epsilon,int& base_i,BOOL& bReverse);
	virtual int GetAllBoltHoles(ATOM_LIST<BOLTHOLE_INFO>& holeList);
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart);
#endif	
	virtual double GetHuoQuAngle(int iFace,f3dLine *pHuoQuLine=NULL)=0;
	//要求输入的顶点必须是此连接板上的顶点,否则
	//可能不进行转换或转换错误(由展开相对坐标-->实际相对坐标)
	//bCalVertex=true表示获取顶点false表示获取矢量方向
	virtual f3dPoint GetRealVertex(f3dPoint vertice,bool bCalVertex=true)=0;//{return vertice;}
	//将制弯面上的绝对坐标（方向）-->展开至钢板相对（装配）坐标系;
	//bSpreadVertex=true表示展开顶点false表示展开矢量方向
	virtual f3dPoint GetSpreadVertex(const double* V,int face_i=0,bool bSpreadVertex=true)=0;//{return V;}
	//要求输入的螺栓必须是此连接板上的螺栓,否则
	//可能不进行转换或转换错误(由实际绝对坐标-->展开相对坐标)
	virtual BOOL SpreadLsPos(CLDSBolt *pLs,double* ls_pos){return FALSE;}
	//要求输入的顶点必须是此连接板上的顶点,否则
	//可能不进行转换或转换错误(由未考虑变形的初始点转换为考虑变形的点)
	virtual f3dPoint GetDeformedVertex(f3dPoint vertice);
	//调整钢板坐标系后计算火曲变形点坐标时使用 new_ucs为传入坐标所在的相对坐标系 wht 10-12-08
	virtual f3dPoint GetDeformedVertex(f3dPoint vertice,UCS_STRU new_ucs);
	//iResidePlane=0,1,2,分别表示基准面、１火曲面、２火曲面
	static const int TOP_PLANE	= 2;	//计算钢板顶面与螺栓轴线交点
	static const int MID_PLANE	= 3;	//计算钢板中性层与螺栓轴线交点
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* iResidePlane=NULL,int iIntersPlane=0){return false;}
	//GetBoltIntersPos参数顺序不合理(作为可选输出参数应置于最后)，出于代码维护安全考虑以后应逐步用CalBoltIntersPos取代 wjh-2016.5.7
	virtual bool CalBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int iIntersPlane=0,int* iResidePlane=NULL){
		return GetBoltIntersPos(pBolt,inters_coords,iResidePlane,iIntersPlane);
	}
	//cbFlag由FROM_MCS、FROM_ACS、IS_VECTOR与IS_VERTEX进行组合，默认为FROM_MCS|IS_VERTEX
	//iResidePlane取值:1、2、3表示待转换点或矢量所驻存的面号 wjh-2015.2.27
	static const BYTE FROM_MCS =0x00;	//coord3d为模型坐标
	static const BYTE FROM_ACS =0x01;	//coord3d为当前钢板下装配坐标(即相对坐标)
	static const BYTE IS_VERTEX=0x00;	//coord3d为位置点, 执行位置坐标转换
	static const BYTE IS_VECTOR=0x02;	//coord3d为方向量, 执行矢量坐标转换
	virtual f3dPoint SpreadToACS(const double* coord3d,int iResidePlane=1,BYTE cbFlag=0){return f3dPoint(coord3d);};
	static const BYTE TO_MCS =0x04;	//转换至模型坐标
	static const BYTE TO_ACS =0x08;	//转换至为当前钢板下装配坐标(即相对坐标)
	virtual GEPOINT TransToActual(const double* coord3d,int iResidePlane=1,BYTE cbFlag=0)=0;
	//螺栓链表函数
	virtual int GatherHoleWalls(CHash64ListEx<HOLE_WALL>& hashHoles);
	virtual bool UpdateHoleWall(HOLE_WALL* pHoleWall);	//当螺栓或钢板发生变化时，实时更新孔壁
	virtual CLsRefList *GetLsRefList(){return pLsRefList;}
	CLsRef* GetFirstLsRef(){return pLsRefList->GetFirst();}
	CLsRef* GetNextLsRef(){return pLsRefList->GetNext();}
	virtual UINT GetHoleSet(IXhList<HOLE>* pListHoles,bool bOnlyBoltHole=false);	//返回值为孔总数
	virtual int GetLsCount(short d=0);
	virtual CLsRef* AppendLsRef(CLsRef &ls_ref,BOOL bNearLsCheck=TRUE);
	void EmptyLsRef(){ pLsRefList->Empty();}
	CLsRef* FindLsByUcsPos(f3dPoint base,UCS_STRU ucs,int iNormX0Y1Z2,double epsilon=EPS2)
	{return pLsRefList->FindLsByUcsPos(base,ucs,iNormX0Y1Z2,epsilon);}
	CLsRef* FindLsByPos(f3dPoint base,f3dPoint work_norm,double epsilon=EPS2)
	{return pLsRefList->FindLsByPos(base,work_norm,epsilon);}
	virtual CLsRef* FindLsByHandle(THANDLE h){return pLsRefList->FromHandle(h);}
	virtual double GetHuoQuR(int iFace=0);
	virtual double GetDeformCoef(int iFace=0);
	virtual void SetHuoQuR(double r=0,int iFace=0){_fHuoquR=r;}
	virtual void SetDeformCoef(double deform_coef=0.35,int iFace=0){_fDeformCoef=deform_coef;}
	//板上螺栓孔操作
	BOLTHOLE_INFO* EnumFirstBoltHole(){return hole_list.GetFirst();}
	BOLTHOLE_INFO* EnumNextBoltHole(){return hole_list.GetNext();}
	//同步属性
	static const DWORD PROP_DISCONNECT	= 0;	//断开
	static const DWORD PROP_SYNC_COPY	= 1;	//同步对称
	static MEMORY_BLOCK MPCL;	//Mirror Property Control List
	static PROPLIST_TYPE propMirSync;
	static void InitPropMirBitsMap();
	virtual PROPLIST_TYPE* GetSyncPropList(){return &propMirSync;}
	virtual MEMORY_BLOCK GetDefaultMPCL(){return MPCL;}
	virtual bool IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr);
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部位置对称无关的属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
public:
	DECLARE_PROPERTY(BYTE,cDatumAxis); //工作平面上的坐标系定义基准轴只能是 'X'或'Y'
	PROPERTY(bool,bClockwiseCalProfile);
	GET(bClockwiseCalProfile){return m_bClockwiseCalProfile;}
	SET(bClockwiseCalProfile){m_bClockwiseCalProfile=value;}
	PROPERTY(BYTE,m_cRollProcessType);
	GET(m_cRollProcessType){
		if (_cRollProcessType==0xFF)
			return  BelongModel()->GetRollProcessType();
		else 
			return _cRollProcessType;
	}
	SET(m_cRollProcessType){_cRollProcessType=value;}
	PROPERTY(BYTE,m_ciRollOffsetType);
	GET(m_ciRollOffsetType){
		if (_ciRollOffsetType==0xFF)
			return  BelongModel()->GetRollOffsetType();
		else 
			return _ciRollOffsetType;
	}
	SET(m_ciRollOffsetType){_ciRollOffsetType=value;}
	PROPERTY(BYTE,m_ciDeformType);
	GET(m_ciDeformType){
		if (_ciDeformType==0xFF)
			return  BelongModel()->GetDeformType();
		else 
			return _ciDeformType;
	}
	SET(m_ciDeformType){_ciDeformType=value;}
	DECLARE_PROPERTY(double,m_fDeformCoef);
	DECLARE_PROPERTY(double,m_fDeformCoef2);
	DECLARE_PROPERTY(double,m_fHuoquR);
	DECLARE_PROPERTY(double,m_fHuoquR2);
};
class DYNA_PROPITEM_GROUP{
public:
	//static CXhChar100 HELPLINK_URL;
	DWORD m_idGroup;	//分组标识
	CHashStrList<PROPLIST_ITEM>propHashtable;
	CHashSet<PROPLIST_ITEM*> hashPropItemById;
	CHashTable<DWORD>propStatusHashtable;
	//static int GetPropStr(long id, char *propStr);
	//static CXhChar50 GetPropName(long id);
	DYNA_PROPITEM_GROUP(DWORD key){m_idGroup=key;}
	long GetPropID(const char* propStr);
	int GetPropStatus(long id);
	BOOL SetPropStatus(long id,BOOL bHide);
	PROPLIST_ITEM* GetPropItem(long id);
	PROPLIST_ITEM* AddPropItem(char* propKey,PROPLIST_ITEM item);
	PROPLIST_ITEM* UpdatePropItem(char* propKey,char* propName=NULL);
	//BOOL IsPropEqual(CXhPtrSet<cls_name> &objSet,long idProp);
};
struct WELDCOVER_PLATE : public ICompareClass{
	CLDSGeneralPlate* pWeldPlate;
	double alfa;	//弧度制，
	double innerR;	//焊烧死角处工艺流水孔半径
	GEPOINT vxRayVec;
	WELDCOVER_PLATE(CLDSGeneralPlate* pRelaPlate=NULL,double _innerR=0){
		pWeldPlate=pRelaPlate;
		innerR=_innerR;
		alfa=0;	//需要后续计算
	}
	virtual int Compare(const ICompareClass *pCompareObj)const
	{
		WELDCOVER_PLATE* pCover2=(WELDCOVER_PLATE*)pCompareObj;
		if(alfa>pCover2->alfa)
			return -1;
		else if(alfa<pCover2->alfa)
			return  1;
		else
			return 0;
	}
};
class CLDSStdPartModel;
class CLDSParamPlate : public CLDSGeneralPlate{
	//此结构中的所有坐标都是展开到基面后的绝对坐标
	CIRCULAR_PLATE_VERTEXSET vertexset;	//出图时需要使用该结构，不用保存临时使用 wht 110-05-30
	struct CIRCULAR_PLATE_KEYVEC{
		f3dPoint huoqufacenorm;			//制弯面的工作法向(绝对坐标)
		f3dPoint huoquaxis_vec;			//火曲线方向,绕此轴逆时针旋转<180°角可将基面法线旋转至制弯面法线
		f3dPoint weldvec_w,weldvec_n;	//两关联焊缝走向(绝对坐标)
		f3dPoint weldvec_w_vert,weldvec_n_vert;	//两关联焊缝垂线方向(绝对坐标,且指向板内)
		f3dPoint weldvec_nw_vert;		//第二个关联焊缝垂线方向在基面内的展开方向
		f3dPoint weldvec_nw;			//第二个关联方向需制弯时焊缝走向在基面内的展开方向
		f3dPoint freevec,freevec_vert;	//自由边方向,自由边垂线方向
	}keyvecset;	//用于保存火曲信息 wht 11-04-11
	union RIBPROFILE_PARA{
		struct ELBOW{
			double fBottomWidth;//底部宽度
			double fInnerLen;	//内侧长度
			double fTopWidth;	//顶部宽度
			double fOuterLen;	//外侧长度
			double fCutAngleLen;//切角宽度
		}common;//肘板
		struct GENERAL{
			double A;			//肋板高度fBottomWidth;
			double B;			//肋板长度fInnerLen;	
			double C;			//台阶高度fTopWidth;	
			double D;			//顶部宽度fOuterLen;	
			double E;			//切角宽度fCutAngleLen;
		}corner;	//塔脚肋板及其它角肋板
		void ClearZero(){corner.A=corner.B=corner.C=corner.D=corner.E=0.0;}
	};
	//计算外形顶点根据连接方向
	BOOL CalProfileVertexByConnVector(CONNECT_DIRECTION *pVec,CLDSNode *pBaseNode);
protected:
	int _iParamType;	//PARAM_PLATE_TYPE
	/*参数化板的具体样式，如
	肋板：普通型、角钢肋板、角型肋板、槽型肋板
	环向板：普通型、封板型、直线型槽型支撑、圆弧型槽型支撑、直线型角型支撑、圆弧型角型支撑、遮水板
	*/
	long _iStyle;
	BOOL _bStdPart;	//TRUE:标准构件 FALSE:非标准构件
public:	//公用属性
	PROPERTY(int,m_iParamType); 
	GET(m_iParamType){
		CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
		if(pMotherObj==this)
			return _iParamType;
		else
			return pMotherObj->m_iParamType;
	}
	SET(m_iParamType){
		CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
		_iParamType = value;
		if(pMotherObj!=this)
			pMotherObj->m_iParamType=value;
	}
	PROPERTY(long,m_iStyle); 
	GET(m_iStyle){
		CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
		if(pMotherObj==this)
			return _iStyle;
		else
			return pMotherObj->m_iStyle;
	}
	SET(m_iStyle){
		if(value>=0)
		{
			CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
			_iStyle = value;
			if(pMotherObj!=this)
				pMotherObj->m_iStyle=value;
		}
	}
	PROPERTY(double,Thick); 
	VIRTUAL_SET(Thick);
	VIRTUAL_GET(Thick);
	//与CLDSPart::Thick进行统一处理，否则易出现代码编写隐患 wjh-2014.12.31
	PROPERTY(long,thick); 
	GET(thick){ return ftoi(Thick); }
	SET(thick){ Thick=value; }

	PROPERTY(BOOL,m_bStdPart); 
	GET(m_bStdPart){
		CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
		if(pMotherObj==this)
			return _bStdPart;
		else
			return pMotherObj->m_bStdPart;
	}
	SET(m_bStdPart){
		CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
		_bStdPart = value;
		if(pMotherObj!=this)
			pMotherObj->m_bStdPart=value;
	}
public:
	struct PROP_PARAMITEM
	{
		WORD wItemKey;
		CXhChar50 strPropName,strPropHelp;
		void Set(DWORD key){wItemKey=(WORD)key;}
		PROP_PARAMITEM(const char* propName="",const char* propHelp="")
		{
			strPropName.Copy(propName);
			strPropHelp.Copy(propHelp);
		}
	};
	bool IsElbowRibPlate();	//是否为钢管肘板型肋板，为统一肘板与肘板型肋板而设 wjh-2015.8.6
	struct RIBPLATE_CORNER{	//角肋板-普通两板间的角肋板(如角钢塔脚肋板)
		static const BYTE SUB_TYPE = 1;
		static CHashListEx<PROP_PARAMITEM> hashPropItems;
	public:
		DESIGN_PARAM hDatumPlate;	//对应数据中的'A'参数
		DESIGN_PARAM hNode;			//对应数据中的'B'参数
		DESIGN_PARAM hAssisPlate1;	//对应数据中的'C'参数
		DESIGN_PARAM hAssisPlate2;	//对应数据中的'D'参数
		DESIGN_PARAM hBolt;			//'O'参数
		DESIGN_PARAM hVecPart;		//'V'参数
		DESIGN_PARAM fOffsetX;		//'X'参数
		DESIGN_PARAM fOffsetZ;		//'Z'参数
		DESIGN_PARAM blSideEdgeLength;	//是否根据高度计算侧边长度A
		DESIGN_PARAM A,B,C,D,E,F,G,H,I;	//肋板示意图中的各外形参数
		DESIGN_PARAM Ox;			//"Ox";
		DESIGN_PARAM Oy;			//"Oy";
		DESIGN_PARAM hRelatedAngle;	//"Pa";
		DESIGN_PARAM rayAngle;		//"Ra";
		DESIGN_PARAM rayDist;		//"Rd"
		DESIGN_PARAM Xx;//,&VXx
		DESIGN_PARAM Xy;//,&VXy
		DESIGN_PARAM fGradientDegAngle;//"Ga"
		DESIGN_PARAM e;	//倒角模式：0.切角 1.切弧		wxc-2016.12.20
		void LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl=NULL);
	};
	struct RIBPLATE_ELBOW{	//角肋板-肘板型肋板
		static const BYTE SUB_TYPE = 2;
		static CHashListEx<PROP_PARAMITEM> hashPropItems;
	public:
		DESIGN_PARAM A,B,C,D,E,F,G,H;
		DESIGN_PARAM J;			//环向定位角的旋转起始边类型：0.基准边;1.焊道线;2.定位螺栓;3.指定(全局)方向;4.底板局部坐标点
		DESIGN_PARAM hBtmPlate;	//底部基准钢板句柄,与肘板‘L’参数对应
		DESIGN_PARAM hDatumTube;//侧向靠接钢管句柄,与肘板‘K’参数对应
		DESIGN_PARAM rotAngle;	//环向定位角的旋转起始边自上述基准边或焊道线的额外定位偏转角,与肘板‘Y’参数对应，°
		DESIGN_PARAM hDatumBolt;//2.定位螺栓
		// 3.指定(全局)方向
		// 4.肘板底边(X轴)在底板上的延伸方向标识点的局部坐标
		DESIGN_PARAM vAxisXVecX,vAxisXVecY,vAxisXVecZ;
		DESIGN_PARAM margin;	//-1.默认指定底边长度;>=0.延伸到底板边缘(整数)
		DESIGN_PARAM V;	//0.沿钢管轴向肘板；1.竖直带自动正位的钢板
		DESIGN_PARAM R;	//底部外环半径
		DESIGN_PARAM R1;	//流水孔1半径
		DESIGN_PARAM R2;	//流水孔2半径
		DESIGN_PARAM Z;	//-1.钢板底面布置肘板; 1.钢板顶面布置肘板
		DESIGN_PARAM b;	//0.旧版数据; 1.2015年8月规范后新版数据 wjh-2015.8.08
		DESIGN_PARAM e;	//倒角模式：0.切角 1.切弧		wxc-2016.12.20
		void InitPropItemHashtable();	//初始肘板型肋板各参数的基础信息(数据键值、类型、属性名称及帮助提示）
		void LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl=NULL);
	};
	struct RIBPLATE_ANGLE{	//角钢肋板
		//long A=0,B=0,I=0,C=0;
		//double L,W,S,D=0,wing_thick=0,H=0,R=0,d=0,P=0;
		DESIGN_PARAM A,B,C,I;
		DESIGN_PARAM L,W,S,D,wing_thick,H,R,d,P,E;
		DESIGN_PARAM Ly,Wy,Hy,N,Nx,Ny,Nz;
		void LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl=NULL);
	};
	struct RIBPLATE_TUBE{	//钢管上的矩形肋板
		static CHashListEx<PROP_PARAMITEM> hashPropItems;
	public:
		DESIGN_PARAM J;			//环向定位角的旋转起始边类型：0.基准边；1.焊道线
		DESIGN_PARAM Nt;		//法线类型：0.钢板延向 1.钢板轴向 2.自定义
		DESIGN_PARAM hNode;		//对应数据中的'B'参数
		DESIGN_PARAM hDatumTube;//参数 'A'
		DESIGN_PARAM rotAngle;	//环向定位角的旋转起始边自上述基准边或焊道线的额外定位偏转角,与肘板‘Y’参数对应，°
		DESIGN_PARAM W,H;	//沿向宽度与垂向高度
		DESIGN_PARAM Lt,Rt;	//顶部X+/X-圆弧(切角)类型
		DESIGN_PARAM Lw,Lh,Rw,Rh; //顶部X+/X-圆弧(切角)半径
		DESIGN_PARAM DM,DX;
		DESIGN_PARAM Nx,Ny,Nz,Wx,Wy,Wz;
		//long A=0,B=0,I=0,C=0;
		//double L,W,S,D=0,wing_thick=0,H=0,R=0,d=0,P=0;
		DESIGN_PARAM L,S,D,wing_thick,R,d,P;
		void LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl=NULL);
	};
	struct RIBPLATE_STAY_WIRE{	//钢管上的拉线板
		static CHashListEx<PROP_PARAMITEM> hashPropItems;
	public:
		DESIGN_PARAM J;				//环向定位角的旋转起始边类型：0.基准边；1.焊道线
		DESIGN_PARAM hDatumTube;	//参数 'A'
		DESIGN_PARAM hAssisPlate;	//参数 'C'
		DESIGN_PARAM rotAngle;		//环向定位角的旋转起始边自上述基准边或焊道线的额外定位偏转角,与肘板‘Y’参数对应，°
		DESIGN_PARAM L,W,H;			//沿向宽度与垂向高度
		DESIGN_PARAM P,R,d,N;		//拉线孔信息
		void LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl=NULL);
	};
	struct RIBPLATE_PLATE{	//钢板肋板
		static CHashListEx<PROP_PARAMITEM> hashPropItems;
	public:
		DESIGN_PARAM hDatumPlate;//侧向靠接钢管句柄,与肘板‘A’参数对应
		DESIGN_PARAM W,H;	//沿向宽度与垂向高度
		DESIGN_PARAM hStartBolt;//对应O点的起点基准螺栓
		DESIGN_PARAM Ox,Oy;
		DESIGN_PARAM hVecPart;	//O->E的走向基准杆件或E点对应基准螺栓句柄hEndBolt
		DESIGN_PARAM Vx,Vy;		//O->E的走向用户输入定义值
		DESIGN_PARAM OffsetX,OffsetZ;
		DESIGN_PARAM fGradientDegAngle;//"Ga"
		//扩展功能后的新增参数 wjh-2016.5.4
		//O点及E点均为hDatumPlate钢板下的局部坐标
		DESIGN_PARAM Ex,Ey;			//O->E为肋板基准面的走向始末基点
		DESIGN_PARAM hTipBolt;		//制弯面末端T点的基准螺栓
		DESIGN_PARAM Bx,By;			//O->E为肋板基准面的走向始末基点
		DESIGN_PARAM Tx,Ty;			//制弯面末端T点的用户输入定义值
		DESIGN_PARAM hBendVecPart;	//B->T为带制弯肋板基面终端火曲点B->制弯面末端T点
		DESIGN_PARAM TipOffsetX,TipOffsetZ;
		void LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl=NULL);
	};
public:	//仅设计以钢板为底板的参数化肋板时使用以下两类
	struct BASERIBPLANE{	//存储模型坐标
		int iResidePlane;	//-1表示未初始化
		GEPOINT origin;
		GEPOINT vpBendPoint;
		GEPOINT direction,norm;
		BASERIBPLANE(){iResidePlane=-1;}
	};
	struct BENDRIBPLANE{	//存储模型坐标
		int iResidePlane;	//-1表示未初始化
		GEPOINT vpTipPoint,pickPoint;	//制弯点及制弯面末端顶点
		GEPOINT direction,norm;
		BENDRIBPLANE(){iResidePlane=-1;}
	};
	//BYTE m_cPlateType;	//肋板类型 0.角肋板 1.槽肋板 2.角钢肋板 3.钢板肋板 4.钢管肋板
public:
	CHashListEx<ATTACHPART_ITEM> AttachPart;
	CLDSVector heteroFlConnVec[3];	//变坡处异型法兰的三根杆件连接方向
	CHashListEx<CONNECT_DIRECTION> ConnDirectList;
	long m_iLsRayNo;	//螺栓所在的射线号
	BOOL m_bAutoCalParamPlateUcs;	//TRUE:自动计算参数板坐标系 FALSE:指定参数板坐标系
	CLDSVector workNorm;	//工作法线的设计参数
	struct DESIGN{
		bool inited;	//由于origin暂未存到数据文件中,临时添加此属性表示是否已为origin,alongvec进行赋值 wjh-2016.9.6
		CLDSPoint origin;
		CLDSVector alongvec;	//靠接板相对坐标
	}design;
public:
	//1.表示最小包容矩形统计;2.表示净面积统计(去掉内环)
	static int m_iCirclePlankAreaType;
	//标准构件相关
	double m_fNormOffset;	//钢板基准工作面相对于默认位置的法向偏心距
	void SetNormOffset(double fNormOffset);	//fNormOffset为钢板底面相对于基准原点的法向偏移量
	virtual double GetNormOffset();		//获取钢板底面相对于基准原点的法向偏移量
	virtual long get_BaseNode();//返回连接板的依赖节点
	virtual long get_BaseRod();	//返回钢板基准节点  wht 09-12-12
	virtual bool HasPreferShadowDirection(double* vShadowDirection);
	virtual bool IsBelongTo(int iClassTypeId)const;
	virtual CLDSPart* NewCopy(bool bByShadowMode=false);
	virtual CLDSGeneralPlate* CloneCopy(double offset,double* vUdfDirection);
	virtual CLDSGeneralPlate* ShadowCopy(double offset,double* vUdfDirection);
	virtual CLDSGeneralPlate* CloneCopy(GEPOINT vOffset,bool localAcsCoord=true);
	virtual CLDSGeneralPlate* ShadowCopy(double offset,bool localAcsCoord=true);
	double GetOutterPlaneOffset(const f3dPoint &outter_vec);
	CLDSParamPlate();
	~CLDSParamPlate();
	void SetPartNo(const char *part_no,bool bIncStdPart=false);		//自动编号时方便使用同一函数设置编号，不会错误修改标准构件编号	
	bool IsPlate() const{return true;}
	bool IsZhiWan();
	double GetWidth();				//获得展开图宽度
	double GetLength();				//获得展开图长度
	double GetPrjDrawingWidth();	//获得投影图宽度
	double GetPrjDrawingLength();	//获得投影图长度
	double GetWeight(BOOL bSolidLength=TRUE);
	virtual CLDSNode *GetBaseNode(bool bClearLocks=true);	//返回钢板基准节点  wht 09-12-12
	void CopyProperty(CLDSDbObject *pObj);
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	BOOL IsCanManualModify();	//判断是否支持手动修改
	BOOL UpdateProfileVertex(ATOM_LIST<PROFILE_VERTEX> &src_vertex_list);	//更新参数化板外形轮廓点
	BOOL DesignPlate(bool bDesignBolt=true);	//在某些情况下螺栓是单独设计，为加快速度不需要重新设计螺栓位置
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	virtual CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
	//static const BYTE VALIDATE_PROFILE_BASIC	 = 0x01;
	//static const BYTE VALIDATE_PROFILE_EDGESPACE = 0x02;
	virtual BOOL ValidateProfile(double epsilon=1.0,EDGEBOLT *endBoltInfoArr=NULL,int *count=NULL,BYTE cbValidateFlag=0xff);
	virtual int IsEqualProfile(CLDSGeneralPlate* pMirPlate,BOOL bCompareLs=TRUE,double epsilon=1.0);
	bool IsEqual(CLDSParamPlate* pMirPlate,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	BOOL IsValidProfile(double epsilon=1.0);	//检查插板外形是否合法，目前主要检查边距 wht 11-04-16
	//设计参数列表函数
	void EmptyDesignItems(){params.Empty();}
	char* GetDesignItemNotes(WORD wKey);
	BOOL IsHasDesignItemValue(WORD wKey);
	BOOL DeleteDesignItem(WORD wKey);
	//BOOL SetDesignItemValue(char cKey,double fValue,char *sNotes=NULL);
	BOOL SetDesignItemValue(WORD wKey,double fValue,char *sNotes=NULL,int iParamType=PARAM_POS,int iDatatype=0,BOOL bModify=FALSE);
	BOOL SetDesignItem(WORD wKey,DESIGN_PARAM& param,char *sNotes=NULL,int iParamType=PARAM_POS,BOOL bModify=FALSE);
	BOOL GetStdPartParamValue(WORD wKey, double *pfValue);	//得到标准构件相应设计参数的值
	//附加构件列表函数
	CHashListEx<ATTACHPART_ITEM> *GetAttachPartList(){return &AttachPart;}
	ATTACHPART_ITEM* GetFirstAttachPart(){return AttachPart.GetFirst();}
	ATTACHPART_ITEM* GetNextAttachPart(){return AttachPart.GetNext();}
	ATTACHPART_ITEM* AppendAttachPart(CLDSDbObject *pObj);
	BOOL DeleteAttachPartItem(long handle);
	int GetAttachPartCount(){return AttachPart.GetNodeNum();}
	void EmptyAttachPartList(){AttachPart.Empty();}
	BOOL IsInAttachPartList(long h);
	//得到十字插板上下两块立板 wht 10-07-06
	BOOL GetVertPlateInXEndPlate(CLDSPlate **pUpPlate,CLDSPlate **pDownPlate);
	BOOL DesignStdPartUcs();
	bool GetStdPartModel(CLDSStdPartModel *pStdPartModel);
	double GetThick();
	//克隆参数化钢板
	void CloneTo(CLDSParamPlate &plate);
	virtual bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	//根据RIBPLATE_ELBOW初始化肘板式肋板的设计参数
	void ShiftElbowAndRibPlateSchema(int iRibPlateSchema=TYPE_RIBPLATE);
	static const int TYPE_RIB_PLATE_ON_PLATE  = 1;		//钢板矩形肋板
	static const int TYPE_RIB_PLATE_ON_ANGLE  = 2;		//角钢肋板
	static const int TYPE_RIB_PLATE_ON_TUBE   = 3;		//钢管矩形肋板
	static const int TYPE_RIB_ELBOW_PLATE	  = 4;		//肘板型肋板
	static const int TYPE_RIB_CORNER_PLATE	  = 5;		//角肋板
	static const int TYPE_RIB_SLOT_PLATE	  = 6;		//槽肋板
	int GetRibPlateSubType();
	//
	static const int TYPE_COMMON_CIR_PLATE	=1;	//普通环向板
	static const int TYPE_3WL_CIR_PLATE		=2; //3WL环向板
	static const int TYPE_3WA_CIR_PLATE		=3;	//3WA环向板
	static const int TYPE_2WL_CIR_PLATE		=4;	//2WL环向板
	static const int TYPE_2WA_CIR_PLATE		=5;	//2WA环向板
	static const int TYPE_SPECIAL_CIR_PLATE =6;	//异型法兰
	static const int TYPE_CIR_AISLE_PLATE	=7;	//环向走道板
	int GetCircularPlateSubType();
	//属性管理
	DECLARE_PROP_FUNC(CLDSParamPlate);
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//检测属性值是否相等
	//
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//得到构件属性的默认只读状态
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//得到构件属性的默认只读状态
private:
	//static long m_idPropUdf,;						//用户当前可定义使用的属性Id
	static long m_idMinPropUdf;						//可使用的最小用户自定义属性Id
public:	//初始化参数化板实例的参数项属性
	static CHashPtrList<DYNA_PROPITEM_GROUP> hashParamItemPropGroup;
	static void InitObjParamItemsPropHashTable(long idInitial);
	static long GetPropID(const char* propStr,int iParamType);
	static KEY2C GetPropKey(long id,int iParamType);
	static long GetDbPropId(KEY2C paramKey,int iParamType);
public:
	//连接方向函数serial:0:默认非关联钢板的连接方向;1表示第一块关联钢板上的连接方向;2:表示第二块关联钢板上的连接方向
	CONNECT_DIRECTION* GetRelaConnInfo(int serial);
	//返回指定关联面的连接方向(焊缝走向)
	f3dPoint GetRelaConnVec(int serial,CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	//返回指定关联面的工作法向
	f3dPoint GetRelaConnPlaneNorm(int serial,CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	long GetRelaConnVecLinePart(int serial);
	//根据序列号删除关联方向 wht 11-05-18
	void DeleteRelaConnVec(int serial);
	//将制弯面上的绝对坐标（方向）-->展开至钢板相对（装配）坐标系;
	//bSpreadVertex=true表示展开顶点false表示展开矢量方向
	f3dPoint SpreadTransV(f3dPoint V,bool bSpreadVertex=true);
	f3dPoint HuoquFaceNorm(){return keyvecset.huoqufacenorm;}
	f3dPoint HuoquAxisVec(){return keyvecset.huoquaxis_vec;}
	f3dPoint HuoquDirection();
	//返回值表示是否存在内圆,如果存在r表示内圆半径,column_norm表示圆柱法线 wht 11-05-27
	BOOL GetInnerCirRadius(double &r,double* column_norm);
	//获取环向板顶点集合,主要在出图时使用 wht 11-05-30
	CIRCULAR_PLATE_VERTEXSET GetCirPlateVertexSet();
	//是否为法兰
	BOOL IsFL(){ return m_iParamType==TYPE_FL||m_iParamType==TYPE_FLD||m_iParamType==TYPE_FLP||m_iParamType==TYPE_FLG||m_iParamType==TYPE_FLR; }
	//是否为插板
	BOOL IsInsertPlate(){ return m_iParamType==TYPE_ROLLEND||m_iParamType==TYPE_UEND||m_iParamType==TYPE_XEND;}
	//输出最小包络矩形范围及面积
	virtual f2dRect GetMinWrapRect(UCS_STRU *Rectucs=NULL,double offset=0,BOOL bExportDeformedProfile=FALSE);
	/* 有特例化的函数在此声明实现
	virtual double CalAreaOfMinWrapRect(SCOPE_STRU *pScope=NULL);
	virtual double CalAreaOfNetWrap();
	virtual double CalPerimeter();		//计算钢板周长,mm
	*/
	virtual double CalWeldEdgeLength();	//计算钢板焊缝边长,mm
	virtual bool ShadowDesign();
	virtual int GetFaceN();						//钢板面数
	virtual f3dLine GetHuoquLine(int index);	//获取指火曲线(0.第1、2面的火曲线 1.第2、3面的获取线)，为板坐标系下的相对坐标
	virtual f3dPoint GetHuoquFaceNorm(int faceI);
	virtual double GetHuoQuAngle(int iFace,f3dLine *pHuoQuLine=NULL);
	//要求输入的顶点必须是此连接板上的顶点,否则
	//可能不进行转换或转换错误(由展开相对坐标-->实际相对坐标) wht 11-05-26
	//bCalVertex=true表示获取顶点false表示获取矢量方向
	virtual f3dPoint GetRealVertex(f3dPoint vertice,bool bCalVertex=true);
	virtual f3dPoint GetSpreadVertex(const double* V,int face_i=0,bool bSpreadVertex=true);
	//要求输入的螺栓必须是此连接板上的螺栓,否则
	//可能不进行转换或转换错误(由实际绝对坐标-->展开相对坐标)
	virtual BOOL SpreadLsPos(CLDSBolt *pLs,double* ls_pos);
	//iResidePlane=0,1,2,分别表示基准面、１火曲面、２火曲面
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* iResidePlane=NULL,int iIntersPlane=0);
	//cbFlag由FROM_MCS、FROM_ACS、IS_VECTOR与IS_VERTEX进行组合，默认为FROM_MCS|IS_VERTEX
	//iResidePlane取值:1、2、3表示待转换点或矢量所驻存的面号 wjh-2015.2.27
	//static const BYTE FROM_MCS =0x00;	//coord3d为模型坐标
	//static const BYTE FROM_ACS =0x01;	//coord3d为当前钢板下装配坐标(即相对坐标)
	//static const BYTE IS_VERTEX=0x00;	//coord3d为位置点, 执行位置坐标转换
	//static const BYTE IS_VECTOR=0x02;	//coord3d为方向量, 执行矢量坐标转换
	virtual f3dPoint SpreadToACS(const double* coord3d,int iResidePlane=1,BYTE cbFlag=0);
	virtual GEPOINT TransToActual(const double* coord3d,int iResidePlane=1,BYTE cbFlag=0);
	//转换为低版文件存储时，该参数化板是否需要降级为普通钢板 wjh-2015.2.16
	bool IsNeedConvertToPlate(int doc_type,int nVersion);
	//转换为普通钢板
	BOOL ConvertToPlate(CLDSPlate* pPlate);
	//根据设计参数预计算参数化板的坐标轴方向及原点位置(不影响钢板本身坐标系原值) wjh-2017.1.18
	BOOL PreCalPlateCS(double* axis_x=NULL,double* axis_y=NULL,double* axis_z=NULL,double* origin=NULL);
	//同步属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部位置对称无关的属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
private:
	bool MirDesignItemsFrom(CLDSParamPlate* pSrcParamPlate,MIRMSG mirmsg);
	BOOL DesignCircularPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	BOOL DesignTubeRibPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	BOOL DesignCircularRibPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	BOOL DesignCircularCoverPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	BOOL DesignWaterShieldedPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	BOOL DesignRibElbowPlate(CLDSLineTube* pDatumTube,CLDSGeneralPlate* pBtmPlate,CLDSParamPlate::RIBPLATE_ELBOW& elbow);	//肘板型肋板
	BOOL DesignRibCornerOrSlotPlate(CLDSGeneralPlate* pDatumPlate,CLDSParamPlate::RIBPLATE_CORNER& corner);	//普通角肋板
	BOOL DesignRibPlateOnPlate(CLDSGeneralPlate* pDatumPlate,CLDSParamPlate::RIBPLATE_PLATE& platerib);	//角钢肋板
	BOOL DesignRibPlateOnTube(CLDSLineTube* pDatumTube,CLDSNode* pDatumNode,CLDSParamPlate::RIBPLATE_TUBE& tuberib);	//角钢肋板
	BOOL DesignRibPlateOnAngle(CLDSLineAngle* pDatumAngle,CLDSParamPlate::RIBPLATE_ANGLE& anglerib);	//角钢肋板
	BOOL DesignTubeStayWirePlate(CLDSParamPlate::RIBPLATE_STAY_WIRE& stay_wire);	//钢管拉线板
	BOOL DesignRibPlate2(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);//肋板
	BOOL DesignRibPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//肋板
	BOOL DesignBasePlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);//设计底板
	BOOL DesignShoePlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);//设计靴板
	BOOL DesignShoePlate2(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);//设计靴板
	BOOL DesignPedalPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl = NULL);//设计脚踏板
	BOOL DesignStationConnPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL); //设计端撑端板
	BOOL DesignStationShoePlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//设计立柱夹板
	BOOL DesignStationCrossPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//设计端撑插板
	BOOL DesignStationLinkPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//设计端撑中间连接板
	BOOL DesignStationNodePlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//人字柱中间连接板	BOOL DesignWalkWayCirPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//走道的加劲环板
	BOOL DesignWalkWayCirPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//走道的加劲环板
};
class CLDSStdPartModel{ //: public CLDSPart{
	ATOM_LIST<f3dPoint> boltPosList;
	void CreateLsHole(fBody* pBody,f3dPolyFace *pTopFace,f3dPolyFace *pBottomFace,f3dPoint norm);
public:
	int m_iStdPartType;//标准构件类型
	union DES_PARAM{
		INSERT_PLATE_PARAM insert_plate_param;	//插板参数
		FL_PARAM fl_param;		//平焊法兰、对焊法兰参数
		FL_PARAM2 fl_param2;	//刚性法兰、柔性法兰参数
	}param;
public:
	double m_fRelateTubeThick;
	CLDSStdPartModel();
	~CLDSStdPartModel();
	char* GetPartNo();
	void LayoutBolt();
	BOOL CreateBoltInPlate(CLDSParamPlate *pParamPlate);
	BOOL Create3dSolidModel(fBody* pBody,BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36,BYTE cRollType=0xFF);
	f3dPoint* GetFirstBoltPos(){return boltPosList.GetFirst();}
	f3dPoint* GetNextBoltPos(){return boltPosList.GetNext();}
	int GetBoltCount(){return boltPosList.GetNodeNum();}
};
struct RAYNO_RECORD{long hPart;DWORD dwRayNo;double yCoord;};
// 板
class CLDSPlate : public CLDSGeneralPlate {
protected:
	long _face_N;						//面数
	double _fInnerRadius;				//内圆弧半径
	//旧0.表示普通板 1.交叉板 2.共用板 3.折叠板 4.单螺栓设计5.多孔螺栓垫板
	//新0.表示普通板 1.折叠板 2.环向板 3.钢管夹板 4.螺栓垫板 5.相对于基准构件位置不变的附加板 6.共用板
	/* 移至CLDSGeneralPlate中 wjh-2017.1.21
	union UNION_TYPE{
		UINT iLegacyStyle;		//由旧_jdb_style更名而来 wjh-2017.1.21
		struct{
			WORD wiLegacyStyle;	//对应TMA V5.1.5之前的_jdb_style
			WORD wiType;		//对应与CLDSParamPlate::m_iParamType整合后的钢板设计参数类型
		};
	}style;*/
	BYTE _cbAlignment;	//0.无对齐;1.十字交叉对齐;2.基准角钢上矩形化对齐;3.整板矩形化
protected://获取钢板的子分类类型
	virtual WORD InternalGetDesignType();		//该函数纯粹是因为解决不能直接get=派生虚函数导致 wjh-2017.1.20
	virtual void InternalSetDesignType(WORD wiDesignType);
public:	//公用属性
	PROPERTY(double,Thick); 
	VIRTUAL_SET(Thick);
	VIRTUAL_GET(Thick);
	PROPERTY(BYTE, cbAlignment); 
	virtual GET(cbAlignment){
		CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
		if(pMotherPlate!=NULL&&pMotherPlate!=this)
			return pMotherPlate->cbAlignment;
		else
			return _cbAlignment;
	}
	virtual SET(cbAlignment){
		if(value>=0)
		{
			CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
			_cbAlignment = ftoi(value);
			SetModified();
			if(pMotherObj!=NULL&&pMotherObj!=this)
				pMotherObj->cbAlignment=value;
		}
	}
	PROPERTY(long,face_N); 
	GET(face_N){
		CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
		if(pMotherObj!=NULL&&pMotherObj!=this)
			return pMotherObj->face_N;
		else
			return _face_N;
	}
	SET(face_N){
		if(value>=0)
		{
			CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
			_face_N = value;
			SetModified();
			if(pMotherObj!=NULL&&pMotherObj!=this)
				pMotherObj->face_N=value;
		}
	}
	PROPERTY(double,m_fInnerRadius); 
	GET(m_fInnerRadius){
		CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
		if(pMotherObj!=NULL&&pMotherObj!=this)
			return pMotherObj->m_fInnerRadius;
		else
			return _fInnerRadius;
	}
	SET(m_fInnerRadius){
		if(value>=0)
		{
			CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
			_fInnerRadius = value;
			SetModified();
			if(pMotherObj!=NULL&&pMotherObj!=this)
				pMotherObj->m_fInnerRadius=value;
		}
	}
	DECLARE_PROPERTY(long,jdb_style); 
public:
	//0.表示正交矩形统计;1.表示最小包容矩形统计;2.表示净面积统计
	static int m_iPlankWeightStatType;
	//m_siProfileAlgor为临时属性,当新算法经全面测试稳定无误后,将完全取代旧算法 wjh-2015.11.08
	static short m_siProfileAlgor;	//0.传统外形计算算法;1.新改进包络外形计算算法
	//连接板的设计参数
	UCS_STRU ucsView;					//此板设计时的显示视图坐标系，临时加载不必存于文件
	long neutrosphere;					//表示火曲时中性层位于几分之一的位置
	CDesignLjPara designInfo;
	double m_fNormOffset;				//装配时法向位移量，设此参数便于钢管上焊接板的位置设定
	f3dPoint m_tInnerOrigin;			//内圆圆心位置---相对坐标	wht 09-08-25
	f3dPoint m_tInnerColumnNorm;		//内圆法线方向
	BOOL m_bReplPart;					//代用构件
	BOOL m_bWeldPart;					//焊接件
	BOOL m_bNeedFillet;					//需要坡口
	int  m_iLsArrayStyle;				//钢板上螺栓的阵列方式	0-矩形阵列 非0-环形阵列	wht08-07-31	
	double m_fWeldWidth;				//焊缝间隙
	double m_fDimStartDist;				//起始方向标定间隙
	double m_fDimEndDist;				//终止方向标定间隙

	//多面板火曲面与基准面间的法线夹角
	f3dPoint huoqufacenorm[2];		//节点板各面的法线方向
	f3dLine HuoQuLine[2];	//火曲线（板相对坐标）
	bool m_bBendPlane2ResideBendPlane1;	//第二火曲面以第一火曲面为基准进行制弯 wjh-2017.12.1
protected:
	PROFILE_VERTEX _xPeakVertex;
	virtual PROFILE_VERTEX* InternalGet3PlanePlatePeakVertex(){return &_xPeakVertex;}
	//以下三个参数从钢板轮廓边中提取，仅用于属性显示时调整钢板外形，不存储 wht 18-12-17
	double _peakVertexOffsetY;		//制弯线共线三面板，火曲线交点Y坐标偏移量
	double _extendVertexOffsetLen;	//制弯线共线三面板，切割位置X长度方向沿神量
	double _peakPointMargin;		//两制弯点默认间隙制
public:
	double getPeakVertexOffsetY();
	void setPeakVertexOffsetY(double offset);
	__declspec(property(put=setPeakVertexOffsetY,get=getPeakVertexOffsetY)) double peakVertexOffsetY;
	double getExtendVertexOffsetLen();
	void setExtendVertexOffsetLen(double offset);
	__declspec(property(put=setExtendVertexOffsetLen,get=getExtendVertexOffsetLen)) double extendVertexOffsetLen;
	double getPeakPointMargin();
	void setPeakPointMargin(double margin);
	__declspec(property(put=setPeakPointMargin,get=getPeakPointMargin)) double peakPointMargin;
public:
	f3dPoint& getTopPoint(){return _xPeakVertex.vertex;}
	void setTopPoint(f3dPoint& vxTopPoint){_xPeakVertex.vertex=vxTopPoint;}
	__declspec(property(put=setTopPoint,get=getTopPoint)) f3dPoint& top_point;	//三面板两火曲线的交点（板相对坐标）
	//钢板为三面板时该变量有效	wht 10-01-12
	THANDLE complement_plate_handle;//用来记录三面板补焊钢板的句柄

	char org_material;
	//THANDLE base_node_handle;			//基准节点句柄
	//THANDLE base_part_handle;			//基准杆件句柄(只能而且必须有一根)
	long m_iLsRayNo;	//螺栓所在的射线号
	//如果要在钢板上环向布置肘板或螺栓，此变量表示基准线为焊道线还是为基准边  查看修改此变量只能在布置钢管肘板的对话框中进行
	BOOL m_bAttachPartByWeldRoad;	
	virtual bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
public:
	void DesignSetupUcs();
	virtual bool IsBelongTo(int iClassTypeId)const;
	virtual long get_BaseNode(){return designInfo.m_hBaseNode;}//返回连接板的依赖节点
	virtual long get_BaseRod() {return designInfo.m_hBasePart;}	//返回钢板基准节点  wht 09-12-12
	virtual CLDSPart* NewCopy(bool bByShadowMode=false);
	virtual CLDSGeneralPlate* CloneCopy(double offset,double* vUdfDirection);
	virtual CLDSGeneralPlate* ShadowCopy(double offset,double* vUdfDirection);
	virtual CLDSGeneralPlate* CloneCopy(GEPOINT vOffset,bool localAcsCoord=true);
	virtual CLDSGeneralPlate* ShadowCopy(double offset,bool localAcsCoord=true);
	virtual bool ShadowDesign();			//根据m_xDesOrgInfo、m_xMirAxisInfo及normOffset自动更新ucs
	virtual bool OptimizeStructure();
	void CopyProperty(CLDSDbObject *pObj);
	int GetEdgeFaceNo(int start_feature,int end_feature);
	//CDesignLjPara* DesignLjPara(){return pDesign;}
	CDesignLjPartPara* GetFirstLjPara(){return designInfo.partList.GetFirst();}
	CDesignLjPartPara* GetNextLjPara(){return designInfo.partList.GetNext();}
	CDesignLjPartPara* GetLjRodPara(long handle);
	BOOL UpdateLjRodEdgeSpace(long hPart,BYTE cbEdgeFlag=0x03);	//cbEdgeFlag,0x01:楞线侧；0x02:肢翼侧；0x04:端距
	BOOL UpdateLjRodEdgeSpace(CDesignLjPartPara* pLjPara, BYTE cbEdgeFlag = 0x03);
	BOOL DeleteLjPara(long handle);
	bool DesignTubeFoldPlate();
	bool DesignSubstationSplayFoldPlate();
	bool DesignPlate();
	BOOL DesignRhombusPlate();	//设计菱形交叉垫板
	void SortVertex(PROFILE_VERTEX *pPntArr, int nCount);
	//计算钢板外轮廓在指定投影线上的投影代数长度位置(含正负投影长度值)
	//perpline_pick:塔坐标系下的投影线基点
	//perpline_vec:塔坐标系下的投影线延伸方向
	VALUE_SCOPE WeldLinePerpScope(f3dPoint perpline_pick,f3dPoint perpline_vec);
	void AlignWeldLine();
	void DesignProfile(BOOL bSortingVertex=TRUE);
	void DesignProfile2();	//用包络多边形的理念重写的外形计算方法
	virtual bool IsConnectPlate();	//钢板是否为有杆件及螺栓连接的连接板
	virtual bool IsPeakPointPlate();//是否为带顶角的三面板　wjh-2016.1.9
	virtual bool IsFoldPlate();		//是否为三面折叠板　wjh-2016.1.9
	virtual bool IsHuoQuLineCollinear3FacePlate();	//是否为火曲线共线的三面板 wht 18-12-17
	bool IsPlate() const{return true;}
private:
	BOOL Create3dSolidRollEdge(fBody* pBody,f3dAtomLine *pAtomLine,PROFILE_VERTEX *pFeaVertex,int iFaceNo,COLORREF plank_color);
	void CalWeldLineVertex(CDesignLjPartPara *pLinePartPara,f3dPoint vertex_up,
							f3dPoint vertex_down,f3dPoint bolt_up,f3dPoint bolt_down,
							f3dPoint &weld_vertex_start,f3dPoint &weld_vertex_end,BOOL bFirstRayPart);
	BOOL CalProfileVertex(CDesignLjPartPara *pLinePartPara,BOOL bFirstRayPart=FALSE,f3dPoint *pWeldLineStart=NULL,f3dPoint *pWeldLineEnd=NULL);
	//重新设计板外形时只更新轮廓点坐标,保留焊接、卷边等特殊属性
	BOOL CalProfileVertexPos(CDesignLjPartPara *pLinePartPara,ARRAY_LIST<f3dPoint>& vertexArr,f3dPoint *pBlotUp=NULL,f3dPoint *pBoltDown=NULL);
	bool UpdateProfileVertexAndPos();
	void InitBoltPadLjPartPara(CDesignLjPartPara *pLjPartPara);
public:
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	int ValidCheck();
	static const BYTE VALIDATE_PROFILE_BASIC	 = 0x01;
	static const BYTE VALIDATE_PROFILE_EDGESPACE = 0x02;
	virtual BOOL ValidateProfile(double epsilon=1.0,EDGEBOLT *endBoltInfoArr=NULL,int *count=NULL,BYTE cbValidateFlag=0xff);
	BOOL CalWrapLsProfile(double angle_lim=60.0);
	BOOL AssertValid();
	CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
	bool IsEqual(CLDSPlate* pMirPlank,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	virtual int IsEqualProfile(CLDSGeneralPlate* pMirPlate,BOOL bCompareLs=TRUE,double epsilon=1.0);
	//指定基准索引比较钢板外形 
	int CompareProfile(CLDSPlate *pMirPlate,BOOL bReverseOrder,int base_i,BOOL bCompareLs,double epsilon,
					   ATOM_LIST<PROFILE_VERTEX> &src_vertex_list,ATOM_LIST<PROFILE_VERTEX> &mir_vertex_list);
	CLDSPlate(void);
	~CLDSPlate(void);
	BOOL CalStdProfile();
	/* 已作为虚函数移至CLDSGeneralPlate中
	//输出最小包络矩形范围及面积
	virtual f2dRect GetMinWrapRect(UCS_STRU *Rectucs=NULL,double offset=0,BOOL bExportDeformedProfile=FALSE);
	virtual double CalAreaOfMinWrapRect(SCOPE_STRU *pScope=NULL);
	virtual double CalAreaOfNetWrap();
	virtual double CalPerimeter();		//计算钢板周长,mm
	*/
	virtual double CalWeldEdgeLength();	//计算钢板焊缝边长,mm
	f2dRect GetWrapRect();
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	double GetWidth();
	double GetLength();
	double GetWeight(BOOL bSolidLength=TRUE);
	double GetThick();
	virtual CLDSNode *GetBaseNode(bool bClearLocks=true);	//返回钢板基准节点  wht 09-12-12
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	void DesignProtrusiveProfile();
	//iResidePlane=0,1,2,分别表示基准面、１火曲面、２火曲面
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* iResidePlane=NULL,int iIntersPlane=0);
	//要求输入的螺栓必须是此连接板上的螺栓,否则
	//可能不进行转换或转换错误(由实际绝对坐标-->展开相对坐标)
	virtual BOOL SpreadLsPos(CLDSBolt *pLs,double* ls_pos);
	BOOL SpreadLsPos(const double* ,double* ls_pos_coord);
	BOOL SpreadLsPos(CLsRef *pLsRef,double* ls_pos){return SpreadLsPos(pLsRef->GetLsPtr(),ls_pos);}
	f3dPoint RestoreLsPos(f3dPoint ls_pos,int iFace012=0);
	//计算指定火曲面的火曲角度，第一火曲面就输入1,第二火曲面输入2
	virtual double GetHuoQuAngle(int iFace,f3dLine *pHuoQuLine=NULL);
	virtual double GetNormOffset(){return m_fNormOffset;}//获取钢板底面相对于基准原点的法向偏移量
	//要求输入的顶点必须是此连接板上的顶点,否则
	//可能不进行转换或转换错误(由展开相对坐标-->实际相对坐标)
	virtual f3dPoint GetRealVertex(f3dPoint vertice,bool bCalVertex=true);
	//(由实际相对坐标-->展开相对坐标);face_i取值为1,2,3分别表示点所在面号;
	//bSpreadVertex=true表示展开顶点false表示展开矢量方向
	virtual f3dPoint GetSpreadVertex(const double* coord3d,int face_i=0,bool bSpreadVertex=true);
	//cbFlag由FROM_MCS、FROM_ACS、IS_VECTOR与IS_VERTEX进行组合，默认为FROM_MCS|IS_VERTEX
	//iResidePlane取值:1、2、3表示待转换点或矢量所驻存的面号 wjh-2015.2.27
	//static const BYTE FROM_MCS =0x00;	//coord3d为模型坐标
	//static const BYTE FROM_ACS =0x01;	//coord3d为当前钢板下装配坐标(即相对坐标)
	//static const BYTE IS_VERTEX=0x00;	//coord3d为位置点, 执行位置坐标转换
	//static const BYTE IS_VECTOR=0x02;	//coord3d为方向量, 执行矢量坐标转换
	virtual f3dPoint SpreadToACS(const double* coord3d,int iResidePlane=1,BYTE cbFlag=0);
	//static const BYTE TO_MCS =0x04;	//转换至模型坐标
	//static const BYTE TO_ACS =0x00;	//转换至为当前钢板下装配坐标(即相对坐标)
	virtual GEPOINT TransToActual(const double* coord3d,int iResidePlane=1,BYTE cbFlag=0);
	//将钢板信息复制到指定Buffer中
	BOOL CopyPlateToBuffer(CBuffer &buffer);
	//将缓存中的信息复制到钢板中
	BOOL PastePlateFromBuffer(CBuffer &buffer,CLDSNode *pDatumNode);
	//克隆钢板
	void CloneTo(CLDSPlate &plate);
	//判断是否为贯通钢管的钢板
	BOOL IsTransPlate();
	//得到钢板几何形心位置
	static f3dPoint GetGeomCenter(CLDSPart *pPlate);

	friend BOOL IsLsRefListEqual(CLsRefList *pLsRefList,const UCS_STRU &ucs,
					   CLsRefList *pViceLsRefList, const UCS_STRU &vice_ucs,
					   double epsilon,char *error_info,long *pH);
public:
	double GetOutterPlaneOffset(const f3dPoint &outter_vec);
	//此函数仅用于生成普通三面板剪切面补焊时的补料钢板的多边外形顶点
	bool GetComplementFaceVertex(f3dPoint &vertex1,f3dPoint &vertex2);
	//计算公切点，最开始由PartEditor.cpp中计算公切点源码移入 wjh-2013.6.6
	static bool UpdateVertexTangentPos(PROFILE_VERTEX* pPrev,PROFILE_VERTEX* pCurr,PROFILE_VERTEX* pNext);
	//设计三面板补焊钢板外形
	bool Design3FacePlateComplementPlate(CLDSPlate *pDatumPlate);
	double GetRollEdgeHeight();
	virtual BYTE FuncType();
	virtual double GetDeformCoef(int iFace=0);
	virtual double GetHuoQuR(int iFace=0);
	virtual void SetDeformCoef(double deform_coef=0.35,int iFace=0);
	virtual void SetHuoQuR(double r=0,int iFace=0);
	//管理钢板属性
	DECLARE_PROP_FUNC(CLDSPlate);
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//得到属性值
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//检测属性值是否相等
	//
	static int nPlankEdgeAngleThreshold;
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//得到构件属性的默认只读状态
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//得到构件属性的默认只读状态
	BOOL IsInsideAngle(CLDSLineAngle* pAngle, short* x_wing0_y_wing1);
	BOOL IsInsideAngle(CLDSLineAngle* pAngle, BYTE* x_wing0_y_wing1);
	virtual int GetFaceN(){ return _face_N;}	//钢板面数
	virtual f3dLine GetHuoquLine(int index);	//获取指火曲线(0.第1、2面的火曲线 1.第2、3面的获取线)，为板坐标系下的相对坐标
	virtual f3dPoint GetHuoquFaceNorm(int faceI);
	//同步属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部位置对称无关的属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
};
/* 连板模式复制相关代码已移至ClonePlate.h,ClonePlate.cpp wjh-1017.8.7
struct LINKPOLEINFO{
protected:
	UINT id;
public:
	CLDSLinePart *pLinePart;//记录连接杆件指针
	CLDSLinePart *pMatchLinePart;//记录匹配杆件指针
	long hLinePart;			//连接杆件句柄
	long hMatchPole;		//记录与当前杆件匹配的射线杆件句柄
	double offset_angle;	//记录射线杆件与基准杆件之间的夹角
	int vec_type;			//记录射线杆件延伸方向 1表示与基准角钢当前肢延伸方向相同 -1表示与基准角钢当前肢延伸方向相反
	void SetKey(DWORD key){id=key;}
	ADD_READONLY_PROPERTY(UINT,Id){return id;}
	LINKPOLEINFO()
	{
		pLinePart=NULL;
		pMatchLinePart=NULL;
		hLinePart=0;
		offset_angle=0;
		hMatchPole=0;
		vec_type=0;
	}
};
class CSchemaRod : public LINKPOLEINFO
{
public:
	struct DESIGNLJ_ANGLEPARA
	{
		BYTE ciWorkWingX0Y1;	//当前连接肢
		BYTE ciSonAngleQuad;	//基准角钢若为组合角钢且当前角钢为基准组合角钢的子角钢，所在X-Z平面内的象限号,取值:1-4，0表示其余角钢
		ANGLE_SPACE_FLAG cbSpaceFlag;		//
		BOOL bEndLjAngle;		//端连接角钢
		BOOL bDualEdge;			//是否螺栓线两侧都有外轮廓点
	}angle;
	//BOOL m_bAdjustLjEndPos;		//对称钢板时是否需要调整射线杆件连接端摆放位置，主要在对称共用板时使用 wht 10-02-26 
	long m_idClassType;
	//int m_iAffectTubeEndPlateStyle;
	long hOddRefRod1,hOddRefRod2;;	//正负头校验参照构件
	short siPlaneNo;				//当前杆件所在的连接面,0:基准平面;1.第一火曲面;2.第二火曲面
	short siLinkStart0End1;			//设计始端0，终端1;
	short siEndSpace,siWingSpace,siBerSpace;	//定义板外形时的参数
public:
	CSchemaRod();
	void CopyProperty(CSchemaRod* pScrSchemaRod);
};
//克隆钢板
class CClonePlate{
protected:
	//0.不匹配 
	//1.完全匹配(编号相同)  角度匹配 射线角钢规格匹配 主材规格匹配 射线角钢数量相同
	//2.射线杆件数量匹配规格匹配但角度不匹配  ==>取钢板基本信息,射线角钢螺栓信息基准角钢螺栓信息,自动计算正负头
	//3.仅射线杆件数量匹配
	int m_iMatchResult;	//匹配结果
	IModel *m_pTower;
	LINEPARTSET partset;	//节点当前面相关杆件集合
	CHashListEx<CSchemaRod> hashLinkRods;
	CHashSet<CSchemaRod*> hashLinkRodsByH;
	void EmptyLinkRods();
public:
	char ciDatumAxis;	//一般为'Y'
	char ciDirectionAxisY;
	char ciDirectionAxisZ;	//0表示当前工作肢正法线;1：表示当前工作肢负法线
	struct VECTORS{
		GEPOINT vDatumLenVec;	//基准杆件延伸方向（一般也为Y轴方向），
		GEPOINT vDatumPlaneNorm;//钢板基准平面的法线方向（一般也为基准角钢的工作肢法线方向）
		GEPOINT vBendPlaneNorm[2];
	}xLocalVec;	//xLocalVec为源模板局部坐标系下的矢量,xModelVec当前模型坐标系下的矢量
	VECTORS UpdateModelCSVectors(GECS acs);	//钢板局部装配坐标系
	int JustifySonAngleResideQuad(GECS acs,CLDSLineAngle* pAngle);
public:
	long m_hSrcPlate;		//记录源钢板句柄
	BYTE m_nPlaneN;			//钢板工作面数>1表示火曲板
	UINT m_idDatumRod;		//基准杆件的标识Id
	CLDSPlate *m_pSrcPlate;	//记录指向源钢板的指针
	CLDSNode *m_pDestNode;	//需要粘贴钢板的节点
	CClonePlate();
	~CClonePlate();
	CClonePlate(IModel *pTower,CLDSPlate *pPlate=NULL,CLDSNode *pNode=NULL);
	void SetSuperiorTower(IModel *pTower);
	CSchemaRod* AppendSchemaRod(long hLinkRod,DWORD keyid=0);
	bool RemoveSchemaRod(CSchemaRod* pSchemaRod);
	void AnalyzePlateInfo(CLDSPlate *pPlate,IModel *pTower=NULL);	//分析钢板信息
	int IntelligentMatchPlate(CLDSNode *pNode,PARTSET& DispPartSet,IModel *pTower=NULL);//智能识别当前节点上是否可生成相同编号的钢板
	CLDSPlate *CreatePlate(PARTSET& DispPartSet,IModel *pTower=NULL);	//根据匹配结果生成新钢板
	void ToClipperBoard(CBuffer* pClipperBoard,IModel *pTower=NULL);		//将钢板信息存储到剪贴板中
	void FromClipperBoard(CBuffer* pClipperBoard,IModel *pTower=NULL);		//从剪切板中读取钢板信息
	CLDSLinePart *FindMatchPoleBySrcHandle(long hLinePart);	//通过源杆件句柄查找与之相匹配的杆件
	CLDSLinePart *FindSrcPoleByMatchHandle(long hMatchPole);//通过匹配杆件句柄查找与之相匹配的源杆件
	//排序节点杆件集合 0.按射线杆件与基准杆件fabs(cosa)排序  1.按逆时针方向排序(需指定射线杆件所在钢板)
	void SortLinePartSet(CLDSLinePart *pDatumLinePart=NULL,int iSortType=0,CLDSPlate *pPlate=NULL);
	void CorrectPlateDesignPara(CLDSPlate *pPlate);	//根据射线杆件规格调整钢板射线杆件参数
	void CorrectPlateLsInfo(CLDSPlate *pPlate);		//根据杆件螺栓信息更新螺栓规格以及螺栓间距
	void UpdateRayLinePartOffset(CLDSPlate *pPlate,int nAutoAdjustTubePos=0);//更新钢板射线杆件信息
	void UpdateRayLinePartOddment(CLDSPlate *pPlate);//更新射线杆件正负头
};
*/
//#---------------------------------------------
//焊缝
class CLDSWeldLine : public CLDSPart{
public:
	CLDSWeldLine();
	f3dArcLine weldline;
	short weld_type;	//0:丁字焊;1:搭焊;2:对焊
	double weld_high,weld_wide;
	double thick;		//只对丁字焊和对焊有效，表示这两种焊接时焊接子板的厚度
	//总焊缝体积
	double Volume();
};
// 方钢
class CLDSCube : public CLDSLinePart {
public:
	double b;
	f3dPoint start_xyz,end_xyz;
	f3dPoint work_norm;			// 工作面法线方向
	CLDSCube(void);
};
//钢管接头
struct TUBEJOINT{
	int type;	//0:平面相贯线;1.圆柱面相贯线;2.法兰;3.U型插板;4.C型插板;5.十字插板;6.高颈法兰.
	f3dPoint cutPlanePos,cutPlaneNorm;
	double m_fAxisXRotAngle,m_fWorkPlaneRotAngle;
	THANDLE hLinkObj,hViceLinkObj;	//hViceLinkObj>0 单侧双管相贯 =0普通相贯 <0对接相贯
	BOOL bVertPlate;
	BOOL bEndPlate;
	int typeEndPlate;	//0:圆封头板  1:方封头板
	bool IsValidPlaneCut(){return type==0&&!cutPlaneNorm.IsZero();}
	TUBEJOINT(){hViceLinkObj=-1;}
};
class CTubeEndPosPara : public CRodPosPara{
public:
	BOOL IsInDatumJgBer();		//基点是否在基准角钢楞线上
	BOOL IsInDatumJgWingX();	//基点是否在基准角钢X肢上
	BOOL IsInDatumJgWingY();	//基点是否在基准角钢Y肢上
	CTubeEndPosPara(){memset(this,0,sizeof(CTubeEndPosPara));m_iPosParaType=1;}
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,double* pickpos_coord);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	//0:端点即节点;1:搭接在节点父钢管杆件上;
	//2:节点向指定基准线投影;3.杆件心线交点;4.指定坐标
	long endPosType;	
	// 0:向楞线投影      1:X肢心线投影       2:Y肢心线投影
	// 3:向楞线平推      4:X肢心线平推       5:Y肢心线平推
	BYTE datum_pos_style;		//基点定位方式,基准杆件为角钢时启用 wht 10-06-14
	double fEccentricDist;		//偏心距
	bool bUseFaceOffsetNorm;	//启用平推面法线
	f3dPoint face_offset_norm;	//平推面法线  wht 10-04-20 
	double fNormOffset;		//基准面法向偏移量
	double fTransverseOffset;//横向偏移量 主要用于调整槽钢、扁钢的位置 wht 09-10-17	
	double fLenVecOffsetDist;//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量 wht 11-04-23
	THANDLE hDatumPart;		//搭接位置基准杆件/杆件心线交点定位是的基准杆件
	THANDLE hDatumStartPart;//起始杆件句柄/杆件心线交点定位时的交叉杆件 wht 10-10-18
	THANDLE hDatumEndPart;	//终止杆件句柄
	BOOL bDatumStartStartEnd,bDatumEndStartEnd;	//指定基准角钢的基准端点0:始楞点;1:终楞点
	void FromBuffer(CBuffer &buffer, long version=0,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
};

// 钢管
class CLDSLineTube : public CLDSLinePart {
	f3dPoint workPlaneNorm;
	BOOL _bHasWeldRoad;		//钢管是否存在焊道线
	bool _bCutSlot;			//是否需要进行开槽处理
public:	//公用属性
	PROPERTY(BOOL,m_bHasWeldRoad);
	GET(m_bHasWeldRoad){
		CLDSLineTube* pMotherObj=(CLDSLineTube*)MotherObject();
		if(pMotherObj==this)
			return _bHasWeldRoad;
		else
			return pMotherObj->m_bHasWeldRoad;
	}
	SET(m_bHasWeldRoad){
		CLDSLineTube* pMotherObj=(CLDSLineTube*)MotherObject();
		_bHasWeldRoad=value;
		if(pMotherObj!=this)
			pMotherObj->m_bHasWeldRoad=value;
	}
	PROPERTY(bool,m_bCutSlot);
	GET(m_bCutSlot){
		CLDSLineTube* pMotherObj=(CLDSLineTube*)MotherObject();
		if(pMotherObj==this)
			return _bCutSlot;
		else
			return pMotherObj->m_bCutSlot;
	}
	SET(m_bCutSlot){
		CLDSLineTube* pMotherObj=(CLDSLineTube*)MotherObject();
		_bCutSlot=value;
		if(pMotherObj!=this)
			pMotherObj->m_bCutSlot=value;
	}
public:
	static const BYTE FUNC_COMMON		= 0;	//普通钢管
	static const BYTE FUNC_WIREBUSH		= 3;	//挂线板上的套管
	static BYTE ciFlTubeOddment;	//0.到钢管端面;1.到法兰端面
	//BYTE m_cFuncType;
	union{
		struct{	//普通钢管
			long hTransTube;		//贯穿此钢管的钢管
			long hWorkNormRefPart;	//计算工作面法线时的参照杆件，如果为0，则以始端节点或终端节点的父杆件为参照杆件
			union{
				double pickWorkPlane[3];	//工作平面参照点(m_iNormCalStyle=2)或近似法线方向nearWorkPlaneNorm(m_iNormCalStyle=1) wjh-2015.8.7
				COORD3D nearWorkPlaneNorm;
			};
		};
		struct BUSH{	//挂线孔套管
			short height;			//套管高度
			long hBushResidePlate;	//套管所在挂线钢板
			long hBushRelyBolt;		//套管所依赖的定位挂线孔虚拟螺栓
			long hUpBushRingPlate;	//套管顶部外缘附加衬套环板
			long hDownBushRingPlate;//套管底部外缘附加衬套环板
		}bush;
	};
	f3dPoint cutPlaneOrigin,cutPlaneNorm;		//允许有一个切割面定义实体
	UINT m_uSlices,m_uDisplaySlices;
	TUBEJOINT m_tJointStart,m_tJointEnd;	//始终端钢管接头
	CLDSPoint xBushLocation;	//套管定位点(位于挂线孔中心线与钢板底面交点上)
	CTubeEndPosPara desStartPos,desEndPos;
	f3dPoint nearWeldRoadVec;	//焊道线所在方向
	short  m_iWeldRoadStyle;	//只是临时用来控制焊道线的显示方式不用保存
	short  m_iNormCalStyle;	//基准面法线计算方式 0.直接指定 1.共面参照杆件 2.共面参照点
	static BOOL bDisplaySolidTransectFace;
public:
	static bool IsHasTubeRelaModifiedParts();
	static CLDSLineTube* AppendModifiedTube(CLDSLineTube* pModifiedTube);	//记录需要更新实体的钢管
	static CLDSLineTube* EnumFirstModifiedTube();	//遍历第一根发生变化后需要更新实体的钢管
	static CLDSLineTube* EnumNextModifiedTube();	//遍历下一根发生变化后需要更新实体的钢管
	virtual int GatherOddRelaParts(BYTE start1end2all3=3);
	virtual bool ShadowDesign();
	virtual void TransToCS(GECS& cs);
	virtual void Offset(const double* offset);
	void SetWorkPlaneNorm(f3dPoint work_norm){workPlaneNorm=work_norm;}
	double CalWeldRoadAngle();	//返回值为弧度
	void CalWeldRoadVec(double fAngle);//输入值为角度值
	virtual double GetArea();	//单位为mm2
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	//bSolidLength表示计算长度时按单线计算还是按实体计算,主要是力学分析时统计重量用
	virtual double GetWeight(BOOL bSolidLength=TRUE);	//单位为kg
	double GetEndActualOdd();	//考虑终端有相贯面的情况下实际最少的正负头
	double GetStartActualOdd();	//考虑始端有相贯面的情况下实际最少的正负头
	BOOL CreateRoundBar3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);//钢筋实体生成
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	//当前构件是否可以在指定绘图坐标系(负Z轴为视线方向)下遮掩住另一构件
	virtual bool IsCanHide(CLDSPart* pPart,const double* dcs_norm);
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* cReserved=NULL,int iIntersPlane=0);
	//获得钢管的外立面圆柱面的长度及特征多边形
	double GetCutCylinderFace(int slices,fPtList &vertex_list,f3dPoint &extend_vec);
	void CopyProperty(CLDSDbObject *pObj);
	void CalWorkPlaneNorm();
	virtual f3dPoint WorkPlaneNorm();
	f3dPoint GetPlateVertSpreadVec(CLDSGeneralPlate *pPlate,char ciPlane=0);			//获取钢板平面上垂直于钢管轴线的延展方向
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	//bReadForCheck表示是否读取为了进行模型比对检查(仅读取最少量校审相关信息)
	virtual void FromCdfBuffer(CBuffer &buffer,long version=0,bool bReadForCheck=false);
	virtual void ToCdfBuffer(CBuffer &buffer,long version=0);	
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//自协同中心通过管道读取数据
	virtual void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	//通过管道向协同中心写入数据
	virtual void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	CLDSLineTube(void);
	~CLDSLineTube();
	double GetDiameter(){return size_wide;}
	void CalStartPos(CLDSArcLift *pLift=NULL);
	void CalEndPos(CLDSArcLift *pLift=NULL);
	virtual void CalPosition(bool bIgnoreRepeatFlag=false);
	virtual double GetLength();	//单位为mm
	virtual bool IntelliRecogWeldLine();
	virtual bool IsWeldWith(double* lineStartCoord,double* lineEndCoord);
	virtual bool IsWeldWith(f3dArcLine& arcline);
	UCS_STRU BuildUCS();
	BOOL DesignStartJoint();
	BOOL DesignEndJoint();
	BOOL DesignStartJointUCS(CLDSParamPlate *pParamPlate=NULL);
	BOOL DesignEndJointUCS(CLDSParamPlate *pParamPlate=NULL);
	void GetProcessStr(char *notes, size_t max_len);
	virtual double startOdd(bool bToFL=false) const;
	virtual double endOdd(bool bToFL=false) const;
	virtual void CalStartOddment(char ciAutoCalMode = 0,int liInitOddment=0);	//0.普通碰撞规则算;1.强制性把指定负头情况按碰撞规则算;2.按允许切角时保证切角边距情况的碰撞规则算
	virtual void CalEndOddment(char ciAutoCalMode = 0,int liInitOddment=0);		//0.普通碰撞规则算;1.强制性把指定负头情况按碰撞规则算;2.按允许切角时保证切角边距情况的碰撞规则算
	//计算钢管正负头，但不修改钢管正负头，返回计算得到的正负头值	wht 10-04-07
	double CalOddment(BOOL bStart,CLDSParamPlate *pParamPlate,bool bReturnOddToFL=false);
	//double GetWidth(){return size_wide;}
	//double GetThick(){return size_thick;}
	int GetFootNailCount(); //返回钢管上脚钉的个数 wht 11-04-23
	bool IsEqual(CLDSLineTube *pMirTube,double epsilon=1.0,char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);
	CLDSParamPlate *GetParamPlate(int start0_end1=0);	//获取钢管端头参数化板 wht 10-11-30
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart);
#endif
	//属性管理
	DECLARE_PROP_FUNC(CLDSLineTube);
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);		//根据属性ID得到属性值
	static void InitLineTubePropHashtable();
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//得到构件属性的默认只读状态
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//得到构件属性的默认只读状态
	//同步属性
	static MEMORY_BLOCK MPCL;	//Mirror Property Control List
	static PROPLIST_TYPE propMirSync;
	static void InitPropMirBitsMap();
	virtual PROPLIST_TYPE* GetSyncPropList(){return &propMirSync;}
	virtual MEMORY_BLOCK GetDefaultMPCL(){return MPCL;}
	virtual bool IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr);
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr表示同步对称全部位置对称无关的属性
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
	BOOL SyncMirTransectFaceTo(CLDSLineTube* pDestMirTube,MIRMSG mirmsg);
};
#ifdef __GGG_
// 锥管
class CLDSConeTube : public CLDSLinePart {
	f3dPoint workPlaneNorm;
public:
	int faceN;             //面数
	int nNum;               //塔体数量
	double longth;          //塔高
	TUBEJOINT m_tJointStart,m_tJointEnd;	//始终端钢管接头
	CTubeEndPosPara desStartPos,desEndPos;
	short  m_iNormCalStyle;	//基准面法线计算方式 0.直接指定 1.使用参照杆件计算
	f3dPoint nearWorkPlaneNorm;	//nearWorkPlaneNorm是最终工作面法线的确定依据
	long hWorkNormRefPart;		//计算工作面法线时的参照杆件，如果为0，则以始端节点或终端节点的父杆件为参照杆件
	UINT m_uSlices,m_uDisplaySlices;
	f3dPoint basePoint; 
	BOOL m_bHasWeldRoad;
	f3dPoint cutPlaneOrigin,cutPlaneNorm;		//允许有一个切割面定义实体
	short  m_iWeldRoadStyle;	//只是临时用来控制焊道线的显示方式不用保存
	f3dPoint nearWeldRoadVec;	//焊道线所在方向
	CLDSConeTube(void);
	~CLDSConeTube();
public:
	void CalWeldRoadVec(double fAngle);//输入值为角度值
	double CalWeldRoadAngle();	//返回值为弧度
	double GetRadius(){return size_wide;}
	double GetThick(){return size_thick;}
	void SetWorkPlaneNorm(f3dPoint work_norm){workPlaneNorm=work_norm;}
	void CalWorkPlaneNorm();
	void CopyProperty(CLDSConeTube &conetube);
	f3dPoint WorkPlaneNorm(){return workPlaneNorm;}
	UCS_STRU BuildUCS();
	virtual BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=0);//最后参数做偏移变量使用
	
	//属性管理
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSConeTube,propHashtable)  //定义属性项更新函数
	int GetPropValueStr(long id,char* valueStr);		//根据属性ID得到属性值
	static void InitConeTubePropHashtable();
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	// TODO: add your methods here.
};
#endif
// 圆钢
class CLDSCylinder : public CLDSLinePart {
public:
	double d;			//圆钢直径
	CLDSCylinder(void);
	// TODO: add your methods here.
};

//<DEVELOP_PROCESS_MARK nameId="CLDSLineSlot">
// 槽钢
class CLDSLineSlot : public CLDSLinePart{
	f3dPoint workPlaneNorm;			// 工作面法线方向
public:
	char spec[MAX_CHAR_GUIGE_16+1];
	//double h,b,t;	//h--高 b--宽 t--厚 
	f3dPoint nearWorkPlaneNorm;	//nearWorkPlaneNorm是最终工作面法线的确定依据
	long hWorkNormRefPart;		//计算工作面法线时的参照杆件，如果为0，则以始端节点或终端节点的父杆件为参照杆件
	short m_iNormCalStyle;		//基准面法线计算方式 0.直接指定 1.使用参照杆件计算 2.法线沿水平面方向（用于脚钉座）
	CTubeEndPosPara desStartPos,desEndPos;
	//NAILBASE //依附与钢管上的槽钢定位参数 wht 13-10-17
	long hAttachTube;			//根据依附钢管计算槽钢法线偏移量，hAttachTube>0x20表示该槽钢为脚钉底座
	CLDSPoint desDatumPoint;	//根据desDatumPoint计算槽钢中心位置
	double fLen;				//根据输入长度及基准点位置计算槽钢始终端位置

	CLDSLineSlot();
	~CLDSLineSlot();
	double GetHeight(){return size_height;}
	//double GetWidth(){return size.wide;}
	//double GetThick(){return size.thick;}
	void CopyProperty(CLDSDbObject* pObj);
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	virtual BYTE FuncType();
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//自协同中心通过管道读取数据
	virtual void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	//通过管道向协同中心写入数据
	virtual void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	int GetSpecification(char *slot_spec,BOOL bIncMat=TRUE);
	//bSolidLength表示计算长度时按单线计算还是按实体计算,主要是力学分析时统计重量用
	virtual double GetWeight(BOOL bSolidLength=TRUE);	//单位为kg
	BOOL CalTubeSlotPos(CLDSArcLift *pLift=NULL);
	void CalStartPos(CLDSArcLift *pLift=NULL);
	void CalEndPos(CLDSArcLift *pLift=NULL);
	virtual CRodPosPara* DesStartPos(){return &desStartPos;}
	virtual CRodPosPara* DesEndPos(){return &desStartPos;}
	virtual void CalPosition(bool bIgnoreRepeatFlag=false);
	UCS_STRU BuildUCS();
	void CalWorkPlaneNorm();
	virtual f3dPoint WorkPlaneNorm(){return workPlaneNorm;}
	void SetWorkPlaneNorm(f3dPoint workNorm){workPlaneNorm=workNorm;}
	bool IsEqual(CLDSLineSlot* pMirSlot,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	int IsBaseSlot();	//是否为槽钢底座，1:表示脚钉底座，其它表示其它普通挂接底座
	//获取螺栓穿过槽钢外皮的交点位置
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* btm0_left1_right2=NULL,int iIntersPlane=0);
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart);
#endif
	//属性
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSLineSlot,propHashtable) //定义属性项更新函数
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	static void InitLineSlotPropHashtable();
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//得到构件属性的默认只读状态
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//得到构件属性的默认只读状态
};
//</DEVELOP_PROCESS_MARK>

// 扁钢
class CLDSLineFlat : public CLDSLinePart{
	f3dPoint workPlaneNorm;			// 工作面法线方向
public:
	f3dPoint nearWorkPlaneNorm;	//nearWorkPlaneNorm是最终工作面法线的确定依据
	long hWorkNormRefPart;		//计算工作面法线时的参照杆件，如果为0，则以始端节点或终端节点的父杆件为参照杆件
	short m_iNormCalStyle;		//基准面法线计算方式 0.直接指定 1.使用参照杆件计算
	CTubeEndPosPara desStartPos,desEndPos;
	CLDSLineFlat();
	~CLDSLineFlat();
	void SetWorkPlaneNorm(f3dPoint work_norm){workPlaneNorm=work_norm;}
	void CopyProperty(CLDSDbObject* pObj);
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//自协同中心通过管道读取数据
	virtual void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	//通过管道向协同中心写入数据
	virtual void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	//bSolidLength表示计算长度时按单线计算还是按实体计算,主要是力学分析时统计重量用
	virtual double GetWeight(BOOL bSolidLength=TRUE);	//单位为kg
	void CalStartPos(CLDSArcLift *pLift=NULL);
	void CalEndPos(CLDSArcLift *pLift=NULL);
	virtual CRodPosPara* DesStartPos(){return &desStartPos;}
	virtual CRodPosPara* DesEndPos(){return &desStartPos;}
	virtual void CalPosition(bool bIgnoreRepeatFlag=false);
	//double GetWidth(){return size.wide;}
	//double GetThick(){return size.thick;}
	UCS_STRU BuildUCS();
	void CalWorkPlaneNorm();
	virtual f3dPoint WorkPlaneNorm(){return workPlaneNorm;}
	bool IsEqual(CLDSLineFlat* pMirFlat,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	//属性管理
	static int m_iCurDisplayPropGroup;					//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;		//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;		//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSLineFlat,propHashtable) //定义属性项更新函数
	static void InitLineFlatPropHashtable();
	static int GetPropStr(long id,char* propStr);		//得到属性字符串
	static long GetPropID(char* propStr);				//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//得到构件属性的默认只读状态
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//得到构件属性的默认只读状态
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
};
#ifdef __H_SHAPE_STEEL_
// H型钢
class CLDSLineHShape : public CLDSLinePart{
	f3dPoint workPlaneNorm;			// 工作面法线方向
public:
	BYTE m_ibHShapeType;		//0.HW(宽翼缘) 1.HM(中翼缘) 2.HN(窄翼缘)  3.HP(等翼缘)
	char spec[MAX_CHAR_GUIGE_16+1];
	f3dPoint nearWorkPlaneNorm;	//nearWorkPlaneNorm是最终工作面法线的确定依据
	long hWorkNormRefPart;		//计算工作面法线时的参照杆件，如果为0，则以始端节点或终端节点的父杆件为参照杆件
	short m_iNormCalStyle;		//基准面法线计算方式 0.直接指定 1.使用参照杆件计算
	CTubeEndPosPara desStartPos,desEndPos;
public:
	CLDSLineHShape();
	~CLDSLineHShape();
	//数据存储
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);//自协同中心通过管道读取数据
	void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);	//通过管道向协同中心写入数据
	//bool bAddSubLevelRelaObj=false:表示不添加子级关联对象;true:表示添加子级关联对象 wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//实体+定位操作
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	//bSolidLength表示计算长度时按单线计算还是按实体计算,主要是力学分析时统计重量用
	double GetWeight(BOOL bSolidLength=TRUE);	//单位为kg
	void CalStartPos(CLDSArcLift *pLift=NULL);
	void CalEndPos(CLDSArcLift *pLift=NULL);
	void CalPosition(bool bIgnoreRepeatFlag=false);
	void SetWorkPlaneNorm(f3dPoint work_norm){workPlaneNorm=work_norm;}
	void CopyProperty(CLDSDbObject* pObj);
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	UCS_STRU BuildUCS();
	void CalWorkPlaneNorm();
	virtual f3dPoint WorkPlaneNorm(){return workPlaneNorm;}
	bool IsEqual(CLDSLineHShape* pMirHShape,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);
	//属性管理
	static int m_iCurDisplayPropGroup;					//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;		//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;		//属性状态哈希表
	UPDATE_PROPLISTITEM_FUNC(CLDSLineHShape,propHashtable) //定义属性项更新函数
	static void InitLineHShapePropHashtable();
	static long GetPropID(char* propStr);				//得到属性ID
	static PROPLIST_ITEM* GetPropItem(long id);
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//得到构件属性的默认只读状态
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//得到构件属性的默认只读状态
	int GetPropValueStr(long id,char* valueStr);		//根据属性ID得到属性值
};
#endif
class CLDSExternalPart : public CLDSPart
{
public:
	CLDSExternalPart();
	//double volume;
	//fPtList Vertexs;								//存储顶点
	//fPtList VertexNormals;							//
	//ATOM_LIST< ATOM_LIST<int> > FaceVertexIndexs;	//存储每个面的索引值
	virtual BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
};
//绝缘子
class CLDSInsulator : public CLDSExternalPart
{
public:
	CLDSInsulator(void);
	CXhChar50 sName;	//名称
	f3dPoint pos;		//位置
};
//金具
class CLDSFitting : public CLDSExternalPart
{
public:
	static const BYTE TYPE_EB = 0;	//EB挂接金具
	static const BYTE TYPE_GD = 1;	//GD挂接金具
	static const BYTE TYPE_UR = 2;	//U型挂接环(Ring)
	static const BYTE TYPE_US = 3;	//U型螺丝
	static const BYTE TYPE_UB = 4;	//U型挂板
	BYTE m_ciFittingType;	//0.EB金具;1.GD金具;2.U型挂接环;3.U型螺栓
	//直接与塔身连接的一级挂接金具（EB挂接金具或GD挂接金具）
	short m_iLiveSwingAngle;//一级挂接金具当前摆动角值,°
	short m_siMinSwingAngle,m_siMaxSwingAngle;	//金具摆动角范围
	//与一级挂接金具连接横向摆动U型挂环
	short m_iURLiveSwingAngle;//U型连接挂环的实时摆动角
	short m_siMinURSwingAngle,m_siMaxURSwingAngle;	//U型环金具摆动角范围
	CXhChar50 sizespec;	//型号
	CXhChar50 sizespecUR;//二级连接U型环的型号
	struct HOLE{
		long hBolt,hResidePart;
		HOLE(){hBolt=hResidePart=0;}
	}holes[2];	//实体挂孔，一般由1~2个孔组成
	CHashListEx<CLDSGeneralPlate::DESIGN_PARAM>params;			
	struct BASICINFO {
		char   szSizeCode[16];	//型号代码
		short  siBoardThick;	//挂板厚度b
		double dfBoardL;		//金具(挂板部分)宽度
		//short  siThroughThick;//预留挂线角钢肢(通)厚
		double dfTrunnionD;		//耳轴直径
		double dfTrunnionL;		//耳轴总长
		double dfHoleD;			//挂孔直径(diameter)Ф
		double dfHole2TrunnionH;//挂孔中心到耳轴中心线距离H
		BASICINFO();
	}basic;
public:
	CLDSFitting();
	CLDSFitting(char ciFittingType,const char* szFittingSizeCode);
	~CLDSFitting();
	//
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual bool IsExternalPart() const{return true;}
	virtual BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	virtual CSolidPart GetSolidPartObject();
	void DesignFitting();
	void CopyFrom(CLDSFitting* pSrcFitting);
	bool MatchUring(DWORD dwReservedMatchParam=0);	//根据一级挂接金具中的挂孔直径匹配合适的U形环型号
	//根据一级挂接金具中的挂孔直径匹配合适的U形环型号
	static CXhChar50 MatchUringSizeCode(char ciFittingType,const char* szFittingSizeCode);
	static BASICINFO LoadFittingBasicInfo(char ciFittingType,const char* szFittingSizeCode);
	//属性栏显示
	DECLARE_PROP_FUNC(CLDSFitting);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值	
};

//剖面视图
class CPartGroupLocatePoint
{
	//基准构件为杆件时启用 标定点始/终端偏移量 主要用于钢管端连接法兰的情况，暂时好像无用
	double m_fStartOffset,m_fEndOffset;	
public:
	//标定点相对于剖视图中基准杆件绘图坐标原点的轴向位置
	double m_fLenPosToDatumPart,m_fVertPosToDatumPart;
	//标定点在剖视图绘图坐标系中的坐标位置
	f3dPoint m_xLocalCoordPos;
	long m_hPart;		//基准构件主要用于标注局部间隙尺寸组
	int m_iOrderType;	//标定点级别:0端面定位点±1构件定位尺寸±2局部间隙尺寸3脚钉尺寸
	bool m_bDimSpace;	//是否为绝对尺寸标定点
	IModel *pTower;	//临时变量不存储仅用于排序定位点
	UCS_STRU dcs;	//临时变量不存储仅用于排序定位点
public:
	CPartGroupLocatePoint();
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
};
class CCheckLineDimension
{
public:
	bool m_bSpecStartLocatePart,m_bSpecEndLocatePart;	//是否指定定位构件 
	long m_hStartPart,m_hEndPart;						//定位构件 
	//起始坐标 终止坐标 指定定位构件:相对坐标->绝对坐标->绘图坐标  未指定定位构件:绘图坐标
	f3dPoint m_xStartLocalCoordPos,m_xEndLocalCoordPos;	
	bool m_bBeforeWeld;	//焊前检测线/焊接后检测线
	CCheckLineDimension();
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
};

class CAngularDimension
{
public:
	bool m_bStartLineIsDatumLine;	//TRUE 以基准线为起始边   FALSE 以焊道线为起始边
	long m_hStartPart,m_hEndPart;	//要标住夹角的两个构件
	double m_fDimR;					//标注线半径
	CAngularDimension();
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
};
//绘图辅助	从DefineView.h移植
class CDrawingAssistant	
{ 
	//以下三个变量绘图时临时使用不用存储 wht 11-08-08
	struct ROTAXISLINE{
		GEPOINT startPt,endPt;
	}m_xRotateAxisLine;	//旋转轴
protected:
	double m_fRotateAngle;		//旋转角度
	GEPOINT m_xDrawNorm;		//绘图面法线,用来判断是否需要重新计算旋转轴及旋转角度
	union {
		UINT _uiType;
		struct{
			WORD _wStyle;	//相当于之前的m_nStyle;
			WORD _wFlag;	//针对当前的Style类型的参数配置,如wStyle=0&&wFlag&0x0001时表示m_xLineEnd优先于m_xFaceNorm;
		};
	};
	union {
		UINT uiSpreadPlane;	//0.表示视图基面,其余>0值表示该视图内相应的展开面;;-1表示展开至主管下面,-2表示展至主管上面,-3表示展开至主管前面,-4表示展开至主管背面 wjh-2017.11.25
		struct{
			WORD _wiResidePlane;	//展开至平面的标识Id
			BYTE _ciSpreadDirection;//展开后的目标方位1.主管上侧2.主管下侧3.主管前面4.主管背面
			bool _biSpreadOverRidge;//旋转展开时，跨越了钢管中轴线
		};
	};
public:
	CDrawingAssistant();
	//
	CLDSPoint m_xLineStart,m_xLineEnd;
	CLDSVector m_xFaceNorm;
	/*union{
		//辅助心线或火曲线
		struct ASSIST_LINE{POINT_PARA xLineStart,xLineEnd;}line;
		//绕绘图平面内轴线（制弯线）旋转展开
		struct ASSIST_PLANE{
			VECTOR_PARA xPlaneNormPara;
			POINT_PARA xLineStart;	//CLDSPoint m_xLineStart,m_xLineEnd;
			POINT_PARA xLineEnd;	//CLDSVector m_xFaceNorm;
		}plane;
		//绕任意轴旋转展开
		struct ASSIST_ROTPLANE{
			VECTOR_PARA xRotAxisPara;
			POINT_PARA xLineStart;
			double fRotAngle;	//弧度制
		}rotaion;
	};*/
	UINT iNo;
	static const WORD SPREAD_TO_UPPER	= 1;
	static const WORD SPREAD_TO_UNDER	= 2;
	static const WORD SPREAD_TO_TOP		= 3;
	static const WORD SPREAD_TO_BACK	= 4;
	DECLARE_PROPERTY(bool,bSpreadOverRidge);	//旋转展开时，跨越了钢管中轴线
	DECLARE_PROPERTY(BYTE,ciSpreadDirection);	//展开后的目标方位1.主管上侧2.主管下侧3.主管前面4.主管背面
	DECLARE_PROPERTY(long,iResidePlaneNo);		//0.表示视图基面,其余>0值表示该视图内相应的展开面;;-1表示展开至主管下面,-2表示展至主管上面,-3表示展开至主管前面,-4表示展开至主管背面 wjh-2017.11.25
	SEGI m_iSeg;
	char name[10];	//名称
	void SetKey(DWORD key){iNo=key;}
	DECLARE_PROPERTY(UINT,m_nStyle);	//0.旋转展开面 1.辅助心线 2.辅助火曲线 3.任意轴（xLineStart->xLineEnd)旋转展开
	static const WORD ENABLE_LINE_END = 0x0001;		//m_xLineEnd优先于m_xFaceNorm;
	static const WORD SPREAD_TO_DMPLANE = 0x0002;	//无论近似共面还是垂直，均强制转换到基准平面
	//针对当前的Style类型的参数配置,如wStyle=0&&wFlag&0x0001时表示m_xLineEnd优先于m_xFaceNorm;
	//wStyle=0&&wFlag&0x0002时表示无论是近似共面还是近似垂直，均强制旋转展开到当前基准平面，否则视情况展开到基准平面或垂直于基准平面　wjh-2017.11.8
	DECLARE_PROPERTY(WORD,wStyleFlag);
	DECLARE_PROPERTY(double,m_fRotRadAng);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void GetRotateAxisAndAngle(f3dPoint draw_norm,f3dLine &rotate_axis,double &rotate_angle,IModel *pTower=NULL);
};
//视图:正视图			==>{主视图,普通正视图}		   
//	   端视图==>节间视图==>{节点板视图(无环向板)，环向肋板视图，环向封板视图，跨越连板视图}
//			    管端视图==>{法兰视图，环向板视图，插板视图}	
//				脚钉视图==>{角钢脚钉，槽钢脚钉}	
//						==>{TMA塔脚板/牛蹄板}			
//组焊件视图类型：1.主视图，2.正视图，3.节点板端视图，4.环向肋板视图，5.环向封板视图，
//				  6.跨越连板视图,7.法兰视图，8.环向板视图，9.插板视图,10.角钢脚钉,
//				  11.槽钢脚钉板, 12.塔脚板,13.支管端面图,14.对接钢管简图,15.K节点相贯简图,100.用户定义视图;
enum SECTION_VIEW_TYPE{VIEW_MAIN=1,VIEW_FRONT=2,VIEW_NODEPLATE=3,VIEW_CIRCOVERPLATE=4,VIEW_CIRRIBPLATE=5,
					VIEW_TUBERIBPLATE=6,VIEW_FL=7,VIEW_CIRPLATE=8,VIEW_INSERTPLATE=9,VIEW_FOOTNAILANGLE=10,
					VIEW_FOOTNAILSLOT=11,VIEW_FOOT=12,VIEW_JOINSECTION=13,VIEW_JOINTTUBE=14,VIEW_KNODESKETCH=15,VIEW_USERDEF=100};
#include "LifeObj.h"
struct DRAWFLAG{
	BYTE cDrawFlag;	//构件绘制模式，水平'H'，垂直'V'
public:
	static const BYTE AUTO_JUSTIFY			= 0;	//完全自动判断（组焊件子视图中也可能过滤掉不绘制）
	static const BYTE AUTO_DRAW				= 1;	//绘制方式自动判断（组焊件子视图中一定绘制）
	static const BYTE VIRTUAL_DRAW			= 0x80;	//虚线绘制
	static const BYTE ANGLE_WING_X_INWARD	= 'X';	//X肢里铁绘制
	static const BYTE ANGLE_WING_X_OUTWARD	= 'x';	//X肢外铁绘制
	static const BYTE ANGLE_WING_Y_INWARD	= 'Y';	//Y肢里铁绘制
	static const BYTE ANGLE_WING_Y_OUTWARD	= 'y';	//Y肢外铁绘制
	static const BYTE ANGLE_SECT_Z			= 'Z';	//截面图绘制
	static const BYTE PLATE_WORKPLANE		= 'X';	//基准工作平面平铺(火曲面投影)绘制绘制
	static const BYTE PLATE_SPREADPLANE		= 'S';	//将多面板的火曲面展开绘制
	static const BYTE PLATE_SECT_Z			= 'Z';	//竖直截面图绘制
	static const BYTE PART_HORIZON			= 'H';	//构件平铺图绘制
	static const BYTE PART_VERTICAL			= 'V';	//构件直立(截面)图绘制
public:
	DRAWFLAG(BYTE cPreferDrawFlag=0){cDrawFlag=cPreferDrawFlag;}
	//返回值：0.平铺绘制 1.侧向绘制 2.绘制端面,
	//返回参数：cActualDrawFlag，实际执行的绘制模式
	int GetPartDrawType(CLDSPart* pPart,double* drawPlaneNorm,BYTE* cExecuteDrawFlag=NULL);
};

class CDrawCoordTrans
{
	UCS_STRU _cs;
public:
	double m_fDrawScale;			//绘图比例尺,即单位绘图长度所代表的实际长度,如1:20时m_fDrawScale=20.0
	double m_fSpreadAngle;			//展开角度，弧度制
	f3dPoint m_xSpreadPlaneNorm;	//展开面法线
	f3dLine m_xSpreadLine;			//展开旋转轴线
	struct SPREAD_PLANE{
		double fRotateAngle;	//展开时旋转角度, 弧度制
		GEPOINT vNormal;
		GEPOINT xRotAxisStart,xRotAxisEnd;
	}second;
public:
	CDrawCoordTrans();
	CDrawCoordTrans(UCS_STRU& dcs,double m_fDrawScale=1.0);
	//UCS_STRU CS(){return cs;}
	f3dPoint PrjNorm() const;
	f3dPoint PrjOrigin() const;
	//设定当前绘图转换坐标系
	bool SetCS(UCS_STRU& cs);
	//返回当前绘图转换坐标系
	operator UCS_STRU() const{return _cs;};
	//srcPos为展开前塔坐标系下的坐标,返回绘图坐标(即ACAD下的坐标)
	f3dPoint TransToDraw(f3dPoint srcPos,bool clearCoordZ=true);
	//srcVec为展开前塔坐标系下的矢量,返回绘图矢量(即ACAD下的坐标)
	f3dPoint TransVToDraw(f3dPoint srcVec);
	//drwPos为绘图坐标系下的坐标,返回塔模型坐标系下坐标（用途不大暂不实现）
	//f3dPoint TransFromDraw(const double* drwPos);
	//drwVec为绘图坐标系下的矢量,返回塔模型坐标系下矢量
	f3dPoint TransVFromDraw(const double* drwVec);
public:	//属性
	READONLY_PROPERTY(UCS_STRU,cs);
	GET(cs){return _cs;}
};
class CSlaveSectionView;
class CSectionView
{
	friend class CLDSPartGroup;
protected:
	int m_id;
	int _idViewType;	//用以标识组焊子视图的类别，将来会部分或全部代替m_iViewType，
	SECTION_VIEW_TYPE _iLegacyViewType;
	CLDSPartGroup *m_pPartGroup;	//视图从属的组焊件
	struct XHSTR_LEN2 {static const int BUF_LEN =  3;};
public:
	static const int CLS_BASIC_VIEW = 0;
	static const int CLS_SLAVE_VIEW = 1;
	static const int CLS_STDX_PLATES_VIEW	= 2;//十字交叉连板组焊图的正侧俯视图（贯通板展开图、立板展开图及俯视剖面图）
	static const int CLS_ANGLE_FOOT_VIEW	= 3;//角钢塔脚板视图
	static const int CLS_KAOME_ANGLE_VIEW	= 4;//靠模角钢视图
	static const int CLS_ANGLE_SPREAD_VIEW	= 5;//角钢展开视图(以角钢工艺卡模式绘制)
	static const int CLS_JOINT_TUBES_VIEW	= 6;//角钢展开视图
	static const int CLS_SLAVE_NODESECT_VIEW = 7;
	static const int CLS_HORIZ_BEAM_VIEW	= 8;//三角构架梁底面收口水平钢管梁组焊图
	static const int CLS_DUALTUBE_JOINT_VIEW= 9;//双钢管对接相贯其余为连接钢板组焊主视图
	static const int CLS_BUSH_PLATE_VIEW	= 10;	//钢管衬托板视图	wht 18-03-10
public:
	UCS_STRU ucs;
	CParaCS des_cs;
	char sName[MAX_TEMP_CHAR_50+1];
	int index;		//视图索引  
	BOOL m_bLocked;	//锁定视图  TRUE：更新组焊件时不更新视图 FALSE：更新组焊件时更新视图 wht 09-06-23
	int feature;	//定义用户自己的信息(临时备用,不可持久存储，也不一定都用)
	struct PART_ITEM{
		union {
			UINT uiSpreadPlane;	//所在绘图平面序列号:>0表示位于指定平面;-1表示展开至主管下面,-2表示展至主管上面,-3表示展开至主管前面,-4表示展开至主管背面 wjh-2017.11.25
			struct{
				WORD _wiResidePlane;	//展开至平面的标识Id
				BYTE _ciSpreadDirection;//展开后的目标方位1.主管上侧2.主管下侧3.主管前面4.主管背面
				bool _biSpreadOverRidge;//旋转展开时，跨越了钢管中轴线
			};
		};
		static const WORD SPREAD_TO_UPPER	= 1;
		static const WORD SPREAD_TO_UNDER	= 2;
		static const WORD SPREAD_TO_TOP		= 3;
		static const WORD SPREAD_TO_BACK	= 4;
		DECLARE_PROPERTY(bool,bSpreadOverRidge);	//旋转展开时，跨越了钢管中轴线
		DECLARE_PROPERTY(BYTE,ciSpreadDirection);	//展开后的目标方位1.主管上侧2.主管下侧3.主管前面4.主管背面
		DECLARE_PROPERTY(long,iResidePlaneNo);		//0.表示视图基面,其余>0值表示该视图内相应的展开面;;-1表示展开至主管下面,-2表示展至主管上面,-3表示展开至主管前面,-4表示展开至主管背面 wjh-2017.11.25
		long hSonPart;
		char cFlag;		//绘制模式：0 自动判断  'X':角钢X肢/钢管正视/板截面  'Y'角钢Y肢  'Z'角钢截面/钢管截面/板正视 'S'近似共面
		ITAG *pTagInfo; //临时使用不存储，可省去每次遍历构件链表时重复查找当前构件指针 wht 09-12-12
		void SetTagInfo(ITAG* pTag){pTagInfo=pTag;}
		void SetKey(DWORD key){hSonPart=key;}
		PART_ITEM() {pTagInfo=NULL;uiSpreadPlane=hSonPart=cFlag=0;}
		~PART_ITEM(){
			if(pTagInfo) pTagInfo->DetachObject();
		}
	};
	struct ITEM_TAG_INFO : public TAG_INFO<PART_ITEM>{
	public:
		DRAWFLAG cDrawMode;
		CLDSPart *pCurPart;
	public:
		ITEM_TAG_INFO();
		virtual ~ITEM_TAG_INFO(){DetachObject();}
	};
	struct RELA_RIBPLATE{
		CLDSParamPlate* pPlate;
		long hDatumPlate;
	};
	CHashListEx<PART_ITEM> SonParts;	//键值PART_ITEM::hSonPart
	CHashList<CDrawingAssistant> hashAssistPlanes;	 //展开面
	//当PART_ITEM已经有挂接对象时执行TagInfo(...), 否则创建新挂接数据对象后返回
	ITEM_TAG_INFO* LoadTagInfo(PART_ITEM* pItem);
	void SetBelongPartGroup(CLDSPartGroup *pPartGroup) {m_pPartGroup=pPartGroup;}
	CLDSPartGroup* BelongPartGroup() { return m_pPartGroup; }
public:	//模型坐标与绘图坐标间的变换
	struct ROTATION{
		double  m_fRotAngle;	//由实际坐标向绘图坐标变换过程中的旋转角度，弧度制
		GEPOINT xRotAxisStart,xRotAxisEnd;	//展开旋转轴线
		ROTATION(){m_fRotAngle=0;}
	};
	static GECS TransRotationsToCS(IXhEnumerator *pIter,bool bRotRightHand=true,bool bFowardIter=true);
	bool GetSpreadPlaneRotCS(int iAssistPlaneNo,GECS* pRotCS,IModel *pTower=NULL);
	GEPOINT TransPToDCS(const double* coord3d,GECS* pFromRotPlaneCS=NULL);
	GEPOINT TransPFromDCS(const double* coord3d,GECS* pToRotPlaneCS=NULL);
	GEPOINT TransVToDCS(const double* vector3d,GECS* pFromRotPlaneCS=NULL);
	GEPOINT TransVFromDCS(const double* vector3d,GECS* pToRotPlaneCS=NULL);
public:	//属性
	PROPERTY(SECTION_VIEW_TYPE,m_iViewType);	//旧模式的视图类型
	VIRTUAL_SET(m_iViewType);
	VIRTUAL_GET(m_iViewType);
	READONLY_PROPERTY(int,m_idViewType);//用以标识组焊子视图的类别，将来会部分或全部代替m_iViewType，
	VIRTUAL_GET(m_idViewType){return _idViewType;}
	READONLY_PROPERTY(CSlaveSectionView*,pSlaveView);//用以标识组焊子视图的类别，将来会部分或全部代替m_iViewType，
	VIRTUAL_GET(pSlaveView);
	virtual const char* get_internalSymbol();
	READONLY_PROPERTY(const char*,Symbol);//用以标识组焊子视图的类别，将来会部分或全部代替m_iViewType，
	GET(Symbol){return get_internalSymbol();}
private:
	CHashList<bool>hashDualSideSlotNails;
	ATOM_LIST<ITEM_TAG_INFO>listTagInfo;	//临时挂接的附加到构件项{PART_ITEM}上的数据对象链表
	//判断是否存在相贯支管
	BOOL IsExistIntersectTube(CLDSLineTube *pDatumTube,IModel *pTower);
	//添加相贯支管与基准钢管始末端的检测线
	void AppendIntersectTubeCheckLines(CLDSLineTube* pDatumTube,IModel* pTower);
public:
	double m_fOrgLenOffset;	//对于基准构件为钢管的组焊图，绘图原点(始端外端面)距离钢管原点的长度方向偏移
	CSectionView(int id=0);
	virtual ~CSectionView(){;}	//不能去掉，否则指针类型转换时会出现地址变换异常 wjh-2014.3.29
	virtual int GetViewTypeId(){return m_idViewType;}
	virtual bool IsMainView(){return _iLegacyViewType==VIEW_MAIN;}
	virtual bool IsDualTubeJointView(){return false;}
	virtual bool GetDCS(long hPart,CDrawCoordTrans &dcs);
	bool IsDualSideSlotNail(long hSlot){return hashDualSideSlotNails.GetValue(hSlot)!=NULL;}
	ATOM_LIST<CPartGroupLocatePoint>LocatePoints;	//临时可不存储
	ATOM_LIST<CCheckLineDimension>CheckLines;		//临时可不存储
	ATOM_LIST<CAngularDimension>Angulars;			//临时可不存储
	//添加默认的标定点
	virtual void AddDefaultLocatePoints(CLDSPart *pDatumPart,IModel *pTower);
	//添加默认的角度标注线
	virtual void AddDefaultAngulars(CLDSPart *pDatumPart,IModel *pTower);
	//添加默认的检测线
	virtual void AddDefaultCheckLines(CLDSPart *pDatumPart,IModel *pTower);
	//根据视图类型查找当前视图描述的主构件句柄
	CLDSPart *GetMainPartHandle(IModel *pTower);
	//判断当前视图是否为钢管标准简图
	BOOL IsBriefTubeView(IModel* pTower);
	//判断当前视图是否为钢管塔脚视图 wht 18-08-22
	BOOL IsTubeTowerFootView(IModel* pTower,CLDSPlate **ppTowerFootPlate=NULL);
	//添加指定构件到当前视图构件链表	wht 10-08-10
	CSectionView::PART_ITEM *AddPart(long hSonPart,CLDSPart *pSonPart=NULL,int cFlag=0);
	virtual void ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	int GetId(){return m_id;}
	__declspec( property(get=GetId)) int Id;
	void CopyProperty(CSectionView *pSrcView);
	UCS_STRU UpdateUCS(IModel *pModel,CLDSPartGroup *pPartGroup);
	virtual int SmartFilterNeedDrawParts(IModel *pModel,PARTSET& partset);	//过滤出需要绘制的构件集合
	virtual bool IsHostView() { return true;}
	virtual bool IsSlaveView(){return false;}
	virtual bool IsTubeSectionView(CLDSLineTube** ppSectTube=NULL){return false;}
	virtual char GetViewSubType(){return 0;}
	//管理附加视图属性
	static CHashStrTable<long>propHashtable;	//属性字符串哈希表
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	static void InitViewPropHashtable();
	static int GetPropStr(long id,char* propStr);		//得到属性字符串
	static long GetPropID(char* propStr,...);			//得到属性ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
};
CSectionView::ITEM_TAG_INFO* TagInfo(CSectionView::PART_ITEM* pItem);
CLDSPart* TagPart(CSectionView::PART_ITEM* pItem);
class CJointTubesView : public CSectionView
{	//两根对焊钢管为主的组焊件主视图(以两根对焊钢管交叉面为视图平面)
public:
	char m_ciJointsViewType;	//0:展示对接钢管夹角的俯视图;1:展示对接钢管直线的平视图;2:塔身主材对接正侧面视图
	long m_hRootTube;			//基准钢管（长短差距大时以较长者为基准钢管）
	long m_hJointTube;			//对接钢管（长短差距大时以较长者为基准钢管）
	CJointTubesView(int id=0);
	bool UpdateJointViewType();	//自动计算m_ciJointsViewType的类型值 wjh-2018.3.6
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual bool IsMainView(){return true;}
public:	//属性
	READONLY_PROPERTY(SECTION_VIEW_TYPE,m_iViewType);	//旧模式的视图类型
	VIRTUAL_GET(m_iViewType);
};
class CStdXPlatesView : public CSectionView
{
public:
	char m_cSubType;			//'F'正视图;'S':侧视图;'D':俯视图
	long m_hBasePlate;			//通板句柄
	long m_hFrontVertPlate;		//正面立板
	long m_hBackVertPlate;		//侧面立板
	long m_arrAngleHandle[4];	//关联角钢句柄
	CXhCharTempl<XHSTR_LEN2> symbol;	//剖面图符上的标注文字
	CStdXPlatesView(int id=0);
	virtual char GetViewSubType(){return m_cSubType;}
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual const char* get_internalSymbol(){return symbol;}
};
struct STDX_PLATES_PARTGROUP
{
	CLDSPlate* m_pBasePlate;			//通板
	CLDSPlate* m_pFrontVertPlate;		//正面立板
	CLDSPlate* m_pBackVertPlate;		//侧面立板
	CLDSLineAngle* m_arrAnglePtr[4];	//关联角钢
	STDX_PLATES_PARTGROUP(){memset(this,0,sizeof(STDX_PLATES_PARTGROUP));}
};
struct ANGLE_FOOT_PARTGROUP
{
	CLDSGeneralPlate* m_pFootPlate;
	CLDSGeneralPlate* m_pShoePlate;			//贯通(左侧)靴板
	CLDSGeneralPlate* m_pRightShoePlate;	//右侧靴板
	CLDSGeneralPlate* m_pFrontShoePlate;	//前侧靴板
	CLDSGeneralPlate* m_pBackShoePlate;		//后侧靴板
	CLDSLineAngle* m_arrAnglePtr[4];		//关联角钢
	ANGLE_FOOT_PARTGROUP(){memset(this,0,sizeof(ANGLE_FOOT_PARTGROUP));}
};
class CAngleFootView : public CSectionView
{
public:
	char m_cSubType;			//'F'正视图;'S':侧视图;'D':俯视图
	long m_hFootPlate;			//塔脚基板
	long m_hShoePlate;			//贯通(左侧)靴板
	long m_hRightShoePlate;		//右侧靴板
	long m_hFrontShoePlate;		//前侧靴板
	long m_hBackShoePlate;		//后侧靴板
	long m_arrAngleHandle[4];	//关联角钢句柄	
	long m_hDownLeftShoePlate;	//牛蹄板左下侧靴板
	long m_hDownRightShoePlate;	//牛蹄板右下侧靴板
	long m_hDownFrontShoePlate;	//牛蹄板前下侧靴板
	long m_hDownBackShoePlate;	//牛蹄板后下侧靴板
	long m_arrDownAngleHandle[4];	//牛蹄板关联角钢句柄
	CXhCharTempl<XHSTR_LEN2> symbol;	//剖面图符上的标注文字
	CAngleFootView(int id=0);
	bool IsHoofView(){return m_hDownLeftShoePlate>0;}
	virtual char GetViewSubType(){return m_cSubType;}
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual const char* get_internalSymbol(){return symbol;}
};
class CDualTubeJointView : public CSectionView
{
protected:
	BYTE m_cTemporyJointState;	//临时存储的两钢管梁的对接状态
	bool m_bMasterTubeOnLeft;	//水平基准钢管梁是否在整根曲梁的左侧
public:
	long m_hTemporyStartConnPlate;	//临时存储的左侧钢管梁的端头连接钢板
	long m_hTemporyEndConnPlate;	//临时存储的右侧钢管梁的端头连接钢板
	long m_hMasterTube;	//与X轴平行的水平主管
	long m_hJointTube;	//与水平主管对接的收口钢管
public:
	CDualTubeJointView(int id=0);
	virtual bool IsDualTubeJointView(){return true;}
	virtual bool UpdateConnectState(IModel* pTower,CDualTubeJointView* pCopyFromDualJointTubeView=NULL);
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
public:
	READONLY_PROPERTY(BYTE,cbJointState);
	GET(cbJointState){return m_cTemporyJointState;}
	READONLY_PROPERTY(bool,bMasterTubeOnLeft);	//水平基准钢管梁是否在整根曲梁的左侧
	GET(bMasterTubeOnLeft){return m_bMasterTubeOnLeft;}
	READONLY_PROPERTY(long,hStartJointPlate);
	GET(hStartJointPlate){return m_hTemporyStartConnPlate;}
	READONLY_PROPERTY(long,hEndJointPlate);
	GET(hEndJointPlate){return m_hTemporyEndConnPlate;}
};
class CHorizBeamTubeView : public CDualTubeJointView
{
public:
	static const char HORIZBEAM_FRONT_VIEW		= 'F';	//'F':正视图
	static const char HORIZBEAM_VERTICAL_VIEW	= 'V';	//'V':俯视图
	char m_cSubType;	//
public:
	CHorizBeamTubeView(int id=0);
	virtual char GetViewSubType(){return m_cSubType;}
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
};

class CSlaveSectionView : public CSectionView
{
public:
	int m_idHostView;
	long m_hBaseNode;
	union{
		long m_hRootRod;	//截面图的切割面基准杆件（轴向即截面图法向）
		long m_hVertRod;	//宿主视图绘图坐标系下的剖面图符定位点坐标
	};
	GEPOINT location;	//宿主视图绘图坐标系下的剖面图符定位点坐标
	GEPOINT direction;	//宿主视图绘图坐标系下的剖面图符视线方向
	double direct_offset;	//剖面图符实际标注位置与定位点间沿视线方向的偏移距离
	double symbol_dist;		//剖面图符(成对)间的距离
public:
	CXhCharTempl<XHSTR_LEN2> symbol;	//剖面图符上的标注文字
	virtual const char* get_internalSymbol(){return symbol;}
public:
	CSlaveSectionView(int id=0);
	virtual void ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void InitSlaveSectionView(CSectionView *pHostView,IModel *pModel);
	virtual bool IsHostView() { return false;}
	virtual bool IsSlaveView(){return true;}
	virtual bool IsTubeSectionView(CLDSLineTube** ppSectTube=NULL);
	virtual CLDSGeneralPlate* GetDatumPlateOfFLSectView();
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
};
class CSlaveNodeSectionView : public CSlaveSectionView
{
public:
	CSlaveNodeSectionView(int id=0);
	virtual void ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100){return 0;}	//根据属性ID得到属性值字符串
};
struct KAOMEANGLE{
	long hAngle;
	CLDSLineAngle* pAngle;
	CLDSGeneralPlate *pPlateInWingX,*pPlateInWingY;
	CTmaPtrList<CLDSBolt,CLDSBolt*> *pBoltsInWingX;
	CTmaPtrList<CLDSBolt,CLDSBolt*> *pBoltsInWingY;
	KAOMEANGLE(){
		hAngle=0;
		pAngle=NULL;
		pPlateInWingX=NULL;
		pPlateInWingY=NULL;
		pBoltsInWingX=NULL;
		pBoltsInWingY=NULL;
	}
	~KAOMEANGLE()
	{
		if(pBoltsInWingX)
			delete pBoltsInWingX;
		if(pBoltsInWingY)
			delete pBoltsInWingY;
	}
};
class CKaoMeAngleSectionView : public CSectionView
{
public:
	long m_hAngle;
	CLDSLineAngle *m_pAngle;
	CTmaPtrList<CLDSBolt,CLDSBolt*> boltSetInWingX;
	CTmaPtrList<CLDSBolt,CLDSBolt*> boltSetInWingY;
public:
	CKaoMeAngleSectionView(int id=0);
	virtual void ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void InitView(const KAOMEANGLE &kmAngle);
	CLDSBolt* EnumFirstBolt(BOOL bInWingX);
	CLDSBolt* EnumNextBolt(BOOL bInWingX);
};
class CAngleSpreadSectionView : public CSectionView
{
public:
	CAngleSpreadSectionView(int id=0);
};
class CLDSPartGroup : public CLDSObject
{
protected:
	int	 _iPartGroupType;	//组件类型
public:
	static const short GROUP_FOLDER		=-1;	//组合构件组（目录文件夹）TYPE_STATIONCONNPLATE
	static const short GROUP_COMMON		= 0;	//普通组焊件
	static const short GROUP_ENDCONN	= 1;	//钢管两端插板或无肋(柔性)法兰连接的简单组焊件
	static const short GROUP_ANGLEFOOT	= 2;	//角钢塔脚板
	static const short GROUP_TUBEFOOT	= 3;	//钢管塔脚板
	static const short GROUP_SLOPEJOINT = 4;	//变坡点多支管组焊
	static const short GROUP_SPHERE		= 5;	//球节点
	static const short GROUP_STDX_PLATES= 6;	//组合角钢中标准（十字垂直交叉）连接(填)组焊图
	static const short GROUP_BUSHING	= 7;	//套管挂板组焊图
	static const short GROUP_STATION_BTMTRIBEAM	= 8;//变电构架的三角梁底部收口梁
	static const short GROUP_DUALTUBE_JOINT	= 9;	//双钢管对接相贯其余为连接钢板组焊件
public:
	UCS_STRU ucs;
	//int	 m_iPartGroupType;	//0.组合构件 1.组合构件组 仅在新建组合构件组时临时使用，不存储
	char sFolderName[MAX_CHAR_GUIGE_16+1];	//用于显示构件组时分类显示，默认自动统计时为组合构件所在段号
	char sRootPartNo[MAX_CHAR_GUIGE_16+1];	//临时用不存储
	char sGroupName[MAX_TEMP_CHAR_50+1];	//组合构件名称
	int get_iPartGroupType();				//组件类型
	int set_iPartGroupType(int iValType);	//组件类型（为方便何时更改组焊类型，将此变量改为属性 wjh-2019.8.9）
	__declspec(property(put=set_iPartGroupType,get=get_iPartGroupType)) int m_iPartGroupType;	//组件类型
	long m_hJointTube;		//对接钢管组焊件中对接钢管的句柄
	DWORD m_uStatMatCoef;	//单基塔中的构件组数目
	BOOL m_bFootPartGroup;	//塔脚板组焊件	wht 10-01-14
	CSuperHashList<CSectionView> SonViews;
	
	CLDSPartGroup();
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	//添加默认的组焊件附加视图，在添加组焊件附加视图时为每个视图添加默认的标定点，检测线以及角度标注线
	BOOL AddDefaultAttachViews();
	int SmartIdentifyGroupType();	//智能识别组合类型
	bool IdentifyAngleFoot(ANGLE_FOOT_PARTGROUP *pAngleFoot=NULL,bool upper=true,const double* front_direction=NULL);			//识别是否角钢塔脚
	bool IdentifyStdXPlates(STDX_PLATES_PARTGROUP *pStdXPlatesPartGroup=NULL);//识别是否为组合角钢中标准十字连接组合焊板
	bool IdentifyBushingPlate();	//识别是否含有套管的挂线板组焊件
	bool IdentifyDualTubeJoint(int *horiBeamOrGeneralDualTube);	//识别是否为构架梁收口钢管组焊图
	bool IsNeedSidePlateView();//普通钢板组焊件是否需要立板视图
	KEY2C GetIdleSectSymbol();
	KEY2C GetIdleSectSymbol(KEY2C prefer);	//prefer为优先推荐的键值类型，如'A1',则生成Ax系列的空闲剖视符号
private:
	//1.添加对接相贯钢管视图
	BOOL AddJointTubeViews(CLDSLineTube *pRootTube,PARTSET &partset);		
	//2.四根支管相贯与主管的K节点类组焊件
	BOOL AddKNodeTransectViews(CLDSLineTube *pRootTube,PARTSET &partset);
	//3.添加基准构件为杆件的组焊件视图
	BOOL AddCommonRodPartGroupViews(CLDSLinePart *pRootLinePart,PARTSET &partset);
	//3.1 无主视图(如钢管两端均为法兰且钢管上无连接板的情况)时添加主视图
	//3.2 添加钢管中间节点对应的端面视图
	BOOL AddLineTubeMiddleNodeEndView(CLDSLineTube *pLineTube,CSectionView *pMainView,PARTSET &partset,int &view_index);
	//3.3 添加钢管端板视图(包括平面切割、法兰、插板、塔脚板)
	BOOL AddLineTubeEndLjView(CLDSLineTube *pLineTube,CSectionView *pMainView,PARTSET &partset,int iStartView,int &view_index);
	//3.4 为组焊件中脚钉底座添加端面图
	BOOL AddFootNailView(CLDSLineTube *pLineTube,CSectionView *pMainView,PARTSET &partset,int &view_index);
	//3.5 添加钢管抱杆视图
	BOOL AddTubeBushingPlateView(CLDSLineTube *pLineTube,PARTSET &partset);
	//根据卷边插板修正视图方向 wht 18-09-22
	BOOL AdjustFrontViewWorkNormByInsertPlate(CLDSLinePart *pRootRod,PARTSET &partset);
	//4.添加基准构件为钢板的组焊件视图
	BOOL AddPlatePartGroupViews(CLDSPlate *pRootPlate,PARTSET &partset);
	//5.智能添加标准十字交叉连接组焊视图
	BOOL AddStdXPlatesViews(CLDSPart* pDatumPart,PARTSET& partset);
	//6.智能添加角钢塔脚板组焊视图
	BOOL AddAngleFootViews(CLDSPart* pDatumPart,PARTSET& partset);
	//7.添加主材为角钢的组焊件视图
	BOOL AddAnglePartGroupViews(CLDSLineAngle *pLineAngle,PARTSET& partset);
	//8、智能添加含套管挂板组焊件视图
	BOOL AddBushingPlateView(CLDSPart* pDatumPart,PARTSET& partset);
	//9、智能添加变电构架三角梁底部的收口梁组焊件视图
	BOOL AddHorizBeamTubeView(CLDSPart* pDatumPart,PARTSET& partset);
	//10、智能添加变电构架三角梁底部的收口梁组焊件视图
	BOOL AddGeneralDualTubeJointView(CLDSPart* pDatumPart,PARTSET& partset);
protected:
	void UpdateDualTubeJointView(CDualTubeJointView *pMainView,CDualTubeJointView **ppMainView2,CLDSLineTube *pDatumTube,CLDSLineTube *pJointTube,PARTSET& partset1,PARTSET& partset2);
public:
	// 用于存储与组合类型相关的一些辅助参数，其诠释也与组合类型相关
	// 如球节点时可由用户指定旋转主管和辅助支管 wjh-2013.11.16
	ATOM_LIST<DESIGN_PLATE_ITEM>designItemList;
	//设计参数列表函数
	void EmptyDesignItems(){designItemList.Empty();}
	//BOOL GetDesignItemValue(WORD wKey, double *pfValue,CHashTable<DESIGN_PLATE_ITEM*>*pTbl=NULL);
	BOOL GetDesignItemValue(WORD wKey, long *piVal,CHashTable<DESIGN_PLATE_ITEM*>*pTbl=NULL);
	char* GetDesignItemNotes(WORD wKey);
	DESIGN_PLATE_ITEM *GetDesignItem(WORD wKey);
	BOOL IsHasDesignItemValue(WORD wKey);
	BOOL DeleteDesignItem(WORD wKey);
	BOOL SetDesignItemValue(WORD wKey,double fValue,char *sNotes=NULL,int iParamType=PARAM_POS,int iDatatype=0,BOOL bModify=FALSE);
	void RetrieveKaoMeAngle(CHashList<KAOMEANGLE> &hashKaoMe);
};
#endif
//struct IPartLibrary{;} 转到IPartLibrary.h中定义
class CLDSModule;
class CWorkStatus;
class CDisplayView;
struct IConsole : public IModel
{
	virtual CDisplayView *SetActiveView(long hView)=0;
	virtual CDisplayView* GetActiveView()=0;
	virtual BOOL DeleteDisplayView(long hView)=0;
	virtual long ActiveModuleHandle()=0;
	virtual IModel* GetActiveModel()=0;
	virtual CLDSModule* GetActiveModule()=0;
#ifdef __PART_DESIGN_INC_
	virtual CWorkStatus* GetActiveWorkStatus()=0;
#endif
	virtual UCS_STRU WCS()=0;	//当前工作坐标系
	virtual GEPOINT TransPFromScr(const double* point)=0;
	virtual GEPOINT TransVFromScr(const double* vector)=0; 
public:
	virtual void EmptyBlkObjSetBuf(){;}
	virtual bool AddToObjSetBuf(CLDSObject* pObj)=0;
	virtual CLDSNode* EnumBlkBufNodeFirst()=0;
	virtual CLDSNode* EnumBlkBufNodeNext()=0;
	virtual CLDSPart* EnumBlkBufPartFirst(bool bOnlyRod=false)=0;
	virtual CLDSPart* EnumBlkBufPartNext(bool bOnlyRod=false)=0;
public:
	virtual void EmptyDispNode()=0;
	virtual void EmptyDispPart()=0;
	virtual CLDSNode* AddToDispSet(CLDSNode* pNode)=0;
	virtual CLDSPart* AddToDispSet(CLDSPart* pPart)=0;
	virtual NODESET& GetDispNodeSet()=0;
	virtual PARTSET& GetDispPartSet()=0;
	virtual CLDSNode* EnumFirstDispNode()=0;
	virtual CLDSNode* EnumNextDispNode()=0;
	virtual CLDSPart* EnumFirstDispPart()=0;
	virtual CLDSPart* EnumNextDispPart()=0;
	virtual bool EmptyAllDispObjList()=0;	//清空所有显示对象列表
	//删除事件
	virtual void FireDeleteObject(CLDSObject *pObj,BOOL bDelFromSolidDraw=FALSE)=0;
};

void CfgwordToBuffer(CFGWORD &cfgword,CBuffer &buffer,long version=0,long doc_type=0);
void CfgwordFromBuffer(CFGWORD &cfgword,CBuffer &buffer,long version=0,long doc_type=0);
f3dPoint SpreadFacePntToDatumFace(f3dPoint datum_face_norm,f3dLine huoqu_line,
					f3dPoint spread_face_norm,f3dPoint pick,BOOL bFromMuti=TRUE);
void MirPartHandle(IModel *pTower,long *mir_h,long org_h,MIRMSG msg,COORD3D *pPickPos=NULL);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
typedef CTmaPtrList<CLDSBolt,CLDSBolt*> BOLTSET;
typedef CTmaPtrList<CLDSLineAngle,CLDSLineAngle*> JGSET;
typedef CTmaPtrList<CLDSLineTube,CLDSLineTube*> TUBESET;
typedef CTmaPtrList<CLDSPlate,CLDSPlate*> PLATESET;
#ifdef __GGG_
typedef CTmaPtrList<CLDSConeTube,CLDSConeTube*> CONETUBESET;
#endif
void CreateArcLine(f3dAtomLine *pAtomLine,f3dPoint &workNorm,PROFILE_VERTEX &profileVertex);
BOOL IsPartsCollide(CLDSPart *pMainPart,CLDSPart *pVicePart,int nCollideVertexDist=-1);//,LS_POS_IN_JG_STRU first_ls_pos=NULL);
BOOL IsBerHuoQuJg(CLDSLineAngle *pHuoQuBaseJg, f3dPoint ray_jg_vec,int huoqu_x_wing0_y_wing1);
#endif
typedef CTmaPtrList<CBlockReference,CBlockReference*>BLOCKREFSET;
typedef CTmaPtrList<CLDSFoundation,CLDSFoundation*> FOUNDATIONSET;

#endif
