#pragma once
#include "Hashtable.h"
#include "XhCharString.h"
#include "Buffer.h"
//#include "Resolver.h"

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
	//void ToXmlFile(FILE* fp,DWORD schema);
};
class CWireLoadModel
{
	//CXuanLianResolver* m_pResolver;
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
	//void ToXmlFile(FILE *fp,DWORD schema);
	//判断导地线的控制气象条件并初始化电线状态方程
	void InitDianXianEquation(
		WEATHER &weather,double gama_g,double gama_ice_w,double gama_max_w,double gama_ice);
	//weather是气象条件
	//cal_style=0：使用悬链线方程计算cal_style=1:使用斜抛物线方程计算
	void CalWorkEnvLoad(char cLandscape,char cLineRoadType);
	/*
	//属性管理
	static CStringKeyHashTable<long> propHashtable;
	static CHashTable<DWORD> propStatusHashtable;
	int GetPropValueStr(long id,char *valueStr);				//得到属性值字符串
	static void InitModelPropHashtable();
	static int GetPropStr(long id,char *propStr);		//得到属性字符串
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(OBJSET &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	*/
};
//原始点号,转移自Tower.h（.cpp）中 wjh-2014.4.28
class WIRENODE_STRU
{
public:
	long hNode;	//与该节点相关联的实际模型中节点的句柄值
	DWORD idNodeI;	//节点在模型中的标识编号
	DWORD idPlace;	//节点在荷载工况方案模板中的对应的挂位号
	double fLoadAllocCoefX,fLoadAllocCoefY,fLoadAllocCoefZ;
	CHashList<long>hashWorkEnv;	//键值是原始工况标识号，元素值是相应工况下对应的原始电气工况CRawWireLoad中WorkEnv的标识号
	WIRENODE_STRU();
};
//风荷载结构
struct WIND_LOAD
{
	//X/Y/Z方向的风荷载或换算系数；自动计算风荷载时分别表示风速、风压调整系数和风向角
	double velocity;	//风速
	long  azimuth;		//风向方位角（°），以线路方向为准（即Y轴正向）
};
//节点集中荷载,　转自lds_part.h wjh-2014.4.27
class CExternalNodeLoad
{
public:
	double permanentFz;	//重力方向的准永久外荷载值(N)
	f3dPoint Fxyz;		//三个坐标轴方向的集中可变外荷载值(N)
	CExternalNodeLoad(){permanentFz=0.0;}
};
//原始工况,转移自Tower.h（.cpp）中 wjh-2014.4.28
class RAWWORKSTATUS_STRU
{
public:
	CXhChar100 name;
	CXhChar50 lim_str;
	BYTE m_cHoriEarthQuakeAxis;	//水平地震验算时的振动方向角°，'X'=0或'Y'=90
	CXhChar50 vibration_mode_str;	//振型分解反应谱法分析时需要考虑的振型列表
	long m_hWorkStatus;		//对应的CWorkStatus的句柄值
	BYTE m_cMainLoadType;	//0:大风1:覆冰2:低温3:断线4:安装5:不均匀覆冰6:水平地震7:竖向地震8:仅水平地震9:仅竖向地震10:其余
	bool m_bCheckWorkCond;	//验算工况
	double ice_thick;	//基本裹冰厚度(mm)
	bool m_bMinVertSpace;	//最小垂直档距工况
	WIND_LOAD WindLoad;	//风荷载
	RAWWORKSTATUS_STRU(){memset(this,0,sizeof(RAWWORKSTATUS_STRU));}
};
//点号位,转移自Tower.h（.cpp）中 wjh-2014.4.28
class LOADNODEROOM_STRU
{
public:
	char helpString[101];
	long m_iRawWireLoadNo;		//原始线条荷载标识号
	CHashList<WIRENODE_STRU>hashNode;
	CHashList<long>hashWorkEnv;	//键值是是工况标识号，元素值是相应工况下对应的原始电气工况CRawWireLoad中WorkEnv的标识号
	LOADNODEROOM_STRU(){m_iRawWireLoadNo=0;memset(helpString,0,100);}
};
//工况组合生成信息,转移自Tower.h（.cpp）中 wjh-2014.4.28
class CWorkStatusGroupInfo
{
public:
	double m_fSafetyCoef;	//结构重要性系数
	double m_fGamaG;		//恒荷载分项系数
	double m_fGamaQ;		//可变荷载分项系数
	double m_fLiveLoadFai;	//活荷载组合系数
	double m_fCheckLoadFai;	//验算荷载组合系数
	double m_fLiveLoadZ;	//活荷载重(N)
	CWorkStatusGroupInfo();
	CHashList<LOADNODEROOM_STRU>hashNodeRoom;
	CHashList<RAWWORKSTATUS_STRU>hashWorkStatus;
	int EnumIdleRawStatusNo();
};
//源结构工况(具体架线方案下的结构工况)，将来应取代RAWWORKSTATUS_STRU
class SRCCASE_STRU
{
public:
	long id;
	//long m_idWorkCase;		//对应的CWorkCase的标识值
	char name[101];
	//char lim_str[51];
	BYTE m_cHoriEarthQuakeAxis;	//水平地震验算时的振动方向角°，'X'=0或'Y'=90
	char vibration_mode_str[51];	//振型分解反应谱法分析时需要考虑的振型列表
	BYTE m_cMainLoadType;	//0:大风1:覆冰2:低温3:断线4:安装5:不均匀覆冰6:水平地震7:竖向地震8:仅水平地震9:仅竖向地震10:其余
	bool m_bCheckWorkCond;	//验算工况
	double m_fIceThick;		//基本裹冰厚度(mm)
	bool m_bMinVertSpace;	//最小垂直档距工况
	WIND_LOAD WindLoad;		//风荷载
	SRCCASE_STRU(){memset(this,0,sizeof(SRCCASE_STRU));}
	double Kesai();		//风荷载组合系数
	double GamaQ();		//风荷载组合系数
	void FromBuffer(CBuffer &buffer,long version=0);
	void ToBuffer(CBuffer &buffer);
};
//挂线位置(点位号,挂位号),部分取代LOADNODEROOM_STRU
class WIREPLACE_STRU
{
public:
	long idPlace;
	long m_iRawWireLoadNo;		//原始线条荷载标识号
	char helpString[101];
	CHashList<long>hashWorkEnv;	//键值是是工况标识号，元素值是相应工况下对应的原始电气工况CRawWireLoad中WorkEnv的标识号
	WIREPLACE_STRU(){idPlace=m_iRawWireLoadNo=0;memset(helpString,0,100);}
	void FromBuffer(CBuffer &buffer,long version=0);
	void ToBuffer(CBuffer &buffer);
};
//架线方案,转移自　Tower.h(.cpp)中 wjh-2014.4.28
class CWireLayout //: public ILog
{
public:
	int iNo;		//方案序号
	char name[50];	//方案名称

	CXhChar100 m_sRelaTemplateName;
	double m_fSafetyCoef;	//结构重要性系数
	double m_fGamaG;		//恒荷载分项系数
	double m_fGamaQ;		//可变荷载分项系数
	double m_fLiveLoadFai;	//活荷载组合系数
	double m_fCheckLoadFai;	//验算荷载组合系数
	double m_fLiveLoadZ;	//活荷载重(N)
	CHashList<SRCCASE_STRU>hashSrcCase;	//hashSrcCase同groupWorkStatus.hashWorkStatus
	CHashList<WIREPLACE_STRU>hashWirePlace;	//hashWirePlace同groupWorkStatus.hashNodeRoom
	CHashList<WIRENODE_STRU> hashWireNode;		//实际挂线节点,键值为挂点标识
	CMapList<CExternalNodeLoad> hashNodeLoad;	//当前架线方案在指定挂位和原始工况下的荷载值，键值为:挂位号,原始工况号
	//TODO: 将来会被上面代码彻底取代
	CWorkStatusGroupInfo groupWorkStatus;
	CHashList<CWireLoadModel>hashWireLoadModel;
	CWireLayout();
	void EvolveToNewVersion();	//进行取代groupWorkStatus的新版本转换
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void Empty();
};
class CTower;
class CWireLayoutsEvolvement
{
public:
	WEATHER weather;
	ATOM_LIST<CWireLayout> WireLayouts;
	CWireLayoutsEvolvement(void);
	~CWireLayoutsEvolvement(void);
	bool EvolveToWireLoadBuffer();//CTower* pTower);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
};
extern CWireLayoutsEvolvement evolve;
