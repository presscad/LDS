#pragma once
#include "CreateFace.h"
#include "SolidBody.h"
#include "./Fitting.h"

class UScrew
{
	//���������β���:��ĸ
	double m_fNutDs;	//��ĸ�ߵ���
	double m_fNutDe;	//��ĸ�㵽��  Ds/cos(Pi/6)=De
	double m_fNutH;		//��ĸ�߶�
public:
	FITTING_US us;
	GECS acs;
public:
	UScrew(FITTING_US * pGDFitting);
	~UScrew(void);
	//
	bool Create3dSolidModel(CSolidBody * pSolidBody);
	bool AppendSolidBody(fBody * pBody, bool blTransToWorkEnv = true);
	double GetNutH() { return m_fNutH; }
};
