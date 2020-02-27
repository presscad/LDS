#include "stdafx.h"
#include "Tower.h"
#include "BoltHoleInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//ÿ����ʾ��˨������ʱ����ʱ�����洢������˨��ӳ���ϵ��ϣ�� wjh-2019.8.5
CHashList<QUEUENODE_HOLE> hashBoltHoles;

BOOL BOLTHOLE::IsPropEqual(long idProp)
{
	if(hashBoltHoles.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	QUEUENODE_HOLE *pHole=NULL;
	QUEUENODE_HOLE *pFirstHole = hashBoltHoles.GetFirst();
	pFirstHole->xQueueNode.GetPropValueStr(idProp,valueStr);
	pHole=pFirstHole;
	while(pHole!=NULL)
	{
		if(pHole!=pFirstHole)
		{
			char valueStr2[500]="";
			pHole->xQueueNode.GetPropValueStr(idProp,valueStr2);
			if(stricmp(valueStr,valueStr2)!=0)
				return FALSE;
		}
		if((pHole=pHole->pNext)==NULL)
			pHole=hashBoltHoles.GetNext();
	}
	return TRUE;
}
int BOLTHOLE::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(CLDSBolt::GetPropID("dfHoleDiameter")==id)
		sText.Printf("%g",(this->pLsRef->pBolt->d+pLsRef->pBolt->hole_d_increment));
	else if(CLDSBolt::GetPropID("siWaistLength")==id)
		sText.Printf("%d",this->pLsRef->waistLen);
	else if(CLDSBolt::GetPropID("hHoleResidePart")==id)
		sText.Printf("0x%X",this->hResidePart);	//��˨�����ڹ����ľ��
	else if(CLDSBolt::GetPropID("ciHoleFuncType")==id)
	{	//��˨�׵Ĺ�����������
		CXhChar50 xarrWords[10];
		int count=DELIMITER_STR::ParseWordsFromStr("0.��˨��|1.�Ŷ���|2.���߿�|3.�ӵؿ�|4.װ���|5.���ƿ�|6.������|7.���׽Ŷ�|8.�ؽſ�",
			"|",xarrWords,10);
		if(pLsRef->pBolt->m_cFuncType<count)
			sText=xarrWords[pLsRef->pBolt->m_cFuncType];
	}
	else if(CLDSBolt::GetPropID("ciManuProcessType")==id)
	{
		if((pLsRef->cFlag&CLsRef::DRILL_HOLE)>0)
			sText.Copy("���");
		else //if((pLsRef->cFlag&CLsRef::PUNCH_HOLE)>0)
			sText.Copy("���");
	}
	else if(CLDSBolt::GetPropID("blEnlargeHole")==id)
	{
		if((pLsRef->cFlag&CLsRef::ENLARGE_HOLE)>0)
			sText.Copy("��");
		else
			sText.Copy("��");
	}
#ifdef AFX_TARG_ENU_ENGLISH
	if(strlen(sText)>=nMaxStrBufLen)
		logerr.Log("Bolt hole(0x%X@0x%X)'s property##d string value out of length",pLsRef->pBolt->handle,this->hResidePart,id);
#else
	if(strlen(sText)>=nMaxStrBufLen)
		logerr.Log("��˨��0x%X@0x%X����#%d�ַ���������������",pLsRef->pBolt->handle,this->hResidePart,id);
#endif
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen-1);
	return strlen(sText);
}
bool BOLTHOLE::ModifyPropValueStr(long id,const char* valueStr)
{
	if(id==CLDSBolt::GetPropID("dfHoleDiameter"))
		pLsRef->pBolt->hole_d_increment=atof(valueStr)-pLsRef->pBolt->d;
	else if(id==CLDSBolt::GetPropID("siWaistLength"))
		pLsRef->waistLen=atoi(valueStr);
	//else if(id==CLDSBolt::GetPropID("hHoleResidePart"))
	//	sText.Printf("%0x%X",this->hResidePart);	//��˨�����ڹ����ľ��
	else if(id==CLDSBolt::GetPropID("ciHoleFuncType"))
		pLsRef->pBolt->m_cFuncType=valueStr[0]-'0';
	else if(id==CLDSBolt::GetPropID("ciManuProcessType"))
	{
		pLsRef->cFlag&=0xfe;
		if(stricmp(valueStr,"���")==0)
			pLsRef->cFlag|=CLsRef::DRILL_HOLE;
		//else if(stricmp(valueStr,"���")==0)
		//	pLsRef->cFlag|=CLsRef::PUNCH_HOLE;
	}
	else if(CLDSBolt::GetPropID("blEnlargeHole")==0)
	{
		pLsRef->cFlag&=0xfd;
		if(stricmp(valueStr,"��")==0)
			pLsRef->cFlag|=CLsRef::ENLARGE_HOLE;
	}
	else
		return false;
	return true;
}
