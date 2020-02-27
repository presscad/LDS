#include "stdafx.h"
#include "WireLayoutsEvolvement.h"

////////////////////////////////////////////////////////////////////
CWireLayout::CWireLayout()
{
	iNo=1;
	strcpy(name,"���÷���");
	m_sRelaTemplateName.Copy("");	//TODO:������һ���ؼ���ģ����
	m_fSafetyCoef=1.0;		//�ṹ��Ҫ��ϵ��
	m_fGamaG=1.2;			//����ط���ϵ��
	m_fGamaQ=1.4;			//�ɱ���ط���ϵ��
	m_fLiveLoadFai=0.9;		//��������ϵ��
	m_fCheckLoadFai=0.75;	//����������ϵ��
	m_fLiveLoadZ=1000;		//�������(N)
}
void CWireLayout::Empty()
{
	hashWireLoadModel.Empty();
	groupWorkStatus.hashNodeRoom.Empty();
	groupWorkStatus.hashWorkStatus.Empty();
}
//��LDS V1.2.1.0(��)֮ǰ�汾���ͻ�ʹ�������٣���һ�㲻���ҹ�˾�ṩ�ĺ��ؼ���ģ�飬���ⲿ����Ϣ�ݲ�����Ǩ�ơ�wjh-2014.5.2
void CWireLayout::EvolveToNewVersion()
{
	m_fSafetyCoef=groupWorkStatus.m_fSafetyCoef;	//�ṹ��Ҫ��ϵ��
	m_fGamaG=groupWorkStatus.m_fGamaG;		//����ط���ϵ��
	m_fGamaQ=groupWorkStatus.m_fGamaQ;		//�ɱ���ط���ϵ��
	m_fLiveLoadFai=groupWorkStatus.m_fLiveLoadFai;	//��������ϵ��
	m_fCheckLoadFai=groupWorkStatus.m_fCheckLoadFai;	//����������ϵ��
	m_fLiveLoadZ=groupWorkStatus.m_fLiveLoadZ;	//�������(N)
	hashWirePlace.Empty();
	hashWireNode.Empty();
	for(LOADNODEROOM_STRU* pRoom=groupWorkStatus.hashNodeRoom.GetFirst();pRoom;pRoom=groupWorkStatus.hashNodeRoom.GetNext())
	{
		WIREPLACE_STRU* pWirePlace=hashWirePlace.Add(groupWorkStatus.hashNodeRoom.GetCursorKey());
		pWirePlace->idPlace=hashWirePlace.GetCursorKey();
		pWirePlace->m_iRawWireLoadNo=pRoom->m_iRawWireLoadNo;		//ԭʼ�������ر�ʶ��
		strcpy(pWirePlace->helpString,pRoom->helpString);
		pWirePlace->hashWorkEnv.Empty();
		for(long* pCaseI=pRoom->hashWorkEnv.GetFirst();pCaseI;pCaseI=pRoom->hashWorkEnv.GetNext())	//��ֵ���ǹ�����ʶ�ţ�Ԫ��ֵ����Ӧ�����¶�Ӧ��ԭʼ��������CRawWireLoad��WorkEnv�ı�ʶ��
			pWirePlace->hashWorkEnv.SetValue(pRoom->hashWorkEnv.GetCursorKey(),*pCaseI);
		for(WIRENODE_STRU* pWireNode=pRoom->hashNode.GetFirst();pWireNode;pWireNode=pRoom->hashNode.GetNext())
		{
			WIRENODE_STRU* pNewWireNode=hashWireNode.Add(pRoom->hashNode.GetCursorKey());
			pNewWireNode->hNode=pWireNode->hNode;;
			pNewWireNode->fLoadAllocCoefX=pWireNode->fLoadAllocCoefX;
			pNewWireNode->fLoadAllocCoefY=pWireNode->fLoadAllocCoefY;
			pNewWireNode->fLoadAllocCoefZ=pWireNode->fLoadAllocCoefZ;
			//TODO: ���pNewWireNode->hashWorkEnv=pWireNode->hashWorkEnv�ĸ�ֵ
		}
	}
	for(RAWWORKSTATUS_STRU *pRawCase=groupWorkStatus.hashWorkStatus.GetFirst();pRawCase;pRawCase=groupWorkStatus.hashWorkStatus.GetNext())
	{	//SRCCASE_STRUͬRAWWORKSTATUS_STRU
		SRCCASE_STRU* pSrcCase=hashSrcCase.Add(groupWorkStatus.hashWorkStatus.GetCursorKey());
		strcpy(pSrcCase->name,pRawCase->name);//[MAX_TEMP_CHAR_100+1];
		//TODO: δ����pRawCase->lim_str[MAX_TEMP_CHAR_50+1]����
		pSrcCase->m_cHoriEarthQuakeAxis=pRawCase->m_cHoriEarthQuakeAxis;	//ˮƽ��������ʱ���񶯷���ǡ㣬'X'=0��'Y'=90
		strcpy(pSrcCase->vibration_mode_str,pRawCase->vibration_mode_str);	//[MAX_TEMP_CHAR_50+1];	//���ͷֽⷴӦ�׷�����ʱ��Ҫ���ǵ������б�
		//TODO: δ����pRawCase->m_hWorkStatus;		//��Ӧ��CWorkStatus�ľ��ֵ
		pSrcCase->m_cMainLoadType=pRawCase->m_cMainLoadType;	//0:���1:����2:����3:����4:��װ5:�����ȸ���6:ˮƽ����7:�������8:��ˮƽ����9:���������10:����
		pSrcCase->m_bCheckWorkCond=pRawCase->m_bCheckWorkCond;	//���㹤��
		pSrcCase->m_fIceThick=pRawCase->ice_thick;	//�����������(mm)
		pSrcCase->m_bMinVertSpace=pRawCase->m_bMinVertSpace;	//��С��ֱ���๤��
		pSrcCase->WindLoad=pRawCase->WindLoad;	//�����
		//TODO: δ����pRaseCase->id
	}
	hashNodeLoad;	//��ǰ���߷�����ָ����λ��ԭʼ�����µĺ���ֵ����ֵΪ:��λ��,ԭʼ������
}
void CWireLayout::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	int i,n;
	if((doc_type==3&&version>=1070101)||(doc_type==4&&version>=1000101))
	{
		buffer.ReadInteger(&iNo);
		buffer.ReadString(name,49);
	}
	else
	{	//���ļ���һ��Ĭ��ֵ
		strcpy(name,"���÷���");
		iNo=1;//pNoGroup->EnumIdleNo();
	}
	//ԭʼ�����������
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		int iNo=0;
		buffer.ReadInteger(&iNo);
		CWireLoadModel *pWireModel=hashWireLoadModel.Add(iNo);
		pWireModel->iNo=iNo;
		pWireModel->FromBuffer(buffer,version,doc_type);
	}
	//���������Ϣ
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		long key=0;
		buffer.ReadInteger(&key);
		RAWWORKSTATUS_STRU* pRawStatus=groupWorkStatus.hashWorkStatus.Add(key);
		buffer.ReadString(pRawStatus->name);
		if((doc_type==3&&version>=1070303)||(doc_type==4&&version>=1000303))
		{
			buffer.ReadByte(&pRawStatus->m_cHoriEarthQuakeAxis);
			if((doc_type==3&&version<=1070307)||(doc_type==4&&version<=1000307))
			{
				if(pRawStatus->m_cHoriEarthQuakeAxis=='X')
					pRawStatus->m_cHoriEarthQuakeAxis=0;
				else //if(pRawStatus->m_cHoriEarthQuakeAxis=='Y')
					pRawStatus->m_cHoriEarthQuakeAxis=90;
			}
			buffer.ReadString(pRawStatus->vibration_mode_str);
		}
		buffer.ReadString(pRawStatus->lim_str);	//�¼�
		buffer.ReadInteger(&pRawStatus->m_hWorkStatus);			//��Ӧ��CWorkStatus�ľ��ֵ
		buffer.ReadByte(&pRawStatus->m_cMainLoadType);			//0:���1:����2:����3:����4:��װ5:�����ȸ���6:ˮƽ����7:�������8:��ˮƽ����9:���������10:����
		buffer.ReadBooleanThin(&pRawStatus->m_bCheckWorkCond);	//���㹤��
		buffer.ReadBooleanThin(&pRawStatus->m_bMinVertSpace);	//��С��ֱ���๤��
		buffer.ReadDouble(&pRawStatus->ice_thick);				//�����������(mm)
		buffer.ReadInteger(&pRawStatus->WindLoad.azimuth);		//�����,�����(��)
		buffer.ReadDouble(&pRawStatus->WindLoad.velocity);		//�����,����(m/s)
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		long key=0;
		buffer.ReadInteger(&key);
		LOADNODEROOM_STRU* pRoom=groupWorkStatus.hashNodeRoom.Add(key);
		if((doc_type==3&&version>=1070101)||(doc_type==4&&version>=1000101))
			buffer.ReadInteger(&key);
		pRoom->m_iRawWireLoadNo=key;
		buffer.ReadString(pRoom->helpString);
		//д����λ��Ӧ��Ĭ��ԭʼ������ʶ
		long j,k,n2,n3,key2;
		buffer.ReadInteger(&n2);
		for(j=0;j<n2;j++)
		{
			buffer.ReadInteger(&key);
			buffer.ReadInteger(&key2);
			pRoom->hashWorkEnv.SetValue(key,key2);
		}
		//д�뵱ǰ���λ��Ӧ�ĵ����Ϣ
		buffer.ReadInteger(&n2);
		for(j=0;j<n2;j++)
		{
			buffer.ReadInteger(&key);
			WIRENODE_STRU *pRawNode=pRoom->hashNode.Add(key);
			buffer.ReadDouble(&pRawNode->fLoadAllocCoefX);
			if((doc_type==3&&version>=1070005)||(doc_type==4&&version>=1000016))
			{
				buffer.ReadDouble(&pRawNode->fLoadAllocCoefY);
				buffer.ReadDouble(&pRawNode->fLoadAllocCoefZ);
				buffer.ReadInteger(&pRawNode->hNode);
			}
			else
				pRawNode->fLoadAllocCoefY=pRawNode->fLoadAllocCoefZ=pRawNode->fLoadAllocCoefX;
			buffer.ReadInteger(&n3);
			for(k=0;k<n3;k++)
			{
				buffer.ReadInteger(&key);
				buffer.ReadInteger(&key2);
				pRawNode->hashWorkEnv.SetValue(key,key2);
			}
		}
	}
}
void CWireLayout::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{	//TODO: ����ɴ���
	buffer.WriteString(name);	//��������
	buffer.WriteString(m_sRelaTemplateName);
	//TODO: ����Ӧ��m_fLiveLoadZ����CRawWireLoad��
	buffer.WriteDouble(groupWorkStatus.m_fLiveLoadZ);	//�������(N)
	buffer.WriteInteger(hashWireLoadModel.GetNodeNum());		//ԭʼ������������,hashWireLoadModelͬhashRawWireLoad
	for(CWireLoadModel *pRawWire=hashWireLoadModel.GetFirst();pRawWire;pRawWire=hashWireLoadModel.GetNext())
	{	//CWireLoadModelͬCRawWireLoad
		buffer.WriteInteger(pRawWire->iNo);
		pRawWire->ToBuffer(buffer);
	}
	buffer.WriteInteger(groupWorkStatus.hashWorkStatus.GetNodeNum());			//���߷�����ԭʼ��Ϲ���
	for(SRCCASE_STRU *pSrcCase=hashSrcCase.GetFirst();pSrcCase;pSrcCase=hashSrcCase.GetNext())
	{
		buffer.WriteInteger(pSrcCase->id);
		pSrcCase->ToBuffer(buffer);
	}
	buffer.WriteInteger(hashWirePlace.GetNodeNum());			//���߷����й���λ
	//hashWirePlaceͬhashNodeRoom��LOADNODEROOM_STRUͬWIREPLACE_STRU
	for(WIREPLACE_STRU *pWirePlace=hashWirePlace.GetFirst();pWirePlace;pWirePlace=hashWirePlace.GetNext())
	{
		buffer.WriteInteger(pWirePlace->idPlace);
		pWirePlace->ToBuffer(buffer);
	}
	buffer.WriteInteger(hashWireNode.GetNodeNum());			//���߷����й��߽ڵ�
	for(WIRENODE_STRU *pWireNode=hashWireNode.GetFirst();pWireNode;pWireNode=hashWireNode.GetNext())
	{
		buffer.WriteInteger(pWireNode->idNodeI);
		buffer.WriteInteger(pWireNode->idPlace);
		buffer.WriteDouble(pWireNode->fLoadAllocCoefX);
		buffer.WriteDouble(pWireNode->fLoadAllocCoefY);
		buffer.WriteDouble(pWireNode->fLoadAllocCoefZ);
		//pWireNode->hashWorkEnvת��hashNodeLoad
	}
	buffer.WriteInteger(hashNodeLoad.GetNodeNum());			//���߷�������������λ��ĳ�����µ���ϵ��ԭʼ����
	for(CExternalNodeLoad *pNodeLoad=hashNodeLoad.GetFirst();pNodeLoad;pNodeLoad=hashNodeLoad.GetNext())
	{
		HASH2KEY key=hashNodeLoad.GetCursorKey();
		buffer.WriteInteger(key.key1);	//idPlace
		buffer.WriteInteger(key.key2);	//idCase
		buffer.WritePoint(pNodeLoad->Fxyz);
		buffer.WriteDouble(pNodeLoad->permanentFz);
	}
}
////////////////////////////////////////////////////////////////////
int CWorkStatusGroupInfo::EnumIdleRawStatusNo()
{
	int maxNo=0;
	for(RAWWORKSTATUS_STRU *pRawStatus=hashWorkStatus.GetFirst();pRawStatus;pRawStatus=hashWorkStatus.GetNext())
		maxNo=max(maxNo,(int)hashWorkStatus.GetCursorKey());
	return ++maxNo;
}
CWorkStatusGroupInfo::CWorkStatusGroupInfo()
{
	m_fSafetyCoef=1.0;		//�ṹ��Ҫ��ϵ��
	m_fGamaG=1.2;			//����ط���ϵ��
	m_fGamaQ=1.4;			//�ɱ���ط���ϵ��
	m_fLiveLoadFai=0.9;		//��������ϵ��
	m_fCheckLoadFai=0.75;	//����������ϵ��
	m_fLiveLoadZ=1000;		//�������(N)
}
////////////////////////////////////////////////////////////////////
WIRENODE_STRU::WIRENODE_STRU()
{
	idNodeI=idPlace=hNode=0;	//��ýڵ��������ʵ��ģ���нڵ�ľ��ֵ
	fLoadAllocCoefX=fLoadAllocCoefY=fLoadAllocCoefZ=1.0;
}
void WIREPLACE_STRU::FromBuffer(CBuffer &buffer,long version/*=0*/)
{
	buffer.ReadInteger(&m_iRawWireLoadNo);		//ԭʼ�������ر�ʶ��
	buffer.ReadString(helpString,100);
	long i,n,idCase,idWorkEnv;
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&idCase);
		buffer.ReadInteger(&idWorkEnv);
		hashWorkEnv.SetValue(idCase,idWorkEnv);
	}
}
void WIREPLACE_STRU::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(m_iRawWireLoadNo);		//ԭʼ�������ر�ʶ��
	buffer.WriteString(helpString);
	//��ֵ���ǹ�����ʶ�ţ�Ԫ��ֵ����Ӧ�����¶�Ӧ��ԭʼ��������CRawWireLoad��WorkEnv�ı�ʶ��
	buffer.WriteInteger(hashWorkEnv.GetNodeNum());
	for(long* pWorkEnvId=hashWorkEnv.GetFirst();pWorkEnvId;pWorkEnvId=hashWorkEnv.GetNext())
	{
		buffer.WriteInteger(hashWorkEnv.GetCursorKey());
		buffer.WriteInteger(*pWorkEnvId);
	}
}
//////////////////////////////////////////////////////////////////
double SRCCASE_STRU::Kesai()
{	//�ɱ�������ϵ��
	//if(sysPara==NULL||sysPara->GetExecuteLoadCode()==3)
	//	return Fai;
	//else
	{
		if(m_cMainLoadType<=2)	//������������
			return 1.0;
		else if(m_cMainLoadType>=5)	//���𹤿�
			return 1.0;
		else if(m_bCheckWorkCond)	//���㹤��
			return 0.75;
		else if(m_cMainLoadType==3||m_cMainLoadType==4)	//���߻�װ�������ȸ���
			return 0.9;
		else
			return 1.0;
	}
}
double SRCCASE_STRU::GamaQ()
{	//�ۺϿɱ���ط���ϵ��
	//if(sysPara!=NULL&&sysPara->GetExecuteLoadCode()==3)
	//	return GamaQi;
	//else
	if(m_cMainLoadType<5)
		return 1.4;
	else
		return 0.5;
}
void SRCCASE_STRU::FromBuffer(CBuffer &buffer,long version/*=0*/)
{
	buffer.ReadString(name,100);		//���ڹ����ļ�Ҫ����
	buffer.ReadString(vibration_mode_str,50);	//���ͷֽⷴӦ�׷�����ʱ��Ҫ���ǵ������б�
	buffer.ReadByte(&m_cHoriEarthQuakeAxis);//ˮƽ��������ʱ���񶯷���ǡ㣬'X'=0��'Y'=90
	buffer.ReadByte(&m_cMainLoadType);		//0:���1:����2:����3:���߼������ȸ���4:��װ5:ˮƽ����6:�������7:��ˮƽ����8:���������
	buffer.ReadBooleanThin(&m_bCheckWorkCond);		//���㹤��
	buffer.ReadDouble(&m_fIceThick);	//�����������(mm)
	buffer.ReadBooleanThin(&m_bMinVertSpace);
	buffer.ReadInteger(&WindLoad.azimuth);
	buffer.ReadDouble(&WindLoad.velocity);	//�����
}
void SRCCASE_STRU::ToBuffer(CBuffer &buffer)
{
	buffer.WriteString(name);		//���ڹ����ļ�Ҫ����
	buffer.WriteString(vibration_mode_str);	//���ͷֽⷴӦ�׷�����ʱ��Ҫ���ǵ������б�
	buffer.WriteByte(m_cHoriEarthQuakeAxis);//ˮƽ��������ʱ���񶯷���ǡ㣬'X'=0��'Y'=90
	buffer.WriteByte(m_cMainLoadType);		//0:���1:����2:����3:���߼������ȸ���4:��װ5:ˮƽ����6:�������7:��ˮƽ����8:���������
	buffer.WriteBooleanThin(m_bCheckWorkCond);		//���㹤��
	buffer.WriteDouble(m_fIceThick);	//�����������(mm)
	buffer.WriteBooleanThin(m_bMinVertSpace);
	buffer.WriteInteger(WindLoad.azimuth);
	buffer.WriteDouble(WindLoad.velocity);	//�����
}
////////////////////////////////////////////////////////////////////
CWireLayoutsEvolvement evolve;
CWireLayoutsEvolvement::CWireLayoutsEvolvement(void)
{
}


CWireLayoutsEvolvement::~CWireLayoutsEvolvement(void)
{
}
void CWireLayoutsEvolvement::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if((doc_type==3&&version>=1070004)||(doc_type==4&&version>=1000015))
	{
		int nWireLayout=1;
		if((doc_type==3&&version>=1070101)||(doc_type==4&&version>=1000101))
			buffer.ReadInteger(&nWireLayout);
		evolve.WireLayouts.Empty();
		for(int iLayout=0;iLayout<nWireLayout;iLayout++)
		{
			CWireLayout* pWireLayout=evolve.WireLayouts.append();
			pWireLayout->FromBuffer(buffer,version,doc_type);
			if((doc_type==3&&version<1070101)||(doc_type==4&&version<1000101))
			{
				for(CWireLoadModel* pWireModel=pWireLayout->hashWireLoadModel.GetFirst();pWireModel;pWireModel=pWireLayout->hashWireLoadModel.GetNext())
					pWireModel->weather.CopyWeather(&weather);
			}
		}
	}
}
void CWireLayoutsEvolvement::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(version==0||((doc_type==3&&version>=2000101)||(doc_type==4&&version>=1020101)))
		return;	//��LDS V1.2.1.1��ʼ���ع��߷�����Ϣ������CAttachBufferר�û����У�����WireLoad.exe���̹���
	buffer.WriteInteger(evolve.WireLayouts.GetNodeNum());
	for(CWireLayout *pWireLayout=evolve.WireLayouts.GetFirst();pWireLayout;pWireLayout=evolve.WireLayouts.GetNext())
	{
		buffer.WriteInteger(pWireLayout->iNo);
		buffer.WriteString(pWireLayout->name);
		//ԭʼ�����������
		buffer.WriteInteger(pWireLayout->hashWireLoadModel.GetNodeNum());
		for(CWireLoadModel *pWireModel=pWireLayout->hashWireLoadModel.GetFirst();pWireModel;pWireModel=pWireLayout->hashWireLoadModel.GetNext())
		{
			buffer.WriteInteger(pWireModel->iNo);
			pWireModel->ToBuffer(buffer);
		}
		//���������Ϣ
		buffer.WriteInteger(pWireLayout->groupWorkStatus.hashWorkStatus.GetNodeNum());
		for(RAWWORKSTATUS_STRU* pRawStatus=pWireLayout->groupWorkStatus.hashWorkStatus.GetFirst();pRawStatus;pRawStatus=pWireLayout->groupWorkStatus.hashWorkStatus.GetNext())
		{
			buffer.WriteInteger(pWireLayout->groupWorkStatus.hashWorkStatus.GetCursorKey());
			buffer.WriteString(pRawStatus->name);
			buffer.WriteByte(pRawStatus->m_cHoriEarthQuakeAxis);
			buffer.WriteString(pRawStatus->vibration_mode_str);
			buffer.WriteString(pRawStatus->lim_str);	//�¼�
			buffer.WriteInteger(pRawStatus->m_hWorkStatus);			//��Ӧ��CWorkStatus�ľ��ֵ
			buffer.WriteByte(pRawStatus->m_cMainLoadType);			//0:���1:����2:����3:����4:��װ5:�����ȸ���6:ˮƽ����7:�������8:��ˮƽ����9:���������10:����
			buffer.WriteBooleanThin(pRawStatus->m_bCheckWorkCond);	//���㹤��
			buffer.WriteBooleanThin(pRawStatus->m_bMinVertSpace);	//��С��ֱ���๤��
			buffer.WriteDouble(pRawStatus->ice_thick);				//�����������(mm)
			buffer.WriteInteger(pRawStatus->WindLoad.azimuth);		//�����,�����(��)
			buffer.WriteDouble(pRawStatus->WindLoad.velocity);		//�����,����(m/s)
		}
		buffer.WriteInteger(pWireLayout->groupWorkStatus.hashNodeRoom.GetNodeNum());
		for(LOADNODEROOM_STRU* pRoom=pWireLayout->groupWorkStatus.hashNodeRoom.GetFirst();pRoom;pRoom=pWireLayout->groupWorkStatus.hashNodeRoom.GetNext())
		{
			long *pWorkEnvKey;
			buffer.WriteInteger(pWireLayout->groupWorkStatus.hashNodeRoom.GetCursorKey());
			buffer.WriteInteger(pRoom->m_iRawWireLoadNo);
			buffer.WriteString(pRoom->helpString);
			//д����λ��Ӧ��Ĭ��ԭʼ������ʶ
			buffer.WriteInteger(pRoom->hashWorkEnv.GetNodeNum());
			for(pWorkEnvKey=pRoom->hashWorkEnv.GetFirst();pWorkEnvKey;pWorkEnvKey=pRoom->hashWorkEnv.GetNext())
			{
				buffer.WriteInteger(pRoom->hashWorkEnv.GetCursorKey());	//ԭʼ������ʶ��
				buffer.WriteInteger(*pWorkEnvKey);						//��Ӧԭʼ�����µ�ԭʼ����������ʶ
			}
			//д�뵱ǰ���λ��Ӧ�ĵ����Ϣ
			buffer.WriteInteger(pRoom->hashNode.GetNodeNum());
			for(WIRENODE_STRU *pRawNode=pRoom->hashNode.GetFirst();pRawNode;pRawNode=pRoom->hashNode.GetNext())
			{
				buffer.WriteInteger(pRoom->hashNode.GetCursorKey());
				buffer.WriteDouble(pRawNode->fLoadAllocCoefX);
				buffer.WriteDouble(pRawNode->fLoadAllocCoefY);
				buffer.WriteDouble(pRawNode->fLoadAllocCoefZ);
				buffer.WriteInteger(pRawNode->hNode);
				buffer.WriteInteger(pRawNode->hashWorkEnv.GetNodeNum());
				for(pWorkEnvKey=pRawNode->hashWorkEnv.GetFirst();pWorkEnvKey;pWorkEnvKey=pRawNode->hashWorkEnv.GetNext())
				{
					buffer.WriteInteger(pRawNode->hashWorkEnv.GetCursorKey());	//ԭʼ������ʶ��
					buffer.WriteInteger(*pWorkEnvKey);							//��Ӧԭʼ�����µ�ԭʼ����������ʶ
				}
			}
		}
	}
}

bool CWireLayoutsEvolvement::EvolveToWireLoadBuffer()//CTower* pTower)
{
	/*CAttachBuffer *pWireLoadBuffer=pTower->AttachBuffer.GetValue(CTower::WIRELOAD_MODEL);
	int item_n=0;
	if(pWireLoadBuffer==NULL)
		pWireLoadBuffer=pTower->AttachBuffer.Add(CTower::WIRELOAD_MODEL);
	pWireLoadBuffer->ClearContents();
	CBuffer buffer(1000000);	//1Mb
	buffer.WriteInteger(0);		//�������ػ������ݳ���
	DWORD base_address=buffer.GetCursorPosition();	//�԰汾�����ݿ�ʼ��λ���ڵ�ǰ�������е���ʼ��ַ
	buffer.WriteInteger(1);	//�汾��
	DWORD sect_index_pos=buffer.GetCursorPosition();
	buffer.Write(NULL,20);	//���ظ����ݷ����԰汾����ʼλ�ÿ�ʼ�����ƫ�Ƶ�ַ
	//1.������Ϣ��
	DWORD sect_start_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_index_pos);
	buffer.WriteDword(sect_start_pos-base_address);
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(0);							//�洢�ռ��С
	DWORD current_pos=buffer.GetCursorPosition();
	buffer.WriteAt(current_pos,pTower->m_sTaType,50);		//��������
	CXhChar50 prjCode;
	prjCode.Copy(pTower->m_sPrjCode.GetBuffer());
	buffer.WriteAt(current_pos+ 50,(char*)prjCode,50);	//���̴���
	buffer.WriteAt(current_pos+100,pTower->m_sPrjName,50);	//��������
	buffer.WriteAt(current_pos+150,pTower->m_sOperator,50);	//�����Ա
	buffer.WriteAt(current_pos+200,pTower->weatherCode,16);	//��������
	buffer.WriteWord((WORD)pTower->volt_grade);					//��ѹ�ȼ�
	buffer.WriteByte(pTower->m_cLandscape);					//��λ��ò
	buffer.WriteByte(pTower->m_cLineRoadType);				//��·״��
	buffer.WriteDouble(pTower->m_fIceDensity);				//�������
	buffer.WriteDouble(pTower->m_fGamaGForDesign);			//�־ú��ط���ϵ��
	buffer.WriteWord(pTower->m_siSafetyGrade);				//��ȫ�ȼ�
	current_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(current_pos-sect_start_pos-4);//��д�Ĵ洢�ռ��С
	buffer.SeekPosition(current_pos);
	//2.������Ϣ��
	sect_start_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_index_pos+4);
	buffer.WriteDword(sect_start_pos-base_address);
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(0);							//�洢�ռ��С
	buffer.WriteDouble(pTower->weather.m_fMaxIceThick);		//��󸲱����
	buffer.WriteDouble(pTower->weather.m_fMaxIceThickCheck);//���㸲�����
	buffer.WriteDouble(pTower->weather.m_fMaxIceWindV);		//��󸲱�����
	buffer.WriteDouble(pTower->weather.m_fMaxWindV);		//����ٶ�
	buffer.WriteDouble(pTower->weather.m_fMaxWindVCheck);	//�������
	buffer.WriteDouble(pTower->weather.m_fSetupWindV);		//��װ����
	buffer.WriteInteger(pTower->weather.m_nAverageT);		//ƽ���¶�
	buffer.WriteInteger(pTower->weather.m_nHighestT);		//����¶�
	buffer.WriteInteger(pTower->weather.m_nLowestT);		//����¶�
	buffer.WriteInteger(pTower->weather.m_nMaxIceT);		//��󸲱��¶�
	buffer.WriteInteger(pTower->weather.m_nMaxWindT);		//�������¶�
	buffer.WriteInteger(pTower->weather.m_nSetupT);			//��װ�¶�
	current_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(current_pos-sect_start_pos-4);//��д�Ĵ洢�ռ��С
	buffer.SeekPosition(current_pos);
	//3.����������
	sect_start_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_index_pos+8);
	buffer.WriteDword(sect_start_pos-base_address);
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(0);							//д�빤�����ݿ鳤��
	//д�빤��������
	buffer.WriteInteger(pTower->WorkStatus.GetNodeNum());	//����״������ֵΪCWorkCase::iNo
	CWorkStatus *pCase;
	for(pCase=pTower->WorkStatus.GetFirst();pCase;pCase=pTower->WorkStatus.GetNext())
	{
		buffer.WriteInteger(pCase->handle);
		DWORD case_sec_start_pos=buffer.GetCursorPosition();
		buffer.WriteInteger(0);						//���������С
		pCase->ToRawBuffer(buffer);
		//
		current_pos=buffer.GetCursorPosition();
		buffer.SeekPosition(case_sec_start_pos);
		buffer.WriteDword(current_pos-case_sec_start_pos-4);//��д�Ĺ��������С
		buffer.SeekPosition(current_pos);
	}
	current_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(current_pos-sect_start_pos-4);//��д�Ĵ洢�ռ��С
	buffer.SeekPosition(current_pos);
	//4.���߷�����
	sect_start_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_index_pos+12);
	buffer.WriteDword(sect_start_pos-base_address);
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(0);							//д����߷������ݿ鳤��
	buffer.WriteInteger(evolve.WireLayouts.GetNodeNum());
	for(CWireLayout *pLayout=evolve.WireLayouts.GetFirst();pLayout;pLayout=evolve.WireLayouts.GetNext())
	{
		buffer.WriteInteger(pLayout->iNo);
		pLayout->ToBuffer(buffer);
	}
	current_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(current_pos-sect_start_pos-4);//��д�Ĵ洢�ռ��С
	buffer.SeekPosition(current_pos);
	//5.�ڵ������
	sect_start_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_index_pos+16);
	buffer.WriteDword(sect_start_pos-base_address);
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(0);							//д��ڵ�������ݿ鳤��
	DWORD tempos=buffer.GetCursorPosition();
	buffer.WriteInteger(0);//hashActualNodes.GetNodeNum());
	int load_nodes=0;
	buffer.WriteInteger(pTower->WorkStatus.GetNodeNum());
	//for(WIRENODE_STRU* pWireNode=hashActualNodes.GetFirst();pWireNode;pWireNode=hashActualNodes.GetNext())
	for(CLDSNode* pLoadNode=pTower->Node.GetFirst();pLoadNode;pLoadNode=pTower->Node.GetNext())
	{
		if(pLoadNode->m_idNodePlace<=0)
			continue;//TODO: ɸѡ�����ؽڵ�
		load_nodes++;
		buffer.WriteInteger(pLoadNode->point_i);
		buffer.WriteInteger(pLoadNode->m_idNodePlace);	//TODO: δ���ǵ�һ�������
		//TODO: �ҵ�ĺ��ط�������ϵ������ҲҪ���ص��������� wjh-2014.4.12
		for(pCase=pTower->WorkStatus.GetFirst();pCase;pCase=pTower->WorkStatus.GetNext())
		{
			buffer.WriteInteger(pCase->handle);
			//NODE_LOAD load=GetActualNodeLoad(pWireNode->idNode,pCase->id,true);
			CExternalNodeLoad* pLoad=pCase->hashNodeLoad.GetValue(pLoadNode->handle);
			//TODO: pLoad->Fxyz��pLoad->permanentFz�˸���ϵ��
			buffer.WritePoint(pLoad->Fxyz);
			buffer.WriteDouble(pLoad->permanentFz);
		}
	}
	current_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(tempos);	//д��ʵ�ʻ��ܳ��ĺ��عҵ���Ŀ
	buffer.WriteInteger(load_nodes);
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(current_pos-sect_start_pos-4);//��д�Ĵ洢�ռ��С
	buffer.SeekPosition(current_pos);*/
	return true;
}
////////////////////////////
