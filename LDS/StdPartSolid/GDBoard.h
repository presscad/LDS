#pragma once
#include "CreateFace.h"
#include "SolidBody.h"
#include "./Fitting.h"

class CGDBoard
{
	//非主控外形参数:螺母+垫片+挡板
	double m_fWasherD;	//D=De
	double m_fWasherH;	//h=0.1*D
	double m_fNutH;  //0.6*Ds
	double m_fNutTaper;//0.35
	double m_fNutDe;//螺母点到点半长  Ds/cos(Pi/6)=De
	double m_fNutDs;//螺母边到边半长  M/3*4
	double m_fBaffleH;//内侧圆挡板厚	0.1*D
	double m_fBaffleD;//内侧圆挡板厚直径Ds
public:
	FITTING_GD gd;
	GECS acs;	//装配坐标系：挂板耳轴的中心点为原点，X轴为耳轴方向，挂板孔侧为负Y轴方向，垂直挂板为Z轴方向
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

