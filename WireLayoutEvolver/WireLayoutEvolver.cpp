// WireLayout.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "WireLayoutEvolver.h"
#include "WireLayoutsEvolvement.h"

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� WireLayout.h
CWireLayoutEvolver::CWireLayoutEvolver()
{
	return;
}
bool CWireLayoutEvolver::WriteWeather(char* weather_buf,unsigned long size)
{
	CBuffer buffer(weather_buf,size);
	evolve.weather.FromBuffer(buffer);
	return true;
}
bool CWireLayoutEvolver::ReadWireLayoutData(CBuffer* pBuff,long version/*=0*/,long doc_type/*=0*/)
{
	evolve.FromBuffer(*pBuff,version,doc_type);
	return true;
}
bool CWireLayoutEvolver::EvolveWireLayoutToWireLoadBuffer(CBuffer *pBuffer)
{
	return true;
}
