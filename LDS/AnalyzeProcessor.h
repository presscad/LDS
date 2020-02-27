// AnalyzeProcessor.h: interface for the CAnalyzeProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALYZEPROCESSOR_H__03EC360F_FE92_404A_848E_6AC3EFCC5269__INCLUDED_)
#define AFX_ANALYZEPROCESSOR_H__03EC360F_FE92_404A_848E_6AC3EFCC5269__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Tower.h"
#include "XhCharString.h"

#if defined(__LDS_)||defined(__TSA_)
#include "Resolver.h"
struct ELEMFORCE_ENV{
	bool bEffectN;
	double N;	//广义力：表示轴力时单位,N;表示弯矩时单位,N*mm
	bool bothComp;	// true:交叉材同时受压控制;false:非交叉材或交叉材一拉一压工况控制
	//THANDLE hWorkStatus;//工况句柄
	SUBID hWorkStatus;
	THANDLE hModule;	//模型句柄
	long leg_no[4];		//四象限接腿号
	ELEMFORCE_ENV(){bEffectN=bothComp=false;N=0;}
};
struct ELEMCOMPSTABLE_ENV{
	bool bEffectN;
	double N;		// (N)
	bool bothComp;	// true:交叉材同时受压控制;false:非交叉材或交叉材一拉一压工况控制
	double sigema;	// N/(A*φ),(MPa)
	//所有不考虑同时受压的杆件单元，信息按一拉一压存储
	double Afai_bothcomp,Afai_tenscomp;//A*φ
	double fai_bothcomp,fai_tenscomp;	//同时受压和一拉一压时的压稳系数
	double L_bothcomp,Lo_r_bothcomp;	//同时受压控制工况下的计算长度（含计算长度折算系数）及长细比（含长细比修正系数）
	double muCoefL_bothcomp;			//同时受压控制工况下的计算长度折算系数
	BYTE lamdatype_bothcomp,lamdatype_tenscomp;	// 0:lamdaRyo控制;1:lamdaRx控制;2:lamdaRx2控制
	SUBID hWorkStatus;//工况句柄
	THANDLE hModule;	//模型句柄
	long leg_no[4];		//四象限接腿号
	ELEMCOMPSTABLE_ENV(){bEffectN=false;}
};
struct LAMDA_STABLE_INFO{
	CLDSNode *pStart,*pEnd;	//计算长度的起始、终止节点
	double L,R,Lo_r;		//计算长度,回转半径,实际长细比
	//计算长度基准折算系数（静态值，L中已计入此系数值，已考虑腿部斜材长细比放大系数及交叉斜材之外的折算系数值)
	double muCoefBasic;		//此系数仅为动态生成braceStr时用（L及Lo_r中已计入此系数）,等同于SMC_ITEM.Lo_CALINFO.coef
	double muCoefLive;		//交叉斜材按同时受压考虑时的计算长度动态(实时)折算系数(L及Lo_r中未计入此系数影响，须动态计入）
	bool bCalByElemLen;		//pStart=NULL,pEnd=NULL时，是否表示按单元整长计算长度还是不考虑该计算长度
	bool bothCompLive;		//True:当前工况中视为同时受压;False:当前工况中视为无同时受压情况出现
	double coefFai;			//压稳系数，用于计算压稳状态下的设计应力
	double coefAreaFai;		//压稳系数与面积乘积，用于计算压稳状态下的设计应力
	CXhChar50 braceStr;		//计算长度取值区间字符串
	double MuCoef();		//coefMuBasic*coefMuLive;}
	void ClearZero();
	DECLARE_READONLY_PROPERTY(bool,Validate);
	LAMDA_STABLE_INFO();
};
struct ELEMDESIGN_INFO{
	double hole_out;	//减孔数
	double hole_d;		//孔径
	double w_t;
	double fCoefTensStrengthDecrease,fCoefCompStrengthDecrease;
	double fCoefStablity;	//轴心受压构件稳定系数
	double fCoefEffic;		//控制工况下设计指标占指标允许极限的比例系数，无控制工况启用则为零
	double fCoefOfBearLoad;	//控制工况下承受荷载占承载极限的比例系数，无控制工况启用则为零
	int    iDangerousRule;	//危险控制规则
	int iDangerousLoadCase;	//最危险的承载指标
	bool bBlockShearActivated;	//抗拉控制时块剪验算被激活
	double mN;		//压杆稳定强度折减系数
	double workSigema;	//最大计算工作应力
	CXhChar50 LoExpression,lamdaExpression;		//长细比计算表达式
	BOOL bStartRotRest,bEndRotRest;
	BYTE lamdaSelType;	// 0:lamdaRyo控制;1:lamdaRx控制;2:lamdaRx2控制
	char cLamdaTypeR;	//0:Rx;1:Ry0
	double lamdaL,lamdaR,Lo_r;		//计算长度,回转半径,实际长细比
	double UltimateLo_r;	//允许最大长细比
};
#include "MemberProperty.h"
class CElement
{
	f3dPoint lenVec,lenStdVec;			//单位化的单元长度方向矢量
	double length;				//单元长度mm，临时记录为了将来优化
	CProcessBodyLoad bodyLoad;	//含覆冰荷载在内的单元体荷载
public:
	static double COMPRESSION_ZERO_FORCE;	//最小压力(N)，大于此值不视为压杆
	static double DIFFERENCE_ZERO_FORCE;	//最小轴力差(N)，大于此值才视为轴力有差异
	static double DIFFERENCE_ZERO_STRESS;	//最小应力差(MPa)，大于此值才视为轴力有差异
	CFGWORD cfgword;
	CLDSNode *pStart,*pEnd;
	BYTE ciFreedomStart,ciFreedomEnd;	//临时存储始末端实际有限元计算时的自由度　wjh-2017.7.14
	CLDSLinePart *pOrgPole;
	struct RELATIVE_ELEM
	{
		long idRefElem;		//交叉斜材考虑同时受压参照单元
		long idRefElem2;	//考虑同端节点同时受压参照单元
		bool bFatherElem;	//此单元是否为对称关联单元中的父单元，即节点号综合较小的单元
		long idElemMirX,idElemMirY,idElemMirZ;
	}relaElem;
	int    BearTens1Comp2;	//1:承拉杆承压;2:承压杆承拉;其余为正常杆。1、2为零杆，其余情况为活动杆
private:
	struct SIZE_PARA_TYPE{
		bool m_bOutdated;	//是否已过期
		union PARA_TYPE{
			JG_PARA_TYPE angle;
			TUBE_PARA_TYPE tube;
			BIAN_PARA_TYPE flat;
			SLOT_PARA_TYPE slot;
		}para;
	}m_xParaType;
public:
	int m_iSizeIndex;		//用于按照规格临时排序比较规格大小
	PROPERTY(void*,pParaType); 
	virtual GET(pParaType){
		if(m_xParaType.m_bOutdated)
			return NULL;	//过期
		else
			return &m_xParaType.para;
	}
	virtual SET(pParaType){
		if(value!=NULL)
		{
			m_xParaType.m_bOutdated=false;
			if(pOrgPole->size_idClassType==CLS_LINEANGLE||pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
				memcpy(&m_xParaType.para,value,sizeof(JG_PARA_TYPE));
			else if(pOrgPole->size_idClassType==CLS_LINETUBE)
				memcpy(&m_xParaType.para,value,sizeof(TUBE_PARA_TYPE));
			else if(pOrgPole->size_idClassType==CLS_LINEFLAT)
				memcpy(&m_xParaType.para,value,sizeof(BIAN_PARA_TYPE));
			else if(pOrgPole->size_idClassType==CLS_LINESLOT)
				memcpy(&m_xParaType.para,value,sizeof(SLOT_PARA_TYPE));
			else
				m_xParaType.m_bOutdated=true;
		}
		else
			m_xParaType.m_bOutdated=true;
	}
	//void* pParaType;
	bool bEffectN;			//N值为斜(辅)材考虑到支撑作用时的力 wjh-2016.12.14
	double N;				//内力(N)
	double T0;				//锚固端预张力(N)
	double W0,H0,V0;		//拉线预张紧后后的总重力(N)及自由端在预张力作用下产生的张力水平分量和竖直分量
	bool bAnchorEnd;		//true:拉线终端对地锚固;false:拉线始端对地锚固
	GEPOINT moment,VFS,VFE;	//单元内力矩(N*mm),具体始末端截面剪力
	short elem_type;	//0:表示二力杆或经典梁;1:表示柔索单元
	LAMDA_STABLE_INFO lamdaRyo;//最小轴长细比计算信息
	LAMDA_STABLE_INFO lamdaRx;	//普通材或交叉斜材一拉一压时平行轴长细比计算信息
	LAMDA_STABLE_INFO _lamdaRc;	//交叉斜材同时受压时平行轴长细比计算信息(过时的废弃不用的) wjh 2013.9.22
	LAMDA_STABLE_INFO LamdaRc();	//交叉斜材同时受压时平行轴长细比计算信息
	SMC_ITEM ActiveSmcItem();
	struct SMCITEM_GROUP{	//最多考虑三根同时受压参照杆件单元（0.含交叉斜材及1-2.同端节点材）
		bool hasBothComp;	//True:实际工况中有同时受压情况出现;False:实际情况中无同时受压情况出现
		char iActiveItem;//最终激活的同时受压项类型与lamdaRc同步更新
		SMC_ITEM smcItems[3];
		LAMDA_STABLE_INFO lamdaItems[3];
		bool ValidateActiveItem();
		SMCITEM_GROUP();
	}smcinfo;
	double m_fStartBraceNCoef;	//辅助材始端对受力材的支撑力系数
	double m_fEndBraceNCoef;	//辅助材终端对受力材的支撑力系数
	double m_fWeightRaiseCoef;	//自身重力增大系数
	int m_iCtrlRule;	//1.长细比2.受拉3.受压4.压稳5.统材6.螺栓
	//long m_idPartClsType;
	//int iSizeIndex;
	BOOL m_bValidStartRestStiff,m_bValidEndRestStiff;	//是否有承受部分转动约束的能力
	//BOOL m_bTensAndCompElem,m_bBothCompElem;	//前者表示对称一拉一压，后者表示对称同时受压
	//xx_joint_type代表可能性，而real_xx_joint_type则代表实际计算时的支座类型
	//0：完全铰接JOINT_HINGE；1：部分刚接JOINT_RIGID（限制除扭转外的转动约束）,2：自动判断JOINT_AUTOJUSTIFY
	int start_joint_type,end_joint_type;
	int real_start_joint_type,real_end_joint_type;	//0：完全铰接JOINT_HINGE1；部分刚接JOINT_RIGID（限制除扭转外的转动约束）
	int start_force_type,end_force_type;			//角钢两端受力状况CENTRIC_FORCE,ECCENTRIC_FORCE
	ELEMDESIGN_INFO design_info;
	ELEMFORCE_ENV MaxTension,MaxCompression,MaxMoment;	//MaxMoment的弯矩单位为(N*mm),
	ELEMCOMPSTABLE_ENV MaxCompStable;			//压稳控制时的最大压稳计算应力
	//对于辅助材单元表示始末端所支撑的受力材单元，用于计算辅材验算压力
	CElement *pStartBraceForceElem,*pEndBraceForceElem;
	FEM_ELEMENT *pFemElem;
	long id;	//单元标识，在前处理器中生成，一直贯穿始终
	long feature;	//便于某些算法，临时设置变量不存储，只在特定算法中有含义
	//double* elemStiffMat;	//double [dimension][dimension]
public:
	CElement();
	CElement& operator=(const CElement& element);
	double Length() const{return length;}
	__declspec(property(get=Length)) double dfLength;	//单元长度
	f3dPoint LenStdVec() const{return lenStdVec;}
	//<-2.0直线定义有误;<0.0||>1.0:点不在线上或直线内;0.0:点与直线始端重合;<1.0点在直线内;1.0点与直线终端重合
	double CheckPoint(const f3dPoint& pick);
	void CalLength();
	double GetThick();
	double GetWidth();
	bool IsExportBothCompLamda(IMemberDesignCode* pDesignCode=NULL);	//是否在报告书中输出同时受压时的长细比(只有同时受压时长细比过载系数<承载系数时输出)
	double LamdaByType(int lamdaType);
	//pbSpecBothCompState为调用上下文中指定是否考虑同时受压同时受压状态 wjh-2017.2.4
	double CompStableLamda(bool* pbSpecBothCompState = NULL);
	void UpdateLamdaInfo();
	char CalLenType(); 
	char LamdaRType(){return design_info.cLamdaTypeR;}
	double LamdaL(){return design_info.lamdaL;}	//计算长度
	double LamdaR(){return design_info.lamdaR;}	//回转半径
	double Lamda(){return design_info.Lo_r;}//实际长细比
	//复制除CElemInfo::relaElem及标识之外的原始设计信息到原始设计单元中
	void CopyBasicInfoTo(CElemInfo* pElemInfo,bool copyCalLenInfo=true);
	bool IsHasMirInfo();
	//是否是对称单元中始末节点号最小的单元
	BOOL IsFatherElem(){return pOrgPole->IsFatherObject();}
	//是否与pElem有对称关联
	BOOL IsMirWithElem(CElement *pElem);
	//比较(对称)单元的始末节点号
	int CompareElemPointI(CElement *pElem);
	double GetWorkI(int para=0);	//返回杆件指定（角钢时para=0平行轴或para=1最小轴）惯性矩(mm4)
	double GetWorkW(int para=0);	//返回杆件指定（角钢时para=0平行轴或para=1最小轴）抗弯模量(mm3)
	double GetWorkR(int para=0);	//返回杆件指定（角钢时para=0平行轴或para=1最小轴）回转半径(mm)
	double SelfWeight(){return bodyLoad.SelfWeight();}
	CProcessBodyLoad CalBodyLoad(double lowest_z,double ice_density=900);	//
	CProcessBodyLoad BodyLoad() const{return bodyLoad;}	//
};
typedef CElement* ELEM_PTR;

//应力计算时将长杆打碎后的全塔单元节点（一个计算体内的临时集合）
class CPreProcessor;
class CPostProcessor;
class CAnalyzeProcessor  
{
	CPreProcessor *m_pPreProcessor;
	CPostProcessor *m_pPostProcessor;
	//计算单元动态(实时)同时受压压稳信息，主要用于交叉斜材动态长细比及压稳系数实时更新计算
	void CalElemCompStableLiveSMCInfo(CElement *pElem);
public:
	NODESET nodeset;//一个计算体内的全部有限元力学分析节点集合,与femNodeList中的FEM_NODE一一对应
	ATOM_LIST<FEM_NODE>femNodeList;		//与nodeset中的CLDSNode一一对应
	ATOM_LIST<FEM_ELEMENT>femElemList;	//不含辅材在内的一个计算体内的全部有限元力学分析单元集合
	CHashList<CElement>hashElems;		//含辅材单元在内的一个计算体内的全部单元集合
	CHashList<FEM_NODE_PTR>hashFemNodes;
	bool m_bHasCableElem;				//当前结构体内是否有拉线单元，有拉线单元属于非线性范畴
public:
	CAnalyzeProcessor();
	virtual ~CAnalyzeProcessor();
	//一个计算模型的所有工况计算过程分析完毕后调用
	void UpdateInfoAtModuleChange(CLDSModule *pModule);
	//一个计算模型的当前工况计算过程分析完毕后调用
	void UpdateInfoAtStatusChange(CLDSModule *pModule,CWorkStatus *pStatus);
	//所有计算完毕后，更新单元关联杆件端头的最大设计轴力和弯矩
	void UpdateElemRelaRodDesignForce();
	void ApplyBraceForceToElem();
	void SetPreProcessor(CPreProcessor *pPreProcessor){m_pPreProcessor=pPreProcessor;}
	void SetPostProcessor(CPostProcessor *pPostProcessor){m_pPostProcessor=pPostProcessor;}
	int GetForceFaceNum(CLDSNode *pNode,f3dPoint axis_vec,f3dPoint *force_face_norm=FALSE);
	void AnalyzeElemDegree();
	void AnalyzeElemDegree(CBlockResolver* pResolver);
	BOOL CreateAnalyzeFile(char *file_name);
};
extern CAnalyzeProcessor analysis; 
#endif
#endif // !defined(AFX_ANALYZEPROCESSOR_H__03EC360F_FE92_404A_848E_6AC3EFCC5269__INCLUDED_)
