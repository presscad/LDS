#pragma once
#include "CreateFace.h"
#include "SolidBody.h"
#include "./Fitting.h"

class CGDBoard
{
	//���������β���:��ĸ+��Ƭ+����
	double m_fWasherD;	//D=De
	double m_fWasherH;	//h=0.1*D
	double m_fNutH;  //0.6*Ds
	double m_fNutTaper;//0.35
	double m_fNutDe;//��ĸ�㵽��볤  Ds/cos(Pi/6)=De
	double m_fNutDs;//��ĸ�ߵ��߰볤  M/3*4
	double m_fBaffleH;//�ڲ�Բ�����	0.1*D
	double m_fBaffleD;//�ڲ�Բ�����ֱ��Ds
public:
	FITTING_GD gd;
	GECS acs;	//װ������ϵ���Ұ��������ĵ�Ϊԭ�㣬X��Ϊ���᷽�򣬹Ұ�ײ�Ϊ��Y�᷽�򣬴�ֱ�Ұ�ΪZ�᷽��
public:
	CGDBoard(FITTING_GD *pGDFitting=NULL);
	~CGDBoard(void);
	//
	BOOL AppendSolidBody(fBody *pBody,bool blTransToWorkEnv=true);
	BOOL Create3dSolidModel(CSolidBody *pSolidBody);
protected:
	long GetTopArcVertexes(GEPOINT* parrVertexes,long liMaxSlices);
	long GetChinArcVertexes(GEPOINT* parrVertexes,long liMaxSlices);
	int CreateBoardBody(fBody* pBody, COLORREF crColor = RGB(145, 149, 145));
};

