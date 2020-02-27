#pragma once
#include "CreateFace.h"
#include "SolidBody.h"
#include "HoleMaker.h"
#include "./Fitting.h"

class CEBBoard
{
	//非主控外形参数:螺母+垫片
	double m_fWasherD;	//D=De
	double m_fWasherH;	//h=0.1*D
	double m_fNutH;		//0.5*Ds
	double m_fNutTaper;	//0.35
	double m_fNutDe;	//螺母点到点  Ds/cos(Pi/6)=De
	double m_fNutDs;	//螺母边到边
public:
	FITTING_EB eb;
	GECS acs;
public:
	CEBBoard(FITTING_EB *pEBFitting=NULL);
	~CEBBoard(void);
	//
	BOOL AppendSolidBody(fBody *pBody,bool blTransToWorkEnv=true);
	BOOL Create3dSolidModel(CSolidBody *pSolidBody);
protected:
	int CreatEBBoard(fBody* pBody, COLORREF crColor = RGB(145, 149, 145));
};

