#ifndef __EMEND_BOLT_LOCATION_
#define __EMEND_BOLT_LOCATION_
#include "StdAfx.h"
#include "ProcessPart.h"
// ��˨λ��������
//////////////////////////////////////////////////////////////////////////
//AngleDgree
//////////////////////////////////////////////////////////////////////////
class CEmendBoltPosition
{
public:
	CEmendBoltPosition(float fThick=0,float fDgree=0,float fWidth=0);
	//��ȡ�����ľ���
	float GetEmendDistance(void);
	// ������˨�����posY
	bool EmendBoltPosY(ATOM_LIST<BOLT_INFO>& boltlist);
	//��������
	bool EmendDistance(void);
private:
	float m_fWidth;	// ���
	float m_fThick;	// ���
	float m_fHalfDgree;	// �Ƕ�
	float m_fEmendWidth;// �����ľ���
};
//////////////////////////////////////////////////////////////////////////
//TEST
//////////////////////////////////////////////////////////////////////////
void TestBolt(void);
void TestBoltWidth(void);
void TestBoltlist(int);
#endif