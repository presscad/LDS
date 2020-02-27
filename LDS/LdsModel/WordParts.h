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
	GEPOINT xInitOrgin;	//���ڶ��Ƴ�ʼ�ʻ�������ԭ��
public:
	void AppendWords(GECS acs,IModel* pModel);
	BOOL Create3dSolidModel(CSolidBody *pSolidBody);
	BOOL LoadSizeParams(const char* label);//��ȡ�ļ��ڵĵ�
protected:
	GEPOINT** m_innerLoop;
	ARRAY_LIST<int> m_nLoopPointCount;//ÿ���ڻ��������������
	int m_nInnerLoopArr;//�ڻ�������
	GEPOINT** m_gepoint;
	ARRAY_LIST<int> m_nGepointArrCount;//ÿ���������������
	int m_nGepointArr;//���������
	int CreatWordPart(fBody* pBody, GEPOINT* pGepoint,int nGepoint, double fThick, double fRadio = 1);
};






