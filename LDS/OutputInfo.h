// ExtendItemInfo.h: interface for the COutputInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTENDITEMINFO_H__6FA66137_1CA6_4EA8_834A_67510409C72C__INCLUDED_)
#define AFX_EXTENDITEMINFO_H__6FA66137_1CA6_4EA8_834A_67510409C72C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "f_ent.h"
#include "SegI.h"
#include "ArrayList.h"
#include "XhCharString.h"
#include "BasicOutput.h"
#include "MemberProperty.h"
#include "lds_part.h"

//扩展项类型
enum OUTPUT_INFO_TYPE{
	//1.智能修正
	INFO_EMEND_SINGULAR_BOLTLO=1,	//螺栓通厚修订
	INFO_EMEND_CROSSRODINFO,		//更新交叉材计算信息
	INFO_EMEND_MIRRODINFO,			//修正关联对称信息
	INFO_EMEND_BOLTGASKET,			//增补修订螺栓垫片或垫圈
	INFO_EMEND_LAYER,				//修正对象图层名称
	INFO_EMEND_TUBEENDJOINTLJ,		//校核修正钢管端头连接信息
	//2.常规校核
	//编号前检查
	INFO_CHK_MIRROD,				//对比对称杆件的规格材质及基准长度，以便下一步进行构件编号
	INFO_CHK_MIRROD_LENGTH,			//对比对称杆件的基准长度
	INFO_CHK_MIRPLATE,				//对称钢板检测
	INFO_CHK_BOLTOVERLAP,			//重叠螺栓检测
	INFO_CHK_HOLEMISSING,			//漏孔检测
	INFO_CHK_REDUNDANTBOLT,			//冗余螺栓检测
	//螺栓合法性校核
	INFO_CHK_BOLT_ROLLINGSPACE,		//角钢轧制边距
	INFO_CHK_BOLT_CUTANGLESPACE,	//角钢切角边距
	INFO_CHK_BOLT_ANGLE_ENDSPACE,	//角钢螺栓端距
	INFO_CHK_BOLT_ANGLE_FORGEERR,	//角钢压扁与螺栓孔位冲突
	INFO_CHK_BOLT_PLATE_ENDSPACE,	//连板螺栓边距
	INFO_CHK_BOLT_HUOQULINE_SPACE,	//连板螺栓边距(到火曲线)
	INFO_CHK_BOLT_L0,				//螺栓通厚范围
	INFO_CHK_BOLT_SPEC,				//螺栓合法规格
	INFO_CHK_BOLT_GRADE,			//螺栓级别与规格库是否相符
	//数据完整性校核，数据完整性校验(含错误实体数据无法显示的构件, 多为板构件)
	INFO_ROD_LOST_STARTNODE,		//杆件始端节点丢失
	INFO_ROD_LOST_ENDNODE,			//杆件终端节点丢失
	INFO_NODE_LOST_FATHERROD,		//节点父杆件丢失
	INFO_NODE_LOST_ATTACHNODE,		//依附节点
	INFO_NODE_LOST_ATTACHROD,		//依附杆件
	INFO_NODE_LOST_ATTACHPLANE,		//依附平面
	INFO_NODE_LOST_ATTACHPOINT,		//依附基准点
	INFO_PART_LOST_MATERIAL,		//构件缺少材质信息
	INFO_PART_INVALID_MATERIAL,		//构件材质信息不正确
	INFO_ANGLE_LOST_LSLIST,			//角钢缺少螺栓链表
	INFO_ANGLESTART_LOST_BENDLINE,	//角钢始端缺少火曲基准角钢
	INFO_ANGLESTART_LOST_PRJLINE,	//角钢始端缺少共线位置基准角钢
	INFO_ANGLEEND_LOST_BENDLINE,	//角钢终端缺少火曲基准角钢
	INFO_ANGLEEND_LOST_PRJLINE,		//角钢终端缺少共线位置基准角钢
	INFO_ANGLEWINGX_LOST_REF_ROD,	//角钢X肢法线计算过程中缺少参照杆件
	INFO_ANGLEWINGY_LOST_REF_ROD,	//角钢Y肢法线计算过程中缺少参照杆件
	INFO_PLATE_LOST_DATUM_REF_NODE,	//基准节点句柄为空
	INFO_PLATE_LOST_DATUM_POS_DES_PARA,	//基准点参数
	INFO_PLATE_LOST_NORM_DES_PARA,	//基准面法线参数
	INFO_PLATE_LOST_LINK_REF_ROD,	//连接杆件
	INFO_BOLT_LOST_DATUM_REF_NODE,	//螺栓丢失基准节点
	INFO_BOLT_LOST_DATUM_REF_PART,	//螺栓丢失基准构件
	INFO_BOLT_LOST_NORM_DES_PARA,	//螺栓丢失法线设计参数
	//合并重复节点及重复杆件
	INFO_EMEND_OVERLAPNODE,			//合并重复节点(修正)
	INFO_EMEND_OVERLAPROD,			//合并重复杆件(修正)
	//构件实体数据合法性检查
	INFO_BOLT_NORM_ZERO,			//螺栓法线为空
	INFO_PART_LOCATION_ABNORMAL,	//构件位置异常
	INFO_INVALID_PART_UCS,			//坐标系不合法
	INFO_INVALID_PROFILE,			//非法外形,实体顶点数过少、长度或宽度或厚度为0
	//
	INFO_CHK_MODELINTEGRITY,		//模型内杆件节点的匹配关系检查
	INFO_CHK_BOLTCONNECT,			//杆件上螺栓连接强度
	INFO_CHK_MODELSTABILITY,		//体系稳定性检查
	//3.编号校核
	//杆件
	INFO_CHK_NULLPARTNO,			//空编号构件
	INFO_CHK_PARTTYPE_DIFFER,		//构件类型不同
	INFO_CHK_RODINFO,				//同编号杆件长度及正负头、规格、材质等基本信息
	INFO_CHK_RODBOLTINFO,			//同编号杆件上螺栓数量、规格,杆件上螺栓位置信息（含颠倒对比）
	INFO_CHK_ANGLECUT,				//角钢上切角信息（含颠倒对比）
	//钢板
	INFO_CHK_PLATEINFO,				//同编号钢板厚度、材质、火曲面数
	INFO_CHK_PLATEPROFILEANDBOLT,	//钢板螺栓数数量及规格及相对孔位、卷边及外形，颠倒互换性
	//影射构件
	INFO_CHK_SHADOWPARTINFO,		//影射构件与始祖级构件的对比信息
	//4.碰撞校核
	INFO_CHK_COLLIDE_RODS,		//杆件与杆件碰撞(可指定区域)
	INFO_CHK_COLLIDE_RODBOLT,	//杆件与螺栓碰撞(可指定区域)
	INFO_CHK_COLLIDE_PLATEBOLT,	//钢板与螺栓碰撞(可指定区域)
	INFO_CHK_COLLIDE_BOLTS,		//螺栓与螺栓碰撞(可指定区域)
	INFO_CHK_COLLIDE_RODPLATE,	//杆件与连板碰撞(可指定区域)
	INFO_CHK_COLLIDE_PLATES,	//连板与连板碰撞(可指定区域)
	//
	INFO_ONEKEY_CUTANGLE_CHECK,	//一键切角检测
	//5.查找统计
	INFO_STAT_BOLTRELAPARTS,	//查询螺栓关联构件
	INFO_STAT_RODLENGTH,		//长度区间内的杆件统计
	INFO_STAT_RODSIZE,			//指定规格的杆件统计
	//6.计算报告比对
	INFO_COMPARE_REPORT_WARNNING,	//计算报告比对中的纯文本警告信息
	INFO_COMPARE_REPORT_NODES,		//计算报告中与节点相关的警告信息
	INFO_COMPARE_REPORT_ELEMS,		//计算报告中与单元相关的警告信息
	INFO_COMPARE_REPORT_RODS,		//计算报告中与杆件相关的警告信息
};
struct AUTOCORRECT_CMDINFO{
	//0.可安全地自动进行错误修正;1.可带有风险(需要通过菜单命令确认)地自动进行错误修正;2.需要用户交互的修正错误
	short safeType;
	CXhChar16 nativeName;
};
class COutputInfo : public IBasicOutput
{
protected:
	bool state;	//true:已解决;false:待解决
public:
	int m_uType;//类型
	//long m_hRelaObj1;		//关联对象id1
	//long m_hRelaObj2;		//关联对象id2
	double tolerance;		//外形校核时的允许误差范围,默认0.5mm
	CString m_sDescription;	//描述信息
	GEPOINT collisionPt;	//实体碰撞点
#ifdef __PART_DESIGN_INC_
public:	//单元验算结果比对信息, 以后应抽时间单提出COutputElemComparison类中 wjh-2017.3.15
	static const BYTE ELEM_DIFF_BOTH_COMPRESSION = 1;		//同时受压类型
	static const BYTE ELEM_DIFF_TENSION_REDUCTION_COEF		= 2;//受拉强度折减系数
	static const BYTE ELEM_DIFF_COMPRESSION_REDUCTION_COEF	= 3;//受压强度折减系数
	static const BYTE ELEM_DIFF_BLOCK_SHEAR_STATE	= 4;//块剪受控状态
	static const BYTE ELEM_DIFF_STABLE_COEF			= 5;//压稳系数
	static const BYTE ELEM_DIFF_STABLE_mN			= 6;//压杆稳定强度折减系数
	static const BYTE ELEM_DIFF_LAMDA_L				= 7;//计算长度
	static const BYTE ELEM_DIFF_GYRATION			= 8;//回转半径
	static const BYTE ELEM_DIFF_LAMDA				= 9;//长细比
	static const BYTE ELEM_DIFF_MAX_TENSION			=10;//最大拉力,N
	static const BYTE ELEM_DIFF_MAX_TENSION_CASE	=11;//最大拉力工况
	static const BYTE ELEM_DIFF_MAX_COMPRESSION		=12;//最大压力,N
	static const BYTE ELEM_DIFF_MAX_COMPRESSION_CASE=13;//最大压力工况
	static const BYTE ELEM_DIFF_MAX_STABLE_N		=14;//最大压稳力,N
	static const BYTE ELEM_DIFF_MAX_STABLE_SIGEMA	=15;//最大压稳应力,MPa
	static const BYTE ELEM_DIFF_MAX_STABLE_CASE		=16;//最大压稳力工况
	static const BYTE ELEM_DIFF_DANGEROUS_CASE		=17;//受控承载条件
	static const BYTE ELEM_DIFF_MAX_WORK_SIGEMA		=18;//最大工作应力,MPa
	static const BYTE ELEM_DIFF_BEARLOAD_COEF		=19;//承载系数
	static const BYTE ELEM_DIFF_OUTSIDE_MISS	=100;	//外界导入单元缺失
	BYTE m_ciElemDiffType;
	union COMPARE_ITEM_VALUE{
		bool bValue;
		long iValue;
		double fValue;
		char strbuf[16];
	}compare;
	PROPERTY(long,m_idElem);	//当前模型单元
	GET(m_idElem){return m_hRelaObj1;}
	SET(m_idElem){m_hRelaObj1=value;}
	PROPERTY(long,m_idElem2);	//参照比对单元
	GET(m_idElem2){return m_hRelaObj2;}
	SET(m_idElem2){m_hRelaObj2=value;}
#endif
public:
	COutputInfo();
	virtual ~COutputInfo();
	virtual void SetMessage(const char* message){m_sDescription=message;}
	virtual const char* get_Message(){return m_sDescription;}
	virtual const char* GetTypeName();
	virtual void SelectRelationObj();
	virtual CString GetKeyStr();
	virtual bool CopyFrom(IBasicOutput* pOutput);

	int GetLegacyTypeId() const {return m_uType;}
	DWORD ChkStyleFlag();
	bool IsCollisionOutputType();
	bool SolvedState(){return state;}
	virtual bool CorrectError();
	// 返回值>=0表示支持错误修正功能
	virtual int CorrectErrorCmdInfo(AUTOCORRECT_CMDINFO &cmdInfo);
	virtual bool Recheck();
	static const char* GetTypeName(int uiType,int m_idOutputType=1);
};
class COutputElemComparison : public IBasicOutput
{
public:
	PROPERTY(long,m_idElem);	//当前模型单元
	GET(m_idElem){return m_hRelaObj1;}
	SET(m_idElem){m_hRelaObj1=value;}
	PROPERTY(long,m_idElem2);	//参照比对单元
	GET(m_idElem2){return m_hRelaObj2;}
	SET(m_idElem2){m_hRelaObj2=value;}
	//long m_hRelaObj1;		//关联对象id1
	//long m_hRelaObj2;		//关联对象id2
	//double tolerance;		//外形校核时的允许误差范围,默认0.5mm
	//CString m_sDescription;	//描述信息
public:
	COutputElemComparison();
	virtual ~COutputElemComparison();
	//virtual void SetMessage(const char* message){m_sDescription=message;}
	virtual const char* GetTypeName();
	virtual void SelectRelationObj();
	virtual CString GetKeyStr();
};
class COutputInfoAgent{
	void* m_pContext;
public:
	COutputInfoAgent(void* pContext){m_pContext=pContext;}
	OUTPUT_INFO_TYPE m_uType;
	COutputInfo* (*ListAppendFunc)(void* pContext);
	COutputInfo* Append(long hRelaObj1,long hRelaObj2,char* format,...);
};
class CCheckPartNoInfo : public COutputInfo
{
public:
	CString m_sPartNo;
public:
	CCheckPartNoInfo();
	~CCheckPartNoInfo();
	CCheckPartNoInfo(CString sPartNo,long hObj1,long hObj2,CString sDescription);
	virtual bool Recheck();
	static void PartNoCheck(CTaskOutputList &itemList,CString sSegStr,double fScope,char *sPartNo=NULL);
};
void AppendCheckLabelOutputInfo(CTaskOutputList* pItemList,CCheckPartNoInfo& checklabel);
class CCheckGasketErrorInfo : public COutputInfo
{
public:
	CCheckGasketErrorInfo();
	~CCheckGasketErrorInfo();
	//CCheckPartNoInfo(CString sPartNo,long hObj1,long hObj2,CString sDescription);
	short siGasketThick;//垫板垫圈厚度(负值表示垫板自基点沿螺栓工作朝向向下）
	struct ANCHOR{
		long hAnchorPart;	//如需添加垫板时，垫板的定位基准构件
		double gasketOffset;//垫板自基准螺栓在基准构件上的穿插交点的偏移值，用来确定垫板位置（基准构件的工作法线为偏移正向)
		double washerOffset;//垫圈自螺栓的偏移距
		ANCHOR(){hAnchorPart=0;gasketOffset=washerOffset=0;}
	}anchor;
	//this->m_uType=ERROR_GASKET_PROFILE时表示需要统一外形的多孔垫板句柄
	//this->m_uType=ERROR_MISSING_ENDGASKET及ERROR_MISSING_CROSSGASKET时表示需要添加垫板所包括的螺栓句柄集合
	//this->m_uType==ERROR_BOLTPAD_CUTANGLE时表示垫板切角的参照角钢句柄，此时仅首个预置数组有效
	PRESET_LONG4ARR relaObjIds; 
	virtual bool CorrectError();
	// 返回值>=0表示支持错误修正功能
	virtual int CorrectErrorCmdInfo(AUTOCORRECT_CMDINFO &cmdInfo);
	virtual bool Recheck();
	virtual bool CopyFrom(IBasicOutput* pOutput);
public:	//属性
	long get_hRelaBolt();
	long set_hRelaBolt(long hBolt);
	__declspec(property(put=set_hRelaBolt,get=get_hRelaBolt)) long hRelaBolt;
	long get_hRelaGasket();
	long set_hRelaGasket(long hGasket);
	__declspec(property(put=set_hRelaGasket,get=get_hRelaGasket)) long hRelaGasket;
};
class DATACENTERCMD_CONTEXT
{
public:
	void* m_pOutputInfoList;
	void* m_pMainWnd;
	void* m_pTag;
	DWORD chkStyleFlag;
	double tolerance;	//误差(deviation)允许范围
	long nMaxWarnings;	//报警项数目超出此值时中止进行校审
	CHashList<SEGI> segTable;
	DATACENTERCMD_CONTEXT(){
		m_pOutputInfoList=NULL;
		m_pMainWnd=NULL;
		m_pTag=NULL;
		chkStyleFlag=0;
		nMaxWarnings=0;
		tolerance=0.5;
	}
	void InitByCmd(CMDCFG_ITEM &cmdItem)
	{
		int j=0;
#ifndef __ENABLE_XHSTRING_LENGTH_PROPERTY_
		if(cmdItem.properties[0].bActive&&cmdItem.properties[0].name.Length()==0)
#else
		if(cmdItem.properties[0].bActive&&cmdItem.properties[0].name.Length==0)
#endif
			chkStyleFlag=cmdItem.properties[0].val.dwVal;	//内置的校核内容掩码字
		else
			j=1;
		for(j=j;j<4;j++)
		{
			if(!cmdItem.properties[j].bActive)
				break;
#ifdef AFX_TARG_ENU_ENGLISH
			else if(cmdItem.properties[j].name.Equal("Permissible error(mm)"))
#else 
			else if(cmdItem.properties[j].name.Equal("允许误差(mm)"))
#endif
				tolerance=cmdItem.properties[j].val.dblVal;
		}
	}
	BOOL IsGreaterThanMaxWarnings(int nWarnings)
	{
		if(nMaxWarnings>0&&nWarnings>=nMaxWarnings)
			return TRUE;
		return FALSE;
	}
};
#ifdef __COMMON_PART_INC_
//INFO_CHK_NULLPARTNO 构件号检查共用同一个函数
typedef ARRAY_LIST<CLDSPart*> PART_PTR_ARR;
typedef ARRAY_LIST<CLDSPolyPart*>POLYPART_PTR_ARR;
class PARTNO_CHK_CONTEXT : public DATACENTERCMD_CONTEXT
{
public:
	int nSumPartNum;	//待检查的构件总数(普通构件及制弯构件总和)，主要用于显示进度条
	CHashStrList<PART_PTR_ARR> partsHasListPartNo;
	CHashStrList<POLYPART_PTR_ARR> polyPartsHasListPartNo;
	CXhChar16 sSpecifyPartNo;
	BOOL m_bOutputToNotepad;
	PARTNO_CHK_CONTEXT()
	{
		nSumPartNum=0;
		m_bOutputToNotepad=FALSE;
		partsHasListPartNo.Empty();
		polyPartsHasListPartNo.Empty();
	}
};
#endif
extern CHashStrList<COutputInfo> markInfoList;
#endif // !defined(AFX_EXTENDITEMINFO_H__6FA66137_1CA6_4EA8_834A_67510409C72C__INCLUDED_)