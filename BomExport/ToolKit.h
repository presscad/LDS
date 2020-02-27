#pragma once
#include "stdafx.h"
#include "BomExport.h"
#include "ITowerBom.h"
#include "XhCharString.h"
#include "BOM.h"
#include "HashTable.h"
#include "SegI.h"
#include "ParseAdaptNo.h"

template<class TYPE> class ARRAY_LIFE{
	TYPE *m_pArr;
	int m_nSize;
public:
	ARRAY_LIFE(){m_pArr=NULL; m_nSize=0;}
	~ARRAY_LIFE(){
		if(m_pArr!=NULL)
			delete[] m_pArr;
		m_nSize=0;
		m_pArr=NULL;
	}
	void* Create(int size){
		m_nSize=size;
		if(m_nSize>0)
			m_pArr=new TYPE[size];
		return m_pArr;
	}
};
CXhChar16 GetPartSpec(BOMPART *pPart);
int GetCompareGrade(const BOMBOLT& bomBolt);
int GetPartCompareGrade(const BOMBOLT& bomBolt);
int CompareBomBolt(const BOMBOLT& item1,const BOMBOLT& item2);
int CompareBomBoltIncSegI(const BOMBOLT& item1,const BOMBOLT& item2);
void CopyBomBoltProp(BOMBOLT &srcBolt,BOMBOLT &destBolt);
int ComparePartNoString(const char *str1,const char *str2,char* materialPrefixCharSet=NULL);
double GetPartZincCoatedArea(BOMPART *pBomPart);
int CorrectBomPartSegI(BOMPART *pBomPart);

struct BOMBOLT_BOX
{
	BOMBOLT bomBolt;
	CHashList<long> m_hashNumBySegI;
	BOMBOLT_BOX(){;}
	~BOMBOLT_BOX(){;}
	BOMBOLT_BOX(const BOMBOLT_BOX &srcBoltBox);
	BOMBOLT_BOX& operator=(const BOMBOLT_BOX &srcBoltBox);
	void Clone(const BOMBOLT_BOX &srcBoltBox);
};
int CompareBomBoltBox(const BOMBOLT_BOX& item1,const BOMBOLT_BOX& item2);
void GetBomBoltBoxArr(CModelBOM &bom,BOMBOLT_BOX *boltArr,int &arr_size);
int GetCutAngleSwordNum(PART_ANGLE *pBomAngle);
BOMPROFILE_VERTEX::COORD3D NormalizedVector(BOMPROFILE_VERTEX::COORD3D &vec);
short GetKaiHeJiaoN(PART_ANGLE *pAngle);
//
enum COLOR_INDEX{
	Q235_COLOR = 2,
	Q345_COLOR = 6,
	Q390_COLOR = 34,
	Q420_COLOR = 33,
	Q460_COLOR = 31,
	OTHER_COLOR= 1
};
int GetMatColorIndex(CXhChar16 sMat);
//
long ftol(double fVal);
double CalSteelPlankWei(double volume);
double Distance(BOMPROFILE_VERTEX::COORD2D  pos1,BOMPROFILE_VERTEX::COORD2D pos2);
CXhChar16 QuerySteelMatMark(char cMat);
CXhChar16 GetPartTypeName(BOMPART *pPart);
BOOL IsUnequalAngle(double x_wing_wide,double y_wing_wide,double thick);
bool IsRectPlate(BOMPART *pBomPart);