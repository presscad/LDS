// WireLayout.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "WireLayoutEvolver.h"
#include "WireLayoutsEvolvement.h"

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 WireLayout.h
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
