#include "StdAfx.h"
#include "SegI.h"
#include "lds_co_part.h"
//1.��ϽǸֱ�ż���϶�Ŵ�ֵ
struct EXPORT_INFO 
{
	SEGI segI;
	CLDSGroupLineAngle* pGroupAngle;
	double fEnlarge;
	double fSpace;
	EXPORT_INFO(){fEnlarge=0;fSpace=0;pGroupAngle=NULL;}
};
//2.���߰弰���߰屨�漰��Բ������
struct RIB_EXPORT_INFO
{
	SEGI segI;
	CXhChar16 bStart;
	double fThick;
	CXhChar16 sAngle;		//����
	CXhChar16 partLable;
	CXhChar16 sPlateType;
	RIB_EXPORT_INFO(){fThick=0;sAngle=0;}
};
//3.��ߴ���
struct ROLL_EDGE_EXPORT_INFO
{
	SEGI segI;
	BOOL bStart;
	CXhChar100 sRollEdgeHeigth;
	int   nRollEdge;
	char cMaterial;
	CXhChar16 partLable;
	CXhChar16 sRollEdgeDispose;
	ROLL_EDGE_EXPORT_INFO(){bStart=FALSE;nRollEdge=0;cMaterial=' ';}
};