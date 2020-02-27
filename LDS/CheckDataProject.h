#include "StdAfx.h"
#include "SegI.h"
#include "lds_co_part.h"
//1.组合角钢编号及间隙放大值
struct EXPORT_INFO 
{
	SEGI segI;
	CLDSGroupLineAngle* pGroupAngle;
	double fEnlarge;
	double fSpace;
	EXPORT_INFO(){fEnlarge=0;fSpace=0;pGroupAngle=NULL;}
};
//2.角肋板及槽肋板报告及内圆弧报告
struct RIB_EXPORT_INFO
{
	SEGI segI;
	CXhChar16 bStart;
	double fThick;
	CXhChar16 sAngle;		//倒角
	CXhChar16 partLable;
	CXhChar16 sPlateType;
	RIB_EXPORT_INFO(){fThick=0;sAngle=0;}
};
//3.卷边处理
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