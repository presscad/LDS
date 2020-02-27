#ifndef __EMEND_BOLT_LOCATION_
#define __EMEND_BOLT_LOCATION_
#include "StdAfx.h"
#include "ProcessPart.h"
// 螺栓位置修正类
//////////////////////////////////////////////////////////////////////////
//AngleDgree
//////////////////////////////////////////////////////////////////////////
class CEmendBoltPosition
{
public:
	CEmendBoltPosition(float fThick=0,float fDgree=0,float fWidth=0);
	//获取修正的距离
	float GetEmendDistance(void);
	// 更改螺栓链表的posY
	bool EmendBoltPosY(ATOM_LIST<BOLT_INFO>& boltlist);
	//修正距离
	bool EmendDistance(void);
private:
	float m_fWidth;	// 宽度
	float m_fThick;	// 厚度
	float m_fHalfDgree;	// 角度
	float m_fEmendWidth;// 修正的距离
};
//////////////////////////////////////////////////////////////////////////
//TEST
//////////////////////////////////////////////////////////////////////////
void TestBolt(void);
void TestBoltWidth(void);
void TestBoltlist(int);
#endif