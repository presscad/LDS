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
DWORD HashStrFunc(const char *key,DWORD nHashSize,BYTE cnMaxKeyBufSize/*=0*/)	//根据输入的关键字key计算相应的Hash映像值
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
	//key为空时默认以"CHashStrTable:NULL-KEY"作为键值,
	//否则清空链表，再次用空字符串做键值查询时返回无效值 wht 16-12-05
	const char *sRealKey=(key!=NULL&&*key!=0)?key:"CHashStrTable:NULL-KEY";
	union KEY_VALUE
	{
		BYTE key[4]; //0.A1 1.A2 2.A3 3.A4
		DWORD dwKey; //A4A3A2A1 从高到低
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
/** 经验证最好的哈希算法就是直接整数取余
DWORD HashId64Func(XHUINT64 key,DWORD nHashSize)	//根据输入的关键字key计算相应的Hash映像值
{
	if(key<nHashSize)	//由于缺少无符号64位整数，XHUINT64位为负时表示的数更大
		return (DWORD)key;
	else
	{	//防止因XHUINT64过大时（如负数），直接取模出现异常（甚至可能为负或大于m_nHashSize)
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
	*pObjSubId =_ModuleAndFlag.dwModuleAndFlag<<27;	//呼高号最多为24,故占高位的5 bits足够 0xf8000000
	*pObjSubId|=(m_dwStatusNo<<16)&0x07ff0000;	//工况号占中间的11 bits最多可表示2047种工况足够 0x07ff0000
	if(legword.arrQuad[0]==legword.arrQuad[1]&&legword.arrQuad[0]==legword.arrQuad[2]&&legword.arrQuad[0]==legword.arrQuad[3])
	{
		//*pObjSubId&=0xffff00ff;	//等高腿轮换(第3byte为0),最低位8bit表示一个接腿序号
		*pObjSubId|=legword.arrQuad[0];
	}
	else
	{	//高低腿轮换,最多可轮换至第15个接腿
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
	m_iModuleNo=(subid&0xf8000000)>>27;	//呼高号最多为24,故占高位的5 bits足够 0xf8000000
	m_dwStatusNo=(subid&0x07ff0000)>>16;//工况号占中间的11 bits最多可表示2047种工况足够 0x07ff0000
	if(subid&0x0000ff00)
	{	//高低腿轮换,最多可轮换至第15个接腿
		legword.arrQuad[0]=(subid&0xf000)>>12;
		legword.arrQuad[1]=(subid&0x0f00)>>8;
		legword.arrQuad[2]=(subid&0x00f0)>>4;
		legword.arrQuad[3]=(subid&0x0f);
	}
	else	//等高腿轮换(第3byte为0),最低位8bit表示一个接腿序号
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
bool RSLTOBJ_HEADER::ReadKeyId(BUFFER_IO* pBuff,DWORD version)	//将键值信息写入缓存
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
void RSLTOBJ_HEADER::WriteKeyId(BUFFER_IO* pBuff,DWORD version)	//将键值信息写入缓存
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
	{	//首节点
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
	m_dwVersion=3;	//1版本内存占用较大
	m_nObjCount=0;
	//hashObjBuffPosition.HashFunc=HashId64Func;
}
CHashElemAnalyzeInfo::CHashElemAnalyzeInfo()
{
	//m_dwVersion=2;//1版本内存占用较大
	m_dwVersion=3;	//相对2版本将弯矩分为始末端//内存占用较大
	m_nObjCount=0;
	//hashObjBuffPosition.HashFunc=HashId64Func;
}
void CHashNodeAnalyzeInfo::EmptyListAddrTempBuff(){
	m_nObjCount=0;
	listAddrOfObjs.DeleteList();
}
DWORD CHashNodeAnalyzeInfo::AppendNodeRsltObject(CNodeAnalyzeInfo& obj)//返回当前对象在缓存文件中的存储起始位置
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
	DWORD hashsize=nodecount+(nodecount>>4);	//人为多出6.25%,否则当节点数多时，有可能因为哈希的不够稀疏，导致反而额外多分配很多节点
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
{	//从IO当前位置开始读取结果记录
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
	DWORD hashsize=nodecount+(nodecount>>4);	//人为多出6.25%,否则当节点数多时，有可能因为哈希的不够稀疏，导致反而额外多分配很多节点
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
{	//从IO当前位置开始读取结果记录
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
	DWORD hashsize=nodecount+(nodecount>>4);	//人为多出6.25%,否则当节点数多时，有可能因为哈希的不够稀疏，导致反而额外多分配很多节点
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
DWORD CHashElemAnalyzeInfo::AppendElemRsltObject(ELEM_ANALYZE_INFO& obj)//返回当前对象在缓存文件中的存储起始位置
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
	DWORD hashsize=nodecount+(nodecount>>4);	//人为多出6.25%,否则当节点数多时，有可能因为哈希的不够稀疏，导致反而额外多分配很多节点
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
// CPostProcessReport 类 Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPostProcessReport postreport;
UINT CNodeAnalyzeInfo::KeyHeadInfoSize(DWORD version)
{
	if(version==0||version>=2)
		return 8;
	else
		return 16;
}	//Read/WriteObjectKey读写的缓存大小
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
void CNodeAnalyzeInfo::ToBuffer(BUFFER_IO* pBuff,DWORD version)		//不含键值信息
{
	if(version==0||version>1)
	{
		ToThinBuffer(pBuff);
		return;
	}
	pBuff->WriteBooleanThin(m_bFemNode);
	pBuff->WritePoint(offset);
	//pBuff->WritePoint(rot_offset);			//节点的偏移量
	//pBuff->WritePoint(internal_force);
	pBuff->WritePoint(external_force);
	pBuff->WritePoint(unbalance_force);
	pBuff->WritePoint(internal_moment);
	pBuff->WritePoint(windload);
}
void CNodeAnalyzeInfo::FromBuffer(BUFFER_IO* pBuff,DWORD version)	//不含键值信息
{
	if(version==0||version>1)
	{
		FromThinBuffer(pBuff);
		return;
	}
	pBuff->ReadBooleanThin(&m_bFemNode);
	pBuff->ReadPoint(offset);
	//pBuff->ReadPoint(rot_offset);			//节点的偏移量
	//pBuff->ReadPoint(internal_force);
	pBuff->ReadPoint(external_force);
	pBuff->ReadPoint(unbalance_force);
	pBuff->ReadPoint(internal_moment);
	pBuff->ReadPoint(windload);
}
void CNodeAnalyzeInfo::ToThinBuffer(BUFFER_IO* pBuff)		//不含键值信息
{	//共写入57 Bytes
	pBuff->WriteBooleanThin(m_bFemNode);
	pBuff->WriteThinPoint(offset);	//Write 12 Bytes
	pBuff->WriteThinPoint(external_force);
	pBuff->WriteThinPoint(unbalance_force);
	pBuff->WriteThinPoint(internal_moment);
	pBuff->WriteFloat(windload.x);
	pBuff->WriteFloat(windload.y);
}
void CNodeAnalyzeInfo::FromThinBuffer(BUFFER_IO* pBuff)	//不含键值信息
{	//共读入57 Bytes
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
}	//Read/WriteObjectKey读写的缓存大小
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
void ELEM_ANALYZE_INFO::ToBuffer(BUFFER_IO* pBuff,DWORD version)		//不含键值信息
{
	if(version==0||version>1)
	{
		pBuff->WriteInteger(iStartI);
		pBuff->WriteInteger(iEndI);
		pBuff->WriteInteger(m_hParentRod);
		pBuff->WriteFloat(weight);
		pBuff->WriteFloat(N);
		if(version==2)
			pBuff->WriteThinPoint(moment);	//单元弯矩
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
	pBuff->WritePoint(moment);	//单元弯矩
}
void ELEM_ANALYZE_INFO::FromBuffer(BUFFER_IO* pBuff,DWORD version)	//不含键值信息
{
	if(version==0||version>1)
	{
		pBuff->ReadInteger(&iStartI);
		pBuff->ReadInteger(&iEndI);
		pBuff->ReadInteger(&m_hParentRod);
		pBuff->ReadFloat(&weight);
		pBuff->ReadFloat(&N);
		if(version==2)
			pBuff->ReadThinPoint(moment);	//单元的弯矩
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
	pBuff->ReadPoint(moment);			//单元的弯矩
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
	m_bHasStdFoundationForce=false;		//是否包含基础作用力标准值信息
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
	Reaction.Empty();	//基础反作用力,	只是计算临时存储,计算结束内容清空
	TowerWeight.Empty();	//各基塔塔重(kg),只是计算临时存储,计算结束内容清空
	hashElems.Empty();	// 应力计算时将长杆打碎后的全塔杆件（一个计算操作内的集合）
	ElemPtrs.Empty();
}
 
//from AnalyzeProcessor.cpp
int compare_func1(const NODE_PTR& pNode1,const NODE_PTR& pNode2);
int compare_func2(const ELEM_PTR &pElem1,const ELEM_PTR &pElem2);
void CPostProcessReport::CreateProcessReport()
{
	DisplayPrompt("正在汇总计算结果!");
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
	//报告输出节点统计
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
		if(node_arr[i]->IsFatherObject())	//父节点
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
				pRptNode->symmetry.Copy("  无");
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
					pRptNode->symmetry.Copy("  无");
				if(g_sysPara.bMergeRptFileMirObj)
				{
					pRptNode->bFatherNode=false;
					continue;	//合并关联对称项时，子对称项不显示输出
				}
			}
			else
				pRptNode->symmetry.Copy("  无");
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
		//TODO:需要仔细看看_lamdaRc是否还有用
		pElem->_lamdaRc=pElem->LamdaRc();
		ElemPtrs.SetValue(pElem->pStart->handle,pElem->pEnd->handle,pElem);
		//由于是选材时的临时用内存指针绘随CPartSizeOptimizor销毁而销毁，因此必须赋空，否则会导致死机异常
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
	buffer.ReadInteger(&version);	//起始版本号
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
			logerr.Log("计算结果数据中关联的节点0X%X,已不存在!",h);
		buffer.ReadInteger(&h);pReact->pModule=Ta.Module.FromHandle(h);
		buffer.ReadInteger(&h);pReact->pStatus=Ta.WorkStatus.FromHandle(h);
		if(pReact->pModule==NULL)
			logerr.Log("计算结果数据中关联的某个呼高,已不存在!");
		if(pReact->pStatus==NULL)
			logerr.Log("计算结果数据中关联的某个工况,已不存在!");
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
		buffer.ReadInteger(&key);	//单元标识，在前处理器中生成，一直贯穿始终
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
		buffer.ReadInteger(&pElem->relaElem.idRefElem);	//交叉斜材考虑同时受压参照单元
		buffer.ReadInteger(&pElem->relaElem.idRefElem2);//考虑同端节点同时受压参照单元
		buffer.ReadInteger(&pElem->relaElem.idElemMirX);//此单元是否为对称关联单元中的父单元，即节点号综合较小的单元
		buffer.ReadInteger(&pElem->relaElem.idElemMirY);
		buffer.ReadInteger(&pElem->relaElem.idElemMirZ);
		//int BearTens1Comp2;	//1:承拉杆承压;2:承压杆承拉;其余为正常杆。1、2为零杆，其余情况为活动杆
		//void* pParaType;
		//buffer.WriteDouble(pElem->N);				//内力(N)
		buffer.ReadDouble(&pElem->T0);				//锚固端预张力(N)
		//double W0,H0,V0;		//拉线预张紧后后的总重力(N)及自由端在预张力作用下产生的张力水平分量和竖直分量
		//bool bAnchorEnd;		//true:拉线终端对地锚固;false:拉线始端对地锚固
		//buffer.WritePoint(pElem->moment);		//单元内力矩(N*mm)
		//short elem_type;	//0:表示二力杆或经典梁;1:表示柔索单元
		//最小轴计算长度信息
		buffer.ReadInteger(&h);pElem->lamdaRyo.pStart=Ta.Node.FromHandle(h);	//计算长度起始点
		buffer.ReadInteger(&h);pElem->lamdaRyo.pEnd=Ta.Node.FromHandle(h);		//计算长度终止点
		buffer.ReadDouble(&pElem->lamdaRyo.muCoefBasic);	//计算长度相当系数
		buffer.ReadDouble(&pElem->lamdaRyo.L);		//计算长度
		buffer.ReadDouble(&pElem->lamdaRyo.R);		//执行回转半径
		buffer.ReadDouble(&pElem->lamdaRyo.Lo_r);	//计算长细比
		buffer.ReadString(pElem->lamdaRyo.braceStr);//长细比计算过程字符串
		//普通材或交叉斜材一拉一压时平行轴长细比计算信息
		buffer.ReadInteger(&h);pElem->lamdaRx.pStart=Ta.Node.FromHandle(h);	//计算长度起始点
		buffer.ReadInteger(&h);pElem->lamdaRx.pEnd=Ta.Node.FromHandle(h);		//计算长度终止点
		buffer.ReadDouble(&pElem->lamdaRx.muCoefBasic);	//计算长度相当系数
		buffer.ReadDouble(&pElem->lamdaRx.L);		//计算长度
		buffer.ReadDouble(&pElem->lamdaRx.R);		//执行回转半径
		buffer.ReadDouble(&pElem->lamdaRx.Lo_r);	//计算长细比
		buffer.ReadString(pElem->lamdaRx.braceStr);//长细比计算过程字符串
		//交叉材同时受压时平行轴计算长度信息
		buffer.ReadInteger(&h);pElem->_lamdaRc.pStart=Ta.Node.FromHandle(h);	//计算长度起始点
		buffer.ReadInteger(&h);pElem->_lamdaRc.pEnd=Ta.Node.FromHandle(h);		//计算长度终止点
		buffer.ReadDouble(&pElem->_lamdaRc.muCoefBasic);	//计算长度相当系数静态基本值部分
		if(version>=2)	//计算长度相当系数动态值部分
			buffer.ReadDouble(&pElem->_lamdaRc.muCoefLive);
		else
			pElem->_lamdaRc.muCoefLive=1.0;
		buffer.ReadDouble(&pElem->_lamdaRc.L);		//计算长度
		buffer.ReadDouble(&pElem->_lamdaRc.R);		//执行回转半径
		buffer.ReadDouble(&pElem->_lamdaRc.Lo_r);	//计算长细比
		buffer.ReadString(pElem->_lamdaRc.braceStr);//长细比计算过程字符串
		//m_fStartBraceNCoef;	//辅助材始端对受力材的支撑力系数
		//m_fEndBraceNCoef;	//辅助材终端对受力材的支撑力系数
		buffer.ReadDouble(&pElem->m_fWeightRaiseCoef);	//自身重力增大系数
		buffer.ReadInteger(&pElem->m_iCtrlRule);
		//int iSizeIndex;		//用于按照规格排序比较规格大小
		buffer.ReadInteger(&pElem->m_bValidStartRestStiff);
		buffer.ReadInteger(&pElem->m_bValidEndRestStiff);	//是否有承受部分转动约束的能力
	//xx_joint_type代表可能性，而real_xx_joint_type则代表实际计算时的支座类型
	//0：完全铰接JOINT_HINGE；1：部分刚接JOINT_RIGID（限制除扭转外的转动约束）,2：自动判断JOINT_AUTOJUSTIFY
		JOINT_TYPE joint_type_arr[]={JOINT_HINGE,JOINT_RIGID,JOINT_AUTOJUSTIFY};
		FORCE_TYPE force_type_arr[]={CENTRIC_FORCE,ECCENTRIC_FORCE};
		int joint_type,force_type;
		buffer.ReadInteger(&joint_type);	pElem->start_joint_type=joint_type_arr[joint_type];
		buffer.ReadInteger(&joint_type);	pElem->end_joint_type=joint_type_arr[joint_type];	//JOINT_TYPE
		buffer.ReadInteger(&joint_type);	pElem->real_start_joint_type=joint_type_arr[joint_type];
		buffer.ReadInteger(&joint_type);	pElem->real_end_joint_type=joint_type_arr[joint_type];	//0：完全铰接JOINT_HINGE1；部分刚接JOINT_RIGID（限制除扭转外的转动约束）
		buffer.ReadInteger(&force_type);	pElem->start_force_type=force_type_arr[force_type];
		buffer.ReadInteger(&force_type);	pElem->end_force_type=force_type_arr[force_type];	//	//角钢两端受力状况CENTRIC_FORCE,ECCENTRIC_FORCE
	//写入设计信息design_info;
		buffer.ReadDouble(&pElem->design_info.hole_out);	//减孔数
		buffer.ReadDouble(&pElem->design_info.hole_d);		//孔径
		buffer.ReadDouble(&pElem->design_info.w_t);
		buffer.ReadDouble(&pElem->design_info.fCoefTensStrengthDecrease);
		buffer.ReadDouble(&pElem->design_info.fCoefCompStrengthDecrease);
		buffer.ReadDouble(&pElem->design_info.fCoefStablity);
		buffer.ReadDouble(&pElem->design_info.fCoefEffic);		//控制工况下设计指标占指标允许极限的比例系数，无控制工况启用则为零
		buffer.ReadDouble(&pElem->design_info.fCoefOfBearLoad);	//控制工况下承受荷载占承载极限的比例系数，无控制工况启用则为零
		buffer.ReadInteger(&pElem->design_info.iDangerousRule);	//危险控制规则
		buffer.ReadInteger(&pElem->design_info.iDangerousLoadCase);	//最危险的承载指标
		if(version>=2)
			buffer.ReadBooleanThin(&pElem->design_info.bBlockShearActivated);
		else
			pElem->design_info.bBlockShearActivated=false;
		buffer.ReadDouble(&pElem->design_info.mN);				//压杆稳定强度折减系数
		buffer.ReadString(pElem->design_info.LoExpression);
		buffer.ReadString(pElem->design_info.lamdaExpression);		//长细比计算表达式
		buffer.ReadInteger(&pElem->design_info.bStartRotRest);
		buffer.ReadInteger(&pElem->design_info.bEndRotRest);
		buffer.ReadByte(&pElem->design_info.lamdaSelType);	// 0:lamdaRyo控制;1:lamdaRx控制;2:lamdaRx2控制
		buffer.ReadByte(&pElem->design_info.cLamdaTypeR);	//0:Rx;1:Ry0
		buffer.ReadDouble(&pElem->design_info.lamdaL);
		buffer.ReadDouble(&pElem->design_info.lamdaR);
		buffer.ReadDouble(&pElem->design_info.Lo_r);		//计算长度,回转半径,实际长细比
		buffer.ReadDouble(&pElem->design_info.UltimateLo_r);//允许最大长细比
		//写入ELEMFORCE_ENV MaxTension,MaxCompression,MaxMoment;
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
		//写入ELEMCOMPSTABLE_ENV MaxCompStable;			//压稳控制时的最大压稳计算应力
		if(version>=4)
			buffer.ReadBooleanThin(&pElem->smcinfo.hasBothComp);
		buffer.ReadInteger(&pElem->MaxCompStable.hModule);	//模型句柄
		buffer.ReadInteger((int*)&pElem->MaxCompStable.hWorkStatus);//工况句柄
		buffer.Read(pElem->MaxCompStable.leg_no,sizeof(long)*4);		//四象限接腿号
		buffer.ReadDouble(&pElem->MaxCompStable.N);		// (N)
		buffer.ReadBooleanThin(&pElem->MaxCompStable.bothComp);	// true:交叉材同时受压控制;false:非交叉材或交叉材一拉一压工况控制
		buffer.ReadDouble(&pElem->MaxCompStable.sigema);	// N/(A*φ),(MPa)
		if(version<2)
		{
			double fai;
			buffer.ReadDouble(&fai);		// 最终最工况压稳系数pElem->MaxCompStable.fai
		}
	//所有不考虑同时受压的杆件单元，信息按一拉一压存储
		buffer.ReadDouble(&pElem->MaxCompStable.Afai_bothcomp);
		buffer.ReadDouble(&pElem->MaxCompStable.Afai_tenscomp);
		buffer.ReadDouble(&pElem->MaxCompStable.fai_bothcomp);
		buffer.ReadDouble(&pElem->MaxCompStable.fai_tenscomp);	//同时受压和一拉一压时的压稳系数
		buffer.ReadByte(&pElem->MaxCompStable.lamdatype_bothcomp);
		buffer.ReadByte(&pElem->MaxCompStable.lamdatype_tenscomp);	// 0:lamdaRyo控制;1:lamdaRx控制;2:lamdaRx2控制
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
		//对于辅助材单元表示始末端所支撑的受力材单元，用于计算辅材验算压力
		//CElement *pStartBraceForceElem,*pEndBraceForceElem;
	}
	//读取杆件两端螺栓承载系数
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
		if(hashNodeAnalBuffer.GetFileName(NULL)==0||hashElemAnalBuffer.GetFileName(NULL)==0)	//无文件名，需要先创建后台文件名
		{
			char filename[MAX_PATH];
			theApp.GetFileName(filename);
			InitProcTempFile(filename,dwRsltObjBuffVersion);
		}
		//读取节点计算结果信息
		DWORD nBufSize=0;
		buffer.ReadDword(&nBufSize);
		char pool[1024]={0};
		hashNodeAnalBuffer.ClearBuffer();
		hashNodeAnalBuffer.WriteBuffer(&buffer,nBufSize,pool,1024);
		//读取单元计算结果信息
		buffer.ReadDword(&nBufSize);
		hashElemAnalBuffer.ClearBuffer();
		hashElemAnalBuffer.WriteBuffer(&buffer,nBufSize,pool,1024);
	}
}
void CPostProcessReport::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(7);	//起始版本号
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
		buffer.WriteInteger(pElem->id);	//单元标识，在前处理器中生成，一直贯穿始终
		//CFGWORD cfgword;
		buffer.WriteInteger(pElem->pStart->handle);
		buffer.WriteInteger(pElem->pEnd->handle);
		buffer.WriteInteger(pElem->pOrgPole->handle);
		buffer.WriteBooleanThin(pElem->relaElem.bFatherElem);
		buffer.WriteInteger(pElem->relaElem.idRefElem);	//交叉斜材考虑同时受压参照单元
		buffer.WriteInteger(pElem->relaElem.idRefElem2);//考虑同端节点同时受压参照单元
		buffer.WriteInteger(pElem->relaElem.idElemMirX);//此单元是否为对称关联单元中的父单元，即节点号综合较小的单元
		buffer.WriteInteger(pElem->relaElem.idElemMirY);
		buffer.WriteInteger(pElem->relaElem.idElemMirZ);
		//int BearTens1Comp2;	//1:承拉杆承压;2:承压杆承拉;其余为正常杆。1、2为零杆，其余情况为活动杆
		//void* pParaType;
		//buffer.WriteDouble(pElem->N);				//内力(N)
		buffer.WriteDouble(pElem->T0);				//锚固端预张力(N)
		//double W0,H0,V0;		//拉线预张紧后后的总重力(N)及自由端在预张力作用下产生的张力水平分量和竖直分量
		//bool bAnchorEnd;		//true:拉线终端对地锚固;false:拉线始端对地锚固
		//buffer.WritePoint(pElem->moment);		//单元内力矩(N*mm)
		//short elem_type;	//0:表示二力杆或经典梁;1:表示柔索单元
		//最小轴计算长度信息
		if(pElem->lamdaRyo.pStart!=NULL)
			buffer.WriteInteger(pElem->lamdaRyo.pStart->handle);	//计算长度起始点
		else
			buffer.WriteInteger(0);
		if(pElem->lamdaRyo.pEnd!=NULL)
			buffer.WriteInteger(pElem->lamdaRyo.pEnd->handle);		//计算长度终止点
		else
			buffer.WriteInteger(0);
		buffer.WriteDouble(pElem->lamdaRyo.muCoefBasic);	//计算长度相当系数
		buffer.WriteDouble(pElem->lamdaRyo.L);		//计算长度
		buffer.WriteDouble(pElem->lamdaRyo.R);		//执行回转半径
		buffer.WriteDouble(pElem->lamdaRyo.Lo_r);	//计算长细比
		buffer.WriteString(pElem->lamdaRyo.braceStr);//长细比计算过程字符串
		//普通材或交叉斜材一拉一压时平行轴长细比计算信息
		if(pElem->lamdaRx.pStart!=NULL)
			buffer.WriteInteger(pElem->lamdaRx.pStart->handle);	//计算长度起始点
		else
			buffer.WriteInteger(0);
		if(pElem->lamdaRx.pEnd!=NULL)
			buffer.WriteInteger(pElem->lamdaRx.pEnd->handle);		//计算长度终止点
		else
			buffer.WriteInteger(0);
		buffer.WriteDouble(pElem->lamdaRx.muCoefBasic);	//计算长度相当系数
		buffer.WriteDouble(pElem->lamdaRx.L);		//计算长度
		buffer.WriteDouble(pElem->lamdaRx.R);		//执行回转半径
		buffer.WriteDouble(pElem->lamdaRx.Lo_r);	//计算长细比
		buffer.WriteString(pElem->lamdaRx.braceStr);//长细比计算过程字符串
		//交叉材同时受压时平行轴计算长度信息
		if(pElem->LamdaRc().pStart!=NULL)
			buffer.WriteInteger(pElem->LamdaRc().pStart->handle);	//计算长度起始点
		else
			buffer.WriteInteger(0);
		if(pElem->LamdaRc().pEnd!=NULL)
			buffer.WriteInteger(pElem->LamdaRc().pEnd->handle);		//计算长度终止点
		else
			buffer.WriteInteger(0);
		buffer.WriteDouble(pElem->LamdaRc().muCoefBasic);	//计算长度相当系数静态值部分
		buffer.WriteDouble(pElem->LamdaRc().muCoefLive);	//计算长度相当系数动态值部分
		buffer.WriteDouble(pElem->LamdaRc().L);		//计算长度
		buffer.WriteDouble(pElem->LamdaRc().R);		//执行回转半径
		buffer.WriteDouble(pElem->LamdaRc().Lo_r);	//计算长细比
		buffer.WriteString(pElem->LamdaRc().braceStr);//长细比计算过程字符串
		//m_fStartBraceNCoef;	//辅助材始端对受力材的支撑力系数
		//m_fEndBraceNCoef;	//辅助材终端对受力材的支撑力系数
		buffer.WriteDouble(pElem->m_fWeightRaiseCoef);	//自身重力增大系数
		buffer.WriteInteger(pElem->m_iCtrlRule);
		//int iSizeIndex;		//用于按照规格排序比较规格大小
		buffer.WriteInteger(pElem->m_bValidStartRestStiff);
		buffer.WriteInteger(pElem->m_bValidEndRestStiff);	//是否有承受部分转动约束的能力
	//xx_joint_type代表可能性，而real_xx_joint_type则代表实际计算时的支座类型
	//0：完全铰接JOINT_HINGE；1：部分刚接JOINT_RIGID（限制除扭转外的转动约束）,2：自动判断JOINT_AUTOJUSTIFY
		buffer.WriteInteger(pElem->start_joint_type);
		buffer.WriteInteger(pElem->end_joint_type);	//JOINT_TYPE
		buffer.WriteInteger(pElem->real_start_joint_type);
		buffer.WriteInteger(pElem->real_end_joint_type);	//0：完全铰接JOINT_HINGE1；部分刚接JOINT_RIGID（限制除扭转外的转动约束）
		buffer.WriteInteger(pElem->start_force_type);
		buffer.WriteInteger(pElem->end_force_type);	//	//角钢两端受力状况CENTRIC_FORCE,ECCENTRIC_FORCE
	//写入设计信息design_info;
		buffer.WriteDouble(pElem->design_info.hole_out);	//减孔数
		buffer.WriteDouble(pElem->design_info.hole_d);		//孔径
		buffer.WriteDouble(pElem->design_info.w_t);
		buffer.WriteDouble(pElem->design_info.fCoefTensStrengthDecrease);
		buffer.WriteDouble(pElem->design_info.fCoefCompStrengthDecrease);
		buffer.WriteDouble(pElem->design_info.fCoefStablity);
		buffer.WriteDouble(pElem->design_info.fCoefEffic);		//控制工况下设计指标占指标允许极限的比例系数，无控制工况启用则为零
		buffer.WriteDouble(pElem->design_info.fCoefOfBearLoad);	//控制工况下承受荷载占承载极限的比例系数，无控制工况启用则为零
		buffer.WriteInteger(pElem->design_info.iDangerousRule);	//危险控制规则
		buffer.WriteInteger(pElem->design_info.iDangerousLoadCase);	//最危险的承载指标
		buffer.WriteBooleanThin(pElem->design_info.bBlockShearActivated);
		buffer.WriteDouble(pElem->design_info.mN);				//压杆稳定强度折减系数
		buffer.WriteString(pElem->design_info.LoExpression);
		buffer.WriteString(pElem->design_info.lamdaExpression);		//长细比计算表达式
		buffer.WriteInteger(pElem->design_info.bStartRotRest);
		buffer.WriteInteger(pElem->design_info.bEndRotRest);
		buffer.WriteByte(pElem->design_info.lamdaSelType);	// 0:lamdaRyo控制;1:lamdaRx控制;2:lamdaRx2控制
		buffer.WriteByte(pElem->design_info.cLamdaTypeR);	//0:Rx;1:Ry0
		buffer.WriteDouble(pElem->design_info.lamdaL);
		buffer.WriteDouble(pElem->design_info.lamdaR);
		buffer.WriteDouble(pElem->design_info.Lo_r);		//计算长度,回转半径,实际长细比
		buffer.WriteDouble(pElem->design_info.UltimateLo_r);//允许最大长细比
		//写入ELEMFORCE_ENV MaxTension,MaxCompression,MaxMoment;
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
		//写入ELEMCOMPSTABLE_ENV MaxCompStable;			//压稳控制时的最大压稳计算应力
		buffer.WriteBooleanThin(pElem->smcinfo.hasBothComp);
		buffer.WriteInteger(pElem->MaxCompStable.hModule);	//模型句柄
		buffer.WriteInteger(pElem->MaxCompStable.hWorkStatus);//工况句柄
		buffer.Write(pElem->MaxCompStable.leg_no,sizeof(long)*4);		//四象限接腿号
		buffer.WriteDouble(pElem->MaxCompStable.N);		// (N)
		buffer.WriteBooleanThin(pElem->MaxCompStable.bothComp);	// true:交叉材同时受压控制;false:非交叉材或交叉材一拉一压工况控制
		buffer.WriteDouble(pElem->MaxCompStable.sigema);	// N/(A*φ),(MPa)
		//buffer.WriteDouble(pElem->MaxCompStable.fai);		// 最终最工况压稳系数
	//所有不考虑同时受压的杆件单元，信息按一拉一压存储
		buffer.WriteDouble(pElem->MaxCompStable.Afai_bothcomp);
		buffer.WriteDouble(pElem->MaxCompStable.Afai_tenscomp);
		buffer.WriteDouble(pElem->MaxCompStable.fai_bothcomp);
		buffer.WriteDouble(pElem->MaxCompStable.fai_tenscomp);	//同时受压和一拉一压时的压稳系数
		buffer.WriteByte(pElem->MaxCompStable.lamdatype_bothcomp);
		buffer.WriteByte(pElem->MaxCompStable.lamdatype_tenscomp);	// 0:lamdaRyo控制;1:lamdaRx控制;2:lamdaRx2控制
		buffer.WriteDouble(pElem->MaxCompStable.L_bothcomp);
		buffer.WriteDouble(pElem->MaxCompStable.Lo_r_bothcomp);
		buffer.WriteDouble(pElem->MaxCompStable.muCoefL_bothcomp);
		//对于辅助材单元表示始末端所支撑的受力材单元，用于计算辅材验算压力
		//CElement *pStartBraceForceElem,*pEndBraceForceElem;
	}
	//存储杆件两端螺栓承载系数
	buffer.WriteInteger(hashRodPtrs.GetNodeNum());
	for(CLDSLinePartPtr *ppRod=hashRodPtrs.GetFirst();ppRod;ppRod=hashRodPtrs.GetNext())
	{
		buffer.WriteInteger((*ppRod)->handle);
		buffer.WriteDouble((*ppRod)->connectStart.bearloadcoef);
		buffer.WriteDouble((*ppRod)->connectEnd.bearloadcoef);
	}
	DWORD dwCurrRsltBuffVersion=hashNodeAnalBuffer.BuffVersion();
	if(dwCurrRsltBuffVersion==0)	//0表示当前最新版本
		dwCurrRsltBuffVersion=CURR_RSLTOBJBUFF_VERSION;
	buffer.WriteDword(dwCurrRsltBuffVersion);	//if(version>=7)
	//存储节点计算结果信息，本节占用缓存空间较大，所以需要提前设大增量否则写缓存效率低
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
	//比对节点信息输出
	CTaskOutputList infoList;
	COutputInfoAgent agent(&infoList);
	agent.ListAppendFunc=AppendOutputInfoToList;
	agent.m_uType=INFO_COMPARE_REPORT_WARNNING;
	if(legacyreport.hashNodes.GetNodeNum()!=this->hashNodes.GetNodeNum())
	{
		rpt.Log("节点数原有数量为%d,当前数量为%d\n",legacyreport.hashNodes.GetNodeNum(),this->hashNodes.GetNodeNum());
		agent.Append(0,0,rpt.LatestLog());
		return false;
	}
	POSTRPT_NODE *pLegacyNode=legacyreport.hashNodes.GetFirst();
	agent.m_uType=INFO_COMPARE_REPORT_NODES;
	for(POSTRPT_NODE *pRptNode=this->hashNodes.GetFirst();pRptNode&&pLegacyNode;pRptNode=this->hashNodes.GetNext())
	{
		if(pRptNode->pNode!=pLegacyNode->pNode)
		{
			rpt.Log("原有节点{%d}不同于当前节点{%d}\n",pLegacyNode->pNode->point_i,pRptNode->pNode->point_i);
			agent.Append(pRptNode->pNode->handle,pLegacyNode->pNode->handle,rpt.LatestLog());
		}
		if(pRptNode->bFatherNode!=pLegacyNode->bFatherNode)
		{
			rpt.Log("原有节点{%d}不同于当前节点{%d},父节点类型不同\n",pLegacyNode->pNode->point_i,pRptNode->pNode->point_i);
			agent.Append(pRptNode->pNode->handle,pLegacyNode->pNode->handle,rpt.LatestLog());
		}
		if(!pRptNode->symmetry.Equal(pLegacyNode->symmetry))
		{
			rpt.Log("原有节点{%d}对称信息为%s,不同于当前节点{%d}的对称信息%s,父节点类型不同\n",pLegacyNode->pNode->point_i,
				(char*)pLegacyNode->symmetry,pRptNode->pNode->point_i,(char*)pRptNode->symmetry);
			agent.Append(pRptNode->pNode->handle,pLegacyNode->pNode->handle,rpt.LatestLog());
		}
		pLegacyNode=legacyreport.hashNodes.GetNext();
	}
	agent.m_uType=INFO_COMPARE_REPORT_WARNNING;
	if(legacyreport.Reaction.GetNodeNum()!=this->Reaction.GetNodeNum())
	{
		rpt.Log("基础反力原有数量为%d,当前数量为%d\n",legacyreport.Reaction.GetNodeNum(),this->Reaction.GetNodeNum());
		agent.Append(0,0,rpt.LatestLog());
		return false;
	}
	agent.m_uType=INFO_COMPARE_REPORT_NODES;
	BASENODE_REACTION *pLegacyReact=legacyreport.Reaction.GetFirst();
	for(BASENODE_REACTION *pReact=this->Reaction.GetFirst();pReact;pReact=this->Reaction.GetNext())
	{
		if(pReact->pNode!=pLegacyReact->pNode)
			rpt.Log("原有基础节点{%d}不同于当前基础节点{%d}\n",pLegacyReact->pNode->point_i,pReact->pNode->point_i);
		if(pReact->pModule!=pLegacyReact->pModule||pReact->pStatus!=pLegacyReact->pStatus)
			rpt.Log("原有基础节点{%d}的工况{%d-%d}不同于当前基础节点{%d}的工况{%d-%d}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->pModule->iNo,pLegacyReact->pStatus->iNo,
			pReact->pNode->point_i,pReact->pModule->iNo,pReact->pStatus->iNo);
		if( pReact->leg_no[0]!=pLegacyReact->leg_no[0]||pReact->leg_no[1]!=pLegacyReact->leg_no[1]||
			pReact->leg_no[2]!=pLegacyReact->leg_no[2]||pReact->leg_no[3]!=pLegacyReact->leg_no[3])
			rpt.Log("原有基础节点{%d}的接腿{%d-%d-%d-%d}不同于当前基础节点{%d}的工况{%d-%d-%d-%d}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->leg_no[0],pLegacyReact->leg_no[1],pLegacyReact->leg_no[2],pLegacyReact->leg_no[3],
			pReact->pNode->point_i,pReact->leg_no[0],pReact->leg_no[1],pReact->leg_no[2],pReact->leg_no[3]);
		GEPOINT diff=pLegacyReact->reaction_force_design-pReact->reaction_force_design;
		if(!pReact->reaction_force_design.IsEqual(pLegacyReact->reaction_force_design.x,
			pLegacyReact->reaction_force_design.y,pLegacyReact->reaction_force_design.z,0.5))
			rpt.Log("原有基础设计反力{%d　%d-%d}较当前基础设计反力{%d　%d-%d}，差额{%8.3f %8.3f %8.3f kN}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->pModule->iNo,pLegacyReact->pStatus->iNo,
			pReact->pNode->point_i,pReact->pModule->iNo,pReact->pStatus->iNo,
			diff.x*0.001,diff.y*0.001,diff.z*0.001);
		/**现在基础作用力标准值已被随设计值进行计算了　wjh-2016.4.22
		diff=pLegacyReact->reaction_force_std-pReact->reaction_force_std;
		if(!pReact->reaction_force_std.IsEqual(pLegacyReact->reaction_force_std,0.5))
			rpt.Log("原有基础标准反力{%d　%d-%d}较当前基础标准反力{%d　%d-%d}，差额{%8.3f %8.3f %8.3f kN}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->pModule->iNo,pLegacyReact->pStatus->iNo,
			pReact->pNode->point_i,pReact->pModule->iNo,pReact->pStatus->iNo,
			diff.x*0.001,diff.y*0.001,diff.z*0.001);
		*/
		pLegacyReact=legacyreport.Reaction.GetNext();
	}
	if(legacyreport.TowerWeight.GetNodeNum()!=this->TowerWeight.GetNodeNum())
	{
		rpt.Log("本体接腿重量统计单元原数量为%d,当前数量为%d\n",legacyreport.TowerWeight.GetNodeNum(),this->TowerWeight.GetNodeNum());
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
			rpt.Log("重量统计记录原记录{%C%d=%.1f}与当前记录{%C%d=%.1f}不匹配\n",
				cLegacy,pLegacyWeight->cfg_no,pLegacyWeight->weight,cCurrent,pTowerWeight->cfg_no,pTowerWeight->weight);
		}
		pLegacyWeight=legacyreport.TowerWeight.GetNext();
	}
	if(legacyreport.hashElems.GetNodeNum()!=this->hashElems.GetNodeNum())
		rpt.Log("单元原数量为%d,当前数量为%d\n",legacyreport.hashElems.GetNodeNum(),this->hashElems.GetNodeNum());
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
			rpt.Log("原有单元{%5d:%5d-%5d}与当前单元{%5d:%5d-%5d}标识不同\n",
			pLegacyElem->id,arrStartI[0],arrEndI[0],pElem->id,arrStartI[1],arrEndI[1]);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->relaElem.bFatherElem!=pLegacyElem->relaElem.bFatherElem)
		{
			rpt.Log("当前单元%5d-%5d的父子类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#ifdef __ALFA_TEST_VERSION_
		if(pElem->relaElem.idRefElem!=pLegacyElem->relaElem.idRefElem)
		{
			rpt.Log("当前单元%5d-%5d的受压参照单元与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->relaElem.idRefElem2!=pLegacyElem->relaElem.idRefElem2)
		{
			rpt.Log("当前单元%5d-%5d的同端节点受压参照单元与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#endif
		if(pElem->relaElem.idElemMirX!=pLegacyElem->relaElem.idElemMirX)
		{		
			rpt.Log("当前单元%5d-%5d的X轴对称单元与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->relaElem.idElemMirY!=pLegacyElem->relaElem.idElemMirY)
		{
			rpt.Log("当前单元%5d-%5d的Y轴对称单元与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->relaElem.idElemMirZ!=pLegacyElem->relaElem.idElemMirZ)
		{
			rpt.Log("当前单元%5d-%5d的Z轴对称单元与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//int BearTens1Comp2;	//1:承拉杆承压;2:承压杆承拉;其余为正常杆。1、2为零杆，其余情况为活动杆
		//void* pParaType;
		if(pElem->elem_type==1&&fabs(pElem->T0-pLegacyElem->T0)>0.5)
		{	//柔索单元
			rpt.Log("当前单元%5d-%5d的锚固端预张力%8.3fkN与原有单元%8.3fkN不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->T0*0.001,pLegacyElem->T0*0.001);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//double W0,H0,V0;		//拉线预张紧后后的总重力(N)及自由端在预张力作用下产生的张力水平分量和竖直分量
		//bool bAnchorEnd;		//true:拉线终端对地锚固;false:拉线始端对地锚固
		//short elem_type;	//0:表示二力杆或经典梁;1:表示柔索单元
		//最小轴计算长度信息
		if(pElem->MaxCompStable.bothComp!=pLegacyElem->MaxCompStable.bothComp)
		{
			rpt.Log("当前单元%5d-%5d的同时受压类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.bothComp,pLegacyElem->MaxCompStable.bothComp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#ifdef __ALFA_TEST_VERSION_
		if(pElem->lamdaRyo.pStart!=pLegacyElem->lamdaRyo.pStart||pElem->lamdaRyo.pEnd!=pLegacyElem->lamdaRyo.pEnd)
		{
			int starti=0,endi=0,legacysi=0,legacyei=0;
			if(pElem->lamdaRyo.pStart!=NULL)
				starti=pElem->lamdaRyo.pStart->point_i;	//计算长度起始点
			if(pElem->lamdaRyo.pEnd!=NULL)
				endi=pElem->lamdaRyo.pEnd->point_i;		//计算长度终止点
			if(pLegacyElem->lamdaRyo.pStart!=NULL)
				legacysi=pLegacyElem->lamdaRyo.pStart->point_i;	//计算长度起始点
			if(pLegacyElem->lamdaRyo.pEnd!=NULL)
				legacyei=pLegacyElem->lamdaRyo.pEnd->point_i;		//计算长度终止点
			rpt.Log("当前单元%5d-%5d的最小轴计算区间{%d-%d}与原有单元{%d-%d}不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
				starti,endi,legacysi,legacyei);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->lamdaRyo.muCoefBasic!=pLegacyElem->lamdaRyo.muCoefBasic)
		{
			rpt.Log("当前单元%5d-%5d的最小轴计算长度相当系数%8.3f与原有单元%8.3f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRyo.muCoefBasic,pLegacyElem->lamdaRyo.muCoefBasic);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->lamdaRyo.L-pLegacyElem->lamdaRyo.L)>0.1)
		{
			rpt.Log("当前单元%5d-%5d的最小轴计算长度%8.1f与原有单元%8.1f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRyo.L,pLegacyElem->lamdaRyo.L);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->lamdaRyo.R-pLegacyElem->lamdaRyo.R)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的最小轴执行回转半径%8.3f与原有单元%8.3f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRyo.R,pLegacyElem->lamdaRyo.R);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->lamdaRyo.Lo_r-pLegacyElem->lamdaRyo.Lo_r)>0.1)
		{
			rpt.Log("当前单元%5d-%5d的最小轴计算长细比%8.1f与原有单元%8.1f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRyo.Lo_r,pLegacyElem->lamdaRyo.Lo_r);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(!pElem->lamdaRyo.braceStr.EqualNoCase(pLegacyElem->lamdaRyo.braceStr))
		{
			rpt.Log("当前单元%5d-%5d的最小轴长细比计算字符串%s与原有单元%s不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			(char*)pElem->lamdaRyo.braceStr,(char*)pLegacyElem->lamdaRyo.braceStr);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//普通材或交叉斜材一拉一压时平行轴长细比计算信息
		if(pElem->lamdaRx.pStart!=pLegacyElem->lamdaRx.pStart||pElem->lamdaRx.pEnd!=pLegacyElem->lamdaRx.pEnd)
		{
			int starti=0,endi=0,legacysi=0,legacyei=0;
			if(pElem->lamdaRx.pStart!=NULL)
				starti=pElem->lamdaRx.pStart->point_i;	//计算长度起始点
			if(pElem->lamdaRx.pEnd!=NULL)
				endi=pElem->lamdaRx.pEnd->point_i;		//计算长度终止点
			if(pLegacyElem->lamdaRx.pStart!=NULL)
				legacysi=pLegacyElem->lamdaRx.pStart->point_i;	//计算长度起始点
			if(pLegacyElem->lamdaRx.pEnd!=NULL)
				legacyei=pLegacyElem->lamdaRx.pEnd->point_i;		//计算长度终止点
			rpt.Log("当前单元%5d-%5d的平行轴计算区间{%d-%d}与原有单元{%d-%d}不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
				starti,endi,legacysi,legacyei);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->lamdaRx.muCoefBasic!=pLegacyElem->lamdaRx.muCoefBasic)
		{
			rpt.Log("当前单元%5d-%5d的平行轴计算长度相当系数%8.3f与原有单元%8.3f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRx.muCoefBasic,pLegacyElem->lamdaRx.muCoefBasic);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->lamdaRx.L-pLegacyElem->lamdaRx.L)>0.1)
		{
			rpt.Log("当前单元%5d-%5d的平行轴计算长度%8.1f与原有单元%8.1f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRx.L,pLegacyElem->lamdaRx.L);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->lamdaRx.R-pLegacyElem->lamdaRx.R)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的平行轴执行回转半径%8.3f与原有单元%8.3f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRx.R,pLegacyElem->lamdaRx.R);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->lamdaRx.Lo_r-pLegacyElem->lamdaRx.Lo_r)>0.1)
		{
			rpt.Log("当前单元%5d-%5d的平行轴计算长细比%8.1f与原有单元%8.1f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->lamdaRx.Lo_r,pLegacyElem->lamdaRx.Lo_r);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(!pElem->lamdaRx.braceStr.EqualNoCase(pLegacyElem->lamdaRx.braceStr))
		{
			rpt.Log("当前单元%5d-%5d的平行轴长细比计算字符串%s与原有单元%s不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			(char*)pElem->lamdaRx.braceStr,(char*)pLegacyElem->lamdaRx.braceStr);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//交叉材同时受压时平行轴计算长度信息
		if(pElem->_lamdaRc.pStart!=pLegacyElem->_lamdaRc.pStart||pElem->_lamdaRc.pEnd!=pLegacyElem->_lamdaRc.pEnd)
		{
			int starti=0,endi=0,legacysi=0,legacyei=0;
			if(pElem->_lamdaRc.pStart!=NULL)
				starti=pElem->_lamdaRc.pStart->point_i;	//计算长度起始点
			if(pElem->_lamdaRc.pEnd!=NULL)
				endi=pElem->_lamdaRc.pEnd->point_i;		//计算长度终止点
			if(pLegacyElem->_lamdaRc.pStart!=NULL)
				legacysi=pLegacyElem->_lamdaRc.pStart->point_i;	//计算长度起始点
			if(pLegacyElem->_lamdaRc.pEnd!=NULL)
				legacyei=pLegacyElem->_lamdaRc.pEnd->point_i;		//计算长度终止点
			rpt.Log("当前单元%5d-%5d的同时受压平行轴计算区间{%d-%d}与原有单元{%d-%d}不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
				starti,endi,legacysi,legacyei);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//if(pElem->_lamdaRc.MuCoef()!=pLegacyElem->_lamdaRc.MuCoef())
		if(pElem->smcinfo.hasBothComp&&fabs(pElem->MaxCompStable.muCoefL_bothcomp-pLegacyElem->MaxCompStable.muCoefL_bothcomp)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的同时受压计算长度相当系数%8.3f与原有单元%8.3f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.muCoefL_bothcomp,pLegacyElem->MaxCompStable.muCoefL_bothcomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//if(fabs(pElem->_lamdaRc.L-pLegacyElem->_lamdaRc.L)>0.1)
		if(pElem->smcinfo.hasBothComp&&fabs(pElem->MaxCompStable.L_bothcomp-pLegacyElem->MaxCompStable.L_bothcomp)>0.1)
		{
			rpt.Log("当前单元%5d-%5d的同时受压平行轴计算长度%8.1f与原有单元%8.1f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.L_bothcomp,pLegacyElem->MaxCompStable.L_bothcomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->_lamdaRc.R-pLegacyElem->_lamdaRc.R)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的同时受压平行轴执行回转半径%8.3f与原有单元%8.3f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->_lamdaRc.R,pLegacyElem->_lamdaRc.R);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//if(fabs(pElem->_lamdaRc.Lo_r-pLegacyElem->_lamdaRc.Lo_r)>0.1)
		if(pElem->smcinfo.hasBothComp&&fabs(pElem->MaxCompStable.Lo_r_bothcomp-pLegacyElem->MaxCompStable.Lo_r_bothcomp)>0.1)
		{
			rpt.Log("当前单元%5d-%5d的同时受压平行轴长细比%8.1f与原有单元%8.1f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.Lo_r_bothcomp,pLegacyElem->MaxCompStable.Lo_r_bothcomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(!pElem->_lamdaRc.braceStr.EqualNoCase(pLegacyElem->_lamdaRc.braceStr))
		{
			rpt.Log("当前单元%5d-%5d的同时受压平行轴长细比计算字符串%s与原有单元%s不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			(char*)pElem->_lamdaRc.braceStr,(char*)pLegacyElem->_lamdaRc.braceStr);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->MaxCompStable.lamdatype_bothcomp!=pLegacyElem->MaxCompStable.lamdatype_bothcomp)
		{
			rpt.Log("当前单元%5d-%5d的同时受压长细比类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.lamdatype_bothcomp,pLegacyElem->MaxCompStable.lamdatype_bothcomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->smcinfo.hasBothComp&&fabs(pElem->MaxCompStable.fai_bothcomp-pLegacyElem->MaxCompStable.fai_bothcomp)>0.001)
		//if(fabs(pElem->LamdaRc().coefFai-pLegacyElem->MaxCompStable.fai_bothcomp)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的同时受压工况压稳系数%8.3f与原有单元不同%8.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.fai_bothcomp,pLegacyElem->MaxCompStable.fai_bothcomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->MaxCompStable.fai_tenscomp-pLegacyElem->MaxCompStable.fai_tenscomp)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的一拉一压工况压稳系数%8.3f与原有单元不同%8.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.fai_tenscomp,pLegacyElem->MaxCompStable.fai_tenscomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->MaxCompStable.lamdatype_tenscomp!=pLegacyElem->MaxCompStable.lamdatype_tenscomp)
		{
			rpt.Log("当前单元%5d-%5d的一拉一压长细比类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.lamdatype_tenscomp,pLegacyElem->MaxCompStable.lamdatype_tenscomp);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->m_fWeightRaiseCoef!=pLegacyElem->m_fWeightRaiseCoef)
		{
			rpt.Log("当前单元%5d-%5d的重力增大系数与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#endif
		if(pElem->m_iCtrlRule!=pLegacyElem->m_iCtrlRule)
		{
			rpt.Log("当前单元%5d-%5d的受控设计条件与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//m_fStartBraceNCoef;	//辅助材始端对受力材的支撑力系数
		//m_fEndBraceNCoef;	//辅助材终端对受力材的支撑力系数
		//int iSizeIndex;		//用于按照规格排序比较规格大小
		if(pElem->m_bValidStartRestStiff!=pLegacyElem->m_bValidStartRestStiff)
		{
			rpt.Log("当前单元%5d-%5d的始端转动约束类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->m_bValidEndRestStiff!=pLegacyElem->m_bValidEndRestStiff)
		{
			rpt.Log("当前单元%5d-%5d的终端转动约束类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
	//xx_joint_type代表可能性，而real_xx_joint_type则代表实际计算时的支座类型
	//0：完全铰接JOINT_HINGE；1：部分刚接JOINT_RIGID（限制除扭转外的转动约束）,2：自动判断JOINT_AUTOJUSTIFY
		if(pElem->start_joint_type!=pLegacyElem->start_joint_type)
		{
			rpt.Log("当前单元%5d-%5d的缺省始端连接类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->end_joint_type!=pLegacyElem->end_joint_type)
		{
			rpt.Log("当前单元%5d-%5d的缺省终端连接类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->real_start_joint_type!=pLegacyElem->real_start_joint_type)
		{
			rpt.Log("当前单元%5d-%5d的始端连接类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->real_end_joint_type!=pLegacyElem->real_end_joint_type)
		{
			rpt.Log("当前单元%5d-%5d的终端连接类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->start_force_type!=pLegacyElem->start_force_type)
		{
			rpt.Log("当前单元%5d-%5d的始端杆端受力状态与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->end_force_type!=pLegacyElem->end_force_type)
		{
			rpt.Log("当前单元%5d-%5d的终端杆端受力状态与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//写入设计信息design_info;
		if(fabs(pElem->design_info.hole_out-pLegacyElem->design_info.hole_out)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的减孔系数%5.3f与原有单元不同%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.hole_out,pLegacyElem->design_info.hole_out);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.fCoefTensStrengthDecrease-pLegacyElem->design_info.fCoefTensStrengthDecrease)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的受拉强度折减系数%5.3f与原有单元%5.3f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefTensStrengthDecrease,pLegacyElem->design_info.fCoefTensStrengthDecrease);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.fCoefCompStrengthDecrease-pLegacyElem->design_info.fCoefCompStrengthDecrease)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的受压强度折减系数%5.3f与原有单元不同%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
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
			rpt.Log("当前单元%5d-%5d的块剪受控状态%s与原有单元不同%s\n",pElem->pStart->point_i,pElem->pEnd->point_i,(char*)state,(char*)legacy);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.fCoefStablity-pLegacyElem->design_info.fCoefStablity)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的压稳系数%5.3f与原有单元不同%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefStablity,pLegacyElem->design_info.fCoefStablity);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.mN-pLegacyElem->design_info.mN)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的压杆稳定强度折减系数%5.3f与原有单元不同%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.mN,pLegacyElem->design_info.mN);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(!pElem->design_info.LoExpression.EqualNoCase(pLegacyElem->design_info.LoExpression))
		{
			rpt.Log("当前单元%5d-%5d的计算长度表达式%s与原有单元不同%s\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			(char*)pElem->design_info.LoExpression,(char*)pLegacyElem->design_info.LoExpression);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(!pElem->design_info.lamdaExpression.EqualNoCase(pLegacyElem->design_info.lamdaExpression))
		{
			rpt.Log("当前单元%5d-%5d的长细比计算表达式%s与原有单元不同%s\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			(char*)pElem->design_info.lamdaExpression,(char*)pLegacyElem->design_info.lamdaExpression);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.bStartRotRest!=pLegacyElem->design_info.bStartRotRest)
		{
			rpt.Log("当前单元%5d-%5d的始端长细比受控约束%d与原有单元不同%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.bStartRotRest,pLegacyElem->design_info.bStartRotRest);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.bEndRotRest!=pLegacyElem->design_info.bEndRotRest)
		{
			rpt.Log("当前单元%5d-%5d的终端长细比受控约束%d与原有单元不同%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.bEndRotRest,pLegacyElem->design_info.bEndRotRest);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.lamdaSelType!=pLegacyElem->design_info.lamdaSelType)
		{
			rpt.Log("当前单元%5d-%5d的长细比受控类型%d与原有单元不同%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.lamdaSelType,pLegacyElem->design_info.lamdaSelType);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.cLamdaTypeR!=pLegacyElem->design_info.cLamdaTypeR)
		{
			rpt.Log("当前单元%5d-%5d的回转轴类型%d与原有单元不同%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.cLamdaTypeR,pLegacyElem->design_info.cLamdaTypeR);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.lamdaL-pLegacyElem->design_info.lamdaL)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的计算长度%8.0f与原有单元不同%8.0f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.lamdaL,pLegacyElem->design_info.lamdaL);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.lamdaR-pLegacyElem->design_info.lamdaR)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的回转半径%8.3f与原有单元不同%8.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.lamdaR,pLegacyElem->design_info.lamdaR);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.fCoefOfBearLoad-pLegacyElem->design_info.fCoefOfBearLoad)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的承载系数%5.3f与原有单元不同%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefOfBearLoad,pLegacyElem->design_info.fCoefOfBearLoad);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.Lo_r-pLegacyElem->design_info.Lo_r)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的长细比%8.1f与原有单元不同%8.1f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.Lo_r,pLegacyElem->design_info.Lo_r);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.UltimateLo_r-pLegacyElem->design_info.UltimateLo_r)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的允许长细比%8.0f与原有单元不同%8.0f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.UltimateLo_r,pLegacyElem->design_info.UltimateLo_r);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(fabs(pElem->design_info.fCoefEffic-pLegacyElem->design_info.fCoefEffic)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的效用系数%5.3f与原有单元不同%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefEffic,pLegacyElem->design_info.fCoefEffic);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.iDangerousRule!=pLegacyElem->design_info.iDangerousRule)
		{
			rpt.Log("当前单元%5d-%5d的受控条件%d与原有单元不同%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.iDangerousRule,pLegacyElem->design_info.iDangerousRule);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.iDangerousLoadCase!=pLegacyElem->design_info.iDangerousLoadCase)
		{
			rpt.Log("当前单元%5d-%5d的受控承载条件%d与原有单元不同%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.iDangerousLoadCase,pLegacyElem->design_info.iDangerousLoadCase);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//写入ELEMFORCE_ENV MaxTension,MaxCompression,MaxMoment;
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
				rpt.Log("当前单元%5d-%5d的最大拉力出现工况{%d-%d%s#%d-%d-%d-%d}与原有单元不同{%d-%d%s#%d-%d-%d-%d}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],group_str,pElem->MaxTension.leg_no[0],pElem->MaxTension.leg_no[1],pElem->MaxTension.leg_no[2],pElem->MaxTension.leg_no[3],
					module_no[1],status_no[1],legacy_group_str,pLegacyElem->MaxTension.leg_no[0],pLegacyElem->MaxTension.leg_no[1],pLegacyElem->MaxTension.leg_no[2],pLegacyElem->MaxTension.leg_no[3]);
				agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			}
			rpt.Log("当前单元%5d-%5d的最大拉力%8.1fkN与原有单元不同%8.1fkN\n",pElem->pStart->point_i,pElem->pEnd->point_i,
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
				rpt.Log("当前单元%5d-%5d的最大压力出现工况{%d-%d%s#%d-%d-%d-%d}与原有单元不同{%d-%d%s#%d-%d-%d-%d}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],group_str,pElem->MaxCompression.leg_no[0],pElem->MaxCompression.leg_no[1],pElem->MaxCompression.leg_no[2],pElem->MaxCompression.leg_no[3],
					module_no[1],status_no[1],legacy_group_str,pLegacyElem->MaxCompression.leg_no[0],pLegacyElem->MaxCompression.leg_no[1],pLegacyElem->MaxCompression.leg_no[2],pLegacyElem->MaxCompression.leg_no[3]);
				agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			}
			rpt.Log("当前单元%5d-%5d的最大压力%8.1fkN与原有单元不同%8.1fkN\n",pElem->pStart->point_i,pElem->pEnd->point_i,
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
				rpt.Log("当前单元%5d-%5d的最大弯矩出现工况{%d-%d#%d-%d-%d-%d}与原有单元不同{%d-%d#%d-%d-%d-%d}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],pElem->MaxMoment.leg_no[0],pElem->MaxMoment.leg_no[1],pElem->MaxMoment.leg_no[2],pElem->MaxMoment.leg_no[3],
					module_no[1],status_no[1],pLegacyElem->MaxMoment.leg_no[0],pLegacyElem->MaxMoment.leg_no[1],pLegacyElem->MaxMoment.leg_no[2],pLegacyElem->MaxMoment.leg_no[3]);
				agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			}
			rpt.Log("当前单元%5d-%5d的最大弯矩%8.1fN*m与原有单元不同%8.1fN*m\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxMoment.N*0.001,pLegacyElem->MaxMoment.N*0.001);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}

#ifdef __ALFA_TEST_VERSION_
		if(fabs(pElem->MaxCompStable.N-pLegacyElem->MaxCompStable.N)>1)
#else
		if(fabs(pElem->MaxCompStable.N-pLegacyElem->MaxCompStable.N)>10)
#endif
		{
			//写入ELEMCOMPSTABLE_ENV MaxCompStable;			//压稳控制时的最大压稳计算应力
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
				rpt.Log("当前单元%5d-%5d的最大压稳力出现工况{%d-%d%s#%d-%d-%d-%d}与原有单元不同{%d-%d%s#%d-%d-%d-%d}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],group_str,pElem->MaxCompStable.leg_no[0],pElem->MaxCompStable.leg_no[1],pElem->MaxCompStable.leg_no[2],pElem->MaxCompStable.leg_no[3],
					module_no[1],status_no[1],legacy_group_str,pLegacyElem->MaxCompStable.leg_no[0],pLegacyElem->MaxCompStable.leg_no[1],pLegacyElem->MaxCompStable.leg_no[2],pLegacyElem->MaxCompStable.leg_no[3]);
				agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			}
			rpt.Log("当前单元%5d-%5d的最大压稳力%8.3fkN与原有单元不同%8.3fkN\n",pElem->pStart->point_i,pElem->pEnd->point_i,
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
				rpt.Log("当前单元%5d-%5d的压稳应力%8.1fMPa与原有单元不同%8.1fMPa\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					pElem->MaxCompStable.sigema,correctLegacySigema);
				agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			}
		}
		//对于辅助材单元表示始末端所支撑的受力材单元，用于计算辅材验算压力
		//CElement *pStartBraceForceElem,*pEndBraceForceElem;
	}
	//存储杆件两端螺栓承载系数
	agent.m_uType=INFO_COMPARE_REPORT_RODS;
	for(ROD_BEARLOADCOEF *pRod=legacyreport.hashRodConnect.GetFirst();pRod;pRod=legacyreport.hashRodConnect.GetNext())
	{
		CSuperSmartPtr<CLDSPart> pRodPart=Ta.Parts.FromHandle(pRod->hRod);
		if( pRodPart->IsLinePart()&&(
			fabs(pRod->fStartBearCoef-pRodPart.LinePartPointer()->connectStart.bearloadcoef)>0.001||
			fabs(pRod->fEndBearCoef-pRodPart.LinePartPointer()->connectEnd.bearloadcoef)>0.001))
		{
			rpt.Log("原有杆件0X%X始末端螺栓承载系数为：%5.3f-%5.3f,当前始末端螺栓承载系数为：%5.3f-%5.3f\n",
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
		sprintf(sTitle,"当前有%d项任务",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"当前有0项任务");
	pTaskListDlg->SetInfoTitle(sTitle);
	CRevisionOperDlg *pRevision=(CRevisionOperDlg*)((CMainFrame*)theApp.m_pMainWnd)->GetRevisionOperPage();
	pRevision->UpdateRevision(postreport);
	return true;
}
#include "TtaDataFile.h"
bool CPostProcessReport::ReadFromTtaRsltOutFile(LPCTSTR lpszPathName/*=NULL*/)
{
	//读入螺栓及杆件规格信息
	CLDSDoc *pDoc=theApp.GetLDSDoc();
	CString outfile;
	if(lpszPathName==NULL)
	{
		outfile=pDoc->GetPathName();
		outfile=outfile.Left(outfile.GetLength()-4);
		outfile+=".out";
		CFileDialog dlg(TRUE,"out",outfile,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"TTA计算结果输出文件(*.out)|*.out||");
		if(dlg.DoModal()!=IDOK)
			return false;
		outfile=dlg.GetPathName();
	}
	else
		outfile=lpszPathName;
	FILE* fp=fopen(outfile,"rt");
	if(fp==NULL)
		return false;	//未找到计算报告输出文件
	CTtaDataFile tta;
	tta.ReadOutFile(fp,NULL,this);
	fclose(fp);
	return true;
}
bool CPostProcessReport::CompareWithOtherProgramReport(CPostProcessReport& outside, CLogFile& rpt)
{
	//比对节点信息输出
	CTaskOutputList infoList;
	COutputInfoAgent agent(&infoList);
	agent.ListAppendFunc=AppendOutputInfoToList;
	agent.m_uType=INFO_COMPARE_REPORT_WARNNING;
	/*if(outside.Reaction.GetNodeNum()!=this->Reaction.GetNodeNum())
	{
		rpt.Log("基础反力原有数量为%d,当前数量为%d\n",outside.Reaction.GetNodeNum(),this->Reaction.GetNodeNum());
		agent.Append(0,0,rpt.LatestLog());
		return false;
	}
	agent.m_uType=INFO_COMPARE_REPORT_NODES;
	BASENODE_REACTION *pLegacyReact=outside.Reaction.GetFirst();
	for(BASENODE_REACTION *pReact=this->Reaction.GetFirst();pReact;pReact=this->Reaction.GetNext())
	{
		if(pReact->pNode!=pLegacyReact->pNode)
			rpt.Log("原有基础节点{%d}不同于当前基础节点{%d}\n",pLegacyReact->pNode->point_i,pReact->pNode->point_i);
		if(pReact->pModule!=pLegacyReact->pModule||pReact->pStatus!=pLegacyReact->pStatus)
			rpt.Log("原有基础节点{%d}的工况{%d-%d}不同于当前基础节点{%d}的工况{%d-%d}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->pModule->iNo,pLegacyReact->pStatus->iNo,
			pReact->pNode->point_i,pReact->pModule->iNo,pReact->pStatus->iNo);
		if( pReact->leg_no[0]!=pLegacyReact->leg_no[0]||pReact->leg_no[1]!=pLegacyReact->leg_no[1]||
			pReact->leg_no[2]!=pLegacyReact->leg_no[2]||pReact->leg_no[3]!=pLegacyReact->leg_no[3])
			rpt.Log("原有基础节点{%d}的接腿{%d-%d-%d-%d}不同于当前基础节点{%d}的工况{%d-%d-%d-%d}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->leg_no[0],pLegacyReact->leg_no[1],pLegacyReact->leg_no[2],pLegacyReact->leg_no[3],
			pReact->pNode->point_i,pReact->leg_no[0],pReact->leg_no[1],pReact->leg_no[2],pReact->leg_no[3]);
		GEPOINT diff=pLegacyReact->reaction_force_design-pReact->reaction_force_design;
		if(!pReact->reaction_force_design.IsEqual(pLegacyReact->reaction_force_design,0.5))
			rpt.Log("原有基础设计反力{%d　%d-%d}较当前基础设计反力{%d　%d-%d}，差额{%8.3f %8.3f %8.3f kN}\n",
			pLegacyReact->pNode->point_i,pLegacyReact->pModule->iNo,pLegacyReact->pStatus->iNo,
			pReact->pNode->point_i,pReact->pModule->iNo,pReact->pStatus->iNo,
			diff.x*0.001,diff.y*0.001,diff.z*0.001);
		pLegacyReact=outside.Reaction.GetNext();
	}
	if(outside.TowerWeight.GetNodeNum()!=this->TowerWeight.GetNodeNum())
	{
		rpt.Log("本体接腿重量统计单元原数量为%d,当前数量为%d\n",outside.TowerWeight.GetNodeNum(),this->TowerWeight.GetNodeNum());
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
			rpt.Log("重量统计记录原记录{%C%d=%.1f}与当前记录{%C%d=%.1f}不匹配\n",
				cLegacy,pLegacyWeight->cfg_no,pLegacyWeight->weight,cCurrent,pTowerWeight->cfg_no,pTowerWeight->weight);
		}
		pLegacyWeight=outside.TowerWeight.GetNext();
	}*/
	if(outside.hashElems.GetNodeNum()!=this->hashElems.GetNodeNum())
		rpt.Log("单元原数量为%d,当前数量为%d\n",outside.hashElems.GetNodeNum(),this->hashElems.GetNodeNum());
	agent.m_uType=INFO_COMPARE_REPORT_ELEMS;
	CElement *pLegacyElem=outside.hashElems.GetFirst();
	COutputInfo* pOutput=NULL;
	for(CElement *pElem=this->hashElems.GetFirst();pElem;pElem=this->hashElems.GetNext())
	{
		if((pLegacyElem=outside.hashElems.GetValue(pElem->id))==NULL)
		{
			int minPointI=min(pElem->pStart->point_i,pElem->pEnd->point_i);
			int bigPointI=max(pElem->pStart->point_i,pElem->pEnd->point_i);
			rpt.Log("外部计算报告中缺失单元{%5d:%5d-%5d}",pElem->id,minPointI,bigPointI);
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
			rpt.Log("原有单元{%5d:%5d-%5d}与当前单元{%5d:%5d-%5d}标识不同\n",
			pLegacyElem->id,arrStartI[0],arrEndI[0],pElem->id,arrStartI[1],arrEndI[1]);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#ifdef __ALFA_TEST_VERSION_
		if(pElem->relaElem.idRefElem!=pLegacyElem->relaElem.idRefElem)
		{
			rpt.Log("当前单元%5d-%5d的受压参照单元与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->relaElem.idRefElem2!=pLegacyElem->relaElem.idRefElem2)
		{
			rpt.Log("当前单元%5d-%5d的同端节点受压参照单元与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
#endif
		//double W0,H0,V0;		//拉线预张紧后后的总重力(N)及自由端在预张力作用下产生的张力水平分量和竖直分量
		//bool bAnchorEnd;		//true:拉线终端对地锚固;false:拉线始端对地锚固
		//short elem_type;	//0:表示二力杆或经典梁;1:表示柔索单元
		//最小轴计算长度信息
		if(pElem->MaxCompStable.bothComp!=pLegacyElem->MaxCompStable.bothComp)
		{
			rpt.Log("当前单元%5d-%5d的同时受压类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->MaxCompStable.bothComp,pLegacyElem->MaxCompStable.bothComp);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_BOTH_COMPRESSION;
			pOutput->compare.bValue=pLegacyElem->MaxCompStable.bothComp;
		}
		/*一般比对时都是读取TTA数据指定杆端状态,TTA计算报告中也没有杆端状态,故不比对杆端状态
		if(pElem->m_bValidStartRestStiff!=pLegacyElem->m_bValidStartRestStiff)
		{
			rpt.Log("当前单元%5d-%5d的始端转动约束类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->m_bValidEndRestStiff!=pLegacyElem->m_bValidEndRestStiff)
		{
			rpt.Log("当前单元%5d-%5d的终端转动约束类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//xx_joint_type代表可能性，而real_xx_joint_type则代表实际计算时的支座类型
		//0：完全铰接JOINT_HINGE；1：部分刚接JOINT_RIGID（限制除扭转外的转动约束）,2：自动判断JOINT_AUTOJUSTIFY
		if(pElem->start_joint_type!=pLegacyElem->start_joint_type)
		{
			rpt.Log("当前单元%5d-%5d的缺省始端连接类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->end_joint_type!=pLegacyElem->end_joint_type)
		{
			rpt.Log("当前单元%5d-%5d的缺省终端连接类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->real_start_joint_type!=pLegacyElem->real_start_joint_type)
		{
			rpt.Log("当前单元%5d-%5d的始端连接类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->real_end_joint_type!=pLegacyElem->real_end_joint_type)
		{
			rpt.Log("当前单元%5d-%5d的终端连接类型与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->start_force_type!=pLegacyElem->start_force_type)
		{
			rpt.Log("当前单元%5d-%5d的始端杆端受力状态与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->end_force_type!=pLegacyElem->end_force_type)
		{
			rpt.Log("当前单元%5d-%5d的终端杆端受力状态与原有单元不同\n",pElem->pStart->point_i,pElem->pEnd->point_i);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		//写入设计信息design_info;
		//一般比对时都是读取TTA数据指定减孔系数,TTA计算报告中也没有减孔数,故不比对减孔系数
		if(fabs(pElem->design_info.hole_out-pLegacyElem->design_info.hole_out)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的减孔系数%5.3f与原有单元不同%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.hole_out,pLegacyElem->design_info.hole_out);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		*/
		if( pElem->design_info.iDangerousLoadCase==2&&
			fabs(pElem->design_info.fCoefTensStrengthDecrease-pLegacyElem->design_info.fCoefTensStrengthDecrease)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的受拉强度折减系数%5.3f与原有单元%5.3f不同\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefTensStrengthDecrease,pLegacyElem->design_info.fCoefTensStrengthDecrease);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_TENSION_REDUCTION_COEF;
			pOutput->compare.fValue=pLegacyElem->design_info.fCoefTensStrengthDecrease;
		}
		if( pElem->design_info.iDangerousLoadCase>=3&&
			fabs(pElem->design_info.fCoefCompStrengthDecrease-pLegacyElem->design_info.fCoefCompStrengthDecrease)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的受压强度折减系数%5.3f与原有单元不同%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
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
			rpt.Log("当前单元%5d-%5d的块剪受控状态%s与原有单元不同%s\n",pElem->pStart->point_i,pElem->pEnd->point_i,(char*)state,(char*)legacy);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_BLOCK_SHEAR_STATE;
			pOutput->compare.bValue=pLegacyElem->design_info.bBlockShearActivated;
		}
		if( pElem->MaxCompression.N<CElement::COMPRESSION_ZERO_FORCE&&
			fabs(pElem->design_info.fCoefStablity-pLegacyElem->design_info.fCoefStablity)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的压稳系数%5.3f与原有单元不同%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefStablity,pLegacyElem->design_info.fCoefStablity);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_STABLE_COEF;
			pOutput->compare.fValue=pLegacyElem->design_info.fCoefStablity;
		}
		if( pElem->MaxCompression.N<CElement::COMPRESSION_ZERO_FORCE&&
			fabs(pElem->design_info.mN-pLegacyElem->design_info.mN)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的压杆稳定强度折减系数%5.3f与原有单元不同%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.mN,pLegacyElem->design_info.mN);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_STABLE_mN;
			pOutput->compare.fValue=pLegacyElem->design_info.mN;
		}
		//TODO:始端长细比受控约束类型可以根据原始数据输入文件进行部分比对，未来还需完善　wjh-2017.2.6
		/*if(pElem->design_info.bStartRotRest!=pLegacyElem->design_info.bStartRotRest)
		{
			rpt.Log("当前单元%5d-%5d的始端长细比受控约束%d与原有单元不同%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.bStartRotRest,pLegacyElem->design_info.bStartRotRest);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.bEndRotRest!=pLegacyElem->design_info.bEndRotRest)
		{
			rpt.Log("当前单元%5d-%5d的终端长细比受控约束%d与原有单元不同%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.bEndRotRest,pLegacyElem->design_info.bEndRotRest);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.lamdaSelType!=pLegacyElem->design_info.lamdaSelType)
		{
			rpt.Log("当前单元%5d-%5d的长细比受控类型%d与原有单元不同%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.lamdaSelType,pLegacyElem->design_info.lamdaSelType);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.cLamdaTypeR!=pLegacyElem->design_info.cLamdaTypeR)
		{
			rpt.Log("当前单元%5d-%5d的回转轴类型%d与原有单元不同%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.cLamdaTypeR,pLegacyElem->design_info.cLamdaTypeR);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}*/
		if(ftoi(pElem->design_info.lamdaL)!=ftoi(pLegacyElem->design_info.lamdaL))
		{
			rpt.Log("当前单元%5d-%5d的计算长度%8.0f与原有单元不同%8.0f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.lamdaL,pLegacyElem->design_info.lamdaL);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_LAMDA_L;
			pOutput->compare.fValue=pLegacyElem->design_info.lamdaL;
		}
		if(fabs(pElem->design_info.lamdaR-pLegacyElem->design_info.lamdaR)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的回转半径%8.3f与原有单元不同%8.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.lamdaR,pLegacyElem->design_info.lamdaR);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_GYRATION;
			pOutput->compare.fValue=pLegacyElem->design_info.lamdaR;
		}
		if(ftoi(pElem->design_info.Lo_r)!=ftoi(pLegacyElem->design_info.Lo_r))
		{
			rpt.Log("当前单元%5d-%5d的长细比%8.1f与原有单元不同%8.1f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.Lo_r,pLegacyElem->design_info.Lo_r);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_LAMDA;
			pOutput->compare.fValue=pLegacyElem->design_info.Lo_r;
		}
		//目前还不清楚TTA计算结果文件中如果允许长细比控制时的输出情况 wjh-2017.2.6
		/*if(fabs(pElem->design_info.fCoefEffic-pLegacyElem->design_info.fCoefEffic)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的效用系数%5.3f与原有单元不同%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefEffic,pLegacyElem->design_info.fCoefEffic);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}
		if(pElem->design_info.iDangerousRule!=pLegacyElem->design_info.iDangerousRule)
		{
			rpt.Log("当前单元%5d-%5d的受控条件%d与原有单元不同%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.iDangerousRule,pLegacyElem->design_info.iDangerousRule);
			agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
		}*/
		if(pElem->design_info.iDangerousLoadCase!=pLegacyElem->design_info.iDangerousLoadCase)
		{
			rpt.Log("当前单元%5d-%5d的受控承载条件%d与原有单元不同%d\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.iDangerousLoadCase,pLegacyElem->design_info.iDangerousLoadCase);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_DANGEROUS_CASE;
			pOutput->compare.iValue=pLegacyElem->design_info.iDangerousLoadCase;
		}
		if(fabs(pElem->design_info.fCoefOfBearLoad-pLegacyElem->design_info.fCoefOfBearLoad)>0.001)
		{
			rpt.Log("当前单元%5d-%5d的承载系数%5.3f与原有单元不同%5.3f\n",pElem->pStart->point_i,pElem->pEnd->point_i,
			pElem->design_info.fCoefOfBearLoad,pLegacyElem->design_info.fCoefOfBearLoad);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_BEARLOAD_COEF;
			pOutput->compare.fValue=pLegacyElem->design_info.fCoefOfBearLoad;
		}
		//写入ELEMFORCE_ENV MaxTension,MaxCompression,MaxMoment;
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
				rpt.Log("当前单元%5d-%5d的最大拉力出现工况{%d-%d%s}与原有单元不同{%d-%d%s}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],group_str,module_no[1],status_no[1],legacy_group_str);
				pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
				pOutput->m_idElem=pElem->id;
				pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_TENSION_CASE;
				CXhString casestr(pOutput->compare.strbuf,16,"%d-%d%s",module_no[1],status_no[1],legacy_group_str);
			}
			rpt.Log("当前单元%5d-%5d的最大拉力%8.1fkN与原有单元不同%8.1fkN\n",pElem->pStart->point_i,pElem->pEnd->point_i,
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
				rpt.Log("当前单元%5d-%5d的最大压力出现工况{%d-%d%s}与原有单元不同{%d-%d%s}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],group_str,module_no[1],status_no[1],legacy_group_str);
				pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
				pOutput->m_idElem=pElem->id;
				pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_COMPRESSION_CASE;
				CXhString casestr(pOutput->compare.strbuf,16,"%d-%d%s",module_no[1],status_no[1],legacy_group_str);
			}
			rpt.Log("当前单元%5d-%5d的最大压力%8.1fkN与原有单元不同%8.1fkN\n",pElem->pStart->point_i,pElem->pEnd->point_i,
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
			//写入ELEMCOMPSTABLE_ENV MaxCompStable;			//压稳控制时的最大压稳计算应力
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
				rpt.Log("当前单元%5d-%5d的最大压稳力出现工况{%d-%d%s}与原有单元不同{%d-%d%s}\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					module_no[0],status_no[0],group_str,module_no[1],status_no[1],legacy_group_str);
				pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
				pOutput->m_idElem=pElem->id;
				pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_STABLE_CASE;
				CXhString casestr(pOutput->compare.strbuf,16,"%d-%d%s",module_no[1],status_no[1],legacy_group_str);
			}
			rpt.Log("当前单元%5d-%5d的最大压稳力%8.3fkN与原有单元不同%8.3fkN\n",pElem->pStart->point_i,pElem->pEnd->point_i,
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
				rpt.Log("当前单元%5d-%5d的压稳应力%8.1fMPa与原有单元不同%8.1fMPa\n",pElem->pStart->point_i,pElem->pEnd->point_i,
					pElem->MaxCompStable.sigema,correctLegacySigema);
				pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
				pOutput->m_idElem=pElem->id;
				pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_STABLE_SIGEMA;
				pOutput->compare.fValue=correctLegacySigema;
			}
		}
		//实时计算工作应力
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
			else	//由于最大压稳应力出现工况不一定与最大压稳力工况一致，故不能用最大压稳力除压稳应力控制工况下的压稳系数 wjh-2013.10.15
				pElem->design_info.workSigema=pElem->MaxCompStable.N/(A*pElem->design_info.fCoefStablity);
		}
		if(fabs(pElem->design_info.workSigema-pLegacyElem->design_info.workSigema)>0.1)
		{
			rpt.Log("当前单元%5d-%5d的工作应力%8.1fMPa与原有单元不同%8.1fMPa\n",pElem->pStart->point_i,pElem->pEnd->point_i,
				pElem->design_info.workSigema,pLegacyElem->design_info.workSigema);
			pOutput=agent.Append(pElem->pStart->handle,pElem->pEnd->handle,rpt.LatestLog());
			pOutput->m_idElem=pElem->id;
			pOutput->m_ciElemDiffType=COutputInfo::ELEM_DIFF_MAX_WORK_SIGEMA;
			pOutput->compare.fValue=pLegacyElem->design_info.workSigema;
		}
		//对于辅助材单元表示始末端所支撑的受力材单元，用于计算辅材验算压力
		//CElement *pStartBraceForceElem,*pEndBraceForceElem;
	}
	COutputInfoDlg *pTaskListDlg = (COutputInfoDlg*)((CMainFrame*)theApp.m_pMainWnd)->GetOutputInfoPage();
	//if(!g_sysPara.dock.pageOutput.bDisplay)
	//	((CMainFrame*)theApp.m_pMainWnd)->OnDockpageOutput();
	((CMainFrame*)theApp.m_pMainWnd)->ActivateDockpage(RUNTIME_CLASS(COutputInfoDlg));
	pTaskListDlg->InitOutputInfoList(infoList);
	char sTitle[50]="";
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"当前有%d项任务",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"当前有0项任务");
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
//仅用于生成分析过程文件，不考虑对称项合并因素
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
		logerr.Log("文件'%s'写入打开失败!",file_path);
		return FALSE;
	}
	CLogErrorLife loglife;
	fprintf(fp,"\n");
	if(!m_bLinearAnalysis)
	{
		fprintf(fp,"  No.  δx(mm)  δy(mm)  δz(mm)   Fex(kN)  Fey(kN)  Fez(kN)  Fwx(kN)  Fwy(kN)  Fu (kN)\n");
		fprintf(fp,"---------------------------------------------------------------------------------------\n");
	}
	else
	{
		fprintf(fp,"  No.  δx(mm)  δy(mm)  δz(mm)   Fex(kN)  Fey(kN)  Fez(kN)  Fwx(kN)  Fwy(kN)\n");
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
		{	//由于结果记录都是以计算过程为单位连续记忆的，开始读取后一旦发现有非当前过程的计算结果就表示读取结束
			if(read_started)
				break;
			continue;
		}
		read_started=true;
		anal_node.pNode=Ta.Node.FromHandle(anal_node.m_hNode);
		if(anal_node.pNode==NULL)
			logerr.Log("0X%X节点在数据模型中未找到,报告书生成不完整!",anal_node.m_hNode);
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
	//杆件受力分析选材报告
	ELEM_ANALYZE_INFO elem_anal;
	hashElemAnalBuffer.SeekPosition(0);
	read_started=false;
	while(hashElemAnalBuffer.GetNextObject(&elem_anal))
	{
		if( elem_anal.m_dwStatusNo!=(DWORD)iStatusIncEiffel||
			elem_anal.m_iModuleNo!=iModuleNo||elem_anal.legword.dwQuadLeg!=dwQuadLeg)
		{	//由于结果记录都是以计算过程为单位连续记忆的，开始读取后一旦发现有非当前过程的计算结果就表示读取结束
			if(read_started)
				break;
			continue;
		}
		read_started=true;
		elem_anal.pParentRod=Ta.FromRodHandle(elem_anal.m_hParentRod);
		if(elem_anal.pParentRod==NULL)
			logerr.Log("0X%X杆件在数据模型中未找到,报告书生成不完整!",elem_anal.pParentRod);
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
		logerr.Log("文件'%s'写入打开失败!",file_path);
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
		{	//由于结果记录都是以计算过程为单位连续记忆的，开始读取后一旦发现有非当前过程的计算结果就表示读取结束
			if(read_started)
				break;
			continue;
		}
		read_started=true;
		anal_node.pNode=Ta.Node.FromHandle(anal_node.m_hNode);
		if(anal_node.pNode==NULL)
			logerr.Log("0X%X节点在数据模型中未找到,报告书生成不完整!",anal_node.m_hNode);
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
		logerr.Log("没有找到节点号%d",pointI);
	//杆件受力分析选材报告
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
		{	//由于结果记录都是以计算过程为单位连续记忆的，开始读取后一旦发现有非当前过程的计算结果就表示读取结束
			if(read_started)
				break;
			continue;
		}
		read_started=true;
		CLDSLinePart* pParentRod=Ta.FromRodHandle(elem_anal.m_hParentRod);
		if(pParentRod==NULL)
			logerr.Log("0X%X杆件在数据模型中未找到,报告书生成不完整!",elem_anal.pParentRod);
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
			//根据始末端弯矩计算弯矩在另一端的等效抵抗力
			/*if(elem_anal.iStartI==pointI)
			{
				moment=elem_anal.ME-elem_anal.MS;
				//计算弯矩向（扣除扭矩值）
				double momentPrjL=moment*lenStdVec;
				moment.Set(moment.x-lenStdVec.x*momentPrjL,moment.y-lenStdVec.y*momentPrjL,moment.z-lenStdVec.z*momentPrjL);
				MF=(moment^lenStdVec)/length;
			}
			else
			{
				moment=elem_anal.MS-elem_anal.ME;
				//计算弯矩向（扣除扭矩值）
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
	fprintf(fp,"不平衡力(N),UF.x=%.1f,UF.y=%.1f,UF.z=%.1f\n",UF.x,UF.y,UF.z);
	fprintf(fp,"UF表示不平衡力,NT为杆件轴力,VF为梁弯矩产生的端面横向剪力,Fe为当前节点实际执行的外荷载（如线条荷载、重力及风载等）\n");
	fclose(fp);
	return TRUE;
}
#endif
