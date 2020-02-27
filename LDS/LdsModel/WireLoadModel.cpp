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
	buffer.ReadInteger(&m_nHighestT);	//�������
	buffer.ReadInteger(&m_nLowestT);	//�������
	buffer.ReadInteger(&m_nAverageT);	//ƽ������
	buffer.ReadDouble(&m_fMaxWindV);		//������
	buffer.ReadInteger(&m_nMaxWindT);	//������ʱ�¶�
	buffer.ReadDouble(&m_fMaxIceThick);			//��󸲱���(mm)
	buffer.ReadInteger(&m_nMaxIceT);			//��󸲱�ʱ�¶�
	buffer.ReadDouble(&m_fMaxIceWindV);			//��󸲱�ʱ����
	buffer.ReadInteger(&m_nSetupT);				//��װʱ�¶�
	buffer.ReadDouble(&m_fSetupWindV);	//��װʱ����
}
void WEATHER::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(m_nHighestT);	//�������
	buffer.WriteInteger(m_nLowestT);	//�������
	buffer.WriteInteger(m_nAverageT);	//ƽ������
	buffer.WriteDouble(m_fMaxWindV);	//������
	buffer.WriteInteger(m_nMaxWindT);	//������ʱ�¶�
	buffer.WriteDouble(m_fMaxIceThick);	//��󸲱���(mm)
	buffer.WriteInteger(m_nMaxIceT);	//��󸲱�ʱ�¶�
	buffer.WriteDouble(m_fMaxIceWindV);	//��󸲱�ʱ����
	buffer.WriteInteger(m_nSetupT);		//��װʱ�¶�
	buffer.WriteDouble(m_fSetupWindV);	//��װʱ����
}
void WEATHER::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[50]="";
	fprintf(fp,"<������Ϣ>\n");
	fprintf(fp,"<������� HighestT=\"%d\"/>\n",m_nHighestT);
	fprintf(fp,"<������� LowestT=\"%d\"/>\n",m_nLowestT);
	fprintf(fp,"<ƽ������ AvergeT=\"%d\"/>\n",m_nAverageT);
	sprintf(sText,"%f",m_fMaxWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<������ MaxWindV=\"%s\"/>\n",sText);
	fprintf(fp,"<������ʱ�¶� MaxWindT=\"%d\"/>\n",m_nMaxWindT);
	sprintf(sText,"%f",m_fMaxIceThick);
	SimplifiedNumString(sText);
	fprintf(fp,"<��󸲱��� MaxIceThick=\"%s\"/>\n",sText);
	fprintf(fp,"<��󸲱�ʱ�¶� MaxIceT=\"%d\"/>\n",m_nMaxIceT);
	sprintf(sText,"%f",m_fMaxIceWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<��󸲱�ʱ���� MaxIceWindV=\"%s\"/>\n",sText);
	fprintf(fp,"<��װʱ�¶� SetupT=\"%d\"/>\n",m_nSetupT);
	sprintf(sText,"%f",m_fSetupWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<��װʱ���� SetupWindV=\"%s\"/>\n",sText);
	fprintf(fp,"</������Ϣ>\n");
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
	fprintf(fp,"<�������� weatherCode=\"%s\">\n",weatherCode);
	WEATHER::ToXmlFile(fp,schema);
	fprintf(fp,"</��������>\n");
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
	propHashtable.SetValueAt("m_nHighestT",id++);//�������
	propHashtable.SetValueAt("m_nLowestT",id++);	//�������
	propHashtable.SetValueAt("m_nAverageT",id++);//ƽ���¶�
	propHashtable.SetValueAt("m_nMaxWindT",id++);	//�������¶�
	propHashtable.SetValueAt("m_fMaxWindV",id++);		//������
	propHashtable.SetValueAt("m_fMaxWindVCheck",id++);	//����������
	propHashtable.SetValueAt("m_nMaxIceT",id++);	//��󸲱��¶�
	propHashtable.SetValueAt("m_fMaxIceWindV",id++);//��󸲱�����
	propHashtable.SetValueAt("m_fMaxIceThick",id++);			//��󸲱����
	propHashtable.SetValueAt("m_fMaxIceThickCheck",id++);		//��󸲱��������
	propHashtable.SetValueAt("m_nSetupT",id++);	//��װ�¶�
	propHashtable.SetValueAt("m_fSetupWindV",id++);	//��װ����
}

int CWireLoadModel::GetPropValueStr(long id,char *valueStr)				//�õ�����ֵ�ַ���
{
	char sText[100]="";
	if(GetPropID("hangingWireType")==id)
	{
		if(wireInfo.m_cXianType=='E')
			strcpy(sText,"����");
		else if(wireInfo.m_cXianType=='C')
			strcpy(sText,"����");
		else if(wireInfo.m_cXianType!=0)
			strcpy(sText,"����");
		else
			strcpy(sText,"δ����");
	}
	else if(GetPropID("hangingStyle")==id)
	{
		if(wireInfo.m_bNaiZhang)
			strcpy(sText,"���Ź���");
		else
			strcpy(sText,"��������");
	}
	else if(GetPropID("hangingSide")==id)
	{
		if(iNo%10==0)
			strcpy(sText,"ǰ��");
		else
			strcpy(sText,"���");
	}
	else if(GetPropID("m_cFormula")==id)
	{
		if(wireInfo.m_cFormula==0)
			strcpy(sText,"������");
		else
			strcpy(sText,"б������");
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

int CWireLoadModel::GetPropStr(long id,char *propStr)		//�õ������ַ���
{
	char sText[100]="";
	if(GetPropID("wireInfo")==0)
		strcpy(sText,"wireInfo");
	if(propStr)
		strcpy(propStr,sText);
	return strlen(sText);
}
long CWireLoadModel::GetPropID(char* propStr)	//�õ�����ID
{
	long id;
	if(CWireLoadModel::propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}
BOOL CWireLoadModel::IsPropEqual(OBJSET &selectObjs, long idProp)	//�������ֵ�Ƿ����
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
int CWireLoadModel::GetPropStatus(long id)					//�õ�����״̬
{
	DWORD dwStatus;
	if(CWireLoadModel::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}
BOOL CWireLoadModel::SetPropStatus(long id,BOOL bHide)		//��������״̬
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
	if(wireInfo.m_bNaiZhang)	//����
	{
		CWorkEnvLoad *pRawLoad=hashLoad.Add(WORKENVLOAD_WIND);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"���");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_ICE);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"����");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_LOWTEMP);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"����");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_ICELESS);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"�ѱ������ٱ�");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_ICEMORE);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"�ѱ��������");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_DUANXIANRUNNING);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"�¹�������");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_DUANXIANBREAK);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"�¹���");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPSUSPEND);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"�Ѱ�װ��");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPJINXIAN_OTHER_FINISH);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"��װ�������ѹ�");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPJINXIAN_OTHER_EMPTY);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"��װ������δ��");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_ANCHORFINISHED);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"��װ��ê��");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPANCHORING);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"��װ������");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_EARTHQUAKE);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"��������");
	}
	else	//��������
	{
		
		CWorkEnvLoad *pRawLoad=hashLoad.Add(WORKENVLOAD_WIND);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"���");

		pRawLoad=hashLoad.Add(WORKENVLOAD_ICE);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"����");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_ICEDIFF);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"�����ȸ���");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_DUANXIANRUNNING);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"�¹�������");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_DUANXIANBREAK);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"�¹���");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPSUSPEND);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"��װ�Ѱ�װ��");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPSUSPENDING);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"��װ������");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_ANCHORFINISHED);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"��װ��ê��");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_SETUPANCHORING);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"��װ������");
		
		pRawLoad=hashLoad.Add(WORKENVLOAD_EARTHQUAKE);
		memset(pRawLoad,0,sizeof(CWorkEnvLoad));
		strcpy(pRawLoad->name,"��������");
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
	//��������
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
	buffer.ReadByte(&wireInfo.m_cFormula);	//�¼�
	buffer.ReadBooleanThin(&wireInfo.m_bNaiZhang);
	buffer.ReadInteger(&wireInfo.m_nVoltGrade);
	buffer.ReadByte(&wireInfo.m_nDaoXianFengLieNum);
	buffer.ReadString(wireInfo.specAttachment);			//��߼���Ե�ӵȸ�������ϸ
	buffer.ReadDouble(&wireInfo.m_fAttachmentWeight);			//��߼���Ե�ӵȸ���������(kg)
	buffer.ReadDouble(&wireInfo.m_fAttachmentDiameter);		//��Ե�ӵȸ�������Ȼ��⾶(mm)
	buffer.ReadDouble(&wireInfo.m_fAttachmentLength);			//��Ե�ӵȸ���������(m)
	buffer.ReadDouble(&wireInfo.m_fAchorLineAngle);			//ê�߶Եؼн�
	buffer.ReadDouble(&wireInfo.m_fTractionLineAngle);		//ǣ���߶Եؼн�
	//��������
	if(nVersion==0||(doc_type==3&&nVersion>=1070101)||(doc_type==4&&nVersion>=1000101))
	{
		buffer.ReadDouble(&weather.m_fMaxIceThick);		//��󸲱����(mm)
		buffer.ReadDouble(&weather.m_fMaxIceThickCheck);//���㸲�����(mm)
		buffer.ReadDouble(&weather.m_fMaxIceWindV);
		buffer.ReadDouble(&weather.m_fMaxWindV);		//����ٶ�(m/s)
		buffer.ReadDouble(&weather.m_fMaxWindVCheck);	//�������(m/s)
		buffer.ReadDouble(&weather.m_fSetupWindV);
		buffer.ReadInteger(&weather.m_nAverageT);
		buffer.ReadInteger(&weather.m_nHighestT);
		buffer.ReadInteger(&weather.m_nLowestT);
		buffer.ReadInteger(&weather.m_nMaxIceT);
		buffer.ReadInteger(&weather.m_nMaxWindT);
		buffer.ReadInteger(&weather.m_nSetupT);
	}
	//��·��Ϣ
	buffer.ReadDouble(&wireInfo.xianlu.horizon_space);		//ˮƽ����
	buffer.ReadDouble(&wireInfo.xianlu.vertical_space);		//��ֱ����
	buffer.ReadDouble(&wireInfo.xianlu.min_vert_space);		//��С��ֱ����
	buffer.ReadDouble(&wireInfo.xianlu.horizon_space_allotcoef);	//ˮƽ�������ϵ��
	buffer.ReadDouble(&wireInfo.xianlu.vertical_space_allotcoef);//��ֱ�������ϵ��
	buffer.ReadDouble(&wireInfo.xianlu.represent_space);	//������
	buffer.ReadDouble(&wireInfo.xianlu.HtoL);				//�߲�ϵ��
	buffer.ReadDouble(&wireInfo.xianlu.xianlu_angle);		//��λ��(��)
	//��������Ϣ
	buffer.ReadDouble(&wireInfo.wire.m_fDiameter);		// �����⾶,mm
	buffer.ReadDouble(&wireInfo.wire.m_fArea);			// ���߼�������,mm2
	buffer.ReadDouble(&wireInfo.wire.m_fUnitWeight);	// ���ߵ�λ������,kg/km
	buffer.ReadDouble(&wireInfo.wire.m_fForcePermit);	// ���߶������,N
	buffer.ReadDouble(&wireInfo.wire.m_fSafeCoef);		// ���߰�ȫϵ��
	buffer.ReadDouble(&wireInfo.wire.m_fMaxUseTension);// �������ʹ������,N
	buffer.ReadDouble(&wireInfo.wire.m_fElasticCoef);	// ���ߵ���ϵ��,MPa
	buffer.ReadDouble(&wireInfo.wire.m_fExpansionCoef);// �����¶�������ϵ��,1/1/��C
	buffer.ReadString(wireInfo.wire.specWireType);	// ���ߵ����ʹ���
	buffer.ReadString(wireInfo.wire.specWire);		// ���ߵĹ���ͺ�����
	//��������Ϣ
	if(nVersion==0||(doc_type==3&&nVersion>=1070005)||(doc_type==4&&nVersion>=1000016))
	{
		long n2;
		buffer.ReadInteger(&n2);
		for(int j=0;j<n2;j++)
		{
			long serial=0;
			buffer.ReadInteger(&serial);
			CWorkEnvLoad *pLoad=hashLoad.Add(serial);
			buffer.ReadString(pLoad->name);					//��Ӧ��������
			buffer.ReadDouble(&pLoad->m_fLoadX);				//�����ṩ������������ܺ�(N)
			buffer.ReadDouble(&pLoad->m_fLoadY);				//�����ṩ��������ܺ�(N)
			buffer.ReadDouble(&pLoad->m_fLoadZ);				//�����ṩ��ֱ�����ܺ�(N)
			buffer.ReadDouble(&pLoad->m_fMinVertSpaceLoadX);	//�����ṩ�������(��С��ֱ����)�ܺ�(N)
			buffer.ReadDouble(&pLoad->m_fMinVertSpaceLoadY);	//�����ṩ�������(��С��ֱ����)�ܺ�(N)
			buffer.ReadDouble(&pLoad->m_fMinVertSpaceLoadZ);	//�����ṩ��ֱ����(��С��ֱ����)�ܺ�(N)
			buffer.ReadDouble(&pLoad->m_fWireLoadX);			//���ߵķ����(N)
			buffer.ReadDouble(&pLoad->m_fWireLoadZ);			//���ߵĴ�ֱ����(N)
			buffer.ReadDouble(&pLoad->m_fMinVertSpaceWireLoadZ);//��С��ֱ����ʱ���ߵĴ�ֱ����(N)
			buffer.ReadDouble(&pLoad->m_fAttachmentLoadX);		//�����������(������Ӱ��)(N)
			buffer.ReadDouble(&pLoad->m_fAttachmentLoadZ);		//��������ֱ����(������Ӱ��)(N)
			buffer.ReadDouble(&pLoad->m_fSetupGa);				//��װʱ����ĸ��Ӵ�ֱ����(N)
			if(nVersion==0||(doc_type==3&&nVersion>=1070006)||(doc_type==4&&nVersion>=1000017))
				buffer.ReadDouble(&pLoad->huchui);					//����
		}
	}
}
void CWireLoadModel::ToBuffer(CBuffer &buffer)
{
	buffer.WriteString(name);
	buffer.WriteDouble(wireInfo.land_high);
	buffer.WriteByte(wireInfo.m_cXianType);
	buffer.WriteByte(wireInfo.m_cFormula);	//�¼�
	buffer.WriteBooleanThin(wireInfo.m_bNaiZhang);
	buffer.WriteInteger(wireInfo.m_nVoltGrade);
	buffer.WriteByte(wireInfo.m_nDaoXianFengLieNum);
	buffer.WriteString(wireInfo.specAttachment);			//��߼���Ե�ӵȸ�������ϸ
	buffer.WriteDouble(wireInfo.m_fAttachmentWeight);			//��߼���Ե�ӵȸ���������(kg)
	buffer.WriteDouble(wireInfo.m_fAttachmentDiameter);		//��Ե�ӵȸ�������Ȼ��⾶(mm)
	buffer.WriteDouble(wireInfo.m_fAttachmentLength);			//��Ե�ӵȸ���������(m)
	buffer.WriteDouble(wireInfo.m_fAchorLineAngle);			//ê�߶Եؼн�
	buffer.WriteDouble(wireInfo.m_fTractionLineAngle);		//ǣ���߶Եؼн�
	//��������
	buffer.WriteDouble(weather.m_fMaxIceThick);		//��󸲱����(mm)
	buffer.WriteDouble(weather.m_fMaxIceThickCheck);//���㸲�����(mm)
	buffer.WriteDouble(weather.m_fMaxIceWindV);
	buffer.WriteDouble(weather.m_fMaxWindV);		//����ٶ�(m/s)
	buffer.WriteDouble(weather.m_fMaxWindVCheck);	//�������(m/s)
	buffer.WriteDouble(weather.m_fSetupWindV);
	buffer.WriteInteger(weather.m_nAverageT);
	buffer.WriteInteger(weather.m_nHighestT);
	buffer.WriteInteger(weather.m_nLowestT);
	buffer.WriteInteger(weather.m_nMaxIceT);
	buffer.WriteInteger(weather.m_nMaxWindT);
	buffer.WriteInteger(weather.m_nSetupT);
	//��·��Ϣ
	buffer.WriteDouble(wireInfo.xianlu.horizon_space);	//ˮƽ����
	buffer.WriteDouble(wireInfo.xianlu.vertical_space);	//��ֱ����
	buffer.WriteDouble(wireInfo.xianlu.min_vert_space);	//��С��ֱ����
	buffer.WriteDouble(wireInfo.xianlu.horizon_space_allotcoef);	//ˮƽ�������ϵ��
	buffer.WriteDouble(wireInfo.xianlu.vertical_space_allotcoef);//��ֱ�������ϵ��
	buffer.WriteDouble(wireInfo.xianlu.represent_space);	//������
	buffer.WriteDouble(wireInfo.xianlu.HtoL);			//�߲�ϵ��
	buffer.WriteDouble(wireInfo.xianlu.xianlu_angle);	//��λ��(��)
	//��������Ϣ
	buffer.WriteDouble(wireInfo.wire.m_fDiameter);		// �����⾶,mm
	buffer.WriteDouble(wireInfo.wire.m_fArea);			// ���߼�������,mm2
	buffer.WriteDouble(wireInfo.wire.m_fUnitWeight);	// ���ߵ�λ������,kg/km
	buffer.WriteDouble(wireInfo.wire.m_fForcePermit);	// ���߶������,N
	buffer.WriteDouble(wireInfo.wire.m_fSafeCoef);		// ���߰�ȫϵ��
	buffer.WriteDouble(wireInfo.wire.m_fMaxUseTension);// �������ʹ������,N
	buffer.WriteDouble(wireInfo.wire.m_fElasticCoef);	// ���ߵ���ϵ��,MPa
	buffer.WriteDouble(wireInfo.wire.m_fExpansionCoef);// �����¶�������ϵ��,1/1/��C
	buffer.WriteString(wireInfo.wire.specWireType);	// ���ߵ����ʹ���
	buffer.WriteString(wireInfo.wire.specWire);		// ���ߵĹ���ͺ�����
	//��������Ϣ
	buffer.WriteInteger(hashLoad.GetNodeNum());
	for(CWorkEnvLoad *pLoad=hashLoad.GetFirst();pLoad;pLoad=hashLoad.GetNext())
	{
		buffer.WriteInteger(hashLoad.GetCursorKey());
		buffer.WriteString(pLoad->name);					//��Ӧ��������
		buffer.WriteDouble(pLoad->m_fLoadX);				//�����ṩ������������ܺ�(N)
		buffer.WriteDouble(pLoad->m_fLoadY);				//�����ṩ��������ܺ�(N)
		buffer.WriteDouble(pLoad->m_fLoadZ);				//�����ṩ��ֱ�����ܺ�(N)
		buffer.WriteDouble(pLoad->m_fMinVertSpaceLoadX);	//�����ṩ�������(��С��ֱ����)�ܺ�(N)
		buffer.WriteDouble(pLoad->m_fMinVertSpaceLoadY);	//�����ṩ�������(��С��ֱ����)�ܺ�(N)
		buffer.WriteDouble(pLoad->m_fMinVertSpaceLoadZ);	//�����ṩ��ֱ����(��С��ֱ����)�ܺ�(N)
		buffer.WriteDouble(pLoad->m_fWireLoadX);			//���ߵķ����(N)
		buffer.WriteDouble(pLoad->m_fWireLoadZ);			//���ߵĴ�ֱ����(N)
		buffer.WriteDouble(pLoad->m_fMinVertSpaceWireLoadZ);//��С��ֱ����ʱ���ߵĴ�ֱ����(N)
		buffer.WriteDouble(pLoad->m_fAttachmentLoadX);		//�����������(������Ӱ��)(N)
		buffer.WriteDouble(pLoad->m_fAttachmentLoadZ);		//��������ֱ����(������Ӱ��)(N)
		buffer.WriteDouble(pLoad->m_fSetupGa);				//��װʱ����ĸ��Ӵ�ֱ����(N)
		buffer.WriteDouble(pLoad->huchui);					//����
	}
}
void CWireLoadModel::ToXmlFile(FILE *fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<�������ؼ���ģ����Ϣ name=\"%s\">\n",name);
	fprintf(fp,"<������Ϣ>\n");
	sprintf(sText,"%f",wireInfo.land_high);
	SimplifiedNumString(sText);
	fprintf(fp,"<���Ƹ߶� land_high=\"%s\"/>\n",sText);
	fprintf(fp,"<���� XianType=\"%c\"/>\n",wireInfo.m_cXianType);
	fprintf(fp,"<�������㷽ʽ Formula=\"%c\"/>\n",wireInfo.m_cFormula);
	if(wireInfo.m_bNaiZhang)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<���� bNaiZhang=\"%s\"/>\n",sText);
	fprintf(fp,"<��ѹ�ȼ� VoltGrade=\"%d\"/>\n",wireInfo.m_nVoltGrade);
	fprintf(fp,"<���߷����� FengLieNum=\"%d\"/>\n",wireInfo.m_nDaoXianFengLieNum);
	fprintf(fp,"<��������ϸ specAttachment=\"%s\"/>\n",wireInfo.specAttachment);
	sprintf(sText,"%f",wireInfo.m_fAttachmentWeight);
	SimplifiedNumString(sText);
	fprintf(fp,"<���������� AttachmentWeight=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.m_fAttachmentDiameter);
	SimplifiedNumString(sText);
	fprintf(fp,"<�������⾶ AttachmentDiameter=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.m_fAttachmentLength);
	SimplifiedNumString(sText);
	fprintf(fp,"<���������� AttachmentLength=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.m_fAchorLineAngle);
	SimplifiedNumString(sText);
	fprintf(fp,"<ê�߶Եؼн� AttachLineAngle=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.m_fTractionLineAngle);
	SimplifiedNumString(sText);
	fprintf(fp,"<ǣ���߶Եؼн� TractionLineAngle=\"%s\"/>\n",sText);
	fprintf(fp,"</������Ϣ>\n");
	fprintf(fp,"<��������>\n");
	sprintf(sText,"%f",weather.m_fMaxIceThick);
	SimplifiedNumString(sText);
	fprintf(fp,"<��󸲱���� MaxIceThick=\"%s\"/>\n",sText);
	sprintf(sText,"%f",weather.m_fMaxIceThickCheck);
	SimplifiedNumString(sText);
	fprintf(fp,"<���㸲����� MaxIceThickCheck=\"%s\"/>\n",sText);
	sprintf(sText,"%f",weather.m_fMaxIceWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<����ʱ���� MaxIceWindV=\"%s\"/>\n",sText);
	sprintf(sText,"%f",weather.m_fMaxWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<������ MaxWindV=\"%s\"/>\n",sText);
	sprintf(sText,"%f",weather.m_fMaxWindVCheck);
	SimplifiedNumString(sText);
	fprintf(fp,"<������� MaxWindVCheck=\"%s\"/>\n",sText);
	sprintf(sText,"%f",weather.m_fSetupWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<��װʱ���� SetupWindV=\"%s\"/>\n",sText);
	fprintf(fp,"<ƽ������ AverageT=\"%d\"/>\n",weather.m_nAverageT);
	fprintf(fp,"<������� HighestT=\"%d\"/>\n",weather.m_nHighestT);
	fprintf(fp,"<������� LowestT=\"%d\"/>\n",weather.m_nLowestT);
	fprintf(fp,"<�����¶� MaxIceT=\"%d\"/>\n",weather.m_nMaxIceT);
	fprintf(fp,"<���ʱ�¶� MaxWindT=\"%d\"/>\n",weather.m_nMaxWindT);
	fprintf(fp,"<��װʱ�¶� SetupT=\"%d\"/>\n",weather.m_nSetupT);
	fprintf(fp,"</��������>\n");
	fprintf(fp,"<��·��Ϣ>\n");
	sprintf(sText,"%f",wireInfo.xianlu.horizon_space);
	SimplifiedNumString(sText);
	fprintf(fp,"<ˮƽ���� horizon_space=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.vertical_space);
	SimplifiedNumString(sText);
	fprintf(fp,"<��ֱ���� vertical_space=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.min_vert_space);
	SimplifiedNumString(sText);
	fprintf(fp,"<��С��ֱ���� min_vert_space=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.horizon_space_allotcoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<ˮƽ�������ϵ�� horizon_space_allotcoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.vertical_space_allotcoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<��ֱ�������ϵ�� vertical_space_allotcoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.represent_space);
	SimplifiedNumString(sText);
	fprintf(fp,"<������ represent_space=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.HtoL);
	SimplifiedNumString(sText);
	fprintf(fp,"<�߲�ϵ�� HtoL=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.xianlu.xianlu_angle);
	SimplifiedNumString(sText);
	fprintf(fp,"<�߼н� xianlu_angle=\"%s\"/>\n",sText);
	fprintf(fp,"</��·��Ϣ>\n");
	fprintf(fp,"<��������Ϣ>\n");
	sprintf(sText,"%f",wireInfo.wire.m_fDiameter);
	SimplifiedNumString(sText);
	fprintf(fp,"<�����⾶ Dimater=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fArea);
	SimplifiedNumString(sText);
	fprintf(fp,"<���߼������� Area=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fUnitWeight);
	SimplifiedNumString(sText);
	fprintf(fp,"<���ߵ�λ������ UnitWeight=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fForcePermit);
	SimplifiedNumString(sText);
	fprintf(fp,"<���߶������ ForcePermit=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fSafeCoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<���߰�ȫϵ�� SafeCoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fMaxUseTension);
	SimplifiedNumString(sText);
	fprintf(fp,"<�������ʹ������ MaxUseTension=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fElasticCoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<���ߵ���ϵ�� ElasticCoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wireInfo.wire.m_fExpansionCoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<�����¶�����ϵ�� ExpansionCoef=\"%s\"/>\n",sText);
	fprintf(fp,"<�������� specWireType=\"%s\"/>\n",wireInfo.wire.specWireType);
	fprintf(fp,"<���߹���ͺ� specWire=\"%s\"/>\n",wireInfo.wire.specWire);
	fprintf(fp,"</��������Ϣ>\n");
	fprintf(fp,"<��������Ϣ LoadNum=\"%d\">\n",hashLoad.GetNodeNum());
	for(CWorkEnvLoad *pLoad=hashLoad.GetFirst();pLoad;pLoad=hashLoad.GetNext())
	{
		fprintf(fp,"<�����µļ����� key=\"%d\" LoadName=\"%s\">\n",hashLoad.GetCursorKey(),pLoad->name);
		sprintf(sText,"%f",pLoad->m_fLoadX);
		SimplifiedNumString(sText);
		fprintf(fp,"<������������ܺ� LoadX=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fLoadY);
		SimplifiedNumString(sText);
		fprintf(fp,"<������������ܺ� LoadY=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fLoadZ);
		SimplifiedNumString(sText);
		fprintf(fp,"<��ֱ�����ܺ� LoadZ=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fMinVertSpaceLoadX);
		SimplifiedNumString(sText);
		fprintf(fp,"<��С��ֱ�����������ܺ� MinVertSpaceLoadX=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fMinVertSpaceLoadY);
		SimplifiedNumString(sText);
		fprintf(fp,"<��С��ֱ������������ܺ� MinVertSpaceLoadY=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fMinVertSpaceLoadZ);
		SimplifiedNumString(sText);
		fprintf(fp,"<��С��ֱ���ഹֱ�����ܺ� MinVertSpaceLoadZ=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fWireLoadX);
		SimplifiedNumString(sText);
		fprintf(fp,"<���ߵķ���� WireLoadX=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fWireLoadZ);
		SimplifiedNumString(sText);
		fprintf(fp,"<���ߵĴ�ֱ���� WireLoadZ=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fMinVertSpaceWireLoadZ);
		SimplifiedNumString(sText);
		fprintf(fp,"<��С��ֱ����ʱ���ߵĴ�ֱ���� MinVertSpaceWireLoadZ=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fAttachmentLoadX);
		SimplifiedNumString(sText);
		fprintf(fp,"<����������� AttachmentLoadX=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fAttachmentLoadZ);
		SimplifiedNumString(sText);
		fprintf(fp,"<��������ֱ���� AttachmentLoadZ=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->m_fSetupGa);
		SimplifiedNumString(sText);
		fprintf(fp,"<��װʱ����ĸ��Ӵ�ֱ���� SetupGa=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pLoad->huchui);
		SimplifiedNumString(sText);
		fprintf(fp,"<���� huchui=\"%s\"/>\n",sText);
		fprintf(fp,"</�����µļ�����>\n");
	}
	fprintf(fp,"</��������Ϣ>\n");
	fprintf(fp,"</�������ؼ���ģ����Ϣ>\n");
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
	//�жϵ����ߵĿ���������������ʼ������״̬����
void CWireLoadModel::InitDianXianEquation(WEATHER &weather,
										  double gama_g,double gama_ice_w,double gama_max_w,double gama_ice)
{
#if defined(__LDS_)||defined(__TSA_)
	long initT;
	double init_length,length;
	double init_sigema,init_gama,gama;

	//1���жϿ��ƹ���
	//��������
	init_sigema=1e6*wireInfo.wire.m_fMaxUseTension/wireInfo.wire.m_fArea;
	init_gama=sqrt(pow(gama_g+gama_ice,2)+gama_ice_w*gama_ice_w);
	initT=weather.m_nMaxIceT;
	m_pResolver=CXuanLianResolver::CreateResolver();
	m_pResolver->InitXuanLian(1e6*wireInfo.wire.m_fElasticCoef,
		wireInfo.wire.m_fExpansionCoef,init_sigema,init_gama,
		weather.m_nMaxIceT,wireInfo.xianlu.represent_space,
		wireInfo.xianlu.HtoL,wireInfo.m_cFormula);
	init_length=m_pResolver->XuanLianLength();
	//���¹���
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
	//�������ޱ�����
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
	//2����ʼ��������������
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
	//���ԭ������
	for(pCommonLoad=hashLoad.GetFirst();pCommonLoad;pCommonLoad=hashLoad.GetNext())
	{
		pCommonLoad->m_fLoadX=0;				//�����ṩ������������ܺ�(N)
		pCommonLoad->m_fLoadY=0;				//�����ṩ��������ܺ�(N)
		pCommonLoad->m_fLoadZ=0;				//�����ṩ��ֱ�����ܺ�(N)
		pCommonLoad->m_fMinVertSpaceLoadX=0;	//�����ṩ�������(��С��ֱ����)�ܺ�(N)
		pCommonLoad->m_fMinVertSpaceLoadY=0;	//�����ṩ�������(��С��ֱ����)�ܺ�(N)
		pCommonLoad->m_fMinVertSpaceLoadZ=0;	//�����ṩ��ֱ����(��С��ֱ����)�ܺ�(N)
		pCommonLoad->m_fWireLoadX=0;			//���ߵķ����(N)
		pCommonLoad->m_fWireLoadZ=0;			//���ߵĴ�ֱ����(N)
		pCommonLoad->m_fMinVertSpaceWireLoadZ=0;//��С��ֱ����ʱ���ߵĴ�ֱ����(N)
		pCommonLoad->m_fAttachmentLoadX=0;		//�����������(������Ӱ��)(N)
		pCommonLoad->m_fAttachmentLoadZ=0;		//��������ֱ����(������Ӱ��)(N)
		pCommonLoad->m_fSetupGa=0;				//��װʱ����ĸ��Ӵ�ֱ����(N)
	}
	//T0��ʱ����ƽ�⵼���ߵ�ˮƽ����ֵ(N)
	double T0,W0,alfa,muSC,betaC,iceB;
	double gama,gama_w;
	double gama_g,gama_ice_w,gama_max_w,gama_ice;//���͹�������
	double xian_dia=wireInfo.wire.m_fDiameter;	//mm
	double xian_area=wireInfo.wire.m_fArea;		//mm2
	double vert_coef=wireInfo.xianlu.vertical_space_allotcoef;
	double hori_coef=wireInfo.xianlu.horizon_space_allotcoef;
	double max_tension=wireInfo.wire.m_fMaxUseTension;	//N
	double muZ=Find_MuZ(wireInfo.land_high,cLandscape);	//��ѹ�߶ȱ仯ϵ����һ��Ӧ���ߵ����Ե�Ӵ��ټ�2/3����
	//�������ر���
	gama_g=9800*wireInfo.wire.m_fUnitWeight/xian_area;
	//���㸲������
	gama_ice=8820*Pi*weather.m_fMaxIceThick*(weather.m_fMaxIceThick+xian_dia)/xian_area;
	//���㸲�������
	if(wireInfo.wire.m_fDiameter<17||weather.m_fMaxIceThick>0)
		muSC=1.2;
	else
		muSC=1.1;
	alfa=FindAlfa(weather.m_fMaxIceWindV);
	betaC=FindBetaC(weather.m_fMaxIceWindV,wireInfo.m_nVoltGrade);
	iceB=FindIceB1(weather.m_fMaxIceThick);
	W0=weather.m_fMaxIceWindV*weather.m_fMaxIceWindV/1600;
	gama_ice_w=1e6*alfa*W0*muZ*betaC*muSC*iceB*(xian_dia+weather.m_fMaxIceThick*2)/xian_area;
	//��繤�������
	if(wireInfo.wire.m_fDiameter<17)
		muSC=1.2;
	else
		muSC=1.1;
	alfa=FindAlfa(weather.m_fMaxWindV);
	betaC=FindBetaC(weather.m_fMaxWindV,wireInfo.m_nVoltGrade);
	W0=weather.m_fMaxWindV*weather.m_fMaxWindV/1600;
	gama_max_w=1e6*alfa*W0*muZ*betaC*muSC*xian_dia/xian_area;

	if(wireInfo.m_bNaiZhang)	//����
	{
		if(wireInfo.xianlu.represent_space<=0)	//����ֱ����δ�ṩ������
		{
			AfxMessageBox("�����಻��Ϊ0��");
			return;
		}
		InitDianXianEquation(weather,gama_g,gama_ice_w,gama_max_w,gama_ice);
		//1 ���¹���
		CWorkEnvLoad *pLowestTempLoad=hashLoad.GetValue(WORKENVLOAD_LOWTEMP);
		if(pLowestTempLoad==NULL)
		{
			AfxMessageBox("ȱ���������ĵ����������޷���������������أ�");
			return;
		}
		//TODO: �����m_pResolver==NULL���쳣��� wjh-2012.12.23
		m_pResolver->XuanLianByCursorPara(gama_g,weather.m_nLowestT);
		pLowestTempLoad->huchui=m_pResolver->XuanLianPosO();
			//��ֱ����
		pLowestTempLoad->m_fWireLoadZ=wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.vertical_space*vert_coef*wireInfo.wire.m_fUnitWeight*0.0098;
		pLowestTempLoad->m_fAttachmentLoadZ=wireInfo.m_fAttachmentWeight*9.8;
		pLowestTempLoad->m_fMinVertSpaceWireLoadZ=wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.min_vert_space*vert_coef*wireInfo.wire.m_fUnitWeight*0.0098;
			//�������
		pLowestTempLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
			//�������
		pLowestTempLoad->m_fLoadX=0;
		//2 90�ȴ�����
		CWorkEnvLoad *pWind90Load=hashLoad.GetValue(WORKENVLOAD_WIND);
		gama=sqrt(gama_max_w*gama_max_w+gama_g*gama_g);
		m_pResolver->XuanLianByCursorPara(gama,weather.m_nMaxWindT);
		pWind90Load->huchui=m_pResolver->XuanLianPosO();
			//��ֱ����
		pWind90Load->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ;
		pWind90Load->m_fAttachmentLoadZ=pLowestTempLoad->m_fAttachmentLoadZ;
			//�������
		pWind90Load->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
			//�������
		pWind90Load->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_max_w*xian_area*1e-6;
		pWind90Load->m_fAttachmentLoadX=wireInfo.m_fAttachmentLength*wireInfo.m_fAttachmentDiameter*muZ*weather.m_fMaxWindV*weather.m_fMaxWindV/1600000;
		//3 ������������
		CWorkEnvLoad *pIceLoad=hashLoad.GetValue(WORKENVLOAD_ICE);
		gama=sqrt(gama_ice_w*gama_ice_w+pow(gama_g+gama_ice,2));
		m_pResolver->XuanLianByCursorPara(gama,weather.m_nMaxIceT);
		pIceLoad->huchui=m_pResolver->XuanLianPosO();
			//��ֱ����
		pIceLoad->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ+
			wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.vertical_space*vert_coef*gama_ice*xian_area*1e-6;
		pIceLoad->m_fAttachmentLoadZ=pLowestTempLoad->m_fAttachmentLoadZ+
			8820*Pi*weather.m_fMaxIceThick*(weather.m_fMaxIceThick+wireInfo.m_fAttachmentDiameter)*wireInfo.m_fAttachmentLength*1e-9;
			//�������
		pIceLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
			//�������
		pIceLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_ice_w*xian_area*1e-6;
		pIceLoad->m_fAttachmentLoadX=weather.m_fMaxIceWindV*weather.m_fMaxIceWindV*muZ*
			(wireInfo.m_fAttachmentDiameter+weather.m_fMaxIceThick*2)*wireInfo.m_fAttachmentLength/1600000;
		//4 �ѱ���������(-5��C,10m/s)
		CWorkEnvLoad *pMoreIceLoad=hashLoad.GetValue(WORKENVLOAD_ICEMORE);
		alfa=FindAlfa(10);
		betaC=FindBetaC(10,wireInfo.m_nVoltGrade);
		iceB=FindIceB1(weather.m_fMaxIceThick);
		W0=0.0625;//10*10/1600
		gama_w=1e6*alfa*W0*muZ*betaC*muSC*iceB*(xian_dia+weather.m_fMaxIceThick*2)/xian_area;
		gama=sqrt(gama_w*gama_w+pow(gama_g+gama_ice,2));
		m_pResolver->XuanLianByCursorPara(gama,-5);
			//��ֱ����
		pMoreIceLoad->m_fWireLoadZ=pIceLoad->m_fWireLoadZ;
		pMoreIceLoad->m_fAttachmentLoadZ=pIceLoad->m_fAttachmentLoadZ;
			//�������
		pMoreIceLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
			//�������
		pMoreIceLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_w*xian_area*1e-6;
		pMoreIceLoad->m_fAttachmentLoadX=W0*muZ*
			(wireInfo.m_fAttachmentDiameter+weather.m_fMaxIceThick*2)*wireInfo.m_fAttachmentLength/1e6;
		//5 �ѱ������ٱ�(-5��C,10m/s)
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
			//��ֱ����
		pLessIceLoad->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ+
			wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.vertical_space*vert_coef*0.75*gama_ice*xian_area*1e-6;
		pLessIceLoad->m_fAttachmentLoadZ=pMoreIceLoad->m_fAttachmentLoadZ;
			//�������
		if(wireInfo.m_cXianType=='C')	//����
			pLessIceLoad->m_fLoadY=pMoreIceLoad->m_fLoadY-wireInfo.wire.m_fMaxUseTension*0.3;
		else //if(wireInfo.m_cXianType=='E')	//����
			pLessIceLoad->m_fLoadY=pMoreIceLoad->m_fLoadY-wireInfo.wire.m_fMaxUseTension*0.4;
			//�������
		pLessIceLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_w*xian_area*1e-6;
		pLessIceLoad->m_fAttachmentLoadX=pMoreIceLoad->m_fAttachmentLoadX;
		//6 ���߹������������(�б����޷硢-5��C)
		CWorkEnvLoad *pDuanXianLoad=hashLoad.GetValue(WORKENVLOAD_DUANXIANRUNNING);
			//��ֱ����
		pDuanXianLoad->m_fWireLoadZ=pIceLoad->m_fWireLoadZ;
		pDuanXianLoad->m_fAttachmentLoadZ=pIceLoad->m_fAttachmentLoadZ;
			//�������
		if(wireInfo.m_cXianType=='C')
		{	//����
			if(wireInfo.m_nDaoXianFengLieNum<=1)
				pDuanXianLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*max_tension;
			else
				pDuanXianLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*max_tension*0.7;
		}
		else//����
			pDuanXianLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*max_tension;
			//�������
		pDuanXianLoad->m_fLoadX=0;
		//7 ���߹����¹������
		pCommonLoad=hashLoad.GetValue(WORKENVLOAD_DUANXIANBREAK);
		//��ֱ����(��ֱ���ذ�5h���ϵ��߼���)
		double alloc_coef=min(wireInfo.land_high*5/(wireInfo.xianlu.vertical_space*vert_coef),1);
		pCommonLoad->m_fWireLoadZ=pIceLoad->m_fWireLoadZ*alloc_coef;
			//�������
		pCommonLoad->m_fLoadY=0;
			//�������
		pIceLoad->m_fLoadX=0;
		//8 ��װ�ѹ���
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
		//��ֱ����
		pSetupGuaXianLoad->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ;
			//�������
		pSetupGuaXianLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
			//�������
		pSetupGuaXianLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_w*xian_area*1e-6;
		pSetupGuaXianLoad->m_fAttachmentLoadX=W0*muZ*wireInfo.m_fAttachmentDiameter*wireInfo.m_fAttachmentLength/1e6;
		//9 ��װ�������ѹ�
		CWorkEnvLoad* pJinXianLoad=hashLoad.GetValue(WORKENVLOAD_SETUPJINXIAN_OTHER_FINISH);
		pJinXianLoad->huchui=pSetupGuaXianLoad->huchui;
			//��ֱ����
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
			//�������
		pJinXianLoad->m_fLoadY=1.1*pSetupGuaXianLoad->m_fLoadY*(1-cos(tracktion_angle));
		if(wireInfo.m_cXianType=='E')
			pJinXianLoad->m_fLoadY*=1.07625;//1.05*1.025;	��ǣ��ϵ����ʩ�����ϵ��
		else
			pJinXianLoad->m_fLoadY*=1.101875;//1.075*1.025;
			//�������
		pJinXianLoad->m_fWireLoadX=pSetupGuaXianLoad->m_fWireLoadX;
		//10��װ������δ�ң���ǰ������ʱ���ߣ�
		pCommonLoad=hashLoad.GetValue(WORKENVLOAD_SETUPJINXIAN_OTHER_EMPTY);
		pCommonLoad->huchui=pSetupGuaXianLoad->huchui;
		T0=pJinXianLoad->m_fLoadY;	//��ʱ���ߵ�������ʹ�ᵣ�ع�����ԭλ
			//��ֱ����
		pCommonLoad->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ+T0*tan(anchor_angle)+1.1*pSetupGuaXianLoad->m_fLoadY*sin(tracktion_angle);
		pCommonLoad->m_fSetupGa=pJinXianLoad->m_fSetupGa;
			//�������
		pCommonLoad->m_fLoadY=0;
			//�������
		pCommonLoad->m_fWireLoadX=pSetupGuaXianLoad->m_fWireLoadX;
		//11 ��ê��
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
			//��ֱ����
		pCommonLoad->m_fWireLoadZ=pSetupGuaXianLoad->m_fWireLoadZ+T0*tan(anchor_angle);
		pCommonLoad->m_fAttachmentLoadZ=pSetupGuaXianLoad->m_fAttachmentLoadZ;
			//�������
		pCommonLoad->m_fLoadY=fabs(pSetupGuaXianLoad->m_fLoadY-T0);
			//�������
		pCommonLoad->m_fWireLoadX=pSetupGuaXianLoad->m_fWireLoadX;
		pCommonLoad->m_fAttachmentLoadX=pJinXianLoad->m_fAttachmentLoadX;
		//12����ê��(��װ������δ��)
		pCommonLoad=hashLoad.GetValue(WORKENVLOAD_SETUPANCHORING);
		pCommonLoad->huchui=pSetupGuaXianLoad->huchui;
			//��ֱ����
		pCommonLoad->m_fWireLoadZ=pSetupGuaXianLoad->m_fWireLoadZ+T0*tan(anchor_angle);
		pCommonLoad->m_fAttachmentLoadZ=pSetupGuaXianLoad->m_fAttachmentLoadZ;
		pCommonLoad->m_fSetupGa=pJinXianLoad->m_fSetupGa;
			//�������
		pCommonLoad->m_fLoadY=fabs(1.1*pSetupGuaXianLoad->m_fLoadY-T0);
			//�������
		pCommonLoad->m_fWireLoadX=pSetupGuaXianLoad->m_fWireLoadX;
		pCommonLoad->m_fAttachmentLoadX=pJinXianLoad->m_fAttachmentLoadX;
		//15 �������㣨�������ص�30%��,���������ṹ��Ƽ����涨��DL/T 5154-2002 P16��
		CWorkEnvLoad *pWindEQLoad=hashLoad.GetValue(WORKENVLOAD_EARTHQUAKE);
		gama=sqrt(0.3*gama_max_w*gama_max_w+gama_g*gama_g);
		m_pResolver->XuanLianByCursorPara(gama,weather.m_nMaxWindT);
		pWindEQLoad->huchui=m_pResolver->XuanLianPosO();
			//��ֱ����
		pWindEQLoad->m_fWireLoadZ=pLowestTempLoad->m_fWireLoadZ;
		pWindEQLoad->m_fAttachmentLoadZ=pLowestTempLoad->m_fAttachmentLoadZ;
			//�������
		pWindEQLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*m_pResolver->XuanLianHorizonStress()*xian_area*1e-6;
			//������أ����ǵ���������ʱ��������Ҫ��0.5����ϵ�����˴�����ذ�60%��
		pWindEQLoad->m_fWireLoadX=0.6*pWind90Load->m_fWireLoadX;
		pWindEQLoad->m_fAttachmentLoadX=0.6*pWind90Load->m_fAttachmentLoadX;
	}
	else
	{
		InitDianXianEquation(weather,gama_g,gama_ice_w,gama_max_w,gama_ice);
		//1 90�ȴ�����
		CWorkEnvLoad *pWind90Load=hashLoad.GetValue(WORKENVLOAD_WIND);
		//XuanLianByCursorPara(gama,weather.m_nMaxWindT);
		//pWind90Load->huchui=XuanLianPosO();
			//��ֱ����
		pWind90Load->m_fWireLoadZ=wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.vertical_space*vert_coef*wireInfo.wire.m_fUnitWeight*0.0098;
		pWind90Load->m_fAttachmentLoadZ=wireInfo.m_fAttachmentWeight*9.8;
		pWind90Load->m_fMinVertSpaceWireLoadZ=wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.min_vert_space*vert_coef*wireInfo.wire.m_fUnitWeight*0.0098;
			//�������
		pWind90Load->m_fLoadY=0;
			//�������
		pWind90Load->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_max_w*xian_area*1e-6;
		pWind90Load->m_fAttachmentLoadX=wireInfo.m_fAttachmentLength*wireInfo.m_fAttachmentDiameter*muZ*weather.m_fMaxWindV*weather.m_fMaxWindV/1600000;
		//2 ������������
		CWorkEnvLoad *pIceLoad=hashLoad.GetValue(WORKENVLOAD_ICE);
		//XuanLianByCursorPara(gama,weather.m_nMaxIceT);
		//pIceLoad->huchui=XuanLianPosO();
			//��ֱ����
		pIceLoad->m_fWireLoadZ=pWind90Load->m_fWireLoadZ+
			wireInfo.m_nDaoXianFengLieNum*wireInfo.xianlu.vertical_space*vert_coef*gama_ice*xian_area*1e-6;
		pIceLoad->m_fAttachmentLoadZ=pWind90Load->m_fAttachmentLoadZ+
			8820*Pi*weather.m_fMaxIceThick*(weather.m_fMaxIceThick+wireInfo.m_fAttachmentDiameter)*wireInfo.m_fAttachmentLength*1e-9;
			//�������
		pIceLoad->m_fLoadY=0;
			//�������
		pIceLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_ice_w*xian_area*1e-6;
		pIceLoad->m_fAttachmentLoadX=weather.m_fMaxIceWindV*weather.m_fMaxIceWindV*muZ*
			(wireInfo.m_fAttachmentDiameter+weather.m_fMaxIceThick*2)*wireInfo.m_fAttachmentLength/1600000;
		//3 �����ȸ���(-5��C,10m/s)
		CWorkEnvLoad *pDiffIceLoad=hashLoad.GetValue(WORKENVLOAD_ICEDIFF);
		alfa=FindAlfa(10);
		betaC=FindBetaC(10,wireInfo.m_nVoltGrade);
		iceB=FindIceB1(weather.m_fMaxIceThick);
		W0=0.0625;//10*10/1600
		gama_w=1e6*alfa*W0*muZ*betaC*muSC*iceB*(xian_dia+weather.m_fMaxIceThick*2)/xian_area;
			//��ֱ����
		pDiffIceLoad->m_fWireLoadZ=0.875*pIceLoad->m_fWireLoadZ+0.125*pWind90Load->m_fWireLoadZ;
		pDiffIceLoad->m_fAttachmentLoadZ=pIceLoad->m_fAttachmentLoadZ;
			//�������
		if(wireInfo.m_cXianType=='E')
			pDiffIceLoad->m_fLoadY=0.2*max_tension;
		else
			pDiffIceLoad->m_fLoadY=0.1*max_tension;
			//�������
		pDiffIceLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_w*xian_area*1e-6;
		pDiffIceLoad->m_fAttachmentLoadX=W0*muZ*
			(wireInfo.m_fAttachmentDiameter+weather.m_fMaxIceThick*2)*wireInfo.m_fAttachmentLength/1e6;
		//4 ���߹����¹������(�б����޷硢-5��C)
		CWorkEnvLoad *pDuanXianLoad=hashLoad.GetValue(WORKENVLOAD_DUANXIANRUNNING);
			//��ֱ����
		pDuanXianLoad->m_fWireLoadZ=pIceLoad->m_fWireLoadZ;
		pDuanXianLoad->m_fAttachmentLoadZ=pIceLoad->m_fAttachmentLoadZ;
			//�������
		if(wireInfo.m_cXianType=='C')
		{	//����
			if(cLineRoadType<2)	//ƽԭ������
			{
				if(wireInfo.m_nDaoXianFengLieNum<=1)
					pDuanXianLoad->m_fLoadY=0.5*wireInfo.m_nDaoXianFengLieNum*max_tension;
				else if(wireInfo.m_nDaoXianFengLieNum<=2)	//˫����
					pDuanXianLoad->m_fLoadY=0.25*wireInfo.m_nDaoXianFengLieNum*max_tension;
				else
					pDuanXianLoad->m_fLoadY=0.20*wireInfo.m_nDaoXianFengLieNum*max_tension;
			}
			else				//ɽ����·
			{
				if(wireInfo.m_nDaoXianFengLieNum<=1)
					pDuanXianLoad->m_fLoadY=0.5*wireInfo.m_nDaoXianFengLieNum*max_tension;
				else if(wireInfo.m_nDaoXianFengLieNum<=2)	//˫����
					pDuanXianLoad->m_fLoadY=0.30*wireInfo.m_nDaoXianFengLieNum*max_tension;
				else
					pDuanXianLoad->m_fLoadY=0.25*wireInfo.m_nDaoXianFengLieNum*max_tension;
			}
		}
		else//����
			pDuanXianLoad->m_fLoadY=wireInfo.m_nDaoXianFengLieNum*max_tension;
			//�������
		pDuanXianLoad->m_fLoadX=0;
		//5 ���߹������������
		pCommonLoad=hashLoad.GetValue(WORKENVLOAD_DUANXIANBREAK);
		//��ֱ����(��ֱ���ذ�4h���ϵ��߼���)
		pCommonLoad->m_fWireLoadZ=pIceLoad->m_fWireLoadZ;
			//�������
		pCommonLoad->m_fLoadY=0;
			//�������
		pIceLoad->m_fLoadX=0;
		//6 ��װ�ѵ���
		CWorkEnvLoad *pSuspendLoad=hashLoad.GetValue(WORKENVLOAD_SETUPSUSPEND);
		if(xian_dia<17)
			muSC=1.2;
		else
			muSC=1.1;
		alfa=FindAlfa(weather.m_fSetupWindV);
		betaC=FindBetaC(weather.m_fSetupWindV,wireInfo.m_nVoltGrade);
		W0=weather.m_fSetupWindV*weather.m_fSetupWindV/1600;
		gama_w=1e6*alfa*W0*muZ*betaC*muSC*xian_dia/xian_area;
		//��ֱ����
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
			//�������
		pSuspendLoad->m_fLoadY=0;
			//�������
		pSuspendLoad->m_fWireLoadX=wireInfo.m_nDaoXianFengLieNum*
			wireInfo.xianlu.horizon_space*hori_coef*gama_w*xian_area*1e-6;
		pSuspendLoad->m_fAttachmentLoadX=W0*muZ*wireInfo.m_fAttachmentDiameter*wireInfo.m_fAttachmentLength/1e6;
		//7 ��װ������
		CWorkEnvLoad *pSuspendingLoad=hashLoad.GetValue(WORKENVLOAD_SETUPSUSPENDING);
			//��ֱ����
		double multiply_coef=2.0;
		pSuspendingLoad->m_fWireLoadZ=pWind90Load->m_fWireLoadZ*multiply_coef*1.1;
		pSuspendingLoad->m_fAttachmentLoadZ=wireInfo.m_fAttachmentWeight*9.8;
		pSuspendingLoad->m_fSetupGa=pSuspendLoad->m_fSetupGa;
			//�������
		pSuspendingLoad->m_fLoadY=0;
			//�������
		pSuspendingLoad->m_fWireLoadX=pSuspendLoad->m_fWireLoadX;
		pSuspendingLoad->m_fAttachmentLoadX=pSuspendLoad->m_fAttachmentLoadX;
		//8 ��ê��
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
			T*=1.07625;//1.05*1.025;	��ǣ��ϵ����ʩ�����ϵ��
		else
			T*=1.101875;//1.075*1.025;
			//��ֱ����
		pAnchorLoad->m_fWireLoadZ=pSuspendLoad->m_fWireLoadZ+T*sin(anchor_angle);
		pAnchorLoad->m_fAttachmentLoadZ=pSuspendLoad->m_fAttachmentLoadZ;
		pAnchorLoad->m_fSetupGa=pSuspendLoad->m_fSetupGa;
			//�������
		pAnchorLoad->m_fLoadY=T*(1-cos(anchor_angle));
			//�������
		pAnchorLoad->m_fWireLoadX=pSuspendLoad->m_fWireLoadX;
		pAnchorLoad->m_fAttachmentLoadX=pSuspendLoad->m_fAttachmentLoadX;
		//9 ��ê��
		pCommonLoad=hashLoad.GetValue(WORKENVLOAD_SETUPANCHORING);
		pCommonLoad->huchui=pAnchorLoad->huchui;
			//��ֱ����
		pCommonLoad->m_fWireLoadZ=pSuspendLoad->m_fWireLoadZ+1.1*T*sin(anchor_angle);
		pCommonLoad->m_fAttachmentLoadZ=pAnchorLoad->m_fAttachmentLoadZ;
		pCommonLoad->m_fSetupGa=pSuspendingLoad->m_fSetupGa;

			//�������
		pCommonLoad->m_fLoadY=1.1*pAnchorLoad->m_fLoadY;
			//�������
		pCommonLoad->m_fWireLoadX=pAnchorLoad->m_fWireLoadX;
		pCommonLoad->m_fAttachmentLoadX=pAnchorLoad->m_fAttachmentLoadX;
		//15 �������㣨�������ص�30%��,���������ṹ��Ƽ����涨��DL/T 5154-2002 P16��
		CWorkEnvLoad *pWindEQLoad=hashLoad.GetValue(WORKENVLOAD_EARTHQUAKE);
		if(pWindEQLoad)
		{	//��ֱ����
			pWindEQLoad->m_fWireLoadZ=pWind90Load->m_fWireLoadZ;
			pWindEQLoad->m_fAttachmentLoadZ=pWind90Load->m_fAttachmentLoadZ;
			//�������
			pWindEQLoad->m_fLoadY=0;
			//������أ����ǵ���������ʱ��������Ҫ��0.5����ϵ�����˴�����ذ�60%��
			pWindEQLoad->m_fWireLoadX=0.6*pWind90Load->m_fWireLoadX;
			pWindEQLoad->m_fAttachmentLoadX=0.6*pWind90Load->m_fAttachmentLoadX;
		}
	}
	for(pCommonLoad=hashLoad.GetFirst();pCommonLoad;pCommonLoad=hashLoad.GetNext())
		pCommonLoad->SumCal();
#endif
}
#endif