#pragma once
#include "CreateFace.h"
#include "SolidBody.h"
#include "./Fitting.h"

class UScrew
{
	//非主控外形参数:螺母
	double m_fNutDs;	//螺母边到边
	double m_fNutDe;	//螺母点到点  Ds/cos(Pi/6)=De
	double m_fNutH;		//螺母高度
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
