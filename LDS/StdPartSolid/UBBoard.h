#pragma once
#include "CreateFace.h"
#include "SolidBody.h"
#include "Fitting.h"

class UBBoard
{
	double m_fNutH;		//0.5*Ds
	double m_fNutDe;	//螺母点到点  Ds/cos(Pi/6)=De
	double m_fNutDs;	//螺母边到边
public:
	FITTING_UB ub;
	GECS acs;
public:
	UBBoard(FITTING_UB * pGDFitting);
	~UBBoard(void);
	//
	BOOL Create3dSolidModel(CSolidBody * pSolidBody);
	BOOL AppendSolidBody(fBody * pBody, bool blTransToWorkEnv = true);
protected:
	int CreatUBBoard(fBody* pBody, COLORREF crColor = RGB(145, 149, 145));
};
