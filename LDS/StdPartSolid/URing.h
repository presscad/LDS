#pragma once
#include "CreateFace.h"
#include "SolidBody.h"
#include "HoleMaker.h"
#include "./Fitting.h"

class CURing
{
	double m_fNutH;		//0.5*Ds
	double m_fNutTaper;	//0.35
	double m_fNutDe;	//��ĸ�㵽��  Ds/cos(Pi/6)=De
	double m_fNutDs;	//��ĸ�ߵ���
public:
	FITTING_UR uring;
	GECS acs;
public:
	CURing(FITTING_UR *pUring=NULL);
	~CURing(void);
	//
	bool LoadSizeParams(const char* label);
	//װ������ϵ���Ұ��������ĵ�Ϊԭ�㣬X��Ϊ���᷽�򣬹Ұ�ײ�Ϊ��Y�᷽�򣬴�ֱ�Ұ�ΪZ�᷽��
	bool AppendSolidBody(fBody *pBody,bool blTransToWorkEnv=true);
	bool Create3dSolidModel(CSolidBody *pSolidBody);
protected:
	int AppendTube(fBody* pBody, GEPOINT* btm_vertex_arr,int slices, BOOL bLeft = TRUE,COLORREF bolt_color=RGB(206,219,215));
	int AppendURBend(fBody* pBody, int slices = 32, COLORREF crColor = RGB(145, 149, 145));
};
