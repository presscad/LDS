// WireLoadDatabase.cpp: implementation of the CWireLoadDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LDS.h"
#include "WireLoadDatabase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWireLoadDatabase::CWireLoadDatabase()
{
	m_pRootStg = m_pRootWeatherRegionsStg = m_pRootWireTypesStg = NULL;
	m_pWireTypeStg=m_pRootInsulatorTypesStg = m_pRootMetalInstrumentsStg = NULL;
	m_pWireTypesEnum = m_pWireEnum = m_pWeatherRegionEnum = NULL;
}

CWireLoadDatabase::~CWireLoadDatabase()
{
	if(m_pRootWeatherRegionsStg)
		m_pRootWeatherRegionsStg->Release();
	if(m_pRootWireTypesStg)
		m_pRootWireTypesStg->Release();
	if(m_pWireTypeStg)
		m_pWireTypeStg->Release();
	if(m_pRootInsulatorTypesStg)
		m_pRootInsulatorTypesStg->Release();
	if(m_pRootMetalInstrumentsStg)
		m_pRootMetalInstrumentsStg->Release();
	if(m_pRootStg)
		m_pRootStg->Release();
	if(m_pWireTypesEnum)
		m_pWireTypesEnum->Release();
	if(m_pWireEnum)
		m_pWireEnum->Release();
	if(m_pWeatherRegionEnum)
		m_pWeatherRegionEnum->Release();
}
BOOL CWireLoadDatabase::OpenDbFile(char *file_path)
{	//打开复合文档式的数据库文件
	if(m_pRootStg)	//关闭老的数据库文件
		m_pRootStg->Release();
	_bstr_t bstrPathName(file_path);
	DWORD grfMode;
	try{
		grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		HRESULT hr = ::StgOpenStorage(bstrPathName, NULL,grfMode,NULL, 0, &m_pRootStg);
		if(hr==STG_E_FILENOTFOUND)	//没有找到库存文件
		{
			grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE;
			HRESULT hr = ::StgCreateDocfile(bstrPathName,grfMode,0, &m_pRootStg);
			if(FAILED(hr))
				_com_raise_error(hr);
			SysFreeString(bstrPathName);
			InitDatabase();
		}
		else if (FAILED(hr))
			_com_raise_error(hr);
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
		return FALSE;
	}
	return TRUE;
}
//气象区域

void CWireLoadDatabase::ReadWeatherRegion(IStream *pStream, WEATHER_REGION *pRegion)
{
	ULONG uRead;
	int version;
	LARGE_INTEGER move;
	memset(&move,0,sizeof(LARGE_INTEGER));
	pStream->Seek(move,STREAM_SEEK_SET,NULL);
	pStream->Read(&version,sizeof(int),&uRead);
	//最低气温
	pStream->Read(&pRegion->m_nLowestT,sizeof(int),&uRead);
	//平均气温
	pStream->Read(&pRegion->m_nAverageT,sizeof(int),&uRead);
	//最高气温
	pStream->Read(&pRegion->m_nHighestT,sizeof(int),&uRead);
	//大风
	pStream->Read(&pRegion->m_nMaxWindT,sizeof(int),&uRead);
	pStream->Read(&pRegion->m_fMaxWindV,sizeof(double),&uRead);
	//覆冰
	pStream->Read(&pRegion->m_nMaxIceT,sizeof(int),&uRead);
	pStream->Read(&pRegion->m_fMaxIceWindV,sizeof(double),&uRead);
	pStream->Read(&pRegion->m_fMaxIceThick,sizeof(double),&uRead);
	//安装
	pStream->Read(&pRegion->m_nSetupT,sizeof(int),&uRead);
	pStream->Read(&pRegion->m_fSetupWindV,sizeof(double),&uRead);
}

void CWireLoadDatabase::WriteWeatherRegion(IStream *pStream, const WEATHER_REGION &region)
{
	ULONG uWrite;
	int version=1;
	LARGE_INTEGER move;
	memset(&move,0,sizeof(LARGE_INTEGER));
	pStream->Seek(move,STREAM_SEEK_SET,NULL);
	pStream->Write(&version,sizeof(int),&uWrite);
	//最低气温
	pStream->Write(&region.m_nLowestT,sizeof(int),&uWrite);
	//平均气温
	pStream->Write(&region.m_nAverageT,sizeof(int),&uWrite);
	//最高气温
	pStream->Write(&region.m_nHighestT,sizeof(int),&uWrite);
	//大风
	pStream->Write(&region.m_nMaxWindT,sizeof(int),&uWrite);
	pStream->Write(&region.m_fMaxWindV,sizeof(double),&uWrite);
	//覆冰
	pStream->Write(&region.m_nMaxIceT,sizeof(int),&uWrite);
	pStream->Write(&region.m_fMaxIceWindV,sizeof(double),&uWrite);
	pStream->Write(&region.m_fMaxIceThick,sizeof(double),&uWrite);
	//安装
	pStream->Write(&region.m_nSetupT,sizeof(int),&uWrite);
	pStream->Write(&region.m_fSetupWindV,sizeof(double),&uWrite);
}
BOOL CWireLoadDatabase::SetWeatherRegion(char *weatherCode,const WEATHER_REGION& weatherRegion)
{
	try{
		DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		IStream *pStream;
		if(m_pRootStg==NULL)
			return FALSE;
		if(m_pRootWeatherRegionsStg == NULL)
		{
			if(m_pRootStg->OpenStorage(_bstr_t("WeatherRegions"),NULL,grfMode,NULL,0,&m_pRootWeatherRegionsStg)!=S_OK)
				return FALSE;
		}

		if(m_pRootWeatherRegionsStg->OpenStream(_bstr_t(weatherCode),NULL,grfMode,NULL,&pStream)!=S_OK)
			m_pRootWeatherRegionsStg->CreateStream(_bstr_t(weatherCode),grfMode,0,0,&pStream);
		WriteWeatherRegion(pStream,weatherRegion);
		pStream->Release();
		return TRUE;
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
BOOL CWireLoadDatabase::QueryWeatherRegion(char *weatherCode,WEATHER_REGION* pWeatherRegion)
{
	try{
		DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		IStream *pStream;
		if(m_pRootStg==NULL)
			return FALSE;
		if(weatherCode==NULL||strlen(weatherCode)==0)
			return FALSE;
		if(m_pRootWeatherRegionsStg == NULL)
		{
			if(m_pRootStg->OpenStorage(_bstr_t("WeatherRegions"),NULL,grfMode,NULL,0,&m_pRootWeatherRegionsStg)!=S_OK)
				return FALSE;
		}

		if(m_pRootWeatherRegionsStg->OpenStream(_bstr_t(weatherCode),NULL,grfMode,NULL,&pStream)==S_OK)
		{
			ReadWeatherRegion(pStream,pWeatherRegion);
			pStream->Release();
			return TRUE;
		}
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
BOOL CWireLoadDatabase::QueryFirstWeatherRegion(WEATHER_REGION* pWeatherRegion)
{
	try{
		STATSTG statstg;
		IStream *pStream;
		DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		if(m_pRootStg==NULL)
			return FALSE;
		if(m_pRootWeatherRegionsStg == NULL)
		{
			if(m_pRootStg->OpenStorage(_bstr_t("WeatherRegions"),NULL,grfMode,NULL,0,&m_pRootWeatherRegionsStg)!=S_OK)
				return FALSE;
		}
		if(m_pWeatherRegionEnum)
			m_pWeatherRegionEnum->Release();
		m_pRootWeatherRegionsStg->EnumElements(0,NULL,0,&m_pWeatherRegionEnum);
		if(m_pWeatherRegionEnum&&m_pWeatherRegionEnum->Next(1,&statstg,NULL)==S_OK)
		{
			_bstr_t wspec(statstg.pwcsName);
			strcpy(pWeatherRegion->weatherCode,wspec);
			if(m_pRootWeatherRegionsStg->OpenStream(statstg.pwcsName,NULL,grfMode,0,&pStream)==S_OK)
			{
				ReadWeatherRegion(pStream,pWeatherRegion);
				pStream->Release();
			}
			::CoTaskMemFree(statstg.pwcsName);
			return TRUE;
		}
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
BOOL CWireLoadDatabase::QueryNextWeatherRegion(WEATHER_REGION* pWeatherRegion)
{
	try{
		STATSTG statstg;
		IStream *pStream;
		DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		if(m_pWeatherRegionEnum&&m_pWeatherRegionEnum->Next(1,&statstg,NULL)==S_OK)
		{
			_bstr_t wspec(statstg.pwcsName);
			strcpy(pWeatherRegion->weatherCode,wspec);
			if(m_pRootWeatherRegionsStg->OpenStream(statstg.pwcsName,NULL,grfMode,0,&pStream)==S_OK)
			{
				ReadWeatherRegion(pStream,pWeatherRegion);
				pStream->Release();
			}
			::CoTaskMemFree(statstg.pwcsName);
			return TRUE;
		}
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
BOOL CWireLoadDatabase::DelWeatherRegion(char *weatherCode)
{
	try{
		DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		if(m_pRootStg==NULL)
			return FALSE;
		if(m_pRootWeatherRegionsStg == NULL)
		{
			if(m_pRootStg->OpenStorage(_bstr_t("WeatherRegions"),NULL,grfMode,NULL,0,&m_pRootWeatherRegionsStg)!=S_OK)
				return FALSE;
		}

		if(m_pRootWeatherRegionsStg->DestroyElement(_bstr_t(weatherCode))==S_OK)
			return TRUE;
		else
			return FALSE;
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
//电线类型
BOOL CWireLoadDatabase::QueryFirstWireType(char *wireType)
{
	try{
		STATSTG statstg;
		DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		if(m_pRootStg==NULL)
			return FALSE;
		if(m_pRootWireTypesStg == NULL)
		{
			if(m_pRootStg->OpenStorage(_bstr_t("WireTypes"),NULL,grfMode,NULL,0,&m_pRootWireTypesStg)!=S_OK)
				return FALSE;
		}
		if(m_pWireTypesEnum)
			m_pWireTypesEnum->Release();
		m_pRootWireTypesStg->EnumElements(0,NULL,0,&m_pWireTypesEnum);
		if(m_pWireTypesEnum&&m_pWireTypesEnum->Next(1,&statstg,NULL)==S_OK)
		{
			_bstr_t wspec(statstg.pwcsName);
			strcpy(wireType,wspec);
			::CoTaskMemFree(statstg.pwcsName);
			return TRUE;
		}
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
BOOL CWireLoadDatabase::QueryNextWireType(char *wireType)
{
	try{
		STATSTG statstg;
		if(m_pWireTypesEnum&&m_pWireTypesEnum->Next(1,&statstg,NULL)==S_OK)
		{
			_bstr_t wspec(statstg.pwcsName);
			strcpy(wireType,wspec);
			::CoTaskMemFree(statstg.pwcsName);
			return TRUE;
		}
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
BOOL CWireLoadDatabase::IsHasWireType(char *wireType)
{
	try{
		DWORD grfMode = STGM_READ | STGM_SHARE_EXCLUSIVE;
		if(m_pRootStg==NULL)
			return FALSE;
		IStorage *pWireTypeStg;
		if(m_pRootWireTypesStg == NULL)
		{
			if(m_pRootStg->OpenStorage(_bstr_t("WireTypes"),NULL,grfMode,NULL,0,&m_pRootWireTypesStg)!=S_OK)
				return FALSE;
		}
		if(m_nameWireType.Equal(wireType))
			return TRUE;
		HRESULT hr=m_pRootWireTypesStg->OpenStorage(_bstr_t(wireType),NULL,grfMode,NULL,0,&pWireTypeStg);
		if(hr!=S_OK)
			return FALSE;

		if(pWireTypeStg!=NULL)
		{
			pWireTypeStg->Release();
			return TRUE;
		}
		else
			return FALSE;
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
BOOL CWireLoadDatabase::DelWireType(char *wireType)
{
	try{
		DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		if(m_pRootStg==NULL)
			return FALSE;
		if(m_pRootWireTypesStg == NULL)
		{
			if(m_pRootStg->OpenStorage(_bstr_t("WireTypes"),NULL,grfMode,NULL,0,&m_pRootWireTypesStg)!=S_OK)
				return FALSE;
		}

		if(m_pRootWireTypesStg->DestroyElement(_bstr_t(wireType))==S_OK)
			return TRUE;
		else
			return FALSE;
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
//电线型号
void CWireLoadDatabase::WriteWire(IStream *pStream,const DIANXIAN_STRU& wire)
{
	ULONG uWrite;
	int version=1;
	pStream->Write(&version,sizeof(int),&uWrite);
	int lenStr=strlen(wire.specWireType);
	pStream->Write(&lenStr,sizeof(int),&uWrite);
	pStream->Write(wire.specWireType,lenStr,&uWrite);
	lenStr=strlen(wire.specWire);
	pStream->Write(&lenStr,sizeof(int),&uWrite);
	pStream->Write(wire.specWire,lenStr,&uWrite);
	pStream->Write(&wire.m_fDiameter,sizeof(double),&uWrite);
	pStream->Write(&wire.m_fArea,sizeof(double),&uWrite);
	pStream->Write(&wire.m_fUnitWeight,sizeof(double),&uWrite);
	pStream->Write(&wire.m_fForcePermit,sizeof(double),&uWrite);
	pStream->Write(&wire.m_fElasticCoef,sizeof(double),&uWrite);
	pStream->Write(&wire.m_fExpansionCoef,sizeof(double),&uWrite);
}
void CWireLoadDatabase::ReadWire(IStream *pStream,DIANXIAN_STRU *pWire)
{
	ULONG uRead;
	int version,lenStr;
	pStream->Read(&version,sizeof(int),&uRead);
	pStream->Read(&lenStr,sizeof(int),&uRead);
	pStream->Read(pWire->specWireType,lenStr,&uRead);
	pWire->specWireType[lenStr]='\0';
	pStream->Read(&lenStr,sizeof(int),&uRead);
	pStream->Read(pWire->specWire,lenStr,&uRead);
	pWire->specWire[lenStr]='\0';
	pStream->Read(&pWire->m_fDiameter,sizeof(double),&uRead);
	pStream->Read(&pWire->m_fArea,sizeof(double),&uRead);
	pStream->Read(&pWire->m_fUnitWeight,sizeof(double),&uRead);
	pStream->Read(&pWire->m_fForcePermit,sizeof(double),&uRead);
	pStream->Read(&pWire->m_fElasticCoef,sizeof(double),&uRead);
	pStream->Read(&pWire->m_fExpansionCoef,sizeof(double),&uRead);
}
BOOL CWireLoadDatabase::QueryWire(char *wireType,char* spec,DIANXIAN_STRU* pWire)
{
	try{
		DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		IStream *pStream;
		IStorage *pWireTypeStg;
		if(m_pRootStg==NULL)
			return FALSE;
		if(m_pRootWireTypesStg == NULL)
		{
			if(m_pRootStg->OpenStorage(_bstr_t("WireTypes"),NULL,grfMode,NULL,0,&m_pRootWireTypesStg)!=S_OK)
				return FALSE;
		}

		if(m_pRootWireTypesStg->OpenStorage(_bstr_t(wireType),NULL,grfMode,NULL,0,&pWireTypeStg)==S_OK)
		{
			CXhChar100 wireSpec;
			wireSpec.Copy(spec);
			char *separator=strchr(wireSpec,'/');
			if(separator)
				*separator='-';
			if(pWireTypeStg->OpenStream(_bstr_t(wireSpec),NULL,grfMode,NULL,&pStream)==S_OK)
			{
				ReadWire(pStream,pWire);
				pStream->Release();
				pWireTypeStg->Release();
				return TRUE;
			}
			pWireTypeStg->Release();
		}
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
BOOL CWireLoadDatabase::QueryFirstWire(char *wireType,DIANXIAN_STRU* pWire)
{
	try{
		DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		STATSTG statstg;
		DWORD reserved1=0,reserved2=0;
		IStream *pStream;
		if(m_pWireTypeStg!=NULL)
			m_pWireTypeStg->Release();
		if(m_pRootWireTypesStg == NULL)
		{
			if(m_pRootStg->OpenStorage(_bstr_t("WireTypes"),NULL,grfMode,NULL,0,&m_pRootWireTypesStg)!=S_OK)
				return FALSE;
		}
		if(m_pRootWireTypesStg->OpenStorage(_bstr_t(wireType),NULL,grfMode,NULL,0,&m_pWireTypeStg)==S_OK)
		{
			m_nameWireType.Copy(wireType);
			if(m_pWireEnum)	//关闭以前的查询枚举器
				m_pWireEnum->Release();
			m_pWireTypeStg->EnumElements(0,NULL,0,&m_pWireEnum);
			
			if(m_pWireEnum&&m_pWireEnum->Next(1,&statstg,NULL)==S_OK)
			{
				if(statstg.type==STGTY_STREAM)
				{
					if(m_pWireTypeStg->OpenStream(statstg.pwcsName, NULL,grfMode,NULL, &pStream) == S_OK)
					{
						ReadWire(pStream,pWire);
						pStream->Release();
						//返回前清理内存
						::CoTaskMemFree(statstg.pwcsName);
						return TRUE;
					}
				}
				::CoTaskMemFree(statstg.pwcsName);
			}
		}
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
BOOL CWireLoadDatabase::QueryNextWire(DIANXIAN_STRU* pWire)
{
	try{
		DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		STATSTG statstg;
		DWORD reserved1=0,reserved2=0;
		IStream *pStream;

		if(m_pWireTypeStg!=NULL)
		{
			if(m_pWireEnum==NULL)	//如果当前库存料查询枚举器为空时，重新创建
				m_pWireTypeStg->EnumElements(0,NULL,0,&m_pWireEnum);
			if(m_pWireEnum&&m_pWireEnum->Next(1,&statstg,NULL)==S_OK)
			{
				if(statstg.type==STGTY_STREAM)
				{
					if(m_pWireTypeStg->OpenStream(statstg.pwcsName, NULL,grfMode,NULL, &pStream) == S_OK)
					{
						ReadWire(pStream,pWire);
						pStream->Release();
						//返回前清理内存
						::CoTaskMemFree(statstg.pwcsName);
						return TRUE;
					}
				}
				::CoTaskMemFree(statstg.pwcsName);
			}
		}
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
BOOL CWireLoadDatabase::SetWire(char *wireType,char* spec,DIANXIAN_STRU& wire)
{
	try{
		IStream *pStream;
		DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		if(m_pRootStg==NULL)
			return FALSE;
		if(m_pRootWireTypesStg == NULL)
		{
			if(m_pRootStg->OpenStorage(_bstr_t("WireTypes"),NULL,grfMode,NULL,0,&m_pRootWireTypesStg)!=S_OK)
				return FALSE;
		}
		if(m_pWireTypeStg!=NULL)
			m_pWireTypeStg->Release();
		if(	m_pRootWireTypesStg->OpenStorage(_bstr_t(wireType),NULL,grfMode,NULL,0,&m_pWireTypeStg)!=S_OK)
			m_pRootWireTypesStg->CreateStorage(_bstr_t(wireType),grfMode,NULL,0,&m_pWireTypeStg);
		m_nameWireType.Copy(wireType);
		CXhChar100 wireSpec;
		wireSpec.Copy(spec);
		char *separator=strchr(wireSpec,'/');
		if(separator)
			*separator='-';
		strcpy(wire.specWireType,wireType);
		if(m_pWireTypeStg->OpenStream(_bstr_t(wireSpec),NULL,grfMode,NULL,&pStream)!=S_OK)
		{
			if(m_pWireTypeStg->CreateStream(_bstr_t(wireSpec),grfMode,NULL,0,&pStream)==S_OK)
				WriteWire(pStream,wire);
		}
		if(pStream!=NULL)
			pStream->Release();
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
BOOL CWireLoadDatabase::DelWire(char *wireType,char* spec)
{
	try{
		DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
		IStorage *pWireTypeStg;
		if(m_pRootStg==NULL)
			return FALSE;
		if(m_pRootWireTypesStg == NULL)
		{
			if(m_pRootStg->OpenStorage(_bstr_t("WireTypes"),NULL,grfMode,NULL,0,&m_pRootWireTypesStg)!=S_OK)
				return FALSE;
		}
		if (NULL != m_pWireTypeStg)
		{
			pWireTypeStg = m_pWireTypeStg;
			HRESULT hr=pWireTypeStg->DestroyElement(_bstr_t(spec));
			if(hr==S_OK)
				return TRUE;
		}
		else if(m_pRootWireTypesStg->OpenStorage(_bstr_t(wireType),NULL,grfMode,NULL,0,&pWireTypeStg)==S_OK)
		{
			HRESULT hr=pWireTypeStg->DestroyElement(_bstr_t(spec));
			pWireTypeStg->Release();
			if(hr==S_OK)
				return TRUE;
		}
	}
	catch(_com_error &error)
	{
		AfxMessageBox(error.Description());
	}
	return FALSE;
}
//绝缘子
//金具
void CWireLoadDatabase::CloseAllSubEntry()
{
	if(m_pRootWeatherRegionsStg!=NULL)
	{
		m_pRootWeatherRegionsStg->Release();
		m_pRootWeatherRegionsStg=NULL;
	}
	if(m_pRootWireTypesStg!=NULL)
	{
		m_pRootWireTypesStg->Release();
		m_pRootWireTypesStg=NULL;
	}
	if(m_pRootInsulatorTypesStg)
	{
		m_pRootInsulatorTypesStg->Release();
		m_pRootInsulatorTypesStg=NULL;
	}
	if(m_pRootMetalInstrumentsStg)
	{
		m_pRootMetalInstrumentsStg->Release();
		m_pRootMetalInstrumentsStg=NULL;
	}
	if(m_pWireTypesEnum)
	{
		m_pWireTypesEnum->Release();
		m_pWireTypesEnum=NULL;
	}
	if(m_pWireEnum)
	{
		m_pWireEnum->Release();
		m_pWireEnum=NULL;
	}
}

long g_nLGJ_NUM=51;
long g_nLGJQ_NUM=10;
long g_nJLB_NUM=6;
long g_nGJ_NUM=23;
typedef struct tagLGJ_STRU
{
	char mark_area[8];	//标称截面铝/钢（mm2）
	long num_LX;			//铝线根数
	long num_GX;			//钢线根数
	double dia_LX,dia_GX,dia_sum;		//导线直径铝/钢/外径（mm）
	double area_LX,area_GX;	//计算面积铝/钢（mm2）
	double force_permit;	//计算拉断力（N）
	double unit_weight;		//参考计算质量（kg/km）
	double E;				//最终弹性系数（MPa）
	double a;				//线膨胀系数（1/°C）
}LGJ_STRU;
typedef struct tagJLB_STRU
{
	char mark[20];
	double area_sum;		//总面积(mm2)
	double dia_sum;			//总直径(mm)
	double force_permit;	//计算拉断力（N）
	double unit_weight;		//参考计算质量（kg/km）
	double E;				//最终弹性系数（MPa）
	double a;				//线膨胀系数（1/°C）
}JLB_STRU;
typedef LGJ_STRU LGJQ_STRU;	//轻型钢锌铝绞线
struct GJ_STRU
{
public:
	char mark[20];
	double dia_GS,dia_JX;	// 钢丝直径/绞线直径（mm）
	double area;	//断面积
	//整条钢绞线破断拉力=钢丝破断力总和X算系数，换算系数1 X 7结构为0.92；1X19结构为0.9
	double sum_force_permit[5];//破断力总和（kN）
	double unit_weight;		//参考计算质量（kg/km）
	double E;
	double a;				//线膨胀系数（1/°C）
};
//钢芯铝绞线规格（LGJ-XXX-GB/T 1179-1983）
LGJ_STRU LGJ_TBL[51]=
{
	"10/2"  ,  6, 1,1.50,1.50, 4.50, 10.60,  1.77,  4120, 42.88,79000,19.1e-6,
	"16/3"  ,  6, 1,1.85,1.85, 5.55, 16.13,  2.69,  6130, 65.22,79000,19.1e-6,
	"25/4"  ,  6, 1,2.32,2.32, 6.96, 25.36,  4.23,  9290, 102.6,79000,19.1e-6,
	"35/6"  ,  6, 1,2.72,2.72, 8.16, 34.86,  5.81, 12630, 141.0,79000,19.1e-6,
	"50/8"  ,  6, 1,3.20,3.20, 9.60, 48.25,  8.04, 16870, 195.1,79000,19.1e-6,
	"50/30" , 12, 7,2.32,2.32,11.60, 50.73, 29.59, 42620, 372.0,105000,15.3e-6,
	"70/10" ,  6, 1,3.80,3.80,11.40, 68.05, 11.34, 23390, 275.2,79000,19.1e-6,
	"70/40" , 12, 7,2.72,2.72,13.60, 69.73, 40.67, 58300, 511.3,105000,15.3e-6,
	"95/15" , 26, 7,2.15,1.67,13.61, 94.39, 15.33, 35000, 380.8,76000,18.9e-6,
	"95/20" ,  7, 7,4.16,1.85,13.87, 95.14, 18.82, 37200, 408.9,76000,18.5e-6,
	"95/55" , 12, 7,3.20,3.20,16.00, 96.51, 56.30, 78110, 707.7,105000,15.3e-6,
	"120/7" , 18, 1,2.90,2.90,14.50,118.89,  6.61, 27570, 379.0,66000,21.2e-6,
	"120/20", 26, 7,4.72,2.10,15.07,115.67, 18.82, 41000, 466.8,76000,18.9e-6,
	"120/25",  7, 7,4.72,2.10,15.74,122.48, 24.25, 47880, 526.6,76000,18.5e-6,
	"120/70", 12, 7,3.60,3.60,18.00,122.15, 71.25, 98370, 895.6,105000,15.3e-6,
	"150/8" , 18, 1,3.20,3.20,16.00,144.76,  8.04, 32860, 461.4,66000,21.2e-6,
	"150/20", 24, 7,2.78,1.85,16.67,145.68, 18.82, 46630, 549.4,73000,19.6e-6,
	"150/25", 26, 7,2.70,2.10,17.10,148.86, 24.25, 54110, 601.0,76000,18.9e-6,
	"150/35", 30, 7,2.50,2.50,17.50,147.26, 34.36, 65080, 676.2,80000,17.8e-6,
	"185/10", 18, 1,3.60,3.60,18.00,183.22, 10.18, 40880, 584.0,66000,21.2e-6,
	"185/25", 24, 7,3.15,2.10,18.90,187.04, 24.25, 59420, 706.1,73000,19.6e-6,
	"185/30", 26, 7,2.98,2.80,18.88,181.34, 29.59, 64250, 732.6,76000,18.9e-6,
	"185/45", 30, 7,3.80,3.80,19.60,184.73, 43.10, 80190, 848.2,80000,17.8e-6,
	"210/10", 18, 1,3.80,3.80,19.00,204.14, 11.34, 45140, 650.7,66000,21.2e-6,
	"210/25", 24, 7,3.33,2.22,19.98,209.02, 27.10, 65990, 789.1,73000,19.6e-6,
	"210/35", 26, 7,3.22,2.50,20.38,211.73, 34.36, 74250, 853.9,76000,18.9e-6,
	"210/50", 30, 7,2.98,2.98,20.86,209.24, 48.82, 90830, 960.8,80000,17.8e-6,
	"240/30", 24, 7,3.60,2.40,21.60,244.29, 31.67, 75620, 922.2,73000,19.6e-6,
	"240/40", 26, 7,3.42,2.66,21.66,238.85, 38.90, 83370, 964.3,76000,18.9e-6,
	"240/55", 30, 7,3.20,3.20,22.40,241.27, 56.30,102100,1108  ,80000,17.8e-6,
	"300/15", 42, 7,3.00,1.67,23.01,296.88, 15.33, 68060, 939.8,61000,21.4e-6,
	"300/20", 45, 7,2.93,1.95,23.43,303.42, 20.91, 75680,1002,63000,20.9e-6,
	"300/25", 48, 7,2.85,2.22,23.76,306.21, 27.10, 83410,1058,65000,20.5e-6,
	"300/40", 24, 7,3.99,2.66,23.94,300.09, 38.90, 92220,1133,73000,19.6e-6,
	"300/50", 26, 7,3.83,2.98,24.26,299.54, 48.82,103400,1210,76000,18.9e-6,
	"300/70", 30, 7,3.60,3.60,25.20,305.36, 71.25,128000,1402,80000,17.8e-6,
	"400/20", 42, 7,3.51,1.95,26.91,406.40, 20.91, 88850,1286,61000,21.4e-6,
	"400/25", 45, 7,3.33,2.22,26.64,391.91, 27.10, 95940,1295,63000,20.9e-6,
	"400/35", 48, 7,3.22,2.50,26.82,390.88, 34.36,103900,1349,65000,20.5e-6,
	"400/50", 54, 7,3.07,3.07,27.63,399.73, 51.82,123400,1511,69000,19.3e-6,
	"400/65", 26, 7,4.42,3.44,28.00,398.94, 65.06,135200,1611,76000,18.9e-6,
	"400/95", 30,19,4.16,2.50,29.14,407.75, 93.27,171300,1860,78000,18.0e-6,
	"500/35", 45, 7,3.75,2.50,30.00,497.01, 34.36,119500,1642,63000,20.9e-6,
	"500/45", 48, 7,3.60,2.80,30.00,488.58, 43.10,128100,1688,65000,20.5e-6,
	"500/65", 54, 7,3.44,3.44,30.96,501.88, 65.06,154000,1897,69000,19.3e-6,
	"630/45", 45, 7,4.20,2.80,33.60,623.45, 43.10,148700,2060,63000,20.9e-6,
	"630/55", 48, 7,4.12,3.20,34.32,639.92, 56.30,164400,2209,65000,20.5e-6,
	"630/80", 54,19,3.87,2.32,34.82,635.19, 80.32,192900,2388,67000,19.4e-6,
	"800/55", 45, 7,4.80,3.20,38.40,814.30, 56.30,191500,2690,63000,20.9e-6,
	"800/70", 48, 7,4.63,3.60,38.58,808.15, 71.25,207000,2791,65000,20.5e-6,
	"800/100",54,19,4.33,2.60,38.98,795.17,100.88,241100,2991,67000,19.4e-6,
};
//轻型钢芯铝绞,线规格（LGJQ-XXX-GB/T 1179-1983）
LGJQ_STRU LGJQ_TBL[10]=
{
	"150"   ,24, 7,2.76,1.8,16.44,143.6,17.8, 40670, 537,72520,19.8e-6,
	"185"   ,24, 7,3.06,2.0,18.24,176.5,22.0, 50078, 661,72520,19.8e-6,
	"240"   ,24, 7,3.67,2.4,21.88,253.9,31.7, 69776, 951,72520,19.8e-6,
	"300"   ,54, 7,2.65,2.5,23.70,297.8,37.2, 84574,1116,72520,19.8e-6,
	"300(1)",24, 7,3.98,2.5,23.72,298.6,37.2, 81928,1117,72520,19.8e-6,
	"400"   ,54, 7,3.05,3.0,27.36,397.1,49.5,108780,1487,72520,19.8e-6,
	"400(1)",24, 7,4.60,3.0,27.40,398.9,49.5,104860,1491,72520,19.8e-6,
	"500"   ,54,19,3.36,2.0,30.16,478.8,59.7,136220,1795,72520,19.8e-6,
	"600"   ,54,19,3.70,2.2,33.20,580.6,72.2,158760,2175,72520,19.8e-6,
	"700"   ,54,19,4.04,2.4,36.24,692.2,86.0,190120,2592,72520,19.8e-6,
};
JLB_STRU JLB_TBL[6]=
{
	"JLB20A-100", 100.88, 13.00,115577,674.1,147200,13.0e-6,
	"JLB40-100" , 100.88, 13.00, 58653,474.6,103600,15.5e-6,
	"JLB20A-120", 121.21, 14.25,138871,810.0,147200,13.0e-6,
	"JLB40-120" , 121.21, 14.25, 70471,570.3,103600,15.5e-6,
	"JLB20A-150", 148.07, 15.75,169652,989.4,147200,13.0e-6,
	"JLB40-150" , 148.07, 15.75, 86089,696.7,103600,15.5e-6,
};
JLB_STRU LBGJ20AC_TBL[36]=
{
	"LBGJ-16-20AC"	 ,15.93	,5.6	,20280 ,105.81 ,139500,0.0000126,
	"LBGJ-18-20AC"	 ,17.82	,5.93	,22680 ,118.36 ,139500,0.0000126,
	"LBGJ-20(1)-20AC",21.99	,6		,11370 ,146.35 ,139500,0.0000126,	//7X2mm
	"LBGJ-20(2)-20AC",19.14	,6.14	,24360 ,127.13 ,139500,0.0000126,	//3X2.85mm
	"LBGJ-20(3)-20AC",19.82	,6.25	,25230 ,131.63 ,139500,0.0000126,	//3X2.9mm
	"LBGJ-20(4)-20AC",21.21	,6.47	,26990 ,140.86 ,139500,0.0000126,	//3X3mm
	"LBGJ-25-20AC"	 ,24.13	,6.79	,29760 ,155.31 ,139500,0.0000126,
	"LBGJ-30(1)-20AC",28.86	,7.54	,35920 ,191.72 ,139500,0.0000126,	//3X3.5mm
	"LBGJ-30(2)-20AC",29.08	,6.9	,35070 ,193.58 ,139500,0.0000126,	//7X2.3mm
	"LBGJ-35-20AC"	 ,33.13	,8.08	,39660 ,220.09 ,139500,0.0000126,
	"LBGJ-35-20AC"   ,37.17	,7.8	,44820 ,247.38 ,139500,0.0000126,
	"LBGJ-40-20AC"	 ,41.58	,8.25	,50140 ,276.72 ,139500,0.0000126,
	"LBGJ-45-20AC"	 ,46.24	,8.7	,55760 ,307.76 ,139500,0.0000126,
	"LBGJ-50-20AC" 	 ,49.48	,9		,59670 ,329.32 ,139500,0.0000126,
	"LBGJ-55-20AC"	 ,56.3	,9.6	,67890 ,374.71 ,139500,0.0000126,
	"LBGJ-65-20AC"	 ,67.35	,10.5	,79400 ,448.24 ,139500,0.0000126,
	"LBGJ-75-20AC"	 ,77.31	,11.25	,87670 ,514.55 ,139500,0.0000126,
	"LBGJ-70-20AC"	 ,72.23	,11		,87100 ,482.61 ,139500,0.0000126,
	"LBGJ-80-20AC"	 ,78.94	,11.5	,95200 ,527.5  ,139500,0.0000126,
	"LBGJ-100-20AC"	 ,100.88,13		,121660,674.12 ,139500,0.0000126,
	"LBGJ-115-20AC"	 ,112.85,13.75	,136100,754.07 ,139500,0.0000126,
	"LBGJ-120-20AC"	 ,121.21,14.25	,146180,809.94 ,139500,0.0000126,
	"LBGJ-125-20AC"	 ,125.5	,14.5	,151350,838.65 ,139500,0.0000126,
	"LBGJ-150-20AC"	 ,148.07,15.75	,178570,989.5  ,139500,0.0000126,
	"LBGJ-180-20AC"	 ,182.8	,17.5	,215520,1221.46,139500,0.0000126,
	"LBGJ-210-20AC"	 ,209.85,18.75	,237970,1402.2 ,139500,0.0000126,
	"LBGJ-240-20AC"	 ,238.76,20		,260010,1595.42,139500,0.0000126,
	"LBGJ-195-20AC"	 ,196.44,18.2	,236910,1320.52,139500,0.0000126,
	"LBGJ-245-20AC"	 ,244.39,20.3	,294740,1642.82,139500,0.0000126,
	"LBGJ-295-20AC"	 ,297.57,22.4	,358870,2000.22,139500,0.0000126,
	"LBGJ-350-20AC"	 ,355.98,24.5	,419700,2392.72,139500,0.0000126,	
	"LBGJ-420-20AC"	 ,419.62,26.6	,468300,2820.69,139500,0.0000126,	
	"LBGJ-465-20AC"	 ,464.72,28		,506340,3125.25,139500,0.0000126,	
	"LBGJ-510-20AC"	 ,512.61,29.4	,544400,3445.66,139500,0.0000126,	
	"LBGJ-590-20AC"	 ,588.46,31.5	,598460,3955.53,139500,0.0000126,	
	"LBGJ-670-20AC"	 ,669.54,33.6	,662840,4500.5 ,139500,0.0000126,	
};
JLB_STRU LBGJ23AC_TBL[36]=
{
	"LBGJ-16-23AC"	 ,15.93	,5.6	,18160 ,100.67 ,134100,0.0000129,
	"LBGJ-18-23AC"	 ,17.82	,5.93	,20310 ,112.61 ,134100,0.0000129,
	"LBGJ-20(1)-23AC",21.99	,6		,10180 ,139.28 ,134100,0.0000129,	//7X2mm
	"LBGJ-20(2)-23AC",19.14	,6.14	,21820 ,120.96 ,134100,0.0000129,	//3X2.85mm
	"LBGJ-20(3)-23AC",19.82	,6.25	,22590 ,125.22 ,134100,0.0000129,	//3X2.9mm
	"LBGJ-20(4)-23AC",21.21	,6.47	,24170 ,134.02 ,134100,0.0000129,	//3X3mm
	"LBGJ-25-23AC"	 ,24.13	,6.79	,26650 ,147.75 ,134100,0.0000129,
	"LBGJ-30(1)-23AC",28.86	,7.54	,32630 ,182.41 ,134100,0.0000129,	//3X3.5mm
	"LBGJ-30(2)-23AC",29.08	,6.9	,31410 ,184.17 ,134100,0.0000129,	//7X2.3mm
	"LBGJ-35-23AC"	 ,33.13	,8.08	,36510 ,209.41 ,134100,0.0000129,
	"LBGJ-35-23AC"   ,37.17	,7.8	,40140 ,235.36 ,134100,0.0000129,
	"LBGJ-40-23AC"	 ,41.58	,8.25	,44900 ,263.29 ,134100,0.0000129,
	"LBGJ-45-23AC"	 ,46.24	,8.7	,49940 ,292.77 ,134100,0.0000129,
	"LBGJ-50-23AC" 	 ,49.48	,9		,53440 ,313.34 ,134100,0.0000129,
	"LBGJ-55-23AC"	 ,56.3	,9.6	,60800 ,356.54 ,134100,0.0000129,
	"LBGJ-65-23AC"	 ,67.35	,10.5	,72130 ,426.46 ,134100,0.0000129,
	"LBGJ-75-23AC"	 ,77.31	,11.25	,80710 ,489.6  ,134100,0.0000129,
	"LBGJ-70-23AC"	 ,72.23	,11		,78000 ,459.11 ,134100,0.0000129,
	"LBGJ-80-23AC"	 ,78.94	,11.5	,85260 ,501.88 ,134100,0.0000129,
	"LBGJ-100-23AC"	 ,100.88,13		,108950,641.37 ,134100,0.0000129,
	"LBGJ-115-23AC"	 ,112.85,13.75	,121880,717.47 ,134100,0.0000129,
	"LBGJ-120-23AC"	 ,121.21,14.25	,130910,770.64 ,134100,0.0000129,
	"LBGJ-125-23AC"	 ,125.5	,14.5	,135540,797.81 ,134100,0.0000129,
	"LBGJ-150-23AC"	 ,148.07,15.75	,159910,941.34 ,134100,0.0000129,
	"LBGJ-180-23AC"	 ,182.8	,17.5	,195780,1162.13,134100,0.0000129,
	"LBGJ-210-23AC"	 ,209.85,18.75	,219080,1334.17,134100,0.0000129,
	"LBGJ-240-23AC"	 ,238.76,20		,214880,1517.97,134100,0.0000129,
	"LBGJ-195-23AC"	 ,196.44,18.2	,212160,1256.36,134100,0.0000129,
	"LBGJ-245-23AC"	 ,244.39,20.3	,263940,1562.81,134100,0.0000129,
	"LBGJ-295-23AC"	 ,297.57,22.4	,321380,1903.23,134100,0.0000129,
	"LBGJ-350-23AC"	 ,355.98,24.5	,381260,2276.48,134100,0.0000129,	
	"LBGJ-420-23AC"	 ,419.62,26.6	,415430,2683.69,134100,0.0000129,	
	"LBGJ-465-23AC"	 ,464.72,28		,456120,2973.53,134100,0.0000129,	
	"LBGJ-510-23AC"	 ,512.61,29.4	,493650,3278.47,134100,0.0000129,	
	"LBGJ-590-23AC"	 ,588.46,31.5	,540210,3763.43,134100,0.0000129,	
	"LBGJ-670-23AC"	 ,669.54,33.6	,602580,4281.98,134100,0.0000129,	
};
JLB_STRU LBGJ27AC_TBL[36]=
{
	"LBGJ-16-27AC"	 ,15.93	,5.6	, 16040,94.89  ,126000,0.0000134,
	"LBGJ-18-27AC"	 ,17.82	,5.93	, 17940,106.14 ,126000,0.0000134,
	"LBGJ-20(1)-27AC",21.99	,6		,  8990,131.29 ,126000,0.0000134,	//7X2mm
	"LBGJ-20(2)-27AC",19.14	,6.14	, 19270,114	   ,126000,0.0000134,	//3X2.85mm
	"LBGJ-20(3)-27AC",19.82	,6.25	, 19950,118.06 ,126000,0.0000134,	//3X2.9mm
	"LBGJ-20(4)-27AC",21.21	,6.47	, 21350,126.34 ,126000,0.0000134,	//3X3mm
	"LBGJ-25-27AC"	 ,24.13	,6.79	, 23540,139.29 ,126000,0.0000134,
	"LBGJ-30(1)-27AC",28.86	,7.54	, 28240,171.94 ,126000,0.0000134,	//3X3.5mm
	"LBGJ-30(2)-27AC",29.08	,6.9	, 27750,173.57 ,126000,0.0000134,	//7X2.3mm
	"LBGJ-35-27AC"	 ,33.13	,8.08	, 32110,191.38 ,126000,0.0000134,
	"LBGJ-35-27AC"   ,37.17	,7.8	, 35460,221.86 ,126000,0.0000134,
	"LBGJ-40-27AC"	 ,41.58	,8.25	, 39660,248.16 ,126000,0.0000134,
	"LBGJ-45-27AC"	 ,46.24	,8.7	, 44110,276.01 ,126000,0.0000134,
	"LBGJ-50-27AC" 	 ,49.48	,9		, 47200,295.38 ,126000,0.0000134,
	"LBGJ-55-27AC"	 ,56.3	,9.6	, 53710,336.04 ,126000,0.0000134,
	"LBGJ-65-27AC"	 ,67.35	,10.5	, 62430,402	   ,126000,0.0000134,
	"LBGJ-75-27AC"	 ,77.31	,11.25	, 70970,461.46 ,126000,0.0000134,
	"LBGJ-70-27AC"	 ,72.23	,11		, 68900,432.91 ,126000,0.0000134,
	"LBGJ-80-27AC"	 ,78.94	,11.5	, 75310,472.98 ,126000,0.0000134,
	"LBGJ-100-27AC"	 ,100.88,13		, 96240,604.57 ,126000,0.0000134,
	"LBGJ-115-27AC"	 ,112.85,13.75	,107660,676.24 ,126000,0.0000134,
	"LBGJ-120-27AC"	 ,121.21,14.25	,115630,726.33 ,126000,0.0000134,
	"LBGJ-125-27AC"	 ,125.5	,14.5	,119730,752.14 ,126000,0.0000134,
	"LBGJ-150-27AC"	 ,148.07,15.75	,141260,887.39 ,126000,0.0000134,
	"LBGJ-180-27AC"	 ,182.8	,17.5	,169460,1095.46,126000,0.0000134,
	"LBGJ-210-27AC"	 ,209.85,18.75	,192640,1257.49,126000,0.0000134,
	"LBGJ-240-27AC"	 ,238.76,20		,191250,1430.89,126000,0.0000134,
	"LBGJ-195-27AC"	 ,196.44,18.2	,187410,1184.28,126000,0.0000134,
	"LBGJ-245-27AC"	 ,244.39,20.3	,233150,1473.37,126000,0.0000134,
	"LBGJ-295-27AC"	 ,297.57,22.4	,283880,1793.78,126000,0.0000134,
	"LBGJ-350-27AC"	 ,355.98,24.5	,330000,2145.9 ,126000,0.0000134,	
	"LBGJ-420-27AC"	 ,419.62,26.6	,377660,2529.71,126000,0.0000134,	
	"LBGJ-465-27AC"	 ,464.72,28		,405910,2802.95,126000,0.0000134,	
	"LBGJ-510-27AC"	 ,512.61,29.4	,438280,3090.15,126000,0.0000134,	
	"LBGJ-590-27AC"	 ,588.46,31.5	,487240,3547.18,126000,0.0000134,	
	"LBGJ-670-27AC"	 ,669.54,33.6	,536300,4035.92,126000,0.0000134,	
};
JLB_STRU LBGJ30AC_TBL[36]=
{
	"LBGJ-16-30AC"	 ,15.93	,5.6	, 13470,90.08  ,118800,0.0000138,
	"LBGJ-18-30AC"	 ,17.82	,5.93	, 15070,100.76 ,118800,0.0000138,
	"LBGJ-20(1)-30AC",21.99	,6		,  7550,124.57 ,118800,0.0000138,	//7X2mm
	"LBGJ-20(2)-30AC",19.14	,6.14	, 16180,108.23 ,118800,0.0000138,	//3X2.85mm
	"LBGJ-20(3)-30AC",19.82	,6.25	, 16750,112.07 ,118800,0.0000138,	//3X2.9mm
	"LBGJ-20(4)-30AC",21.21	,6.47	, 17930,119.9  ,118800,0.0000138,	//3X3mm
	"LBGJ-25-30AC"	 ,24.13	,6.79	, 19770,132.21 ,118800,0.0000138,
	"LBGJ-30(1)-30AC",28.86	,7.54	, 23860,163.21 ,118800,0.0000138,	//3X3.5mm
	"LBGJ-30(2)-30AC",29.08	,6.9	, 23300,164.8  ,118800,0.0000138,	//7X2.3mm
	"LBGJ-35-30AC"	 ,33.13	,8.08	, 26440,187.37 ,118800,0.0000138,
	"LBGJ-35-30AC"   ,37.17	,7.8	, 29770,210.62 ,118800,0.0000138,
	"LBGJ-40-30AC"	 ,41.58	,8.25	, 33300,235.57 ,118800,0.0000138,
	"LBGJ-45-30AC"	 ,46.24	,8.7	, 37040,262.01 ,118800,0.0000138,
	"LBGJ-50-30AC" 	 ,49.48	,9		, 39630,280.33 ,118800,0.0000138,
	"LBGJ-55-30AC"	 ,56.3	,9.6	, 45090,319	   ,118800,0.0000138,
	"LBGJ-65-30AC"	 ,67.35	,10.5	, 52730,381.57 ,118800,0.0000138,
	"LBGJ-75-30AC"	 ,77.31	,11.25	, 58450,438.06 ,118800,0.0000138,
	"LBGJ-70-30AC"	 ,72.23	,11		, 57850,410.94 ,118800,0.0000138,
	"LBGJ-80-30AC"	 ,78.94	,11.5	, 63230,449.09 ,118800,0.0000138,
	"LBGJ-100-30AC"	 ,100.88,13		, 80800,573.93 ,118800,0.0000138,
	"LBGJ-115-30AC"	 ,112.85,13.75	, 90390,641.94 ,118800,0.0000138,
	"LBGJ-120-30AC"	 ,121.21,14.25	, 97070,689.53 ,118800,0.0000138,
	"LBGJ-125-30AC"	 ,125.5	,14.5	,100520,714	   ,118800,0.0000138,
	"LBGJ-150-30AC"	 ,148.07,15.75	,118600,842.31 ,118800,0.0000138,
	"LBGJ-180-30AC"	 ,182.8	,17.5	,143130,1039.79,118800,0.0000138,
	"LBGJ-210-30AC"	 ,209.85,18.75	,158650,1193.72,118800,0.0000138,
	"LBGJ-240-30AC"	 ,238.76,20		,156870,1358.25,118800,0.0000138,
	"LBGJ-195-30AC"	 ,196.44,18.2	,157350,1124.27,118800,0.0000138,
	"LBGJ-245-30AC"	 ,244.39,20.3	,195760,1398.64,118800,0.0000138,
	"LBGJ-295-30AC"	 ,297.57,22.4	,238350,1702.83,118800,0.0000138,
	"LBGJ-350-30AC"	 ,355.98,24.5	,278730,2036.83,118800,0.0000138,	
	"LBGJ-420-30AC"	 ,419.62,26.6	,309680,2401.02,118800,0.0000138,	
	"LBGJ-465-30AC"	 ,464.72,28		,330580,2660.67,118800,0.0000138,	
	"LBGJ-510-30AC"	 ,512.61,29.4	,364470,2933.15,118800,0.0000138,	
	"LBGJ-590-30AC"	 ,588.46,31.5	,402510,3367.16,118800,0.0000138,	
	"LBGJ-670-30AC"	 ,669.54,33.6	,439890,3831.36,118800,0.0000138,	
};
JLB_STRU LBGJ33AC_TBL[36]=
{
	"LBGJ-16-33AC"	 ,15.93	,5.6	, 11950, 84.28 ,111600,0.0000144,
	"LBGJ-18-33AC"	 ,17.82	,5.93	, 13370, 94.29 ,111600,0.0000144,
	"LBGJ-20(1)-33AC",21.99	,6		,  6700,116.58 ,111600,0.0000144,	//7X2mm
	"LBGJ-20(2)-33AC",19.14	,6.14	, 14360,101.27 ,111600,0.0000144,	//3X2.85mm
	"LBGJ-20(3)-33AC",19.82	,6.25	, 14870,104.87 ,111600,0.0000144,	//3X2.9mm
	"LBGJ-20(4)-33AC",21.21	,6.47	, 15910,112.22 ,111600,0.0000144,	//3X3mm
	"LBGJ-25-33AC"	 ,24.13	,6.79	, 17550,123.71 ,111600,0.0000144,
	"LBGJ-30(1)-33AC",28.86	,7.54	, 21390,152.74 ,111600,0.0000144,	//3X3.5mm
	"LBGJ-30(2)-33AC",29.08	,6.9	, 20680,154.2  ,111600,0.0000144,	//7X2.3mm
	"LBGJ-35-33AC"	 ,33.13	,8.08	, 23920,175.33 ,111600,0.0000144,
	"LBGJ-35-33AC"   ,37.17	,7.8	, 26420,197.04 ,111600,0.0000144,
	"LBGJ-40-33AC"	 ,41.58	,8.25	, 29560,220.44 ,111600,0.0000144,
	"LBGJ-45-33AC"	 ,46.24	,8.7	, 32870,245.19 ,111600,0.0000144,
	"LBGJ-50-33AC" 	 ,49.48	,9		, 35180,262.37 ,111600,0.0000144,
	"LBGJ-55-33AC"	 ,56.3	,9.6	, 40030,298.5  ,111600,0.0000144,
	"LBGJ-65-33AC"	 ,67.35	,10.5	, 47280,357.11 ,111600,0.0000144,
	"LBGJ-75-33AC"	 ,77.31	,11.25	, 52880,409.92 ,111600,0.0000144,
	"LBGJ-70-33AC"	 ,72.23	,11		, 51350,384.55 ,111600,0.0000144,
	"LBGJ-80-33AC"	 ,78.94	,11.5	, 56130,420.19 ,111600,0.0000144,
	"LBGJ-100-33AC"	 ,100.88,13		, 71720,536.94 ,111600,0.0000144,
	"LBGJ-115-33AC"	 ,112.85,13.75	, 80240,600.71 ,111600,0.0000144,
	"LBGJ-120-33AC"	 ,121.21,14.25	, 86180,645.22 ,111600,0.0000144,
	"LBGJ-125-33AC"	 ,125.5	,14.5	, 89230,668.14 ,111600,0.0000144,
	"LBGJ-150-33AC"	 ,148.07,15.75	,105280,788.17 ,111600,0.0000144,
	"LBGJ-180-33AC"	 ,182.8	,17.5	,128330,973.13 ,111600,0.0000144,
	"LBGJ-210-33AC"	 ,209.85,18.75	,143540,1117.04,111600,0.0000144,
	"LBGJ-240-33AC"	 ,238.76,20		,141820,1270.98,111600,0.0000144,
	"LBGJ-195-33AC"	 ,196.44,18.2	,139670,1051.81,111600,0.0000144,
	"LBGJ-245-33AC"	 ,244.39,20.3	,173760,1308.82,111600,0.0000144,
	"LBGJ-295-33AC"	 ,297.57,22.4	,211570,1593.38,111600,0.0000144,
	"LBGJ-350-33AC"	 ,355.98,24.5	,249900,1906.25,111600,0.0000144,	
	"LBGJ-420-33AC"	 ,419.62,26.6	,279470,2247.04,111600,0.0000144,	
	"LBGJ-465-33AC"	 ,464.72,28		,301290,2489.71,111600,0.0000144,	
	"LBGJ-510-33AC"	 ,512.61,29.4	,327560,2745.21,111600,0.0000144,	
	"LBGJ-590-33AC"	 ,588.46,31.5	,365430,3151.29,111600,0.0000144,	
	"LBGJ-670-33AC"	 ,669.54,33.6	,397700,3585.3 ,111600,0.0000144,	
};
JLB_STRU LBGJ40AC_TBL[36]=
{
	"LBGJ-16-33AC"	 ,15.93	,5.6	, 10440,74.51  ,98100,0.0000155,
	"LBGJ-18-33AC"	 ,17.82	,5.93	, 11680,83.34  ,98100,0.0000155,
	"LBGJ-20(1)-33AC",21.99	,6		,  5850,103.08 ,98100,0.0000155,	//7X2mm
	"LBGJ-20(2)-33AC",19.14	,6.14	, 12550,89.51  ,98100,0.0000155,	//3X2.85mm
	"LBGJ-20(3)-33AC",19.82	,6.25	, 12990,92.69  ,98100,0.0000155,	//3X2.9mm
	"LBGJ-20(4)-33AC",21.21	,6.47	, 13900,99.1   ,98100,0.0000155,	//3X3mm
	"LBGJ-25-33AC"	 ,24.13	,6.79	, 15330,109.35 ,98100,0.0000155,
	"LBGJ-30(1)-33AC",28.86	,7.54	, 18650,135	   ,98100,0.0000155,	//3X3.5mm
	"LBGJ-30(2)-33AC",29.08	,6.9	, 18060,136.31 ,98100,0.0000155,	//7X2.3mm
	"LBGJ-35-33AC"	 ,33.13	,8.08	, 21090,154.98 ,98100,0.0000155,
	"LBGJ-35-33AC"   ,37.17	,7.8	, 23080,174.2  ,98100,0.0000155,
	"LBGJ-40-33AC"	 ,41.58	,8.25	, 25820,194.85 ,98100,0.0000155,
	"LBGJ-45-33AC"	 ,46.24	,8.7	, 28710,216.7  ,98100,0.0000155,
	"LBGJ-50-33AC" 	 ,49.48	,9		, 30730,231.9  ,98100,0.0000155,
	"LBGJ-55-33AC"	 ,56.3	,9.6	, 34960,263.85 ,98100,0.0000155,
	"LBGJ-65-33AC"	 ,67.35	,10.5	, 41220,315.6  ,98100,0.0000155,
	"LBGJ-75-33AC"	 ,77.31	,11.25	, 46620,362.34 ,98100,0.0000155,
	"LBGJ-70-33AC"	 ,72.23	,11		, 44850,339.85 ,98100,0.0000155,
	"LBGJ-80-33AC"	 ,78.94	,11.5	, 49020,371.45 ,98100,0.0000155,
	"LBGJ-100-33AC"	 ,100.88,13		, 62640,474.71 ,98100,0.0000155,
	"LBGJ-115-33AC"	 ,112.85,13.75	, 70080,530.97 ,98100,0.0000155,
	"LBGJ-120-33AC"	 ,121.21,14.25	, 75270,570.27 ,98100,0.0000155,
	"LBGJ-125-33AC"	 ,125.5	,14.5	, 77930,590.5  ,98100,0.0000155,
	"LBGJ-150-33AC"	 ,148.07,15.75	, 91950,696.66 ,98100,0.0000155,
	"LBGJ-180-33AC"	 ,182.8	,17.5	,111870,860.03 ,98100,0.0000155,
	"LBGJ-210-33AC"	 ,209.85,18.75	,126540,987.38 ,98100,0.0000155,
	"LBGJ-240-33AC"	 ,238.76,20		,124630,1123.4 ,98100,0.0000155,
	"LBGJ-195-33AC"	 ,196.44,18.2	,121990,929.91 ,98100,0.0000155,
	"LBGJ-245-33AC"	 ,244.39,20.3	,151770,1156.73,98100,0.0000155,
	"LBGJ-295-33AC"	 ,297.57,22.4	,184790,1408.46,98100,0.0000155,
	"LBGJ-350-33AC"	 ,355.98,24.5	,217860,1684.71,98100,0.0000155,	
	"LBGJ-420-33AC"	 ,419.62,26.6	,249260,1985.88,98100,0.0000155,	
	"LBGJ-465-33AC"	 ,464.72,28		,276180,2200.62,98100,0.0000155,	
	"LBGJ-510-33AC"	 ,512.61,29.4	,304490,2425.93,98100,0.0000155,	
	"LBGJ-590-33AC"	 ,588.46,31.5	,323060,2785.21,98100,0.0000155,	
	"LBGJ-670-33AC"	 ,669.54,33.6	,349500,3168.65,98100,0.0000155,	
};
//镀锌钢绞线（XX-XX-GJ- GB/T 1200-1988）
GJ_STRU GJ_TBL[23]=
{
	"5.5-1X7",1.00, 3.0,  5.50,  6.46,  6.98,  7.54,  8.08,  8.64, 43.7,196000,1.2e-5,
	"7.92-1X7",1.20, 3.6,  7.92,  9.31, 10.06, 10.85, 11.64, 12.43, 62.9,196000,1.2e-5,
	"10.78-1X7",1.40, 4.2, 10.78, 12.67, 13.69, 14.77, 15.85, 16.92, 85.6,196000,1.2e-5,
	"14.07-1X7",1.60, 4.8, 14.07, 16.53, 17.87, 19.28, 20.68, 22.09,111.7,196000,1.2e-5,
	"17.81-1X7",1.80, 5.4, 17.81, 20.93, 22.62, 24.40, 26.18, 27.96,141.4,196000,1.2e-5,
	"21.99-1X7",2.00, 6.0, 21.99, 25.84, 29.73, 30.13, 32.32, 34.52,174.6,196000,1.2e-5,
	"29.08-1X7",2.30, 6.9, 29.08, 34.17, 36.93, 39.84, 42.75, 45.66,230.9,196000,1.2e-5,
	"37.17-1X7",2.60, 7.8, 37.17, 43.60, 47.20, 50.92, 54.63, 58.35,295.1,196000,1.2e-5,
	"46.24-1X7",2.90, 8.7, 46.24, 54.33, 58.72, 63.35, 67.97, 72.60,367.1,196000,1.2e-5,
	"49.46-1X7",3.00, 9.0, 49.46, 58.12, 62.81, 67.76, 72.71, 77.65,392.7,196000,1.2e-5,	//*自己添加的
	"56.30-1X7",3.20, 9.6, 56.30, 66.15, 71.50, 77.13, 82.76, 88.39,447.0,196000,1.2e-5,
	"67.35-1X7",3.50,10.5, 67.35, 79.14, 85.85, 92.27, 99.00,105.74,534.8,196000,1.2e-5,
	"79.39-1X7",3.80,11.4, 79.39, 93.28,100.82,108.76,116.70,124.64,630.4,196000,1.2e-5,
	"87.96-1X7",4.00,12.0, 87.96,103.35,111.71,120.50,129.30,138.10,698.4,196000,1.2e-5,
	"38.20-1X19",1.60, 8.0, 38.20, 44.88, 48.51, 52.33, 56.15, 59.97,304.0,196000,1.2e-5,
	"48.35-1X19",1.80, 9.0, 48.35, 56.81, 61.40, 66.24, 71.07, 75.91,384.9,196000,1.2e-5,
	"59.69-1X19",2.00,10.0, 59.69, 70.14, 75.81, 81.78, 87.74, 93.71,475.1,196000,1.2e-5,
	"78.94-1X19",2.30,11.5, 78.94, 92.75,100.25,108.15,116.04,123.94,628.4,196000,1.2e-5,
	"100.88-1X19",2.60,13.0,100.88,118.53,128.12,138.20,148.29,158.38,803.0,196000,1.2e-5,
	"125.50-1X19",2.90,14.5,125.50,147.46,159.38,171.93,184.48,197.03,999.0,196000,1.2e-5,
	"152.81-1X19",3.20,16.0,152.81,179.55,194.06,209.35,224.63,239.91,1216.4,196000,1.2e-5,
	"182.80-1X19",3.50,17.5,182.80,214.79,232.16,250.44,268.72,287.00,1455.1,196000,1.2e-5,
	"238.76-1X19",4.00,20.0,238.76,280.54,303.23,327.10,350.98,374.86,1900.5,196000,1.2e-5,
};
void CWireLoadDatabase::InitDatabase()
{
	DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
	if(m_pRootWireTypesStg == NULL)
	{
		if(m_pRootStg->OpenStorage(_bstr_t("WireTypes"),NULL,grfMode,NULL,0,&m_pRootWireTypesStg)!=S_OK)
			m_pRootStg->CreateStorage(_bstr_t("WireTypes"),grfMode,0,0,&m_pRootWireTypesStg);
	}
	if(m_pRootWeatherRegionsStg == NULL)
	{
		if(m_pRootStg->OpenStorage(_bstr_t("WeatherRegions"),NULL,grfMode,NULL,0,&m_pRootWeatherRegionsStg)!=S_OK)
			m_pRootStg->CreateStorage(_bstr_t("WeatherRegions"),grfMode,0,0,&m_pRootWeatherRegionsStg);
	}
	int i;
	DIANXIAN_STRU wire;
	for(i=0;i<g_nLGJ_NUM;i++)
	{
		strcpy(wire.specWire,"LGJ-");
		strcat(wire.specWire,LGJ_TBL[i].mark_area);	//标称截面铝/钢（mm2）
		wire.m_fDiameter=LGJ_TBL[i].dia_sum;		//导线外径（mm）
		wire.m_fArea=LGJ_TBL[i].area_LX+LGJ_TBL[i].area_GX;	//计算面积铝/钢（mm2）
		wire.m_fForcePermit=LGJ_TBL[i].force_permit*0.95;	//计算拉断力（N）
		wire.m_fUnitWeight=LGJ_TBL[i].unit_weight;		//参考计算质量（kg/km）
		wire.m_fElasticCoef=LGJ_TBL[i].E;				//最终弹性系数（MPa）
		wire.m_fExpansionCoef=LGJ_TBL[i].a;				//线膨胀系数（1/°C）
		SetWire("LGJ",wire.specWire,wire);
	}
	for(i=0;i<g_nLGJQ_NUM;i++)
	{
		strcpy(wire.specWire,"LGJQ-");
		strcat(wire.specWire,LGJQ_TBL[i].mark_area);	//标称截面铝/钢（mm2）
		wire.m_fDiameter=LGJQ_TBL[i].dia_sum;		//导线外径（mm）
		wire.m_fArea=LGJQ_TBL[i].area_LX+LGJQ_TBL[i].area_GX;	//计算面积铝/钢（mm2）
		wire.m_fForcePermit=LGJQ_TBL[i].force_permit*0.95;	//计算拉断力（N）
		wire.m_fUnitWeight=LGJQ_TBL[i].unit_weight;		//参考计算质量（kg/km）
		wire.m_fElasticCoef=LGJQ_TBL[i].E;				//最终弹性系数（MPa）
		wire.m_fExpansionCoef=LGJQ_TBL[i].a;				//线膨胀系数（1/°C）
		SetWire("LGJQ",wire.specWire,wire);
	}
	for(i=0;i<g_nJLB_NUM;i++)
	{
		strcpy(wire.specWire,JLB_TBL[i].mark);		//标称总面积（mm2）
		wire.m_fDiameter=JLB_TBL[i].dia_sum;		//导线外径（mm）
		wire.m_fArea=JLB_TBL[i].area_sum;			//计算面积铝/钢（mm2）
		wire.m_fForcePermit=JLB_TBL[i].force_permit;//计算拉断力（N）*0.95系数已在数据中考虑
		wire.m_fUnitWeight=JLB_TBL[i].unit_weight;	//参考计算质量（kg/km）
		wire.m_fElasticCoef=JLB_TBL[i].E;			//最终弹性系数（MPa）
		wire.m_fExpansionCoef=JLB_TBL[i].a;			//线膨胀系数（1/°C）
		SetWire("JLB",wire.specWire,wire);
	}
	//铝包钢绞LBGJ-XX-20AC
	for(i=0;i<36;i++)
	{
		strcpy(wire.specWire,LBGJ20AC_TBL[i].mark);		//标称总面积（mm2）
		wire.m_fDiameter=LBGJ20AC_TBL[i].dia_sum;		//导线外径（mm）
		wire.m_fArea=LBGJ20AC_TBL[i].area_sum;			//计算面积铝/钢（mm2）
		wire.m_fForcePermit=LBGJ20AC_TBL[i].force_permit*0.95;//计算拉断力（N）*0.95系数已在数据中考虑
		wire.m_fUnitWeight=LBGJ20AC_TBL[i].unit_weight;	//参考计算质量（kg/km）
		wire.m_fElasticCoef=LBGJ20AC_TBL[i].E;			//最终弹性系数（MPa）
		wire.m_fExpansionCoef=LBGJ20AC_TBL[i].a;		//线膨胀系数（1/°C）
		SetWire("LBGJ-20AC",wire.specWire,wire);
	}
	//铝包钢绞LBGJ-XX-23AC
	for(i=0;i<36;i++)
	{
		strcpy(wire.specWire,LBGJ23AC_TBL[i].mark);		//标称总面积（mm2）
		wire.m_fDiameter=LBGJ23AC_TBL[i].dia_sum;		//导线外径（mm）
		wire.m_fArea=LBGJ23AC_TBL[i].area_sum;			//计算面积铝/钢（mm2）
		wire.m_fForcePermit=LBGJ23AC_TBL[i].force_permit*0.95;//计算拉断力（N）*0.95系数已在数据中考虑
		wire.m_fUnitWeight=LBGJ23AC_TBL[i].unit_weight;	//参考计算质量（kg/km）
		wire.m_fElasticCoef=LBGJ23AC_TBL[i].E;			//最终弹性系数（MPa）
		wire.m_fExpansionCoef=LBGJ23AC_TBL[i].a;		//线膨胀系数（1/°C）
		SetWire("LBGJ-23AC",wire.specWire,wire);
	}
	//铝包钢绞LBGJ-XX-27AC
	for(i=0;i<36;i++)
	{
		strcpy(wire.specWire,LBGJ27AC_TBL[i].mark);		//标称总面积（mm2）
		wire.m_fDiameter=LBGJ27AC_TBL[i].dia_sum;		//导线外径（mm）
		wire.m_fArea=LBGJ27AC_TBL[i].area_sum;			//计算面积铝/钢（mm2）
		wire.m_fForcePermit=LBGJ27AC_TBL[i].force_permit*0.95;//计算拉断力（N）*0.95系数已在数据中考虑
		wire.m_fUnitWeight=LBGJ27AC_TBL[i].unit_weight;	//参考计算质量（kg/km）
		wire.m_fElasticCoef=LBGJ27AC_TBL[i].E;			//最终弹性系数（MPa）
		wire.m_fExpansionCoef=LBGJ27AC_TBL[i].a;		//线膨胀系数（1/°C）
		SetWire("LBGJ-27AC",wire.specWire,wire);
	}
	//铝包钢绞LBGJ-XX-30AC
	for(i=0;i<36;i++)
	{
		strcpy(wire.specWire,LBGJ30AC_TBL[i].mark);		//标称总面积（mm2）
		wire.m_fDiameter=LBGJ30AC_TBL[i].dia_sum;		//导线外径（mm）
		wire.m_fArea=LBGJ30AC_TBL[i].area_sum;			//计算面积铝/钢（mm2）
		wire.m_fForcePermit=LBGJ30AC_TBL[i].force_permit*0.95;//计算拉断力（N）*0.95系数已在数据中考虑
		wire.m_fUnitWeight=LBGJ30AC_TBL[i].unit_weight;	//参考计算质量（kg/km）
		wire.m_fElasticCoef=LBGJ30AC_TBL[i].E;			//最终弹性系数（MPa）
		wire.m_fExpansionCoef=LBGJ30AC_TBL[i].a;		//线膨胀系数（1/°C）
		SetWire("LBGJ-30AC",wire.specWire,wire);
	}
	//铝包钢绞LBGJ-XX-33AC
	for(i=0;i<36;i++)
	{
		strcpy(wire.specWire,LBGJ33AC_TBL[i].mark);		//标称总面积（mm2）
		wire.m_fDiameter=LBGJ33AC_TBL[i].dia_sum;		//导线外径（mm）
		wire.m_fArea=LBGJ33AC_TBL[i].area_sum;			//计算面积铝/钢（mm2）
		wire.m_fForcePermit=LBGJ33AC_TBL[i].force_permit*0.95;//计算拉断力（N）*0.95系数已在数据中考虑
		wire.m_fUnitWeight=LBGJ33AC_TBL[i].unit_weight;	//参考计算质量（kg/km）
		wire.m_fElasticCoef=LBGJ33AC_TBL[i].E;			//最终弹性系数（MPa）
		wire.m_fExpansionCoef=LBGJ33AC_TBL[i].a;		//线膨胀系数（1/°C）
		SetWire("LBGJ-33AC",wire.specWire,wire);
	}
	//铝包钢绞LBGJ-XX-40AC
	for(i=0;i<36;i++)
	{
		strcpy(wire.specWire,LBGJ40AC_TBL[i].mark);		//标称总面积（mm2）
		wire.m_fDiameter=LBGJ40AC_TBL[i].dia_sum;		//导线外径（mm）
		wire.m_fArea=LBGJ40AC_TBL[i].area_sum;			//计算面积铝/钢（mm2）
		wire.m_fForcePermit=LBGJ40AC_TBL[i].force_permit*0.95;//计算拉断力（N）*0.95系数已在数据中考虑
		wire.m_fUnitWeight=LBGJ40AC_TBL[i].unit_weight;	//参考计算质量（kg/km）
		wire.m_fElasticCoef=LBGJ40AC_TBL[i].E;			//最终弹性系数（MPa）
		wire.m_fExpansionCoef=LBGJ40AC_TBL[i].a;		//线膨胀系数（1/°C）
		SetWire("LBGJ-40AC",wire.specWire,wire);
	}
	for(i=0;i<g_nGJ_NUM;i++)
	{
		strcpy(wire.specWire,"GJ-");//int stru_type;	//0表示1X7，1表示1X19
		strcat(wire.specWire,GJ_TBL[i].mark);
		wire.m_fDiameter=GJ_TBL[i].dia_JX;	// 钢丝直径/绞线直径（mm）
		wire.m_fArea=GJ_TBL[i].area;	//断面积
		//整条钢绞线破断拉力=钢丝破断力总和X算系数，换算系数1 X 7结构为0.92；1X19结构为0.9
		if(i<14)
			wire.m_fForcePermit=GJ_TBL[i].sum_force_permit[1]*0.92*1000;//破断力总和（N）
		else
			wire.m_fForcePermit=GJ_TBL[i].sum_force_permit[1]*0.9*1000;//破断力总和（N）
		wire.m_fUnitWeight=GJ_TBL[i].unit_weight;		//参考计算质量（kg/km）
		wire.m_fElasticCoef=GJ_TBL[i].E;
		wire.m_fExpansionCoef=GJ_TBL[i].a;
		SetWire("GJ-1270",wire.specWire,wire);
	}
	WEATHER_REGION wr;
	//Ⅰ号气象区
	wr.m_nHighestT=40;	//最高气温
	wr.m_nLowestT=-5;	//最低气温
	wr.m_nAverageT=20;	//平均气温
	wr.m_nMaxWindT=10;	//大风气温
	wr.m_nMaxIceT=-5;	//覆冰气温
	wr.m_nSetupT=0;		//安装气温
	wr.m_fMaxWindV=35;	//最大风速
	wr.m_fMaxIceWindV=10;	//覆冰风速
	wr.m_fSetupWindV=10;//安装风速
	wr.m_fMaxIceThick=0;//最大覆冰
	SetWeatherRegion("典型Ⅰ",wr);
	//Ⅱ号气象区
	wr.m_nHighestT=40;	//最高气温
	wr.m_nLowestT=-10;	//最低气温
	wr.m_nAverageT=15;	//平均气温
	wr.m_nMaxWindT=10;	//大风气温
	wr.m_nMaxIceT=-5;	//覆冰气温
	wr.m_nSetupT=0;		//安装气温
	wr.m_fMaxWindV=35;	//最大风速
	wr.m_fMaxIceWindV=10;	//覆冰风速
	wr.m_fSetupWindV=10;//安装风速
	wr.m_fMaxIceThick=5;//最大覆冰
	SetWeatherRegion("典型Ⅱ",wr);
	//Ⅲ号气象区
	wr.m_nHighestT=40;	//最高气温
	wr.m_nLowestT=-10;	//最低气温
	wr.m_nAverageT=15;	//平均气温
	wr.m_nMaxWindT=-5;	//大风气温
	wr.m_nMaxIceT=-5;	//覆冰气温
	wr.m_nSetupT=-5;	//安装气温
	wr.m_fMaxWindV=30;	//最大风速
	wr.m_fMaxIceWindV=10;	//覆冰风速
	wr.m_fSetupWindV=10;//安装风速
	wr.m_fMaxIceThick=5;//最大覆冰
	SetWeatherRegion("典型Ⅲ",wr);
	//Ⅳ号气象区
	wr.m_nHighestT=40;	//最高气温
	wr.m_nLowestT=-20;	//最低气温
	wr.m_nAverageT=10;	//平均气温
	wr.m_nMaxWindT=-5;	//大风气温
	wr.m_nMaxIceT=-5;	//覆冰气温
	wr.m_nSetupT=-10;	//安装气温
	wr.m_fMaxWindV=25;	//最大风速
	wr.m_fMaxIceWindV=10;	//覆冰风速
	wr.m_fSetupWindV=10;//安装风速
	wr.m_fMaxIceThick=5;//最大覆冰
	SetWeatherRegion("典型Ⅳ",wr);
	//Ⅴ号气象区
	wr.m_nHighestT=40;	//最高气温
	wr.m_nLowestT=-10;	//最低气温
	wr.m_nAverageT=15;	//平均气温
	wr.m_nMaxWindT=10;	//大风气温
	wr.m_nMaxIceT=-5;	//覆冰气温
	wr.m_nSetupT=-5;	//安装气温
	wr.m_fMaxWindV=25;	//最大风速
	wr.m_fMaxIceWindV=10;	//覆冰风速
	wr.m_fSetupWindV=10;//安装风速
	wr.m_fMaxIceThick=10;//最大覆冰
	SetWeatherRegion("典型Ⅴ",wr);
	//Ⅵ号气象区
	wr.m_nHighestT=40;	//最高气温
	wr.m_nLowestT=-20;	//最低气温
	wr.m_nAverageT=10;	//平均气温
	wr.m_nMaxWindT=-5;	//大风气温
	wr.m_nMaxIceT=-5;	//覆冰气温
	wr.m_nSetupT=-10;		//安装气温
	wr.m_fMaxWindV=30;	//最大风速
	wr.m_fMaxIceWindV=10;	//覆冰风速
	wr.m_fSetupWindV=10;//安装风速
	wr.m_fMaxIceThick=10;//最大覆冰
	SetWeatherRegion("典型Ⅵ",wr);
	//Ⅶ号气象区
	wr.m_nHighestT=40;	//最高气温
	wr.m_nLowestT=-40;	//最低气温
	wr.m_nAverageT=-5;	//平均气温
	wr.m_nMaxWindT=-5;	//大风气温
	wr.m_nMaxIceT=-5;	//覆冰气温
	wr.m_nSetupT=-15;	//安装气温
	wr.m_fMaxWindV=25;	//最大风速
	wr.m_fMaxIceWindV=10;	//覆冰风速
	wr.m_fSetupWindV=10;//安装风速
	wr.m_fMaxIceThick=10;//最大覆冰
	SetWeatherRegion("典型Ⅶ",wr);
	//Ⅷ号气象区
	wr.m_nHighestT=40;	//最高气温
	wr.m_nLowestT=-20;	//最低气温
	wr.m_nAverageT=10;	//平均气温
	wr.m_nMaxWindT=-5;	//大风气温
	wr.m_nMaxIceT=-5;	//覆冰气温
	wr.m_nSetupT=-10;	//安装气温
	wr.m_fMaxWindV=30;	//最大风速
	wr.m_fMaxIceWindV=15;	//覆冰风速
	wr.m_fSetupWindV=10;//安装风速
	wr.m_fMaxIceThick=15;//最大覆冰
	SetWeatherRegion("典型Ⅷ",wr);
	//Ⅸ号气象区
	wr.m_nHighestT=40;	//最高气温
	wr.m_nLowestT=-20;	//最低气温
	wr.m_nAverageT=10;	//平均气温
	wr.m_nMaxWindT=-5;	//大风气温
	wr.m_nMaxIceT=-5;	//覆冰气温
	wr.m_nSetupT=-10;	//安装气温
	wr.m_fMaxWindV=30;	//最大风速
	wr.m_fMaxIceWindV=15;	//覆冰风速
	wr.m_fSetupWindV=10;//安装风速
	wr.m_fMaxIceThick=20;//最大覆冰
	SetWeatherRegion("典型Ⅸ",wr);
}
