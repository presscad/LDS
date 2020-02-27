// ClassicJgPart.h: interface for the CClassicJgPart class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLASSICJGPART_H__99677AB7_0FD8_4F88_8759_3F988A9FA754__INCLUDED_)
#define AFX_CLASSICJGPART_H__99677AB7_0FD8_4F88_8759_3F988A9FA754__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "XhCharString.h"

class CClassicJgPart  
{
public:
	CXhChar16 sPartNo;	//构件编号
	CXhChar16 spec;		//规格
	char cMaterial;		//材质
	int	 num;			//单基数量
	CXhChar50 sNotes;	//备注，用以注明特殊工艺
	CClassicJgPart();
	virtual ~CClassicJgPart();

};
enum JGWING{X_WING,Y_WING};
//单面有两孔角钢构件
class C2HoleJgPart : public CClassicJgPart
{
public:
	JGWING m_eBaseWing;
	int m_nG;
	int m_nStartSpace;
	int m_nEndSpace;
	int m_nLength;
	double m_fD1;	//螺栓直径
	double m_fD2;	//螺栓直径
	C2HoleJgPart();
};
//单面有三孔角钢构件
class C3HoleJgPart : public CClassicJgPart
{
public:
	JGWING m_eBaseWing;
	int m_nG;
	int m_nStartSpace;
	int m_nEndSpace;
	int m_nMiddleLsLeftSpace;
	int m_nMiddleLsRightSpace;
	double m_fD1;	//螺栓直径
	double m_fD2;	//螺栓直径
	double m_fD3;	//螺栓直径
	C3HoleJgPart();
};
//单面有五孔角钢构件
class C5HoleJgPart : public CClassicJgPart
{
public:
	JGWING m_eBaseWing;
	int m_nG;
	int m_nStartSpace;
	int m_nEndSpace;
	int m_nLeftLsSpace;
	int m_nMiddleLsLeftSpace;
	int m_nMiddleLsRightSpace;
	int m_nRightLsSpace;
	double m_fD1;	//螺栓直径
	double m_fD2;	//螺栓直径
	double m_fD3;	//螺栓直径
	double m_fD4;	//螺栓直径
	double m_fD5;	//螺栓直径
	C5HoleJgPart();
};
#endif // !defined(AFX_CLASSICJGPART_H__99677AB7_0FD8_4F88_8759_3F988A9FA754__INCLUDED_)
