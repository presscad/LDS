// tma_co_part.h: interface for the CJoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TDA_CO_PART_H__415A1A1F_534B_4ACC_B588_13AA8178D691__INCLUDED_)
#define AFX_TDA_CO_PART_H__415A1A1F_534B_4ACC_B588_13AA8178D691__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __COMMON_PART_INC_
#include "lds_part.h"
#include "lds_list.h"
#include "ldsptr_list.h"
//接头
typedef struct tagPART_ITEM
{
	long h;
	char cID;
}PART_ITEM;
class CJoint : public CLDSPart
{
	ATOM_LIST<PART_ITEM> *part_list;//子构件（包括：钢板、角钢等）
public:
	CJoint();	//新增
	~CJoint();	//新增
	// 接头方式0.外包铁 1.内外包铁 2.内包铁外包钢板 3.外包铁内包钢板4.内包铁X肢钢板截断
	// 5.内包铁Y肢钢板截断6.四包铁X肢钢板截断7.四包铁Y肢钢板截断8.十字接头X肢钢板截断9.十字接头Y肢钢板截断
	long joint_style;
	double length;	//接头长度
	//0:Y轴对称;1:X轴对称;2:Z轴对称
	THANDLE mir_joint_handle[3];
	THANDLE base_node_handle;
	THANDLE base_jg_handle_arr[2];
	THANDLE get_hMainAngle() { return base_jg_handle_arr[0]; }
	THANDLE set_hMainAngle(THANDLE hAngle) { return base_jg_handle_arr[0] = hAngle; }
	__declspec(property(put = set_hMainAngle, get = get_hMainAngle)) THANDLE hMainAngle;	//正头角钢
	THANDLE get_hCoupleAngle() { return base_jg_handle_arr[1]; }
	THANDLE set_hCoupleAngle(THANDLE hAngle) { return base_jg_handle_arr[1] = hAngle; }
	__declspec(property(put = set_hCoupleAngle, get = get_hCoupleAngle)) THANDLE hCoupleAngle;	//负头角钢
	bool blInheritExistBoltsInMainAngle, blInheritExistBoltsInCoupleAngle;
	bool blIntelliCalOddment;	//自动计算角钢断面偏移量(正负头)
public:
	CLsRefList boltRefList;
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//void Serialize(CArchive &ar);
	CLDSLineAngle *AppendWrapJg(char cId);
	CLDSLineAngle *AppendWrapJg(CLDSLineAngle *pJg,char cId);
	BOOL AppendWrapJg(long h,char cId);
	//CLDSPolyAngle *AppendHuoQuWrapJg(char cId);
	//CLDSPolyAngle *AppendHuoQuWrapJg(CLDSPolyAngle *pHuoQuJg,char cId);
	CLDSPlate* AppendWrapPlank(char cId);
	BOOL AppendWrapPlank(long h,char cId);
	CLDSPlate* AppendWrapPlank(CLDSPlate *pPlank,char cId);
	CLDSPart *GetFirstPart();
	CLDSPart *GetNextPart();
	PART_ITEM *GetFirstPartItem(){return part_list->GetFirst();}
	PART_ITEM *GetNextPartItem(){return part_list->GetNext();}
	long GetPartCount(){return part_list->GetNodeNum();}
	PART_ITEM *FromPartId(char cId);
	void EmptyAllSubPart();
	void EmptyAllSubPartH(){part_list->Empty();}
};

//火曲角钢中,每一直线段模型
class CHuoQuSeg{
public:
	CLDSNode *pStart,*pEnd;
	THANDLE handle;
	f3dPoint start_xyz,end_xyz;
	f3dPoint norm_x_wing,norm_y_wing;
	double huoqu_r;
	CHuoQuSeg();
public:
	void getUCS(UCS_STRU& ucs);
};
//杆件变形点
class CLDSPolyJoint : public CLDSPart
{
public:
	static UINT crPolyJoint;
	double rBending;	//制弯时的制弯点内圆弧半径(mm)
	CLDSLinePart *pPrevLinePart,*pNextLinePart;
	CLDSPolyJoint();
	~CLDSPolyJoint();
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//属性管理
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	int GetPropValueStr(long id,char* valueStr);		//根据属性ID得到属性值
	static void InitPolyJointPropHashtable();
	UPDATE_PROPLISTITEM_FUNC(CLDSPolyJoint,propHashtable)	//定义属性项更新函数
	static long GetPropID(char* propStr);	//得到属性ID
	static int GetPropStr(long id,char* propStr);		//得到属性字符串
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
};

class CLDSGroupLineAngle : public CLDSLineAngle
{
	union GROUP_STYLE{
		long dwStyle;	//实体状态时的火曲角钢组合型式0:对角;1:T型;2:十字交叉型
		struct{
			WORD wSectStyle;	//截面类型
			BYTE ciMasterIndex;	//象征组合角钢的特征子角钢索引，0 based index
			BYTE ciMasterDirectIndex;	//肢朝向,0:虚拟组合角钢;1:1号子角钢;2:2号子角钢,类推3、4号子角钢
		};
	}style;
	double _jg_space;	//子角钢之间间隙
public:	//公用属性
	PROPERTY(WORD,group_style);
	GET(group_style);
	SET(group_style);
	PROPERTY(WORD,wSectStyle);
	GET(wSectStyle){return group_style;}
	SET(wSectStyle){group_style=value;}
	
	PROPERTY(BYTE,ciMasterIndex);	//基准子角钢的索引号，0 base index
	GET(ciMasterIndex);
	SET(ciMasterIndex);
	PROPERTY(BYTE,ciMasterDirectIndex);	//肢朝向,0:虚拟组合角钢;1:1号子角钢;2:2号子角钢,类推3、4号子角钢
	GET(ciMasterDirectIndex);
	SET(ciMasterDirectIndex);
	//
	READONLY_PROPERTY(long,m_hDatumSonAngle);
	GET(m_hDatumSonAngle);
	READONLY_PROPERTY(CLDSLineAngle*,m_pDatumSonAngle);
	GET(m_pDatumSonAngle);

	PROPERTY(double,jg_space); 
	GET(jg_space){
		CLDSGroupLineAngle* pMotherObj=(CLDSGroupLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _jg_space;
		else
			return pMotherObj->jg_space;
	}
	SET(jg_space){
		if(value>=0)
		{
			CLDSGroupLineAngle* pMotherObj=(CLDSGroupLineAngle*)MotherObject();
			_jg_space= value;
			if(pMotherObj!=this)
				pMotherObj->jg_space=value;
		}
	}
public:
	virtual char* GetBriefPartTypeMark();
	bool IsNeedUpdateGroupStyle(){return style.wSectStyle!=group_style;}
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void UpdateSonJgPerm();
	int AppendSonJg();
	virtual bool SetDirectMasterAngle();	//设定组合角钢自身为组合角钢及子角钢的肢法线计算基准角钢 wjh-2016.9.19
	void cal_y_wing_norm();
	void cal_x_wing_norm();
	virtual void CalPosition(bool bIgnoreRepeatFlag=FALSE);
	virtual void CalStartOddment(char ciAutoCalMode=0,int liInitOddment=0);	//0.普通碰撞规则算;1.强制性把指定负头情况按碰撞规则算;2.按允许切角时保证切角边距情况的碰撞规则算
	virtual void CalEndOddment(char ciAutoCalMode=0,int liInitOddment=0);	//0.普通碰撞规则算;1.强制性把指定负头情况按碰撞规则算;2.按允许切角时保证切角边距情况的碰撞规则算
	void UpdateSonJgBasicInfo();
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void ToNormInfoBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromNormInfoBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual bool IsBelongTo(int iClassTypeId)const;
	//自协同中心通过管道读取数据
	virtual void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	//通过管道向协同中心写入数据
	virtual void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	void UpdateSonJgWingNorm(BYTE cbSonAnglesFlag=0xff);
	void UpdateSonJgPos();
	void UpdateSonJgOddment();
	bool ShiftMasterIndex(BYTE masterIndex=0);	//特征子角钢的索引，0 based index
	CLDSGroupLineAngle();
	~CLDSGroupLineAngle();
	BOOL m_bUpdateSonAngleOddment;
	//属性
	int GetPropValueStr(long id,char* valueStr);	//根据属性ID得到属性值字符串
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static void InitGroupLineAnglePropHashtable();
	ADD_PROPLISTITEM_FUNC(CLDSGroupLineAngle,propHashtable)	//定义属性项更新函数
	static PROPLIST_ITEM* UpdatePropItem(char* propKey,char* propName=NULL);		//得到属性字符串
	static int GetPropStr(long id,char* propStr);		//得到属性字符串
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//检测属性值是否相等
	static CHashTable<DWORD>propStatusHashtable;		//属性状态哈希表
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//得到构件属性的默认只读状态
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//得到构件属性的默认只读状态
	static int m_iCurDisplayPropGroup;					//记录属性栏当前显示组的ID
	/*CLsRef* AppendStartLsRef(CLsRef ls_ref,BOOL bNearLsCheck=TRUE);
	CLsRef* AppendMidLsRef(CLsRef ls_ref,BOOL bNearLsCheck=TRUE);
	CLsRef* AppendEndLsRef(CLsRef ls_ref,BOOL bNearLsCheck=TRUE);
	BOOL DeleteLsRef(THANDLE ls_handle);
	int GetLsCount(short d=0);
	void EmptyLsRef();*/
};
class CGroupAngleLifeObj{
private:
	bool m_bRestore;
	long m_iVersion;
	long m_iDocType;
	char _pPresetMemPool[360];	//为提升速度为m_xBuffer预留的内存
	CBuffer m_xBuffer;
	CLDSGroupLineAngle* m_pSrcAngle;
public:
	CGroupAngleLifeObj();
	CGroupAngleLifeObj(CLDSGroupLineAngle* pAngle,long version=0,long doc_type=0);
	~CGroupAngleLifeObj();
};
//底脚板
class CFoot : public CLDSPart
{
public:
	ATOM_LIST<THANDLE> *plank_list;
	THANDLE attach_jg_handle[2];		//所依附基础角钢的句柄
	THANDLE xie_jg_handle[4];		//所基础角钢上所连接的两根斜材的句柄
	THANDLE base_node_handle;		//附属节点句柄
	short int A;					//靴板前侧台阶高
	short int B;					//靴板顶边距角钢外皮的边距
	short int C;					//基础角钢底端最低点距底板间距
	BOOL joint_in_zhun;			//斜材交于主材准线,否则交于外皮
	BOOL lj_by_weld;				//靴板角钢连接类型：TRUE:对焊 FALSE:螺栓连接
public:
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer);
	void ToXmlFile(FILE* fp,DWORD schema);
	CFoot();
	~CFoot();
	void Create3dFoot();
	void DesignXieJg(f3dPoint norm[]);
	BOOL CreateRefAxis();	//根据靴板结构得到相对坐标系

	CLDSPlate* GetFirstPlank();
	CLDSPlate* GetNextPlank();
	CLDSPlate* AppendPlank();
	void PushPlankPos(){if(plank_list) plank_list->push_stack();}
	void PopPlankPos(){if(plank_list) plank_list->pop_stack();}
	BOOL DeletePlank(long ii){return plank_list->DeleteAt(ii);}
	int GetPlankCount(){return plank_list->GetNodeNum();}
};

class CWeldPartView
{
public:
	UCS_STRU ucs;
	char sName[50];
	struct PART_ITEM{long hSonPart;BOOL bSectionView;};
	ATOM_LIST<PART_ITEM> SonPart;
	CWeldPartView()	{	strcpy(sName,"");	}
};
//焊接组合件
class CWeldPart : public CLDSDbObject
{
public:
	void FromBuffer(CBuffer &buffer,long version,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer);
	void ToXmlFile(FILE* fp,DWORD schema);
	SEGI iSeg;	//所从属的段号
	ULONG  m_uStatMatCoef;		//统材系数
	char sPartNo[MAX_CHAR_GUIGE_16+1],sName[MAX_TEMP_CHAR_50+1];
	struct PART_ITEM{long hSonPart; long num;};
	ATOM_LIST<PART_ITEM> SonPart;
	ATOM_LIST<CWeldPartView> SonView;
	CWeldPart()
	{
		m_nClassTypeId = CLS_WELDPART;
		//strcpy(m_sClassName,"CWeldPart");
		m_uStatMatCoef = 1;
		strcpy(sPartNo,"");
		strcpy(sName,"");
	}
	~CWeldPart()
	{
		SonPart.Empty();
		SonView.Empty();
	}
};
typedef CLdsList<CLDSPolyPart>POLYPARTLIST;
#endif
#endif // !defined(AFX_TDA_CO_PART_H__415A1A1F_534B_4ACC_B588_13AA8178D691__INCLUDED_)
