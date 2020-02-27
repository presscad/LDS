#pragma once
#include "f_ent.h"
#include "adsdef.h"
class CSearchBlankPosition
{
protected:
	double* m_pxUpdatePoint;
public:
	struct SEARCH_CELL{
		int xi,yj;
		double weight;//搜索优先权重，越小越优先
		static double fHoriPriorCoef;
		static double fVertPriorCoef;
	public:
		SEARCH_CELL(int x=0,int y=0);
		double CalPriorWeight(int x,int y);
	};
	CSearchBlankPosition(void);
	virtual ~CSearchBlankPosition(void);
	void SetBasePointAddr(double* pxBasePoint){m_pxUpdatePoint=pxBasePoint;}
	int IsHasIntersObj(f3dPoint rgn_vert[4],const struct resbuf *filter=NULL);
	bool SearchNoneIntersPos(f3dPoint rgnVert[4],const double* vcSearchFlowDirection,
		double maxHoriSearchDist=5.0,double maxVertSearchDist=5,double fHoriPriorCoef=0.5,double fVertPriorCoef=0.5,
		char cbDualDirectMode=0x0f,void *pExcusiveSelfDrawObj=NULL);
};

