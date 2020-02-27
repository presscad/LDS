// PostProcessReport.cpp: implementation of the CPostProcessReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lds.h"
#include "LDS.h"
#include "PreProcessor.h"
#include "SortFunc.h"
#include "env_def.h"
#include "ArrayList.h"
#include ".\PostProcessReport.h"
#include "..\MainFrm.h"
#include "..\RevisionOperDlg.h"
#include "LogFile.h"
#include "OutputInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef __TIMER_COUNT_
#include "TimerCount.h"
extern CTimerCount timer;
#endif
#if defined(__LDS_)||defined(__TSA_)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DWORD HashStrFunc(const char *key,DWORD nHashSize,BYTE cnMaxKeyBufSize/*=0*/)	//��������Ĺؼ���key������Ӧ��Hashӳ��ֵ
{
	BYTE i;
	DWORD dwkey=0;
	for(i=0;i<16;i+=4)
	{
		DWORD* pdword=(DWORD*)(key+i);
		dwkey+=*pdword;
	}
	return dwkey%nHashSize;
	DWORD keylen=cnMaxKeyBufSize>0?cnMaxKeyBufSize:strlen(key);
	//keyΪ��ʱĬ����"CHashStrTable:NULL-KEY"��Ϊ��ֵ,
	//������������ٴ��ÿ��ַ�������ֵ��ѯʱ������Чֵ wht 16-12-05
	const char *sRealKey=(key!=NULL&&*key!=0)?key:"CHashStrTable:NULL-KEY";
	union KEY_VALUE
	{
		BYTE key[4]; //0.A1 1.A2 2.A3 3.A4
		DWORD dwKey; //A4A3A2A1 �Ӹߵ���
	}key_value,vkey;
	key_value.dwKey=0;
	BYTE iterc=*key;
	for(i=0;i<keylen&&iterc>0;i++)
	{
		BYTE nRemainder=i%3;
		if(iterc>0)
		{
			if(nRemainder==0)
				vkey.dwKey=0;
			vkey.key[nRemainder]=sRealKey[i];
		}
		iterc=*(key+i+1);
		if(nRemainder==2||iterc==0)
			key_value.dwKey+=vkey.dwKey;
	}
	key_value.key[3]=i+1;
	return key_value.dwKey%nHashSize;
}
/** ����֤��õĹ�ϣ�㷨����ֱ������ȡ��
DWORD HashId64Func(XHUINT64 key,DWORD nHashSize)	//��������Ĺؼ���key������Ӧ��Hashӳ��ֵ
{
	if(key<nHashSize)	//����ȱ���޷���64λ������XHUINT64λΪ��ʱ��ʾ��������
		return (DWORD)key;
	else
	{	//��ֹ��XHUINT64����ʱ���縺������ֱ��ȡģ�����쳣����������Ϊ�������m_nHashSize)
		return key%nHashSize;
		DWORD lowid =*((DWORD*)&key);
		DWORD highid=*(((DWORD*)&key)+1);
		return (lowid+highid)%nHashSize;
		//DWORD M1=lowid%m_nHashSize;
		//DWORD M2=highid%m_nHashSize;
		//return (M1+M2)%m_nHashSize;
	}
}*/
XHUINT64 RSLTOBJ_HEADER::GetCompositeId() const
{
	XHUINT64 idid;
	DWORD *pObjId=(DWORD*)&idid;
	DWORD *pObjSubId=pObjId+1;
	*pObjId=id;
	*pObjSubId =_ModuleAndFlag.dwModuleAndFlag<<27;	//���ߺ����Ϊ24,��ռ��λ��5 bits�㹻 0xf8000000
	*pObjSubId|=(m_dwStatusNo<<16)&0x07ff0000;	//������ռ�м��11 bits���ɱ�ʾ2047�ֹ����㹻 0x07ff0000
	if(legword.arrQuad[0]==legword.arrQuad[1]&&legword.arrQuad[0]==legword.arrQuad[2]&&legword.arrQuad[0]==legword.arrQuad[3])
	{
		//*pObjSubId&=0xffff00ff;	//�ȸ����ֻ�(��3byteΪ0),���λ8bit��ʾһ���������
		*pObjSubId|=legword.arrQuad[0];
	}
	else
	{	//�ߵ����ֻ�,�����ֻ�����15������
		*pObjSubId|=(legword.arrQuad[0]&0x0f)<<12;
		*pObjSubId|=(legword.arrQuad[1]&0x0f)<<8;
		*pObjSubId|=(legword.arrQuad[2]&0x0f)<<4;
		*pObjSubId|=(legword.arrQuad[3]&0x0f);
	}
	return idid;
}
void RSLTOBJ_HEADER::FromCompositeId(XHUINT64 idid)
{
	id=*((DWORD*)&idid);
	DWORD subid=*(((DWORD*)&idid)+1);
	m_iModuleNo=(subid&0xf8000000)>>27;	//���ߺ����Ϊ24,��ռ��λ��5 bits�㹻 0xf8000000
	m_dwStatusNo=(subid&0x07ff0000)>>16;//������ռ�м��11 bits���ɱ�ʾ2047�ֹ����㹻 0x07ff0000
	if(subid&0x0000ff00)
	{	//�ߵ����ֻ�,�����ֻ�����15������
		legword.arrQuad[0]=(subid&0xf000)>>12;
		legword.arrQuad[1]=(subid&0x0f00)>>8;
		legword.arrQuad[2]=(subid&0x00f0)>>4;
		legword.arrQuad[3]=(subid&0x0f);
	}
	else	//�ȸ����ֻ�(��3byteΪ0),���λ8bit��ʾһ���������
		legword.arrQuad[0]=legword.arrQuad[1]=legword.arrQuad[2]=legword.arrQuad[3]=subid&0xff;
}
CXhChar50 RSLTOBJ_HEADER::GetCompositeIdString() const
{
	XHUINT64 idid=GetCompositeId();
	DWORD *pId=(DWORD*)&idid;
	char keystr[17]={0};
	sprintf(keystr,"%8X%8X",*pId,*(pId+1));
	//CXhChar50 keystr("%X-%d-%s-%C%C%C%C",id,m_iModuleNo,(char*)dwStatus.ToString(),
	//	quadword.arrQuad[0]+'@',quadword.arrQuad[1]+'@',quadword.arrQuad[2]+'@',quadword.arrQuad[3]+'@');
	return keystr;
}
bool RSLTOBJ_HEADER::ReadKeyId(BUFFER_IO* pBuff,DWORD version)	//����ֵ��Ϣд�뻺��
{
	if(version==0||version>1)
	{
		if(pBuff->GetLength()-pBuff->GetCursorPosition()<8)
			return false;
		XHUINT64 idid;
		pBuff->Read(&idid,8);
		FromCompositeId(idid);
	}
	else 
	{
		if(pBuff->GetLength()-pBuff->GetCursorPosition()<16)
			return false;
		pBuff->ReadDword(&id);
		pBuff->ReadInteger(&_ModuleAndFlag.dwModuleAndFlag);
		pBuff->ReadDword(&m_dwStatusNo);
		pBuff->ReadDword(&legword.dwQuadLeg);
	}
	return true;
}
void RSLTOBJ_HEADER::WriteKeyId(BUFFER_IO* pBuff,DWORD version)	//����ֵ��Ϣд�뻺��
{
	if(version==0||version>1)
	{
		XHUINT64 idid=this->GetCompositeId();
		pBuff->Write(&idid,8);
	}
	else
	{
		pBuff->WriteInteger(id);
		pBuff->WriteInteger(_ModuleAndFlag.dwModuleAndFlag);
		pBuff->WriteDword(m_dwStatusNo);
		pBuff->WriteDword(legword.dwQuadLeg);
	}
}
CListKeyPos::~CListKeyPos()
{
	DeleteList();
}
void CListKeyPos::DeleteList()
{
	KEYPOS_PACK* iter=head.next;
	while(iter!=NULL)
	{
		head.next=iter->next;
		delete iter;
		iter=head.next;
	}
	tail=current=&head;
	ciCurrent=-1;
	wLocalPosition=ciTailCurrent=0;
}
bool CListKeyPos::AppendObject(const RSLTOBJ_KEYPOS& keypos)
{
	if(tail==NULL)
		return false;
	BYTE cnPrevKeySize=0;
	if(ciTailCurrent==KEYPOS_PACK::KEY_COUNT)
	{
		ciTailCurrent=0;
		tail->keys[KEYPOS_PACK::KEY_COUNT-1].cnObjBuffsize=(BYTE)(keypos.dwAddr-tail->dwStartAddr-wLocalPosition);
		tail->next=new KEYPOS_PACK();
		tail=tail->next;
		tail->dwStartAddr=keypos.dwAddr;
		wLocalPosition=cnPrevKeySize=0;
	}
	else if(ciTailCurrent>0)
		tail->keys[ciTailCurrent-1].cnObjBuffsize=cnPrevKeySize=(BYTE)(keypos.dwAddr-tail->dwStartAddr-wLocalPosition);
	else if(tail==&head&&ciTailCurrent==0)
		tail->dwStartAddr=keypos.dwAddr;
	wLocalPosition+=cnPrevKeySize;
	tail->keys[ciTailCurrent].id64=keypos.keyid64;
	ciTailCurrent++;
	return true;
}
bool CListKeyPos::ResetToHead()
{
	current=&head;
	ciCurrent=-1;
	return head.keys[0].id64>0;
}
bool CListKeyPos::MoveNext()
{
	if(ciCurrent==-1)
	{	//�׽ڵ�
		ciCurrent=0;
		CurrentKeyPos.keyid64=head.keys[0].id64;
		CurrentKeyPos.dwAddr=head.dwStartAddr;
		return CurrentKeyPos.keyid64>0;
	}
	else if(current==NULL||CurrentKeyPos.keyid64==0)
		return false;
	DWORD prevkey_addr=CurrentKeyPos.dwAddr;
	BYTE prevsonkey_size=current->keys[ciCurrent].cnObjBuffsize;
	ciCurrent++;
	if(ciCurrent==KEYPOS_PACK::KEY_COUNT)
	{
		ciCurrent=0;
		current=current->next;
	}
	if(current==NULL||current->keys[ciCurrent].id64==0)
		return false;
	CurrentKeyPos.keyid64=current->keys[ciCurrent].id64;
	CurrentKeyPos.dwAddr=(ciCurrent==0)?current->dwStartAddr:prevkey_addr+prevsonkey_size;
	return true;
}
CHashNodeAnalyzeInfo::CHashNodeAnalyzeInfo()
{
	m_dwVersion=3;	//1�汾�ڴ�ռ�ýϴ�
	m_nObjCount=0;
	//hashObjBuffPosition.HashFunc=HashId64Func;
}
CHashElemAnalyzeInfo::CHashElemAnalyzeInfo()
{
	//m_dwVersion=2;//1�汾�ڴ�ռ�ýϴ�
	m_dwVersion=3;	//���2�汾����ط�Ϊʼĩ��//�ڴ�ռ�ýϴ�
	m_nObjCount=0;
	//hashObjBuffPosition.HashFunc=HashId64Func;
}
void CHashNodeAnalyzeInfo::EmptyListAddrTempBuff(){
	m_nObjCount=0;
	listAddrOfObjs.DeleteList();
}
DWORD CHashNodeAnalyzeInfo::AppendNodeRsltObject(CNodeAnalyzeInfo& obj)//���ص�ǰ�����ڻ����ļ��еĴ洢��ʼλ��
{
	RSLTOBJ_HEADER header;
	header.id=obj.m_hNode;
	header.m_iModuleNo=obj.m_iModuleNo;
	header.m_dwStatusNo=obj.m_dwStatusNo;
	header.legword=obj.legword.dwQuadLeg;
	header.dwAddr=GetCursorPosition();
	//TODO:Validate
	AddObject(obj,false);
	listAddrOfObjs.AppendObject(RSLTOBJ_KEYPOS(&header));
	m_nObjCount++;
	return header.dwAddr;
}
bool CHashNodeAnalyzeInfo::RebuildHashTable()
{
	CNodeAnalyzeInfo obj;
	SeekPosition(0);
	hashObjBuffPosition.Empty();
	DWORD nodecount=GetObjectNum();
	DWORD hashsize=nodecount+(nodecount>>4);	//��Ϊ���6.25%,���򵱽ڵ�����ʱ���п�����Ϊ��ϣ�Ĳ���ϡ�裬���·�����������ܶ�ڵ�
	if(!hashObjBuffPosition.CreateHashTable(hashsize))
	{
		logerr.Log("CHashNodeAnalyzeInfo::RebuildHashTable() failed! elem count=%d",GetObjectNum());
		return false;
	}
	UINT hits=0;
	listAddrOfObjs.ResetToHead();
	while(listAddrOfObjs.MoveNext())
	{
		RSLTOBJ_KEYPOS* pRsltObj=&listAddrOfObjs.CurrentKeyPos;
		hashObjBuffPosition.SetValueAt(pRsltObj->keyid64,pRsltObj->dwAddr);
		hits++;
	}
	if(hits!=m_nObjCount&&PRODUCT_FUNC::IsHasInternalTest())
		logerr.Log("RsltNode Counts error hits%d!=count%d",hits,m_nObjCount);
#ifdef __TIMER_COUNT_
	DWORD idle_count=0,extra_count=0;
	int depth=hashObjBuffPosition.GetMaxDepth(&idle_count,&extra_count);
	logerr.Log("RsltNode depth= %d,idle_count=%d,extra_count=%d",depth,idle_count,extra_count);
#endif
	return true;
}
DWORD CHashNodeAnalyzeInfo::WriteBuffer(BUFFER_IO *pIO,DWORD size,char* pool/*=NULL*/,DWORD pool_size/*=0*/)
{	//��IO��ǰλ�ÿ�ʼ��ȡ�����¼
	EmptyListAddrTempBuff();
	char block[1024];
	DWORD nread=0,nwritten=0;
	if(pool==NULL)
	{
		pool=block;
		pool_size=1024;
	}
	nread=m_nSizeOfObj;//min(size,pool_size);

	CNodeAnalyzeInfo obj;
	hashObjBuffPosition.Empty();
	DWORD nodecount=size/m_nSizeOfObj;
	DWORD hashsize=nodecount+(nodecount>>4);	//��Ϊ���6.25%,���򵱽ڵ�����ʱ���п�����Ϊ��ϣ�Ĳ���ϡ�裬���·�����������ܶ�ڵ�
	hashObjBuffPosition.CreateHashTable(hashsize);
	while(nread>0&&(nread=pIO->Read(pool,nread))>0)
	{
		if(obj.ReadObjectKey(&CBuffer(pool,nread),m_dwVersion))
		{
			DWORD position=GetCursorPosition();
			hashObjBuffPosition.SetValueAt(obj.GetCompositeKey(),position);
		}
		nwritten+=Write(pool,nread);
		nread=min(size-nwritten,(WORD)m_nSizeOfObj);
	}
	return nwritten;
}
DWORD CHashElemAnalyzeInfo::WriteBuffer(BUFFER_IO *pIO,DWORD size,char* pool/*=NULL*/,DWORD pool_size/*=0*/)
{	//��IO��ǰλ�ÿ�ʼ��ȡ�����¼
	EmptyListAddrTempBuff();
	char block[1024];
	DWORD nread=0,nwritten=0;
	if(pool==NULL)
	{
		pool=block;
		pool_size=1024;
	}
	nread=m_nSizeOfObj;//min(size,pool_size);

	ELEM_ANALYZE_INFO obj;
	hashObjBuffPosition.Empty();
	DWORD nodecount=size/m_nSizeOfObj;
	DWORD hashsize=nodecount+(nodecount>>4);	//��Ϊ���6.25%,���򵱽ڵ�����ʱ���п�����Ϊ��ϣ�Ĳ���ϡ�裬���·�����������ܶ�ڵ�
	hashObjBuffPosition.CreateHashTable(hashsize);
	while(nread>0&&(nread=pIO->Read(pool,nread))>0)
	{
		if(obj.ReadObjectKey(&CBuffer(pool,nread),m_dwVersion))
		{
			DWORD position=GetCursorPosition();
			hashObjBuffPosition.SetValueAt(obj.GetCompositeKey(),position);
		}
		nwritten+=Write(pool,nread);
		nread=min(size-nwritten,(WORD)m_nSizeOfObj);
	}
	return nwritten;
}
void CHashElemAnalyzeInfo::EmptyListAddrTempBuff(){
	m_nObjCount=0;
	listAddrOfObjs.DeleteList();
}
DWORD CHashElemAnalyzeInfo::AppendElemRsltObject(ELEM_ANALYZE_INFO& obj)//���ص�ǰ�����ڻ����ļ��еĴ洢��ʼλ��
{
	RSLTOBJ_HEADER header;
	header.id=obj.idElem;
	header.m_iModuleNo=obj.m_iModuleNo;
	header.m_dwStatusNo=obj.m_dwStatusNo;
	header.legword=obj.legword.dwQuadLeg;
	header.dwAddr=GetCursorPosition();
	AddObject(obj,false);
	listAddrOfObjs.AppendObject(RSLTOBJ_KEYPOS(&header));
	m_nObjCount++;
	return header.dwAddr;
}
bool CHashElemAnalyzeInfo ::RebuildHashTable()
{
	ELEM_ANALYZE_INFO obj;
	SeekPosition(0);
	hashObjBuffPosition.Empty();
	DWORD nodecount=GetObjectNum();
	DWORD hashsize=nodecount+(nodecount>>4);	//��Ϊ���6.25%,���򵱽ڵ�����ʱ���п�����Ϊ��ϣ�Ĳ���ϡ�裬���·�����������ܶ�ڵ�
	if(!hashObjBuffPosition.CreateHashTable(hashsize))
	{
		logerr.Log("CHashElemAnalyzeInfo::RebuildHashTable() failed! elem count=%d",GetObjectNum());
		return false;
	}
	listAddrOfObjs.ResetToHead();
	UINT hits=0;
	while(listAddrOfObjs.MoveNext())
	{
		RSLTOBJ_KEYPOS* pRsltObj=&listAddrOfObjs.CurrentKeyPos;
		hashObjBuffPosition.SetValueAt(pRsltObj->keyid64,pRsltObj->dwAddr);
		hits++;
	}
	if(hits!=m_nObjCount&&PRODUCT_FUNC::IsHasInternalTest())
		logerr.Log("RsltElem Counts error hits%d!=count%d",hits,m_nObjCount);
#ifdef __TIMER_COUNT_
	DWORD idle_count=0,extra_count=0;
	int depth=hashObjBuffPosition.GetMaxDepth(&idle_count,&extra_count);
	logerr.Log("RsltElem depth= %d,idle_count=%d,extra_count=%d",depth,idle_count,extra_count);
#endif
	return true;
}
//////////////////////////////////////////////////////////////////////
// CPostProcessReport �� Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPostProcessReport postreport;
UINT CNodeAnalyzeInfo::KeyHeadInfoSize(DWORD version)
{
	if(version==0||version>=2)
		return 8;
	else
		return 16;
}	//Read/WriteObjectKey��д�Ļ����С
UINT CNodeAnalyzeInfo::BufferSize(DWORD version)
{
	if(version==0||version>=2)
		return 65;
	else
		return 137;
}
UINT CNodeAnalyzeInfo::GetRsltObjBuffSize(DWORD version)
{
	if(version==0||version>=2)
		return 65;
	else
		return 137;
}
void CNodeAnalyzeInfo::ToBuffer(BUFFER_IO* pBuff,DWORD version)		//������ֵ��Ϣ
{
	if(version==0||version>1)
	{
		ToThinBuffer(pBuff);
		return;
	}
	pBuff->WriteBooleanThin(m_bFemNode);
	pBuff->WritePoint(offset);
	//pBuff->WritePoint(rot_offset);			//�ڵ��ƫ����
	//pBuff->WritePoint(internal_force);
	pBuff->WritePoint(external_force);
	pBuff->WritePoint(unbalance_force);
	pBuff->WritePoint(internal_moment);
	pBuff->WritePoint(windload);
}
void CNodeAnalyzeInfo::FromBuffer(BUFFER_IO* pBuff,DWORD version)	//������ֵ��Ϣ
{
	if(version==0||version>1)
	{
		FromThinBuffer(pBuff);
		return;
	}
	pBuff->ReadBooleanThin(&m_bFemNode);
	pBuff->ReadPoint(offset);
	//pBuff->ReadPoint(rot_offset);			//�ڵ��ƫ����
	//pBuff->ReadPoint(internal_force);
	pBuff->ReadPoint(external_force);
	pBuff->ReadPoint(unbalance_force);
	pBuff->ReadPoint(internal_moment);
	pBuff->ReadPoint(windload);
}
void CNodeAnalyzeInfo::ToThinBuffer(BUFFER_IO* pBuff)		//������ֵ��Ϣ
{	//��д��57 Bytes
	pBuff->WriteBooleanThin(m_bFemNode);
	pBuff->WriteThinPoint(offset);	//Write 12 Bytes
	pBuff->WriteThinPoint(external_force);
	pBuff->WriteThinPoint(unbalance_force);
	pBuff->WriteThinPoint(internal_moment);
	pBuff->WriteFloat(windload.x);
	pBuff->WriteFloat(windload.y);
}
void CNodeAnalyzeInfo::FromThinBuffer(BUFFER_IO* pBuff)	//������ֵ��Ϣ
{	//������57 Bytes
	pBuff->ReadBooleanThin(&m_bFemNode);
	pBuff->ReadThinPoint(offset);
	pBuff->ReadThinPoint(external_force);
	pBuff->ReadThinPoint(unbalance_force);
	pBuff->ReadThinPoint(internal_moment);
	pBuff->ReadFloat(&windload.x);
	pBuff->ReadFloat(&windload.y);
}
CXhChar50 CNodeAnalyzeInfo::GetKeyString() const
{
	return GetCompositeIdString();
}
bool CNodeAnalyzeInfo::ReadObject(BUFFER_IO* pBuff,DWORD version)
{
	if(pBuff->GetLength()-pBuff->GetCursorPosition()<BufferSize(version))
		return false;
	if(!ReadObjectKey(pBuff,version))
		return false;
	FromBuffer(pBuff,version);
	return true;
}
bool CNodeAnalyzeInfo::WriteObject(BUFFER_IO* pBuff,DWORD version)
{
	WriteObjectKey(pBuff,version);
	ToBuffer(pBuff,version);
	return true;
}
UINT ELEM_ANALYZE_INFO::KeyHeadInfoSize(DWORD version)
{
	if(version==0||version>=2)
		return 8;
	else
		return 16;
}	//Read/WriteObjectKey��д�Ļ����С
UINT ELEM_ANALYZE_INFO::BufferSize(DWORD version)
{
	if(version==0||version>=3)
		return 52;
	else if(version==2)
		return 40;
	else
		return 68;
}
UINT ELEM_ANALYZE_INFO::GetRsltObjBuffSize(DWORD version)
{
	if(version==0||version>=3)
		return 52;
	else if(version==2)
		return 40;
	else
		return 68;
}
void ELEM_ANALYZE_INFO::ToBuffer(BUFFER_IO* pBuff,DWORD version)		//������ֵ��Ϣ
{
	if(version==0||version>1)
	{
		pBuff->WriteInteger(iStartI);
		pBuff->WriteInteger(iEndI);
		pBuff->WriteInteger(m_hParentRod);
		pBuff->WriteFloat(weight);
		pBuff->WriteFloat(N);
		if(version==2)
			pBuff->WriteThinPoint(moment);	//��Ԫ���
		else
		{
			pBuff->WriteThinPoint(VFS);
			pBuff->WriteThinPoint(VFE);
		}
		return;
	}
	pBuff->WriteInteger(iStartI);
	pBuff->WriteInteger(iEndI);
	pBuff->WriteInteger(m_hParentRod);
	pBuff->WriteDouble(weight);
	pBuff->WriteDouble(N);
	pBuff->WritePoint(moment);	//��Ԫ���
}
void ELEM_ANALYZE_INFO::FromBuffer(BUFFER_IO* pBuff,DWORD version)	//������ֵ��Ϣ
{
	if(version==0||version>1)
	{
		pBuff->ReadInteger(&iStartI);
		pBuff->ReadInteger(&iEndI);
		pBuff->ReadInteger(&m_hParentRod);
		pBuff->ReadFloat(&weight);
		pBuff->ReadFloat(&N);
		if(version==2)
			pBuff->ReadThinPoint(moment);	//��Ԫ�����
		else
		{
			pBuff->ReadThinPoint(VFS);
			pBuff->ReadThinPoint(VFE);
			if(VFS.IsZero())
				moment= VFE;
			else if(VFE.IsZero())
				moment=-VFS;
			else if(VFE.mod2()>VFS.mod2())
				moment= VFE;
			else
				moment=-VFS;
		}
		return;
	}
	pBuff->ReadInteger(&iStartI);
	pBuff->ReadInteger(&iEndI);
	pBuff->ReadInteger(&m_hParentRod);
	pBuff->ReadDouble(&weight);
	pBuff->ReadDouble(&N);
	pBuff->ReadPoint(moment);			//��Ԫ�����
}
CXhChar50 ELEM_ANALYZE_INFO::GetKeyString() const
{
	return GetCompositeIdString();
}
bool ELEM_ANALYZE_INFO::ReadObject(BUFFER_IO* pBuff,DWORD version)
{
	if(pBuff->GetLength()-pBuff->GetCursorPosition()<BufferSize(version))
		return false;
	if(!ReadObjectKey(pBuff,version))
		return false;
	FromBuffer(pBuff,version);
	return true;
}
bool ELEM_ANALYZE_INFO::WriteObject(BUFFER_IO* pBuff,DWORD version)
{
	WriteObjectKey(pBuff,version);
	ToBuffer(pBuff,version);
	return true;
}

CPostProcessReport::CPostProcessReport()
{
	m_bHasReportData=false;
	m_bHasStdFoundationForce=false;		//�Ƿ����������������׼ֵ��Ϣ
	m_sRsltItem.Copy("***");
}
bool CPostProcessReport::InitProcTempFile(char* filename,DWORD dwRsltObjBuffVersion/*=0*/)
{
	hashNodeAnalBuffer.EmptyListAddrTempBuff();
	hashElemAnalBuffer.EmptyListAddrTempBuff();
	hashNodeAnalBuffer.InitBuffer(CXhChar200("%s_node.tmp",filename),1024,CNodeAnalyzeInfo::BufferSize(dwRsltObjBuffVersion),dwRsltObjBuffVersion);
	hashElemAnalBuffer.InitBuffer(CXhChar200("%s_elem.tmp",filename),1024,ELEM_ANALYZE_INFO::BufferSize(dwRsltObjBuffVersion),dwRsltObjBuffVersion);
	return 0;
}

void CPostProcessReport::ClearAnalInfoBuffer()
{
	hashNodeAnalBuffer.ClearContents();
	hashElemAnalBuffer.ClearContents();
}
void CPostProcessReport::AppendToAnalyzeBuffer(CNodeAnalyzeInfo& xNodeAnalyzeInfo)
{
	DWORD dwAddr=hashNodeAnalBuffer.AppendNodeRsltObject(xNodeAnalyzeInfo);
}
void CPostProcessReport::AppendToAnalyzeBuffer(ELEM_ANALYZE_INFO& xElemAnalyzeInfo)
{
	DWORD dwAddr=hashElemAnalBuffer.AppendElemRsltObject(xElemAnalyzeInfo);
}

void CPostProcessReport::Empty()
{
	m_sRsltItem.Copy("***");
	m_bHasReportData=false;
	hashNodes.Empty();
	Reaction.Empty();	//������������,	ֻ�Ǽ�����ʱ�洢,��������������
	TowerWeight.Empty();	//����������(kg),ֻ�Ǽ�����ʱ�洢,��������������
	hashElems.Empty();	// Ӧ������ʱ�����˴�����ȫ���˼���һ����������ڵļ��ϣ�
	ElemPtrs.Empty();
}
 
//from AnalyzeProcessor.cpp
int compare_func1(const NODE_PTR& pNode1,const NODE_PTR& pNode2);
int compare_func2(const ELEM_PTR &pElem1,const ELEM_PTR &pElem2);
void CPostProcessReport::CreateProcessReport()
{
	DisplayPrompt("���ڻ��ܼ�����!");
	Empty();
#ifdef __TIMER_COUNT_
	DWORD rsltnode_count=hashNodeAnalBuffer.GetLength()/CNodeAnalyzeInfo::BufferSize(0);
	DWORD rsltelem_count=hashElemAnalBuffer.GetLength()/ELEM_ANALYZE_INFO::BufferSize(0);
	DWORD reaction_count=postprocessor.Reaction.GetNodeNum();
	logerr.Log("\nrsltnoderec_count=%d,rsltelem_countrec=%d,reactionrec_count=%d",rsltnode_count,rsltelem_count,reaction_count);
	timer.Relay();
#endif
	hashNodeAnalBuffer.RebuildHashTable();
	hashElemAnalBuffer.RebuildHashTable();
#ifdef __TIMER_COUNT_
	timer.Relay(101);
#endif
	//hashNodeAnalBuffer.EmptyListAddrTempBuff();
	//hashElemAnalBuffer.EmptyListAddrTempBuff();
	m_bHasReportData=true;
	CString product_version;
	theApp.GetProductVersion(product_version);
	m_sProductVersion.Copy(product_version);
	if(g_sysPara.AnalysisPara.m_iNonlinearStyle==0)
		m_bLinearAnalysis=true;
	else
		m_bLinearAnalysis=false;
	m_bHasStdFoundationForce=g_sysPara.bCalStdFoundationForce==TRUE;
	m_iMemberSizeCheckCode=Ta.m_iMemberSizeCheckCode;
	//��������ڵ�ͳ��
	ARRAY_LIST<NODE_PTR>node_arr;
	node_arr.SetSize(0,1000);
	for(CLDSNode *pNode=preprocessor.result.allnodeset.GetFirst();pNode;pNode=preprocessor.result.allnodeset.GetNext())
	{
		if(pNode->Layer(0)!='L'&&!pNode->cfgword.And(preprocessor.result.cfgwordBody))
			continue;
		if(pNode->Layer(0)=='L'&&!pNode->cfgword.And(preprocessor.result.cfgwordLeg))
			continue;
		node_arr.append(pNode);
	}
	CQuickSort<NODE_PTR>::QuickSort(node_arr.m_pData,node_arr.GetSize(),compare_func1);
	for(int i=0;i<node_arr.GetSize();i++)
	{
		POSTRPT_NODE *pRptNode=hashNodes.Add(node_arr[i]->handle);
		pRptNode->pNode=Ta.Node.FromHandle(node_arr[i]->handle);
		pRptNode->bFatherNode=true;
		if(node_arr[i]->IsFatherObject())	//���ڵ�
		{
			long hMirX=node_arr[i]->GetRelativeMirObj(MIRMSG(1),FALSE);
			long hMirY=node_arr[i]->GetRelativeMirObj(MIRMSG(2),FALSE);
			long hMirZ=node_arr[i]->GetRelativeMirObj(MIRMSG(3),FALSE);
			if(hMirX>0x20&&hMirX!=node_arr[i]->handle)
				pRptNode->symmetry.Copy(" X");
			else
				pRptNode->symmetry.Copy("  ");
			if(hMirY>0x20&&hMirY!=node_arr[i]->handle)
				pRptNode->symmetry.Append("Y");
			else
				pRptNode->symmetry.Append(" ");
			if(hMirZ>0x20&&hMirZ!=hMirX&&hMirZ!=hMirY&&hMirZ!=node_arr[i]->handle)
				pRptNode->symmetry.Append("Z");
			else
				pRptNode->symmetry.Append(" ");
			if(stricmp(pRptNode->symmetry,"    ")==0)
				pRptNode->symmetry.Copy("  ��");
		}
		else
		{
			RELATIVE_OBJECT relaObj;
			if(node_arr[i]->GetFatherMirObj(&relaObj))
			{
				if(relaObj.mirInfo.axis_flag==1||(fabs(node_arr[i]->Position(false).x)<EPS&&relaObj.mirInfo.axis_flag==4))
					pRptNode->symmetry.Copy("-X  ");
				else if(relaObj.mirInfo.axis_flag==2||(fabs(node_arr[i]->Position(false).y)<EPS&&relaObj.mirInfo.axis_flag==4))
					pRptNode->symmetry.Copy(" -Y ");
				else if(relaObj.mirInfo.axis_flag==4)
					pRptNode->symmetry.Copy("  -Z");
				else
					pRptNode->symmetry.Copy("  ��");
				if(g_sysPara.bMergeRptFileMirObj)
				{
					pRptNode->bFatherNode=false;
					continue;	//�ϲ������Գ���ʱ���ӶԳ����ʾ���
				}
			}
			else
				pRptNode->symmetry.Copy("  ��");
		}
	}
	for(BASENODE_REACTION *pSrcReact=postprocessor.Reaction.GetFirst();pSrcReact;pSrcReact=postprocessor.Reaction.GetNext())
	{
		BASENODE_REACTION *pReact=Reaction.Add(postprocessor.Reaction.GetCursorKey());
		*pReact=*pSrcReact;
	}
	for(TOWERWEIGHT *pTowerWeight=postprocessor.TowerWeight.GetFirst();pTowerWeight;pTowerWeight=postprocessor.TowerWeight.GetNext())
	{
		TOWERWEIGHT *pWeight=TowerWeight.Add(postprocessor.TowerWeight.GetCursorKey());
		*pWeight=*pTowerWeight;
	}
	for(CElement *pSrcElem=postprocessor.hashElems.GetFirst();pSrcElem;pSrcElem=postprocessor.hashElems.GetNext())
	{
		CElement *pElem=hashElems.Add(pSrcElem->id);
		*pElem=*pSrcElem;
		//TODO:��Ҫ��ϸ����_lamdaRc�Ƿ�����
		pElem->_lamdaRc=pElem->LamdaRc();
		ElemPtrs.SetValue(pElem->pStart->handle,pElem->pEnd->handle,pElem);
		//������ѡ��ʱ����ʱ���ڴ�ָ�����CPartSizeOptimizor���ٶ����٣���˱��븳�գ�����ᵼ�������쳣
		pElem->pParaType=NULL;
	}
}
const UINT CPostProcessReport::CURR_RSLTOBJBUFF_VERSION = 3;
CPostProcessReport::~CPostProcessReport()
{
	Empty();
}
void CPostProcessReport::FromBuffer(CBuffer &buffer)
{
	long h;
	int i,n,key,version;
	Empty();
	buffer.ReadInteger(&version);	//��ʼ�汾��
	buffer.ReadString(m_sProductVersion);
	buffer.ReadBooleanThin(&m_bLinearAnalysis);
	buffer.ReadBooleanThin(&m_bHasStdFoundationForce);
	buffer.ReadInteger(&m_iMemberSizeCheckCode);
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&key);
		POSTRPT_NODE *pRptNode=hashNodes.Add(key);
		buffer.ReadBooleanThin(&pRptNode->bFatherNode);
		buffer.ReadString(pRptNode->symmetry);
		pRptNode->pNode=Ta.Node.FromHandle(key);
	}
	buffer.ReadInteger(&n);
	Reaction.SetHashTableGrowSize(n);
	for(i=0;i<n;i++)
	{
		char str_key[100]="";
		buffer.ReadString(str_key,100);
		BASENODE_REACTION *pReact=Reaction.Add(str_key);
		buffer.ReadInteger(&h);pReact->pNode=Ta.Node.FromHandle(h);
		if(pReact->pNode==NULL)
			logerr.Log("�����������й����Ľڵ�0X%X,�Ѳ�����!",h);
		buffer.ReadInteger(&h);pReact->pModule=Ta.Module.FromHandle(h);
		buffer.ReadInteger(&h);pReact->pStatus=Ta.WorkStatus.FromHandle(h);
		if(pReact->pModule==NULL)
			logerr.Log("�����������й�����ĳ������,�Ѳ�����!");
		if(pReact->pStatus==NULL)
			logerr.Log("�����������й�����ĳ������,�Ѳ�����!");
		buffer.Read(pReact->leg_no,4*sizeof(WORD));
		buffer.ReadInteger(&pReact->status);
		buffer.ReadPoint(pReact->reaction_force_design);
		buffer.ReadPoint(pReact->reaction_force_std);
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&key);
		TOWERWEIGHT *pTowerWeight=TowerWeight.Add(key);
		buffer.ReadInteger(&pTowerWeight->cfg_no);
		buffer.ReadInteger(&pTowerWeight->iBody0Leg1);
		buffer.ReadDouble(&pTowerWeight->weight);
		if(version>=6)
			buffer.ReadDouble(&pTowerWeight->netWeight);
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&key);	//��Ԫ��ʶ����ǰ�����������ɣ�һֱ�ᴩʼ��
		CElement *pElem=hashElems.Add(key);
		//FEM_ELEMENT *pFemElem;
		pElem->id=key;
		//CFGWORD cfgword;
		buffer.ReadInteger(&h);pElem->pStart=Ta.Node.FromHandle(h);
		buffer.ReadInteger(&h);pElem->pEnd=Ta.Node.FromHandle(h);
		ElemPtrs.SetValue(pElem->pStart->handle,pElem->pEnd->handle,pElem);
		pElem->CalLength();
		buffer.ReadInteger(&h);pElem->pOrgPole=(CLDSLinePart*)Ta.Parts.FromHandle(h,CLS_LINEPART);
		buffer.ReadBooleanThin(&pElem->relaElem.bFatherElem);
		buffer.ReadInteger(&pElem->relaElem.idRefElem);	//����б�Ŀ���ͬʱ��ѹ���յ�Ԫ
		buffer.ReadInteger(&pElem->relaElem.idRefElem2);//����ͬ�˽ڵ�ͬʱ��ѹ���յ�Ԫ
		buffer.ReadInteger(&pElem->relaElem.idElemMirX);//�˵�Ԫ�Ƿ�Ϊ�Գƹ�����Ԫ�еĸ���Ԫ�����ڵ���ۺϽ�С�ĵ�Ԫ
		buffer.ReadInteger(&pElem->relaElem.idElemMirY);
		buffer.ReadInteger(&pElem->relaElem.idElemMirZ);
		//int BearTens1Comp2;	//1:�����˳�ѹ;2:��ѹ�˳���;����Ϊ�����ˡ�1��2Ϊ��ˣ��������Ϊ���
		//void* pParaType;
		//buffer.WriteDouble(pElem->N);				//����(N)
		buffer.ReadDouble(&pElem->T0);				//ê�̶�Ԥ����(N)
		//double W0,H0,V0;		//����Ԥ�Ž�����������(N)�����ɶ���Ԥ���������²���������ˮƽ��������ֱ����
		//bool bAnchorEnd;		//true:�����ն˶Ե�ê��;false:����ʼ�˶Ե�ê��
		//buffer.WritePoint(pElem->moment);		//��Ԫ������(N*mm)
		//short elem_type;	//0:��ʾ�����˻򾭵���;1:��ʾ������Ԫ
		//��С����㳤����Ϣ
		buffer.ReadInteger(&h);pElem->lamdaRyo.pStart=Ta.Node.FromHandle(h);	//���㳤����ʼ��
		buffer.ReadInteger(&h);pElem->lamdaRyo.pEnd=Ta.Node.FromHandle(h);		//���㳤����ֹ��
		buffer.ReadDouble(&pElem->lamdaRyo.muCoefBasic);	//���㳤���൱ϵ��
		buffer.ReadDouble(&pElem->lamdaRyo.L);		//���㳤��
		buffer.ReadDouble(&pElem->lamdaRyo.R);		//ִ�л�ת�뾶
		buffer.ReadDouble(&pElem->lamdaRyo.Lo_r);	//���㳤ϸ��
		buffer.ReadString(pElem->lamdaRyo.braceStr);//��ϸ�ȼ�������ַ���
		//��ͨ�Ļ򽻲�б��һ��һѹʱƽ���᳤ϸ�ȼ�����Ϣ
		buffer.ReadInteger(&h);pElem->lamdaRx.pStart=Ta.Node.FromHandle(h);	//���㳤����ʼ��
		buffer.ReadInteger(&h);pElem->lamdaRx.pEnd=Ta.Node.FromHandle(h);		//���㳤����ֹ��
		buffer.ReadDouble(&pElem->lamdaRx.muCoefBasic);	//���㳤���൱ϵ��
		buffer.ReadDouble(&pElem->lamdaRx.L);		//���㳤��
		buffer.ReadDouble(&pElem->lamdaRx.R);		//ִ�л�ת�뾶
		buffer.ReadDouble(&pElem->lamdaRx.Lo_r);	//���㳤ϸ��
		buffer.ReadString(pElem->lamdaRx.braceStr);//��ϸ�ȼ�������ַ���
		//�����ͬʱ��ѹʱƽ������㳤����Ϣ
		buffer.ReadInteger(&h);pElem->_lamdaRc.pStart=Ta.Node.FromHandle(h);	//���㳤����ʼ��
		buffer.ReadInteger(&h);pElem->_lamdaRc.pEnd=Ta.Node.FromHandle(h);		//���㳤����ֹ��
		buffer.ReadDouble(&pElem->_lamdaRc.muCoefBasic);	//���㳤���൱ϵ����̬����ֵ����
		if(version>=2)	//���㳤���൱ϵ����ֵ̬����
			buffer.ReadDouble(&pElem->_lamdaRc.muCoefLive);
		else
			pElem->_lamdaRc.muCoefLive=1.0;
		buffer.ReadDouble(&pElem->_lamdaRc.L);		//���㳤��
		buffer.ReadDouble(&pElem->_lamdaRc.R);		//ִ�л�ת�뾶
		buffer.ReadDouble(&pElem->_lamdaRc.Lo_r);	//���㳤ϸ��
		buffer.ReadString(pElem->_lamdaRc.braceStr);//��ϸ�ȼ�������ַ���
		//m_fStartBraceNCoef;	//������ʼ�˶������ĵ�֧����ϵ��
		//m_fEndBraceNCoef;	//�������ն˶������ĵ�֧����ϵ��
		buffer.ReadDouble(&pElem->m_fWeightRaiseCoef);	//������������ϵ��
		buffer.ReadInteger(&pElem->m_iCtrlRule);
		//int iSizeIndex;		//���ڰ��չ������ȽϹ���С
		buffer.ReadInteger(&pElem->m_bValidStartRestStiff);
		buffer.ReadInteger(&pElem->m_bValidEndRestStiff);	//�Ƿ��г��ܲ���ת��Լ��������
	//xx_joint_type��������ԣ���real_xx_joint_type�����ʵ�ʼ���ʱ��֧������
	//0����ȫ�½�JOINT_HINGE��1�����ָս�JOINT_RIGID�����Ƴ�Ťת���ת��Լ����,2���Զ��ж�JOINT_AUTOJUSTIFY
		JOINT_TYPE joint_type_arr[]={JOINT_HINGE,JOINT_RIGID,JOINT_AUTOJUSTIFY};
		FORCE_TYPE force_type_arr[]={CENTRIC_FORCE,ECCENTRIC_FORCE};
		int joint_type,force_type;
		buffer.ReadInteger(&joint_type);	pElem->start_joint_type=joint_type_arr[joint_type];
		buffer.ReadInteger(&joint_type);	pElem->end_joint_type=joint_type_arr[joint_type];	//JOINT_TYPE
		buffer.ReadInteger(&joint_type);	pElem->real_start_joint_type=joint_type_arr[joint_type];
		buffer.ReadInteger(&joint_type);	pElem->real_end_joint_type=joint_type_arr[joint_type];	//0����ȫ�½�JOINT_HINGE1�����ָս�JOINT_RIGID�����Ƴ�Ťת���ת��Լ����
		buffer.ReadInteger(&force_type);	pElem->start_force_type=force_type_arr[force_type];
		buffer.ReadInteger(&force_type);	pElem->end_force_type=force_type_arr[force_type];	//	//�Ǹ���������״��CENTRIC_FORCE,ECCENTRIC_FORCE
	//д�������Ϣdesign_info;
		buffer.ReadDouble(&pElem->design_info.hole_out);	//������
		buffer.ReadDouble(&pElem->design_info.hole_d);		//�׾�
		buffer.ReadDouble(&pElem->design_info.w_t);
		buffer.ReadDouble(&pElem->design_info.fCoefTensStrengthDecrease);
		buffer.ReadDouble(&pElem->design_info.fCoefCompStrengthDecrease);
		buffer.ReadDouble(&pElem->design_info.fCoefStablity);
		buffer.ReadDouble(&pElem->design_info.fCoefEffic);		//���ƹ��������ָ��ռָ�������޵ı���ϵ�����޿��ƹ���������Ϊ��
		buffer.ReadDouble(&pElem->design_info.fCoefOfBearLoad);	//���ƹ����³��ܺ���ռ���ؼ��޵ı���ϵ�����޿��ƹ���������Ϊ��
		buffer.ReadInteger(&pElem->design_info.iDangerousRule);	//Σ�տ��ƹ���
		buffer.ReadInteger(&pElem->design_info.iDangerousLoadCase);	//��Σ�յĳ���ָ��
		if(version>=2)
			buffer.ReadBooleanThin(&pElem->design_info.bBlockShearActivated);
		else
			pElem->design_info.bBlockShearActivated=false;
		buffer.ReadDouble(&pElem->design_info.mN);				//ѹ���ȶ�ǿ���ۼ�ϵ��
		buffer.ReadString(pElem->design_info.LoExpression);
		buffer.ReadString(pElem->design_info.lamdaExpression);		//��ϸ�ȼ�����ʽ
		buffer.ReadInteger(&pElem->design_info.bStartRotRest);
		buffer.ReadInteger(&pElem->design_info.bEndRotRest);
		buffer.ReadByte(&pElem->design_info.lamdaSelType);	// 0:lamdaRyo����;1:lamdaRx����;2:lamdaRx2����
		buffer.ReadByte(&pElem->design_info.cLamdaTypeR);	//0:Rx;1:Ry0
		buffer.ReadDouble(&pElem->design_info.lamdaL);
		buffer.ReadDouble(&pElem->design_info.lamdaR);
		buffer.ReadDouble(&pElem->design_info.Lo_r);		//���㳤��,��ת�뾶,ʵ�ʳ�ϸ��
		buffer.ReadDouble(&pElem->design_info.UltimateLo_r);//�������ϸ��
		//д��ELEMFORCE_ENV MaxTension,MaxCompression,MaxMoment;
		buffer.ReadInteger(&pElem->MaxTension.hModule);
		buffer.ReadInteger((int*)&pElem->MaxTension.hWorkStatus);
		buffer.Read(pElem->MaxTension.leg_no,sizeof(long)*4);
		buffer.ReadDouble(&pElem->MaxTension.N);
		buffer.ReadInteger(&pElem->MaxCompression.hModule);
		buffer.ReadInteger((int*)&pElem->MaxCompression.hWorkStatus);
		buffer.Read(pElem->MaxCompression.leg_no,sizeof(long)*4);
		buffer.ReadDouble(&pElem->MaxCompression.N);
		buffer.ReadInteger(&pElem->MaxMoment.hModule);
		buffer.ReadInteger((int*)&pElem->MaxMoment.hWorkStatus);
		buffer.Read(pElem->MaxMoment.leg_no,sizeof(long)*4);
		buffer.ReadDouble(&pElem->MaxMoment.N);
		//д��ELEMCOMPSTABLE_ENV MaxCompStable;			//ѹ�ȿ���ʱ�����ѹ�ȼ���Ӧ��
		if(version>=4)
			buffer.ReadBooleanThin(&pElem->smcinfo.hasBothComp);
		buffer.ReadInteger(&pElem->MaxCompStable.hModule);	//ģ�;��
		buffer.ReadInteger((int*)&pElem->MaxCompStable.hWorkStatus);//�������
		buffer.Read(pElem->MaxCompStable.leg_no,sizeof(long)*4);		//�����޽��Ⱥ�
		buffer.ReadDouble(&pElem->MaxCompStable.N);		// (N)
		buffer.ReadBooleanThin(&pElem->MaxCompStable.bothComp);	// true:�����ͬʱ��ѹ����;false:�ǽ���Ļ򽻲��һ��һѹ��������
		buffer.ReadDouble(&pElem->MaxCompStable.sigema);	// N/(A*��),(MPa)
		if(version<2)
		{
			double fai;
			buffer.ReadDouble(&fai);		// �������ѹ��ϵ��pElem->MaxCompStable.fai
		}
	//���в�����ͬʱ��ѹ�ĸ˼���Ԫ����Ϣ��һ��һѹ�洢
		buffer.ReadDouble(&pElem->MaxCompStable.Afai_bothcomp);
		buffer.ReadDouble(&pElem->MaxCompStable.Afai_tenscomp);
		buffer.ReadDouble(&pElem->MaxCompStable.fai_bothcomp);
		buffer.ReadDouble(&pElem->MaxCompStable.fai_tenscomp);	//ͬʱ��ѹ��һ��һѹʱ��ѹ��ϵ��
		buffer.ReadByte(&pElem->MaxCompStable.lamdatype_bothcomp);
		buffer.ReadByte(&pElem->MaxCompStable.lamdatype_tenscomp);	// 0:lamdaRyo����;1:lamdaRx����;2:lamdaRx2����
		if(version>=3)
		{
			buffer.ReadDouble(&pElem->MaxCompStable.L_bothcomp);
			buffer.ReadDouble(&pElem->MaxCompStable.Lo_r_bothcomp);
			buffer.ReadDouble(&pElem->MaxCompStable.muCoefL_bothcomp);
		}
		else
		{
			pElem->MaxCompStable.L_bothcomp=pElem->_lamdaRc.L;
			pElem->MaxCompStable.Lo_r_bothcomp=pElem->_lamdaRc.Lo_r;
			pElem->MaxCompStable.muCoefL_bothcomp=pElem->_lamdaRc.muCoefBasic*pElem->_lamdaRc.muCoefLive;
		}
		//���ڸ����ĵ�Ԫ��ʾʼĩ����֧�ŵ������ĵ�Ԫ�����ڼ��㸨������ѹ��
		//CElement *pStartBraceForceElem,*pEndBraceForceElem;
	}
	//��ȡ�˼�������˨����ϵ��
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		ROD_BEARLOADCOEF* pConnect=hashRodConnect.Add(h);
		buffer.ReadInteger(&pConnect->hRod);
		buffer.ReadDouble(&pConnect->fStartBearCoef);
		buffer.ReadDouble(&pConnect->fEndBearCoef);
		CLDSPart *pPart=Ta.Parts.FromHandle(pConnect->hRod);
		if(pPart->IsLinePart())
		{
			CLDSLinePart* pRod=(CLDSLinePart*)pPart;
			pRod->connectStart.bearloadcoef=pConnect->fStartBearCoef;
			pRod->connectEnd.bearloadcoef=pConnect->fEndBearCoef;
		}
	}
	if(version>=5)
	{
		DWORD dwRsltObjBuffVersion=0;
		if(version>=7)
		{
			buffer.ReadDword(&dwRsltObjBuffVersion);
			hashNodeAnalBuffer.SetBuffVersion(dwRsltObjBuffVersion);
			hashElemAnalBuffer.SetBuffVersion(dwRsltObjBuffVersion);
		}
		else
		{
			dwRsltObjBuffVersion=1;
			hashNodeAnalBuffer.SetBuffVersion(1);
			hashElemAnalBuffer.SetBuffVersion(1);
		}
		if(hashNodeAnalBuffer.GetFileName(NULL)==0||hashElemAnalBuffer.GetFileName(NULL)==0)	//���ļ�������Ҫ�ȴ�����̨�ļ���
		{
			char filename[MAX_PATH];
			theApp.GetFileName(filename);
			InitProcTempFile(filename,dwRsltObjBuffVersion);
		}
		//��ȡ�ڵ��������Ϣ
		DWORD nBufSize=0;
		buffer.ReadDword(&nBufSize);
		char pool[1024]={0};
		hashNodeAnalBuffer.ClearBuffer();
		hashNodeAnalBuffer.WriteBuffer(&buffer,nBufSize,pool,1024);
		//��ȡ��Ԫ��������Ϣ
		buffer.ReadDword(&nBufSize);
		hashElemAnalBuffer.ClearBuffer();
		hashElemAnalBuffer.WriteBuffer(&buffer,nBufSize,pool,1024);
	}
}
void CPostProcessReport::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(7);	//��ʼ�汾��
	buffer.WriteString(m_sProductVersion);
	buffer.WriteBooleanThin(m_bLinearAnalysis);
	buffer.WriteBooleanThin(m_bHasStdFoundationForce);
	buffer.WriteInteger(m_iMemberSizeCheckCode);
	buffer.WriteInteger(hashNodes.GetNodeNum());
	for(POSTRPT_NODE *pRptNode=hashNodes.GetFirst();pRptNode;pRptNode=hashNodes.GetNext())
	{
		buffer.WriteInteger(hashNodes.GetCursorKey());
		buffer.WriteBooleanThin(pRptNode->bFatherNode);
		buffer.WriteString(pRptNode->symmetry);
	}
	buffer.WriteInteger(Reaction.GetNodeNum());
	for(BASENODE_REACTION *pReact=Reaction.GetFirst();pReact;pReact=Reaction.GetNext())
	{
		buffer.WriteString(Reaction.GetCursorKey());
		buffer.WriteInteger(pReact->pNode->handle);
		buffer.WriteInteger(pReact->pModule->handle);
		buffer.WriteInteger(pReact->pStatus->handle);
		buffer.Write(pReact->leg_no,4*sizeof(WORD));
		buffer.WriteInteger(pReact->status);
		buffer.WritePoint(pReact->reaction_force_design);
		buffer.WritePoint(pReact->reaction_force_std);
	}
	buffer.WriteInteger(TowerWeight.GetNodeNum());
	for(TOWERWEIGHT *pTowerWeight=TowerWeight.GetFirst();pTowerWeight;pTowerWeight=TowerWeight.GetNext())
	{
		buffer.WriteInteger(TowerWeight.GetCursorKey());
		buffer.WriteInteger(pTowerWeight->cfg_no);
		buffer.WriteInteger(pTowerWeight->iBody0Leg1);
		buffer.WriteDouble(pTowerWeight->weight);
		buffer.WriteDouble(pTowerWeight->netWeight);
	}
	buffer.WriteInteger(hashElems.GetNodeNum());
	CHashList<CLDSLinePartPtr>hashRodPtrs;
	for(CElement *pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
	{
		hashRodPtrs.SetValue(pElem->pOrgPole->handle,pElem->pOrgPole);
		//FEM_ELEMENT *pFemElem;
		buffer.WriteInteger(pElem->id);	//��Ԫ��ʶ����ǰ�����������ɣ�һֱ�ᴩʼ��
		//CFGWORD cfgword;
		buffer.WriteInteger(pElem->pStart->handle);
		buffer.WriteInteger(pElem->pEnd->handle);
		buffer.WriteInteger(pElem->pOrgPole->handle);
		buffer.WriteBooleanThin(pElem->relaElem.bFatherElem);
		buffer.WriteInteger(pElem->relaElem.idRefElem);	//����б�Ŀ���ͬʱ��ѹ���յ�Ԫ
		buffer.WriteInteger(pElem->relaElem.idRefElem2);//����ͬ�˽ڵ�ͬʱ��ѹ���յ�Ԫ
		buffer.WriteInteger(pElem->relaElem.idElemMirX);//�˵�Ԫ�Ƿ�Ϊ�Գƹ�����Ԫ�еĸ���Ԫ�����ڵ���ۺϽ�С�ĵ�Ԫ
		buffer.WriteInteger(pElem->relaElem.idElemMirY);
		buffer.WriteInteger(pElem->relaElem.idElemMirZ);
		//int BearTens1Comp2;	//1:�����˳�ѹ;2:��ѹ�˳���;����Ϊ�����ˡ�1��2Ϊ��ˣ��������Ϊ���
		//void* pParaType;
		//buffer.WriteDouble(pElem->N);				//����(N)
		buffer.WriteDouble(pElem->T0);				//ê�̶�Ԥ����(N)
		//double W0,H0,V0;		//����Ԥ�Ž�����������(N)�����ɶ���Ԥ���������²���������ˮƽ��������ֱ����
		//bool bAnchorEnd;		//true:�����ն˶Ե�ê��;false:����ʼ�˶Ե�ê��
		//buffer.WritePoint(pElem->moment);		//��Ԫ������(N*mm)
		//short elem_type;	//0:��ʾ�����˻򾭵���;1:��ʾ������Ԫ
		//��С����㳤����Ϣ
		if(pElem->lamdaRyo.pStart!=NULL)
			buffer.WriteInteger(pElem->lamdaRyo.pStart->handle);	//���㳤����ʼ��
		else
			buffer.WriteInteger(0);
		if(pElem->lamdaRyo.pEnd!=NULL)
			buffer.WriteInteger(pElem->lamdaRyo.pEnd->handle);		//���㳤����ֹ��
		else
			buffer.WriteInteger(0);
		buffer.WriteDouble(pElem->lamdaRyo.muCoefBasic);	//���㳤���൱ϵ��
		buffer.WriteDouble(pElem->lamdaRyo.L);		//���㳤��
		buffer.WriteDouble(pElem->lamdaRyo.R);		//ִ�л�ת�뾶
		buffer.WriteDouble(pElem->lamdaRyo.Lo_r);	//���㳤ϸ��
		buffer.WriteString(pElem->lamdaRyo.braceStr);//��ϸ�ȼ�������ַ���
		//��ͨ�Ļ򽻲�б��һ��һѹʱƽ���᳤ϸ�ȼ�����Ϣ
		if(pElem->lamdaRx.pStart!=NULL)
			buffer.WriteInteger(pElem->lamdaRx.pStart->handle);	//���㳤����ʼ��
		else
			buffer.WriteInteger(0);
		if(pElem->lamdaRx.pEnd!=NULL)
			buffer.WriteInteger(pElem->lamdaRx.pEnd->handle);		//���㳤����ֹ��
		else
			buffer.WriteInteger(0);
		buffer.WriteDouble(pElem->lamdaRx.muCoefBasic);	//���㳤���൱ϵ��
		buffer.WriteDouble(pElem->lamdaRx.L);		//���㳤��
		buffer.WriteDouble(pElem->lamdaRx.R);		//ִ�л�ת�뾶
		buffer.WriteDouble(pElem->lamdaRx.Lo_r);	//���㳤ϸ��
		buffer.WriteString(pElem->lamdaRx.braceStr);//��ϸ�ȼ�������ַ���
		//�����ͬʱ��ѹʱƽ������㳤����Ϣ
		if(pElem->LamdaRc().pStart!=NULL)
			buffer.WriteInteger(pElem->LamdaRc().pStart->handle);	//���㳤����ʼ��
		else
			buffer.WriteInteger(0);
		if(pElem->LamdaRc().pEnd!=NULL)
			buffer.WriteInteger(pElem->LamdaRc().pEnd->handle);		//���㳤����ֹ��
		else
			buffer.WriteInteger(0);
		buffer.WriteDouble(pElem->LamdaRc().muCoefBasic);	//���㳤���൱ϵ����ֵ̬����
		buffer.WriteDouble(pElem->LamdaRc().muCoefLive);	//���㳤���൱ϵ����ֵ̬����
		buffer.WriteDouble(pElem->LamdaRc().L);		//���㳤��
		buffer.WriteDouble(pElem->LamdaRc().R);		//ִ�л�ת�뾶
		buffer.WriteDouble(pElem->LamdaRc().Lo_r);	//���㳤ϸ��
		buffer.WriteString(pElem->LamdaRc().braceStr);//��ϸ�ȼ�������ַ���
		//m_fStartBraceNCoef;	//������ʼ�˶������ĵ�֧����ϵ��
		//m_fEndBraceNCoef;	//�������ն˶������ĵ�֧����ϵ��
		buffer.WriteDouble(pElem->m_fWeightRaiseCoef);	//������������ϵ��
		buffer.WriteInteger(pElem->m_iCtrlRule);
		//int iSizeIndex;		//���ڰ��չ������ȽϹ���С
		buffer.WriteInteger(pElem->m_bValidStartRestStiff);
		buffer.WriteInteger(pElem->m_bValidEndRestStiff);	//�Ƿ��г��ܲ���ת��Լ��������
	//xx_joint_type��������ԣ���real_xx_joint_type�����ʵ�ʼ���ʱ��֧������
	//0����ȫ�½�JOINT_HINGE��1�����ָս�JOINT_RIGID�����Ƴ�Ťת���ת��Լ����,2���Զ��ж�JOINT_AUTOJUSTIFY
		buffer.WriteInteger(pElem->start_joint_type);
		buffer.WriteInteger(pElem->end_joint_type);	//JOINT_TYPE
		buffer.WriteInteger(pElem->real_start_joint_type);
		buffer.WriteInteger(pElem->real_end_joint_type);	//0����ȫ�½�JOINT_HINGE1�����ָս�JOINT_RIGID�����Ƴ�Ťת���ת��Լ����
		buffer.WriteInteger(pElem->start_force_type);
		buffer.WriteInteger(pElem->end_force_type);	//	//�Ǹ���������״��CENTRIC_FORCE,ECCENTRIC_FORCE
	//д�������Ϣdesign_info;
		buffer.WriteDouble(pElem->design_info.hole_out);	//������
		buffer.WriteDouble(pElem->design_info.hole_d);		//�׾�
		buffer.WriteDouble(pElem->design_info.w_t);
		buffer.WriteDouble(pElem->design_info.fCoefTensStrengthDecrease);
		buffer.WriteDouble(pElem->design_info.fCoefCompStrengthDecrease);
		buffer.WriteDouble(pElem->design_info.fCoefStablity);
		buffer.WriteDouble(pElem->design_info.fCoefEffic);		//���ƹ��������ָ��ռָ�������޵ı���ϵ�����޿��ƹ���������Ϊ��
		buffer.WriteDouble(pElem->design_info.fCoefOfBearLoad);	//���ƹ����³��ܺ���ռ���ؼ��޵ı���ϵ�����޿��ƹ���������Ϊ��
		buffer.WriteInteger(pElem->design_info.iDangerousRule);	//Σ�տ��ƹ���
		buffer.WriteInteger(pElem->design_info.iDangerousLoadCase);	//��Σ�յĳ���ָ��
		buffer.WriteBooleanThin(pElem->design_info.bBlockShearActivated);
		buffer.WriteDouble(pElem->design_info.mN);				//ѹ���ȶ�ǿ���ۼ�ϵ��
		buffer.WriteString(pElem->design_info.LoExpression);
		buffer.WriteString(pElem->design_info.lamdaExpression);		//��ϸ�ȼ�����ʽ
		buffer.WriteInteger(pElem->design_info.bStartRotRest);
		buffer.WriteInteger(pElem->design_info.bEndRotRest);
		buffer.WriteByte(pElem->design_info.lamdaSelType);	// 0:lamdaRyo����;1:lamdaRx����;2:lamdaRx2����
		buffer.WriteByte(pElem->design_info.cLamdaTypeR);	//0:Rx;1:Ry0
		buffer.WriteDouble(pElem->design_info.lamdaL);
		buffer.WriteDouble(pElem->design_info.lamdaR);
		buffer.WriteDouble(pElem->design_info.Lo_r);		//���㳤��,��ת�뾶,ʵ�ʳ�ϸ��
		buffer.WriteDouble(pElem->design_info.UltimateLo_r);//�������ϸ��
		//д��ELEMFORCE_ENV MaxTension,MaxCompression,MaxMoment;
		buffer.WriteInteger(pElem->MaxTension.hModule);
		buffer.WriteInteger(pElem->MaxTension.hWorkStatus);
		buffer.Write(pElem->MaxTension.leg_no,sizeof(long)*4);
		buffer.WriteDouble(pElem->MaxTension.N);
		buffer.WriteInteger(pElem->MaxCompression.hModule);
		buffer.WriteInteger(pElem->MaxCompression.hWorkStatus);
		buffer.Write(pElem->MaxCompression.leg_no,sizeof(long)*4);
		buffer.WriteDouble(pElem->MaxCompression.N);
		buffer.WriteInteger(pElem->MaxMoment.hModule);
		buffer.WriteInteger(pElem->MaxMoment.hWorkStatus);
		buffer.Write(pElem->MaxMoment.leg_no,sizeof(long)*4);
		buffer.WriteDouble(pElem->MaxMoment.N);
		//д��ELEMCOMPSTABLE_ENV MaxCompStable;			//ѹ�ȿ���ʱ�����ѹ�ȼ���Ӧ��
		buffer.WriteBooleanThin(pElem->smcinfo.hasBothComp);
		buffer.WriteInteger(pElem->MaxCompStable.hModule);	//ģ�;��
		buffer.WriteInteger(pElem->MaxCompStable.hWorkStatus);//�������
		buffer.Write(pElem->MaxCompStable.leg_no,sizeof(long)*4);		//�����޽��Ⱥ�
		buffer.WriteDouble(pElem->MaxCompStable.N);		// (N)
		buffer.WriteBooleanThin(pElem->MaxCompStable.bothComp);	// true:�����ͬʱ��ѹ����;false:�ǽ���Ļ򽻲��һ��һѹ��������
		buffer.WriteDouble(pElem->MaxCompStable.sigema);	// N/(A*��),(MPa)
		//buffer.WriteDouble(pElem->MaxCompStable.fai);		// �������ѹ��ϵ��
	//���в�����ͬʱ��ѹ�ĸ˼���Ԫ����Ϣ��һ��һѹ�洢
		buffer.WriteDouble(pElem->MaxCompStable.Afai_bothcomp);
		buffer.WriteDouble(pElem->MaxCompStable.Afai_tenscomp);
		buffer.WriteDouble(pElem->MaxCompStable.fai_bothcomp);
		buffer.WriteDouble(pElem->MaxCompStable.fai_tenscomp);	//ͬʱ��ѹ��һ��һѹʱ��ѹ��ϵ��
		buffer.WriteByte(pElem->MaxCompStable.lamdatype_bothcomp);
		buffer.WriteByte(pElem->MaxCompStable.lamdatype_tenscomp);	// 0:lamdaRyo����;1:lamdaRx����;2:lamdaRx2����
		buffer.WriteDouble(pElem->MaxCompStable.L_bothcomp);
		buffer.WriteDouble(pElem->MaxCompStable.Lo_r_bothcomp);
		buffer.WriteDouble(pElem->MaxCompStable.muCoefL_bothcomp);
		//���ڸ����ĵ�Ԫ��ʾʼĩ����֧�ŵ������ĵ�Ԫ�����ڼ��㸨������ѹ��
		//CElement *pStartBraceForceElem,*pEndBraceForceElem;
	}
	//�洢�˼�������˨����ϵ��
	buffer.WriteInteger(hashRodPtrs.GetNodeNum());
	for(CLDSLinePartPtr *ppRod=hashRodPtrs.GetFirst();ppRod;ppRod=hashRodPtrs.GetNext())
	{
		buffer.WriteInteger((*ppRod)->handle);
		buffer.WriteDouble((*ppRod)->connectStart.bearloadcoef);
		buffer.WriteDouble((*ppRod)->connectEnd.bearloadcoef);
	}
	DWORD dwCurrRsltBuffVersion=hashNodeAnalBuffer.BuffVersion();
	if(dwCurrRsltBuffVersion==0)	//0��ʾ��ǰ���°汾
		dwCurrRsltBuffVersion=CURR_RSLTOBJBUFF_VERSION;
	buffer.WriteDword(dwCurrRsltBuffVersion);	//if(version>=7)
	//�洢�ڵ��������Ϣ������ռ�û���ռ�ϴ�������Ҫ��ǰ�����������д����Ч�ʵ�
	DYN_ARRAY<char>pool(min(hashNodeAnalBuffer.GetLength(),0x500000));
	buffer.WriteDword(hashNodeAnalBuffer.GetLength());
	hashNodeAnalBuffer.SeekPosition(0);
	buffer.WriteBuffer(&hashNodeAnalBuffer,hashNodeAnalBuffer.GetLength(),pool,pool.Size());
	buffer.WriteDword(hashElemAnalBuffer.GetLength());
	hashElemAnalBuffer.SeekPosition(0);
	buffer.WriteBuffer(&hashElemAnalBuffer,hashElemAnalBuffer.GetLength(),pool,pool.Size());
}
COutputInfo* AppendOutputInfoToList(void* pContext)
{
	ATOM_LIST<COutputInfo>* pInfoList=(ATOM_LIST<COutputInfo>*)pContext;
	return pInfoList->append();
}
bool CPostProcessReport::CompareWithLegacyReport(CPostProcessReport& legacyreport, CLogFile& rpt)
{
	//�ȶԽڵ���Ϣ���
	CTaskOutputList infoList;
	COutputInfoAgent agent(&infoList);
	agent.ListAppendFunc=AppendOutputInfoToList;
	agent.m_uType=INFO_COMPARE_REPORT_WARNNING;
	if(legacyreport.hashNodes.GetNodeNum()!=this->hashNodes.GetNodeNum())
	{
		rpt.Log("�ڵ���ԭ������Ϊ%d,��ǰ����Ϊ%d\n",legacyreport.hashNodes.GetNodeNum(),this->hashNodes.GetNodeNum());
		agent.Append(0,0,rpt.LatestLog());
		return false;
	}
	POSTRPT_NODE *pLegacyNode=legacyreport.hashNodes.GetFirst();
	agent.m_uType=INFO_COMPARE_REPORT_NODES;
	for(POSTRPT_NODE *pRptNode=this->hashNodes.GetFirst();pRptNode&&pLegacyNode;pRptNode=this->hashNodes.GetNext())
	{
		if(pRptNode->pNode!=pLegacyNode->pNode)
		{
			rpt.Log("ԭ�нڵ�{%d}��ͬ�ڵ�ǰ�ڵ�{%d}\n",pLegacyNode->pNode->point_i,pRptNode->pNode->point_i);
			agent.Append(pRptNode->pNode->handle,pLegacyNode->pNode->handle,rpt.LatestLog());
		}
		if(pRptNode->bFatherNode!=pLegacyNode->bFatherNode)
		{
			rpt.Log("ԭ�нڵ�{%d}��ͬ�ڵ�ǰ�ڵ�{%d},���ڵ����Ͳ�ͬ\n",pLegacyNode->pNode->point_i,pRptNode->pNode->point_i);
			agent.Append(pRptNode->pNode->handle,pLegacyNode->pNode->handle,rpt.LatestLog());
		}
		if(!pRptNode->symmetry.Equal(pLegacyNode->symmetry))
		{
			rpt.Log("ԭ�нڵ�{%d}�Գ���ϢΪ%s,��ͬ�ڵ�ǰ�ڵ�{%d}�ĶԳ���Ϣ%s,���ڵ����Ͳ�ͬ\n",pLegacyNode->pNode->point_i,
				(char*)pLegacyNode->symmetry,pRptNode->pNode->point_i,(char*)pRptNode->symmetry);
			agent.Append(pRptNode->pNode->handle,pLegacyNode->pNode->handle,rpt.LatestLog());
		}
		pLegacyNode=legacyreport.hashNodes.GetNext();
	}
	agent.m_uType=INFO_COMPARE_REPORT_WARNNING;
	if(legacyreport.Reaction.GetNodeNum()!=this->Reaction.GetNodeNum())
	{
		rpt.Log("��������ԭ������Ϊ%d,��ǰ����Ϊ%d\n",legacyreport.Reaction.GetNodeNum(),this->Reaction.GetNodeNum());
		agent.Append(0,0,rpt.LatestLog());
		return false;
	}
	agent.m_uType=INFO_COMPARE_REPORT_NODES;
	BASENODE_REACTION *pLegacyReact=legacyreport.Reaction.GetFirst();
	for(BASENODE_REACTION *pReact=this->Reaction.GetFirst();pReact;pReact=this->Reaction.GetNext())
	{
		if(pReact->pNode!=pLegacyReact->pNode)
			rpt.Log("ԭ�л����ڵ�{%d}��ͬ�ڵ�ǰ�����ڵ�{%d}\n",pLegacyReact->pNode->point_i,pReact->pNode->point_i);
		if(pReact->pModule!=pLegacyReact->pModule||pReact->pStatus!=pLegacyReact->pStatus)
			rpt.Log("ԭ�л����ڵ�{%d}�Ĺ���{%d-%d}��ͬ�ڵ�ǰ�����ڵ�{%d}�Ĺ���{%d-%d}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->pModule->iNo,pLegacyReact->pStatus->iNo,
			pReact->pNode->point_i,pReact->pModule->iNo,pReact->pStatus->iNo);
		if( pReact->leg_no[0]!=pLegacyReact->leg_no[0]||pReact->leg_no[1]!=pLegacyReact->leg_no[1]||
			pReact->leg_no[2]!=pLegacyReact->leg_no[2]||pReact->leg_no[3]!=pLegacyReact->leg_no[3])
			rpt.Log("ԭ�л����ڵ�{%d}�Ľ���{%d-%d-%d-%d}��ͬ�ڵ�ǰ�����ڵ�{%d}�Ĺ���{%d-%d-%d-%d}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->leg_no[0],pLegacyReact->leg_no[1],pLegacyReact->leg_no[2],pLegacyReact->leg_no[3],
			pReact->pNode->point_i,pReact->leg_no[0],pReact->leg_no[1],pReact->leg_no[2],pReact->leg_no[3]);
		GEPOINT diff=pLegacyReact->reaction_force_design-pReact->reaction_force_design;
		if(!pReact->reaction_force_design.IsEqual(pLegacyReact->reaction_force_design.x,
			pLegacyReact->reaction_force_design.y,pLegacyReact->reaction_force_design.z,0.5))
			rpt.Log("ԭ�л�����Ʒ���{%d��%d-%d}�ϵ�ǰ������Ʒ���{%d��%d-%d}�����{%8.3f %8.3f %8.3f kN}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->pModule->iNo,pLegacyReact->pStatus->iNo,
			pReact->pNode->point_i,pReact->pModule->iNo,pReact->pStatus->iNo,
			diff.x*0.001,diff.y*0.001,diff.z*0.001);
		/**���ڻ�����������׼ֵ�ѱ������ֵ���м����ˡ�wjh-2016.4.22
		diff=pLegacyReact->reaction_force_std-pReact->reaction_force_std;
		if(!pReact->reaction_force_std.IsEqual(pLegacyReact->reaction_force_std,0.5))
			rpt.Log("ԭ�л�����׼����{%d��%d-%d}�ϵ�ǰ������׼����{%d��%d-%d}�����{%8.3f %8.3f %8.3f kN}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->pModule->iNo,pLegacyReact->pStatus->iNo,
			pReact->pNode->point_i,pReact->pModule->iNo,pReact->pStatus->iNo,
			diff.x*0.001,diff.y*0.001,diff.z*0.001);
		*/
		pLegacyReact=legacyreport.Reaction.GetNext();
	}
	if(legacyreport.TowerWeight.GetNodeNum()!=this->TowerWeight.GetNodeNum())
	{
		rpt.Log("�����������ͳ�Ƶ�Ԫԭ����Ϊ%d,��ǰ����Ϊ%d\n",legacyreport.TowerWeight.GetNodeNum(),this->TowerWeight.GetNodeNum());
		return false;
	}
	TOWERWEIGHT *pLegacyWeight=legacyreport.TowerWeight.GetFirst();
	for(TOWERWEIGHT *pTowerWeight=this->TowerWeight.GetFirst();pTowerWeight;pTowerWeight=this->TowerWeight.GetNext())
	{
		if(pLegacyWeight->cfg_no!=pTowerWeight->cfg_no||pLegacyWeight->iBody0Leg1!=pTowerWeight->iBody0Leg1||
			fabs(pLegacyWeight->weight-pTowerWeight->weight)>0.1)
		{
			char cLegacy='B',cCurrent='B';
			if(pLegacyWeight->iBody0Leg1==1)
				cLegacy='L';
			if(pTowerWeight->iBody0Leg1==1)
				cCurrent='L';
			rpt.Log("����ͳ�Ƽ�¼ԭ��¼{%C%d=%.1f}�뵱ǰ��¼{%C%d=%.1f}��ƥ��\n",
				cLegacy,pLegacyWeight->cfg_no,pLegacyWeight->weight,cCurrent,pTowerWeight->cfg_no,pTowerWeight->weight);
		}
		pLegacyWeight=legacyreport.TowerWeight.GetNext();
	}
	if(legacyreport.hashElems.GetNodeNum()!=this->hashElems.GetNodeNum())
		rpt.Log("��Ԫԭ����Ϊ%d,��ǰ����Ϊ%d\n",legacyreport.hashElems.GetNodeNum(),this->hashElems.GetNodeNum());
	agent.m_uType=INFO_COMPARE_REPORT_ELEMS;
	CElement *pLegacyElem=legacyreport.hashElems.GetFirst();
	for(CElement *pElem=this->hashElems.GetFirst();pElem;pElem=this->hashElems.GetNext())
	{
		if((pLegacyElem=legacyreport.hashElems.GetValue(pElem->id))==NULL)
			break;
		//rpt.InsertBlankRow();
		long arrStartI[2],arrEndI[2];
		arrStartI[0]=min(pLegacyElem->pStart->point_i,pLegacyElem->pEnd->point_i);
		arrEndI[0]=max(pLegacyElem->pStart->point_i,pLegacyElem->pEnd->point_i);
		arrStartI[1]=min(pElem->pStart->point_i,pElem->pEnd->point_i);
		arrEndI[1]=max(pElem->pStart->point_i,pElem->pEnd->point_i);
		if(pElem->id!=pLegacyElem->id||arrStartI[1]!=arrStartI[0]||
			arrEndI[1]!=arrEndI[0])
		{
			rpt.Log("ԭ�е�Ԫ{%5d:%5d-%5d}�뵱ǰ��Ԫ{%5d:%5d-%5d}��ʶ��ͬ\n",
			pLegacyElem->id,arrStartI[0],arrEndI[0],pElem->id,arrStartI[1],arrEndI[1]);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->relaElem.bFatherElem!=pLegacyElem->relaElem.bFatherElem)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ĸ���������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#ifdef __ALFA_TEST_VERSION_
		if(pElem->relaElem.idRefElem!=pLegacyElem->relaElem.idRefElem)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d����ѹ���յ�Ԫ��ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->relaElem.idRefElem2!=pLegacyElem->relaElem.idRefElem2)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ͬ�˽ڵ���ѹ���յ�Ԫ��ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#endif
		if(pElem->relaElem.idElemMirX!=pLegacyElem->relaElem.idElemMirX)
		{		
			rpt.Log("��ǰ��Ԫ%5d-%5d��X��ԳƵ�Ԫ��ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->relaElem.idElemMirY!=pLegacyElem->relaElem.idElemMirY)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��Y��ԳƵ�Ԫ��ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->relaElem.idElemMirZ!=pLegacyElem->relaElem.idElemMirZ)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��Z��ԳƵ�Ԫ��ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//int BearTens1Comp2;	//1:�����˳�ѹ;2:��ѹ�˳���;����Ϊ�����ˡ�1��2Ϊ��ˣ��������Ϊ���
		//void* pParaType;
		if(pElem->elem_type==1&&fabs(pElem->T0-pLegacyElem->T0)>0.5)
		{	//������Ԫ
			rpt.Log("��ǰ��Ԫ%5d-%5d��ê�̶�Ԥ����%8.3fkN��ԭ�е�Ԫ%8.3fkN��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->T0*0.001,pLegacyElem->T0*0.001);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//double W0,H0,V0;		//����Ԥ�Ž�����������(N)�����ɶ���Ԥ���������²���������ˮƽ��������ֱ����
		//bool bAnchorEnd;		//true:�����ն˶Ե�ê��;false:����ʼ�˶Ե�ê��
		//short elem_type;	//0:��ʾ�����˻򾭵���;1:��ʾ������Ԫ
		//��С����㳤����Ϣ
		if(pElem->MaxCompStable.bothComp!=pLegacyElem->MaxCompStable.bothComp)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ͬʱ��ѹ������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.bothComp,pLegacyElem->MaxCompStable.bothComp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#ifdef __ALFA_TEST_VERSION_
		if(pElem->lamdaRyo.pStart!=pLegacyElem->lamdaRyo.pStart||pElem->lamdaRyo.pEnd!=pLegacyElem->lamdaRyo.pEnd)
		{
			int starti=0,endi=0,legacysi=0,legacyei=0;
			if(pElem->lamdaRyo.pStart!=NULL)
				starti=pElem->lamdaRyo.pStart->point_i;	//���㳤����ʼ��
			if(pElem->lamdaRyo.pEnd!=NULL)
				endi=pElem->lamdaRyo.pEnd->point_i;		//���㳤����ֹ��
			if(pLegacyElem->lamdaRyo.pStart!=NULL)
				legacysi=pLegacyElem->lamdaRyo.pStart->point_i;	//���㳤����ʼ��
			if(pLegacyElem->lamdaRyo.pEnd!=NULL)
				legacyei=pLegacyElem->lamdaRyo.pEnd->point_i;		//���㳤����ֹ��
			rpt.Log("��ǰ��Ԫ%5d-%5d����С���������{%d-%d}��ԭ�е�Ԫ{%d-%d}��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
				starti,endi,legacysi,legacyei);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->lamdaRyo.muCoefBasic!=pLegacyElem->lamdaRyo.muCoefBasic)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d����С����㳤���൱ϵ��%8.3f��ԭ�е�Ԫ%8.3f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRyo.muCoefBasic,pLegacyElem->lamdaRyo.muCoefBasic);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->lamdaRyo.L-pLegacyElem->lamdaRyo.L)>0.1)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d����С����㳤��%8.1f��ԭ�е�Ԫ%8.1f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRyo.L,pLegacyElem->lamdaRyo.L);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->lamdaRyo.R-pLegacyElem->lamdaRyo.R)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d����С��ִ�л�ת�뾶%8.3f��ԭ�е�Ԫ%8.3f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRyo.R,pLegacyElem->lamdaRyo.R);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->lamdaRyo.Lo_r-pLegacyElem->lamdaRyo.Lo_r)>0.1)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d����С����㳤ϸ��%8.1f��ԭ�е�Ԫ%8.1f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRyo.Lo_r,pLegacyElem->lamdaRyo.Lo_r);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(!pElem->lamdaRyo.braceStr.EqualNoCase(pLegacyElem->lamdaRyo.braceStr))
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d����С�᳤ϸ�ȼ����ַ���%s��ԭ�е�Ԫ%s��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			(char*)pElem->lamdaRyo.braceStr,(char*)pLegacyElem->lamdaRyo.braceStr);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//��ͨ�Ļ򽻲�б��һ��һѹʱƽ���᳤ϸ�ȼ�����Ϣ
		if(pElem->lamdaRx.pStart!=pLegacyElem->lamdaRx.pStart||pElem->lamdaRx.pEnd!=pLegacyElem->lamdaRx.pEnd)
		{
			int starti=0,endi=0,legacysi=0,legacyei=0;
			if(pElem->lamdaRx.pStart!=NULL)
				starti=pElem->lamdaRx.pStart->point_i;	//���㳤����ʼ��
			if(pElem->lamdaRx.pEnd!=NULL)
				endi=pElem->lamdaRx.pEnd->point_i;		//���㳤����ֹ��
			if(pLegacyElem->lamdaRx.pStart!=NULL)
				legacysi=pLegacyElem->lamdaRx.pStart->point_i;	//���㳤����ʼ��
			if(pLegacyElem->lamdaRx.pEnd!=NULL)
				legacyei=pLegacyElem->lamdaRx.pEnd->point_i;		//���㳤����ֹ��
			rpt.Log("��ǰ��Ԫ%5d-%5d��ƽ�����������{%d-%d}��ԭ�е�Ԫ{%d-%d}��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
				starti,endi,legacysi,legacyei);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->lamdaRx.muCoefBasic!=pLegacyElem->lamdaRx.muCoefBasic)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ƽ������㳤���൱ϵ��%8.3f��ԭ�е�Ԫ%8.3f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRx.muCoefBasic,pLegacyElem->lamdaRx.muCoefBasic);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->lamdaRx.L-pLegacyElem->lamdaRx.L)>0.1)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ƽ������㳤��%8.1f��ԭ�е�Ԫ%8.1f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRx.L,pLegacyElem->lamdaRx.L);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->lamdaRx.R-pLegacyElem->lamdaRx.R)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ƽ����ִ�л�ת�뾶%8.3f��ԭ�е�Ԫ%8.3f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRx.R,pLegacyElem->lamdaRx.R);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->lamdaRx.Lo_r-pLegacyElem->lamdaRx.Lo_r)>0.1)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ƽ������㳤ϸ��%8.1f��ԭ�е�Ԫ%8.1f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRx.Lo_r,pLegacyElem->lamdaRx.Lo_r);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(!pElem->lamdaRx.braceStr.EqualNoCase(pLegacyElem->lamdaRx.braceStr))
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ƽ���᳤ϸ�ȼ����ַ���%s��ԭ�е�Ԫ%s��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			(char*)pElem->lamdaRx.braceStr,(char*)pLegacyElem->lamdaRx.braceStr);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//�����ͬʱ��ѹʱƽ������㳤����Ϣ
		if(pElem->_lamdaRc.pStart!=pLegacyElem->_lamdaRc.pStart||pElem->_lamdaRc.pEnd!=pLegacyElem->_lamdaRc.pEnd)
		{
			int starti=0,endi=0,legacysi=0,legacyei=0;
			if(pElem->_lamdaRc.pStart!=NULL)
				starti=pElem->_lamdaRc.pStart->point_i;	//���㳤����ʼ��
			if(pElem->_lamdaRc.pEnd!=NULL)
				endi=pElem->_lamdaRc.pEnd->point_i;		//���㳤����ֹ��
			if(pLegacyElem->_lamdaRc.pStart!=NULL)
				legacysi=pLegacyElem->_lamdaRc.pStart->point_i;	//���㳤����ʼ��
			if(pLegacyElem->_lamdaRc.pEnd!=NULL)
				legacyei=pLegacyElem->_lamdaRc.pEnd->point_i;		//���㳤����ֹ��
			rpt.Log("��ǰ��Ԫ%5d-%5d��ͬʱ��ѹƽ�����������{%d-%d}��ԭ�е�Ԫ{%d-%d}��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
				starti,endi,legacysi,legacyei);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//if(pElem->_lamdaRc.MuCoef()!=pLegacyElem->_lamdaRc.MuCoef())
		if(pElem->smcinfo.hasBothComp&&fabs(pElem->MaxCompStable.muCoefL_bothcomp-pLegacyElem->MaxCompStable.muCoefL_bothcomp)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ͬʱ��ѹ���㳤���൱ϵ��%8.3f��ԭ�е�Ԫ%8.3f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.muCoefL_bothcomp,pLegacyElem->MaxCompStable.muCoefL_bothcomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//if(fabs(pElem->_lamdaRc.L-pLegacyElem->_lamdaRc.L)>0.1)
		if(pElem->smcinfo.hasBothComp&&fabs(pElem->MaxCompStable.L_bothcomp-pLegacyElem->MaxCompStable.L_bothcomp)>0.1)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ͬʱ��ѹƽ������㳤��%8.1f��ԭ�е�Ԫ%8.1f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.L_bothcomp,pLegacyElem->MaxCompStable.L_bothcomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->_lamdaRc.R-pLegacyElem->_lamdaRc.R)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ͬʱ��ѹƽ����ִ�л�ת�뾶%8.3f��ԭ�е�Ԫ%8.3f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->_lamdaRc.R,pLegacyElem->_lamdaRc.R);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//if(fabs(pElem->_lamdaRc.Lo_r-pLegacyElem->_lamdaRc.Lo_r)>0.1)
		if(pElem->smcinfo.hasBothComp&&fabs(pElem->MaxCompStable.Lo_r_bothcomp-pLegacyElem->MaxCompStable.Lo_r_bothcomp)>0.1)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ͬʱ��ѹƽ���᳤ϸ��%8.1f��ԭ�е�Ԫ%8.1f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.Lo_r_bothcomp,pLegacyElem->MaxCompStable.Lo_r_bothcomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(!pElem->_lamdaRc.braceStr.EqualNoCase(pLegacyElem->_lamdaRc.braceStr))
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ͬʱ��ѹƽ���᳤ϸ�ȼ����ַ���%s��ԭ�е�Ԫ%s��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			(char*)pElem->_lamdaRc.braceStr,(char*)pLegacyElem->_lamdaRc.braceStr);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->MaxCompStable.lamdatype_bothcomp!=pLegacyElem->MaxCompStable.lamdatype_bothcomp)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ͬʱ��ѹ��ϸ��������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.lamdatype_bothcomp,pLegacyElem->MaxCompStable.lamdatype_bothcomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->smcinfo.hasBothComp&&fabs(pElem->MaxCompStable.fai_bothcomp-pLegacyElem->MaxCompStable.fai_bothcomp)>0.001)
		//if(fabs(pElem->LamdaRc().coefFai-pLegacyElem->MaxCompStable.fai_bothcomp)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ͬʱ��ѹ����ѹ��ϵ��%8.3f��ԭ�е�Ԫ��ͬ%8.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.fai_bothcomp,pLegacyElem->MaxCompStable.fai_bothcomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->MaxCompStable.fai_tenscomp-pLegacyElem->MaxCompStable.fai_tenscomp)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��һ��һѹ����ѹ��ϵ��%8.3f��ԭ�е�Ԫ��ͬ%8.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.fai_tenscomp,pLegacyElem->MaxCompStable.fai_tenscomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->MaxCompStable.lamdatype_tenscomp!=pLegacyElem->MaxCompStable.lamdatype_tenscomp)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��һ��һѹ��ϸ��������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.lamdatype_tenscomp,pLegacyElem->MaxCompStable.lamdatype_tenscomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->m_fWeightRaiseCoef!=pLegacyElem->m_fWeightRaiseCoef)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d����������ϵ����ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#endif
		if(pElem->m_iCtrlRule!=pLegacyElem->m_iCtrlRule)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ܿ����������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//m_fStartBraceNCoef;	//������ʼ�˶������ĵ�֧����ϵ��
		//m_fEndBraceNCoef;	//�������ն˶������ĵ�֧����ϵ��
		//int iSizeIndex;		//���ڰ��չ������ȽϹ���С
		if(pElem->m_bValidStartRestStiff!=pLegacyElem->m_bValidStartRestStiff)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ʼ��ת��Լ��������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->m_bValidEndRestStiff!=pLegacyElem->m_bValidEndRestStiff)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ն�ת��Լ��������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
	//xx_joint_type��������ԣ���real_xx_joint_type�����ʵ�ʼ���ʱ��֧������
	//0����ȫ�½�JOINT_HINGE��1�����ָս�JOINT_RIGID�����Ƴ�Ťת���ת��Լ����,2���Զ��ж�JOINT_AUTOJUSTIFY
		if(pElem->start_joint_type!=pLegacyElem->start_joint_type)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ȱʡʼ������������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->end_joint_type!=pLegacyElem->end_joint_type)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ȱʡ�ն�����������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->real_start_joint_type!=pLegacyElem->real_start_joint_type)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ʼ������������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->real_end_joint_type!=pLegacyElem->real_end_joint_type)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ն�����������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->start_force_type!=pLegacyElem->start_force_type)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ʼ�˸˶�����״̬��ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->end_force_type!=pLegacyElem->end_force_type)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ն˸˶�����״̬��ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//д�������Ϣdesign_info;
		if(fabs(pElem->design_info.hole_out-pLegacyElem->design_info.hole_out)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ļ���ϵ��%5.3f��ԭ�е�Ԫ��ͬ%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.hole_out,pLegacyElem->design_info.hole_out);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.fCoefTensStrengthDecrease-pLegacyElem->design_info.fCoefTensStrengthDecrease)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d������ǿ���ۼ�ϵ��%5.3f��ԭ�е�Ԫ%5.3f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefTensStrengthDecrease,pLegacyElem->design_info.fCoefTensStrengthDecrease);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.fCoefCompStrengthDecrease-pLegacyElem->design_info.fCoefCompStrengthDecrease)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d����ѹǿ���ۼ�ϵ��%5.3f��ԭ�е�Ԫ��ͬ%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefCompStrengthDecrease,pLegacyElem->design_info.fCoefCompStrengthDecrease);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.bBlockShearActivated!=pLegacyElem->design_info.bBlockShearActivated)
		{
			CXhChar16 state("False"),legacy("False");
			if(pElem->design_info.bBlockShearActivated)
				state.Copy("True");
			if(pLegacyElem->design_info.bBlockShearActivated)
				legacy.Copy("True");
			rpt.Log("��ǰ��Ԫ%5d-%5d�Ŀ���ܿ�״̬%s��ԭ�е�Ԫ��ͬ%s\n",pElem->pStart->point_i,pElem->pEnd->point_i,(char*)state,(char*)legacy);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.fCoefStablity-pLegacyElem->design_info.fCoefStablity)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ѹ��ϵ��%5.3f��ԭ�е�Ԫ��ͬ%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefStablity,pLegacyElem->design_info.fCoefStablity);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.mN-pLegacyElem->design_info.mN)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ѹ���ȶ�ǿ���ۼ�ϵ��%5.3f��ԭ�е�Ԫ��ͬ%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.mN,pLegacyElem->design_info.mN);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(!pElem->design_info.LoExpression.EqualNoCase(pLegacyElem->design_info.LoExpression))
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ļ��㳤�ȱ��ʽ%s��ԭ�е�Ԫ��ͬ%s\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			(char*)pElem->design_info.LoExpression,(char*)pLegacyElem->design_info.LoExpression);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(!pElem->design_info.lamdaExpression.EqualNoCase(pLegacyElem->design_info.lamdaExpression))
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ĳ�ϸ�ȼ�����ʽ%s��ԭ�е�Ԫ��ͬ%s\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			(char*)pElem->design_info.lamdaExpression,(char*)pLegacyElem->design_info.lamdaExpression);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.bStartRotRest!=pLegacyElem->design_info.bStartRotRest)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ʼ�˳�ϸ���ܿ�Լ��%d��ԭ�е�Ԫ��ͬ%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.bStartRotRest,pLegacyElem->design_info.bStartRotRest);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.bEndRotRest!=pLegacyElem->design_info.bEndRotRest)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ն˳�ϸ���ܿ�Լ��%d��ԭ�е�Ԫ��ͬ%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.bEndRotRest,pLegacyElem->design_info.bEndRotRest);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.lamdaSelType!=pLegacyElem->design_info.lamdaSelType)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ĳ�ϸ���ܿ�����%d��ԭ�е�Ԫ��ͬ%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.lamdaSelType,pLegacyElem->design_info.lamdaSelType);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.cLamdaTypeR!=pLegacyElem->design_info.cLamdaTypeR)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�Ļ�ת������%d��ԭ�е�Ԫ��ͬ%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.cLamdaTypeR,pLegacyElem->design_info.cLamdaTypeR);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.lamdaL-pLegacyElem->design_info.lamdaL)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ļ��㳤��%8.0f��ԭ�е�Ԫ��ͬ%8.0f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.lamdaL,pLegacyElem->design_info.lamdaL);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.lamdaR-pLegacyElem->design_info.lamdaR)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�Ļ�ת�뾶%8.3f��ԭ�е�Ԫ��ͬ%8.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.lamdaR,pLegacyElem->design_info.lamdaR);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.fCoefOfBearLoad-pLegacyElem->design_info.fCoefOfBearLoad)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ĳ���ϵ��%5.3f��ԭ�е�Ԫ��ͬ%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefOfBearLoad,pLegacyElem->design_info.fCoefOfBearLoad);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.Lo_r-pLegacyElem->design_info.Lo_r)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ĳ�ϸ��%8.1f��ԭ�е�Ԫ��ͬ%8.1f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.Lo_r,pLegacyElem->design_info.Lo_r);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.UltimateLo_r-pLegacyElem->design_info.UltimateLo_r)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d������ϸ��%8.0f��ԭ�е�Ԫ��ͬ%8.0f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.UltimateLo_r,pLegacyElem->design_info.UltimateLo_r);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.fCoefEffic-pLegacyElem->design_info.fCoefEffic)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��Ч��ϵ��%5.3f��ԭ�е�Ԫ��ͬ%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefEffic,pLegacyElem->design_info.fCoefEffic);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.iDangerousRule!=pLegacyElem->design_info.iDangerousRule)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ܿ�����%d��ԭ�е�Ԫ��ͬ%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.iDangerousRule,pLegacyElem->design_info.iDangerousRule);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.iDangerousLoadCase!=pLegacyElem->design_info.iDangerousLoadCase)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ܿس�������%d��ԭ�е�Ԫ��ͬ%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.iDangerousLoadCase,pLegacyElem->design_info.iDangerousLoadCase);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//д��ELEMFORCE_ENV MaxTension,MaxCompression,MaxMoment;
#ifdef __ALFA_TEST_VERSION_
		if(fabs(pElem->MaxTension.N-pLegacyElem->MaxTension.N)>0.1)
#else
		if(fabs(pElem->MaxTension.N-pLegacyElem->MaxTension.N)>100)
#endif
		{
			if( pElem->MaxTension.hModule!=pLegacyElem->MaxTension.hModule||pElem->MaxTension.hWorkStatus!=pLegacyElem->MaxTension.hWorkStatus||
				pElem->MaxTension.leg_no[0]!=pLegacyElem->MaxTension.leg_no[0]||
				pElem->MaxTension.leg_no[1]!=pLegacyElem->MaxTension.leg_no[1]||
				pElem->MaxTension.leg_no[2]!=pLegacyElem->MaxTension.leg_no[2]||
				pElem->MaxTension.leg_no[3]!=pLegacyElem->MaxTension.leg_no[3])
			{
				CLDSModule* pModule=Ta.Module.FromHandle(pElem->MaxTension.hModule);
				CLDSModule* pLegacyModule=Ta.Module.FromHandle(pLegacyElem->MaxTension.hModule);
				CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pElem->MaxTension.hWorkStatus.Id());
				CWorkStatus* pLegacyStatus=Ta.WorkStatus.FromHandle(pLegacyElem->MaxTension.hWorkStatus.Id());
				int module_no[2]={0,0},status_no[2]={0,0};
				if(pModule)
					module_no[0]=pModule->iNo;
				if(pLegacyModule)
					module_no[1]=pLegacyModule->iNo;
				if(pStatus)
					status_no[0]=pStatus->iNo;
				if(pLegacyStatus)
					status_no[1]=pLegacyStatus->iNo;
				char group_str[4]="",legacy_group_str[4]="";
				if(pElem->MaxCompStable.hWorkStatus.Group()>0)
					sprintf(group_str,"-%d",pElem->MaxCompStable.hWorkStatus.Group());
				if(pLegacyElem->MaxCompStable.hWorkStatus.Group()>0)
					sprintf(legacy_group_str,"-%d",pLegacyElem->MaxCompStable.hWorkStatus.Group());
				rpt.Log("��ǰ��Ԫ%5d-%5d������������ֹ���{%d-%d%s#%d-%d-%d-%d}��ԭ�е�Ԫ��ͬ{%d-%d%s#%d-%d-%d-%d}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],group_str,pElem->MaxTension.leg_no[0],pElem->MaxTension.leg_no[1],pElem->MaxTension.leg_no[2],pElem->MaxTension.leg_no[3],
					module_no[1],status_no[1],legacy_group_str,pLegacyElem->MaxTension.leg_no[0],pLegacyElem->MaxTension.leg_no[1],pLegacyElem->MaxTension.leg_no[2],pLegacyElem->MaxTension.leg_no[3]);
				agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			}
			rpt.Log("��ǰ��Ԫ%5d-%5d���������%8.1fkN��ԭ�е�Ԫ��ͬ%8.1fkN\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxTension.N*0.001,pLegacyElem->MaxTension.N*0.001);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#ifdef __ALFA_TEST_VERSION_
		if(fabs(pElem->MaxCompression.N-pLegacyElem->MaxCompression.N)>0.1)
#else
		if(fabs(pElem->MaxCompression.N-pLegacyElem->MaxCompression.N)>100)
#endif
		{
			if( pElem->MaxCompression.hModule!=pLegacyElem->MaxCompression.hModule||pElem->MaxCompression.hWorkStatus!=pLegacyElem->MaxCompression.hWorkStatus||
				pElem->MaxCompression.leg_no[0]!=pLegacyElem->MaxCompression.leg_no[0]||
				pElem->MaxCompression.leg_no[1]!=pLegacyElem->MaxCompression.leg_no[1]||
				pElem->MaxCompression.leg_no[2]!=pLegacyElem->MaxCompression.leg_no[2]||
				pElem->MaxCompression.leg_no[3]!=pLegacyElem->MaxCompression.leg_no[3])
			{
				CLDSModule* pModule=Ta.Module.FromHandle(pElem->MaxCompression.hModule);
				CLDSModule* pLegacyModule=Ta.Module.FromHandle(pLegacyElem->MaxCompression.hModule);
				CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pElem->MaxCompression.hWorkStatus.Id());
				CWorkStatus* pLegacyStatus=Ta.WorkStatus.FromHandle(pLegacyElem->MaxCompression.hWorkStatus.Id());
				int module_no[2]={0,0},status_no[2]={0,0};
				if(pModule)
					module_no[0]=pModule->iNo;
				if(pLegacyModule)
					module_no[1]=pLegacyModule->iNo;
				if(pStatus)
					status_no[0]=pStatus->iNo;
				if(pLegacyStatus)
					status_no[1]=pLegacyStatus->iNo;
				char group_str[4]="",legacy_group_str[4]="";
				if(pElem->MaxCompStable.hWorkStatus.Group()>0)
					sprintf(group_str,"-%d",pElem->MaxCompStable.hWorkStatus.Group());
				if(pLegacyElem->MaxCompStable.hWorkStatus.Group()>0)
					sprintf(legacy_group_str,"-%d",pLegacyElem->MaxCompStable.hWorkStatus.Group());
				rpt.Log("��ǰ��Ԫ%5d-%5d�����ѹ�����ֹ���{%d-%d%s#%d-%d-%d-%d}��ԭ�е�Ԫ��ͬ{%d-%d%s#%d-%d-%d-%d}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],group_str,pElem->MaxCompression.leg_no[0],pElem->MaxCompression.leg_no[1],pElem->MaxCompression.leg_no[2],pElem->MaxCompression.leg_no[3],
					module_no[1],status_no[1],legacy_group_str,pLegacyElem->MaxCompression.leg_no[0],pLegacyElem->MaxCompression.leg_no[1],pLegacyElem->MaxCompression.leg_no[2],pLegacyElem->MaxCompression.leg_no[3]);
				agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			}
			rpt.Log("��ǰ��Ԫ%5d-%5d�����ѹ��%8.1fkN��ԭ�е�Ԫ��ͬ%8.1fkN\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompression.N*0.001,pLegacyElem->MaxCompression.N*0.001);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->MaxMoment.N-pLegacyElem->MaxMoment.N)>100)
		{
			if( pElem->MaxMoment.hModule!=pLegacyElem->MaxMoment.hModule||pElem->MaxMoment.hWorkStatus!=pLegacyElem->MaxMoment.hWorkStatus||
				pElem->MaxMoment.leg_no[0]!=pLegacyElem->MaxMoment.leg_no[0]||
				pElem->MaxMoment.leg_no[1]!=pLegacyElem->MaxMoment.leg_no[1]||
				pElem->MaxMoment.leg_no[2]!=pLegacyElem->MaxMoment.leg_no[2]||
				pElem->MaxMoment.leg_no[3]!=pLegacyElem->MaxMoment.leg_no[3])
			{
				CLDSModule* pModule=Ta.Module.FromHandle(pElem->MaxMoment.hModule);
				CLDSModule* pLegacyModule=Ta.Module.FromHandle(pLegacyElem->MaxMoment.hModule);
				CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pElem->MaxMoment.hWorkStatus.Id());
				CWorkStatus* pLegacyStatus=Ta.WorkStatus.FromHandle(pLegacyElem->MaxMoment.hWorkStatus.Id());
				int module_no[2]={0,0},status_no[2]={0,0};
				if(pModule)
					module_no[0]=pModule->iNo;
				if(pLegacyModule)
					module_no[1]=pLegacyModule->iNo;
				if(pStatus)
					status_no[0]=pStatus->iNo;
				if(pLegacyStatus)
					status_no[1]=pLegacyStatus->iNo;
				rpt.Log("��ǰ��Ԫ%5d-%5d�������س��ֹ���{%d-%d#%d-%d-%d-%d}��ԭ�е�Ԫ��ͬ{%d-%d#%d-%d-%d-%d}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],pElem->MaxMoment.leg_no[0],pElem->MaxMoment.leg_no[1],pElem->MaxMoment.leg_no[2],pElem->MaxMoment.leg_no[3],
					module_no[1],status_no[1],pLegacyElem->MaxMoment.leg_no[0],pLegacyElem->MaxMoment.leg_no[1],pLegacyElem->MaxMoment.leg_no[2],pLegacyElem->MaxMoment.leg_no[3]);
				agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			}
			rpt.Log("��ǰ��Ԫ%5d-%5d��������%8.1fN*m��ԭ�е�Ԫ��ͬ%8.1fN*m\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxMoment.N*0.001,pLegacyElem->MaxMoment.N*0.001);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}

#ifdef __ALFA_TEST_VERSION_
		if(fabs(pElem->MaxCompStable.N-pLegacyElem->MaxCompStable.N)>1)
#else
		if(fabs(pElem->MaxCompStable.N-pLegacyElem->MaxCompStable.N)>10)
#endif
		{
			//д��ELEMCOMPSTABLE_ENV MaxCompStable;			//ѹ�ȿ���ʱ�����ѹ�ȼ���Ӧ��
			if( pElem->MaxCompStable.hModule!=pLegacyElem->MaxCompStable.hModule||pElem->MaxCompStable.hWorkStatus!=pLegacyElem->MaxCompStable.hWorkStatus||
				pElem->MaxCompStable.leg_no[0]!=pLegacyElem->MaxCompStable.leg_no[0]||
				pElem->MaxCompStable.leg_no[1]!=pLegacyElem->MaxCompStable.leg_no[1]||
				pElem->MaxCompStable.leg_no[2]!=pLegacyElem->MaxCompStable.leg_no[2]||
				pElem->MaxCompStable.leg_no[3]!=pLegacyElem->MaxCompStable.leg_no[3])
			{
				CLDSModule* pModule=Ta.Module.FromHandle(pElem->MaxCompStable.hModule);
				CLDSModule* pLegacyModule=Ta.Module.FromHandle(pLegacyElem->MaxCompStable.hModule);
				CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pElem->MaxCompStable.hWorkStatus.Id());
				CWorkStatus* pLegacyStatus=Ta.WorkStatus.FromHandle(pLegacyElem->MaxCompStable.hWorkStatus.Id());
				int module_no[2]={0,0},status_no[2]={0,0};
				if(pModule)
					module_no[0]=pModule->iNo;
				if(pLegacyModule)
					module_no[1]=pLegacyModule->iNo;
				if(pStatus)
					status_no[0]=pStatus->iNo;
				if(pLegacyStatus)
					status_no[1]=pLegacyStatus->iNo;
				char group_str[4]="",legacy_group_str[4]="";
				if(pElem->MaxCompStable.hWorkStatus.Group()>0)
					sprintf(group_str,"-%d",pElem->MaxCompStable.hWorkStatus.Group());
				if(pLegacyElem->MaxCompStable.hWorkStatus.Group()>0)
					sprintf(legacy_group_str,"-%d",pLegacyElem->MaxCompStable.hWorkStatus.Group());
				rpt.Log("��ǰ��Ԫ%5d-%5d�����ѹ�������ֹ���{%d-%d%s#%d-%d-%d-%d}��ԭ�е�Ԫ��ͬ{%d-%d%s#%d-%d-%d-%d}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],group_str,pElem->MaxCompStable.leg_no[0],pElem->MaxCompStable.leg_no[1],pElem->MaxCompStable.leg_no[2],pElem->MaxCompStable.leg_no[3],
					module_no[1],status_no[1],legacy_group_str,pLegacyElem->MaxCompStable.leg_no[0],pLegacyElem->MaxCompStable.leg_no[1],pLegacyElem->MaxCompStable.leg_no[2],pLegacyElem->MaxCompStable.leg_no[3]);
				agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			}
			rpt.Log("��ǰ��Ԫ%5d-%5d�����ѹ����%8.3fkN��ԭ�е�Ԫ��ͬ%8.3fkN\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.N*0.001,pLegacyElem->MaxCompStable.N*0.001);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->MaxCompStable.sigema-pLegacyElem->MaxCompStable.sigema)>0.1)
		{
			double correctLegacySigema=pLegacyElem->MaxCompStable.sigema;
			if(!pLegacyElem->MaxCompStable.bothComp&&pLegacyElem->MaxCompStable.Afai_tenscomp>0)
				correctLegacySigema=pLegacyElem->MaxCompStable.N/pLegacyElem->MaxCompStable.Afai_tenscomp;
			else if(pLegacyElem->MaxCompStable.bothComp&&pLegacyElem->MaxCompStable.Afai_bothcomp>0)
				correctLegacySigema=pLegacyElem->MaxCompStable.N/pLegacyElem->MaxCompStable.Afai_bothcomp;
			if(fabs(pElem->MaxCompStable.sigema-correctLegacySigema)>0.1)
			{
				rpt.Log("��ǰ��Ԫ%5d-%5d��ѹ��Ӧ��%8.1fMPa��ԭ�е�Ԫ��ͬ%8.1fMPa\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					pElem->MaxCompStable.sigema,correctLegacySigema);
				agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			}
		}
		//���ڸ����ĵ�Ԫ��ʾʼĩ����֧�ŵ������ĵ�Ԫ�����ڼ��㸨������ѹ��
		//CElement *pStartBraceForceElem,*pEndBraceForceElem;
	}
	//�洢�˼�������˨����ϵ��
	agent.m_uType=INFO_COMPARE_REPORT_RODS;
	for(ROD_BEARLOADCOEF *pRod=legacyreport.hashRodConnect.GetFirst();pRod;pRod=legacyreport.hashRodConnect.GetNext())
	{
		CSuperSmartPtr<CLDSPart> pRodPart=Ta.Parts.FromHandle(pRod->hRod);
		if( pRodPart->IsLinePart()&&(
			fabs(pRod->fStartBearCoef-pRodPart.LinePartPointer()->connectStart.bearloadcoef)>0.001||
			fabs(pRod->fEndBearCoef-pRodPart.LinePartPointer()->connectEnd.bearloadcoef)>0.001))
		{
			rpt.Log("ԭ�и˼�0X%Xʼĩ����˨����ϵ��Ϊ��%5.3f-%5.3f,��ǰʼĩ����˨����ϵ��Ϊ��%5.3f-%5.3f\n",
				pRod->hRod,pRod->fStartBearCoef,pRod->fEndBearCoef,
				pRodPart.LinePartPointer()->connectStart.bearloadcoef,pRodPart.LinePartPointer()->connectEnd.bearloadcoef);
			agent.Append(pRodPart->handle,0,rpt.LatestLog());
		}
	}
	COutputInfoDlg *pTaskListDlg = (COutputInfoDlg*)((CMainFrame*)theApp.m_pMainWnd)->GetOutputInfoPage();
	//if(!g_sysPara.dock.pageOutput.bDisplay)
	//	((CMainFrame*)theApp.m_pMainWnd)->OnDockpageOutput();
	((CMainFrame*)theApp.m_pMainWnd)->ActivateDockpage(RUNTIME_CLASS(COutputInfoDlg));
	pTaskListDlg->InitOutputInfoList(infoList);
	char sTitle[50]="";
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"��ǰ��%d������",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"��ǰ��0������");
	pTaskListDlg->SetInfoTitle(sTitle);
	CRevisionOperDlg *pRevision=(CRevisionOperDlg*)((CMainFrame*)theApp.m_pMainWnd)->GetRevisionOperPage();
	pRevision->UpdateRevision(postreport);
	return true;
}
#include "TtaDataFile.h"
bool CPostProcessReport::ReadFromTtaRsltOutFile(LPCTSTR lpszPathName/*=NULL*/)
{
	//������˨���˼������Ϣ
	CLDSDoc *pDoc=theApp.GetLDSDoc();
	CString outfile;
	if(lpszPathName==NULL)
	{
		outfile=pDoc->GetPathName();
		outfile=outfile.Left(outfile.GetLength()-4);
		outfile+=".out";
		CFileDialog dlg(TRUE,"out",outfile,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"TTA����������ļ�(*.out)|*.out||");
		if(dlg.DoModal()!=IDOK)
			return false;
		outfile=dlg.GetPathName();
	}
	else
		outfile=lpszPathName;
	FILE* fp=fopen(outfile,"rt");
	if(fp==NULL)
		return false;	//δ�ҵ����㱨������ļ�
	CTtaDataFile tta;
	tta.ReadOutFile(fp,NULL,this);
	fclose(fp);
	return true;
}
bool CPostProcessReport::CompareWithOtherProgramReport(CPostProcessReport& outside, CLogFile& rpt)
{
	//�ȶԽڵ���Ϣ���
	CTaskOutputList infoList;
	COutputInfoAgent agent(&infoList);
	agent.ListAppendFunc=AppendOutputInfoToList;
	agent.m_uType=INFO_COMPARE_REPORT_WARNNING;
	/*if(outside.Reaction.GetNodeNum()!=this->Reaction.GetNodeNum())
	{
		rpt.Log("��������ԭ������Ϊ%d,��ǰ����Ϊ%d\n",outside.Reaction.GetNodeNum(),this->Reaction.GetNodeNum());
		agent.Append(0,0,rpt.LatestLog());
		return false;
	}
	agent.m_uType=INFO_COMPARE_REPORT_NODES;
	BASENODE_REACTION *pLegacyReact=outside.Reaction.GetFirst();
	for(BASENODE_REACTION *pReact=this->Reaction.GetFirst();pReact;pReact=this->Reaction.GetNext())
	{
		if(pReact->pNode!=pLegacyReact->pNode)
			rpt.Log("ԭ�л����ڵ�{%d}��ͬ�ڵ�ǰ�����ڵ�{%d}\n",pLegacyReact->pNode->point_i,pReact->pNode->point_i);
		if(pReact->pModule!=pLegacyReact->pModule||pReact->pStatus!=pLegacyReact->pStatus)
			rpt.Log("ԭ�л����ڵ�{%d}�Ĺ���{%d-%d}��ͬ�ڵ�ǰ�����ڵ�{%d}�Ĺ���{%d-%d}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->pModule->iNo,pLegacyReact->pStatus->iNo,
			pReact->pNode->point_i,pReact->pModule->iNo,pReact->pStatus->iNo);
		if( pReact->leg_no[0]!=pLegacyReact->leg_no[0]||pReact->leg_no[1]!=pLegacyReact->leg_no[1]||
			pReact->leg_no[2]!=pLegacyReact->leg_no[2]||pReact->leg_no[3]!=pLegacyReact->leg_no[3])
			rpt.Log("ԭ�л����ڵ�{%d}�Ľ���{%d-%d-%d-%d}��ͬ�ڵ�ǰ�����ڵ�{%d}�Ĺ���{%d-%d-%d-%d}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->leg_no[0],pLegacyReact->leg_no[1],pLegacyReact->leg_no[2],pLegacyReact->leg_no[3],
			pReact->pNode->point_i,pReact->leg_no[0],pReact->leg_no[1],pReact->leg_no[2],pReact->leg_no[3]);
		GEPOINT diff=pLegacyReact->reaction_force_design-pReact->reaction_force_design;
		if(!pReact->reaction_force_design.IsEqual(pLegacyReact->reaction_force_design,0.5))
			rpt.Log("ԭ�л�����Ʒ���{%d��%d-%d}�ϵ�ǰ������Ʒ���{%d��%d-%d}�����{%8.3f %8.3f %8.3f kN}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->pModule->iNo,pLegacyReact->pStatus->iNo,
			pReact->pNode->point_i,pReact->pModule->iNo,pReact->pStatus->iNo,
			diff.x*0.001,diff.y*0.001,diff.z*0.001);
		pLegacyReact=outside.Reaction.GetNext();
	}
	if(outside.TowerWeight.GetNodeNum()!=this->TowerWeight.GetNodeNum())
	{
		rpt.Log("�����������ͳ�Ƶ�Ԫԭ����Ϊ%d,��ǰ����Ϊ%d\n",outside.TowerWeight.GetNodeNum(),this->TowerWeight.GetNodeNum());
		return false;
	}
	TOWERWEIGHT *pLegacyWeight=outside.TowerWeight.GetFirst();
	for(TOWERWEIGHT *pTowerWeight=this->TowerWeight.GetFirst();pTowerWeight;pTowerWeight=this->TowerWeight.GetNext())
	{
		if(pLegacyWeight->cfg_no!=pTowerWeight->cfg_no||pLegacyWeight->iBody0Leg1!=pTowerWeight->iBody0Leg1||
			fabs(pLegacyWeight->weight-pTowerWeight->weight)>0.1)
		{
			char cLegacy='B',cCurrent='B';
			if(pLegacyWeight->iBody0Leg1==1)
				cLegacy='L';
			if(pTowerWeight->iBody0Leg1==1)
				cCurrent='L';
			rpt.Log("����ͳ�Ƽ�¼ԭ��¼{%C%d=%.1f}�뵱ǰ��¼{%C%d=%.1f}��ƥ��\n",
				cLegacy,pLegacyWeight->cfg_no,pLegacyWeight->weight,cCurrent,pTowerWeight->cfg_no,pTowerWeight->weight);
		}
		pLegacyWeight=outside.TowerWeight.GetNext();
	}*/
	if(outside.hashElems.GetNodeNum()!=this->hashElems.GetNodeNum())
		rpt.Log("��Ԫԭ����Ϊ%d,��ǰ����Ϊ%d\n",outside.hashElems.GetNodeNum(),this->hashElems.GetNodeNum());
	agent.m_uType=INFO_COMPARE_REPORT_ELEMS;
	CElement *pLegacyElem=outside.hashElems.GetFirst();
	COutputInfo* pOutput=NULL;
	for(CElement *pElem=this->hashElems.GetFirst();pElem;pElem=this->hashElems.GetNext())
	{
		if((pLegacyElem=outside.hashElems.GetValue(pElem->id))==NULL)
		{
			int minPointI=min(pElem->pStart->point_i,pElem->pEnd->point_i);
			int bigPointI=max(pElem->pStart->point_i,pElem->pEnd->point_i);
			rpt.Log("�ⲿ���㱨����ȱʧ��Ԫ{%5d:%5d-%5d}",pElem->id,minPointI,bigPointI);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_OUTSIDE_MISS;
			continue;
		}
		//rpt.InsertBlankRow();
		long arrStartI[2],arrEndI[2];
		arrStartI[0]=min(pLegacyElem->pStart->point_i,pLegacyElem->pEnd->point_i);
		arrEndI[0]=max(pLegacyElem->pStart->point_i,pLegacyElem->pEnd->point_i);
		arrStartI[1]=min(pElem->pStart->point_i,pElem->pEnd->point_i);
		arrEndI[1]=max(pElem->pStart->point_i,pElem->pEnd->point_i);
		if(pElem->id!=pLegacyElem->id||arrStartI[1]!=arrStartI[0]||
			arrEndI[1]!=arrEndI[0])
		{
			rpt.Log("ԭ�е�Ԫ{%5d:%5d-%5d}�뵱ǰ��Ԫ{%5d:%5d-%5d}��ʶ��ͬ\n",
			pLegacyElem->id,arrStartI[0],arrEndI[0],pElem->id,arrStartI[1],arrEndI[1]);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#ifdef __ALFA_TEST_VERSION_
		if(pElem->relaElem.idRefElem!=pLegacyElem->relaElem.idRefElem)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d����ѹ���յ�Ԫ��ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->relaElem.idRefElem2!=pLegacyElem->relaElem.idRefElem2)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ͬ�˽ڵ���ѹ���յ�Ԫ��ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#endif
		//double W0,H0,V0;		//����Ԥ�Ž�����������(N)�����ɶ���Ԥ���������²���������ˮƽ��������ֱ����
		//bool bAnchorEnd;		//true:�����ն˶Ե�ê��;false:����ʼ�˶Ե�ê��
		//short elem_type;	//0:��ʾ�����˻򾭵���;1:��ʾ������Ԫ
		//��С����㳤����Ϣ
		if(pElem->MaxCompStable.bothComp!=pLegacyElem->MaxCompStable.bothComp)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ͬʱ��ѹ������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.bothComp,pLegacyElem->MaxCompStable.bothComp);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_BOTH_COMPRESSION;
			pOutput->compare.bValue=pLegacyElem->MaxCompStable.bothComp;
		}
		/*һ��ȶ�ʱ���Ƕ�ȡTTA����ָ���˶�״̬,TTA���㱨����Ҳû�и˶�״̬,�ʲ��ȶԸ˶�״̬
		if(pElem->m_bValidStartRestStiff!=pLegacyElem->m_bValidStartRestStiff)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ʼ��ת��Լ��������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->m_bValidEndRestStiff!=pLegacyElem->m_bValidEndRestStiff)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ն�ת��Լ��������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//xx_joint_type��������ԣ���real_xx_joint_type�����ʵ�ʼ���ʱ��֧������
		//0����ȫ�½�JOINT_HINGE��1�����ָս�JOINT_RIGID�����Ƴ�Ťת���ת��Լ����,2���Զ��ж�JOINT_AUTOJUSTIFY
		if(pElem->start_joint_type!=pLegacyElem->start_joint_type)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ȱʡʼ������������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->end_joint_type!=pLegacyElem->end_joint_type)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ȱʡ�ն�����������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->real_start_joint_type!=pLegacyElem->real_start_joint_type)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ʼ������������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->real_end_joint_type!=pLegacyElem->real_end_joint_type)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ն�����������ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->start_force_type!=pLegacyElem->start_force_type)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ʼ�˸˶�����״̬��ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->end_force_type!=pLegacyElem->end_force_type)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ն˸˶�����״̬��ԭ�е�Ԫ��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//д�������Ϣdesign_info;
		//һ��ȶ�ʱ���Ƕ�ȡTTA����ָ������ϵ��,TTA���㱨����Ҳû�м�����,�ʲ��ȶԼ���ϵ��
		if(fabs(pElem->design_info.hole_out-pLegacyElem->design_info.hole_out)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ļ���ϵ��%5.3f��ԭ�е�Ԫ��ͬ%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.hole_out,pLegacyElem->design_info.hole_out);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		*/
		if( pElem->design_info.iDangerousLoadCase==2&&
			fabs(pElem->design_info.fCoefTensStrengthDecrease-pLegacyElem->design_info.fCoefTensStrengthDecrease)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d������ǿ���ۼ�ϵ��%5.3f��ԭ�е�Ԫ%5.3f��ͬ\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefTensStrengthDecrease,pLegacyElem->design_info.fCoefTensStrengthDecrease);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_TENSION_REDUCTION_COEF;
			pOutput->compare.fValue=pLegacyElem->design_info.fCoefTensStrengthDecrease;
		}
		if( pElem->design_info.iDangerousLoadCase>=3&&
			fabs(pElem->design_info.fCoefCompStrengthDecrease-pLegacyElem->design_info.fCoefCompStrengthDecrease)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d����ѹǿ���ۼ�ϵ��%5.3f��ԭ�е�Ԫ��ͬ%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefCompStrengthDecrease,pLegacyElem->design_info.fCoefCompStrengthDecrease);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_COMPRESSION_REDUCTION_COEF;
			pOutput->compare.fValue=pLegacyElem->design_info.fCoefCompStrengthDecrease;
		}
		if(pElem->design_info.bBlockShearActivated!=pLegacyElem->design_info.bBlockShearActivated)
		{
			CXhChar16 state("False"),legacy("False");
			if(pElem->design_info.bBlockShearActivated)
				state.Copy("True");
			if(pLegacyElem->design_info.bBlockShearActivated)
				legacy.Copy("True");
			rpt.Log("��ǰ��Ԫ%5d-%5d�Ŀ���ܿ�״̬%s��ԭ�е�Ԫ��ͬ%s\n",pElem->pStart->point_i,pElem->pEnd->point_i,(char*)state,(char*)legacy);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_BLOCK_SHEAR_STATE;
			pOutput->compare.bValue=pLegacyElem->design_info.bBlockShearActivated;
		}
		if( pElem->MaxCompression.N<CElement::COMPRESSION_ZERO_FORCE&&
			fabs(pElem->design_info.fCoefStablity-pLegacyElem->design_info.fCoefStablity)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ѹ��ϵ��%5.3f��ԭ�е�Ԫ��ͬ%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefStablity,pLegacyElem->design_info.fCoefStablity);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_STABLE_COEF;
			pOutput->compare.fValue=pLegacyElem->design_info.fCoefStablity;
		}
		if( pElem->MaxCompression.N<CElement::COMPRESSION_ZERO_FORCE&&
			fabs(pElem->design_info.mN-pLegacyElem->design_info.mN)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ѹ���ȶ�ǿ���ۼ�ϵ��%5.3f��ԭ�е�Ԫ��ͬ%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.mN,pLegacyElem->design_info.mN);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_STABLE_mN;
			pOutput->compare.fValue=pLegacyElem->design_info.mN;
		}
		//TODO:ʼ�˳�ϸ���ܿ�Լ�����Ϳ��Ը���ԭʼ���������ļ����в��ֱȶԣ�δ���������ơ�wjh-2017.2.6
		/*if(pElem->design_info.bStartRotRest!=pLegacyElem->design_info.bStartRotRest)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��ʼ�˳�ϸ���ܿ�Լ��%d��ԭ�е�Ԫ��ͬ%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.bStartRotRest,pLegacyElem->design_info.bStartRotRest);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.bEndRotRest!=pLegacyElem->design_info.bEndRotRest)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ն˳�ϸ���ܿ�Լ��%d��ԭ�е�Ԫ��ͬ%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.bEndRotRest,pLegacyElem->design_info.bEndRotRest);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.lamdaSelType!=pLegacyElem->design_info.lamdaSelType)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ĳ�ϸ���ܿ�����%d��ԭ�е�Ԫ��ͬ%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.lamdaSelType,pLegacyElem->design_info.lamdaSelType);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.cLamdaTypeR!=pLegacyElem->design_info.cLamdaTypeR)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�Ļ�ת������%d��ԭ�е�Ԫ��ͬ%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.cLamdaTypeR,pLegacyElem->design_info.cLamdaTypeR);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}*/
		if(ftoi(pElem->design_info.lamdaL)!=ftoi(pLegacyElem->design_info.lamdaL))
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ļ��㳤��%8.0f��ԭ�е�Ԫ��ͬ%8.0f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.lamdaL,pLegacyElem->design_info.lamdaL);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_LAMDA_L;
			pOutput->compare.fValue=pLegacyElem->design_info.lamdaL;
		}
		if(fabs(pElem->design_info.lamdaR-pLegacyElem->design_info.lamdaR)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�Ļ�ת�뾶%8.3f��ԭ�е�Ԫ��ͬ%8.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.lamdaR,pLegacyElem->design_info.lamdaR);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_GYRATION;
			pOutput->compare.fValue=pLegacyElem->design_info.lamdaR;
		}
		if(ftoi(pElem->design_info.Lo_r)!=ftoi(pLegacyElem->design_info.Lo_r))
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ĳ�ϸ��%8.1f��ԭ�е�Ԫ��ͬ%8.1f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.Lo_r,pLegacyElem->design_info.Lo_r);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_LAMDA;
			pOutput->compare.fValue=pLegacyElem->design_info.Lo_r;
		}
		//Ŀǰ�������TTA�������ļ����������ϸ�ȿ���ʱ�������� wjh-2017.2.6
		/*if(fabs(pElem->design_info.fCoefEffic-pLegacyElem->design_info.fCoefEffic)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d��Ч��ϵ��%5.3f��ԭ�е�Ԫ��ͬ%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefEffic,pLegacyElem->design_info.fCoefEffic);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.iDangerousRule!=pLegacyElem->design_info.iDangerousRule)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ܿ�����%d��ԭ�е�Ԫ��ͬ%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.iDangerousRule,pLegacyElem->design_info.iDangerousRule);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}*/
		if(pElem->design_info.iDangerousLoadCase!=pLegacyElem->design_info.iDangerousLoadCase)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d���ܿس�������%d��ԭ�е�Ԫ��ͬ%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.iDangerousLoadCase,pLegacyElem->design_info.iDangerousLoadCase);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_DANGEROUS_CASE;
			pOutput->compare.iValue=pLegacyElem->design_info.iDangerousLoadCase;
		}
		if(fabs(pElem->design_info.fCoefOfBearLoad-pLegacyElem->design_info.fCoefOfBearLoad)>0.001)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�ĳ���ϵ��%5.3f��ԭ�е�Ԫ��ͬ%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefOfBearLoad,pLegacyElem->design_info.fCoefOfBearLoad);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_BEARLOAD_COEF;
			pOutput->compare.fValue=pLegacyElem->design_info.fCoefOfBearLoad;
		}
		//д��ELEMFORCE_ENV MaxTension,MaxCompression,MaxMoment;
#ifdef __ALFA_TEST_VERSION_
		if(fabs(pElem->MaxTension.N-pLegacyElem->MaxTension.N)>0.1)
#else
		if(fabs(pElem->MaxTension.N-pLegacyElem->MaxTension.N)>100)
#endif
		{
			if(pElem->MaxTension.hModule!=pLegacyElem->MaxTension.hModule||pElem->MaxTension.hWorkStatus!=pLegacyElem->MaxTension.hWorkStatus)
			{
				CLDSModule* pModule=Ta.Module.FromHandle(pElem->MaxTension.hModule);
				CLDSModule* pLegacyModule=Ta.Module.FromHandle(pLegacyElem->MaxTension.hModule);
				CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pElem->MaxTension.hWorkStatus.Id());
				CWorkStatus* pLegacyStatus=Ta.WorkStatus.FromHandle(pLegacyElem->MaxTension.hWorkStatus.Id());
				int module_no[2]={0,0},status_no[2]={0,0};
				if(pModule)
					module_no[0]=pModule->iNo;
				if(pLegacyModule)
					module_no[1]=pLegacyModule->iNo;
				if(pStatus)
					status_no[0]=pStatus->iNo;
				if(pLegacyStatus)
					status_no[1]=pLegacyStatus->iNo;
				char group_str[4]="",legacy_group_str[4]="";
				if(pElem->MaxCompStable.hWorkStatus.Group()>0)
					sprintf(group_str,"-%d",pElem->MaxCompStable.hWorkStatus.Group());
				if(pLegacyElem->MaxCompStable.hWorkStatus.Group()>0)
					sprintf(legacy_group_str,"-%d",pLegacyElem->MaxCompStable.hWorkStatus.Group());
				rpt.Log("��ǰ��Ԫ%5d-%5d������������ֹ���{%d-%d%s}��ԭ�е�Ԫ��ͬ{%d-%d%s}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],group_str,module_no[1],status_no[1],legacy_group_str);
				pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
				pOutput->m_idElem=pElem->id;
				pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_TENSION_CASE;
				CXhString casestr(pOutput->compare.strbuf,16,"%d-%d%s",module_no[1],status_no[1],legacy_group_str);
			}
			rpt.Log("��ǰ��Ԫ%5d-%5d���������%8.1fkN��ԭ�е�Ԫ��ͬ%8.1fkN\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxTension.N*0.001,pLegacyElem->MaxTension.N*0.001);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_TENSION;
			pOutput->compare.fValue=pLegacyElem->MaxTension.N;
		}
#ifdef __ALFA_TEST_VERSION_
		if(fabs(pElem->MaxCompression.N-pLegacyElem->MaxCompression.N)>0.1)
#else
		if(fabs(pElem->MaxCompression.N-pLegacyElem->MaxCompression.N)>100)
#endif
		{
			if( pElem->MaxCompression.hModule!=pLegacyElem->MaxCompression.hModule||pElem->MaxCompression.hWorkStatus!=pLegacyElem->MaxCompression.hWorkStatus)
			{
				CLDSModule* pModule=Ta.Module.FromHandle(pElem->MaxCompression.hModule);
				CLDSModule* pLegacyModule=Ta.Module.FromHandle(pLegacyElem->MaxCompression.hModule);
				CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pElem->MaxCompression.hWorkStatus.Id());
				CWorkStatus* pLegacyStatus=Ta.WorkStatus.FromHandle(pLegacyElem->MaxCompression.hWorkStatus.Id());
				int module_no[2]={0,0},status_no[2]={0,0};
				if(pModule)
					module_no[0]=pModule->iNo;
				if(pLegacyModule)
					module_no[1]=pLegacyModule->iNo;
				if(pStatus)
					status_no[0]=pStatus->iNo;
				if(pLegacyStatus)
					status_no[1]=pLegacyStatus->iNo;
				char group_str[4]="",legacy_group_str[4]="";
				if(pElem->MaxCompStable.hWorkStatus.Group()>0)
					sprintf(group_str,"-%d",pElem->MaxCompStable.hWorkStatus.Group());
				if(pLegacyElem->MaxCompStable.hWorkStatus.Group()>0)
					sprintf(legacy_group_str,"-%d",pLegacyElem->MaxCompStable.hWorkStatus.Group());
				rpt.Log("��ǰ��Ԫ%5d-%5d�����ѹ�����ֹ���{%d-%d%s}��ԭ�е�Ԫ��ͬ{%d-%d%s}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],group_str,module_no[1],status_no[1],legacy_group_str);
				pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
				pOutput->m_idElem=pElem->id;
				pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_COMPRESSION_CASE;
				CXhString casestr(pOutput->compare.strbuf,16,"%d-%d%s",module_no[1],status_no[1],legacy_group_str);
			}
			rpt.Log("��ǰ��Ԫ%5d-%5d�����ѹ��%8.1fkN��ԭ�е�Ԫ��ͬ%8.1fkN\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompression.N*0.001,pLegacyElem->MaxCompression.N*0.001);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_COMPRESSION;
			pOutput->compare.fValue=pLegacyElem->MaxCompression.N;
		}
#ifdef __ALFA_TEST_VERSION_
		if(fabs(pElem->MaxCompStable.N-pLegacyElem->MaxCompStable.N)>1)
#else
		if(fabs(pElem->MaxCompStable.N-pLegacyElem->MaxCompStable.N)>10)
#endif
		{
			//д��ELEMCOMPSTABLE_ENV MaxCompStable;			//ѹ�ȿ���ʱ�����ѹ�ȼ���Ӧ��
			if( pElem->MaxCompStable.hModule!=pLegacyElem->MaxCompStable.hModule||pElem->MaxCompStable.hWorkStatus!=pLegacyElem->MaxCompStable.hWorkStatus)
			{
				CLDSModule* pModule=Ta.Module.FromHandle(pElem->MaxCompStable.hModule);
				CLDSModule* pLegacyModule=Ta.Module.FromHandle(pLegacyElem->MaxCompStable.hModule);
				CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pElem->MaxCompStable.hWorkStatus.Id());
				CWorkStatus* pLegacyStatus=Ta.WorkStatus.FromHandle(pLegacyElem->MaxCompStable.hWorkStatus.Id());
				int module_no[2]={0,0},status_no[2]={0,0};
				if(pModule)
					module_no[0]=pModule->iNo;
				if(pLegacyModule)
					module_no[1]=pLegacyModule->iNo;
				if(pStatus)
					status_no[0]=pStatus->iNo;
				if(pLegacyStatus)
					status_no[1]=pLegacyStatus->iNo;
				char group_str[4]="",legacy_group_str[4]="";
				if(pElem->MaxCompStable.hWorkStatus.Group()>0)
					sprintf(group_str,"-%d",pElem->MaxCompStable.hWorkStatus.Group());
				if(pLegacyElem->MaxCompStable.hWorkStatus.Group()>0)
					sprintf(legacy_group_str,"-%d",pLegacyElem->MaxCompStable.hWorkStatus.Group());
				rpt.Log("��ǰ��Ԫ%5d-%5d�����ѹ�������ֹ���{%d-%d%s}��ԭ�е�Ԫ��ͬ{%d-%d%s}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],group_str,module_no[1],status_no[1],legacy_group_str);
				pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
				pOutput->m_idElem=pElem->id;
				pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_STABLE_CASE;
				CXhString casestr(pOutput->compare.strbuf,16,"%d-%d%s",module_no[1],status_no[1],legacy_group_str);
			}
			rpt.Log("��ǰ��Ԫ%5d-%5d�����ѹ����%8.3fkN��ԭ�е�Ԫ��ͬ%8.3fkN\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.N*0.001,pLegacyElem->MaxCompStable.N*0.001);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_STABLE_N;
			pOutput->compare.fValue=pLegacyElem->MaxCompStable.N;
		}
		if(fabs(pElem->MaxCompStable.sigema-pLegacyElem->MaxCompStable.sigema)>0.1)
		{
			double correctLegacySigema=pLegacyElem->MaxCompStable.sigema;
			if(!pLegacyElem->MaxCompStable.bothComp&&pLegacyElem->MaxCompStable.Afai_tenscomp>0)
				correctLegacySigema=pLegacyElem->MaxCompStable.N/pLegacyElem->MaxCompStable.Afai_tenscomp;
			else if(pLegacyElem->MaxCompStable.bothComp&&pLegacyElem->MaxCompStable.Afai_bothcomp>0)
				correctLegacySigema=pLegacyElem->MaxCompStable.N/pLegacyElem->MaxCompStable.Afai_bothcomp;
			if(fabs(pElem->MaxCompStable.sigema-correctLegacySigema)>0.1)
			{
				rpt.Log("��ǰ��Ԫ%5d-%5d��ѹ��Ӧ��%8.1fMPa��ԭ�е�Ԫ��ͬ%8.1fMPa\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					pElem->MaxCompStable.sigema,correctLegacySigema);
				pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
				pOutput->m_idElem=pElem->id;
				pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_STABLE_SIGEMA;
				pOutput->compare.fValue=correctLegacySigema;
			}
		}
		//ʵʱ���㹤��Ӧ��
		double coefStrength=1.0;
		double A=pElem->pOrgPole->GetArea();
		double An=A-pElem->design_info.hole_out*pElem->design_info.hole_d*pElem->pOrgPole->GetThick();
		if(pElem->design_info.iDangerousLoadCase==2)		//T
		{
			coefStrength=pElem->design_info.fCoefTensStrengthDecrease;
			pElem->design_info.workSigema=pElem->MaxTension.N/An;
		}
		else if(pElem->design_info.iDangerousLoadCase==3)	//C
		{
			coefStrength=pElem->design_info.fCoefCompStrengthDecrease;
			pElem->design_info.workSigema=pElem->MaxCompression.N/An;
		}
		else //if(pElem->design_info.iDangerousLoadCase==4)	//S
		{
			coefStrength=pElem->design_info.mN;
			if(pElem->MaxCompression.N>-EPS||pElem->design_info.fCoefStablity<EPS)
				pElem->design_info.workSigema=0;
			else	//�������ѹ��Ӧ�����ֹ�����һ�������ѹ��������һ�£��ʲ��������ѹ������ѹ��Ӧ�����ƹ����µ�ѹ��ϵ�� wjh-2013.10.15
				pElem->design_info.workSigema=pElem->MaxCompStable.N/(A*pElem->design_info.fCoefStablity);
		}
		if(fabs(pElem->design_info.workSigema-pLegacyElem->design_info.workSigema)>0.1)
		{
			rpt.Log("��ǰ��Ԫ%5d-%5d�Ĺ���Ӧ��%8.1fMPa��ԭ�е�Ԫ��ͬ%8.1fMPa\n",pElem->pStart->point_i,pElem->pEnd->point_i,
				pElem->design_info.workSigema,pLegacyElem->design_info.workSigema);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_WORK_SIGEMA;
			pOutput->compare.fValue=pLegacyElem->design_info.workSigema;
		}
		//���ڸ����ĵ�Ԫ��ʾʼĩ����֧�ŵ������ĵ�Ԫ�����ڼ��㸨������ѹ��
		//CElement *pStartBraceForceElem,*pEndBraceForceElem;
	}
	COutputInfoDlg *pTaskListDlg = (COutputInfoDlg*)((CMainFrame*)theApp.m_pMainWnd)->GetOutputInfoPage();
	//if(!g_sysPara.dock.pageOutput.bDisplay)
	//	((CMainFrame*)theApp.m_pMainWnd)->OnDockpageOutput();
	((CMainFrame*)theApp.m_pMainWnd)->ActivateDockpage(RUNTIME_CLASS(COutputInfoDlg));
	pTaskListDlg->InitOutputInfoList(infoList);
	char sTitle[50]="";
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"��ǰ��%d������",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"��ǰ��0������");
	pTaskListDlg->SetInfoTitle(sTitle);
	CRevisionOperDlg *pRevision=(CRevisionOperDlg*)((CMainFrame*)theApp.m_pMainWnd)->GetRevisionOperPage();
	pRevision->UpdateRevision(postreport);
	return true;
}
int compare_node_anal_func(const CNodeAnalyzeInfo& node1,const CNodeAnalyzeInfo& node2)
{
	if(node1.pNode->point_i<node2.pNode->point_i)
		return -1;
	else if(node1.pNode->point_i==node2.pNode->point_i)
		return 0;
	else
		return 1;
}
//���������ɷ��������ļ��������ǶԳ���ϲ�����
int compare_elem_anal_func(const ELEM_ANALYZE_INFO &elem1,const ELEM_ANALYZE_INFO &elem2)
{
	long no_start[2],no_end[2];
	no_start[0]=min(elem1.iStartI,elem1.iEndI);
	no_end[0]=max(elem1.iStartI,elem1.iEndI);
	no_start[1]=min(elem2.iStartI,elem2.iEndI);
	no_end[1]=max(elem2.iStartI,elem2.iEndI);
	if(no_start[0]<no_start[1])
		return -1;
	else if(no_start[0]==no_start[1])
	{
		if(no_end[0]<no_end[1])
			return -1;
		else if(no_end[0]==no_end[1])
			return 0;
		else
			return 1;
	}
	else
		return 1;
}
bool CPostProcessReport::CreateAnalyzeFile(char* file_name,long iModuleNo,DWORD dwQuadLeg,SUBID iStatusIncEiffel)
{
	char file_path[MAX_PATH];
	strcpy(file_path,file_name);
	strcat(file_path,".anl");
	FILE *fp=fopen(file_path,"wt");
	if(fp==NULL)
	{
		logerr.Log("�ļ�'%s'д���ʧ��!",file_path);
		return FALSE;
	}
	CLogErrorLife loglife;
	fprintf(fp,"\n");
	if(!m_bLinearAnalysis)
	{
		fprintf(fp,"  No.  ��x(mm)  ��y(mm)  ��z(mm)   Fex(kN)  Fey(kN)  Fez(kN)  Fwx(kN)  Fwy(kN)  Fu (kN)\n");
		fprintf(fp,"---------------------------------------------------------------------------------------\n");
	}
	else
	{
		fprintf(fp,"  No.  ��x(mm)  ��y(mm)  ��z(mm)   Fex(kN)  Fey(kN)  Fez(kN)  Fwx(kN)  Fwy(kN)\n");
		fprintf(fp,"---------------------------------------------------------------------------------\n");
	}
	int i=0;
	CWorkStatus *pStatus;
	ARRAY_LIST<CNodeAnalyzeInfo> node_arr(0,1000);
	ARRAY_LIST<ELEM_ANALYZE_INFO> elem_arr(0,1000);
	if(dwQuadLeg==0)
		dwQuadLeg=0x01010101;
	CNodeAnalyzeInfo anal_node;
	hashNodeAnalBuffer.SeekPosition(0);
	bool read_started=false;
	while(hashNodeAnalBuffer.GetNextObject(&anal_node))
	{
		if( anal_node.m_dwStatusNo!=(DWORD)iStatusIncEiffel||!anal_node.m_bFemNode||
			anal_node.m_iModuleNo!=iModuleNo||anal_node.legword.dwQuadLeg!=dwQuadLeg)
		{	//���ڽ����¼�����Լ������Ϊ��λ��������ģ���ʼ��ȡ��һ�������зǵ�ǰ���̵ļ������ͱ�ʾ��ȡ����
			if(read_started)
				break;
			continue;
		}
		read_started=true;
		anal_node.pNode=Ta.Node.FromHandle(anal_node.m_hNode);
		if(anal_node.pNode==NULL)
			logerr.Log("0X%X�ڵ�������ģ����δ�ҵ�,���������ɲ�����!",anal_node.m_hNode);
		else
			node_arr.append(anal_node);
	}
	CQuickSort<CNodeAnalyzeInfo>::QuickSort(node_arr.m_pData,node_arr.GetSize(),compare_node_anal_func);
	for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext()){
		if(pStatus->iNo==iStatusIncEiffel.Id())
			break;
	}
	for(i=0;i<node_arr.GetSize();i++)
	{
		f3dPoint offset;
		offset=node_arr[i].offset;
		f3dPoint WindLoad=node_arr[i].windload;
		WindLoad*=pStatus->Kesai()*pStatus->GamaQ();
		if(Ta.m_siSafetyGrade==0)
			WindLoad*=1.1;
		else if(Ta.m_siSafetyGrade==1)
			WindLoad*=1.0;
		else// if(Ta.m_siSafetyGrade==2)
			WindLoad*=0.9;
		if(!m_bLinearAnalysis)
			fprintf(fp,"%5d %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n",node_arr[i].pNode->point_i,offset.x,offset.y,offset.z,
				node_arr[i].external_force.x/1000,node_arr[i].external_force.y/1000,node_arr[i].external_force.z/1000,
				WindLoad.x/1000,WindLoad.y/1000,node_arr[i].unbalance_force.mod()/1000);
		else
			fprintf(fp,"%5d %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n",node_arr[i].pNode->point_i,offset.x,offset.y,offset.z,
				node_arr[i].external_force.x/1000,node_arr[i].external_force.y/1000,node_arr[i].external_force.z/1000,
				WindLoad.x/1000,WindLoad.y/1000);
	}
	//�˼���������ѡ�ı���
	ELEM_ANALYZE_INFO elem_anal;
	hashElemAnalBuffer.SeekPosition(0);
	read_started=false;
	while(hashElemAnalBuffer.GetNextObject(&elem_anal))
	{
		if( elem_anal.m_dwStatusNo!=(DWORD)iStatusIncEiffel||
			elem_anal.m_iModuleNo!=iModuleNo||elem_anal.legword.dwQuadLeg!=dwQuadLeg)
		{	//���ڽ����¼�����Լ������Ϊ��λ��������ģ���ʼ��ȡ��һ�������зǵ�ǰ���̵ļ������ͱ�ʾ��ȡ����
			if(read_started)
				break;
			continue;
		}
		read_started=true;
		elem_anal.pParentRod=Ta.FromRodHandle(elem_anal.m_hParentRod);
		if(elem_anal.pParentRod==NULL)
			logerr.Log("0X%X�˼�������ģ����δ�ҵ�,���������ɲ�����!",elem_anal.pParentRod);
		else
			elem_arr.append(elem_anal);
	}
	CQuickSort<ELEM_ANALYZE_INFO>::QuickSort(elem_arr.m_pData,elem_arr.GetSize(),compare_elem_anal_func);
	fprintf(fp,"\n");
	fprintf(fp,"   ELEMENT     SIZE         N(kN)  WEIGHT(kg)     M.X(N*m)    M.Y(N*m)   M.Z(N*m)\n");
	fprintf(fp,"---------------------------------------------------------------------------------\n");
	for(i=0;i<elem_arr.GetSize();i++)
	{
		long No[2];
		double elem_N=elem_arr[i].N;
		f3dPoint moment=elem_arr[i].moment;
		No[0]=min(elem_arr[i].iStartI,elem_arr[i].iEndI);
		No[1]=max(elem_arr[i].iStartI,elem_arr[i].iEndI);
		if(elem_arr[i].pParentRod->size_idClassType==0)
		{
			fprintf(fp,"%4d-%4d    U%3.0fX%2.0f  %10.3f    %8.3f\n",No[0],No[1],
				elem_arr[i].pParentRod->GetWidth(),elem_arr[i].pParentRod->GetThick(),
				elem_N/1000,elem_arr[i].weight/9.8);
		}
		else if(elem_arr[i].pParentRod->size_idClassType==CLS_CABLE)
		{
			fprintf(fp,"%4d-%4d    %s  %10.3f    %8.3f\n",No[0],No[1],elem_arr[i].pParentRod->GetSizeSpec().spec,
				elem_N/1000,elem_arr[i].weight/9.8);
		}
		else if(strlen(elem_arr[i].pParentRod->GetBriefPartTypeMark())==1)
		{
			fprintf(fp,"%4d-%4d    %s%3.0fX%2.0f%C %10.3f    %8.3f    %8.3f    %8.3f    %8.3f\n",No[0],No[1],
				elem_arr[i].pParentRod->GetBriefPartTypeMark(),
				elem_arr[i].pParentRod->GetWidth(),elem_arr[i].pParentRod->GetThick(),
				elem_arr[i].pParentRod->cMaterial,elem_N/1000,elem_arr[i].weight/9.8,
				moment.x*1e-6,moment.y*1e-6,moment.z*1e-6);
		}
		else //if(strlen(elem_arr[i].pParentRod->GetBriefPartTypeMark())==2)
		{
			fprintf(fp,"%4d-%4d   %s%3.0fX%2.0f%C %10.3f    %8.3f    %8.3f    %8.3f    %8.3f\n",No[0],No[1],
				elem_arr[i].pParentRod->GetBriefPartTypeMark(),
				elem_arr[i].pParentRod->GetWidth(),elem_arr[i].pParentRod->GetThick(),
				elem_arr[i].pParentRod->cMaterial,elem_N/1000,elem_arr[i].weight/9.8,
				moment.x*1e-6,moment.y*1e-6,moment.z*1e-6);
		}
	}
	fclose(fp);
	return TRUE;
}
bool CPostProcessReport::CreateNodeEquation(char* file_name,long iModuleNo,DWORD dwQuadLeg,SUBID iStatusIncEiffel,long pointI)
{
	char file_path[MAX_PATH];
	strcpy(file_path,file_name);
	strcat(file_path,".bal");
	FILE *fp=fopen(file_path,"wt");
	if(fp==NULL)
	{
		logerr.Log("�ļ�'%s'д���ʧ��!",file_path);
		return FALSE;
	}
	CLogErrorLife loglife;
	fprintf(fp,"\n");
	fprintf(fp,"  No.   Fex(kN)  Fey(kN)  Fez(kN)");//  Fwx(kN)  Fwy(kN)\n");
	fprintf(fp,"----------------------------------\n");//----------------------\n");

	int i=0;
	CWorkStatus *pStatus;
	ARRAY_LIST<CNodeAnalyzeInfo> node_arr(0,1000);
	ARRAY_LIST<ELEM_ANALYZE_INFO> elem_arr(0,1000);
	if(dwQuadLeg==0)
		dwQuadLeg=0x01010101;
	CNodeAnalyzeInfo anal_node;
	hashNodeAnalBuffer.SeekPosition(0);
	bool read_started=false;
	bool finded=false;
	GEPOINT xExternalForce,xWindLoad,SEF,SMF;
	for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext()){
		if(pStatus->iNo==iStatusIncEiffel.Id())
			break;
	}
	while(hashNodeAnalBuffer.GetNextObject(&anal_node))
	{
		if( anal_node.m_dwStatusNo!=(DWORD)iStatusIncEiffel||!anal_node.m_bFemNode||
			anal_node.m_iModuleNo!=iModuleNo||anal_node.legword.dwQuadLeg!=dwQuadLeg)
		{	//���ڽ����¼�����Լ������Ϊ��λ��������ģ���ʼ��ȡ��һ�������зǵ�ǰ���̵ļ������ͱ�ʾ��ȡ����
			if(read_started)
				break;
			continue;
		}
		read_started=true;
		anal_node.pNode=Ta.Node.FromHandle(anal_node.m_hNode);
		if(anal_node.pNode==NULL)
			logerr.Log("0X%X�ڵ�������ģ����δ�ҵ�,���������ɲ�����!",anal_node.m_hNode);
		else if(anal_node.pNode->pointI==pointI)
		{
			xExternalForce=anal_node.external_force;
			xWindLoad=anal_node.windload;
			xWindLoad*=pStatus->Kesai()*pStatus->GamaQ();
			if(Ta.m_siSafetyGrade==0)
				xWindLoad*=1.1;
			else if(Ta.m_siSafetyGrade==1)
				xWindLoad*=1.0;
			else// if(Ta.m_siSafetyGrade==2)
				xWindLoad*=0.9;
			//fprintf(fp,"%5d %8.3f %8.3f %8.3f %8.3f %8.3f\n",pointI,
			fprintf(fp,"%5d %8.3f %8.3f %8.3f\n",pointI,
				xExternalForce.x/1000,xExternalForce.y/1000,xExternalForce.z/1000);//,xWindLoad.x/1000,xWindLoad.y/1000);
			finded=true;
			break;
		}
	}
	if(!finded)
		logerr.Log("û���ҵ��ڵ��%d",pointI);
	//�˼���������ѡ�ı���
	ELEM_ANALYZE_INFO elem_anal;
	hashElemAnalBuffer.SeekPosition(0);
	read_started=false;
	fprintf(fp,"\n");
	fprintf(fp,"  ELEMENT      NT(kN)      NT.x(kN)    NT.y(kN)    NT.z(kN)     W.z(kN)    VF.x(N*m)   VF.y(N*m)   VF.z(N*m)\n");
	fprintf(fp,"------------------------------------------------------------------------------------------------\n");
	while(hashElemAnalBuffer.GetNextObject(&elem_anal))
	{
		if( elem_anal.m_dwStatusNo!=(DWORD)iStatusIncEiffel||
			elem_anal.m_iModuleNo!=iModuleNo||elem_anal.legword.dwQuadLeg!=dwQuadLeg)
		{	//���ڽ����¼�����Լ������Ϊ��λ��������ģ���ʼ��ȡ��һ�������зǵ�ǰ���̵ļ������ͱ�ʾ��ȡ����
			if(read_started)
				break;
			continue;
		}
		read_started=true;
		CLDSLinePart* pParentRod=Ta.FromRodHandle(elem_anal.m_hParentRod);
		if(pParentRod==NULL)
			logerr.Log("0X%X�˼�������ģ����δ�ҵ�,���������ɲ�����!",elem_anal.pParentRod);
		else if(elem_anal.iStartI==pointI||elem_anal.iEndI==pointI)
		{
			GEPOINT lenStdVec=pParentRod->pEnd->Position(false)-pParentRod->pStart->Position(false);
			double length=lenStdVec.mod();
			lenStdVec*=(1/length);
			if(elem_anal.iEndI==pointI)
				lenStdVec*=-1.0;
			long startI =min(elem_anal.iStartI,elem_anal.iEndI);
			long endI   =max(elem_anal.iStartI,elem_anal.iEndI);
			GEPOINT EF=elem_anal.N*lenStdVec;
			GEPOINT MF,moment=elem_anal.moment;
			//����ʼĩ����ؼ����������һ�˵ĵ�Ч�ֿ���
			/*if(elem_anal.iStartI==pointI)
			{
				moment=elem_anal.ME-elem_anal.MS;
				//��������򣨿۳�Ť��ֵ��
				double momentPrjL=moment*lenStdVec;
				moment.Set(moment.x-lenStdVec.x*momentPrjL,moment.y-lenStdVec.y*momentPrjL,moment.z-lenStdVec.z*momentPrjL);
				MF=(moment^lenStdVec)/length;
			}
			else
			{
				moment=elem_anal.MS-elem_anal.ME;
				//��������򣨿۳�Ť��ֵ��
				double momentPrjL=moment*lenStdVec;
				moment.Set(moment.x-lenStdVec.x*momentPrjL,moment.y-lenStdVec.y*momentPrjL,moment.z-lenStdVec.z*momentPrjL);
				MF=-(moment^lenStdVec)/length;
			}*/
			MF=elem_anal.iStartI==pointI?-elem_anal.VFS:-elem_anal.VFE;
			SEF+=EF;
			SMF+=MF;
			fprintf(fp,"%4d-%4d    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f\n",startI,endI,
				elem_anal.N/1000,EF.x/1000,EF.y/1000,EF.z/1000,elem_anal.weight/1000,MF.x*1e-3,MF.y*1e-3,MF.z*1e-3);
		}
	}
	GEPOINT UF=xExternalForce+SEF+SMF;
	fprintf(fp,"\n");
	fprintf(fp,"UF=(NT+VF)+Fe\n");
	fprintf(fp,"��ƽ����(N),UF.x=%.1f,UF.y=%.1f,UF.z=%.1f\n",UF.x,UF.y,UF.z);
	fprintf(fp,"UF��ʾ��ƽ����,NTΪ�˼�����,VFΪ����ز����Ķ���������,FeΪ��ǰ�ڵ�ʵ��ִ�е�����أ����������ء����������صȣ�\n");
	fclose(fp);
	return TRUE;
}
#endif
