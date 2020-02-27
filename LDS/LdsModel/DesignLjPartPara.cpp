//<LOCALE_TRANSLATE BY hxr /> 2015-1-10
// DesignLjPartPara.cpp: implementation of the DesignLjPartPara class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include "Tower.h"
#include "HeapMemory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IModel* CParaThick::m_pTower=NULL;
IModel* CDesignThickPara::m_pTower=NULL;
IHeapMemory* CParaThick::m_pExterHeapMemory=NULL;
bool CDesignThickPara::AddThick(int val, BOOL bHandle/*=FALSE*/,BOOL bSamePartFilter/*=FALSE*/)
{
	char ss[50];
	if(bHandle)
	{
		if(bSamePartFilter)
		{
			for(THICK_ITEM item=GetFirstItem();!item.IsFinalKey();item=GetNextItem())
			{
				if(item.IsHandle()&&item.val==val)
					return true;	//�˹����Ѿ������ں���ַ���֮��
			}
		}
		if(val>0)
			sprintf(ss,"0X%X",val);
		else
			sprintf(ss,"-0X%X",-val);
	}
	else
		sprintf(ss,"%d",val);
	int len=key_str.GetLength();
	if(len>0)
	{
		if(len+strlen(ss)>49)
		{
			ClearRedundant();	//���ܴ����������ݣ��������һ���������ݣ���Ч�ٱ���
			len=strlen(key_str);
		}
		if(len+strlen(ss)<=49)
		{
			key_str.Append(",");
			key_str.Append(ss);
		}
		else //�ڴ����
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Thickness parameter string overflows, system automatically truncates string!");
#else 
			logerr.Log("��Ȳ����ַ��������ϵͳ�Զ��ض��ַ�����");
#endif
			return false;
		}
	}
	else
		key_str.Copy(ss);
	return true;
}

bool CParaThick::AddThick(int val, BOOL bHandle/*=FALSE*/,BOOL bSamePartFilter/*=FALSE*/)
{
	for(int i=0;i< MAX_BUFF_LEN;i+=5)
	{
		THICK_ITEM item(data+i);
		if(bHandle&&bSamePartFilter&&item.IsHandle()&&item.val==val)
			return true;
		else if(item.cItemType==2&&!bHandle)
		{
			item.val+=val;
			memcpy(data+i+1,&item.val,4);
			return true;
		}
		else if(item.IsFinalKey())
		{
			*(data+i)=bHandle?1:2;	//���ֵ
			memcpy(data+i+1,&val,4);
			return true;
		}
	}
	return false;
}
void CParaThick::FromBuffStr(CBuffer &buffer)
{
	CXhChar500 keystr;
	buffer.ReadString(keystr,500);
	BuildFromStr(keystr);
}
void CParaThick::BuildFromStr(const char *str)
{
	m_pAllocData=NULL;
	m_wnAllocDataLength=0;

	ciIndex=0;
	memset(data,0,MaxBuffLen());
	if(str==NULL||strlen(str)==0)
		return;
	int nMaxItemCount = MaxBuffLen()/5 - 1;
	CXhChar500 key_str;
	key_str.Copy(str);
	CXhChar500 keystr=str;
	THICK_ITEM item;
	item.val=0;
	item.cItemType=0;
	char* key=strtok(keystr,", ");
	while(key)
	{
		if(key[0]=='0'||(strlen(key)>2&&key[0]=='-'&&key[1]=='0'))	//���
		{
			if(key[1]=='*')
			{
				key[1]='x';
				item.cItemType=3;
			}
			else if(key[2]=='*')
			{
				key[2]='x';
				item.cItemType=3;
			}
			else
				item.cItemType=1;
			sscanf(key,"%X",&item.val);
		}
		else	//ָ�����ֵ
		{
			item.val=atoi(key);
			item.cItemType=2;
		}
		if(item.val!=0)
		{
			if (ciIndex<nMaxItemCount)
				memcpy(&data[ciIndex*5],&item,5);
			ciIndex++;
		}
		key=strtok(NULL,", ");
	}
	if(ciIndex>=nMaxItemCount)
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"ͨ������Ŀ{%d>%d}���ޣ�������Ŀ������!",ciIndex,nMaxItemCount);
}
THICK_ITEM::THICK_ITEM(const char* data/*=NULL*/)
{
	if(data==NULL)
		val=cItemType=0;
	else{
		cItemType=*data;
		memcpy(&val,data+1,4);
	}
}
bool THICK_ITEM::IsFinalKey()
{
	return (cItemType<=0||cItemType>3);
}
bool THICK_ITEM::IsHandle()
{
	return cItemType==1||cItemType==3;
}
CDesignThickPara::CDesignThickPara(double dblThick)
{
	if(dblThick==0)
		key_str.Copy("0");
	else if(fabs(dblThick)<1e-6)
	{
		key_str.Printf("%E",dblThick);
		int str_len=key_str.GetLength();
		char* splitDot=strchr(key_str,'.');
		char* splitE=strchr(key_str,'E');
		for(char *valid=splitE-1;valid>=splitDot;valid--)
		{
			if(*valid!='0'&&*valid!='.')
				break;
			else
				*valid=0;
		}
		valid++;
		size_t sub_len=splitE-valid;
		if(sub_len>0)
		{
			int copy_len=strlen(splitE);
			memmove(valid,splitE,copy_len);
			key_str[str_len-sub_len]=0;
		}
		return;
	}
	key_str.Printf("%lf",dblThick);
	char* split=strchr(key_str,'.');
	if(split==NULL)
		return;
	int len=strlen(split);
	for(int i=len-1;i>0;i--)
	{
		if(split[i]=='0')
		{
			split[i]=0;
			if(i==1)	//��С������
				*split=0;
		}
		else
			break;
	}
}

void CDesignThickPara::ToBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	buffer.WriteString(key_str);
}

long CParaThick::get_hHeapMemory()
{
	return *((long*)datapool);
}
long CParaThick::set_hHeapMemory(long hMemory)
{
	return *((long*)datapool)=hMemory;
}
char* CParaThick::get_pData()
{
	if (m_wnAllocDataLength>0)
		return m_pAllocData;
	else
		return datapool;
}
void CParaThick::AttachExterData(char* pExterData,UINT uiDataLength)
{
	m_pAllocData=pExterData;
	m_wnAllocDataLength=(WORD)uiDataLength;
}
void CParaThick::CopyFromData(char* pExterData,UINT uiDataLength)
{
	memset(datapool,0,MAX_BUFF_LEN);
	if (uiDataLength<=MAX_BUFF_LEN)
	{
		m_pAllocData=NULL;
		m_wnAllocDataLength=0;
		memcpy(datapool,pExterData,uiDataLength);
	}
	else
	{	//�Զ������ڴ�
		m_pAllocData=pExterData;
		m_wnAllocDataLength=(WORD)uiDataLength;
	}
}
void CParaThick::DetachAllocData()
{
	m_pAllocData=NULL;
	m_wnAllocDataLength=0;
	memset(datapool,0,4);
}
UINT CParaThick::MaxBuffLen() const { 
	if (m_wnAllocDataLength>0)
		return m_wnAllocDataLength;
	else
		return MAX_BUFF_LEN;
}
CParaThick::CParaThick(double dblThick)
{
	m_pAllocData=NULL;
	m_wnAllocDataLength=0;
	ciIndex=0;
	double nThick=ftoi(dblThick);
	memset(datapool,0,MAX_BUFF_LEN);
	*datapool=2;
	memcpy(datapool+1,&nThick,4);
}
void CParaThick::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	fprintf(fp,"<������>\n");
	fprintf(fp,"<����ַ��� key_str=\"%s\"/>\n",(char*)ToThickStr());
	fprintf(fp,"</������>\n");
}

void CDesignThickPara::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	buffer.ReadString(key_str);
}

CXhChar500 CParaThick::ToThickStr(bool toLegacyMode/*=false*/) const
{
	CXhChar500 keystr;
	const char* pDataBuff=m_wnAllocDataLength==0?datapool:m_pAllocData;
	for(UINT i=0;i< MAX_BUFF_LEN;i+=5){
		THICK_ITEM item(&pDataBuff[i]);
		if(item.cItemType==0)
			break;
		else if(item.IsHandle())//cItemType==1)	//���ֵ
		{
			if(keystr.GetLength()>0)
				keystr.Append(',');
			if(item.val<0)
				keystr.Append('-');
			if(item.cItemType==THICK_ITEM::PART_THROUGHnT&&!toLegacyMode)
				keystr.Append(CXhChar16("0*%X",abs(item.val)));
			else
				keystr.Append(CXhChar16("0X%X",abs(item.val)));
		}
		else if(item.cItemType==2)
		{
			if(keystr.GetLength()>0)
				keystr.Append(',');
			keystr.Append(CXhChar16("%d",item.val));
		}
	}
	return keystr;
}

bool CParaThick::SetItem(int i,THICK_ITEM item)	//iȡֵ0~19,��ʾ���������������ڴ�data��������wjh-2015.4.10
{
	if(i<0||i>=MAX_ITEM_COUNT)
		return false;
	data[i*5]=item.cItemType;
	memcpy(&data[i*5+1],&item.val,4);
	return true;
}
void CParaThick::ToBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( version==0||
	   (version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
	   (version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
	{
		WORD i=0;
		for(i=0;i< MAX_BUFF_LEN;i+=5){
			if(data[i]==0)
				break;
		}
		if (version==0||(
			(doc_type==1&&version>=5020200)||	//TMA V5,2,2,0
			(doc_type==2&&version>=2010200)||	//LMA V2,1,2,0
			(doc_type==3&&version>=2010401)||	//TSA V2,1,4,1
			(doc_type==4&&version>=1031000)||	//LDS V1,3,10,0
			(doc_type==5&&version>=1031000)))	//TDA V1,3,10,0
			buffer.WriteWord(i);//��ȡ��Ч���泤��
		else
			buffer.WriteByte((BYTE)i);//��Ч���泤��
		buffer.Write(data,i);
	}
	else
	{
		CXhChar50 key_str=ToThickStr(true);
		buffer.WriteString(key_str);
	}
}
void CParaThick::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( version==0||
	   (version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
	   (version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
	{
		WORD wiDataLen=0;
		memset(datapool,0,MAX_BUFF_LEN);
		if (version==0||(
			(doc_type==1&&version>=5020200)||	//TMA V5,2,2,0
			(doc_type==2&&version>=2010200)||	//LMA V2,1,2,0
			(doc_type==3&&version>=2010401)||	//TSA V2,1,4,1
			(doc_type==4&&version>=1031000)||	//LDS V1,3,10,0
			(doc_type==5&&version>=1031000)))	//TDA V1,3,10,0
			buffer.ReadWord(&wiDataLen);//��ȡ��Ч���泤��
		else
			buffer.ReadByte((BYTE*)&wiDataLen);//��ȡ��Ч���泤��BYTE cbDataLen
		if (wiDataLen<=MAX_BUFF_LEN)
		{
			short siReadBytes=min(wiDataLen,MAX_BUFF_LEN);
			buffer.Read(datapool,siReadBytes);
			if (wiDataLen>siReadBytes)
				buffer.SeekOffset(wiDataLen-siReadBytes);
			this->m_pExterHeapMemory=NULL;
			this->m_wnAllocDataLength=0;
		}
		else
		{
			char stackbuf[500];	//����ջ�ڴ������Ŀ����Ϊ�˾�������new-delete�����ڴ���� wjh-2019.8.12
			CHAR_ARRAY databuf(wiDataLen,false,stackbuf,500);
			buffer.Read(databuf,wiDataLen);
			CopyFromData(databuf,wiDataLen);
		}
	}
	else
	{
		CXhChar500 key_str;
		buffer.ReadString(key_str,500);
		BuildFromStr(key_str);
	}
}

void CDesignThickPara::ClearRedundant(IModel* pTower/*=NULL*/)
{
	CDesignThickPara thick;
	if(pTower==NULL)
		pTower=m_pTower;
	for(THICK_ITEM item=GetFirstItem();item.val!=0;item=GetNextItem())
	{
		if(!item.IsHandle())
			thick.AddThick(item.val);
		else
		{
			CLDSPart *pPart=pTower->FromPartHandle(abs(item.val));
			if(pPart&&(pPart->GetClassTypeId()!=CLS_BOLT))
				thick.AddThick(item.val,TRUE);
		}
	}
	key_str.Copy(thick.key_str);
}
void CParaThick::ClearRedundant(IModel* pTower/*=NULL*/)
{
	CParaThick thick;
	if(pTower==NULL)
		pTower=m_pTower;
	for(THICK_ITEM item=GetFirstItem();item.val!=0;item=GetNextItem())
	{
		if(!item.IsHandle())
			thick.AddThick(item.val);
		else
		{
			CLDSPart *pPart=pTower->FromPartHandle(abs(item.val));
			if(pPart&&(pPart->GetClassTypeId()!=CLS_BOLT))
				thick.AddThick(item.val,TRUE);
		}
	}
	*this=thick;
}
void CParaThick::EmptyThick()
{
	memset(datapool,0,MAX_BUFF_LEN);
	if(m_wnAllocDataLength>0)
		memset(data,0,MaxBuffLen());
}
void CDesignThickPara::EmptyThick()
{
	memset(key_str,0,50);
}

long CDesignThickPara::Thick(IModel* pTower,bool bEnableModuleFilter/*=false*/)
{
	long thick=0;
	keystr.Copy(key_str);
	char* key=strtok(keystr,", ");
	if(pTower==NULL)
		pTower=m_pTower;
	CLDSModule* pModule=NULL;
	if(pTower&&pTower->IsTowerModel())
		pModule=((CTower*)pTower)->GetActiveModule();
	while(key)
	{
		int sign=1;
		if(key[0]=='-')
			sign=-1;
		if(pTower!=NULL && (key[0]=='0'||(strlen(key)>2&&key[0]=='-'&&key[1]=='0')))	//���
		{
			long h,t=0;
			h=HexStrToLong(key);
			CLDSPart *pPart=pTower->FromPartHandle(abs(h));
			if(bEnableModuleFilter&&pPart&&pModule&&!pModule->IsSonPart(pPart))
				pPart=NULL;	//�ǵ�ǰ���ߵ���Ч����
			else if(pPart)
			{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
				if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					CLDSParamPlate *pParamPlate = ((CLDSParamPlate*)pPart); 
					if(pParamPlate->m_iParamType==TYPE_UEND)
						t = ftol(pParamPlate->GetThick())*2;
					else 
						t = ftol(pParamPlate->GetThick());	
				}
				else if(pPart->IsArcPart()||pPart->IsLinePart()||pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
#endif
					t=ftol(pPart->GetThick());
			}
			thick+=t*sign;
		}
		else	//ָ�����ֵ
			thick+=atoi(key);
		key=strtok(NULL,",");
	}
	return thick;
}
CDesignThickPara CDesignThickPara::MirThickPara(IModel* pModel,MIRMSG msg)
{
	CDesignThickPara mirthick;
	strcpy(mirthick.key_str,"");
	for(THICK_ITEM item=GetFirstItem();item.val!=0;item=GetNextItem())
	{
		if(item.IsHandle())
		{
			int sign=1;
			if(item.val<0)
				sign=-1;
			item.val*=sign;
			CLDSPart *pPart=pModel->FromPartHandle(item.val);
			if(pPart==NULL)
				continue;
			f3dPoint pickPos;
			CLDSPart *pMirPart=pPart->GetMirPart(msg,pickPos);
			if(pMirPart)
				mirthick.AddThick(sign*pMirPart->handle,TRUE);
			else
				mirthick.AddThick(item.val,item.IsHandle());
		}
		else	//ֱ����Ӻ��
			mirthick.AddThick(item.val);
	}
	return mirthick;
}
long CParaThick::Thick(IModel* pTower,bool bEnableModuleFilter/*=false*/,bool* pHasPriorPartThick/*=NULL*/)
{
	long thick=0;
	if(pTower==NULL)
		pTower=m_pTower;
	CLDSModule* pModule=NULL;
	if(pTower&&pTower->IsTowerModel())
		pModule=((CTower*)pTower)->GetActiveModule();
	int i=0;
	CLDSPart* pPriorThickPart=NULL;
	bool hasPriorPartThick=false;	//�Ƿ���в��ܺ������ƵĹ���ͨ����
	do
	{
		THICK_ITEM item(data+i);
		if(!hasPriorPartThick&&item.cItemType==3&&(pPriorThickPart=pTower->FromPartHandle(abs(item.val)))!=NULL)
			hasPriorPartThick=true;
		i+=5;
	}while(i< MAX_BUFF_LEN &&!hasPriorPartThick);
	if(pHasPriorPartThick)
		*pHasPriorPartThick=hasPriorPartThick;
	for(i=0;i< MAX_BUFF_LEN;i+=5)
	{
		THICK_ITEM item(data+i);
		int sign=1;
		if(item.val<0)
			sign=-1;
		if(pTower!=NULL && item.IsHandle())	//���
		{
			long t=0;
			CLDSPart *pPart=pTower->FromPartHandle(abs(item.val));
			if(hasPriorPartThick&&pPriorThickPart&&item.cItemType!=THICK_ITEM::PART_THROUGHnT&&!pPriorThickPart->ModuleTogetherWith(pPart))
				continue;
			else if(bEnableModuleFilter&&pPart&&pModule&&!pModule->IsSonPart(pPart)&&
				item.cItemType==THICK_ITEM::PART_THROUGH)
				pPart=NULL;	//�ǵ�ǰ���ߵ���Ч����
			else if(pPart)
			{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
				if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					CLDSParamPlate *pParamPlate = ((CLDSParamPlate*)pPart); 
					if(pParamPlate->m_iParamType==TYPE_UEND)
						t = ftol(pParamPlate->GetThick())*2;
					else 
						t = ftol(pParamPlate->GetThick());	
				}
				else if(pPart->IsArcPart()||pPart->IsLinePart()||pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
#endif
					t=ftol(pPart->GetThick());
			}
			thick+=t*sign;
		}
		else	//ָ�����ֵ
			thick+=item.val;
	}
	return thick;
}
CParaThick CParaThick::MirThickPara(IModel* pModel,MIRMSG msg)
{
	CParaThick mirthick;
	//strcpy(mirthick.key_str,"");
	for(THICK_ITEM item=GetFirstItem();item.val!=0;item=GetNextItem())
	{
		if(item.IsHandle())
		{
			int sign=1;
			if(item.val<0)
				sign=-1;
			item.val*=sign;
			CLDSPart *pPart=pModel->FromPartHandle(item.val);
			if(pPart==NULL)
				continue;
			f3dPoint pickPos;
			CLDSPart *pMirPart=pPart->GetMirPart(msg,pickPos);
			if(pMirPart)
				mirthick.AddThick(sign*pMirPart->handle,TRUE);
			else
				mirthick.AddThick(item.val,item.IsHandle());
		}
		else	//ֱ����Ӻ��
			mirthick.AddThick(item.val);
	}
	return mirthick;
}
THICK_ITEM CDesignThickPara::GetFirstItem()
{
	THICK_ITEM item;
	item.val=0;
	item.cItemType=0;
	keystr.Copy(key_str);
	char* key=strtok(keystr,", ");
	if(key)
	{
		int sign=1;
		if(key[0]=='-')
			sign=-1;
		if(key[0]=='0'||(strlen(key)>2&&key[0]=='-'&&key[1]=='0'))	//���
		{
			sscanf(key,"%X",&item.val);
			item.cItemType=1;
			return item;
		}
		else	//ָ�����ֵ
		{
			item.val=atoi(key);
			item.cItemType=2;
			return item;
		}
	}
	return item;
}
THICK_ITEM CParaThick::GetFirstItem()
{
	ciIndex=0;
	return THICK_ITEM(data);
}
THICK_ITEM CDesignThickPara::GetNextItem()
{
	THICK_ITEM item;
	item.val=0;
	item.cItemType=0;
	keystr.Copy(key_str);
	char* key=strtok(NULL,", ");
	if(key)
	{
		int sign=1;
		if(key[0]=='-')
			sign=-1;
		if(key[0]=='0'||(strlen(key)>2&&key[0]=='-'&&key[1]=='0'))	//���
		{
			sscanf(key,"%X",&item.val);
			item.cItemType=1;
			return item;
		}
		else	//ָ�����ֵ
		{
			item.val=atoi(key);
			item.cItemType=2;
			return item;
		}
	}
	return item;
}
THICK_ITEM CParaThick::GetNextItem()
{
	ciIndex+=1;
	int siOffset=((int)ciIndex)*5;
	if(siOffset< MAX_BUFF_LEN)
		return THICK_ITEM(data+siOffset);
	else if(ciIndex==MAX_ITEM_COUNT&&m_pAllocData!=NULL)
	{
		if ((siOffset=(ciIndex-MAX_ITEM_COUNT)*5)<this->m_wnAllocDataLength)
			return THICK_ITEM(m_pAllocData+siOffset);
		else
			return THICK_ITEM(NULL);
	}
	else
		return THICK_ITEM(NULL);
}

long CDesignThickPara::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	pListHandlers->Append(HANDLE_ADDR((char*)keystr,51));
	return 1;
}
bool CDesignThickPara::ReplaceReferenceHandle(CHandleReplacer& replacer)
{
	CDesignThickPara thick;
	bool replaced=false;
	for(THICK_ITEM item=GetFirstItem();item.val!=0;item=GetNextItem())
	{
		if(!item.IsHandle())
			thick.AddThick(item.val);
		else
		{
			int sign=1;
			if(item.val<0)
				sign=-1;
			long h=abs(item.val);
			if(replacer.VerifyAndReplace(&h))
			{
				replaced=true;
				thick.AddThick(sign*h,TRUE);
			}
			else
				thick.AddThick(item.val,TRUE);
		}
	}
	key_str.Copy(thick.key_str);
	return replaced;
}
long CParaThick::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	int i=0,hits=0;
	for (THICK_ITEM item = GetFirstItem(); item.val != 0; item = GetNextItem(),i++)
	{
		if (!item.IsHandle())
			continue;
		pListHandlers->Append(HANDLE_ADDR((long*)&data[i*5+1]));
		hits++;
	}
	return hits;
}
bool CParaThick::ReplaceReferenceHandle(CHandleReplacer& replacer)
{
	CParaThick thick;
	bool replaced=false;
	for(THICK_ITEM item=GetFirstItem();item.val!=0;item=GetNextItem())
	{
		if(!item.IsHandle())
			thick.AddThick(item.val);
		else
		{
			int sign=1;
			if(item.val<0)
				sign=-1;
			long h=abs(item.val);
			if(replacer.VerifyAndReplace(&h))
			{
				replaced=true;
				thick.AddThick(sign*h,TRUE);
			}
			else
				thick.AddThick(item.val,TRUE);
		}
	}
	*this=thick;
	//strcpy(key_str,thick.key_str);
	return replaced;
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
CDesignLsPara::CDesignLsPara()
{
	memset(this,0,sizeof(CDesignLsPara));
	des_base_pos.m_biWingOffsetStyle=4;//Ĭ��Ϊ�Զ���,������ܻ���ԭ�����ͻ�ظ��������ƫ���� wjh-2016.4.14
	iNo=0;
	strcpy(grade,"");	//������Ԥ���趨��˨���������˨�������Զ���������㼶�� wjh-2014.10.08
	d=16;
	hole_d_increment=1.5;
}
bool CDesignLsPara::DesignLs(CLDSBolt *pLs)
{
	pLs->set_d(d);
	pLs->SetGrade(grade);
	if(L0.Thick(pLs->BelongModel())!=0)
		pLs->AddL0Thick(L0.Thick(pLs->BelongModel()),FALSE);
	pLs->hole_d_increment = (float)hole_d_increment;
	pLs->m_bVirtualPart=m_bVirtualPart;
	pLs->dwRayNo=dwRayNo;
	pLs->des_work_norm=des_work_norm;
	//long h=pLs->des_base_pos.hLs;
	pLs->des_base_pos=des_base_pos;
	//pLs->des_base_pos.hLs=h;
	long thick=pad_thick.Thick(pLs->BelongModel());
	if(thick>0)
	{
		pLs->DianQuan.offset=pad_offset.Thick(pLs->BelongModel());
		pLs->CalPadPara(thick);
	}
	//pLs->m_nLsCap=m_nLsCap;//m_bTwoCapLs;
	pLs->correct_worknorm();
	pLs->correct_pos();
	pLs->CalGuigeAuto();
	pLs->CalPadPara(pad_thick.Thick(pLs->BelongModel()));
	pLs->DianQuan.offset=pad_offset.Thick(pLs->BelongModel());
	pMapLs=pLs;
	return true;
}

void CDesignLsPara::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteInteger(d);
	buffer.WriteInteger(dwRayNo);
	buffer.WriteString(grade);
	buffer.WriteInteger(iNo);
	buffer.WriteDouble(hole_d_increment);
	//buffer.WriteInteger(m_nLsCap);//m_bTwoCapLs);	//���˲飬�˲�����δʵ��ʹ�ã�Ҳδ�漰�ļ��汾��ʽ����ȥ�� wjh 2011.11.26
	buffer.WriteBooleanThin(m_bVirtualPart);
	buffer.WriteString(L0.ToThickStr());//key_str);
	buffer.WriteString(pad_thick.key_str);
	buffer.WriteString(pad_offset.key_str);
	des_base_pos.datumPoint.ToBuffer(buffer,version,doc_type);
	buffer.WriteInteger(des_base_pos.hPart);
	buffer.WriteDouble(des_base_pos.len_offset_dist);
	buffer.WriteDouble(des_base_pos.wing_offset_dist);
	buffer.WriteByte(des_base_pos.direction);
	buffer.WriteByte(des_base_pos.offset_wing);
	buffer.WriteString(des_base_pos.norm_offset.key_str);
	des_work_norm.ToBuffer(buffer,version,doc_type);
}
void CDesignLsPara::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<��˨��Ʋ���>\n");
	fprintf(fp,"<��˨ֱ�� d=\"%d\"/>\n",d);
	fprintf(fp,"<���ڵ����������� dwRayNo=\"%d\"/>\n",dwRayNo);
	fprintf(fp,"<���� grade=\"%s\"/>\n",grade);
	fprintf(fp,"<��� iNo=\"%d\"/>\n",iNo);
	sprintf(sText,"%f",hole_d_increment);
	SimplifiedNumString(sText);
	fprintf(fp,"<��˨��ֱ������ hole_d_increment=\"%s\"/>\n",sText);
	if (m_bVirtualPart)
		fprintf(fp,"<������˨ m_bVirtualPart=\"true\"/>\n");
	else
		fprintf(fp,"<������˨ m_bVirtualPart=\"false\"/>\n");
	fprintf(fp,"<ͨ�� L0.key_str=\"%s\"/>\n",(char*)L0.ToThickStr());
	fprintf(fp,"<��˨�� pad_thick.key_str=\"%s\"/>\n",(char*)pad_thick.key_str);
	fprintf(fp,"<��˨���Ի���ƫ���� pad_offset.key_str=\"%s\"/>\n",(char*)pad_offset.key_str);
	des_base_pos.datumPoint.ToXmlFile(fp,schema);
	fprintf(fp,"<��λ��׼������� des_base_pos.hPart=\"0X%X\"/>\n",des_base_pos.hPart);
	sprintf(sText,"%f",des_base_pos.len_offset_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<����ƫ��ֵ des_base_pos.len_offset_dist=\"%s\"/>\n",sText);
	sprintf(sText,"%f",des_base_pos.wing_offset_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<֫��ƫ��ֵ des_base_pos.wing_offset_dist=\"%s\"/>\n",sText);
	if (des_base_pos.direction==0)
		fprintf(fp,"<��׼�Ǹַ��� des_base_pos.direction=\"0.ʼ->��\"/>\n");
	else if (des_base_pos.direction==1)
		fprintf(fp,"<��׼�Ǹַ��� des_base_pos.direction=\"1.��->ʼ\"/>\n");
	if (des_base_pos.offset_wing==0)
		fprintf(fp,"<ƫ��֫ des_base_pos.offset_wing=\"0.X֫\"/>\n");
	else if (des_base_pos.offset_wing==1)			   
		fprintf(fp,"<ƫ��֫ des_base_pos.offset_wing=\"1.Y֫\"/>\n");
	fprintf(fp,"<����ƫ�����ַ��� des_base_pos.norm_offset.key_str=\"%s\"/>\n",(char*)des_base_pos.norm_offset.key_str);
	fprintf(fp,"</��˨��Ʋ���>\n");
}
void CDesignLsPara::FromBuffer(CBuffer &buffer)
{
	DWORD dw;
	bool bPrevDeflateVersion=false;	//�ļ�����֮ǰ�İ汾
/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version>0&&version<4000027)//version!=NULL&&compareVersion(version,"4.0.0.27")<0)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version>0&&version<1000082)//version!=NULL&&compareVersion(version,"1.0.0.82")<0)
#else
	if(version>0&&version<1000009)//version!=NULL&&compareVersion(version,"1.0.0.09")<0)
#endif*/
	//if(version>0&&((doc_type==1&&version<4000027)||(doc_type==2&&version<1000082)||(doc_type==4&&version<1000009)))
	//	bPrevDeflateVersion=true;
	buffer.ReadInteger(&d);
	buffer.ReadInteger(&dwRayNo);
	buffer.ReadString(grade);
	buffer.ReadInteger(&iNo);
	buffer.ReadDouble(&hole_d_increment);
	//buffer.ReadInteger(&m_nLsCap);//m_bTwoCapLs);
	if(bPrevDeflateVersion)
	{
		buffer.ReadInteger(&dw);
		m_bVirtualPart=(dw!=0);
	}
	else
		buffer.ReadBooleanThin(&m_bVirtualPart);
	//buffer.ReadString(L0.key_str);
	L0.FromBuffStr(buffer);
	buffer.ReadString(pad_thick.key_str);
	buffer.ReadString(pad_offset.key_str);
	/*if(version>=1000021)//compareVersion(version,"1.0.0.21")>=0)
	{
		WORD w;
		int iTm;
		double dd;
		buffer.ReadInteger(&iTm);
		//des_base_pos.datum_pos=TranslateLsPosStyleFromI(iTm);;
		buffer.ReadInteger(&iTm);//des_base_pos.hNode);
		buffer.ReadInteger(&iTm);//des_base_pos.hLs);
		buffer.ReadWord(&w);des_base_pos.direction=(BYTE)w;
		buffer.ReadInteger(&iTm);//des_base_pos.hPart1);
		buffer.ReadInteger(&iTm);//des_base_pos.hPart2);
		buffer.ReadDouble(&dd);//des_base_pos.g1);
		buffer.ReadDouble(&dd);//des_base_pos.g2);
		buffer.ReadDouble(&des_base_pos.len_offset_dist);
		buffer.ReadDouble(&des_base_pos.wing_offset_dist);
		buffer.ReadWord(&w);des_base_pos.offset_wing = (BYTE)w;
	}
	else*/
	{
		des_base_pos.datumPoint.FromBuffer(buffer,0,0);
		buffer.ReadInteger(&des_base_pos.hPart);
		buffer.ReadDouble(&des_base_pos.len_offset_dist);
		buffer.ReadDouble(&des_base_pos.wing_offset_dist);
		buffer.ReadByte(&des_base_pos.direction);
		buffer.ReadByte(&des_base_pos.offset_wing);
	}
	buffer.ReadString(des_base_pos.norm_offset.key_str);
	des_work_norm.FromBuffer(buffer);
}
void CDesignLsPara::CloneTo(CDesignLsPara &ls_para)
{
	CBuffer buffer;
	ToBuffer(buffer);
	buffer.SeekToBegin();
	ls_para.FromBuffer(buffer);
}

void CDesignPadPlankPara::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(iNo);
	buffer.WriteString(sPartNo);
  	buffer.WriteString(ls_str);	    // ��˨��Χ�ַ���
	buffer.WriteInteger(hBaseJg);			// ������׼�Ǹ�
	buffer.WriteInteger(hCutRefJg);			// �н�ʱ�Ļ�׼�Ǹ�,һ�������ӻ�׼�Ǹ�
  	thick.ToBuffer(buffer);	// ��������
}
void CDesignPadPlankPara::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<��˨�����Ʋ���>\n");
	fprintf(fp,"<��� iNo=\"%d\"/>\n",iNo);
	fprintf(fp,"<��幹����� sPartNo=\"%s\"/>\n",(char*)sPartNo);
	fprintf(fp,"<��˨��Χ�ַ��� ls_str=\"%s\"/>\n",ls_str);
	fprintf(fp,"<������׼�Ǹ� hCutRefJg=\"OX%X\"/>\n",hCutRefJg);
	fprintf(fp,"<�н�ʱ�Ļ�׼�Ǹ� hBaseJg=\"OX%X\"/>\n",hBaseJg);
	CParaThick parathick(thick.key_str);
	parathick.ToXmlFile(fp,schema);
	fprintf(fp,"</��˨�����Ʋ���>\n");
}
void CDesignPadPlankPara::FromBuffer(CBuffer &buffer, long version)
{
	buffer.ReadInteger(&iNo);
	buffer.ReadString(sPartNo);
  	buffer.ReadString(ls_str);	    // ��˨��Χ�ַ���
	buffer.ReadInteger(&hBaseJg);	// ������׼�Ǹ�
	buffer.ReadInteger(&hCutRefJg);	// �н�ʱ�Ļ�׼�Ǹ�,һ�������ӻ�׼�Ǹ�
  	thick.FromBuffer(buffer);		// ��������
}
BYTE ANGLE_SPACE_FLAG::StyleOfBerSpace()
{	//ռ��1,2��λ
	return cbFlag&0x03;
}
BYTE ANGLE_SPACE_FLAG::StyleOfWingSpace()
{	//ռ��3,4��λ
	return ((cbFlag&0x0C)>>2);
}
BYTE ANGLE_SPACE_FLAG::StyleOfEndSpace()
{	//ռ��5,6��λ
	return ((cbFlag&0x30)>>4);
}
BYTE ANGLE_SPACE_FLAG::SetBerSpaceStyle(BYTE cbSpaceStyle)
{	//ռ��1,2��λ
	cbSpaceStyle&=0x03;
	cbFlag&=0xFC;	//�����1,2ռλ
	return cbFlag|=cbSpaceStyle;
}
BYTE ANGLE_SPACE_FLAG::SetWingSpaceStyle(BYTE cbSpaceStyle)
{	//ռ��3,4��λ
	cbSpaceStyle=((cbSpaceStyle&0x03)<<2);
	cbFlag&=0xF3;	//�����3,4ռλ
	return cbFlag|=cbSpaceStyle;
}
BYTE ANGLE_SPACE_FLAG::SetEndSpaceStyle(BYTE cbSpaceStyle)
{	//ռ��5,6��λ
	cbSpaceStyle=((cbSpaceStyle&0x03)<<4);
	cbFlag&=0xCF;	//�����5,6ռλ
	return cbFlag|=cbSpaceStyle;
}

CDesignLjPartPara::CDesignLjPartPara()
{
	memset(this,0,sizeof(CDesignLjPartPara));
	m_bAdjustLjEndPos=TRUE;	//Ĭ�϶Գ�ʱ���߸˼����Ӷ�λ�þ���Ҫ���� wht 10-02-26
	dualEdgeStartSideWingSpace=dualEdgeStartSideBerSpace=dualEdgeStartSideEndSpace=end_space=wing_space=ber_space=30;
	m_ciAsynDualEdgeBerSpace=m_ciAsynDualEdgeWingSpace=m_ciAsynDualEdgeEndSpace=0;	//Ĭ��Ϊ����ͬ��
	angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
	angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
	angle.cbSpaceFlag.SetEndSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
}

void CDesignLjPartPara::AddLsNo(int iLsNo)
{
	/*if(strlen(ls_str)>0)
		sprintf(ls_str,"%s,%d",ls_str,iLsNo);
	else
		sprintf(ls_str,"%d",iLsNo);*/
}

IMPLEMENT_GET(CDesignLjPara,m_bWeldToDatumJg)
{
	return (uiStyleFlag&0x01)>0;
}
IMPLEMENT_SET(CDesignLjPara,m_bWeldToDatumJg)
{
	if(value)
		uiStyleFlag|=0x01;
	else
		uiStyleFlag-=uiStyleFlag%2;
}
CDesignLjPara::CDesignLjPara()
{
	//hBaseNode=hBasePart=0;	//��׼�ڵ�,��׼�Ǹ�
	iProfileStyle0123=1;	//���������巽ʽ,0:��׼����1:��������2:��������3:���������
	iFaceType=1;
	iWeldLineStyle=0;
	m_bEnableFlexibleDesign=FALSE;
	weld_start_high=weld_end_high=0;
	m_hBasePart=m_hBaseNode=0;
	weld_width=60;
	m_bWeldToDatumJg=FALSE;	//������Ƿ񺸽ӵ���׼�Ǹ� wht 10-06-06
	m_bSelfCorrectBendline=false;
	uiStyleFlag=0;
}

//doc_tma1_lma2_tsa3_lds4_tda5_default0
void CDesignLjPara::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	bool bPrevDeflateVersion=false;	//�ļ�����֮ǰ�İ汾
/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version>0&&version<4000027)//version!=NULL&&compareVersion(version,"4.0.0.27")<0)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version>0&&version<1000082)//version!=NULL&&compareVersion(version,"1.0.0.82")<0)
#elif defined(__TDA_FILE_)
	if(false)//version!=NULL&&compareVersion(version,"1.0.0.82")<0)
#else
	if(version>0&&version<1000009)//version!=NULL&&compareVersion(version,"1.0.0.09")<0)
#endif*/
	if(version>0&&((doc_type==1&&version<4000027)||(doc_type==2&&version<1000082)||(doc_type==4&&version<1000009)))
		bPrevDeflateVersion=true;
	buffer.ReadInteger(&m_hBaseNode);
	buffer.ReadInteger(&m_hBasePart);
	origin.FromBuffer(buffer,version,doc_type);
	norm.FromBuffer(buffer,version,doc_type);
	buffer.ReadWord(&iFaceType);
	buffer.ReadWord(&iProfileStyle0123);
	buffer.ReadInteger(&iWeldLineStyle);
	buffer.ReadDouble(&weld_start_high);
	buffer.ReadDouble(&weld_end_high);
	buffer.ReadDouble(&weld_width);
/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version==0||version>=4000030)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version==0||version>=1000085)
#elif defined(__TDA_FILE_)
	if(true)
#else
	if(version==0||version>=1000013)
#endif*/
	if( version==0||((doc_type==1&&version>=4000030)||
		(doc_type==2&&version>=1000085)||(doc_type==4&&version>=1000013)||doc_type==5))
		buffer.ReadInteger(&uiStyleFlag);	//�Ƿ񺸽����׼�Ǹ� wht 10-06-09
	//���ӽǸ���Ʋ���
	int i,n;
	buffer.ReadInteger(&n);
	partList.Empty();
	for(i=0;i<n;i++)
	{
		long hPart=0;
		buffer.ReadInteger(&hPart);
		CDesignLjPartPara *pLinePartPara=partList.Add(hPart);
		buffer.ReadWord(&pLinePartPara->iFaceNo);
		buffer.ReadWord(&pLinePartPara->start0_end1);
		buffer.ReadInteger(&pLinePartPara->angle.bEndLjJg);
		buffer.ReadInteger(&pLinePartPara->angle.bTwoEdge);
		buffer.ReadDouble(&pLinePartPara->ber_space);
		buffer.ReadDouble(&pLinePartPara->end_space);
		double endSpace2=pLinePartPara->end_space;
		if(	version==0&&doc_type!=3||
			(doc_type==1&&version>=5010800)||	//TMA V5.1.7.0
			(doc_type==2&&version>=2000900)||	//LMA V2.0.9.0
			(doc_type==4&&version>=1030900)||	//LDS V1.3.9.0
			(doc_type==5&&version>=1030800))	//TDA V1.3.8.0
		{
			if(pLinePartPara->angle.bTwoEdge)
			{
				buffer.ReadDouble(&endSpace2);
				pLinePartPara->startSideEndSpace=endSpace2;//���д���Ŀ�����趨m_bSetDualEdgeEndSpace=true; wjh-2017.11.23
				buffer.ReadDouble(&endSpace2);
				pLinePartPara->startSideBerSpace=endSpace2;
				buffer.ReadDouble(&endSpace2);
				pLinePartPara->startSideWingSpace=endSpace2;
				bool bTemporyVersionDisableAsyncStatus=false;
				if(	doc_type==1&&version>0&&version<5010900)	//TMA V5.1.8.0
					bTemporyVersionDisableAsyncStatus=true;
				if(!bTemporyVersionDisableAsyncStatus)
				{
					BYTE ciStatus=0;
					buffer.ReadByte(&ciStatus);pLinePartPara->ciAsynDualEdgeEndSpace=ciStatus;
					buffer.ReadByte(&ciStatus);pLinePartPara->ciAsynDualEdgeBerSpace=ciStatus;
					buffer.ReadByte(&ciStatus);pLinePartPara->ciAsynDualEdgeWingSpace=ciStatus;
				}
			}
		}
		buffer.ReadDouble(&pLinePartPara->wing_space);
		//angle.cur_wing_x0_y1��short����������ֽڣ��ڲ��ı��ʽǰ��������������Ϣ wjh-2014.12.09
		//buffer.ReadWord(&pLinePartPara->angle.cur_wing_x0_y1);
		buffer.ReadByte(&pLinePartPara->angle.cur_wing_x0_y1);
		buffer.ReadByte(&pLinePartPara->angle.cbSpaceFlag.cbFlag);
		buffer.ReadWord(&pLinePartPara->start0_end1);
		//�ԳƸְ�ʱ�Ƿ���Ҫ�������߸˼����Ӷ�λ�ã���Ҫ�ڶԳƹ��ð�ʱʹ�� wht 10-02-26
/*#if defined(__TMA_FILE_)||defined(__TMA_)
		if(version==0||version>=4000026)//version==NULL||compareVersion(version,"4.0.0.26")>=0)
#elif defined(__LMA_FILE_)||defined(__LMA_)
		if(version==0||version>=1000081)//version==NULL||compareVersion(version,"1.0.0.81")>=0)
#elif defined(__TDA_FILE_)
	if(true)
#else
		if(version==0||version>=1000009)//version==NULL||compareVersion(version,"1.0.0.9")>=0)
#endif*/
	if( version==0||((doc_type==1&&version>=4000026)||
		(doc_type==2&&version>=1000081)||(doc_type==4&&version>=1000009)||doc_type==5))
			buffer.ReadInteger(&pLinePartPara->m_bAdjustLjEndPos);
	}
	//������������������Ʋ���
	buffer.ReadInteger(&n);
	additional_vertex.Empty();
	for(i=0;i<n;i++)
	{
		f3dPoint *pPt=additional_vertex.append();
		buffer.ReadPoint(*pPt);
		buffer.ReadInteger(&pPt->feature);
	}
	buffer.ReadInteger(&m_bEnableFlexibleDesign);
	if(bPrevDeflateVersion)
	{
		BOOL bFirstAcuteAngle,bSecondAcuteAngle;//ԭΪ���������ߵ�������Ƿ�Ϊ��ǣ�������Ч
		buffer.ReadInteger(&bFirstAcuteAngle);
		buffer.ReadInteger(&bSecondAcuteAngle);
		//��ƻ����淨�߲���
		for(int j=0;j<2;j++)
		{
			huoqufacenorm[j].FromBuffer(buffer,version,doc_type);
			huoquline_start[j].FromBuffer(buffer,version,doc_type);
			huoquline_end[j].FromBuffer(buffer,version,doc_type);
		}
	}
	else
	{
		if(	iFaceType>1&&(version==0||
			(doc_type==1&&version>=5010300)||	//TMA V5.1.3.0
			(doc_type==2&&version>=2000600)||	//LMA V2.0.6.0
			(doc_type==4&&version>=1030600)||	//LDS V1.3.6.0
			(doc_type==5&&version>=1030600)))	//TDA V1.3.6.0
			buffer.ReadBooleanThin(&m_bSelfCorrectBendline);
		BOOL bHasAcuteAnglePlace,bFirstAcuteAngle,bSecondAcuteAngle;//ԭΪ���������ߵ�������Ƿ�Ϊ��ǣ�������Ч wjh-2019.11.26
		if (version==0||
			(doc_type==1&&version>=5020300)||	//TMA V5.2.3.0
			(doc_type==2&&version>=2010300)||	//LMA V2.1.3.0
			(doc_type==4&&version>=1031100)||	//LDS V1.3.11.0
			(doc_type==5&&version>=1031100))	//TDA V1.3.11.0
			bHasAcuteAnglePlace=false;
		else
			bHasAcuteAnglePlace=true;
		if(iFaceType>1)
		{
			if(bHasAcuteAnglePlace)
				buffer.ReadInteger(&bFirstAcuteAngle);
			huoqufacenorm[0].FromBuffer(buffer,version,doc_type);
			huoquline_start[0].FromBuffer(buffer,version,doc_type);
			huoquline_end[0].FromBuffer(buffer,version,doc_type);
		}
		if(iFaceType>2)
		{
			if(bHasAcuteAnglePlace)
				buffer.ReadInteger(&bSecondAcuteAngle);
			huoqufacenorm[1].FromBuffer(buffer,version,doc_type);
			huoquline_start[1].FromBuffer(buffer,version,doc_type);
			huoquline_end[1].FromBuffer(buffer,version,doc_type);
		}
	}
}
void CDesignLjPara::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteInteger(m_hBaseNode);
	buffer.WriteInteger(m_hBasePart);
	origin.ToBuffer(buffer,version,doc_type);
	norm.ToBuffer(buffer,version,doc_type);
	buffer.WriteWord(iFaceType);
	buffer.WriteWord(iProfileStyle0123);
	buffer.WriteInteger(iWeldLineStyle);
	buffer.WriteDouble(weld_start_high);
	buffer.WriteDouble(weld_end_high);
	buffer.WriteDouble(weld_width);
	buffer.WriteInteger(uiStyleFlag);	//�Ƿ񺸽����׼�Ǹ� wht 10-06-09
	//���ӽǸ���Ʋ���
	buffer.WriteInteger(partList.GetNodeNum());
	BOOL bPush=partList.push_stack();
	BUFFERPOP stack(&buffer,partList.GetNodeNum());
	for(CDesignLjPartPara *pLinePartPara=partList.GetFirst();pLinePartPara;
		pLinePartPara=partList.GetNext())
	{
		buffer.WriteInteger(pLinePartPara->hPart);
		buffer.WriteWord(pLinePartPara->iFaceNo);
		buffer.WriteWord(pLinePartPara->start0_end1);
		buffer.WriteInteger(pLinePartPara->angle.bEndLjJg);
		buffer.WriteInteger(pLinePartPara->angle.bTwoEdge);
		buffer.WriteDouble(pLinePartPara->ber_space);
		buffer.WriteDouble(pLinePartPara->end_space);
		if(	version==0&&doc_type!=3||
			(doc_type==1&&version>=5010800)||	//TMA V5.1.7.0
			(doc_type==2&&version>=2000900)||	//LMA V2.0.9.0
			(doc_type==4&&version>=1030900)||	//LDS V1.3.9.0
			(doc_type==5&&version>=1030800))	//TDA V1.3.8.0
		{
			if(pLinePartPara->angle.bTwoEdge)
			{
				buffer.WriteDouble(pLinePartPara->startSideEndSpace);
				buffer.WriteDouble(pLinePartPara->startSideBerSpace);
				buffer.WriteDouble(pLinePartPara->startSideWingSpace);
				bool bTemporyVersionDisableAsyncStatus=false;
				if(	doc_type==1&&version>0&&version<5010900)	//TMA V5.1.8.0
					bTemporyVersionDisableAsyncStatus=true;
				if(!bTemporyVersionDisableAsyncStatus)
				{
					buffer.WriteByte(pLinePartPara->ciAsynDualEdgeEndSpace);
					buffer.WriteByte(pLinePartPara->ciAsynDualEdgeBerSpace);
					buffer.WriteByte(pLinePartPara->ciAsynDualEdgeWingSpace);
				}
			}
		}
		buffer.WriteDouble(pLinePartPara->wing_space);
		//angle.cur_wing_x0_y1��short����������ֽڣ��ڲ��ı��ʽǰ��������������Ϣ wjh-2014.12.09
		if(	version==0||
			(doc_type==1&&version>=5000000)||	//TMA V5.0.0.0
			(doc_type==2&&version>=2000000)||	//LMA V2.0.0.0
			(doc_type==3&&version>=2010000)||	//TSA V2.1.0.0
			(doc_type==4&&version>=1030000)||	//LDS V1.3.0.0
			(doc_type==5&&version>=1030000))	//TDA V1.3.0.0
		{
			buffer.WriteByte(pLinePartPara->angle.cur_wing_x0_y1);
			buffer.WriteByte(pLinePartPara->angle.cbSpaceFlag);
		}
		else
			buffer.WriteWord((WORD)pLinePartPara->angle.cur_wing_x0_y1);
		buffer.WriteWord(pLinePartPara->start0_end1);
		//�ԳƸְ�ʱ�Ƿ���Ҫ�������߸˼����Ӷ�λ�ã���Ҫ�ڶԳƹ��ð�ʱʹ�� wht 10-02-26
		buffer.WriteInteger(pLinePartPara->m_bAdjustLjEndPos);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of part is wrong!");
#else
		CLDSObject::Log2File()->Log("������¼�����ִ���!");
#endif

	if(bPush)
		partList.pop_stack();
	//������������������Ʋ���
	buffer.WriteInteger(additional_vertex.GetNodeNum());
	for(f3dPoint *pPt=additional_vertex.GetFirst();pPt;pPt=additional_vertex.GetNext())
	{
		buffer.WritePoint(*pPt);
		buffer.WriteInteger(pPt->feature);	//���Ӷ����������
	}
	buffer.WriteInteger(m_bEnableFlexibleDesign);
	//��ƻ����淨�߲���
	if(	iFaceType>1&&(version==0||
		(doc_type==1&&version>=5010300)||	//TMA V5.1.3.0
		(doc_type==2&&version>=2000600)||	//LMA V2.0.6.0
		(doc_type==4&&version>=1030600)||	//LDS V1.3.6.0
		(doc_type==5&&version>=1030600)))	//TDA V1.3.6.0
		buffer.WriteBooleanThin(m_bSelfCorrectBendline);
	//ԭΪ���������ߵ�������Ƿ�Ϊ��ǣ�������Ч wjh-2019.11.26
	BOOL bHasAcuteAnglePlace,bFirstAcuteAngle=true,bSecondAcuteAngle=true;
	if (version==0||
		(doc_type==1&&version>=5020300)||	//TMA V5.2.3.0
		(doc_type==2&&version>=2010300)||	//LMA V2.1.3.0
		(doc_type==4&&version>=1031100)||	//LDS V1.3.11.0
		(doc_type==5&&version>=1031100))	//TDA V1.3.11.0
		bHasAcuteAnglePlace=false;
	else
		bHasAcuteAnglePlace=true;
	if(iFaceType>1)
	{
		if(bHasAcuteAnglePlace)
			buffer.WriteInteger(bFirstAcuteAngle);
		huoqufacenorm[0].ToBuffer(buffer,version,doc_type);
		huoquline_start[0].ToBuffer(buffer,version,doc_type);
		huoquline_end[0].ToBuffer(buffer,version,doc_type);
	}
	if(iFaceType>2)
	{
		if(bHasAcuteAnglePlace)
			buffer.WriteInteger(bSecondAcuteAngle);
		huoqufacenorm[1].ToBuffer(buffer,version,doc_type);
		huoquline_start[1].ToBuffer(buffer,version,doc_type);
		huoquline_end[1].ToBuffer(buffer,version,doc_type);
	}
}
void CDesignLjPara::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<������Ʋ���>\n");
	fprintf(fp,"<��׼�ڵ� m_hBaseNode=\"OX%X\"/>\n",m_hBaseNode);
	fprintf(fp,"<��׼�˼� m_hBasePart=\"OX%X\"/>\n",m_hBasePart);
	origin.ToXmlFile(fp,schema);
	norm.ToXmlFile(fp,schema);
	if (iFaceType==1)
		fprintf(fp,"<�������� iFaceType=\"1.�����\"/>\n");
	else if (iFaceType==2)
		fprintf(fp,"<�������� iFaceType=\"2.˫���\"/>\n");
	else if (iFaceType==3)
		fprintf(fp,"<�������� iFaceType=\"3.�����\"/>\n");
	else if (iFaceType==4)
		fprintf(fp,"<�������� iFaceType=\"4.�۵���\"/>\n");
	else if (iFaceType==5)
		fprintf(fp,"<�������� iFaceType=\"5.��˨���\"/>\n");
	if (iProfileStyle0123==0)
		fprintf(fp,"<���������巽ʽ iProfileStyle0123=\"0.��׼����\"/>\n");
	else if (iProfileStyle0123==1)
		fprintf(fp,"<���������巽ʽ iProfileStyle0123=\"1.��������\"/>\n");
	else if (iProfileStyle0123==2)						
		fprintf(fp,"<���������巽ʽ iProfileStyle0123=\"2.��������\"/>\n");
	else if (iProfileStyle0123==3)						
		fprintf(fp,"<���������巽ʽ iProfileStyle0123=\"3.���������\"/>\n");
	if (iWeldLineStyle==0)
		fprintf(fp,"<�����߷�ʽ iWeldLineStyle=\"0.���ɼ���\"/>\n");
	else if (iWeldLineStyle==1)					 
		fprintf(fp,"<�����߷�ʽ iWeldLineStyle=\"1.ָ��������\"/>\n");
	else if (iWeldLineStyle==2)					 
		fprintf(fp,"<�����߷�ʽ iWeldLineStyle=\"2.���뺸����\"/>\n");
	sprintf(sText,"%f",weld_start_high);
	SimplifiedNumString(sText);
	fprintf(fp,"<������ʼ�� weld_start_high=\"%s\"/>\n",sText);
	sprintf(sText,"%f",weld_end_high);
	SimplifiedNumString(sText);
	fprintf(fp,"<������ֹ�� weld_end_high=\"%s\"/>\n",sText);
	sprintf(sText,"%f",weld_width);
	SimplifiedNumString(sText);
	fprintf(fp,"<���ߺ����� weld_width=\"%s\"/>\n",sText);
	if (m_bWeldToDatumJg)
		fprintf(fp,"<������Ƿ񺸽ӵ���׼�Ǹ� m_bWeldToDatumJg=\"true\"/>\n");
	else
		fprintf(fp,"<������Ƿ񺸽ӵ���׼�Ǹ� m_bWeldToDatumJg=\"false\"/>\n");
	//���ӽǸ���Ʋ���
	fprintf(fp,"<���ӽǸ���Ʋ���>\n");
	fprintf(fp,"<���Ӹ˼����� partList.GetNodeNum=\"%d\"/>\n",partList.GetNodeNum());
	BOOL bPush=partList.push_stack();
	for(CDesignLjPartPara *pLinePartPara=partList.GetFirst();pLinePartPara;
		pLinePartPara=partList.GetNext())
	{
		fprintf(fp,"<���Ӹ˼����� ��ǰ�ֹܾ��=\"0X%X\"",pLinePartPara->hPart);
		fprintf(fp," ����������=\"%d\"",pLinePartPara->iFaceNo);
		if (pLinePartPara->start0_end1==0)
			fprintf(fp," �������=\"0.ʼ��\"");
		else if (pLinePartPara->start0_end1==0)
			fprintf(fp," �������=\"1.�ն�\"");
		if (pLinePartPara->angle.bEndLjJg)
			fprintf(fp," �����ӽǸ�=\"true\"");
		else
			fprintf(fp," �����ӽǸ�=\"false\"");
		if (pLinePartPara->angle.bTwoEdge)
			fprintf(fp," �Ƿ���˨�����඼����������=\"true\"");
		else
			fprintf(fp," �Ƿ���˨�����඼����������=\"false\"");
		sprintf(sText,"%f",pLinePartPara->ber_space);
		SimplifiedNumString(sText);
		fprintf(fp," ber_space=\"%s\"",sText);
		sprintf(sText,"%f",pLinePartPara->end_space);
		SimplifiedNumString(sText);
		fprintf(fp," end_space=\"%s\"",sText);
		sprintf(sText,"%f",pLinePartPara->wing_space);
		SimplifiedNumString(sText);
		fprintf(fp," wing_space=\"%s\"",sText);
		if (pLinePartPara->angle.cur_wing_x0_y1==0)
			fprintf(fp," ��ǰ����֫=\"0.X֫\"");
		else if (pLinePartPara->angle.cur_wing_x0_y1==1)
			fprintf(fp," ��ǰ����֫=\"1.Y֫\"");
		if (pLinePartPara->m_bAdjustLjEndPos)
			fprintf(fp," �ԳƸְ�ʱ�Ƿ���Ҫ�������߸˼����Ӷ˰ڷ�λ��=\"true\"");
		else
			fprintf(fp," �ԳƸְ�ʱ�Ƿ���Ҫ�������߸˼����Ӷ˰ڷ�λ��=\"false\"");
		fprintf(fp,"/>\n");
	}
	fprintf(fp,"</���ӽǸ���Ʋ���>\n");
	if(bPush)
		partList.pop_stack();
	//������������������Ʋ���
	fprintf(fp,"<������������������Ʋ���>\n");
	fprintf(fp,"<������������������ additional_vertex.GetNodeNum=\"%d\"/>\n",additional_vertex.GetNodeNum());
	for(f3dPoint *pPt=additional_vertex.GetFirst();pPt;pPt=additional_vertex.GetNext())
	{
		sprintf(sText,"%f",pPt->x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",pPt->y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",pPt->z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<���Ӷ��� pPt=\"(%s,%s,%s)\"",sText,sText2,sText3);
		fprintf(fp," ���Ӷ����������=\"%d\"",pPt->feature);
		fprintf(fp,"/>\n");
	}
	fprintf(fp,"</������������������Ʋ���>\n");
	if (m_bEnableFlexibleDesign)
		fprintf(fp,"<���ò������������ m_bEnableFlexibleDesign=\"true\"/>\n");
	else
		fprintf(fp,"<���ò������������ m_bEnableFlexibleDesign=\"false\"/>\n");
	//��ƻ����淨�߲���
	if(iFaceType>1)
	{
		huoqufacenorm[0].ToXmlFile(fp,schema);
		huoquline_start[0].ToXmlFile(fp,schema);
		huoquline_end[0].ToXmlFile(fp,schema);
	}
	if(iFaceType>2)
	{
		huoqufacenorm[1].ToXmlFile(fp,schema);
		huoquline_start[1].ToXmlFile(fp,schema);
		huoquline_end[1].ToXmlFile(fp,schema);
	}
	fprintf(fp,"</������Ʋ���>\n");
}
bool CDesignLjPara::ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType/*=0*/)
{
	bool replaced=false;
	if(idClassType==0||idClassType==CLS_NODE)
		replaced|=replacer.VerifyAndReplace(&m_hBaseNode);
	if(idClassType==0||idClassType!=CLS_NODE)
		replaced|=replacer.VerifyAndReplace(&m_hBasePart);
	replaced|=origin.ReplaceReferenceHandle(replacer,NULL,idClassType);
	int i=0;
	for(i=0;i<2;i++)
	{	//����ճ��˫�����������ʱ��Ҫ���»�����ʼ�ն���Ʋ���	wht 10-01-13
		replaced|=huoquline_start[i].ReplaceReferenceHandle(replacer,NULL,idClassType);
		replaced|=huoquline_end[i].ReplaceReferenceHandle(replacer,NULL,idClassType);
	}
	if(idClassType==0||idClassType!=CLS_NODE)
	{
		replaced|=huoqufacenorm[0].ReplaceReferenceHandle(replacer);
		replaced|=huoqufacenorm[1].ReplaceReferenceHandle(replacer);
		replaced|=norm.ReplaceReferenceHandle(replacer);
		for(CDesignLjPartPara *pLjPara=partList.GetFirst();pLjPara;pLjPara=partList.GetNext())
		{
			if(pLjPara->hPart==replacer.hOld)
			{	
				if(replacer.IsReverseStartAndEnd()&&pLjPara->start0_end1<2)
					pLjPara->start0_end1=(pLjPara->start0_end1==1)?0:1;
				if(replacer.IsReverseWingXAndWingY())
					pLjPara->angle.cur_wing_x0_y1=(pLjPara->angle.cur_wing_x0_y1==1)?0:1;
			}
			replaced|=replacer.VerifyAndReplace(&pLjPara->hPart);
		}
	}
	return replaced;
}
long CDesignLjPara::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	long nInitCount = pListHandlers->Count;
	pListHandlers->Append(HANDLE_ADDR(&m_hBaseNode));
	pListHandlers->Append(HANDLE_ADDR(&m_hBasePart));
	origin.StatObjHandleReference(pListHandlers);
	int i = 0;
	for (i = 0; i < 2; i++)
	{	//����ճ��˫�����������ʱ��Ҫ���»�����ʼ�ն���Ʋ���	wht 10-01-13
		huoquline_start[i].StatObjHandleReference(pListHandlers);
		huoquline_end[i].StatObjHandleReference(pListHandlers);
	}
	huoqufacenorm[0].StatObjHandleReference(pListHandlers);
	huoqufacenorm[1].StatObjHandleReference(pListHandlers);
	norm.StatObjHandleReference(pListHandlers);
	for (CDesignLjPartPara *pLjPara = partList.GetFirst(); pLjPara; pLjPara = partList.GetNext())
		pListHandlers->Append(HANDLE_ADDR(&pLjPara->hPart));
	return pListHandlers->Count-nInitCount;
}

void CDesignLjPara::CloneTo(CDesignLjPara &designInfo)
{
	CBuffer buffer;
	ToBuffer(buffer);
	buffer.SeekToBegin();
	designInfo.FromBuffer(buffer);
}

//���ݸ˼���� �������Ӹ˼�����
CDesignLjPartPara *CDesignLjPara::FromHandle(long hPart)
{	
	CDesignLjPartPara *pLjPara=NULL;
	BOOL bPush=partList.push_stack();
	for(pLjPara=partList.GetFirst();pLjPara;pLjPara=partList.GetNext())
	{
		if(pLjPara->hPart==hPart)
			break;
	}
	if(bPush)
		partList.pop_stack();
	return pLjPara;
}
////////////////////////////////////////////////////////////////////////////////
//CDesignLjPartPara��������
////////////////////////////////////////////////////////////////////////////////
static const DWORD HASHTABLESIZE = 500;
static const DWORD STATUSHASHTABLESIZE = 50;
IMPLEMENT_PROP_FUNC(CDesignLjPartPara);
void CDesignLjPartPara::InitViewPropHashtable()
{
	return InitPropHashtable();
}
void CDesignLjPartPara::InitPropHashtable()
{
	int id = 1;
	CDesignLjPartPara::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CDesignLjPartPara::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CDesignLjPartPara::AddPropItem("baseInfo", PROPLIST_ITEM(id++, "basic info"));
	CDesignLjPartPara::AddPropItem("iFaceNo", PROPLIST_ITEM(id++, "plane", "plane", "Plane No.1|Plane No.2|Plane No.3"));
	CDesignLjPartPara::AddPropItem("hPart", PROPLIST_ITEM(id++, "handle", "connecting rods handles"));
	CDesignLjPartPara::AddPropItem("start0_end1", PROPLIST_ITEM(id++, "connecting end", "", "starting|ending|middle"));
	CDesignLjPartPara::AddPropItem("angle.cur_wing_x0_y1", PROPLIST_ITEM(id++, "connecting wing", "", "X Leg|Y Leg"));
	CDesignLjPartPara::AddPropItem("profileInfo", PROPLIST_ITEM(id++, "profile definition"));
	CDesignLjPartPara::AddPropItem("angle.bTwoEdge", PROPLIST_ITEM(id++, "two ends profile", "Does outline vertex exist on both sides of bolts line", "Yes|No"));
	CDesignLjPartPara::AddPropItem("ber_space_style", PROPLIST_ITEM(id++, "ridge side space style", "", "0.Specified|1.Standard Margin|2.Extend To Margin|3.Extend To Margin-thickness"));
	CDesignLjPartPara::AddPropItem("ber_space", PROPLIST_ITEM(id++, "ridge side space", ""));
	CDesignLjPartPara::AddPropItem("end_space_style", PROPLIST_ITEM(id++, "end space style", ""));
	CDesignLjPartPara::AddPropItem("end_space", PROPLIST_ITEM(id++, "end distance", ""));
	CDesignLjPartPara::AddPropItem("wing_space_style", PROPLIST_ITEM(id++, "wing side space type", "distance measuring way from wing side bolts to plate margin", "0.Specified|1.Standard Margin|2.Extend To Margin|3.Extend To Margin-thickness"));
	CDesignLjPartPara::AddPropItem("wing_space", PROPLIST_ITEM(id++, "wing side space", ""));
	CDesignLjPartPara::AddPropItem("link_ls", PROPLIST_ITEM(id++, "link bolt", ""));
#else
	CDesignLjPartPara::AddPropItem("baseInfo", PROPLIST_ITEM(id++, "������Ϣ"));
	CDesignLjPartPara::AddPropItem("iFaceNo", PROPLIST_ITEM(id++, "������","���Ӹ˼�������","1����|2����|3����"));
	CDesignLjPartPara::AddPropItem("hPart", PROPLIST_ITEM(id++,"���","���Ӹ˼����"));
	CDesignLjPartPara::AddPropItem("start0_end1", PROPLIST_ITEM(id++, "���Ӷ�", "���Ӷ�","ʼ��|�ն�|�м�"));
	CDesignLjPartPara::AddPropItem("angle.cur_wing_x0_y1", PROPLIST_ITEM(id++, "����֫", "����֫","X֫|Y֫"));
	CDesignLjPartPara::AddPropItem("profileInfo", PROPLIST_ITEM(id++, "���ζ���"));
	CDesignLjPartPara::AddPropItem("angle.bTwoEdge", PROPLIST_ITEM(id++, "��������", "�Ƿ���˨�����඼����������","��|��"));
	CDesignLjPartPara::AddPropItem("ber_space_style", PROPLIST_ITEM(id++, "���߲�෽ʽ", "���߲���˨���ߵľ���ȷ����ʽ","0.ָ���߾�|1.��׼�߾�|2.��������|3.��������-��"));
	CDesignLjPartPara::AddPropItem("ber_space", PROPLIST_ITEM(id++, "���߲��", "���߲��"));
	CDesignLjPartPara::AddPropItem("end_space_style", PROPLIST_ITEM(id++, "�˾෽ʽ", ""));
	CDesignLjPartPara::AddPropItem("end_space", PROPLIST_ITEM(id++, "�˾�", "�˾�"));
	CDesignLjPartPara::AddPropItem("wing_space_style", PROPLIST_ITEM(id++, "֫���෽ʽ", "֫�����˨���ߵľ���ȷ����ʽ","0.ָ���߾�|1.��׼�߾�|2.��������|3.��������-��"));
	CDesignLjPartPara::AddPropItem("wing_space", PROPLIST_ITEM(id++, "֫����", "֫����"));
	CDesignLjPartPara::AddPropItem("link_ls", PROPLIST_ITEM(id++, "������˨", "�˼��͸ְ����ӵ���˨"));
#endif
}
IMPLEMENT_SET(CDesignLjPartPara,startSideEndSpace)
{
	CLDSLinePart* pRod=CLDSPart::_console?CLDSPart::_console->FromRodHandle(hPart):NULL;
	if(pRod&&pRod->IsAngle()&&angle.bTwoEdge)
	{
		if(m_ciAsynDualEdgeEndSpace==0)
			end_space=value;			//���ֶ��趨λ����ͬ���˾�
		dualEdgeStartSideEndSpace=value;
	}
	else
		end_space=value;
}
IMPLEMENT_GET(CDesignLjPartPara,startSideEndSpace)
{
	CLDSLinePart* pRod=CLDSPart::_console?CLDSPart::_console->FromRodHandle(hPart):NULL;
	if(pRod&&pRod->IsAngle()&&angle.bTwoEdge)
		return m_ciAsynDualEdgeEndSpace==1?dualEdgeStartSideEndSpace:end_space;
	else
		return end_space;
}
IMPLEMENT_SET(CDesignLjPartPara,endSideEndSpace)
{
	end_space=value;
	if(m_ciAsynDualEdgeEndSpace==0)	//�ֶ��趨Ϊͬ������˾�
		dualEdgeStartSideEndSpace=end_space;
}
IMPLEMENT_GET(CDesignLjPartPara,endSideEndSpace)
{
	return end_space;
}
//////////////////////////////////////////////
IMPLEMENT_SET(CDesignLjPartPara,startSideBerSpace)
{
	CLDSLinePart* pRod=CLDSPart::_console?CLDSPart::_console->FromRodHandle(hPart):NULL;
	if(pRod&&pRod->IsAngle()&&angle.bTwoEdge)
	{
		dualEdgeStartSideBerSpace=value;
		if(m_ciAsynDualEdgeBerSpace==0)
			ber_space=value;			//���ֶ��趨λ����ͬ�����߾�
	}
	else
		ber_space=value;
}
IMPLEMENT_GET(CDesignLjPartPara,startSideBerSpace)
{
	CLDSLinePart* pRod=CLDSPart::_console?CLDSPart::_console->FromRodHandle(hPart):NULL;
	if(pRod&&pRod->IsAngle()&&angle.bTwoEdge)
		return m_ciAsynDualEdgeBerSpace==1?dualEdgeStartSideBerSpace:ber_space;
	else
		return ber_space;
}
IMPLEMENT_SET(CDesignLjPartPara,endSideBerSpace)
{
	ber_space=value;
	if(m_ciAsynDualEdgeBerSpace==0)	//�ֶ��趨Ϊͬ�����౳�߾�
		dualEdgeStartSideBerSpace=ber_space;
}
IMPLEMENT_GET(CDesignLjPartPara,endSideBerSpace)
{
	return ber_space;
}
//////////////////////////////////////////////
IMPLEMENT_SET(CDesignLjPartPara,startSideWingSpace)
{
	CLDSLinePart* pRod=CLDSPart::_console?CLDSPart::_console->FromRodHandle(hPart):NULL;
	if(pRod&&pRod->IsAngle()&&angle.bTwoEdge)
	{
		dualEdgeStartSideWingSpace=value;
		if(m_ciAsynDualEdgeWingSpace==0)
			wing_space=value;			//���ֶ��趨λ����ͬ��֫��߾�
	}
	else
		wing_space=value;
}
IMPLEMENT_GET(CDesignLjPartPara,startSideWingSpace)
{
	CLDSLinePart* pRod=CLDSPart::_console?CLDSPart::_console->FromRodHandle(hPart):NULL;
	if(pRod&&pRod->IsAngle()&&angle.bTwoEdge)
		return m_ciAsynDualEdgeWingSpace==1?dualEdgeStartSideWingSpace:wing_space;
	else
		return wing_space;
}
IMPLEMENT_SET(CDesignLjPartPara,endSideWingSpace)
{
	wing_space=value;
	if(m_ciAsynDualEdgeWingSpace==0)	//�ֶ��趨Ϊͬ������֫��߾�
		dualEdgeStartSideWingSpace=wing_space;
}
IMPLEMENT_GET(CDesignLjPartPara,endSideWingSpace)
{
	return wing_space;
}
//////////////////////////////////////////////
IMPLEMENT_SET(CDesignLjPartPara,ciAsynDualEdgeEndSpace){m_ciAsynDualEdgeEndSpace=min(value,1);}
IMPLEMENT_GET(CDesignLjPartPara,ciAsynDualEdgeEndSpace){
	if(angle.cbSpaceFlag.StyleOfEndSpace()!=ANGLE_SPACE_FLAG::SPACE_UDF)
		return 0;	//����ȡֵ��ͬ
	else
		return m_ciAsynDualEdgeEndSpace;
}
IMPLEMENT_SET(CDesignLjPartPara,ciAsynDualEdgeBerSpace){m_ciAsynDualEdgeBerSpace=min(value,1);}
IMPLEMENT_GET(CDesignLjPartPara,ciAsynDualEdgeBerSpace){
	if(angle.cbSpaceFlag.StyleOfBerSpace()!=ANGLE_SPACE_FLAG::SPACE_UDF)
		return 0;	//����ȡֵ��ͬ
	else
		return m_ciAsynDualEdgeBerSpace;	//��ָ�����߲�߾�ʱ����������ֵ��ͬ��wjh-2018.1.18
}
IMPLEMENT_SET(CDesignLjPartPara,ciAsynDualEdgeWingSpace){m_ciAsynDualEdgeWingSpace=min(value,1);}
IMPLEMENT_GET(CDesignLjPartPara,ciAsynDualEdgeWingSpace){
	if(angle.cbSpaceFlag.StyleOfWingSpace()!=ANGLE_SPACE_FLAG::SPACE_UDF)
		return 0;	//����ȡֵ��ͬ
	else
		return m_ciAsynDualEdgeWingSpace;
}

int CDesignLjPartPara::GetPropValueStr(long id,char* valueStr, UINT nMaxStrBufLen /*= 100*/)
{
	char sText[100]="";
	if(GetPropID("baseInfo")==id)
		strcpy(sText,"");
	else if(GetPropID("iFaceNo")==id)
	{
		if(iFaceNo==3)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"No.3 Plane");
		else if(iFaceNo==2)
			strcpy(sText,"No.2 Plane");
		else
			strcpy(sText,"No.1 Plane");
#else 
			strcpy(sText,"3����");
		else if(iFaceNo==2)
			strcpy(sText,"2����");
		else
			strcpy(sText,"1����");
#endif
	}
	else if(GetPropID("hPart")==id)
		sprintf(sText,"0X%X",hPart);
	else if(GetPropID("start0_end1")==id)
	{
		if(start0_end1==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Start");
		else if(start0_end1==1)
			strcpy(sText,"End");
		else
			strcpy(sText,"Middle");
#else 
			strcpy(sText,"ʼ��");
		else if(start0_end1==1)
			strcpy(sText,"�ն�");
		else
			strcpy(sText,"�м�");
#endif
	}
	else if(GetPropID("ber_space_style")==id)
	{
		if(angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Specify Space");
		else if(angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			strcpy(sText,"1.Standard Space");
		else if(angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_TOEDGE)
			strcpy(sText,"2.Extend To Edge");
		else if(angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_TOBER)
			strcpy(sText,"3.Extend To Edge-Thick");
#else 
			strcpy(sText,"0.ָ���߾�");
		else if(angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			strcpy(sText,"1.��׼�߾�");
		else if(angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_TOEDGE)
			strcpy(sText,"2.��������");
		else if(angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_TOBER)
			strcpy(sText,"3.��������-��");
#endif
	}
	else if(GetPropID("wing_space_style")==id)
	{
		if(angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Specify Space");
		else if(angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			strcpy(sText,"1.Standard Space");
		else if(angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_TOEDGE)
			strcpy(sText,"2.Extend To Edge");
		else if(angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_TOBER)
			strcpy(sText,"3.Extend To Edge-Thick");
#else 
			strcpy(sText,"0.ָ���߾�");
		else if(angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			strcpy(sText,"1.��׼�߾�");
		else if(angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_TOEDGE)
			strcpy(sText,"2.��������");
		else if(angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_TOBER)
			strcpy(sText,"3.��������-��");
#endif
	}
	else if(GetPropID("ber_space")==id)
		sprintf(sText,"%g",ber_space);	//'%g'��ʾ���������β0����Ч���֣�significant digits only)
	else if(GetPropID("angle.cur_wing_x0_y1")==id)
	{
		if(angle.cur_wing_x0_y1==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Leg");
		else
			strcpy(sText,"Y Leg");
#else 
			strcpy(sText,"X֫");
		else
			strcpy(sText,"Y֫");
#endif
	}
	else if(GetPropID("angle.bTwoEdge")==id)
	{
		if(angle.bTwoEdge==TRUE)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("wing_space")==id)
		sprintf(sText,"%g",wing_space);
	else if(GetPropID("end_space")==id)
	{
		sprintf(sText,"%f",end_space);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);

}
#endif