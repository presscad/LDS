//Translated by hxr 2014-12-8
// Tower.h: interface for the CTower class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOWER_H__06FC49B6_50F1_4315_A1B2_BAFF7A74AA17__INCLUDED_)
#define AFX_TOWER_H__06FC49B6_50F1_4315_A1B2_BAFF7A74AA17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdlib.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "list.h"
#include "HashTable.h"
#include ".\Buffer.h"
#include "lds_part.h"
#include "DefinedView.h"
#include "lds_co_part.h"
#include "lds_list.h"
#include "ldsptr_list.h"
#include "Block.h"
#include "lslist.h"
#include "HoleMaker.h"
#include "CirBufferStack.h"
#include "ArrayList.h"
#include "Interpolator.h"
#include "FoundationParam.h"
#include "Catlog.h"
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
#include "Resolver.h"
#endif

template<class BASE_CLS> class CSuperSmartPtr
{
	BASE_CLS* m_pSrcPtr;
	BASE_CLS* ValidPointer()const{
		if(!m_pSrcPtr)
#ifdef	AFX_TARG_ENU_ENGLISH
			throw "Encounter an pointer error";
#else
			throw "指针遭遇失败";
#endif
		return (BASE_CLS*)m_pSrcPtr;
	};
public:
	CSuperSmartPtr(BASE_CLS* pSrcPtr=NULL){m_pSrcPtr = pSrcPtr;}
	~CSuperSmartPtr(){;}
	bool IsHasPtr()
	{
		if(m_pSrcPtr!=NULL)
			return TRUE;
		else 
			return FALSE;
	}
	bool IsNULL()
	{
		if(m_pSrcPtr==NULL)
			return TRUE;
		else 
			return FALSE;
	}
	__declspec(property(get=IsHasPtr)) bool blValid;
	BOOL IsEqualPtr(const void* rPtr)
	{
		if(m_pSrcPtr==rPtr) return TRUE;
		else return FALSE;
	}
	BOOL operator == (CSuperSmartPtr& rPtr)
	{	return m_pSrcPtr==rPtr.LinePartPointer();}
	CSuperSmartPtr& operator = (CSuperSmartPtr& rPtr)
	{
		m_pSrcPtr = rPtr;
		return *this;
	}
	operator BASE_CLS*(){return ValidPointer();}
	CSuperSmartPtr& operator = (BASE_CLS* rPtr)
	{
		m_pSrcPtr = rPtr;
		return *this;
	}
	BASE_CLS* operator->()const{return ValidPointer();}
	CLDSPart *PartPointer(){
		if(!ValidPointer()->IsPart())
		{
			char error[100];
#ifdef  AFX_TARG_ENU_ENGLISH
			sprintf(error,"0X%X type cast error!",m_pSrcPtr->handle);
#else
			sprintf(error,"0X%X类型转换失败!",m_pSrcPtr->handle);
#endif
			throw error;
		}
		return (CLDSPart*)ValidPointer();
	}
	__declspec( property(get=PartPointer)) CLDSPart* pPart;
	CLDSLinePart *RodPointer(){
		if(!ValidPointer()->IsLinePart())
		{
			char error[100];
#ifdef  AFX_TARG_ENU_ENGLISH
			sprintf(error,"0X%X type cast error!",m_pSrcPtr->handle);
#else
			sprintf(error,"0X%X类型转换失败!",m_pSrcPtr->handle);
#endif
			throw error;
		}
		return (CLDSLinePart*)ValidPointer();
	}
	CLDSLinePart *LinePartPointer(){return RodPointer();}
	__declspec( property(get=RodPointer)) CLDSLinePart* pRod;

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLDSLineAngle *LineAnglePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_LINEANGLE&&m_pSrcPtr->GetClassTypeId()!=CLS_GROUPLINEANGLE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSLineAngle*)ValidPointer();
	}
	__declspec( property(get=LineAnglePointer)) CLDSLineAngle* pAngle;
	CLDSGroupLineAngle *GroupAnglePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_GROUPLINEANGLE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSGroupLineAngle*)ValidPointer();
	}
	__declspec( property(get=GroupAnglePointer)) CLDSGroupLineAngle* pGroupAngle;

	CLDSLineTube *LineTubePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_LINETUBE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSLineTube*)ValidPointer();
	}
	__declspec( property(get=LineTubePointer)) CLDSLineTube* pTube;
#ifdef __GGG_
	CLDSConeTube *ConeTubePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_CONETUBE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSConeTube*)ValidPointer();
	}
#endif
	CLDSGeneralPlate *GeneralPlatePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_PLATE&&m_pSrcPtr->GetClassTypeId()!=CLS_PARAMPLATE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSGeneralPlate*)ValidPointer();
	}
	__declspec( property(get=GeneralPlatePointer)) CLDSGeneralPlate* pBasicPlate;

	CLDSPlate *PlatePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_PLATE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSPlate*)ValidPointer();
	}
	__declspec( property(get=PlatePointer)) CLDSPlate* pPlate;

	CLDSParamPlate *ParamPlatePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_PARAMPLATE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSParamPlate*)ValidPointer();
	}
	__declspec( property(get=ParamPlatePointer)) CLDSParamPlate* pParamPlate;

	CLDSBolt *BoltPointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_BOLT)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSBolt*)ValidPointer();
	}
	__declspec( property(get=BoltPointer)) CLDSBolt* pBolt;

	CLDSLineSlot *LineSlotPointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_LINESLOT)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSLineSlot*)ValidPointer();
	}
	__declspec( property(get=LineSlotPointer)) CLDSLineSlot* pSlot;

	CLDSLineFlat *LineFlatPointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_LINEFLAT)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSLineFlat*)ValidPointer();
	}
	__declspec( property(get=LineFlatPointer)) CLDSLineFlat* pFlat;

	CLDSArcPart *ArcPartPointer(){
		if(!ValidPointer()->IsArcPart())
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSArcPart*)ValidPointer();
	}
	__declspec( property(get=ArcPartPointer)) CLDSArcPart* pArcRod;

	CLDSArcAngle *ArcAnglePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_ARCANGLE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSArcAngle*)ValidPointer();
	}
	__declspec( property(get=ArcAnglePointer)) CLDSArcAngle* pArcAngle;

	CLDSArcSlot *ArcSlotPointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_ARCSLOT)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSArcSlot*)ValidPointer();
	}
	__declspec( property(get=ArcSlotPointer)) CLDSArcSlot* pArcSlot;

	CLDSArcFlat *ArcFlatPointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_ARCFLAT)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSArcFlat*)ValidPointer();
	}
	__declspec( property(get=ArcFlatPointer)) CLDSArcFlat* pArcFlat;

	CLDSSphere *SpherePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_SPHERE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSSphere*)ValidPointer();
	}
#endif
	__declspec( property(get=SpherePointer)) CLDSSphere* pSphere;

	CLDSNode *NodePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_NODE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "类型转换失败!";
#endif
		return (CLDSNode*)ValidPointer();
	}
	__declspec( property(get=NodePointer)) CLDSNode* pNode;

};
#define SmartPartPtr CSuperSmartPtr<CLDSPart>
#define SmartPlatePtr CSuperSmartPtr<CLDSGeneralPlate>
#define SmartArcPtr CSuperSmartPtr<CLDSArcPart>
#define SmartRodPtr CSuperSmartPtr<CLDSLinePart>
//风荷载结构
typedef struct tagWIND_LOAD
{
	//X/Y/Z方向的风荷载或换算系数；自动计算风荷载时分别表示风速、风压调整系数和风向角
	double velocity;	//风速
	long  azimuth;		//风向方位角（°），以线路方向为准（即Y轴正向）
}WIND_LOAD;
//节点集中荷载,　转自lds_part.h wjh-2014.4.27
class CExternalNodeLoad
{
public:
	double permanentFz;	//重力方向的准永久外荷载值(N)
	GEPOINT Fxyz;		//三个坐标轴方向的集中可变外荷载值(N)
	GEPOINT designLoad;	//荷载设计值，主用于杆塔设计计算（包含结构重要性系数在内的所有系数），将来用其取代Fxyz及permanentZ
	GEPOINT stdLoad;	//荷载标准值，主用于基础作用力计算（包含结构重要性系数在内的所有系数）
	CExternalNodeLoad(){permanentFz=0.0;}
};
class CTower;
struct SUB_OBJ{
public:
	BYTE ciSubType;	//子对象类型IDbEntity::GetDbEntType()或其它
	long iSubId;	//在组缓存内（可能以哈希表形式存在）的标识号,一般在向哈希表增加元素时的初始化函数中进行赋值
	long hObj;		//上级对象的标识号或构件孔壁时的原始螺栓
	void* pSubObj;		//关联的子对象
public:
	SUB_OBJ(){memset(this,0,sizeof(SUB_OBJ));}
	friend void LoadSubGeometryEnt(SUB_OBJ *pSubObj,DWORD serial);
	long SubId(){return iSubId;}
	void* SubObj(IModel* pModel);
};
struct PROP_OBJ{
public:
	static const BYTE TYPE_LDSOBJ	= 0;
	static const BYTE TYPE_DBENTITY	= 1;
	static const BYTE TYPE_HOLEWALL	= 2;
	static const BYTE TYPE_WIREPOINTARROW	= 3;
	BYTE ciObjType;	//0.普通LDS数据对象;1.线框图元，来源于图纸图元IDbEntity;2.构件上孔壁实体
	UINT uiSolidId;	//图形引擎中显示实体的内部标识Id
public:
	union{
		CLDSObject* pLdsObj;
		struct{
			CLDSBolt* pBolt;		//关联的子对象
			long hResidePart;	//孔壁的宿主构件
		}hole;
		struct SUB_OBJ{
			BYTE ciSubType;	//子对象类型IDbEntity::GetDbEntType()或其它
			long hObj;		//上级对象的标识号或构件孔壁时的原始螺栓
			long iSubId;	//在组缓存内（可能以哈希表形式存在）的标识号,一般在向哈希表增加元素时的初始化函数中进行赋值
			void* pSubObj;	//关联的子对象
		}subobj;
	}obj;
public:
	PROP_OBJ(CLDSObject* pObj=NULL);
	PROP_OBJ(CLDSBolt* pBolt,long hResidePart);
	void SetKey(DWORD key){uiSolidId=key;}
	//LDS对象
	CLDSObject *set_LdsObject(CLDSObject* pPropObj);
	CLDSObject *get_LdsObject();
	__declspec(property(put=set_LdsObject,get=get_LdsObject)) CLDSObject* pObj;
	//LDS对象上的线框图元子对象属性
	long get_hObject();
	__declspec(property(get=get_hObject)) long hObj;
	long SubId();
	__declspec(property(get=SubId)) long liSubId;
	void* SubObj();
	__declspec(property(get=SubObj)) void* pSubObj;
	//构件上的孔壁实体复合对象
	CLDSBolt* set_pBolt(CLDSBolt* pHoleBolt);
	CLDSBolt* get_pBolt();
	__declspec(property(put=set_pBolt,get=get_pBolt)) CLDSBolt* pBolt;
	long set_hResidePart(long hPart);
	long get_hResidePart();
	__declspec(property(put=set_hResidePart,get=get_hResidePart)) long hResidePart;
};
class CWorkStatus : public CLDSObject
{
	//以下荷载设计值取值系数可能会因为用户设定遵循规范的不同自动取值，故应在取值类中综合考虑后取值
	//此处设取值类为友元类
	struct DESIGN_COEF_ICE
	{
		//覆冰后风荷载的增大系数，一般根据覆冰厚度而定5mm取1.1,10mm取1.2,15mm 冰区取1.6，20mm 取1.8，20mm 以上冰区取2.0～2.5
		double B_I;
		double gamaQ;	//可变荷载分项系数
		double Kesai;	//ψ可变荷载组合系数
	}coefIce;	//覆冰荷载及电力规范的其余可变荷载的荷载设计取值系数
	struct DESIGN_COEF_WIND
	{
		double gamaQ;	//可变荷载分项系数
		double Kesai;	//ψ可变荷载组合系数
	}coefWind,coefAn,coefU,coefT;	//风荷载、安装荷载、断线及不均匀覆冰导致的不衡力荷载、温度作用荷载的荷载设计取值系数
	double gamaEh,gamaEv;	//水平、竖向地震作用荷载的分项系数
	double GamaQi;	//第i项可变荷载的分项系数，应取1.4
	/*可变荷载的组合系数:
	各级电压线路的正常运行情况，应取fai=1.0;220V及以上送电线路的断线情况和各及
	电压线路的安装情况，应取fai=0.9;各级电压线路的验算情况和110V线路的断线情况，
	应取fai=0.75。
	*/
	double Fai;
	int m_iEiffelCaseNo;	//临时变量
public:
	CWorkStatus();
	~CWorkStatus();
	CTower* SuperiorTower();
	void FromRawBuffer(CBuffer &buffer,long version=0);	//用于从荷载计算程序导入数据块
	void ToRawBuffer(CBuffer &buffer,long version=0,long doc_type=0);	//用于向荷载计算程序导出数据块
	void FromBasicCoefBuffer(CBuffer &buffer);	//用于工况间拷贝各项系数及基本属性
	void ToBasicCoefBuffer(CBuffer &buffer);	//用于工况间拷贝各项系数及基本属性
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tsa3_lds4_default3=3);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void Serialize(CArchive &ar); 
	int SetEiffelCaseNo(int iCaseNo);
	int GetEiffelCaseNo();
	CXhChar16 ToCaseNoString();
	long GetEiffelStatusId();	//返回包括埃菲尔效应工况号的组合Id
	long GetEiffelStatusNo();	//返回包括埃菲尔效应工况号的组合序号
	//荷载计算系数
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	double SetBw_I(double Bw_I);		//覆冰时杆塔风荷载增大系数
	double SetGamaQ(double GamaQ);		//风荷载分项系数
	double SetGamaQ_W(double GamaQ_W);	//风荷载分项系数
	double SetGamaQ_I(double GamaQ_I);	//覆冰荷载分项系数
	double SetGamaQ_A(double GamaQ_A);	//安装荷载分项系数
	double SetGamaQ_U(double GamaQ_U);	//断线、不均匀覆冰荷载分项系数
	double SetGamaQ_T(double GamaQ_T);	//温度荷载分项系数
	double SetGamaQ_Eh(double GamaQ_Eh);	//水平向地震荷载分项系数
	double SetGamaQ_Ev(double GamaQ_Ev);	//竖向地震荷载分项系数
	double SetKesai(double Kesai_W);	//风荷载组合系数
	double SetKesai_W(double Kesai_W);	//风荷载组合系数
	double SetKesai_I(double Kesai_I);	//覆冰荷载组合系数
	double SetKesai_A(double Kesai_A);	//安装荷载组合系数
	double SetKesai_U(double Kesai_U);	//断线、不均匀覆冰荷载组合系数
	double SetKesai_T(double Kesai_T);	//温度荷载组合系数
	double Bw_I();		//覆冰时杆塔风荷载增大系数
	double GamaQ();		//导地线张力荷载分项系数
	double GamaQ_W();	//风荷载分项系数
	double GamaQ_I();	//覆冰荷载分项系数
	double GamaQ_A();	//安装荷载分项系数
	double GamaQ_U();	//断线、不均匀覆冰荷载分项系数
	double GamaQ_T();	//温度荷载分项系数
	double GamaQ_Eh();	//水平向地震荷载分项系数
	double GamaQ_Ev();	//竖向地震荷载分项系数
	double Kesai();		//风荷载组合系数
	double Kesai_W();	//风荷载组合系数
	double Kesai_I();	//覆冰荷载组合系数
	double Kesai_A();	//安装荷载组合系数
	double Kesai_U();	//断线、不均匀覆冰荷载组合系数
	double Kesai_T();	//温度荷载组合系数
#endif
public:
	struct WINDLOAD_REFPARAM{
		double shieldCoef;
		double attachMass;				//附加设施质量等效到每个承力节点上的重力荷载(kg)
		//附加风荷载在节点上产生的风荷载(考虑AttachWindLoad.m_fBetaZ,但未考虑风压高度变化系数MuZ),单位N。
		//临时数据不存储到文件中
		f3dPoint basicAttachWindLoadF;
		WINDLOAD_REFPARAM(){shieldCoef=1.0;attachMass=0;}
	};
	CHashList<WINDLOAD_REFPARAM> AttachWindLoad;	//附加风荷载的挡风系数列表，key值即是附加风荷载的句柄值
	CHashList<CExternalNodeLoad>hashNodeLoad;		//存储荷载节点,key值为节点句柄
	long iNo;
	long iWireLayoutNo;	//架线方案编号
	char description[MAX_TEMP_CHAR_100+1];		//对于工况的简要描述
	char limit_str[MAX_TEMP_CHAR_100+1];		//从属模型描述字符串
	BYTE m_cMainLoadType;	//0:大风1:覆冰2:低温3:断线及不均匀覆冰4:安装5:水平地震6:竖向地震7:仅水平地震8:仅竖向地震
	bool m_bCheckWorkCond;	//验算工况
	bool m_bSpecLoadFactors;//指定各项荷载系数（γG,γQ及ψ），但不含γo
	bool m_bLoadIncFactor;	//直接应用包含有各项系数（γo，γG,γQ及ψ）的荷载设计值CExternalNodeLoad::designLoad
	BYTE m_cHoriEarthQuakeAxis;	//水平地震验算时的振动方向角°，'X'=0或'Y'=90,ASCII码值'X':88，'Y':89
	char vibration_mode_str[MAX_TEMP_CHAR_50+1];	//振型分解反应谱法分析时需要考虑的振型列表

	double ice_thick;	//基本裹冰厚度(mm)
	WIND_LOAD WindLoad;	//风荷载
	//属性栏
	DECLARE_PROP_FUNC(CWorkStatus);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);
#if defined(__PART_DESIGN_INC_)&&!defined(__DRAWING_CONTEXT_)&&!defined(__IGNORE_VERSION_COMPARE_)
	bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents);
#endif
};
//
struct MODULE_LEG : public ICompareClass {
private:
	UINT m_uiLegSerial;
public:
	BOOL valid;
	double m_fLowestZ;
	CLDSLinePart* pLegPostRod;
	CFGWORD cfgword;
	MODULE_LEG(UINT uiLegSerial = 1) {
		valid = false;
		m_fLowestZ = 0;
		pLegPostRod = NULL;
		m_uiLegSerial = uiLegSerial;
	}
	UINT get_uiLegSerial(){return m_uiLegSerial;}
	__declspec(property(get=get_uiLegSerial)) UINT uiLegSerial;
	virtual int Compare(const ICompareClass *pCompareObj)const
	{	//接腿默认由高到低排序
		MODULE_LEG* pSubLeg = (MODULE_LEG*)pCompareObj;
		if (m_fLowestZ > pSubLeg->m_fLowestZ)
			return -1;
		else if (m_fLowestZ < pSubLeg->m_fLowestZ)
			return 1;
		else
			return 0;
	}
};
//各项荷载设计系数的提取类

int GetMinNo(DWORD dwWord);
int GetMaxNo(DWORD dwWord);
typedef void* MODULE_LEGVOIDPTR;
class CLDSModule : public CLDSObject
{
	//ATOM_LIST<long> *AttachWindLoad;	//内力分析
	double lowest_z;
	WORD m_iBodyNo;			//塔主体(身&头部)所对应的配材（接腿）号
public:
	CLDSModule();
	CTower* SuperiorTower();
	CFGWORD GetBodyCfgWord();			//配材号m_iBodyLegNo的另一种表述方式
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromCdfBuffer(CBuffer &buffer,long version=0);
	void ToCdfBuffer(CBuffer &buffer,long version=0);	
	void ToXmlFile(FILE* fp,DWORD schema);
	//void Serialize(CArchive &ar);
	//只生成符合本计算模型条件的节点和杆件集合,不负责清空输入的节点集和杆件集
	void RefreshModule(NODESET &nodeset,LINEPARTSET &poleset,BOOL bVerifyLeg=FALSE);
#ifdef __PART_DESIGN_INC_
	bool SubBlockCheck(NODESET &boundary,CXhChar50* error_info);
#endif
public:
	//返回参数最低的基础Z坐标（按照杆件的最低点计算）
	//返回参数最高的头部Z坐标（按照杆件的最高点计算）
	//返回全塔的总高度(mm)
	double GetModuleScopeZ(double *highest=NULL,double *lowest=NULL);
	double LowestZ(){return lowest_z;}
	BOOL IsSonPart(CLDSDbObject *pObj,CFGWORD *pSpecCfgWord=NULL,bool bVerityActLeg=true);
	WORD SetBodyNo(WORD iBodyNo);
	WORD GetBodyNo(bool bVerityValid=true) const;	//单提为一函数，是为防止本体号超出1~24界限导致应用时数组越界 wjh-2013.4.12
	WORD get_idBodyNo(){return GetBodyNo();}
	__declspec(property(get=get_idBodyNo)) WORD idBodyNo;
public:
	bool m_bUserSpecifyLowestZ;//false:表示系统自动计算呼高Z0基准平面;true:根据用户指定值计算
	double m_fNamedHeight;			//指定杆塔呼高计算时呼高0基准平面的Z坐标值
	double set_NamedHeight(double fNamedHeight);
	double get_NamedHeight();
	__declspec(property(put=set_NamedHeight,get=get_NamedHeight)) double NamedHeight;
	bool m_bSpecifyLevelheightDiff;
	WORD m_wnLevelHeight;			//呼高各子接腿层级间高度差值(mm)
	UINT set_uiLevelHeight(UINT uiLevelHeightDiff);
	UINT get_uiLevelHeight();
	__declspec(property(put=set_uiLevelHeight,get=get_uiLevelHeight)) UINT uiLevelHeight;
	GEPOINT xarrBaseLocations[24];		//每呼高中最多容纳24个腿，数组中存储每个腿的基础顶面中心点位置（导出TID时临时更新存储）
	bool UpdateFoundationLocations();	//导出TID时用此函数临时更新上面的xarrBaseLocations属性 wjh-2019.3.31
	GEPOINT GetFoundationLocation(BYTE ciLegSerial=1);	//取值1~24，表示接腿号
	UINT UpdateValidLegs(MODULE_LEGVOIDPTR parrSortedLegs=NULL,UINT uiMaxCount=16);
public:
	DWORD ActiveQuadLegDword();
	short GetCfgWordLegCount();	//当前呼高m_dwLegCfgWord属性中包含的接腿数量
	//根据当前呼高内的接腿序号（1为基数,1~24）转换为全模型的配材序号(1~192) wjh-2019.7.22
	WORD  ToLegSerialBySerialInHeight(int serialInHeight);
	short GetLegCount();	//统计当前呼高组包含的实际有效接腿数量 wjh-2016.6.14
	long iNo;
	char description[MAX_TEMP_CHAR_50+1];		//对于计算模型的简要描述
	char segStr[MAX_TEMP_CHAR_200+1];			//本呼高所包括的段号字符串
	double land_high;				//地形高(m)，用以计算杆塔风压高度变化系数
	CFGWORD m_dwLegCfgWord;		//接腿配材号
	WORD m_arrActiveQuadLegNo[4];	//本模型当前第一～四象限的当前接腿号
	double m_fBurglarLsStartZ;	//防盗螺栓范围起始Z坐标
	double m_fBurglarLsEndZ;	//防盗螺栓范围终止Z坐标
	double m_fLockPadStartZ;	//防松垫圈范围起始Z坐标  wht 09-12-22
	double m_fLockPadEndZ;		//防松垫圈范围终止Z坐标
};
#ifdef __PART_DESIGN_INC_
struct FREEDOMGROUP
{
	long hNode;	//节点句柄
	int dimension;
	double v[6];
};
class CVibrationMode{
public:
	CVibrationMode();
	~CVibrationMode();
	double frequency;	//频率
	FREEDOMGROUP *freedom_arr;
};
//输出动力特性分析结果数据
struct DYNAMIC_ANAL_NODE
{
	DWORD  id;		//节点标识，临时存储节点指针、编号或句柄
	double mass;	//对应的等效节点质量(kg)
};
#endif
class CConnectFL
{
	BYTE condFlag;	//0x01指定法兰类型;0x02指定螺栓圆;0x04指定螺栓直径;0x08指定螺栓数量
public:
	CConnectFL(){memset(this,0,sizeof(CConnectFL));}
	double m_fThickTube;			//主管壁厚(mm)
	double m_fInnerRadius;			//基准法兰内环半径(mm)
	double m_fCoupleInnerRadius;	//配对法兰内环半径(mm)
	char bolt_grade[9];		//螺栓级别
	long bolt_d;			//螺栓直径（规格）
	long bolt_n;			//螺栓组中的螺栓数量
	double N,M;				//N 轴向力(N),M弯矩(N*m)
	double m_fBoltRadius;	//螺栓圆半径(mm)
	long m_nThickFL;		//法兰盘厚度(mm)
	long m_nWidthFL;		//法兰环形带宽(mm)
	long m_nThickRib;		//肋板厚度(mm),厚度为0表示无加劲肋
	long m_nHeightRib;		//肋板高度(mm)
	long m_nRibS;			//肋板焊缝切角高(宽)度
	long m_nCoupleRibS;		//配对肋板焊缝切角高(宽)度
	long m_nCoupleThickFL;	//法兰盘厚度(mm)
	long m_nCoupleHeightRib;//配对肋板高度(mm)
	char cMaterial;			//同时表示法兰板及加劲肋板的材质，目前仅支持法兰与肋板同材质的设计
	BYTE cFLType;			//0.普通法兰；1.平焊法兰；2.对焊法兰
	char m_sPartNo[16];     //构件编号
	double SingleWeight();	//单个法兰重
	double SumWeight();		//成对法兰重
	void Optimize(BYTE flag);
	BOOL IsEqual(CConnectFL flConnectStru,BOOL isNear = FALSE);//isNear=trues时是相近法兰判断
};
struct ANGLE_JOINT
{
private:
	DWORD _id;
public:
	//0.内包铁外包钢板；1.外包铁；2.内外包铁；'A'.组合角钢接头A；'B'.组合角钢接头B；'C'组合角钢接头C；'D'.组合角钢接头D
	BYTE cJointType;
	long hNode;				//接头关联节点
	long hSmallRod,hBigRod;	//基准杆件(连接的正头杆件)与共线对接的杆件
	struct BOLT{
		BYTE rows;	//螺栓排数
		BYTE n;		//螺栓数量
		BYTE grade;	//螺栓级别,48: 4.8级；68:6.8级；88:8.8级；109: 10.9级
		BYTE d;		//螺栓直径
		double holeout_coef;//减孔系数
	}bolt;
	SIZE_WTM innerAngle;	//内包铁肢宽肢厚及材质
	union OUTER{
		SIZE_WTM plate;	//外包钢板宽厚及材质
		SIZE_WTM angle;	//外包铁肢宽肢厚及材质
	}outer;
	SIZE_WTM smallAngle;	//与hSmallRod对应的角钢规格
	SIZE_WTM bigAngle;		//与hBigRod对应角钢规格
	MIRMSG mirinfo;			//接头对称信息
	ANGLE_JOINT();
	~ANGLE_JOINT();
	DWORD Id(){return _id;}
	DWORD InitId(DWORD id);
	ANGLE_JOINT operator =(ANGLE_JOINT& joint);
	bool VerifyLinkAngles();
	int  TestCalMinBoltN();	//计算最少需要的螺栓数
	bool DesignCheckAreaAndBoltForceScale();	//计算结果存在标签当中
	CXhChar50 GetJointNo();
	CXhChar100 GetBoltInfo(bool bAlignRowCount=TRUE);
	CXhChar100 GetInnerPartSpec(bool bIncAngleType=FALSE);
	CXhChar100 GetOutterPartSpec(bool bIncAngleType=FALSE);
public:	//附加便签属性及方法（便于设计的临时统计系数，不用存储）
	struct ITEM_TAG_INFO : public TAG_INFO<ANGLE_JOINT>{
	public:
		CLDSNode* pNode;
		CLDSLineAngle* pSmallAngle,*pBigAngle;
		double scaleAreaJ2A1;	//接头与被连接小角钢的截面积之比
		double scaleAreaJ2A2;	//接头与被连接大角钢的截面积之比
		double scaleForceB2A1;	//螺栓与被连接小角钢的承载内力之比
		double scaleForceB2A2;	//螺栓与被连接大角钢的承载内力之比
	public:
		ITEM_TAG_INFO();
		virtual ~ITEM_TAG_INFO(){DetachObject();}
	}*pTagInfo; //临时使用不存储，可省去每次遍历构件链表时重复查找当前构件指针 wht 09-12-12
	void SetTagInfo(ITAG* pTag){pTagInfo=(ITEM_TAG_INFO*)pTag;}
	//当PART_ITEM已经有挂接对象时执行TagInfo(...), 否则创建新挂接数据对象后返回
	ITEM_TAG_INFO* LoadTagInfo();
public:
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
private:
	static ATOM_LIST<ITEM_TAG_INFO> listTagInfo;	//临时挂接的附加到构件项{ANGLE_JOINT}上的数据对象链表
};
ANGLE_JOINT::ITEM_TAG_INFO* TagInfo(ANGLE_JOINT* pItem);

typedef struct BOLT_SELCOND{
	long d;			//适用螺栓标称直径
	char grade[8];	//适用螺栓等级
	char matStr[8];	//由材质标识符组成的适用连接件材质范置
	long upWidthLim;//≤肢宽分界上限
	BOLT_SELCOND(){memset(this,0,sizeof(BOLT_SELCOND));d=16;strcpy(grade,"6.8");strcpy(matStr,"*");upWidthLim=56;}
}*BOLT_SELCONDPTR;
class CTowerArchiveLibrary
{
public:
	ARRAY_LIST<ANGLE_G_REC>AngleG;
	ARRAY_LIST<STEELMAT>SteelMaterial;
	ARRAY_LIST<JG_PARA_TYPE>AngleSize;
	ARRAY_LIST<TUBE_PARA_TYPE>TubeSize;
	ARRAY_LIST<LSSPACEEX>BoltSpace;
	//CHashList<CLsFamily> BoltLib;	//力学计算与螺栓规格无关不需要添加此项日志
	//钢管塔的标准件库
	int rollend_param_N,uend_param_N,xend_param_N,fl_couple_rule_N;
	int fld_param_N,flp_param_N,flg_param_N,flr_param_N;
	ARRAY_LIST<INSERT_PLATE_PARAM> rollend_param_table;//[200]
	ARRAY_LIST<INSERT_PLATE_PARAM> uend_param_table;//[200]
	ARRAY_LIST<INSERT_PLATE_PARAM> xend_param_table;//[200]
	ARRAY_LIST<FL_COUPLE_RULE> fl_couple_rule_table;//[200]
	ARRAY_LIST<FL_PARAM> fld_param_table;//[200]
	ARRAY_LIST<FL_PARAM> flp_param_table;//[200]
	ARRAY_LIST<FL_PARAM2> flg_param_tbl;//[200]
	ARRAY_LIST<FL_PARAM2> flr_param_tbl;//[200]
	CTowerArchiveLibrary(){
		rollend_param_N=uend_param_N=xend_param_N=fl_couple_rule_N=0;
		fld_param_N=flp_param_N=flg_param_N=flr_param_N=0;
	}
};
class COldVerDataFile{
public:
	struct GROUP_ANGLE{
		long hGroupAngle;
		WORD wiSectStyle;	//0.对角双拼;1.T型双拼
		GROUP_ANGLE(){hGroupAngle=0;wiSectStyle=1;}
		void SetKey(DWORD key){hGroupAngle=key;}
	};
	struct SIMPLE_DATUMPOINT{
		long hNodeOrRod;
		long hNewTmpDatumPoint;
		char ciHidType;	//handle含义0.CLDSPoint.handle;'1':CLDSNode.handle;'S':CLDSLinePart.handle->PosStart;'E':CLDSLinePart.handle->PosEnd
		SIMPLE_DATUMPOINT(long _hNode=0,long _hNewDatumPoint=0);
		void SetKey(DWORD key){hNodeOrRod=key;}
	};
	CHashListEx<GROUP_ANGLE> hashGroupAngles;			//需要把ciMasterIndex由1->0转换的组合角钢
	CHashListEx<SIMPLE_DATUMPOINT> hashTmpDatumPoints;	//直接通过节点定义的基准面，降版时应临时转为参数化点
	void Empty();
	char SafeAngleWing(long hGroupAngle,char cAngleWing,long doc_type,long version);
};
//typedef BOLT_SELCOND* BOLT_SELCONDPTR;
//from LoadLibrary("WireLayoutEvolver.dll")
typedef bool  (*DefWriteWeatherFunc)(char* weather_buf,unsigned long size);
typedef unsigned long (*DefReadWireLayoutDataFunc)(char* pBuff,unsigned long dwBufSize,long version,long doc_type);
//返回从pBuffer中成功读取的字节数
DWORD ReadWireLayoutData(char* pBuff,DWORD dwBufSize,long version=0,long doc_type=0);
class CBriefWeldPart;
class CUniWireModel;
class CExterDataModel;	//外挂数据模型与CTower共用全局唯一句柄标识
#ifdef __LDS_CONTEXT_
struct IMemberDesignCode;
#endif
class CTower : public IModel
{
	long m_nFileDivisionPos;
	bool m_bInitNewObjDefaultInfo;
	CParaCS defaultAcsMirX,defaultAcsMirY,defaultAcsMirZ;
	CFGWORD cfglegMap[160];	//用于旧版配材号升迁变换的映射表
public:
	static double W0_EIFFEL_FACTOR;	//塔身风埃菲尔效应风压折减系数,暂按规范确定
	CHashListEx<CParaCS> Acs;			//以坐标系id为键值存储坐标系
	CTowerArchiveLibrary archiveLibrary;//仅为版本比较时存档备用各准数据表
	CExterDataModel* m_pExterModel;
	ATOM_LIST<CLDSParamPlate> localStdPartLibray;	//本地零件库
	CTower();
	virtual ~CTower();
	long hTower;
	double m_fTowerHeight;	//全塔高单位(m)
	ATOM_LIST<BOLT_SELCOND>listSelBoltCond;	//螺栓选择条件集合
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	struct{
		BYTE grade;			//地震烈度6,7,8,9
		double alfa_max;	//水平地震影响系数最大值
		BYTE cLandType;		//场地类别1,2,3,4
		BYTE cDesignGroup;	//设计地震分组1,2,3
		int nFreqsRank;		//动力特性分析时求解振型阶数，即求解前nFreqsRank阶振型，最大值不超过8;
		double Tg;			//特征周期
		double zeta;		//阻尼比ζ=0.02s
		double gamaRE;		//承载力抗震调整系数,一般跨越塔取0.85，其余塔取0.8
		double alfa[16];	//抗震验算时各振型水平地震影响系数
		VIBRAMODE_PROP modes_arr[16];	//最大容许存储前16阶振型的频率
	}EarthQuake;
#endif
public:
	bool m_bUserSpecifyNamedHeightZeroZ;//false:表示系统自动计算呼高Z0基准平面;true:根据用户指定值计算
	double m_fNamedHeightZeroZ;			//指定杆塔呼高计算时呼高0基准平面的Z坐标值
	double set_NamedHeightZeroZ(double fNamedHeightZeroZ);
	double get_NamedHeightZeroZ();
	__declspec(property(put=set_NamedHeightZeroZ,get=get_NamedHeightZeroZ)) double NamedHeightZeroZ;
	double _IntelliCalNamedHeightZeroZ(bool *pblCalSuccessed=NULL);
public:
	UCS_STRU ucs;
	CLdsPartList Parts;					//各种型钢制成的铁塔构件，如角钢、钢板、钢管、槽钢、螺栓等
	CHashListEx<CONSTRAINT>hashConstraints;
	CXhSimpleList<CATEGORY> listParamCategory;	//各系统全局参数在属性栏显示时的目录项
	CHashList<DESIGN_PARAM_ITEM>hashParams;	//键为KEY4C类型，用以表示杆塔整体模型中的全局参数，可与hashSlaveNodes配合使用
	CHashListEx<CATLOG_ITEM> hashCatlogs;	//系统参数的目录项清单
	CHashListEx<SLAVE_NODE>hashSlaveNodes;	//键为(坐标)受控节点的句柄，值为节点的受控信息SLAVE_NODE对象
	CHashList<CKaoMeAngle>hashKaoMeAngles;	//组合角钢中间十字连接(填)板加工所需的靠模角钢集合
	CBlockList Block;
    CLdsList<CLDSNode>  Node;			//节点
	CLdsList<CLDSModule> Module;		//模型
	CLdsList<CLDSPoint> Point;			//基准点
	CLdsList<CLDSLine>  Line;			//基准线
	CLdsList<CLDSPlane> Plane;			//基准平面
	CLdsList<CLDSArcLift> ArcLift;		//圆弧预拱项
	CLdsList<CDisplayView> View;		//显示视图
	CLdsList<CBlockReference> BlockRef;
	FOUNDATION_PARA m_xFoundation;
	CLdsList<CLDSFoundation> Foundations;	//模拟基础实体
	CHashListEx<NODERELAINFO> hashNodeRelations;
	virtual void* Get_hashNodeRelations(){return &hashNodeRelations;}
	virtual NODERELAINFO* GetNodeRelaInfo(long hNode){return hashNodeRelations.GetValue(hNode);}
#ifdef __COMMON_PART_INC_
	CLdsList<CSupplementPart> SuppPart;	//附加构件
	CLdsList<CLDSPartGroup> PartGroup;
#endif
	const static int BOM_CONFIG = 1;	//智能绘图数据缓存
	const static int SMART_DRAW = 2;	//智能绘图数据缓存
	const static int TOWER_MODEL= 3;	//版本日志初始模型数据缓存
	const static int LOGGER		= 4;	//版本日志模式及内容数据缓存
	const static int CLIPPER	= 5;	//剪切板
	const static int WIRELOAD_MODEL = 6;//导地线荷载计算
	const static int RSLT_REPORT= 7;	//杆塔计算结果报告
	const static int PRJPARAMPART_LIBRAY=8;	//工程参数化零件库(主要指工程范围内相对的标准参数化板构件)
	const static int PPE_MODEL	= 9;	//工艺管理缓存 wht 16-03-18
	const static int BOM_COMPARE_MODEL=10;	//施工图图像信息及构件明细表信息, BOM对比数据
	const static int MODEL_SIZE_CHECK =11;	//三维模型的校审信息
	const static int TSA_TURBO_MODEL  =12;	//用于加速有限元计算及单元设计验算时的TSA模型缓存信息
	const static int UNI_WIREPOINT_MODEL  =13;	//挂点模型
	CHashPtrList<CAttachBuffer>AttachBuffer;	//附加缓存
#ifdef __PART_DESIGN_INC_
	CLdsList<CWorkStatus>WorkStatus;	//工作状况
	CLdsList<CAttachWindLoad>AttachWindLoad;	//附加风荷载
	CMapList<CElemInfo>Elems;			//含辅助材在内的所有单元集合,键值为起末端节点的句柄
	ARRAY_LIST<ELEMINFO_PTR>ElemPtrArr;	//与Elems同步，并辅助实现以CElemInfo::Id-1为索引主要用于图形显示及属性栏中标识索引
	//ATOM_LIST<CWireLayout>groupWireLayout; 移至WireLayoutsEvolvement.h(.cpp)中，wjh-2014.4.28
	ATOM_LIST<CWindSegment>WindLoadSeg;	//风压段信息
#endif
	ATOM_LIST<CConnectFL>hashConnectFl;
	CHashList<ANGLE_JOINT>hashAngleJoint;
	CHashList<SUB_OBJ>GeEnts;	//当前显示的可定位标识的几何图元集合
	CHashSet<CLDSObject*>hashObjs;//对象哈希表
	BOOL m_bCooperativeWork;
	char version[20];
	char segstr[MAX_TEMP_CHAR_200+1];
	CObjNoManager NoManager;
	//PARTSET DispPartSet;
	//NODESET DispNodeSet;
	//LINEPARTSET RodSet;
	long m_idServerFile;	//与TMS同步时对应的TMS数据库中的文件Id
	WORD m_wiVerSerial;		//放样过程中产生的TMS文件版本存储流水号(与数据格式无关) 默认值为0 wjh-2017.1.17
	THANDLE m_hActiveModule;	//当前活动塔模型
	THANDLE m_hActiveStatus;	//当前活动工况
	WORD m_arrActiveQuadLegNo[4];	//当前第一~四象限的当前接腿号
	CFGWORD cfgword;		//所有构件的总配材号
	THANDLE handle;				//当前铁塔中最大句柄
	BOOL m_bDataModified;		//数据修改标识
	WORD m_iCurLeg;				//当前显示的接腿（可能已无用）
	SEGI m_iCurSeg;				//当前操作的分段号(ACAD出图时会用)
	double m_fMaxXieJGLen;		//最大斜材长度(mm)
	BYTE m_cStructureType;		//结构类型，0：单塔身主体格构塔架(考虑埃菲尔风压效应)；1: 普通(变电)复合构架
	union{
		DWORD user_pipeline_no;		//用户流水号
		DWORD m_uiOriginalDogKey;	//文件最初创建时的加密锁号
	};
	DWORD m_uiLastSaveDogKey;	//文件最近一次存储使用的加密锁号
	CString m_sPrjCode;		//工程编号
	CString m_sCompanyName;		//设计单位
	char m_sPrjName[MAX_TEMP_CHAR_50+1];		//工程名称
	char m_sTaType [MAX_TEMP_CHAR_50+1];		//塔型
	char m_sTaAlias [MAX_TEMP_CHAR_50+1];		//代号
	char m_sTaStampNo [MAX_TEMP_CHAR_50+1];		//钢印号
	char m_sOperator [MAX_TEMP_CHAR_50+1];		//操作员（制表人）
	char m_sAuditor [MAX_TEMP_CHAR_50+1];		//审核人
	char m_sCritic [MAX_TEMP_CHAR_50+1];		//评审人
	int m_nTaNum;				//塔基数
	/** 按照TMAV5.1.1.0之前版本的模式进行到位计算 wjh-2016.4.28
		自TMAV5.1.1.0以来心线交点模式的CLDSPoint及具有自身工作肢法向偏移量的搭接角钢到位均发生了细微调整。
		为兼容新旧版本格式转换，特设此属性开关
	*/
	BYTE m_ciRepositionAlgor;//0.表示当前;1:V5.1.1之前;2.V5.1.7之前;其余数越大越新，m_bRepositionByModeBeforeV51;
#ifdef __COMMON_PART_INC_
	//钢板卷边处理方式     0.展开后倒角处为两条直线 
	//					   1.最少刀数保证卷边外侧边长度为L-N 
	//					   2.最少刀数保证卷边内侧边长度为L-N  wht 11-04-18
	//钢板只有0和1种， 插板有0、1、2三种
	BYTE m_ciRollProcessType;
	//钢板卷边线的计算方式 0--外侧(以前处理方式)
	//					   1--内侧(现在处理方式)
	BYTE m_ciRollOffsetType;	//卷边偏移方式（计算火曲线和卷边线）
	//钢板火曲变形处理方式 0.装配面 1.中性层 2.凸面  3.凹面
	BYTE m_ciDeformType;
	double m_fDeformCoef;	//火曲变形系数
	double m_fHuoquR;		//火曲半径
	C2dInterpolator mapNeutroCoef,mapBendR;
#endif
	//CBuffer m_bufferDrawingScript;	//绘图脚本缓存区,需要存储	wht 10-12-14
	//CBuffer m_bufferSumTaMatCfg;		//材料汇总配置文件缓存区,需要存储
#ifdef __PART_DESIGN_INC_
	long m_iIteration;		//迭代序号
	WEATHER_REGION weather;
	char weatherCode[50];
	long volt_grade;		//电压等级:330,500
	double m_fIceDensity;	//杆塔裹冰重度kg/m3
	double m_fVibrationT1;	//基本自振周期(s)
	char m_cLandscape;	//塔位地形:'A','B','C','D'
	char m_cLineRoadType;//线路路况:0平路线路;1丘陵线路2山地线路
	/*安全等级(决定结构重要性系数gama0)：
	0:一级特别重要的杆塔结构(gama0=1.1)；
	1:二级各级电压线路的各类杆塔(gama0=1.0)；
	2:三级临时使用的各类杆塔(gama0=0.9).*/
	short m_siSafetyGrade;
	double m_fSafetyCoef;	//结构重要性系数γo
	//基本设计参数（规程规范相关）
	int m_iLoadExecuteCode;		//0:电力线路规范1:高耸结构规范2:建筑结构荷载规范3:用户指定;荷载计算执行规范，风振系数、构件体型系数、可变荷载组合系数及分项系数取值所遵循的规范
	int m_iMemberSizeCheckCode;	//构件规格验算选材规范0:电力线路规范1:高耸结构规范2:美国ASCE10-97
	int m_iSlendernessLimCode;	//杆件长细比限制规范0:电力线路规范1:高耸结构规范2:美国ASCE10-97　3:用户指定
	double m_fGamaGForDesign;		//杆塔设计时重力荷载分项系数
	double m_fGamaGForFoundation;	//基础设计时重力荷载分项系数
	double m_fGamaQForTowerWindLoad;//杆塔本身及附加风荷载分项系数
	double m_fLimitBearCoef;		//承载系数最大设计值
	bool m_bWeightAmplifyCoefIncBraceRods;	//风压段增重系数中是否包含辅材杆件 wjh-2019.7.13
	bool m_bAreaAmplifyCoefIncBraceRods;	//风压段面积放大系数中是否包含辅材杆件 wjh-2019.7.13
	//满应力优化先材参数
	struct{
		long m_nMainPoleSlendernessLimit;
		long m_nXiePoleSlendernessLimit;
		long m_nAuxPoleSlendernessLimit;
		long m_nTensivePoleSlendernessLimit;
		long m_nMainTubeSlendernessLimit;
		long m_nXieTubeSlendernessLimit;
		long m_nAuxTubeSlendernessLimit;
		long m_nTensiveTubeSlendernessLimit;
		char m_csMinAngleSize[10],m_csMinMainAngleSize[10],m_csMinForceAngleSize[10];
		BOOL m_bPreferSingleAngle;
		int  m_iSizeOrderRule;	//0.按截面积排序规格库1.按回转半径排序规格库2.宽度厚度排序
		long m_nMaxSelMatCount;
		long m_nConvergeLim;	//选材收敛阈值
	}SelMatPara;
#endif

//公有成员函数
public:
	//根据已读取的Ta.Module初始化旧版LDSV1.2.0.6(不含)之前配材号的演进升迁映射表
	bool InitEvolveCfgwordMap(int version,long doc_type);
	bool EvolveCfgword(CFGWORD& cfgword,int version,long doc_type);	//旧版LDSV1.2.0.6(不含)之前配材号的演进升迁
	bool ReplaceObjBodyNo(const int* fromBodyNoArr,const int* toBodyNoArr,int nBodyNo=1);
	bool ReBuildObjsHashTable();
	long GetMinIdleHandle(long hStart=0x20);
	//基本功能
	bool IsNeedInitNewObjDefaultInfo(){return m_bInitNewObjDefaultInfo;}
	void InitTower();
	CLDSModule* InitModule();
	void Empty();
	BOOL SetDocModifiedFlag(BOOL bModified=TRUE);
	BOOL IsDataModified();
	BOOL SelectNodeSet(NODESET* pNodeSet,char* key,...);

	//获得基本信息
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	void CorrectErrorBefore(long nVersion,long doc_type,BOOL bHasRepeatHandle);
	//获得段号数组或者段号字符串，支持指定模型 wht 10-11-04
	//bEqualCfg=TRUE时获得与指定模型配材号完全相等的段号数组或字符串 wht 11-02-19
	int GetSegStr(char* seg_str,CLDSModule *pModule=NULL,BOOL bEqualCfg=FALSE);
	int GetSegNoList(ATOM_LIST<SEGI> &segList,CLDSModule *pModule=NULL,BOOL bEqualCfg=FALSE);
	static CXhChar200 SegNoListToString(ATOM_LIST<SEGI> &segList);
	int GetSegNoHashList(CHashList<SEGI> &segNoHashList,CLDSModule *pModule=NULL,BOOL bEqualCfg=FALSE);
	//根据新段号修改某段构件件号
	void ModifyPartNoByNewSegNo(SEGI oldSegI,SEGI newSegI);
	void ToTowerSolidDataExchangeBuffer(BUFFER_IO* pIO,BUFFER_IO *pObjHandleIO=NULL);
	void ToTowerAttachBuffer(CBuffer &buffer);
	bool ExportTowerSolidDataExchangeFile(char *file);
	CLDSFoundation* AddFoundation(BYTE ciHeightSerial,BYTE ciLegSerial,BYTE ciLegQuad);
	CLDSFoundation* GetFoundation(BYTE ciHeightSerial,BYTE ciLegSerial,BYTE ciLegQuad);
	bool RetrieveWireModelFromNodes(CUniWireModel* pWireModel);
	UINT RetrieveWireHoleSet(CUniWireModel* pWireModel);
#endif
	CFGWORD GetDefaultCfgPartNo(int iQuad=0);
	CLDSModule *GetActiveModule();
	//
	BOOL SetDesignItem(DWORD dwKey,DESIGN_PARAM_ITEM& param,BOOL bModify=FALSE);
	BOOL SetDesignItemValue(DWORD dwKey,double fValue,int iDataType=0,BOOL bModify=FALSE);
	//显示视图操作函数
	CDisplayView *AppendDisplayView(){return View.append();}
	CDisplayView *GetFirstDisplayView(){return View.GetFirst();}
	CDisplayView *GetNextDisplayView(){return View.GetNext();}
	int GetDisplayViewNum(){return View.GetNodeNum();}

	THANDLE (*GetCooperativeNewHandle)();
	void GetTaSizeScope(SCOPE_STRU &scope);
	long GetSlopeList(ARRAY_LIST<GEPOINT2D>& joint_z_arr,ATOM_LIST<f3dLine>* slopeset);
	//ref_part_arr指带有螺栓引用的构件集合
	int GetLsRefCounts(THANDLE hLs,long **ref_part_arr=NULL);
	//数据查询
	CLDSNode *FromPointI(long point_i);
	//idClassType用于指定类型对象ID的替换，一般用不到主要用于读入老数据时处理不同构件句柄重复的问题
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	//移除与指定对象关联的其它对象
	int RemoveRelativeObjs(CLDSDbObject *pDbObj);
	int RemoveNodeRelativeObjs(CLDSNode *pNode);
	BOOL AssertValid();
	PROGRESS_CONTEXT progress;
	void (*DisplayProcess)(int percent,const char *sTitle);	//进度显示回调函数
	void (*ParseDrawingScriptBuffer)(CBuffer &buffer);	//解析绘图脚本缓存
	void PrepareOldVerInfo(COldVerDataFile* pOldVerInfo,int doc_type,long version);
	bool FromBuffer(CBuffer &buffer,long doc_tma1_lma2_tsa3_lds4_default0=0,bool loadLibToArchive=false);
	void ToBuffer(CBuffer &buffer,DWORD cursor_dog_no,char *file_version,char ciDocType=0);
	void ToXmlFile(FILE* fp,DWORD schema,DWORD cursor_dog_no,char *file_version);
	void FromAttachBuffer(CBuffer &buffer);
	void ToAttachBuffer(CBuffer &buffer);
	bool IsHasRsltBuffer(const char* itemKeyStr);
	bool RemoveRsltBuffer(const char* itemKeyStr);
	bool ReadRsltBuffer(const char* itemKeyStr,CBuffer &buffer);
	bool WriteRsltBuffer(const char* itemKeyStr,CBuffer &buffer);
	DWORD ModelContentsLength(){return m_nFileDivisionPos;}
	void SetModelContentsLen(DWORD dwFileLen){m_nFileDivisionPos=dwFileLen;}
	void WriteAdditionalBuffer(CBuffer &buffer);
	void ReadAdditionalBuffer(CBuffer &buffer);
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	UINT GetNewStatNo();
	double SafetyCoef();
	static DefWriteWeatherFunc WriteWireLayoutWeather;
	static DefReadWireLayoutDataFunc ReadWireLayoutData;
#ifdef __LDS_CONTEXT_
	IMemberDesignCode* DesignCode();	//返回当前单元设计所执行的规范/规定
#endif
	//根据一个包络节点面计算面内的所有杆件(包括边界杆件)的投影受压净截面积(m2)
	//double SnapNodeFace(NODESET &nodeset,f3dPoint prjOrg,double W,int iLoadExecuteCode,
	//						BOOL bAutoCalK,double *weighted_coef,double *net_area,double *BtoH);
	//计算风压段的承受风压净投影面积。iLoadExecuteCode=0:电力设计技术规定1:高耸规范2:建筑结构荷载规范
	//void CalWindSegAf(int iLoadExecuteCode);
	//void CalWindLoad(double preferredBetaZ=0.0,double magnifyCoef=1.0);		//计算风压段的承受风压荷载
	//将原始工况与实际工况同步处理，该计算功能已移至独立进程WireLoad.exe中
	//void SynchronizeWorkStatus(CWireLayout *pWireLayout);
	//计算导地线线条荷载,该计算功能已移至独立进程WireLoad.exe中
	//bool CalWireLoad(CWireLayout *pWireLayout,char* sError,int max_error_len=100);
	//从*.frq文件中读取动力特性振型分析时各节点的等效节点质量
	long ReadVibraNodeMass(FILE *fp,DYNAMIC_ANAL_NODE *node_arr);
	//iMode自0开始计数，表示第iMode+1阶振型
	double ReadVibraMode(FILE *fp,int iMode, bool iReadAllFreqs=false);	//返加当前振型的自振频率
	BOOL ImportLoadFile(char *file,double z2_coef=1.0);
	BOOL ExportLoadFile(char *file,double z2_coef=1.0);
	int GetLoadNodeNum();
	CWorkStatus *GetActiveWorkStatus();
	void AppendGeneralStatus();
    CLDSNode* GetForceNodeByIndex(int index);	//根据索引号获得节点
	int GetForceNodeNum();	//获得受力材端点总数
	int GetForcePoleNum();	//受力材总数
	CLDSNode* FromPointI(int point_i);	//根据节点编号获得节点
	int GetForceIndexFromHandle(THANDLE handle);//根据句柄获得节点受力材端点索引号
	int GetNodeIndexFromHandle(THANDLE handle);
#endif
	void RetrieveUdfViews();
	void RetrieveDrawingViews();
	bool UpgradeToCtrlNode(CLDSNode* pNode);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	BOOL CheckPartGroupValidAndCorrect(CLDSPartGroup *pPartGroup,BOOL bAutoCorrect);
	void AmendPlateWeldLocationY();
	void RetrieveWeldLineInfo();
	void RetrieveBriefWeldPartGroups(CHashListEx<CBriefWeldPart>& hashPartGroups,const char* segstr="*");
	void RefreshPartGroups(CHashList<SEGI>& hashSegTbl,BOOL bDisplayProcess=FALSE);
	int RetrievePolyPartList(POLYPARTLIST &list);
	static int RetrievePolyPartList(IModel *pModel,POLYPARTLIST &list);
	//base点表示螺栓的基点位置(相对坐标系下),ucs表示一个螺栓的所在的相对坐标系,
	//此坐标系必须有一轴线与螺栓的工作法线相靠近,iNormX0Y1Z2就表示
	//与螺栓工作法线相靠近的相对坐标系的坐标轴
	CLDSBolt* FindLsByUcsPos(f3dPoint base,UCS_STRU ucs,int iNormX0Y1Z2,double epsilon=EPS2);
	CLDSBolt* FindLsByPos(f3dPoint base,f3dPoint work_norm,double epsilon=EPS2);
	void RefreshBurglarLs();//刷新防盗螺栓
	void RefreshLockPad();	//刷新防松垫圈
	CLDSObject* FromPartNo(const char *sPartNo);
	//删除数据
	int DelBoltAndRelativeBoltRef(THANDLE ls_handle,BOOL bIncPerm=TRUE);
	BOOL DelTubeEndLj(THANDLE handle);	//删除钢管端连接 主要用于删除十字插板
	//移至IModel类中 wjh-2014.7.01
	//void SmartCalAngleWingNorm(CLDSLineAngle *pLineAngle);
#endif
	// Undo&Redo操作功能实现区
private:
	bool m_bGroupUndoStart;	//启动后所有的BeginUndoListen及EndUndoListen无效
	enum UNDO_OBJECT_TYPE{UNDO_DELETE_OBJ,UNDO_INSERT_OBJ,UNDO_MODIFY_OBJ};
	// 存储Undo或Redo的操作记录
	CCirBufferStack UndoBufferStack;
	// 存储某一操作前后的数据变化信息
	CBuffer UndoBuffer;
	//清空一些无效的句柄（对应句柄的对象已不存在），以避免句柄收缩时带来遗留问题　wjh-2015.3.8
	void CleanInvalidHandleRef();
public:
	bool GroupUndoStarted(){return m_bGroupUndoStart;}
	void StartUndoGroup();	//启用成组Undo功能
	bool EndUndoGroup(int iOperType=0);	//终止成组Undo功能
	// 开始监听Undo操作
	void BeginUndoListen(void);
	// 开始监听Undo操作
	bool EndUndoListen(int iOperType=0);
	// 执行Undo操作
	void UndoTower(void);
	// 执行Redo操作
	void RedoTower(void);
	// 可以执行Undo操作
	BOOL CanUndoOper(void);
	// 可以执行Redo操作
	BOOL CanRedoOper(void);
	// 返回当前Undo记录的操作类型
	int CurrentUndoType();
	// 是否有新的Undo信息需要存储
	BOOL IsHasUndoInfo();
	bool CompareVersion(CTower* pSrcTower,void* pLogContents);
	//bool ImportVersionLog(CTower* pSrcTower,CBuffer& buffer,void* pLogContents);
public:
	long (*GetNewHandleHookFunc)();	//优先外挂句柄生成函数
 	long GetNewHandle();
	virtual DWORD ShrinkObjHandle(DWORD hStartShrink=0x20);//>=hStartShrink的对象句柄才会被收缩句柄号
	virtual long GetMinAvailableHandle();//初始化最小可能的图元句柄;
	virtual bool IsTowerModel(){return true;}
	virtual BYTE RepositionAlgor(){return m_ciRepositionAlgor>0?m_ciRepositionAlgor:255;}
	virtual BYTE GetRollProcessType(){return m_ciRollProcessType;}
	virtual BYTE GetRollOffsetType(){return m_ciRollOffsetType;}
	virtual BYTE GetDeformType(){return m_ciDeformType;}
	virtual double GetDeformCoef();
	virtual double GetHuoquR(){return m_fHuoquR;}
	//获取杆塔模型空间中直接管理并实际存在的构件数组，filterShadowPart=true表示只计入母本构件，影射构件将被怱略
	//pPolyPartList为CLdsList<CLDSPolyPart>*类型
	virtual bool GetRealityPartArr(DYN_ARRAY<CLDSBolt*>& arrBoltPtr,DYN_ARRAY<CLDSPart*>& arrPartPtr,
								   void *pPolyPartList,bool filterShadowPart=false);
	virtual CDisplayView *FromViewHandle(long h){return View.FromHandle(h);}
	virtual CLDSObject *FromHandle(long h,BOOL bDbObjOnly=FALSE);
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
	virtual CLDSAnchorBolt* FromAnchorBolt(long hAnchorBolt);
	virtual CLDSArcLift *FromArcLiftHandle(long h){return ArcLift.FromHandle(h);}
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
	UINT AddJointTubeWeldRelations(CLDSPart* pTubePart);	//补充对接钢管信息(缺少焊接父构件但仍视同焊接关系) wjh-2017.10.14
	UINT AddJointTubeWeldRelations(CHashList<SEGI>* pFilterSegHashtable=NULL);
#endif
public:	//增加数据对象
	virtual CDisplayView *AppendView(BOOL bGetNewHandle=TRUE){return View.append(bGetNewHandle);}
	virtual CLDSPoint *AppendPoint(BOOL bGetNewHandle=TRUE){return Point.append(bGetNewHandle);}
	virtual CLDSLine *AppendLine(BOOL bGetNewHandle=TRUE){return Line.append(bGetNewHandle);}
	virtual CLDSPlane *AppendPlane(BOOL bGetNewHandle=TRUE){return Plane.append(bGetNewHandle);}
	virtual CLDSNode *AppendNode(BOOL bGetNewHandle=TRUE){return Node.append(bGetNewHandle);}
	virtual CLDSPart *AppendPart(int CLASS_TYPEID,BOOL bGetNewHandle=TRUE){return Parts.append(CLASS_TYPEID,bGetNewHandle);}
	virtual CLDSAnchorBolt* AppendAnchorBolt();
public:	//删除数据对象
	virtual BOOL DeleteView(long h,BOOL bIncPerm=TRUE){return View.DeleteNode(h,bIncPerm);}
	virtual BOOL DeleteNode(long h,BOOL bIncPerm=TRUE){return Node.DeleteNode(h,bIncPerm);}
	virtual BOOL DeletePart(long h,BOOL bIncPerm=TRUE){return Parts.DeleteNode(h,bIncPerm);}
public:
	static BOOL IsSonPart(CLDSDbObject *pObj,CFGWORD *pBodyWord=NULL,CFGWORD* pLegWord=NULL);
	//部件装配时根据位置匹配节点
	CLDSNode* MatchExterNode(const double* nodecoords);
	CLDSLinePart* MatchExterRod(int idClsType,BYTE cbValidProp,const double* xStartPosCoords,const double* xEndPosCoords,const double* vxWingNorm=NULL,const double* vyWingNorm=NULL);
	CLDSBolt* MatchExterBolt(WORD d,WORD wL,const double* xBasePosition,const double* vWorkNorm);
	//检测是否在指定(或当前)模型中存在重叠的节点
	CLDSNode* CheckOverlappedNode(f3dPoint pos,const CFGINFO& cfginfo,bool bFromStart=true,long hIgnoreHandle=0);
	CLDSNode* CheckOverlappedNode(f3dPoint pos,CLDSModule* pInModule=NULL,CFGWORD* pBodyWord=NULL,CFGWORD* pLegWord=NULL);
	//检测是否在指定(或当前)模型中存在重叠的杆件
	CLDSLinePart* CheckOverLappedLinePart(f3dPoint start,f3dPoint end,const CFGINFO& cfginfo,long hIgnoreHandle=0,const double* vxWingNorm=NULL,const double* vyWingNorm=NULL);
	CLDSLinePart* CheckOverLappedLinePart(f3dPoint start,f3dPoint end,CLDSModule* pInModule=NULL,CFGWORD* pBodyWord=NULL,CFGWORD* pLegWord=NULL);
	CLDSLinePart* CheckOverLappedLinePart(f3dPoint start,f3dPoint end,CLDSDbObject* pTogetherLiveWith);
	//检测是否在指定(或当前)模型中存在重叠的螺栓
	CLDSBolt* CheckOverLappedBolt(const double* xBasePosition,const double* vWorkNorm,long hIgnoreHandle=0);
	//智能合并部件与当前模型中存在的重叠角钢和螺栓
	BOOL IntelliMergeLineAngles(CLDSLineAngle* pModelJg,CLDSLineAngle* pBlockJg);
	BOOL IntelliMergeBolt(CLDSBolt* pModelBolt,CLDSBolt* pBlockBolt);
	//是否是文件瘦身之前的版本
	static bool IsPrevDeflateVersion(long version,long doc_type);
	static BOOL InitDisplaySetByPolyRgn(POLYGON &selRgn,CLDSDbObject* pSameLifeRefObj=NULL,
		NODESET* pRsltNodeSet=NULL,PARTSET* pRsltPartSet=NULL,bool rapidmode=false);
	static BOOL InitDisplaySetByMirPoint(CLDSLinePart* pLinePart,CLDSNode* pMirStart,CLDSNode* pMirEnd);
public:	//装配坐标系相关函数
	virtual CParaCS *AppendAcs();				//增加装配坐标系
	virtual BOOL DeleteAcs(long hAcs);			//删除装配坐标系
	virtual CParaCS *FromAcsHandle(long hAcs);	//根据句柄查询装配坐标系
};
BOOL VersionOldLTMAFile(char* lic_file,char* version_convert_exefile,CBuffer &buffer);
void DeleteNodePtrFromDisplaySet(CTower* pTower,CLDSObject *pNode);
class CUndoOperObject
{
	CTower *m_pTower;
	bool m_bObjectStart;
	bool m_bGroupUndo;
	int  m_iOperType;
public:
	static const int UNDO_REPOSITION = 1;
	CUndoOperObject(CTower *pTower,bool bGroupUndo=false,int iOperType=0,bool bStartListenInstant=true);
	~CUndoOperObject();
	bool StartUndoListen();
};
#endif // !defined(AFX_TOWER_H__06FC49B6_50F1_4315_A1B2_BAFF7A74AA17__INCLUDED_)
