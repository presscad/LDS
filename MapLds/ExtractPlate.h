#pragma once

#include "StdAfx.h"
#include "StdArx.h"
#include "f_ent.h"
#include "HashTable.h"
#include "RxTools.h"


struct ACDB_CURVE{
	AcDbObjectId curveId;
	AcDbCurve *pCurve;
	f3dPoint startPt;
	f3dPoint endPt;
	int startPtNo;
	int endPtNo;
	//
	ACDB_CURVE()
	{
		curveId=NULL;
		pCurve=NULL;
		startPtNo=endPtNo=0;
	}
};

struct CURVE_VERTEX{
	int vertexNo;
	f3dPoint pos;
	bool bUsed;
	CXhPtrSet<ACDB_CURVE> curveList;
	CURVE_VERTEX(){bUsed=false; vertexNo=0;}
};

class CAcDbPlate{
public:
	CXhPtrSet<ACDB_CURVE> edgeList;
	ATOM_LIST<AcDbObjectId> hashAttachEnts;	//钢板关联的CAD实体id
};

class CExtractPlate
{
	ATOM_LIST<ACDB_CURVE> hashSelectedCurves;	//选中的直(圆弧)线数组
public:
	CExtractPlate(void);
	~CExtractPlate(void);

	ATOM_LIST<CAcDbPlate> plateList;
	void Select();	//框选CAD实体
	void Extract();	//提取板
};

