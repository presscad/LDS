#pragma once

#ifdef __PART_DESIGN_INC_
#include "HashTable.h"
class CLDSNode;
struct MASSNODE{
	long handle;				//�ڵ���
	CLDSNode* pNode;			//���ָ���ģ�ͽڵ����ָ�루��ʱ����ֻ�ֲܾ���ֵ�ֲ�ʹ�ã�
	double dfSummBraceElemMass;	//�������㵽�ýڵ��ϵĸ�����������������������ϵ��,��-�����ģ�
	double dfSummForceElemMass;	//ֱ�����ӵ��ýڵ�������ĵ�Ԫ�����ܺ͵�һ�루��Ԫ���ֵ����˽ڵ�,��������������ϵ����
	double dfSummForceElemNetMass;	//ֱ�����ӵ��ýڵ�������ĵ�Ԫ�����ܺ͵�һ�루��Ԫ���ֵ����˽ڵ�,����������������ϵ����
	//����Ԫ����ǰ���ܵĽڵ�������dfSummStatMass=dfSummBraceElemMass+dfSummForceElemMass
	//����ģ̬����ʱ��Ҫ��������dfSummStatMass-dfSummForceElemNetMass
	double dfSummStatMass;
	struct WINDSEGMASS_ALLOC{
		long idWindSeg;		//��ѹ�α�ʶ���
		double dfAllocMass;	//�ڵ��ڵ�ǰ��ѹ���ڵķ�������
	}xarrMassOfWindSeg[4];	//�ڵ������ѹ���еķ�������
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
	CWindSegment* pWindSegment;	//CWindSegment* ����
	CXhChar16 szLabel;	//��������е�ͳһ����
	double massByKg;	//��ѹ���ż�����(kg)
	double dfNamedPosH;	//��ѹ������߶�Zֵ,��λm
	double dfVibrFaiX;	//X������ϵ��
	double dfVibrFaiY;	//Y������ϵ��
	double dfAf;		//ӭ����ͶӰ���(m2)
	double dfMuS, dfMuZ;//��ѹ������ϵ�����ѹ�߶ȱ仯ϵ��
	double Iz;			//����ǿ��ϵ��
	double get_dfCurrFai();	//��ǰ����ĺ�������ϵ��
	__declspec(property(get = get_dfCurrFai)) double dfCurrFai;
	//�м������ֵ
	double dfMusMuzVibrFaiIzAf;	//Mus*Muz*��*Iz*Af
	double Bz;		//��ѹ�α�������
	double dfBetaZ;	//�ۺϸ��ֲ�������������շ���ϵ��
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
		double mass;			//ԭʼ������kg
		double massIncRaiseCoef;//������ϵ����������kg
		bool frontwind, sidewind;
		ELEMREC()
		{
			frontwind = sidewind = false;
			memset(this, 0, sizeof(ELEMREC));
			muS = 1.3;	//Ĭ�ϽǸ�
		}
		void InitFromFemElement(CElement* pFemElem,bool blCalByBodySeg=true);
	};
	int windSegI;
	BYTE cType;		//��ѹ������0:�˵�����1:�������2:��������
	BYTE cCalStyle;	//0.����ʽ����1.X����ˮƽ�ᵣ2.Y����ˮƽ�ᵣ
	BYTE cMirType;	//�Գ���Ϣ0.�޶Գ�1.���ҶԳ�
	double dfNamedPosH;
	double Wo;	//kN/m2
	double A_a, A_b, Af_a, Af_b;	//m2
	double areaOfFrontAngles, areaOfFrontTubes, areaOfSideAngles, areaOfSideTubes;
	struct B_A {
		int a, b;	//ӭ�������Ĵ���ȣ��߶Ƚ�խ�ķ��򣩣���ѹ�ε�ǰ�������ȣ�mm
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
	double dfSquareR;	//��������ƽ��R^2
	double epsilonT;	//����ϵ������ʱ���ݸ���ȫ��ȡֵ�ĸ�������������ۼ�ϵ��
	double dfI10;		//��Ӧ10m�ߴ�����ǿ��
	double W0;			//��׼��ѹ
	double m_fLowestBtmZ, m_fHighestTopZ;
	CWindSegDispatcher();
	CHashList<long> hashElemBelongWindSeg;	//��ֵΪ��ԪId,����Ϊ��ѹ�����(���ڱ���߽紦���ø˼��ظ����㵲�������
	//��ֵΪ�ڵ�䱾��,����Ϊ�ڷ�ѹ���еķ�������
	CHashListEx<MASSNODE> hashNodeMassAllocInWindSeg;
	CHashListEx<WINDSEGBETAZCAL_EXPORT> hashWindSegExport;
	void CreateBetaZCalReport(FILE* fp, CLDSModule* pCurrCalHeight,double frequency,double dfBetaZCalWindV, CTower* pTowerModel=NULL);
};
#endif