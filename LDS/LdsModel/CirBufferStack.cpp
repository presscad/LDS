#include "stdafx.h"
#include "CirBufferStack.h"

CCirBufferStack::CCirBufferStack()
{
	m_iCursor = m_nCount = 0;
	m_iLastStackCursor=-1;
	m_nMaxCount = 257;
	m_arrBufferRecord = new BUFFER_RECORD[m_nMaxCount];
	memset(m_arrBufferRecord,0,sizeof(BUFFER_RECORD)*m_nMaxCount);
}

CCirBufferStack::CCirBufferStack(long nCount)
{
	m_iCursor = m_nCount = 0;
	m_iLastStackCursor=-1;
	m_nMaxCount = nCount+1;
	m_arrBufferRecord = new BUFFER_RECORD[m_nMaxCount];
	memset(m_arrBufferRecord,0,sizeof(BUFFER_RECORD)*m_nMaxCount);
}
CCirBufferStack::~CCirBufferStack()
{
	for(int i=0;i<m_nMaxCount;i++)
	{
		if(m_arrBufferRecord[i].bStateUsed&&m_arrBufferRecord[i].data!=NULL)
			delete[] m_arrBufferRecord[i].data;
	}
	delete []m_arrBufferRecord;
}
long CCirBufferStack::SetSize(long nCount)
{
	if(nCount<1)
		return m_nMaxCount;
	else
	{
		Empty();
		m_arrBufferRecord = new BUFFER_RECORD[m_nMaxCount];
		memset(m_arrBufferRecord,0,sizeof(BUFFER_RECORD)*m_nMaxCount);
		m_nMaxCount = nCount;
	}
	return m_nMaxCount;
}
void CCirBufferStack::Empty()
{
	for(int i=0;i<m_nMaxCount;i++)
	{
		if(m_arrBufferRecord[i].bStateUsed&&m_arrBufferRecord[i].data!=NULL)
		{
			delete[] m_arrBufferRecord[i].data;
			m_arrBufferRecord[i].bStateUsed=false;
			m_arrBufferRecord[i].data=NULL;
		}
	}
	m_iCursor = m_nCount = 0;
	m_iLastStackCursor=-1;
}
long CCirBufferStack::UndoRecordLength()
{
	int prev_i=(m_iCursor+m_nMaxCount-1)%m_nMaxCount;
	if(m_arrBufferRecord[prev_i].bStateUsed)
		return m_arrBufferRecord[prev_i].nBufferSize;
	else
		return -1;
}
long CCirBufferStack::UndoRecordType()					//获取栈顶元素所存储的操作类型
{
	int prev_i=(m_iCursor+m_nMaxCount-1)%m_nMaxCount;
	if(m_arrBufferRecord[prev_i].bStateUsed)
		return m_arrBufferRecord[prev_i].type;
	else
		return 0;
}
long CCirBufferStack::RedoRecordLength()
{
	int iSafeCursor=m_iCursor%m_nMaxCount;
	if(m_arrBufferRecord[iSafeCursor].bStateUsed)
		return m_arrBufferRecord[iSafeCursor].nBufferSize;
	else
		return -1;
}
long CCirBufferStack::RedoRecordType()					//获取栈顶元素所存储的操作类型
{
	int iSafeCursor=m_iCursor%m_nMaxCount;
	if(m_arrBufferRecord[iSafeCursor].bStateUsed)
		return m_arrBufferRecord[iSafeCursor].type;
	else
		return 0;
}
long CCirBufferStack::UndoRecordBuffer(void* pBuffer)
{
	int prev_i=(m_iCursor+m_nMaxCount-1)%m_nMaxCount;
	if(m_arrBufferRecord[prev_i].bStateUsed)
	{
		memcpy(pBuffer,m_arrBufferRecord[prev_i].data,m_arrBufferRecord[prev_i].nBufferSize);
		return m_arrBufferRecord[prev_i].nBufferSize;
	}
	else
		return 0;
}
long CCirBufferStack::RedoRecordBuffer(void* pBuffer)
{
	int iSafeCursor=m_iCursor%m_nMaxCount;
	if(m_arrBufferRecord[iSafeCursor].bStateUsed)
	{
		memcpy(pBuffer,m_arrBufferRecord[iSafeCursor].data,m_arrBufferRecord[iSafeCursor].nBufferSize);
		return m_arrBufferRecord[iSafeCursor].nBufferSize;
	}
	else
		return 0;
}
void CCirBufferStack::push(void* pBuffer,long bufferLen,int type/*=0*/)
{
	int critical_i=(m_iCursor+1)%m_nMaxCount;
	int iSafeCursor=m_iLastStackCursor=m_iCursor%m_nMaxCount;
	//将缓存区压入当前栈节点
	if(m_arrBufferRecord[iSafeCursor].bStateUsed&&m_arrBufferRecord[iSafeCursor].data!=NULL)
		delete []m_arrBufferRecord[iSafeCursor].data;
	m_arrBufferRecord[iSafeCursor].type=type;
	m_arrBufferRecord[iSafeCursor].nBufferSize=bufferLen;
	m_arrBufferRecord[iSafeCursor].data = new char[bufferLen];
	memcpy(m_arrBufferRecord[iSafeCursor].data,pBuffer,bufferLen);
	m_arrBufferRecord[iSafeCursor].bStateUsed=true;
	//处理临界区域
	if(m_arrBufferRecord[critical_i].bStateUsed&&m_arrBufferRecord[critical_i].data!=NULL)
		delete []m_arrBufferRecord[critical_i].data;
	m_arrBufferRecord[critical_i].bStateUsed=false;
	m_arrBufferRecord[critical_i].data=NULL;
	m_arrBufferRecord[critical_i].nBufferSize=0;
	m_iCursor++;
}
void CCirBufferStack::TerminateLaterRedo()
{
	int iSafeCursor=m_iCursor=(m_iLastStackCursor+1)%m_nMaxCount;
	//处理临界区域
	if(m_arrBufferRecord[iSafeCursor].bStateUsed&&m_arrBufferRecord[iSafeCursor].data!=NULL)
		delete []m_arrBufferRecord[iSafeCursor].data;
	m_arrBufferRecord[iSafeCursor].bStateUsed=false;
	m_arrBufferRecord[iSafeCursor].data=NULL;
	m_arrBufferRecord[iSafeCursor].nBufferSize=0;
}
long CCirBufferStack::popUndo(void* pBuffer)
{
	int prev_i=m_iLastStackCursor=(m_iCursor+m_nMaxCount-1)%m_nMaxCount;
	if(m_arrBufferRecord[prev_i].bStateUsed&&m_arrBufferRecord[prev_i].data!=NULL)
	{
		if(pBuffer)
			memcpy(pBuffer,m_arrBufferRecord[prev_i].data,m_arrBufferRecord[prev_i].nBufferSize);
		m_iCursor=prev_i;
		return m_arrBufferRecord[prev_i].nBufferSize;
	}
	else
		return 0;
}
long CCirBufferStack::popRedo(void* pBuffer)
{
	int iSafeCursor=m_iLastStackCursor=m_iCursor%m_nMaxCount;
	if(m_arrBufferRecord[iSafeCursor].bStateUsed&&m_arrBufferRecord[iSafeCursor].data!=NULL)
	{
		if(pBuffer)
			memcpy(pBuffer,m_arrBufferRecord[iSafeCursor].data,m_arrBufferRecord[iSafeCursor].nBufferSize);
		long nBufferLen=m_arrBufferRecord[iSafeCursor].nBufferSize;
		m_iCursor=(m_iCursor+1)%m_nMaxCount;
		return nBufferLen;
	}
	else
		return 0;
}
//视图操作
void CCirBufferStack::UpdateLastRecord(void* pBuffer,long bufferLen,int type/*=0*/)
{
	if(m_iLastStackCursor<0)
		return push(pBuffer,bufferLen,type);
	int iSafeCursor=m_iLastStackCursor%m_nMaxCount;
	m_arrBufferRecord[iSafeCursor].type=type;
	if(m_arrBufferRecord[iSafeCursor].data!=NULL&&m_arrBufferRecord[iSafeCursor].nBufferSize!=bufferLen)
	{
		delete []m_arrBufferRecord[iSafeCursor].data;
		m_arrBufferRecord[iSafeCursor].data=NULL;
	}
	m_arrBufferRecord[iSafeCursor].nBufferSize=bufferLen;
	if(m_arrBufferRecord[iSafeCursor].data==NULL)
		m_arrBufferRecord[iSafeCursor].data = new char[bufferLen];
	memcpy(m_arrBufferRecord[iSafeCursor].data,pBuffer,bufferLen);
	m_arrBufferRecord[iSafeCursor].bStateUsed=true;
}
long CCirBufferStack::LastRecordLength()				//获取最近一次压/出栈操作元素所占内存大小
{
	if(m_iLastStackCursor<0)
		return 0;
	int iSafeCursor=m_iLastStackCursor%m_nMaxCount;
	if(m_arrBufferRecord[iSafeCursor].bStateUsed)
		return m_arrBufferRecord[iSafeCursor].nBufferSize;
	else
		return -1;
}
long CCirBufferStack::LastRecordType()					//获取最近一次压/出栈操作元素所存储的操作类型
{
	if(m_iLastStackCursor<0)
		return 0;
	int iSafeCursor=m_iLastStackCursor%m_nMaxCount;
	if(m_arrBufferRecord[iSafeCursor].bStateUsed)
		return m_arrBufferRecord[iSafeCursor].type;
	else
		return -1;
}
long CCirBufferStack::LastRecordBuffer(void* pBuffer)	//获取最近一次压/出栈操作元素所占内存大小
{
	if(m_iLastStackCursor<0)
		return 0;
	int iSafeCursor=m_iLastStackCursor%m_nMaxCount;
	if(m_arrBufferRecord[iSafeCursor].bStateUsed)
	{
		memcpy(pBuffer,m_arrBufferRecord[iSafeCursor].data,m_arrBufferRecord[iSafeCursor].nBufferSize);
		return m_arrBufferRecord[iSafeCursor].nBufferSize;
	}
	else
		return 0;
}
const char* CCirBufferStack::LastRecordBuffer()			//获取最近一次压/出栈操作元素所占内存大小
{
	if(m_iLastStackCursor<0)
		return 0;
	int iSafeCursor=m_iLastStackCursor%m_nMaxCount;
	if(m_arrBufferRecord[iSafeCursor].bStateUsed)
		return m_arrBufferRecord[iSafeCursor].data;
	else
		return NULL;
}
