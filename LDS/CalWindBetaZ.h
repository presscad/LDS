#pragma once

#ifdef __PART_DESIGN_INC_
#include "HashTable.h"
class CLDSNode;
struct MASSNODE{
	long handle;				//节点句柄
	CLDSNode* pNode;			//句柄指向的模型节点对象指针（临时对象，只能局部赋值局部使用）
	double dfSummBraceElemMass;	//最终折算到该节点上的辅材质量（包含了重力增大系数,总-受力材）
	double dfSummForceElemMass;	//直接连接到该节点的受力材单元质量总和的一半（单元均分到两端节点,包含了重力增大系数）
	double dfSummForceElemNetMass;	//直接连接到该节点的受力材单元质量总和的一半（单元均分到两端节点,不包含了重力增大系数）
	//有限元分析前汇总的节点总质量dfSummStatMass=dfSummBraceElemMass+dfSummForceElemMass
	//进行模态分析时需要附加质量dfSummStatMass-dfSummForceElemNetMass
	double dfSummStatMass;
	struct WINDSEGMASS_ALLOC{
		long idWindSeg;		//风压段标识序号
		double dfAllocMass;	//节点在当前风压段内的分配质量
	}xarrMassOfWindSeg[4];	//节点归属风压段中的分配质量
	bool AddWindSegMass(long idWindSeg,double mass);
	double MassInWindSeg(long idWindSeg);
	double get_dfSummMass() const;
	__declspec(property(get=get_dfSummMass)) double dfSummMass;
	MASSNODE(){
		memset(this,0,sizeof(MASSNODE));
	}
	void SetKey(DWORD key){handle=(long)key;}
};
class CWindSegment;
struct WINDCALSEG {
	static const BYTE FAI_TYPE_X = 0;
	static const BYTE FAI_TYPE_Y = 1;
	static BYTE CURR_VIBR_FAI_TYPE;	//
	CWindSegment* pWindSegment;	//CWindSegment* 类型
	CXhChar16 szLabel;	//输出报告中的统一命名
	double massByKg;	//风压段团集质量(kg)
	double dfNamedPosH;	//风压段名义高度Z值,单位m
	double dfVibrFaiX;	//X向振型系数
	double dfVibrFaiY;	//Y向振型系数
	double dfAf;		//迎风面投影面积(m2)
	double dfMuS, dfMuZ;//风压段体型系数与风压高度变化系数
	double Iz;			//湍流强度系数
	double get_dfCurrFai();	//当前激活的横向振型系数
	__declspec(property(get = get_dfCurrFai)) double dfCurrFai;
	//中间计算结果值
	double dfMusMuzVibrFaiIzAf;	//Mus*Muz*φ*Iz*Af
	double Bz;		//风压段背景因子
	double dfBetaZ;	//综合各种参数计算出的最终风振系数
	WINDCALSEG() { memset(this, 0, sizeof(WINDCALSEG)); }
};
class CElement;
class WINDSEGBETAZCAL_EXPORT
{
public:
	WINDCALSEG xBetazCal;
	struct ELEMREC {
		int startPointI, endPointI;
		char cMaterial;
		SIZESPEC size;
		double length, area, muS, efficArea;
		double mass;			//原始质量，kg
		double massIncRaiseCoef;//含增大系数的质量，kg
		bool frontwind, sidewind;
		ELEMREC()
		{
			frontwind = sidewind = false;
			memset(this, 0, sizeof(ELEMREC));
			muS = 1.3;	//默认角钢
		}
		void InitFromFemElement(CElement* pFemElem,bool blCalByBodySeg=true);
	};
	int windSegI;
	BYTE cType;		//风压段类型0:八点塔身1:多点轮廓2:三角塔身
	BYTE cCalStyle;	//0.塔身方式计算1.X方向水平横担2.Y方向水平横担
	BYTE cMirType;	//对称信息0.无对称1.左右对称
	double dfNamedPosH;
	double Wo;	//kN/m2
	double A_a, A_b, Af_a, Af_b;	//m2
	double areaOfFrontAngles, areaOfFrontTubes, areaOfSideAngles, areaOfSideTubes;
	struct B_A {
		int a, b;	//迎风面形心处宽度（尺度较窄的方向），风压段的前后面间深度，mm
		B_A() { a = b = 0; }
	}b2aFR, b2aSD;
	double fai_a, fai_b, b_a_a, b_a_b, yita_a, yita_b;
	double muZ, muSa, muSb, betaZ, K, Cqi_a, Cqi_b;
	double Wsa, Wsb, Wsc;	//kN
	double Xcoef_Wsa, Xcoef_Wsb, Xcoef_Wsc;
	double Ycoef_Wsa, Ycoef_Wsb, Ycoef_Wsc;
	double Wx, Wy;
	WINDSEGBETAZCAL_EXPORT();
	void SetKey(DWORD key) { windSegI = (int)key; }
	//CHashListEx<WINDLOADNODE>loadNodeList;
	CMapList<ELEMREC> elemrecs;
};
class CLDSModule;
class CTower;
class CWindSegDispatcher
{
public:
	double dfSquareR;	//共振因子平方R^2
	double epsilonT;	//风振系数计算时根据杆塔全高取值的杆塔风荷载脉动折减系数
	double dfI10;		//对应10m高处湍流强度
	double W0;			//基准风压
	double m_fLowestBtmZ, m_fHighestTopZ;
	CWindSegDispatcher();
	CHashList<long> hashElemBelongWindSeg;	//键值为单元Id,数据为风压段序号(用于避免边界处共用杆件重复计算挡风面积）
	//键值为节点句本柄,数据为在风压段中的分配质量
	CHashListEx<MASSNODE> hashNodeMassAllocInWindSeg;
	CHashListEx<WINDSEGBETAZCAL_EXPORT> hashWindSegExport;
	void CreateBetaZCalReport(FILE* fp, CLDSModule* pCurrCalHeight,double frequency,double dfBetaZCalWindV, CTower* pTowerModel=NULL);
};
#endif