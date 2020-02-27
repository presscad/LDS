#pragma once
#include "Buffer.h"
#include "Tower.h"
#include "XhCharString.h"

class CDataSection
{
protected:
	DWORD m_dwBufSize;
	char* m_data;
public:
	CDataSection(){m_dwBufSize=0;m_data=NULL;}
	char* BufferPtr(){return m_data;}
	DWORD BufferLength(){return m_dwBufSize;}
};
class CBaseInfoSection:public CDataSection
{
public:	//公用属性
	PROPERTY(CXhChar50,m_sTaType);		//塔型名称
	GET(m_sTaType){
		return CXhChar50(m_data);
	}	
	SET(m_sTaType){
		memcpy(m_data,&value,50);
	}
	PROPERTY(CXhChar50,m_sPrjCode);		//工程代号
	GET(m_sPrjCode){
		return CXhChar50(m_data+50);
	}
	SET(m_sPrjCode){
		memcpy(m_data+50,&value,50);
	}
	PROPERTY(CXhChar50,m_sPrjName);		//工程名称
	GET(m_sPrjName){
		return CXhChar50(m_data+100);
	}
	SET(m_sPrjName){
		memcpy(m_data+100,&value,50);
	}
	PROPERTY(CXhChar50,m_sOperator);		//设计人员
	GET(m_sOperator){
		return CXhChar50(m_data+150);
	}
	SET(m_sOperator){
		memcpy(m_data+150,&value,50);
	}
	PROPERTY(CXhChar16,weatherCode);		//气象代码
	GET(weatherCode){
		return CXhChar16(m_data+200);
	}
	SET(weatherCode){
		memcpy(m_data+200,&value,16);
	}
	PROPERTY(short,volt_grade);		//电压等级
	GET(volt_grade){
		return *((short*)(m_data+216));	//2 Bytes
	}
	SET(volt_grade){
		memcpy(m_data+216,&value,2);
	}
	PROPERTY(char,m_cLandScape);		//塔位地貌
	GET(m_cLandScape){
		return *(m_data+218);	//1 Bytes
	}
	SET(m_cLandScape){
		memcpy(m_data+218,&value,1);
	}
	PROPERTY(char,m_cLineRoadType);		//线路状况
	GET(m_cLineRoadType){
		return *(m_data+219);	//1 Bytes
	}
	SET(m_cLineRoadType){
		memcpy(m_data+219,&value,1);
	}
	PROPERTY(double,m_fIceDensity);		//覆冰厚度
	GET(m_fIceDensity){
		return *((double*)(m_data+220));	//8 Bytes
	}
	SET(m_fIceDensity){
		memcpy(m_data+220,&value,8);
	}
	PROPERTY(double,m_fGamaG);		//持久荷载分项系数
	GET(m_fGamaG){
		return *((double*)(m_data+228));	//8 Bytes
	}
	SET(m_fGamaG){
		memcpy(m_data+228,&value,8);
	}
	PROPERTY(short,m_iSafetyGrade);		//安全等级
	GET(m_iSafetyGrade){
		return *((short*)(m_data+236));	//2 Bytes
	}
	SET(m_iSafetyGrade){
		memcpy(m_data+236,&value,2);
	}
public:
	CBaseInfoSection(char* buf=NULL,DWORD size=0);
	~CBaseInfoSection(){;}	
};

class CWeatherSection:public CDataSection
{
public:	//公用属性
	PROPERTY(double,m_fMaxIceThick);	//最大覆冰厚度
	GET(m_fMaxIceThick){
		return *((double*)(m_data));	//8 Bytes
	}
	SET(m_fMaxIceThick){
		memcpy(m_data,&value,8);
	}
	PROPERTY(double,m_fMaxIceThickCheck);	//验算覆冰厚度
	GET(m_fMaxIceThickCheck){
		return *((double*)(m_data+8));	//8 Bytes
	}
	SET(m_fMaxIceThickCheck){
		memcpy(m_data+8,&value,8);
	}
	PROPERTY(double,m_fMaxIceWindV);	//最大覆冰风速
	GET(m_fMaxIceWindV){
		return *((double*)(m_data+16));	//8 Bytes
	}
	SET(m_fMaxIceWindV){
		memcpy(m_data+16,&value,8);
	}
	PROPERTY(double,m_fMaxWindV);	//大风速度
	GET(m_fMaxWindV){
		return *((double*)(m_data+24));	//8 Bytes
	}
	SET(m_fMaxWindV){
		memcpy(m_data+24,&value,8);
	}
	PROPERTY(double,m_fMaxWindVCheck);	//验算风速
	GET(m_fMaxWindVCheck){
		return *((double*)(m_data+32));	//8 Bytes
	}
	SET(m_fMaxWindVCheck){
		memcpy(m_data+32,&value,8);
	}
	PROPERTY(double,m_fSetUpWindV);	//安装风速
	GET(m_fSetUpWindV){
		return *((double*)(m_data+40));	//8 Bytes
	}
	SET(m_fSetUpWindV){
		memcpy(m_data+40,&value,8);
	}
	PROPERTY(long,m_averageT);	//平均温度
	GET(m_averageT){
		return *((long*)(m_data+48));	//4 Bytes
	}
	SET(m_averageT){
		memcpy(m_data+48,&value,4);
	}
	PROPERTY(long,m_highestT);	//最高温度
	GET(m_highestT){
		return *((long*)(m_data+52));	//4 Bytes
	}
	SET(m_highestT){
		memcpy(m_data+52,&value,4);
	}

	PROPERTY(long,m_lowestT);	//最低温度
	GET(m_lowestT){
		return *((long*)(m_data+56));	//4 Bytes
	}
	SET(m_lowestT){
		memcpy(m_data+56,&value,4);
	}

	PROPERTY(long,m_maxIceT);	//最大覆冰温度
	GET(m_maxIceT){
		return *((long*)(m_data+60));	//4 Bytes
	}
	SET(m_maxIceT){
		memcpy(m_data+60,&value,4);
	}
	PROPERTY(long,m_maxWindT);	//最大风速温度
	GET(m_maxWindT){
		return *((long*)(m_data+64));	//4 Bytes
	}
	SET(m_maxWindT){
		memcpy(m_data+64,&value,4);
	}
	PROPERTY(long,m_SetUpT);	//安装温度
	GET(m_SetUpT){
		return *((long*)(m_data+68));	//4 Bytes
	}
	SET(m_SetUpT){
		memcpy(m_data+68,&value,4);
	}
public:
	CWeatherSection(char* buf=NULL,DWORD size=0);
	~CWeatherSection(){;}
};
class CWorkCaseSetSection:public CDataSection
{
public:
	CWorkCaseSetSection(char* buf=NULL,DWORD size=0);
	~CWorkCaseSetSection(){;}
	BOOL RetrieveWorkCaseSet(CTower* pModel);
};

class CWireLayoutSetSection : public CDataSection
{
public:
	CWireLayoutSetSection(char* buf=NULL,DWORD size=0);
	~CWireLayoutSetSection(){;}
	//暂时不需要在客户端提取线条荷载布置方案，将来有需要再加　wjh-2014.4.28
	//BOOL RetrieveWireLayoutSet(ATOM_LIST<CWireLayout> &listWireLayout);
};

class CNodeLoadSetSection : public CDataSection
{
public:
	CNodeLoadSetSection(char* buf=NULL,DWORD size=0);
	~CNodeLoadSetSection(){;}
	BOOL RetrieveWorkCaseActualNodeLoad(CTower* pModel);
};
class CWireLoadModelBuffer:public CBuffer
{
protected:
	double version;
public:
	CWireLoadModelBuffer():CBuffer(){;}
	CWireLoadModelBuffer(char* srcBuf, DWORD buf_size):CBuffer(srcBuf,buf_size){;}
	~CWireLoadModelBuffer(){};
	double Version(){return version;}
	static void InitWireLoadModelBuffer(CBuffer* pBuff,const WEATHER& weather);
public:
	CBaseInfoSection GetBaseInfoSection();
	CWeatherSection GetWeatherSection();
	CWorkCaseSetSection GetWorkCaseSetSection();
	CNodeLoadSetSection GetNodeLoadSetSection();
	CWireLayoutSetSection GetWireLayoutSetSection();
	void RetrieveWireLoadInfo(CTower *pModel);	//根据线条荷载缓存更新杆塔模型中的相关荷载数据
};