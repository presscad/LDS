// ClassicJgPart.cpp: implementation of the CClassicJgPart class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClassicJgPart.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClassicJgPart::CClassicJgPart()
{
	strcpy(sPartNo,"");	//�������
	strcpy(spec,"");		//���
	cMaterial='S';		//����
	num=1;			//��������
	strcpy(sNotes,"");	//��ע������ע�����⹤��
}

CClassicJgPart::~CClassicJgPart()
{

}
C2HoleJgPart::C2HoleJgPart()
{
	m_eBaseWing=X_WING;
	m_nG=20;
	m_nStartSpace=25;
	m_nEndSpace=25;
	m_nLength=0;
	m_fD1=16;
	m_fD2=16;
}
C3HoleJgPart::C3HoleJgPart()
{
	m_eBaseWing=X_WING;
	m_nG=20;
	m_nStartSpace=25;
	m_nEndSpace=25;
	m_nMiddleLsLeftSpace=0;
	m_nMiddleLsRightSpace=0;
	m_fD1=16;
	m_fD2=16;
	m_fD3=16;
}
C5HoleJgPart::C5HoleJgPart()
{
	m_eBaseWing=X_WING;
	m_nG=20;
	m_nStartSpace=25;
	m_nEndSpace=25;
	m_nLeftLsSpace = 50;
	m_nMiddleLsLeftSpace=0;
	m_nMiddleLsRightSpace=0;
	m_nRightLsSpace = 50;
	m_fD1=16;
	m_fD2=16;
	m_fD3=16;
	m_fD4=16;
	m_fD5=16;
}