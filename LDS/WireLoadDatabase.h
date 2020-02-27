// WireLoadDatabase.h: interface for the CStockDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WIRELOADDATABASE_H__6B383B92_AF09_4FFD_A2E9_580C0BF97EFF__INCLUDED_)
#define AFX_WIRELOADDATABASE_H__6B383B92_AF09_4FFD_A2E9_580C0BF97EFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "lds_part.h"
#include "XhCharString.h"
#include "comutil.h"
#include "comdef.h"

class CWireLoadDatabase  
{
	IStorage *m_pRootStg,*m_pRootWeatherRegionsStg,*m_pRootWireTypesStg,*m_pRootInsulatorTypesStg,*m_pRootMetalInstrumentsStg;
	IStorage *m_pWireTypeStg,*m_pStg;
	CXhChar100 m_nameWireType;	//与m_pWireTypeStg相对应线型的名称
	IEnumSTATSTG *m_pWireTypesEnum,*m_pWireEnum,*m_pWeatherRegionEnum;
	void WriteWire(IStream *pStream,const DIANXIAN_STRU& wire);
	void ReadWire(IStream *pStream,DIANXIAN_STRU *pWire);
	void WriteWeatherRegion(IStream *pStream,const WEATHER_REGION& region);
	void ReadWeatherRegion(IStream *pStream,WEATHER_REGION *pRegion);
	void InitDatabase();
public:
	void CloseAllSubEntry();
	CWireLoadDatabase();
	virtual ~CWireLoadDatabase();
	BOOL OpenDbFile(char *file_path);
	//气象区域
	BOOL QueryWeatherRegion(char *weatherCode,WEATHER_REGION* pWeatherRegion);
	BOOL QueryFirstWeatherRegion(WEATHER_REGION* pWeatherRegion);
	BOOL QueryNextWeatherRegion(WEATHER_REGION* pWeatherRegion);
	BOOL SetWeatherRegion(char *weatherCode,const WEATHER_REGION& weatherRegion);
	BOOL DelWeatherRegion(char *weatherCode);
	//电线类型
	BOOL QueryFirstWireType(char *wireType);
	BOOL QueryNextWireType(char *wireType);
	BOOL IsHasWireType(char *wireType);
	BOOL DelWireType(char *wireType);
	//电线型号
	BOOL QueryWire(char *wireType,char* spec,DIANXIAN_STRU* pWire);
	BOOL QueryFirstWire(char *wireType,DIANXIAN_STRU* pWire);
	BOOL QueryNextWire(DIANXIAN_STRU* pWire);
	BOOL SetWire(char *wireType,char* spec, DIANXIAN_STRU& wire);
	BOOL DelWire(char *wireType,char* spec);
	//绝缘子
	//金具
};

#endif // !defined(AFX_WIRELOADDATABASE_H__6B383B92_AF09_4FFD_A2E9_580C0BF97EFF__INCLUDED_)
