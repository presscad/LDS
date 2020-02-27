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
	long index;	//�ڵ�������Ԫ����ģ�ͽڵ㼯���е�������ţ�0Ϊ����)
	GEPOINT pos;
	WORD wRestrict;
	CLDSNode* pLdsNode;
	FEMMODEL_NODE(){memset(this,0,sizeof(FEMMODEL_NODE));}
	void SetKey(DWORD key){pointI=key;}
};
struct FEMMODEL_ELEM{
	DWORD elemI,hPart;
	DWORD startPointI,endPointI;
	DWORD sectionI;	//�����
	GEPOINT QF;		//�����ֲ�����ֵ(��λN/mm)
	double  QWF;	//��������ɵ�Z��ֲ���(��λN/mm)
	GEPOINT F_sect_s,F_sect_e,M_sect_s,M_sect_e;	//ʼĩ����ķ�����������������
	struct SECT_FORCE{
		GEPOINT F_L_S;	//����ԾǨֵ(��λN), Force Transition of Right Section
		GEPOINT M_L_S;	//������ԾǨֵ(��λN*mm), Moment Transition of Right Section
		GEPOINT F_T_S;	//����ԾǨֵ(��λN), Force Transition of Left Section
		GEPOINT M_T_S;	//������ԾǨֵ(��λN*mm), Moment Transition of Left Section
	}sects[3];	//0.25,0.5,0.75,1.0���ȴ��Ľ���
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
	/*	�������÷���
	������ķ���	1==X|2==Y|3==Z
	������صķ���4==X|5==Y|6==Z
	*/
	BYTE directionI;
	double fLoad;	//kN��kN*m
};
struct FEMMODEL_ELEM_LOAD
{
	DWORD elemI;
	BYTE loadType;		//�������ͣ�1��2��3�ֱ�������������������������ż
	BYTE directionI;	//���ط���1��2��3�ֱ����x��y��z����
	double fDist;
	double fLoad;	//kN��kN*m
};
struct FEMMODEL_LOAD
{
	int nNodeLoad,nElemLoad;
	double fGamaG1,fGamaG2;	//��������ϵ��1����������ϵ��2
	ATOM_LIST<FEMMODEL_NODE_LOAD>	nodeLoadList;
	ATOM_LIST<FEMMODEL_ELEM_LOAD> elemLoadList;
public:
	//�������洢
	struct FEMNODE_RSLT{
		int pointI;
		KEY4C idCase;	//���غ�
		GEPOINT vDeita;		//λ��
		GEPOINT vRotTheta;	//ת��λ��
	};
	struct FEMELEM_RSLT{
		int elemI;
		KEY4C idCase;	//���غ�
		GECS lcs;		//ʼ->��ΪX���������������ʱ�ľֲ�����ϵ����ȫ������ϵ��������
		//GEPOINT vStartForce,vStartMoment;	//ʼ�˽����������ֲ�����ϵ��
		//GEPOINT vEndForce,vEndMoment;		//�ն˽����������ֲ�����ϵ��
		struct SECTFORCE{
			GEPOINT vForceStart,vMomentStart;
			GEPOINT vForceEnd  ,vMomentEnd;
		}segments[4];//�м�����������ֲ�����ϵ��
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
	//LDS����Ԫ����ģ��
	ATOM_LIST<FEM_NODE> listFemNodes;
	ATOM_LIST<FEM_ELEMENT> listFemElems;
public:
	CBuffer rsltbuf;
	BYTE m_ciPercent;	//����Ԫ������̵���ɰٷֱ�
	CFemModel(void);
	~CFemModel(void);
	void Empty();
	void ImportSubstationExchangeFile(char* filename);
	void ModelFromBuffer(CBuffer& buffer);
	UINT TestRsltsBufLength();
	void RsltsToBuffer(CBuffer& buffer);
	void InitializeLdsModel();
	//filename=NULLʱ������д��
	bool AnalyzeFemModel(const char* filename=NULL);
	void ModelToBuffer(CBuffer& buffer);
	//װ�������������ڴ����ݿ�
	void ExportResultBuffer(FEMMODEL_LOAD* pCase,int iWeightCoefStyle);
	//���������ݵ������ı��ļ�
	bool ExportResultTxtFile(const char* filename,int iCaseNo,double weightCoef);
};

#endif
