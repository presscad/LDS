#pragma once
#include "SolidBody.h"
#include "ArrayList.h"
#include "IModel.h"
class CWordPartsMaker
{
public:
	CWordPartsMaker();
	CWordPartsMaker(const char* label);
	~CWordPartsMaker();
	double m_fThick, m_fRadio;
	COLORREF m_crWord;
	GEPOINT xInitOrgin;	//用于定制初始笔划描述的原点
public:
	void AppendWords(GECS acs,IModel* pModel);
	BOOL Create3dSolidModel(CSolidBody *pSolidBody);
	BOOL LoadSizeParams(const char* label);//读取文件内的点
protected:
	GEPOINT** m_innerLoop;
	ARRAY_LIST<int> m_nLoopPointCount;//每个内环数组的数量集合
	int m_nInnerLoopArr;//内环的数量
	GEPOINT** m_gepoint;
	ARRAY_LIST<int> m_nGepointArrCount;//每个数组的数量集合
	int m_nGepointArr;//数组的数量
	int CreatWordPart(fBody* pBody, GEPOINT* pGepoint,int nGepoint, double fThick, double fRadio = 1);
};






