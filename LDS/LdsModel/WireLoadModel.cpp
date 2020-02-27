#include "stdafx.h"
#include "stdio.h"
#include "math.h"
#include "Tower.h"
#include "lds_part.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
#if defined(__LDS_)||defined(__TSA_)
#include "Resolver.h"
#include "WindLoad.h"
#endif

void WEATHER::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadInteger(&m_nHighestT);	//最高气温
	buffer.ReadInteger(&m_nLowestT);	//最低气温
	buffer.ReadInteger(&m_nAverageT);	//平均气温
	buffer.ReadDouble(&m_fMaxWindV);		//最大风速
	buffer.ReadInteger(&m_nMaxWindT);	//最大风速时温度
	buffer.ReadDouble(&m_fMaxIceThick);			//最大覆冰厚(mm)
	buffer.ReadInteger(&m_nMaxIceT);			//最大覆冰时温度
	buffer.ReadDouble(&m_fMaxIceWindV);			//最大覆冰时风速
	buffer.ReadInteger(&m_nSetupT);				//安装时温度
	buffer.ReadDouble(&m_fSetupWindV);	//安装时风速
}
void WEATHER::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(m_nHighestT);	//最高气温
	buffer.WriteInteger(m_nLowestT);	//最低气温
	buffer.WriteInteger(m_nAverageT);	//平均气温
	buffer.WriteDouble(m_fMaxWindV);	//最大风速
	buffer.WriteInteger(m_nMaxWindT);	//最大风速时温度
	buffer.WriteDouble(m_fMaxIceThick);	//最大覆冰厚(mm)
	buffer.WriteInteger(m_nMaxIceT);	//最大覆冰时温度
	buffer.WriteDouble(m_fMaxIceWindV);	//最大覆冰时风速
	buffer.WriteInteger(m_nSetupT);		//安装时温度
	buffer.WriteDouble(m_fSetupWindV);	//安装时风速
}
void WEATHER::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[50]="";
	fprintf(fp,"<天气信息>\n");
	fprintf(fp,"<最高气温 HighestT=\"%d\"/>\n",m_nHighestT);
	fprintf(fp,"<最低气温 LowestT=\"%d\"/>\n",m_nLowestT);
	fprintf(fp,"<平均气温 AvergeT=\"%d\"/>\n",m_nAverageT);
	sprintf(sText,"%f",m_fMaxWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<最大风速 MaxWindV=\"%s\"/>\n",sText);
	fprintf(fp,"<最大风速时温度 MaxWindT=\"%d\"/>\n",m_nMaxWindT);
	sprintf(sText,"%f",m_fMaxIceThick);
	SimplifiedNumString(sText);
	fprintf(fp,"<最大覆冰厚 MaxIceThick=\"%s\"/>\n",sText);
	fprintf(fp,"<最大覆冰时温度 MaxIceT=\"%d\"/>\n",m_nMaxIceT);
	sprintf(sText,"%f",m_fMaxIceWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<最大覆冰时风速 MaxIceWindV=\"%s\"/>\n",sText);
	fprintf(fp,"<安装时温度 SetupT=\"%d\"/>\n",m_nSetupT);
	sprintf(sText,"%f",m_fSetupWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<安装时风速 SetupWindV=\"%s\"/>\n",sText);
	fprintf(fp,"</天气信息>\n");
}
void WEATHER_REGION::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadString(weatherCode);
	WEATHER::FromBuffer(buffer,version);
}
void WEATHER_REGION::ToBuffer(CBuffer &buffer)
{
	buffer.WriteString(weatherCode);
	WEATHER::ToBuffer(buffer);
}
void WEATHER_REGION::ToXmlFile(FILE* fp,DWORD schema)
{
	fprintf(fp,"<气候区域 weatherCode=\"%s\">\n",weatherCode);
	WEATHER::ToXmlFile(fp,schema);
	fprintf(fp,"</气候区域>\n");
}
CWorkEnvLoad::CWorkEnvLoad()
{
	memset(this,0,sizeof(CWorkEnvLoad));
}
void CWorkEnvLoad::SumCal()
{
	m_fLoadX=m_fWireLoadX+m_fAttachmentLoadX;
	m_fLoadZ=m_fWireLoadZ+m_fAttachmentLoadZ+m_fSetupGa;
	m_fMinVertSpaceLoadX=m_fLoadX;
	m_fMinVertSpaceLoadY=m_fLoadY;
	if(m_fMinVertSpaceWireLoadZ==0)
		m_fMinVertSpaceWireLoadZ=m_fWireLoadZ;
	m_fMinVertSpaceLoadZ=m_fMinVertSpaceWireLoadZ+m_fAttachmentLoadZ+m_fSetupGa;
}
const DWORD HASHTABLESIZE = 500;
CStringKeyHashTable<long> CWireLoadModel::propHashtable;
CHashTable<DWORD> CWireLoadModel::propStatusHashtable;
void CWireLoadModel::InitModelPropHashtable()
{
	int id=1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(HASHTABLESIZE);
	propHashtable.SetValueAt("wireInfo",id++);
	propHashtable.SetValueAt("hangingInfo",id++);
	propHashtable.SetValueAt("hangingSide",id++);
	propHashtable.SetValueAt("m_cFormula",id++);
	propHashtable.SetValueAt("setupwireInfo",id++);
	propHashtable.SetValueAt("hangingWireType",id++);
	propHashtable.SetValueAt("hangingStyle",id++);
	propHashtable.SetValueAt("voltage",id++);
	propHashtable.SetValueAt("splitNumber",id++);
	propHashtable.SetValueAt("landHigh",id++);
	propHashtable.SetValueAt("attachmentSpec",id++);
	propHashtable.SetValueAt("attachmentWeight",id++);
	propHashtable.SetValueAt("m_fAttachmentDiameter",id++);
	propHashtable.SetValueAt("m_fAttachmentLength",id++);
	propHashtable.SetValueAt("anchorLineAngle",id++);
	propHashtable.SetValueAt("tractionLineAngle",id++);
	propHashtable.SetValueAt("wireType",id++);
	propHashtable.SetValueAt("wireSpec",id++);
	propHashtable.SetValueAt("wireDiameter",id++);
	propHashtable.SetValueAt("wireArea",id++);
	propHashtable.SetValueAt("unitWeight",id++);
	propHashtable.SetValueAt("forcePermit",id++);
	propHashtable.SetValueAt("saftyCoef",id++);
	propHashtable.SetValueAt("maxTension",id++);
	propHashtable.SetValueAt("elasticityCoef",id++);
	propHashtable.SetValueAt("expansionCoef",id++);
	propHashtable.SetValueAt("powerLineInfo",id++);
	propHashtable.SetValueAt("horiSpace",id++);
	propHashtable.SetValueAt("vertSpace",id++);
	propHashtable.SetValueAt("minVertSpace",id++);
	propHashtable.SetValueAt("representSpace",id++);
	propHashtable.SetValueAt("HtoL",id++);
	propHashtable.SetValueAt("allocHoriCoef",id++);
	propHashtable.SetValueAt("allocVertCoef",id++);
	propHashtable.SetValueAt("xianluAngle",id++);
	propHashtable.SetValueAt("m_nHighestT",id++);//最高气温
	propHashtable.SetValueAt("m_nLowestT",id++);	//最低气温
	propHashtable.SetValueAt("m_nAverageT",id++);//平均温度
	propHashtable.SetValueAt("m_nMaxWindT",id++);	//最大风速温度
	propHashtable.SetValueAt("m_fMaxWindV",id++);		//最大风速
	propHashtable.SetValueAt("m_fMaxWindVCheck",id++);	//最大风速验算
	propHashtable.SetValueAt("m_nMaxIceT",id++);	//最大覆冰温度
	propHashtable.SetValueAt("m_fMaxIceWindV",id++);//最大覆冰风速
	propHashtable.SetValueAt("m_fMaxIceThick",id++);			//最大覆冰厚度
	propHashtable.SetValueAt("m_fMaxIceThickCheck",id++);		//最大覆冰厚度验算
	propHashtable.SetValueAt("m_nSetupT",id++);	//安装温度
	propHashtable.SetValueAt("m_fSetupWindV",id++);	//安装风速
}

int CWireLoadModel::GetPropValueStr(long id,char *valueStr)				//得到属性值字符串
{
	char sText[100]="";
	if(GetPropID("hangingWireType")==id)
	{
		if(wireInfo.m_cXianType=='E')
			strcpy(sText,"地线");
		else if(wireInfo.m_cXianType=='C')
			strcpy(sText,"导线");
		else if(wireInfo.m_cXianType!=0)
			strcpy(sText,"跳线");
		else
			strcpy(sText,"未挂线");
	}
	else if(GetPropID("hangingStyle")==id)
	{
		if(wireInfo.m_bNaiZhang)
			strcpy(sText,"耐张挂线");
		else
			strcpy(sText,"悬垂挂线");
	}
	else if(GetPropID("hangingSide")==id)
	{
		if(iNo%10==0)
			strcpy(sText,"前侧");
		else
			strcpy(sText,"后侧");
	}
	else if(GetPropID("m_cFormula")==id)
	{
		if(wireInfo.m_cFormula==0)
			strcpy(sText,"悬链线");
		else
			strcpy(sText,"斜抛物线");
	}
	else if(GetPropID("voltage")==id)
		_snprintf(sText,99,"%d",wireInfo.m_nVoltGrade);
	else if(GetPropID("splitNumber")==id)
		_snprintf(sText,99,"%d",wireInfo.m_nDaoXianFengLieNum);
	else if(GetPropID("landHigh")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.land_high);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("attachmentSpec")==id)
		strcpy(sText,wireInfo.specAttachment);
	else if(GetPropID("attachmentWeight")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.m_fAttachmentWeight);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fAttachmentLength")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.m_fAttachmentLength);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fAttachmentDiameter")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.m_fAttachmentDiameter);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("anchorLineAngle")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.m_fAchorLineAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("tractionLineAngle")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.m_fTractionLineAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("wireType")==id)
		strcpy(sText,wireInfo.wire.specWireType);
	else if(GetPropID("wireSpec")==id)
		strcpy(sText,wireInfo.wire.specWire);
	else if(GetPropID("wireDiameter")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.wire.m_fDiameter);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("wireArea")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.wire.m_fArea);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("unitWeight")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.wire.m_fUnitWeight);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("forcePermit")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.wire.m_fForcePermit);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("saftyCoef")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.wire.m_fSafeCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("maxTension")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.wire.m_fMaxUseTension);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("elasticityCoef")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.wire.m_fElasticCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("expansionCoef")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.wire.m_fExpansionCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("horiSpace")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.xianlu.horizon_space);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("vertSpace")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.xianlu.vertical_space);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("minVertSpace")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.xianlu.min_vert_space);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("representSpace")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.xianlu.represent_space);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HtoL")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.xianlu.HtoL);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("allocHoriCoef")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.xianlu.horizon_space_allotcoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("allocVertCoef")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.xianlu.vertical_space_allotcoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("xianluAngle")==id)
	{
		_snprintf(sText,99,"%f",wireInfo.xianlu.xianlu_angle);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}

int CWireLoadModel::GetPropStr(long id,char *propStr)		//得到属性字符串
{
	char sText[100]="";
	if(GetPropID("wireInfo")==0)
		strcpy(sText,"wireInfo");
	if(propStr)
		strcpy(propStr,sText);
	return strlen(sText);
}
long CWireLoadModel::GetPropID(char* propStr)	//得到属性ID
{
	long id;
	if(CWireLoadModel::propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}
BOOL CWireLoadModel::IsPropEqual(OBJSET &selectObjs, long idProp)	//检测属性值是否相等
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSObject *pFirstObj = selectObjs.GetFirst();
	CWireLoadModel *pDbObject=NULL, *pFirstDbObject = (CWireLoadModel*)pFirstObj;
	pFirstDbObject->GetPropValueStr(idProp,valueStr);
	for(pDbObject=(CWireLoadModel*)selectObjs.GetNext();pDbObject;pDbObject=(CWireLoadModel*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pDbObject->GetPropValueStr(idProp,valueStr2);
		if(_stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}
int CWireLoadModel::GetPropStatus(long id)					//得到属性状态
{
	DWORD dwStatus;
	if(CWireLoadModel::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}
BOOL CWireLoadModel::SetPropStatus(long id,BOOL bHide)		//设置属性状态
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CWireLoadModel::propStatusHashtable.SetValueAt(id,dwStatus);
}
void CWireLoadModel::ResetWorkEnvLoadSet()
{
	int i=0;
	hashLoad.Empty();
	if(wireInfo.m_bNaiZhang)	//耐张
	{
		CWorkEnvLoad *pRawLoad=hashLoad.Add(WORKENVLOAD_WIND);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"大风");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_ICE);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"覆冰");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_LOWTEMP);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"低温");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_ICELESS);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"脱冰工况少冰");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_ICEMORE);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"脱冰工况多冰");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_DUANXIANRUNNING);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"事故正常相");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_DUANXIANBREAK);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"事故相");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPSUSPEND);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"已安装相");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPJINXIAN_OTHER_FINISH);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"安装正紧另档已挂");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPJINXIAN_OTHER_EMPTY);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"安装正紧另档未挂");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_ANCHORFINISHED);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"安装已锚相");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPANCHORING);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"安装正挂相");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_EARTHQUAKE);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"抗震验算");
	}
	else	//悬垂挂线
	{
		
		CWorkEnvLoad *pRawLoad=hashLoad.Add(WORKENVLOAD_WIND);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"大风");

		pRawLoad=hashLoad.Add(WORKENVLOAD_ICE);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"覆冰");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_ICEDIFF);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"不均匀覆冰");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_DUANXIANRUNNING);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"事故正常相");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_DUANXIANBREAK);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"事故相");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPSUSPEND);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"安装已安装相");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPSUSPENDING);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"安装正吊相");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_ANCHORFINISHED);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"安装已锚相");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPANCHORING);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"安装正挂相");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_EARTHQUAKE);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"抗震验算");
	}	
}

void CWireLoadModel::CopyWireLoadModel(CWireLoadModel *pWireLoadModel)
{
	if (pWireLoadModel == NULL)
		return;
	strcpy(name,pWireLoadModel->name);
	iNo=pWireLoadModel->iNo;
	wireInfo=pWireLoadModel->wireInfo;
	hashLoad.Empty();
	for (CWorkEnvLoad* pWorkEnvLoad = pWireLoadModel->hashLoad.GetFirst();pWorkEnvLoad;pWorkEnvLoad = pWireLoadModel->hashLoad.GetNext())
	{
		CWorkEnvLoad* pDestWorkEnvLoad = hashLoad.Add(pWireLoadModel->hashLoad.GetCursorKey());
		*pDestWorkEnvLoad = *pWorkEnvLoad;
	}
	//气象条件
	weather.m_fMaxIceThick=pWireLoadModel->weather.m_fMaxIceThick;
	weather.m_fMaxIceThickCheck=pWireLoadModel->weather.m_fMaxIceThickCheck;
	weather.m_fMaxIceWindV=pWireLoadModel->weather.m_fMaxIceWindV;
	weather.m_fMaxWindV=pWireLoadModel->weather.m_fMaxWindV;
	weather.m_fMaxWindVCheck=pWireLoadModel->weather.m_fMaxWindVCheck;
	weather.m_fSetupWindV=pWireLoadModel->weather.m_fSetupWindV;
	weather.m_nAverageT=pWireLoadModel->weather.m_nAverageT;
	weather.m_nHighestT=pWireLoadModel->weather.m_nHighestT;
	weather.m_nLowestT=pWireLoadModel->weather.m_nLowestT;
	weather.m_nMaxIceT=pWireLoadModel->weather.m_nMaxIceT;
	weather.m_nMaxWindT=pWireLoadModel->weather.m_nMaxWindT;
	weather.m_nSetupT=pWireLoadModel->weather.m_nSetupT;
}
void CWireLoadModel::FromBuffer(CBuffer &buffer,int nVersion/*=0*/,long doc_type/*=3*/)
{
	buffer.ReadString(name);
	buffer.ReadDouble(&wireInfo.land_high);
	buffer.ReadByte(&wireInfo.m_cXianType);
	buffer.ReadByte(&wireInfo.m_cFormula);	//新加
	buffer.ReadBooleanThin(&wireInfo.m_bNaiZhang);
	buffer.ReadInteger(&wireInfo.m_nVoltGrade);
	buffer.ReadByte(&wireInfo.m_nDaoXianFengLieNum);
	buffer.ReadString(wireInfo.specAttachment);			//金具及绝缘子等附属件明细
	buffer.ReadDouble(&wireInfo.m_fAttachmentWeight);			//金具及绝缘子等附属件重量(kg)
	buffer.ReadDouble(&wireInfo.m_fAttachmentDiameter);		//绝缘子等附属件宽度或外径(mm)
	buffer.ReadDouble(&wireInfo.m_fAttachmentLength);			//绝缘子等附属件长度(m)
	buffer.ReadDouble(&wireInfo.m_fAchorLineAngle);			//锚线对地夹角
	buffer.ReadDouble(&wireInfo.m_fTractionLineAngle);		//牵引线对地夹角
	//气象条件
	if(nVersion==0||(doc_type==3&&nVersion>=1070101)||(doc_type==4&&nVersion>=1000101))
	{
		buffer.ReadDouble(&weather.m_fMaxIceThick);		//最大覆冰厚度(mm)
		buffer.ReadDouble(&weather.m_fMaxIceThickCheck);//验算覆冰厚度(mm)
		buffer.ReadDouble(&weather.m_fMaxIceWindV);
		buffer.ReadDouble(&weather.m_fMaxWindV);		//大风速度(m/s)
		buffer.ReadDouble(&weather.m_fMaxWindVCheck);	//验算风速(m/s)
		buffer.ReadDouble(&weather.m_fSetupWindV);
		buffer.ReadInteger(&weather.m_nAverageT);
		buffer.ReadInteger(&weather.m_nHighestT);
		buffer.ReadInteger(&weather.m_nLowestT);
		buffer.ReadInteger(&weather.m_nMaxIceT);
		buffer.ReadInteger(&weather.m_nMaxWindT);
		buffer.ReadInteger(&weather.m_nSetupT);
	}
	//线路信息
	buffer.ReadDouble(&wireInfo.xianlu.horizon_space);		//水平档距
	buffer.ReadDouble(&wireInfo.xianlu.vertical_space);		//垂直档距
	buffer.ReadDouble(&wireInfo.xianlu.min_vert_space);		//最小垂直档距
	buffer.ReadDouble(&wireInfo.xianlu.horizon_space_allotcoef);	//水平档距分配系数
	buffer.ReadDouble(&wireInfo.xianlu.vertical_space_allotcoef);//垂直档距分配系数
	buffer.ReadDouble(&wireInfo.xianlu.represent_space);	//代表档距
	buffer.ReadDouble(&wireInfo.xianlu.HtoL);				//高差系数
	buffer.ReadDouble(&wireInfo.xianlu.xianlu_angle);		//单位度(°)
	//导地线信息
	buffer.ReadDouble(&wireInfo.wire.m_fDiameter);		// 电线外径,mm
	buffer.ReadDouble(&wireInfo.wire.m_fArea);			// 电线计算截面积,mm2
	buffer.ReadDouble(&wireInfo.wire.m_fUnitWeight);	// 电线单位长重量,kg/km
	buffer.ReadDouble(&wireInfo.wire.m_fForcePermit);	// 电线额定抗拉力,N
	buffer.ReadDouble(&wireInfo.wire.m_fSafeCoef);		// 电线安全系数
	buffer.ReadDouble(&wireInfo.wire.m_fMaxUseTension);// 电线最大使用张力,N
	buffer.ReadDouble(&wireInfo.wire.m_fElasticCoef);	// 电线弹性系数,MPa
	buffer.ReadDouble(&wireInfo.wire.m_fExpansionCoef);// 电线温度线膨胀系数,1/1/°C
	buffer.ReadString(wireInfo.wire.specWireType);	// 电线的类型代码
	buffer.ReadString(wireInfo.wire.specWire);		// 电线的规格型号描述
	//计算结果信息
	if(nVersion==0||(doc_type==3&&nVersion>=1070005)||(doc_type==4&&nVersion>=1000016))
	{
		long n2;
		buffer.ReadInteger(&n2);
		for(int j=0;j<n2;j++)
		{
			long serial=0;
			buffer.ReadInteger(&serial);
			CWorkEnvLoad *pLoad=hashLoad.Add(serial);
			buffer.ReadString(pLoad->name);					//对应工况名称
			buffer.ReadDouble(&pLoad->m_fLoadX);				//电气提供横向风力荷载总和(N)
			buffer.ReadDouble(&pLoad->m_fLoadY);				//电气提供纵向荷载总和(N)
			buffer.ReadDouble(&pLoad->m_fLoadZ);				//电气提供垂直荷载总和(N)
			buffer.ReadDouble(&pLoad->m_fMinVertSpaceLoadX);	//电气提供横向荷载(最小垂直档距)总和(N)
			buffer.ReadDouble(&pLoad->m_fMinVertSpaceLoadY);	//电气提供纵向荷载(最小垂直档距)总和(N)
			buffer.ReadDouble(&pLoad->m_fMinVertSpaceLoadZ);	//电气提供垂直荷载(最小垂直档距)总和(N)
			buffer.ReadDouble(&pLoad->m_fWireLoadX);			//电线的风荷载(N)
			buffer.ReadDouble(&pLoad->m_fWireLoadZ);			//电线的垂直荷载(N)
			buffer.ReadDouble(&pLoad->m_fMinVertSpaceWireLoadZ);//最小垂直档距时电线的垂直荷载(N)
			buffer.ReadDouble(&pLoad->m_fAttachmentLoadX);		//附属件风荷载(含覆冰影响)(N)
			buffer.ReadDouble(&pLoad->m_fAttachmentLoadZ);		//附属件垂直荷载(含覆冰影响)(N)
			buffer.ReadDouble(&pLoad->m_fSetupGa);				//安装时所需的附加垂直荷载(N)
			if(nVersion==0||(doc_type==3&&nVersion>=1070006)||(doc_type==4&&nVersion>=1000017))
				buffer.ReadDouble(&pLoad->huchui);					//弧垂
		}
	}
}
void CWireLoadModel::ToBuffer(CBuffer &buffer)
{
	buffer.WriteString(name);
	buffer.WriteDouble(wireInfo.land_high);
	buffer.WriteByte(wireInfo.m_cXianType);
	buffer.WriteByte(wireInfo.m_cFormula);	//新加
	buffer.WriteBooleanThin(wireInfo.m_bNaiZhang);
	buffer.WriteInteger(wireInfo.m_nVoltGrade);
	buffer.WriteByte(wireInfo.m_nDaoXianFengLieNum);
	buffer.WriteString(wireInfo.specAttachment);			//金具及绝缘子等附属件明细
	buffer.WriteDouble(wireInfo.m_fAttachmentWeight);			//金具及绝缘子等附属件重量(kg)
	buffer.WriteDouble(wireInfo.m_fAttachmentDiameter);		//绝缘子等附属件宽度或外径(mm)
	buffer.WriteDouble(wireInfo.m_fAttachmentLength);			//绝缘子等附属件长度(m)
	buffer.WriteDouble(wireInfo.m_fAchorLineAngle);			//锚线对地夹角
	buffer.WriteDouble(wireInfo.m_fTractionLineAngle);		//牵引线对地夹角
	//气象条件
	buffer.WriteDouble(weather.m_fMaxIceThick);		//最大覆冰厚度(mm)
	buffer.WriteDouble(weather.m_fMaxIceThickCheck);//验算覆冰厚度(mm)
	buffer.WriteDouble(weather.m_fMaxIceWindV);
	buffer.WriteDouble(weather.m_fMaxWindV);		//大风速度(m/s)
	buffer.WriteDouble(weather.m_fMaxWindVCheck);	//验算风速(m/s)
	buffer.WriteDouble(weather.m_fSetupWindV);
	buffer.WriteInteger(weather.m_nAverageT);
	buffer.WriteInteger(weather.m_nHighestT);
	buffer.WriteInteger(weather.m_nLowestT);
	buffer.WriteInteger(weather.m_nMaxIceT);
	buffer.WriteInteger(weather.m_nMaxWindT);
	buffer.WriteInteger(weather.m_nSetupT);
	//线路信息
	buffer.WriteDouble(wireInfo.xianlu.horizon_space);	//水平档距
	buffer.WriteDouble(wireInfo.xianlu.vertical_space);	//垂直档距
	buffer.WriteDouble(wireInfo.xianlu.min_vert_space);	//最小垂直档距
	buffer.WriteDouble(wireInfo.xianlu.horizon_space_allotcoef);	//水平档距分配系数
	buffer.WriteDouble(wireInfo.xianlu.vertical_space_allotcoef);//垂直档距分配系数
	buffer.WriteDouble(wireInfo.xianlu.represent_space);	//代表档距
	buffer.WriteDouble(wireInfo.xianlu.HtoL);			//高差系数
	buffer.WriteDouble(wireInfo.xianlu.xianlu_angle);	//单位度(°)
	//导地线信息
	buffer.WriteDouble(wireInfo.wire.m_fDiameter);		// 电线外径,mm
	buffer.WriteDouble(wireInfo.wire.m_fArea);			// 电线计算截面积,mm2
	buffer.WriteDouble(wireInfo.wire.m_fUnitWeight);	// 电线单位长重量,kg/km
	buffer.WriteDouble(wireInfo.wire.m_fForcePermit);	// 电线额定抗拉力,N
	buffer.WriteDouble(wireInfo.wire.m_fSafeCoef);		// 电线安全系数
	buffer.WriteDouble(wireInfo.wire.m_fMaxUseTension);// 电线最大使用张力,N
	buffer.WriteDouble(wireInfo.wire.m_fElasticCoef);	// 电线弹性系数,MPa
	buffer.WriteDouble(wireInfo.wire.m_fExpansionCoef);// 电线温度线膨胀系数,1/1/°C
	buffer.WriteString(wireInfo.wire.specWireType);	// 电线的类型代码
	buffer.WriteString(wireInfo.wire.specWire);		// 电线的规格型号描述
	//计算结果信息
	buffer.WriteInteger(hashLoad.GetNodeNum());
	for(CWorkEnvLoad *pLoad=hashLoad.GetFirst();pLoad;pLoad=hashLoad.GetNext())
	{
		buffer.WriteInteger(hashLoad.GetCursorKey());
		buffer.WriteString(pLoad->name);					//对应工况名称
		buffer.WriteDouble(pLoad->m_fLoadX);				//电气提供横向风力荷载总和(N)
		buffer.WriteDouble(pLoad->m_fLoadY);				//电气提供纵向荷载总和(N)
		buffer.WriteDouble(pLoad->m_fLoadZ);				//电气提供垂直荷载总和(N)
		buffer.WriteDouble(pLoad->m_fMinVertSpaceLoadX);	//电气提供横向荷载(最小垂直档距)总和(N)
		buffer.WriteDouble(pLoad->m_fMinVertSpaceLoadY);	//电气提供纵向荷载(最小垂直档距)总和(N)
		buffer.WriteDouble(pLoad->m_fMinVertSpaceLoadZ);	//电气提供垂直荷载(最小垂直档距)总和(N)
		buffer.WriteDouble(pLoad->m_fWireLoadX);			//电线的风荷载(N)
		buffer.WriteDouble(pLoad->m_fWireLoadZ);			//电线的垂直荷载(N)
		buffer.WriteDouble(pLoad->m_fMinVertSpaceWireLoadZ);//最小垂直档距时电线的垂直荷载(N)
		buffer.WriteDouble(pLoad->m_fAttachmentLoadX);		//附属件风荷载(含覆冰影响)(N)
		buffer.WriteDouble(pLoad->m_fAttachmentLoadZ);		//附属件垂直荷载(含覆冰影响)(N)
		buffer.WriteDouble(pLoad->m_fSetupGa);				//安装时所需的附加垂直荷载(N)
		buffer.WriteDouble(pLoad->huchui);					//弧垂
	}
}
void CWireLoadModel::ToXmlFile(FILE *fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<线条荷载计算模型信息 name=\"%s\">\n",name);
	fprintf(fp,"<基本信息>\n");
	sprintf(sText,"%f",wireInfo.land_high);
	SimplifiedNumString(sText);
	fprintf(fp,"<地势高度 land_high=\"%s\"/>\n",sText);
	fprintf(fp,"<线型 XianType=\"%c\"/>\n",wireInfo.m_cXianType);
	fprintf(fp,"<弧垂计算方式 Formula=\"%c\"/>\n",wireInfo.m_cFormula);
	if(wireInfo.m_bNaiZhang)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<耐张 bNaiZhang=\"%s\"/>\n",sText);
	fprintf(fp,"<电压等级 VoltGrade=\"%d\"/>\n",wireInfo.m_nVoltGrade);
	fprintf(fp,"<导线分裂数 FengLieNum=\"%d\"/>\n",wireInfo.m_nDaoXianFengLieNum);
	fprintf(fp,"<附属件明细 specAttachment=\"%s\"/>\n",wireInfo.specAttachment);
	sprintf(sText,"%f",wireInfo.m_fAttachmentWeight);
	SimplifiedNumString(sText);
	fprintf(fp,"<附属件重量 AttachmentWeight=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.m_fAttachmentDiameter);
	SimplifiedNumString(sText);
	fprintf(fp,"<附属件外径 AttachmentDiameter=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.m_fAttachmentLength);
	SimplifiedNumString(sText);
	fprintf(fp,"<附属件长度 AttachmentLength=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.m_fAchorLineAngle);
	SimplifiedNumString(sText);
	fprintf(fp,"<锚线对地夹角 AttachLineAngle=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.m_fTractionLineAngle);
	SimplifiedNumString(sText);
	fprintf(fp,"<牵引线对地夹角 TractionLineAngle=\"%s\"/>\n",sText);
	fprintf(fp,"</基本信息>\n");
	fprintf(fp,"<气象条件>\n");
	sprintf(sText,"%f",weather.m_fMaxIceThick);
	SimplifiedNumString(sText);
	fprintf(fp,"<最大覆冰厚度 MaxIceThick=\"%s\"/>\n",sText);
	sprintf(sText,"%f",weather.m_fMaxIceThickCheck);
	SimplifiedNumString(sText);
	fprintf(fp,"<验算覆冰厚度 MaxIceThickCheck=\"%s\"/>\n",sText);
	sprintf(sText,"%f",weather.m_fMaxIceWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<覆冰时风速 MaxIceWindV=\"%s\"/>\n",sText);
	sprintf(sText,"%f",weather.m_fMaxWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<最大风速 MaxWindV=\"%s\"/>\n",sText);
	sprintf(sText,"%f",weather.m_fMaxWindVCheck);
	SimplifiedNumString(sText);
	fprintf(fp,"<验算风速 MaxWindVCheck=\"%s\"/>\n",sText);
	sprintf(sText,"%f",weather.m_fSetupWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<安装时风速 SetupWindV=\"%s\"/>\n",sText);
	fprintf(fp,"<平均气温 AverageT=\"%d\"/>\n",weather.m_nAverageT);
	fprintf(fp,"<最高气温 HighestT=\"%d\"/>\n",weather.m_nHighestT);
	fprintf(fp,"<最低气温 LowestT=\"%d\"/>\n",weather.m_nLowestT);
	fprintf(fp,"<覆冰温度 MaxIceT=\"%d\"/>\n",weather.m_nMaxIceT);
	fprintf(fp,"<大风时温度 MaxWindT=\"%d\"/>\n",weather.m_nMaxWindT);
	fprintf(fp,"<安装时温度 SetupT=\"%d\"/>\n",weather.m_nSetupT);
	fprintf(fp,"</气象条件>\n");
	fprintf(fp,"<线路信息>\n");
	sprintf(sText,"%f",wireInfo.xianlu.horizon_space);
	SimplifiedNumString(sText);
	fprintf(fp,"<水平档距 horizon_space=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.vertical_space);
	SimplifiedNumString(sText);
	fprintf(fp,"<垂直档距 vertical_space=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.min_vert_space);
	SimplifiedNumString(sText);
	fprintf(fp,"<最小垂直档距 min_vert_space=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.horizon_space_allotcoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<水平档距分配系数 horizon_space_allotcoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.vertical_space_allotcoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<垂直档距分配系数 vertical_space_allotcoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.represent_space);
	SimplifiedNumString(sText);
	fprintf(fp,"<代表档距 represent_space=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.HtoL);
	SimplifiedNumString(sText);
	fprintf(fp,"<高差系数 HtoL=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.xianlu_angle);
	SimplifiedNumString(sText);
	fprintf(fp,"<线夹角 xianlu_angle=\"%s\"/>\n",sText);
	fprintf(fp,"</线路信息>\n");
	fprintf(fp,"<导地线信息>\n");
	sprintf(sText,"%f",wireInfo.wire.m_fDiameter);
	SimplifiedNumString(sText);
	fprintf(fp,"<电线外径 Dimater=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fArea);
	SimplifiedNumString(sText);
	fprintf(fp,"<电线计算截面积 Area=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fUnitWeight);
	SimplifiedNumString(sText);
	fprintf(fp,"<电线单位长重量 UnitWeight=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fForcePermit);
	SimplifiedNumString(sText);
	fprintf(fp,"<电线额定抗拉力 ForcePermit=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fSafeCoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<电线安全系数 SafeCoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fMaxUseTension);
	SimplifiedNumString(sText);
	fprintf(fp,"<电线最大使用张力 MaxUseTension=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fElasticCoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<电线弹性系数 ElasticCoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fExpansionCoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<电线温度膨胀系数 ExpansionCoef=\"%s\"/>\n",sText);
	fprintf(fp,"<电线类型 specWireType=\"%s\"/>\n",wireInfo.wire.specWireType);
	fprintf(fp,"<电线规格型号 specWire=\"%s\"/>\n",wireInfo.wire.specWire);
	fprintf(fp,"</导地线信息>\n");
	fprintf(fp,"<计算结果信息 LoadNum=\"%d\">\n",hashLoad.GetNodeNum());
	for(CWorkEnvLoad *pLoad=hashLoad.GetFirst();pLoad;pLoad=hashLoad.GetNext())
	{
		fprintf(fp,"<工况下的计算结果 key=\"%d\" LoadName=\"%s\">\n",hashLoad.GetCursorKey(),pLoad->name);
		sprintf(sText,"%f",pLoad->m_fLoadX);
		SimplifiedNumString(sText);
		fprintf(fp,"<横向风力荷载总和 LoadX=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fLoadY);
		SimplifiedNumString(sText);
		fprintf(fp,"<纵向风力荷载总和 LoadY=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fLoadZ);
		SimplifiedNumString(sText);
		fprintf(fp,"<垂直荷载总和 LoadZ=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fMinVertSpaceLoadX);
		SimplifiedNumString(sText);
		fprintf(fp,"<最小垂直档距横向荷载总和 MinVertSpaceLoadX=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fMinVertSpaceLoadY);
		SimplifiedNumString(sText);
		fprintf(fp,"<最小垂直档距纵向荷载总和 MinVertSpaceLoadY=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fMinVertSpaceLoadZ);
		SimplifiedNumString(sText);
		fprintf(fp,"<最小垂直档距垂直荷载总和 MinVertSpaceLoadZ=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fWireLoadX);
		SimplifiedNumString(sText);
		fprintf(fp,"<电线的风荷载 WireLoadX=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fWireLoadZ);
		SimplifiedNumString(sText);
		fprintf(fp,"<电线的垂直荷载 WireLoadZ=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fMinVertSpaceWireLoadZ);
		SimplifiedNumString(sText);
		fprintf(fp,"<最小垂直档距时电线的垂直荷载 MinVertSpaceWireLoadZ=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fAttachmentLoadX);
		SimplifiedNumString(sText);
		fprintf(fp,"<附属件风荷载 AttachmentLoadX=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fAttachmentLoadZ);
		SimplifiedNumString(sText);
		fprintf(fp,"<附属件垂直荷载 AttachmentLoadZ=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fSetupGa);
		SimplifiedNumString(sText);
		fprintf(fp,"<安装时所需的附加垂直荷载 SetupGa=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->huchui);
		SimplifiedNumString(sText);
		fprintf(fp,"<弧垂 huchui=\"%s\"/>\n",sText);
		fprintf(fp,"</工况下的计算结果>\n");
	}
	fprintf(fp,"</计算结果信息>\n");
	fprintf(fp,"</线条荷载计算模型信息>\n");
}
static double FindIceB1(double ice_thick)
{
	if(ice_thick<5)
		return 1.0;
	else if(ice_thick<10)
		return 1.1+(ice_thick-5)*0.02;
	else if(ice_thick<15)
		return 1.2+(ice_thick-10)*0.02;
	else if(ice_thick<20)
		return 1.3+(ice_thick-15)*0.04;
	else if(ice_thick<30)
		return 1.5+(ice_thick-20)*0.05;
	else
		return 2.0;
}
static double FindAlfa(double windV)
{
	if(windV<20)
		return 1.0;
	else if(windV<27)
		return 0.85;
	else if(windV<31.5)
		return 0.75;
	else
		return 0.7;
}
static double FindBetaC(double windV,long voltage)
{
	if(voltage<500)
		return 1.0;
	else if(windV<20)
		return 1.0;
	else if(windV<27)
		return 1.1;
	else if(windV<31.5)
		return 1.2;
	else
		return 1.3;
}
	//判断导地线的控制气象条件并初始化电线状态方程
void CWireLoadModel::InitDianXianEquation(WEATHER &weather,
										  double gama_g,double gama_ice_w,double gama_max_w,double gama_ice)
{
#if defined(__LDS_)||defined(__TSA_)
	long initT;
	double init_length,length;
	double init_sigema,init_gama,gama;

	//1、判断控制工况
	//覆冰工况
	init_sigema=1e6*wireInfo.wire.m_fMaxUseTension/wireInfo.wire.m_fArea;
	init_gama=sqrt(pow(gama_g+gama_ice,2)+gama_ice_w*gama_ice_w);
	initT=weather.m_nMaxIceT;
	m_pResolver=CXuanLianResolver::CreateResolver();
	m_pResolver->InitXuanLian(1e6*wireInfo.wire.m_fElasticCoef,
		wireInfo.wire.m_fExpansionCoef,init_sigema,init_gama,
		weather.m_nMaxIceT,wireInfo.xianlu.represent_space,
		wireInfo.xianlu.HtoL,wireInfo.m_cFormula);
	init_length=m_pResolver->XuanLianLength();
	//低温工况
	m_pResolver->InitXuanLian(1e6*wireInfo.wire.m_fElasticCoef,
		wireInfo.wire.m_fExpansionCoef,init_sigema,gama_g,
		weather.m_nLowestT,wireInfo.xianlu.represent_space,
		wireInfo.xianlu.HtoL,wireInfo.m_cFormula);
	length=m_pResolver->XuanLianLength();
	if(length>init_length)
	{
		init_gama=gama_g;
		initT=weather.m_nLowestT;
		init_length=length;
	}
	//计算大风无冰比载
	gama=sqrt(gama_g*gama_g+gama_max_w*gama_max_w);
	m_pResolver->InitXuanLian(1e6*wireInfo.wire.m_fElasticCoef,
		wireInfo.wire.m_fExpansionCoef,init_sigema,gama,
		weather.m_nMaxWindT,wireInfo.xianlu.represent_space,
		wireInfo.xianlu.HtoL,wireInfo.m_cFormula);
	length=m_pResolver->XuanLianLength();
	if(length>init_length)
	{
		init_gama=gama;
		initT=weather.m_nMaxWindT;
		init_length=length;
	}
	//2、初始化控制气象条件
	m_pResolver->InitXuanLian(1e6*wireInfo.wire.m_fElasticCoef,
		wireInfo.wire.m_fExpansionCoef,init_sigema,init_gama,
		initT,wireInfo.xianlu.represent_space,
		wireInfo.xianlu.HtoL,wireInfo.m_cFormula);
#endif
}
void CWireLoadModel::CalWorkEnvLoad(char cLandscape,char cLineRoadType)
{
#if defined(__LDS_)||defined(__TSA_)
	CWorkEnvLoad *pCommonLoad;
	double anchor_angle=wireInfo.m_fAchorLineAngle*RADTODEG_COEF;
	double tracktion_angle=wireInfo.m_fTractionLineAngle*RADTODEG_COEF;
	if(hashLoad.GetNodeNum()==0)
		return;
	//清空原有数据
	for(pCommonLoad=hashLoad.GetFirst();pCommonLoad;pCommonLoad=hashLoad.GetNext())
	{
		pCommonLoad->m_fLoadX=0;				//电气提供横向风力荷载总和(N)
		pCommonLoad->m_fLoadY=0;				//电气提供纵向荷载总和(N)
		pCommonLoad->m_fLoadZ=0;				//电气提供垂直荷载总和(N)
		pCommonLoad->m_fMinVertSpaceLoadX=0;	//电气提供横向荷载(最小垂直档距)总和(N)
		pCommonLoad->m_fMinVertSpaceLoadY=0;	//电气提供纵向荷载(最小垂直档距)总和(N)
		pCommonLoad->m_fMinVertSpaceLoadZ=0;	//电气提供垂直荷载(最小垂直档距)总和(N)
		pCommonLoad->m_fWireLoadX=0;			//电线的风荷载(N)
		pCommonLoad->m_fWireLoadZ=0;			//电线的垂直荷载(N)
		pCommonLoad->m_fMinVertSpaceWireLoadZ=0;//最小垂直档距时电线的垂直荷载(N)
		pCommonLoad->m_fAttachmentLoadX=0;		//附属件风荷载(含覆冰影响)(N)
		pCommonLoad->m_fAttachmentLoadZ=0;		//附属件垂直荷载(含覆冰影响)(N)
		pCommonLoad->m_fSetupGa=0;				//安装时所需的附加垂直荷载(N)
	}
	//T0临时拉线平衡导地线的水平张力值(N)
	double T0,W0,alfa,muSC,betaC,iceB;
	double gama,gama_w;
	double gama_g,gama_ice_w,gama_max_w,gama_ice;//典型工况比载
	double xian_dia=wireInfo.wire.m_fDiameter;	//mm
	double xian_area=wireInfo.wire.m_fArea;		//mm2
	double vert_coef=wireInfo.xianlu.vertical_space_allotcoef;
	double hori_coef=wireInfo.xianlu.horizon_space_allotcoef;
	double max_tension=wireInfo.wire.m_fMaxUseTension;	//N
	double muZ=Find_MuZ(wireInfo.land_high,cLandscape);	//风压高度变化系数，一般应挂线点减绝缘子串再减2/3弧垂
	//计算自重比载
	gama_g=9800*wireInfo.wire.m_fUnitWeight/xian_area;
	//计算覆冰比载
	gama_ice=8820*Pi*weather.m_fMaxIceThick*(weather.m_fMaxIceThick+xian_dia)/xian_area;
	//计算覆冰风比载
	if(wireInfo.wire.m_fDiameter<17||weather.m_fMaxIceThick>0)
		muSC=1.2;
	else
		muSC=1.1;
	alfa=FindAlfa(weather.m_fMaxIceWindV);
	betaC=FindBetaC(weather.m_fMaxIceWindV,wireInfo.m_nVoltGrade);
	iceB=FindIceB1(weather.m_fMaxIceThick);
	W0=weather.m_fMaxIceWindV*weather.m_fMaxIceWindV/1600;
	gama_ice_w=1e6*alfa*W0*muZ*betaC*muSC*iceB*(xian_dia+weather.m_fMaxIceThick*2)/xian_area;
	//大风工况风比载
	if(wireInfo.wire.m_fDiameter<17)
		muSC=1.2;
	else
		muSC=1.1;
	alfa=FindAlfa(weather.m_fMaxWindV);
	betaC=FindBetaC(weather.m_fMaxWindV,wireInfo.m_nVoltGrade);
	W0=weather.m_fMaxWindV*weather.m_fMaxWindV/1600;
	gama_max_w=1e6*alfa*W0*muZ*betaC*muSC*xian_dia/xian_area;

	if(wireInfo.m_bNaiZhang)	//耐张
	{
		if(wireInfo.xianlu.represent_space<=0)	//悬垂直线塔未提供代表档距
		{
			AfxMessageBox("代表档距不能为0！");
			return;
		}
		InitDianXianEquation(weather,gama_g,gama_ice_w,gama_max_w,gama_ice);
		//1 低温工况
		CWorkEnvLoad *pLowestTempLoad=hashLoad.GetValue(WORKENVLOAD_LOWTEMP);
		if(pLowestTempLoad==NULL)
		{
			AfxMessageBox("缺少耐张塔的电气工况，无法计算电气工况荷载！");
			return;
		}
		//TODO: 会出现m_pResolver==NULL的异常情况 wjh-2012.12.23
		m_pResolver->XuanLianByCursorPara(gama_g,weather.m_nLowestT);
		pLowestTempLoad->huchui=m_pResolver->XuanLianPosO();
			//垂直荷载
		pLowestTempLoad->m_fWireLoadZ=wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.vertical_space*vert_coef*wireInfo.wire.m_fUnitWeight*0.0098;
		pLowestTempLoad->m_fAttachmentLoadZ=wireInfo.m_fAttachmentWeight*9.8;
		pLowestTempLoad->m_fMinVertSpaceWireLoadZ=wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.min_vert_space*vert_coef*wireInfo.wire.m_fUnitWeight*0.0098;
			//纵向荷载
		pLowestTempLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
			//横向荷载
		pLowestTempLoad->m_fLoadX=0;
		//2 90度大风荷载
		CWorkEnvLoad *pWind90Load=hashLoad.GetValue(WORKENVLOAD_WIND);
		gama=sqrt(gama_max_w*gama_max_w+gama_g*gama_g);
		m_pResolver->XuanLianByCursorPara(gama,weather.m_nMaxWindT);
		pWind90Load->huchui=m_pResolver->XuanLianPosO();
			//垂直荷载
		pWind90Load->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ;
		pWind90Load->m_fAttachmentLoadZ=pLowestTempLoad->m_fAttachmentLoadZ;
			//纵向荷载
		pWind90Load->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
			//横向荷载
		pWind90Load->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_max_w*xian_area*1e-6;
		pWind90Load->m_fAttachmentLoadX=wireInfo.m_fAttachmentLength*wireInfo.m_fAttachmentDiameter*muZ*weather.m_fMaxWindV*weather.m_fMaxWindV/1600000;
		//3 覆冰工况荷载
		CWorkEnvLoad *pIceLoad=hashLoad.GetValue(WORKENVLOAD_ICE);
		gama=sqrt(gama_ice_w*gama_ice_w+pow(gama_g+gama_ice,2));
		m_pResolver->XuanLianByCursorPara(gama,weather.m_nMaxIceT);
		pIceLoad->huchui=m_pResolver->XuanLianPosO();
			//垂直荷载
		pIceLoad->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ+
			wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.vertical_space*vert_coef*gama_ice*xian_area*1e-6;
		pIceLoad->m_fAttachmentLoadZ=pLowestTempLoad->m_fAttachmentLoadZ+
			8820*Pi*weather.m_fMaxIceThick*(weather.m_fMaxIceThick+wireInfo.m_fAttachmentDiameter)*wireInfo.m_fAttachmentLength*1e-9;
			//纵向荷载
		pIceLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
			//横向荷载
		pIceLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_ice_w*xian_area*1e-6;
		pIceLoad->m_fAttachmentLoadX=weather.m_fMaxIceWindV*weather.m_fMaxIceWindV*muZ*
			(wireInfo.m_fAttachmentDiameter+weather.m_fMaxIceThick*2)*wireInfo.m_fAttachmentLength/1600000;
		//4 脱冰工况满冰(-5°C,10m/s)
		CWorkEnvLoad *pMoreIceLoad=hashLoad.GetValue(WORKENVLOAD_ICEMORE);
		alfa=FindAlfa(10);
		betaC=FindBetaC(10,wireInfo.m_nVoltGrade);
		iceB=FindIceB1(weather.m_fMaxIceThick);
		W0=0.0625;//10*10/1600
		gama_w=1e6*alfa*W0*muZ*betaC*muSC*iceB*(xian_dia+weather.m_fMaxIceThick*2)/xian_area;
		gama=sqrt(gama_w*gama_w+pow(gama_g+gama_ice,2));
		m_pResolver->XuanLianByCursorPara(gama,-5);
			//垂直荷载
		pMoreIceLoad->m_fWireLoadZ=pIceLoad->m_fWireLoadZ;
		pMoreIceLoad->m_fAttachmentLoadZ=pIceLoad->m_fAttachmentLoadZ;
			//纵向荷载
		pMoreIceLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
			//横向荷载
		pMoreIceLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_w*xian_area*1e-6;
		pMoreIceLoad->m_fAttachmentLoadX=W0*muZ*
			(wireInfo.m_fAttachmentDiameter+weather.m_fMaxIceThick*2)*wireInfo.m_fAttachmentLength/1e6;
		//5 脱冰工况少冰(-5°C,10m/s)
		CWorkEnvLoad *pLessIceLoad=hashLoad.GetValue(WORKENVLOAD_ICELESS);
		if(xian_dia<17||weather.m_fMaxIceThick>0)
			muSC=1.2;
		else
			muSC=1.1;
		alfa=FindAlfa(10);
		betaC=FindBetaC(10,wireInfo.m_nVoltGrade);
		iceB=FindIceB1(weather.m_fMaxIceThick*0.75);
		//W0=0.0625;//10*10/1600
		gama_w=1e6*alfa*W0*muZ*betaC*muSC*iceB*(xian_dia+weather.m_fMaxIceThick*1.5)/xian_area;
		gama=sqrt(gama_w*gama_w+pow(gama_g+gama_ice,2));
			//垂直荷载
		pLessIceLoad->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ+
			wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.vertical_space*vert_coef*0.75*gama_ice*xian_area*1e-6;
		pLessIceLoad->m_fAttachmentLoadZ=pMoreIceLoad->m_fAttachmentLoadZ;
			//纵向荷载
		if(wireInfo.m_cXianType=='C')	//导线
			pLessIceLoad->m_fLoadY=pMoreIceLoad->m_fLoadY-wireInfo.wire.m_fMaxUseTension*0.3;
		else //if(wireInfo.m_cXianType=='E')	//地线
			pLessIceLoad->m_fLoadY=pMoreIceLoad->m_fLoadY-wireInfo.wire.m_fMaxUseTension*0.4;
			//横向荷载
		pLessIceLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_w*xian_area*1e-6;
		pLessIceLoad->m_fAttachmentLoadX=pMoreIceLoad->m_fAttachmentLoadX;
		//6 断线工况正常相荷载(有冰、无风、-5°C)
		CWorkEnvLoad *pDuanXianLoad=hashLoad.GetValue(WORKENVLOAD_DUANXIANRUNNING);
			//垂直荷载
		pDuanXianLoad->m_fWireLoadZ=pIceLoad->m_fWireLoadZ;
		pDuanXianLoad->m_fAttachmentLoadZ=pIceLoad->m_fAttachmentLoadZ;
			//纵向荷载
		if(wireInfo.m_cXianType=='C')
		{	//导线
			if(wireInfo.m_nDaoXianFengLieNum<=1)
				pDuanXianLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*max_tension;
			else
				pDuanXianLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*max_tension*0.7;
		}
		else//地线
			pDuanXianLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*max_tension;
			//横向荷载
		pDuanXianLoad->m_fLoadX=0;
		//7 断线工况事故相荷载
		pCommonLoad=hashLoad.GetValue(WORKENVLOAD_DUANXIANBREAK);
		//垂直荷载(垂直荷载按5h长拖地线计算)
		double alloc_coef=min(wireInfo.land_high*5/(wireInfo.xianlu.vertical_space*vert_coef),1);
		pCommonLoad->m_fWireLoadZ=pIceLoad->m_fWireLoadZ*alloc_coef;
			//纵向荷载
		pCommonLoad->m_fLoadY=0;
			//横向荷载
		pIceLoad->m_fLoadX=0;
		//8 安装已挂相
		CWorkEnvLoad *pSetupGuaXianLoad=hashLoad.GetValue(WORKENVLOAD_SETUPSUSPEND);
		if(xian_dia<17)
			muSC=1.2;
		else
			muSC=1.1;
		alfa=FindAlfa(weather.m_fSetupWindV);
		betaC=FindBetaC(weather.m_fSetupWindV,wireInfo.m_nVoltGrade);
		W0=weather.m_fSetupWindV*weather.m_fSetupWindV/1600;
		gama_w=1e6*alfa*W0*muZ*betaC*muSC*xian_dia/xian_area;
		gama=sqrt(gama_w*gama_w+gama_g*gama_g);
		m_pResolver->XuanLianByCursorPara(gama,weather.m_nSetupT);
		pSetupGuaXianLoad->huchui=m_pResolver->XuanLianPosO();
		//垂直荷载
		pSetupGuaXianLoad->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ;
			//纵向荷载
		pSetupGuaXianLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
			//横向荷载
		pSetupGuaXianLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_w*xian_area*1e-6;
		pSetupGuaXianLoad->m_fAttachmentLoadX=W0*muZ*wireInfo.m_fAttachmentDiameter*wireInfo.m_fAttachmentLength/1e6;
		//9 安装正紧另档已挂
		CWorkEnvLoad* pJinXianLoad=hashLoad.GetValue(WORKENVLOAD_SETUPJINXIAN_OTHER_FINISH);
		pJinXianLoad->huchui=pSetupGuaXianLoad->huchui;
			//垂直荷载
		pJinXianLoad->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ+1.1*pSetupGuaXianLoad->m_fLoadY*sin(tracktion_angle);
		pJinXianLoad->m_fAttachmentLoadZ=0;
		if(wireInfo.m_cXianType=='E')
		{
			if(wireInfo.m_nVoltGrade<=110)
				pJinXianLoad->m_fSetupGa=1500;
			else //if(wireInfo.m_nVoltGrade<=330)
				pJinXianLoad->m_fSetupGa=2000;
		}
		else //if
		{
			if(wireInfo.m_nVoltGrade<=110)
				pJinXianLoad->m_fSetupGa=2000;
			else if(wireInfo.m_nVoltGrade<=330)
				pJinXianLoad->m_fSetupGa=4500;
			else
				pJinXianLoad->m_fSetupGa=6000;
		}
			//纵向荷载
		pJinXianLoad->m_fLoadY=1.1*pSetupGuaXianLoad->m_fLoadY*(1-cos(tracktion_angle));
		if(wireInfo.m_cXianType=='E')
			pJinXianLoad->m_fLoadY*=1.07625;//1.05*1.025;	过牵引系数乘施工误差系数
		else
			pJinXianLoad->m_fLoadY*=1.101875;//1.075*1.025;
			//横向荷载
		pJinXianLoad->m_fWireLoadX=pSetupGuaXianLoad->m_fWireLoadX;
		//10安装正紧另档未挂（当前档有临时拉线）
		pCommonLoad=hashLoad.GetValue(WORKENVLOAD_SETUPJINXIAN_OTHER_EMPTY);
		pCommonLoad->huchui=pSetupGuaXianLoad->huchui;
		T0=pJinXianLoad->m_fLoadY;	//临时拉线的作用是使横担回归中央原位
			//垂直荷载
		pCommonLoad->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ+T0*tan(anchor_angle)+1.1*pSetupGuaXianLoad->m_fLoadY*sin(tracktion_angle);
		pCommonLoad->m_fSetupGa=pJinXianLoad->m_fSetupGa;
			//纵向荷载
		pCommonLoad->m_fLoadY=0;
			//横向荷载
		pCommonLoad->m_fWireLoadX=pSetupGuaXianLoad->m_fWireLoadX;
		//11 已锚相
		pCommonLoad=hashLoad.GetValue(WORKENVLOAD_ANCHORFINISHED);
		pCommonLoad->huchui=pSetupGuaXianLoad->huchui;
		if(wireInfo.m_cXianType=='E')
			T0=5000;
		else
		{
			if(wireInfo.m_nVoltGrade>=500&&wireInfo.m_nDaoXianFengLieNum==4)
				T0=30000;
			else if(wireInfo.m_nVoltGrade>=500&&wireInfo.m_nDaoXianFengLieNum>4)
				T0=40000;
			else
				T0=0.3*pSetupGuaXianLoad->m_fLoadY;
		}
			//垂直荷载
		pCommonLoad->m_fWireLoadZ=pSetupGuaXianLoad->m_fWireLoadZ+T0*tan(anchor_angle);
		pCommonLoad->m_fAttachmentLoadZ=pSetupGuaXianLoad->m_fAttachmentLoadZ;
			//纵向荷载
		pCommonLoad->m_fLoadY=fabs(pSetupGuaXianLoad->m_fLoadY-T0);
			//横向荷载
		pCommonLoad->m_fWireLoadX=pSetupGuaXianLoad->m_fWireLoadX;
		pCommonLoad->m_fAttachmentLoadX=pJinXianLoad->m_fAttachmentLoadX;
		//12、正锚相(安装正挂另档未挂)
		pCommonLoad=hashLoad.GetValue(WORKENVLOAD_SETUPANCHORING);
		pCommonLoad->huchui=pSetupGuaXianLoad->huchui;
			//垂直荷载
		pCommonLoad->m_fWireLoadZ=pSetupGuaXianLoad->m_fWireLoadZ+T0*tan(anchor_angle);
		pCommonLoad->m_fAttachmentLoadZ=pSetupGuaXianLoad->m_fAttachmentLoadZ;
		pCommonLoad->m_fSetupGa=pJinXianLoad->m_fSetupGa;
			//纵向荷载
		pCommonLoad->m_fLoadY=fabs(1.1*pSetupGuaXianLoad->m_fLoadY-T0);
			//横向荷载
		pCommonLoad->m_fWireLoadX=pSetupGuaXianLoad->m_fWireLoadX;
		pCommonLoad->m_fAttachmentLoadX=pJinXianLoad->m_fAttachmentLoadX;
		//15 抗震验算（按大风荷载的30%计,见《杆塔结构设计技术规定》DL/T 5154-2002 P16）
		CWorkEnvLoad *pWindEQLoad=hashLoad.GetValue(WORKENVLOAD_EARTHQUAKE);
		gama=sqrt(0.3*gama_max_w*gama_max_w+gama_g*gama_g);
		m_pResolver->XuanLianByCursorPara(gama,weather.m_nMaxWindT);
		pWindEQLoad->huchui=m_pResolver->XuanLianPosO();
			//垂直荷载
		pWindEQLoad->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ;
		pWindEQLoad->m_fAttachmentLoadZ=pLowestTempLoad->m_fAttachmentLoadZ;
			//纵向荷载
		pWindEQLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
			//横向荷载，考虑到抗震验算时线条荷载要乘0.5分项系数，此处风荷载按60%计
		pWindEQLoad->m_fWireLoadX=0.6*pWind90Load->m_fWireLoadX;
		pWindEQLoad->m_fAttachmentLoadX=0.6*pWind90Load->m_fAttachmentLoadX;
	}
	else
	{
		InitDianXianEquation(weather,gama_g,gama_ice_w,gama_max_w,gama_ice);
		//1 90度大风荷载
		CWorkEnvLoad *pWind90Load=hashLoad.GetValue(WORKENVLOAD_WIND);
		//XuanLianByCursorPara(gama,weather.m_nMaxWindT);
		//pWind90Load->huchui=XuanLianPosO();
			//垂直荷载
		pWind90Load->m_fWireLoadZ=wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.vertical_space*vert_coef*wireInfo.wire.m_fUnitWeight*0.0098;
		pWind90Load->m_fAttachmentLoadZ=wireInfo.m_fAttachmentWeight*9.8;
		pWind90Load->m_fMinVertSpaceWireLoadZ=wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.min_vert_space*vert_coef*wireInfo.wire.m_fUnitWeight*0.0098;
			//纵向荷载
		pWind90Load->m_fLoadY=0;
			//横向荷载
		pWind90Load->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_max_w*xian_area*1e-6;
		pWind90Load->m_fAttachmentLoadX=wireInfo.m_fAttachmentLength*wireInfo.m_fAttachmentDiameter*muZ*weather.m_fMaxWindV*weather.m_fMaxWindV/1600000;
		//2 覆冰工况荷载
		CWorkEnvLoad *pIceLoad=hashLoad.GetValue(WORKENVLOAD_ICE);
		//XuanLianByCursorPara(gama,weather.m_nMaxIceT);
		//pIceLoad->huchui=XuanLianPosO();
			//垂直荷载
		pIceLoad->m_fWireLoadZ=pWind90Load->m_fWireLoadZ+
			wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.vertical_space*vert_coef*gama_ice*xian_area*1e-6;
		pIceLoad->m_fAttachmentLoadZ=pWind90Load->m_fAttachmentLoadZ+
			8820*Pi*weather.m_fMaxIceThick*(weather.m_fMaxIceThick+wireInfo.m_fAttachmentDiameter)*wireInfo.m_fAttachmentLength*1e-9;
			//纵向荷载
		pIceLoad->m_fLoadY=0;
			//横向荷载
		pIceLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_ice_w*xian_area*1e-6;
		pIceLoad->m_fAttachmentLoadX=weather.m_fMaxIceWindV*weather.m_fMaxIceWindV*muZ*
			(wireInfo.m_fAttachmentDiameter+weather.m_fMaxIceThick*2)*wireInfo.m_fAttachmentLength/1600000;
		//3 不均匀覆冰(-5°C,10m/s)
		CWorkEnvLoad *pDiffIceLoad=hashLoad.GetValue(WORKENVLOAD_ICEDIFF);
		alfa=FindAlfa(10);
		betaC=FindBetaC(10,wireInfo.m_nVoltGrade);
		iceB=FindIceB1(weather.m_fMaxIceThick);
		W0=0.0625;//10*10/1600
		gama_w=1e6*alfa*W0*muZ*betaC*muSC*iceB*(xian_dia+weather.m_fMaxIceThick*2)/xian_area;
			//垂直荷载
		pDiffIceLoad->m_fWireLoadZ=0.875*pIceLoad->m_fWireLoadZ+0.125*pWind90Load->m_fWireLoadZ;
		pDiffIceLoad->m_fAttachmentLoadZ=pIceLoad->m_fAttachmentLoadZ;
			//纵向荷载
		if(wireInfo.m_cXianType=='E')
			pDiffIceLoad->m_fLoadY=0.2*max_tension;
		else
			pDiffIceLoad->m_fLoadY=0.1*max_tension;
			//横向荷载
		pDiffIceLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_w*xian_area*1e-6;
		pDiffIceLoad->m_fAttachmentLoadX=W0*muZ*
			(wireInfo.m_fAttachmentDiameter+weather.m_fMaxIceThick*2)*wireInfo.m_fAttachmentLength/1e6;
		//4 断线工况事故相荷载(有冰、无风、-5°C)
		CWorkEnvLoad *pDuanXianLoad=hashLoad.GetValue(WORKENVLOAD_DUANXIANRUNNING);
			//垂直荷载
		pDuanXianLoad->m_fWireLoadZ=pIceLoad->m_fWireLoadZ;
		pDuanXianLoad->m_fAttachmentLoadZ=pIceLoad->m_fAttachmentLoadZ;
			//纵向荷载
		if(wireInfo.m_cXianType=='C')
		{	//导线
			if(cLineRoadType<2)	//平原或丘陵
			{
				if(wireInfo.m_nDaoXianFengLieNum<=1)
					pDuanXianLoad->m_fLoadY=0.5*wireInfo.m_nDaoXianFengLieNum*max_tension;
				else if(wireInfo.m_nDaoXianFengLieNum<=2)	//双分裂
					pDuanXianLoad->m_fLoadY=0.25*wireInfo.m_nDaoXianFengLieNum*max_tension;
				else
					pDuanXianLoad->m_fLoadY=0.20*wireInfo.m_nDaoXianFengLieNum*max_tension;
			}
			else				//山区线路
			{
				if(wireInfo.m_nDaoXianFengLieNum<=1)
					pDuanXianLoad->m_fLoadY=0.5*wireInfo.m_nDaoXianFengLieNum*max_tension;
				else if(wireInfo.m_nDaoXianFengLieNum<=2)	//双分裂
					pDuanXianLoad->m_fLoadY=0.30*wireInfo.m_nDaoXianFengLieNum*max_tension;
				else
					pDuanXianLoad->m_fLoadY=0.25*wireInfo.m_nDaoXianFengLieNum*max_tension;
			}
		}
		else//地线
			pDuanXianLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*max_tension;
			//横向荷载
		pDuanXianLoad->m_fLoadX=0;
		//5 断线工况正常相荷载
		pCommonLoad=hashLoad.GetValue(WORKENVLOAD_DUANXIANBREAK);
		//垂直荷载(垂直荷载按4h长拖地线计算)
		pCommonLoad->m_fWireLoadZ=pIceLoad->m_fWireLoadZ;
			//纵向荷载
		pCommonLoad->m_fLoadY=0;
			//横向荷载
		pIceLoad->m_fLoadX=0;
		//6 安装已吊相
		CWorkEnvLoad *pSuspendLoad=hashLoad.GetValue(WORKENVLOAD_SETUPSUSPEND);
		if(xian_dia<17)
			muSC=1.2;
		else
			muSC=1.1;
		alfa=FindAlfa(weather.m_fSetupWindV);
		betaC=FindBetaC(weather.m_fSetupWindV,wireInfo.m_nVoltGrade);
		W0=weather.m_fSetupWindV*weather.m_fSetupWindV/1600;
		gama_w=1e6*alfa*W0*muZ*betaC*muSC*xian_dia/xian_area;
		//垂直荷载
		pSuspendLoad->m_fWireLoadZ=pWind90Load->m_fWireLoadZ;
		pSuspendLoad->m_fAttachmentLoadZ=pWind90Load->m_fAttachmentLoadZ;
		if(wireInfo.m_cXianType=='E')
		{
			if(wireInfo.m_nVoltGrade<=110)
				pSuspendLoad->m_fSetupGa=1000;
			else //if(wireInfo.m_nVoltGrade<=330)
				pSuspendLoad->m_fSetupGa=2000;
		}
		else //if
		{
			if(wireInfo.m_nVoltGrade<=110)
				pSuspendLoad->m_fSetupGa=1500;
			else if(wireInfo.m_nVoltGrade<=330)
				pSuspendLoad->m_fSetupGa=3500;
			else
				pSuspendLoad->m_fSetupGa=4000;
		}
			//纵向荷载
		pSuspendLoad->m_fLoadY=0;
			//横向荷载
		pSuspendLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_w*xian_area*1e-6;
		pSuspendLoad->m_fAttachmentLoadX=W0*muZ*wireInfo.m_fAttachmentDiameter*wireInfo.m_fAttachmentLength/1e6;
		//7 安装正吊相
		CWorkEnvLoad *pSuspendingLoad=hashLoad.GetValue(WORKENVLOAD_SETUPSUSPENDING);
			//垂直荷载
		double multiply_coef=2.0;
		pSuspendingLoad->m_fWireLoadZ=pWind90Load->m_fWireLoadZ*multiply_coef*1.1;
		pSuspendingLoad->m_fAttachmentLoadZ=wireInfo.m_fAttachmentWeight*9.8;
		pSuspendingLoad->m_fSetupGa=pSuspendLoad->m_fSetupGa;
			//纵向荷载
		pSuspendingLoad->m_fLoadY=0;
			//横向荷载
		pSuspendingLoad->m_fWireLoadX=pSuspendLoad->m_fWireLoadX;
		pSuspendingLoad->m_fAttachmentLoadX=pSuspendLoad->m_fAttachmentLoadX;
		//8 已锚相
		CWorkEnvLoad *pAnchorLoad=hashLoad.GetValue(WORKENVLOAD_ANCHORFINISHED);
		double T=max_tension;
		if(wireInfo.xianlu.represent_space>0)
		{
			double gama=sqrt(gama_g*gama_g+gama_w*gama_w);
			m_pResolver->XuanLianByCursorPara(gama,weather.m_nSetupT);
			pAnchorLoad->huchui=m_pResolver->XuanLianPosO();
			T=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
		}
		if(wireInfo.m_cXianType=='E')
			T*=1.07625;//1.05*1.025;	过牵引系数乘施工误差系数
		else
			T*=1.101875;//1.075*1.025;
			//垂直荷载
		pAnchorLoad->m_fWireLoadZ=pSuspendLoad->m_fWireLoadZ+T*sin(anchor_angle);
		pAnchorLoad->m_fAttachmentLoadZ=pSuspendLoad->m_fAttachmentLoadZ;
		pAnchorLoad->m_fSetupGa=pSuspendLoad->m_fSetupGa;
			//纵向荷载
		pAnchorLoad->m_fLoadY=T*(1-cos(anchor_angle));
			//横向荷载
		pAnchorLoad->m_fWireLoadX=pSuspendLoad->m_fWireLoadX;
		pAnchorLoad->m_fAttachmentLoadX=pSuspendLoad->m_fAttachmentLoadX;
		//9 正锚相
		pCommonLoad=hashLoad.GetValue(WORKENVLOAD_SETUPANCHORING);
		pCommonLoad->huchui=pAnchorLoad->huchui;
			//垂直荷载
		pCommonLoad->m_fWireLoadZ=pSuspendLoad->m_fWireLoadZ+1.1*T*sin(anchor_angle);
		pCommonLoad->m_fAttachmentLoadZ=pAnchorLoad->m_fAttachmentLoadZ;
		pCommonLoad->m_fSetupGa=pSuspendingLoad->m_fSetupGa;

			//纵向荷载
		pCommonLoad->m_fLoadY=1.1*pAnchorLoad->m_fLoadY;
			//横向荷载
		pCommonLoad->m_fWireLoadX=pAnchorLoad->m_fWireLoadX;
		pCommonLoad->m_fAttachmentLoadX=pAnchorLoad->m_fAttachmentLoadX;
		//15 抗震验算（按大风荷载的30%计,见《杆塔结构设计技术规定》DL/T 5154-2002 P16）
		CWorkEnvLoad *pWindEQLoad=hashLoad.GetValue(WORKENVLOAD_EARTHQUAKE);
		if(pWindEQLoad)
		{	//垂直荷载
			pWindEQLoad->m_fWireLoadZ=pWind90Load->m_fWireLoadZ;
			pWindEQLoad->m_fAttachmentLoadZ=pWind90Load->m_fAttachmentLoadZ;
			//纵向荷载
			pWindEQLoad->m_fLoadY=0;
			//横向荷载，考虑到抗震验算时线条荷载要乘0.5分项系数，此处风荷载按60%计
			pWindEQLoad->m_fWireLoadX=0.6*pWind90Load->m_fWireLoadX;
			pWindEQLoad->m_fAttachmentLoadX=0.6*pWind90Load->m_fAttachmentLoadX;
		}
	}
	for(pCommonLoad=hashLoad.GetFirst();pCommonLoad;pCommonLoad=hashLoad.GetNext())
		pCommonLoad->SumCal();
#endif
}
#endif