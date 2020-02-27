#pragma once
#include "f_ent.h"
#include "f_ent_list.h"
#include "Buffer.h"
#include "list.h"
#include "HashTable.h"
#include "Tower.h"

#ifdef __PART_DESIGN_INC_
struct FEMMODEL_NODE{
	DWORD pointI,hNode;
	long index;	//节点在有限元计算模型节点集合中的索引序号（0为基数)
	GEPOINT pos;
	WORD wRestrict;
	CLDSNode* pLdsNode;
	FEMMODEL_NODE(){memset(this,0,sizeof(FEMMODEL_NODE));}
	void SetKey(DWORD key){pointI=key;}
};
struct FEMMODEL_ELEM{
	DWORD elemI,hPart;
	DWORD startPointI,endPointI;
	DWORD sectionI;	//截面号
	GEPOINT QF;		//内力分布渐变值(单位N/mm)
	double  QWF;	//重力折算成的Z向分布力(单位N/mm)
	GEPOINT F_sect_s,F_sect_e,M_sect_s,M_sect_e;	//始末端面的反作用外力及外力矩
	struct SECT_FORCE{
		GEPOINT F_L_S;	//内力跃迁值(单位N), Force Transition of Right Section
		GEPOINT M_L_S;	//内力矩跃迁值(单位N*mm), Moment Transition of Right Section
		GEPOINT F_T_S;	//内力跃迁值(单位N), Force Transition of Left Section
		GEPOINT M_T_S;	//内力矩跃迁值(单位N*mm), Moment Transition of Left Section
	}sects[3];	//0.25,0.5,0.75,1.0长度处的截面
	BYTE cbJointStart,cbJointEnd;
	CLDSLinePart* pLdsRod;
	FEMMODEL_ELEM(){memset(this,0,sizeof(FEMMODEL_ELEM));}
};
struct GENERIC_FORCE{
	GEPOINT Fxyz,Moment;
	GENERIC_FORCE(){;}
	GENERIC_FORCE(GEPOINT F,GEPOINT M){
		Fxyz=F;
		Moment=M;
	}
};
struct ELEMSECT_FORCE{GENERIC_FORCE Fs,Fe;};
struct FEMMODEL_MATERIAL{double E,shear,poson,density;};
struct FEMMODEL_SECTION{
	DWORD sectionI;
	DWORD materialI;
	double area;
	double IX,IY,IZ;
};
struct FEMMODEL_NODE_LOAD
{
	DWORD pointI;
	/*	荷载作用方向
	结点力的方向：	1==X|2==Y|3==Z
	结点力矩的方向：4==X|5==Y|6==Z
	*/
	BYTE directionI;
	double fLoad;	//kN或kN*m
};
struct FEMMODEL_ELEM_LOAD
{
	DWORD elemI;
	BYTE loadType;		//荷载类型：1、2、3分别代表均布力、集中力、集中力偶
	BYTE directionI;	//荷载方向：1、2、3分别代表x，y，z方向
	double fDist;
	double fLoad;	//kN或kN*m
};
struct FEMMODEL_LOAD
{
	int nNodeLoad,nElemLoad;
	double fGamaG1,fGamaG2;	//重力荷载系数1，重力荷载系数2
	ATOM_LIST<FEMMODEL_NODE_LOAD>	nodeLoadList;
	ATOM_LIST<FEMMODEL_ELEM_LOAD> elemLoadList;
public:
	//计算结果存储
	struct FEMNODE_RSLT{
		int pointI;
		KEY4C idCase;	//荷载号
		GEPOINT vDeita;		//位移
		GEPOINT vRotTheta;	//转动位移
	};
	struct FEMELEM_RSLT{
		int elemI;
		KEY4C idCase;	//荷载号
		GECS lcs;		//始->终为X轴正向的受力分析时的局部坐标系（在全局坐标系下描述）
		//GEPOINT vStartForce,vStartMoment;	//始端截面内力（局部坐标系）
		//GEPOINT vEndForce,vEndMoment;		//终端截面内力（局部坐标系）
		struct SECTFORCE{
			GEPOINT vForceStart,vMomentStart;
			GEPOINT vForceEnd  ,vMomentEnd;
		}segments[4];//中间截面内力（局部坐标系）
	};
	CXhSimpleList<FEMNODE_RSLT>listNodeRslts;
	CXhSimpleList<FEMELEM_RSLT>listElemRslts;
};

class CFemModel
{
	CHashList<FEMMODEL_NODE> hashSubstationNodes;
	CHashList<FEMMODEL_ELEM> hashSubstationElems;
	CHashList<FEMMODEL_MATERIAL> hashSubstationMaterial;
	CHashList<FEMMODEL_SECTION> hashSubstationSections;
	ATOM_LIST<FEMMODEL_LOAD> listCaseLoads;
	//LDS有限元计算模型
	ATOM_LIST<FEM_NODE> listFemNodes;
	ATOM_LIST<FEM_ELEMENT> listFemElems;
public:
	CBuffer rsltbuf;
	BYTE m_ciPercent;	//有限元计算进程的完成百分比
	CFemModel(void);
	~CFemModel(void);
	void Empty();
	void ImportSubstationExchangeFile(char* filename);
	void ModelFromBuffer(CBuffer& buffer);
	UINT TestRsltsBufLength();
	void RsltsToBuffer(CBuffer& buffer);
	void InitializeLdsModel();
	//filename=NULL时计算结果写入
	bool AnalyzeFemModel(const char* filename=NULL);
	void ModelToBuffer(CBuffer& buffer);
	//装计算结果导出成内存数据块
	void ExportResultBuffer(FEMMODEL_LOAD* pCase,int iWeightCoefStyle);
	//将计算数据导出成文本文件
	bool ExportResultTxtFile(const char* filename,int iCaseNo,double weightCoef);
};

#endif
