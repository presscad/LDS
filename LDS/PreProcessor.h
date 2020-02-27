// PreProcessor.h: interface for the CPreProcessTower class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREPROCESSOR_H__BA395871_CAF0_4D53_9CB8_F7C8AFD49FBE__INCLUDED_)
#define AFX_PREPROCESSOR_H__BA395871_CAF0_4D53_9CB8_F7C8AFD49FBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HashTable.h"
//#include "ToolKits.h"
#include "Tower.h"
#include "ArrayList.h"
#include "ldsptr_list.h"
#if defined(__LDS_)||defined(__TSA_)
#include "AnalyzeProcessor.h"
#include "Codes.h"
#include "Resolver.h"

class WINDSEGLOADCAL_EXPORT;
class CWindSegElemPicker
{
	double minZ,maxZ;		//风压段轮廓顶点在风压工作面坐标系下的Z坐标范围
	bool bHasTubeRod;
	double m_fNetAreaOfAngle;	//风压段内角钢件实际挡风面积,mm2
	double m_fNetAreaOfTube;	//风压段内钢管件实际挡风面积,mm2
	long m_idWindSeg;			//风压段的标识序号
	bool m_bShiftToSidePlane;	//根据输入的正面轮廓节点，切换致侧面计算（如酒杯塔曲臂计算侧面挡风面积时）
public:
	GECS ucs;			//风压段轮廓顶点工作面相对坐标系，用于拾取判断单元是否在风压段内
	GEPOINT geomCenter;	//几何形心
	double m_fPrjAreaCoef;	//风压段工作面积的风压投影系数
	struct WINDSEG_VERTEX {
		short siNodeSerial;	//轮廓点在风压侧面中的索引序号，中间删除凹点后序号不变
		GEPOINT xModelPos;	//轮廓点模型坐标
		GEPOINT xPlanePos;	//轮廓点迎风面坐标
		GEPOINT xProjPos;	//轮廓点投影面坐标
		WINDSEG_VERTEX() {
			siNodeSerial = 0;
		}
	};
	ARRAY_LIST<WINDSEG_VERTEX>xarrVertices;	//凸多边形顶点工作面相对坐标系下相对坐标
	CFGWORD cfgword;	//下面的单元集合的适用配材号范围（只针对塔身）
	CTmaPtrList<CElement,CElement*>elemSet;
	double a, b, b_a;	//正面塔架迎风面与背风面之间距离/塔架迎风面宽度
	double m_fWrapArea;	//包络轮廓内投影面积，m^2
	CWindSegElemPicker();
	bool get_blShiftToSidePlane() {return m_bShiftToSidePlane;}
	__declspec(property(get = get_blShiftToSidePlane)) bool blShiftToSidePlane;
	bool InitElemPicker(NODESET &nodeset,f3dPoint prjOrg,UINT idWindSeg,bool bShiftToSidePlane=false);
	void StartElemPicker(ATOM_LIST<CElement> &allElemSet,CLDSModule *pModule=NULL, 
		CHashList<long> *pHashElemBelongWindSeg=NULL, bool blCalByBodySeg = true);
	bool IsIncludeElem(CElement* pElem,CLDSModule *pModule=NULL);
	double CalWindSegAf(double W,double *weighted_coef,WINDSEGLOADCAL_EXPORT *pExport=NULL);
	double MinZ()const{return minZ;}
	double MaxZ()const{return maxZ;}
	double get_dfNetAreaOfAngles() { return m_fNetAreaOfAngle; }
	double get_dfNetAreaOfTubes() { return m_fNetAreaOfTube; }
	__declspec(property(get = get_dfNetAreaOfAngles)) double dfNetAreaOfAngles;
	__declspec(property(get = get_dfNetAreaOfTubes))  double dfNetAreaOfTubes;
#ifdef __ALFA_TEST_VERSION_
public:
	double Test_CalWindSegAf(double W,int iLoadExecuteCode,double *weighted_coef);
#endif
};
struct ELEMLATERAL_STIFF
{
	CElement *pElem;
	double stiff;
};
class CNodeLinkElems
{
public:
	CLDSNode *pNode;
	CFGWORD cfgword;		//下面的bSupport3D及supportDirect的适用模型范围
	int supportType;		//<=0:无支撑;1:轴向支撑;2:平面支撑;>=3:三维立体支撑
	f3dPoint supportDirect;	//平面支撑
	ATOM_LIST<ELEMLATERAL_STIFF>linkElems;
	CNodeLinkElems(){pNode=NULL;supportType=0;}
};
struct AUXELEM_CONST
{
	CElement *pElem;	//关联的辅材单元指针
	double sina;		//单元射线方向与支撑面坐标系Ｘ轴的夹角正弦
	double cosa;		//单元射线方向与支撑面坐标系Ｘ轴的夹角余弦
	double A;			//单元的截面积，此处A的量纲不重要，因为最终结果brace_coef是无量纲的
	double k_lcoef;		//单元的轴向刚度系数k=k_lcoef*A
	double k_xcoef;		//单元的在局部支撑坐标系X轴向的刚度系数k_x=k_xcoef*A
	double brace_coef;	//计算输出的单元对受力材的支撑力计算系数
};
class CNodeBraceAuxElems
{
public:
	CLDSNode *pNode;
	CFGWORD cfgword;	//有值时表示linkElems有共同的配材号，即在cfgword范围内不用再次判断辅材单元是否对当前模型有效
	int m_iSecondaryElemStiff;	//无任何一端连接于主材的辅材,但同端点辅材却连于主材
	ARRAY_LIST<AUXELEM_CONST>linkElemsConst;	//该节点连接的所有辅材
	CNodeBraceAuxElems(){pNode=NULL;m_iSecondaryElemStiff=-1;}
	AUXELEM_CONST* SecondaryElemStiff(){
		if(m_iSecondaryElemStiff<0||m_iSecondaryElemStiff>=linkElemsConst.GetSize())
			return NULL;
		else
			return &linkElemsConst[m_iSecondaryElemStiff];
	}
};
struct WINDLOADNODE{
	int nodeI;
	double Fwx,Fwy;	//kN
	void SetKey(DWORD key){nodeI=key;}
};
class WINDSEGLOADCAL_EXPORT
{
public:
	struct ELEMREC{
		int startPointI,endPointI;
		char cMaterial;
		SIZESPEC size;
		double length,area,muS,efficArea;
		double mass;			//原始质量，kg
		double massIncRaiseCoef;//含增大系数的质量，kg
		bool frontwind,sidewind;
		ELEMREC()
		{
			frontwind=sidewind=false;
			memset(this,0,sizeof(ELEMREC));
			muS=1.3;	//默认角钢
		}
		void InitFromFemElement(CElement* pFemElem,bool blCalByBodySeg=true);
	};
	int windSegI;
	BYTE cType;		//风压段类型0:八点塔身1:多点轮廓2:三角塔身
	BYTE cCalStyle;	//0.塔身方式计算1.X方向水平横担2.Y方向水平横担
	BYTE cMirType;	//对称信息0.无对称1.左右对称
	double dfNamedPosH;	//风压段计算高度
	double Wo;	//kN/m2
	double A_a,A_b,Af_a,Af_b;	//m2
	double areaOfFrontAngles, areaOfFrontTubes, areaOfSideAngles, areaOfSideTubes;
	struct B_A {
		int a, b;	//迎风面形心处宽度（尺度较窄的方向），风压段的前后面间深度，mm
		B_A() { a = b = 0; }
	}b2aFR, b2aSD;	//b2aFR:正面(FRont)b/a参数；b2aSD:侧面（SiDe)b/a参数
	double fai_a,fai_b,b_a_a,b_a_b,yita_a,yita_b;
	double muZ,muSa,muSb,betaZ,K,Cqi_a,Cqi_b;
	double Wsa,Wsb,Wsc;	//kN
	double Xcoef_Wsa,Xcoef_Wsb,Xcoef_Wsc;
	double Ycoef_Wsa,Ycoef_Wsb,Ycoef_Wsc;
	double Wx,Wy;
	WINDSEGLOADCAL_EXPORT();
	CHashListEx<WINDLOADNODE>loadNodeList;
	CMapList<ELEMREC> elemrecs;
};
typedef ATOM_LIST<WINDSEGLOADCAL_EXPORT> WINDSEGEXPORT_LIST;

struct INTERVAL_NODE
{
	double posLen;
	CLDSNode *pNode;
	bool bSupport3D;		//三维立体支撑
	f3dPoint supportDirect;	//平面支撑
	INTERVAL_NODE(){memset(this,0,sizeof(INTERVAL_NODE));}
};
typedef INTERVAL_NODE* INTERVAL_NODEPTR;
struct CRodIncIntervals
{
	CHashList<INTERVAL_NODEPTR>hashNodes;	//键值是节点号
	ATOM_LIST<ELEM_PTR>listElems;
};
struct SUBBLOCK
{
	SUBBLOCK(long key){serial=key;}
	GECS lcs;
	DWORD serial;
	//CFGWORD cfgword;				//适用的呼高模型（对应模型序号CLDSModule::iNo)
	ARRAY_LIST<long>elems;			//子结构组成单元
	CHashList<CLDSNodePtr> boundary;//过渡区（联结）节点
};
struct SUBBLOCKREF
{
	DWORD serial;
	GECS acs;
};
class CModuleFrame
{
	long m_iNo;
public:
	CModuleFrame(long key=0){m_iNo=key;}
	ATOM_LIST<SUBBLOCKREF> blkrefs;
	ARRAY_LIST<long> elems;	//子结构之外的架构单元，一般为腿部受力材单元（如塔腿主材、斜材）
	CLDSModule* pModule;
};
class CTsaTurboBuffer : public CAttachBuffer
{
public:
	CTsaTurboBuffer(long key);
};
class CWindSegDispatcher;
class CPreProcessor
{
	CTower *m_pTower;
	CAnalyzeProcessor *m_pAnalysisProcessor;
	CHashTable<CLDSNode*>hashTowerNodeI;	//注意键值为节点号而不是句柄
	ATOM_LIST<CWindSegElemPicker>windSegPickerList;
	bool m_bIncEiffelFactorX,m_bIncEiffelFactorY;	//是否启用斜材埃菲尔效应计算
	bool LOWEST_ROD_POSZ_INITED;
	double LOWEST_ROD_POSZ;	//杆塔最低点Z坐标值(m)，主要用于计算杆塔覆冰荷载时反算离地高（取所有呼高最高者为基准) wjh-2017.3.10
	ARRAY_LIST<double>jointx_z_arr,jointy_z_arr;
	CAttachBuffer* pTurboBuffer;	//
private:
	void VerifyEiffelFactor(CTower* pTower);
	CProcessBodyLoad GetBodyLoad(CLDSNode *pNode,CLDSModule *pActiveModule=NULL);
	CProcessBodyLoad GetStartBodyLoad(CElement *pElem,CLDSModule *pActiveModule=NULL);
	CProcessBodyLoad GetEndBodyLoad(CElement *pElem,CLDSModule *pActiveModule=NULL);
	//根据对称原则提取端点号较小的单元形成父杆件集合
	void RetrieveFatherElemHashSet();
	//根据对称原则提取出输入单元所对应的父单元
	CElement* FindFatherElem(CElement* pElem);
	//计算验证(父)单元是否有足够支撑
	void ValidateElemSupportStiff();
	//根据一个包络节点面计算面内的所有杆件(包括边界杆件)的投影受压净截面积(m2)
	double SnapNodeFace(NODESET &nodeset,f3dPoint prjOrg,double W,int iLoadExecuteCode,
		double *weighted_coef,double *net_area,double *BtoH);
	//根据已初始化的长细比原始设计信息智能提取各单元的最大支撑区间及支撑类型
	void SmartRetrieveCalLenInfo();
	//初始化各单元的长细比计算信息
	void InitElemLamdaInfo(bool loadSrcDesignElemInfo=true);
	//更新单元长度信息，在PreparaTower中调用
	void UpdateElemLengthInfo();
	//更新单元体荷载(自重及覆冰荷载)信息，选材发生变化时调用
	void UpdateElemBodyLoadInfo();
	//计算辅材支撑力计算预备信息
	void CalAuxElemBraceForcePreInfo();
	//计算斜材支撑力计算预备信息
	void CalPrimaryElemBraceForcePreInfo();
	//分析各承力单元端节点的自由度及连接类型
	int GetForceFaceNum(CLDSNode *pNode,f3dPoint axis_vec,f3dPoint *force_face_norm=FALSE);
	void AnalyzeElemDegree();
	//清空以前的缓存数据
	void Empty();
	//将摆动挂线节点上对应工况下(如V串挂线)的外荷载等效分配到非挂线点上
	void DispatchSwingNodeLoad(CLDSModule *pModule,CWorkStatus *pStatus,bool bApplyStdLoad);
	//施加节点集中荷载
	bool ApplyNodeLoad(ATOM_LIST<FEM_NODE> &nodeList,CLDSModule *pModule,CWorkStatus *pStatus,bool bApplyStdLoad);
	//抗震验算时的地震荷载计算
	bool CalEarthQuakeLoad(CTower *pTower, CLDSModule *pModule,CWorkStatus *pStatus,int iLoadExecuteCode);
public:
	//自动将集中力加载点及风压轮廓加载点判断为传力节点，即进行CLDSNode::feature=1;临时赋值
	static void SmartJustifyNodeForceType(CTower *pTower,NODESET& nodeset);
public:
	struct ELEM_BODYLOAD_ALLOC{
		DWORD idElem;	//对应
		CElement* pElem;	//对应单元
		double fAllocCoef;	//分配系数
		ELEM_BODYLOAD_ALLOC(){idElem=0;pElem=NULL;fAllocCoef=0.0;}
		void SetKey(DWORD key){idElem=key;}
	};
	class CNodeBodyLoad{
		DWORD idNode;
	public:
		bool bBraceNode;
		bool hasLegElemLink;
		CLDSNode* pNode;
		CHashListEx<ELEM_BODYLOAD_ALLOC> hashElemAllocs;
		CNodeBodyLoad(){idNode=0;pNode=0;bBraceNode=true;hasLegElemLink=false;}
		void SetKey(DWORD key){idNode=key;}
	};
	struct PREPROCESS_RESULT{
		bool bHasSwingNode;
		CFGWORD cfgwordBody;						//仅用于生成CPostProcessReport
		CFGWORD cfgwordLeg;							//仅用于生成CPostProcessReport
		NODESET allnodeset;							//当前分析模型范围内所有节点集合
		LINEPARTSET allrodset;						//当前分析模型范围内所有杆件集合（不含无端节点杆件）
		NODESET nodeSet;							//所有模型传力节点集合
		ATOM_LIST<CElement>elemList;				//含辅助材在内的所有单元集合
		CHashListEx<CNodeBodyLoad> hashNodeBodyLoads;	//各单元体荷载在节点上的分配
		CHashTable<ELEM_PTR>hashElem;
		CHashSet<ELEM_PTR>hashFatherElemPtrs;		//对称合并后节点号较小的单元为父单元，具体父子原则同CPostProcessor
		CMapList<ELEM_PTR>hashElemsMap;				//键值为始末端节点句柄
		CHashTable<CProcessBodyLoad>hashElemStartBodyLoad;	//分配到单元始端的等效体荷载，单元id为键值
		CHashTable<CProcessBodyLoad>hashElemEndBodyLoad;	//分配到单元终端的等效体荷载，单元id为键值
		CHashTable<CProcessBodyLoad>hashNodeBodyLoad;	//等效分配到节点上的等效体荷载，节点句柄为键值
		CHashList<CRodIncIntervals>hashRodIntervals;	//记录由杆件拆分出来的的单元集合
		CHashList<CNodeLinkElems>hashNodeLinks;			//节点连接单元链表，键值为节点句柄
		ATOM_LIST<CNodeBraceAuxElems>listBraceLinks;	//节点上辅材支撑链表
		CHashPtrList<SUBBLOCK>blocks;
		CHashPtrList<CModuleFrame> hashModuleFrames;
		bool InitNodeBodyLoads();
	}result;

	CPreProcessor();
	virtual ~CPreProcessor();
	static IMemberDesignCode* ActiveDesignCode;
	static ILoadCalCode* ActiveLoadCode;
	static void CalElemCompStableInfo(CElement *pElem,IMemberDesignCode* pDesignCode,bool bIncActualCompForce=true);
/*
	完成如下任务：
	1、分析汇总传力节点
	2、分析拆分杆件为单元
	3、计算各工况下荷载节点的等效体荷载值
	4、计算各工况下荷载节点的等效外荷载值
	5、具有一致配材号的标准风压段挡风面积计算
	ciApplyLoad=0(false)表示普通风载计算情景(不须施加荷载)；
	ciApplyLoad=1(true)表示普通内力分析计算情景(须施加荷载)；
	ciApplyLoad=2表示模态分析情景（不须施加荷载，但须计算结构质量分析及辅材端节点信息）
*/
	bool PrepareTower(CTower *pTower, const CFGWORD& cfgwordBody, const CFGWORD& cfgwordLeg, char ciApplyLoad = true, bool loadSrcDesignElemInfo = true);
	bool PreCalFreqNodeAllocAddedStruMass(CLDSModule* pModule);
	bool PrepareTowerSubBlock(CBlockResolver* pResolver,CTower *pTower,const CFGWORD& cfgwordBody,const CFGWORD& cfgwordLeg);
	//在PrepareTower中快速继承过往分析获得模型中稳固不常变的结构信息
	bool InheritStableModelInfo(CBuffer& buffer);
	bool InitSourceDesignInfo(CTower *pTower);
	//返回考虑埃菲尔效应时的坡段数
	int GetEiffelSlopes(bool bOnlyValid=true,CWorkStatus* pVerifyStatus=NULL);
	//设定分析处理器对象
	void SetAnalysisProcessor(CAnalyzeProcessor *pProcessor);
	//初次调用或在单元材料规格发生变化时调用更新相关计算信息
	void UpdateInfoAtElemSizeChange();
	//初次调用或计算呼高组发生变化时调用更新呼高信息
	void UpdateInfoAtModuleChange(CLDSModule *pModule);
	//初次调用或计算模型(含接腿)发生变化时调用更新有限元分析模型信息
	void RetrieveFemAnalInfo(CLDSModule *pModule);
	//初次调用或计算模型(含接腿)发生变化时调用更新构件验算相关信息
	void UpdateInfoAtLegChange(CLDSModule *pModule,CBlockResolver* pResolver=NULL);
	//初次调用或工况发生变化时调用更新相关计算信息,fLoadStepCoef:线条荷载的加载系数，如0.7、1.0、1.1
	void UpdateInfoAtStatusChange(CLDSModule *pModule,CWorkStatus *pStatus,bool bVerifyFoundation,bool bApplyStdLoad,bool bEqCheck=false,int fLoadStepCoef=1.0);
	//初始化风压段的承受风压净投影面积。
	void InitWindSegElemPicker(CTower *pTower);
	//计算风压段的承受风压荷载。iLoadExecuteCode=0:电力设计技术规定1:高耸规范2:建筑结构荷载规范
	void CalWindAndAttachLoad(CTower *pTower, CLDSModule *pModule, CWorkStatus *pStatus, //CWindSegDispatcher* pDispatcher,
		bool bForFoundation = false, double magnifyCoef = 1.0, WINDSEGEXPORT_LIST* pExportList = NULL);
	long GetFatherElemId(CElement* pElem);
	//以下为旧函数用于与新代码对比测试
/*
#ifdef __ALFA_TEST_VERSION_
private:
	CProcessBodyLoad Test_GetBodyLoad(CLDSNode *pNode,CLDSModule *pActiveModule=NULL);
	CProcessBodyLoad Test_GetStartBodyLoad(CElement *pElem,CLDSModule *pActiveModule=NULL);
	CProcessBodyLoad Test_GetEndBodyLoad(CElement *pElem,CLDSModule *pActiveModule=NULL);
	void Test_InitWindSegElemPicker(CTower *pTower);
	//初始化各单元的长细比计算信息
	void Test_InitElemLamdaInfo();
	//计算辅材支撑力计算预备信息
	void Test_CalAuxElemBraceForcePreInfo();
	//抗震验算时的地震荷载计算
	bool Test_CalEarthQuakeLoad(CTower *pTower, CLDSModule *pModule,CWorkStatus *pStatus,int iLoadExecuteCode);
public:
	int Test_EqualResult(CPreProcessor& test_processor,FILE* fp);
	//初次调用或在单元材料规格发生变化时调用更新相关计算信息
	void Test_UpdateInfoAtElemSizeChange();
	void Test_RetrieveFemAnalInfo(CLDSModule *pModule);
	//初次调用或计算模型中接腿发生变化时调用更新相关计算信息
	//void Test_UpdateInfoAtLegChange(CLDSModule *pModule);
	//初次调用或工况发生变化时调用更新相关计算信息,fLoadStepCoef:线条荷载的加载系数，如0.7、1.0、1.1
	void Test_UpdateInfoAtStatusChange(CLDSModule *pModule,CWorkStatus *pStatus,bool bVerifyFoundation,int fLoadStepCoef=1.0);

	bool Test_PrepareTower(CTower *pTower,const CFGWORD& cfgwordBody,const CFGWORD& cfgwordLeg,bool bApplyLoad=true);
	//计算风压段的承受风压荷载
	void Test_CalWindAndAttachLoad(CTower *pTower,CLDSModule *pModule,CWorkStatus *pStatus,int iLoadExecuteCode,
		bool bForFoundation=false,double magnifyCoef=1.0,WINDSEGEXPORT_LIST* pExportList=NULL) ;
#endif
*/
//---------------风振系数计算函数---------------
private:
	//从PrepareTower中单独提取出来，复原建立有限单元模型，以便后续依照随机振动理论计算各风压段风振系数 wjh-2019.8.30
	bool DiscretizeFemElemModel(CLDSModule *pModule);
	//统计计算风振系数时所需的风压段的迎风面挡风面积、体型系数及风压高度变化系数等
	bool CalTowerWindSegBetaZ_PrepareData(CLDSModule* pModule, double dfWindV, CWindSegDispatcher* pDispatcher = NULL);
	bool CalTowerWindSegBetaZ_AllocMass(CLDSModule* pModule, CWindSegDispatcher* pDispatcher = NULL);
	bool CalTowerWindSegBetaZ_Final(CLDSModule *pModule, double dfMaxWindV, double f1, CWindSegDispatcher* pDispatcher);
public:
	bool CalTowerWindSegBetaZ(CTower *pTower, CLDSModule* pModule, double dfMaxWindV, double dfVibraFreq, CWindSegDispatcher* pDispatcher = NULL);
};
extern CPreProcessor preprocessor;
#endif
#endif // !defined(AFX_PREPROCESSOR_H__BA395871_CAF0_4D53_9CB8_F7C8AFD49FBE__INCLUDED_)
