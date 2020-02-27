// FLSizeOptimizor.h: interface for the CFLSizeOptimizor class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_FLSIZEOPTIMIZOR_H__D080CAF2_07E5_4A8C_B8D0_50B39DE35328__INCLUDED_)
#define AFX_FLSIZEOPTIMIZOR_H__D080CAF2_07E5_4A8C_B8D0_50B39DE35328__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__LDS_)
#include "AnalyzeProcessor.h"

class CJointCouple{
public:
	CLDSNode *pNode;
	CLDSLinePart *pRod;
	CLDSLinePart *pLinkRod;
	union{
		struct FL_INFO{
			CConnectFL *pFLInfo,*pCoupleFlInfo;
			CLDSPart *pFL,*pCoupleFl;
		}FL;			//法兰设计信息
		ANGLE_JOINT* pAngleJoint;	//接头设计信息
	}info;
	double N,M;	//N 轴向力(N),M弯矩(N*m)，一般为pRod端单元为基准
	char label[16],labelCouple[16];
	CJointCouple(){memset(this,0,sizeof(CJointCouple));}
	BOOL IsRelativeMirWith(CJointCouple *pCouple);
	static void StatJointLinkPart(ATOM_LIST<CJointCouple>* pListCouple, int idClassType=0);	//优化选FL时配对法兰附属杆件
};
struct BOLT_SIZE_INFO
{
	double gradeF;		//浮点形式描述的螺栓级别
	int d;				//螺栓直径,mm
	double Nt;			//承受最大轴向力,N
	int inner_space;	//距管壁最小间隙
	int outer_space;	//距管壁最小间隙
};
//FLDESIGN_PROC_INFO类，用于记录法兰设计过程中的一些细节信息,以便进行设计校核
struct FLDESIGN_PROC_INFO
{
	int bolt_n;		//螺栓数量
	double fl_dia;	//法兰直径,mm
	double bolt_dia;//螺栓圆直径,mm
	int rib_height;	//肋板高度,mm
	int rib_height_couple;	//配对肋板高度,mm
	int rib_thick;	//肋板厚度,mm
	double Ntbmax;	//单颗螺栓承受最大拉力,N
	double Lx,Ly;	//加劲法兰片区长,宽,mm
	double q;		//板上均布荷载MPa
	double alfa;	//加劲板反力比系数
	double beta;	//弯矩系数
	double Mox;		//板中弯矩N*m
	double tao_rib;	//劲板剪应力MPa
	double sigm_rib;//劲板正应力MPa
	double f_fl;	//法兰钢材强度及抗剪强度设计值MPa
	double fw_rib,fvw_rib;	//加劲板钢材焊缝强度及抗剪强度设计值MPa
	double m;		//无劲法兰盘中螺栓受力修正系数,一般取0.65
	double Rf;		//无劲法兰板的顶力,N
	double S;		//螺栓间距
	double tao_none;//无劲法兰板剪应力,MPa
	double sigm_none;//无劲法兰板正应力,MPa
	//0.设计成功；1.螺栓强度不足;2.肋板厚度小，不满足强度要求;3.肋板高度不满足强度要求;4.配对法兰肋板高度不满足强度要求;
	int retCode;//5.指定螺栓圆半径过小;6.指定法兰外径太小
	
};
struct BOLTSIZE_ITEM{
	double gradeF;	//螺栓级别
	int d;		//螺栓直径
};
class CFLSizeOptimizor  
{
	CObjNoGroup flNoGroup;
	CHashList<CConnectFL> hashFLStep;
	CHashList<CConnectFL>hashConnectFL;
private:
	static long BOLT_TYPE_N;
	static BOLT_SIZE_INFO BOLTSIZE_TBL[24];
	//0为螺栓序号基数
	double CalYi(int i,int n,double bolt_radius,double axisCoordX);
	double CalY1(int n,double bolt_radius,double axisCoordX);
	double CalNtbmaxCoef(int n,double bolt_radius,double axisCoordX);
	double QueryBetaCoef(double Ly_Lx);
	double QueryAlfaCoef(double Ly_Lx);
	void GenStep1FLSet();
public:
	static void InitBoltSizeTbl(BOLTSIZE_ITEM *boltSizeArr,int size_n=0);
	//设计法兰时遵循的设计规范：
	//0.《架空送电线路钢管塔设计技术规定》	DL/T XXXX-2009《架空输电线路钢管塔构造设计规定》 Q/GDW 391-2009
	//1.《钢结构钢管通信塔技术规程》		CECS 236:2008
	//2.《架空送电线路杆塔结构设计技术规定》DL/T 5154-2002
	int m_iDesignCode;	
	MASKFLAG designStyleFlag;
	static DWORD eSpecBoltNum;		//0x00 0001 00:00000000 0000000000000001 00000000
	static DWORD eSpecFlDiameter;	//0x00 0002 00:00000000 0000000000000010 00000000
	static DWORD eSpecBoltDiameter;	//0x00 0004 00:00000000 0000000000000100 00000000
	static DWORD eSpecRibHeight;	//0x00 0008 00:00000000 0000000000001000 00000000
	static DWORD eSpecRibThick;		//0x00 0010 00:00000000 0000000000010000 00000000
	ATOM_LIST<CJointCouple>listCouple;
	CFLSizeOptimizor();
	virtual ~CFLSizeOptimizor();
	void Clear();	//清空以前法兰优选缓存数据
	//M:弯矩(N*m);N轴力(N,m拉力为正);inner_radius:钢管半径;cMaterial:钢管材质;自动根据重量优先是否要加劲法兰
	CConnectFL SelFL(double M,double N,double inner_radius_arr[2],char cMaterial);
	//M:弯矩;N轴力(拉力为正);inner_radius:钢管半径;t:钢管壁厚;cMaterial:钢管材质;
	CConnectFL SelFLHasRib(FLDESIGN_PROC_INFO* design, double M,double N,double inner_radius_arr[2],char cMaterial);
	//M:弯矩;N轴力(拉力为正);inner_radius:钢管半径;t:钢管壁厚;cMaterial:钢管材质;
	CConnectFL SelFLNoneRib(FLDESIGN_PROC_INFO* design, double M,double N,double inner_radius,char cMaterial);
	void GenFLSet();						    //优化FL结束后为法兰分配代号
};
class CAngleJointDesigner
{
public:	//公用角钢接头设计工具函数
	//重新设计指定接头, ANGLE_JOINT::hSmallRod,hBigRod,smallAngle,bigAngle为输入参数
	static bool SelectAngleJoint(ANGLE_JOINT* pAngleJoint,char cJointType=-1);
	//根据输入的ListCouple，设计生成角钢接头，并输出到hashConnectAngleJoints中
	static void GenAngleJointSet(ATOM_LIST<CJointCouple> *pListCouple,CHashList<ANGLE_JOINT>* pHashAngleJoints);
};
#endif
#endif // !defined(AFX_FLSIZEOPTIMIZOR_H__D080CAF2_07E5_4A8C_B8D0_50B39DE35328__INCLUDED_)
