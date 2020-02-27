// DesignWireHole.h: interface for the CDesignWireHole class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DESIGNJOINT_H__347D30A8_C348_4BBF_AA03_201699D26396__INCLUDED_)
#define AFX_DESIGNJOINT_H__347D30A8_C348_4BBF_AA03_201699D26396__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "lds_co_part.h"

struct GUAXIANHOLE_BASE_INFO	//挂线孔信息
{
	f3dPoint ls_pos;	
	double fRadius;
	double lengthPosToEdge;	//挂线孔在轮廓边的投影位置
};
class CDesignWireHole
{
	ARRAY_LIST<GUAXIANHOLE_BASE_INFO> guaXianHoleInfoArr;	
	//
	void RefreshStartAndEndVertex();
	BOOL RemoveErrorHoles();
public:
	CLDSPlate* m_pPlate;
	PROFILE_VERTEX* m_pVertexS;
	PROFILE_VERTEX* m_pVertexE; 
	BOOL m_bWaveShape;
	double m_fInnerWaveArcRadius;	//波浪型挂线板内凹过渡圆弧半径
public:
	CDesignWireHole(CLDSPlate* pPlate=NULL);
	~CDesignWireHole();
	//
	void AppendHoleInfo(GUAXIANHOLE_BASE_INFO holeInfo){guaXianHoleInfoArr.append(holeInfo);}
	int GetHoleNum(){return guaXianHoleInfoArr.GetSize();}
	GUAXIANHOLE_BASE_INFO* EnumFirstHole(){return guaXianHoleInfoArr.GetFirst();}
	GUAXIANHOLE_BASE_INFO* EnumNextHole(){return guaXianHoleInfoArr.GetNext();}
	GUAXIANHOLE_BASE_INFO* EnumTailHole(){return guaXianHoleInfoArr.GetTail();}
	//
	void DesignWirePlate();
	BOOL CalNearestEdgeToHole();
	BOOL CalStartAndEndTangentPt(PROFILE_VERTEX& startTangPt,PROFILE_VERTEX& endTangPt);
	BOOL CalMultiHoleTangentPt(ATOM_LIST<PROFILE_VERTEX> &vertexList);
	
};
#endif // !defined(AFX_DESIGNJOINT_H__347D30A8_C348_4BBF_AA03_201699D26396__INCLUDED_)