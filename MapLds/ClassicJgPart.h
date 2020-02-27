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
	CXhChar16 sPartNo;	//�������
	CXhChar16 spec;		//���
	char cMaterial;		//����
	int	 num;			//��������
	CXhChar50 sNotes;	//��ע������ע�����⹤��
	CClassicJgPart();
	virtual ~CClassicJgPart();

};
enum JGWING{X_WING,Y_WING};
//���������׽Ǹֹ���
class C2HoleJgPart : public CClassicJgPart
{
public:
	JGWING m_eBaseWing;
	int m_nG;
	int m_nStartSpace;
	int m_nEndSpace;
	int m_nLength;
	double m_fD1;	//��˨ֱ��
	double m_fD2;	//��˨ֱ��
	C2HoleJgPart();
};
//���������׽Ǹֹ���
class C3HoleJgPart : public CClassicJgPart
{
public:
	JGWING m_eBaseWing;
	int m_nG;
	int m_nStartSpace;
	int m_nEndSpace;
	int m_nMiddleLsLeftSpace;
	int m_nMiddleLsRightSpace;
	double m_fD1;	//��˨ֱ��
	double m_fD2;	//��˨ֱ��
	double m_fD3;	//��˨ֱ��
	C3HoleJgPart();
};
//��������׽Ǹֹ���
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
	double m_fD1;	//��˨ֱ��
	double m_fD2;	//��˨ֱ��
	double m_fD3;	//��˨ֱ��
	double m_fD4;	//��˨ֱ��
	double m_fD5;	//��˨ֱ��
	C5HoleJgPart();
};
#endif // !defined(AFX_CLASSICJGPART_H__99677AB7_0FD8_4F88_8759_3F988A9FA754__INCLUDED_)
