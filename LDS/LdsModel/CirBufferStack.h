#ifndef __CIRCULAR_BUFFER_STACK_H_
#define __CIRCULAR_BUFFER_STACK_H_
//缓存区循环堆栈
class CCirBufferStack
{
	typedef struct
	{
		long nBufferSize;
		char *data;
		int  type;	//主要用于存储Undo记录的操作类型 wjh-2015.7.22
		bool bStateUsed;
	}BUFFER_RECORD;
	BUFFER_RECORD *m_arrBufferRecord;
	long m_iCursor,m_nCount,m_nMaxCount;
	long m_iLastStackCursor;	//最近一次的压出栈操作的游标索引，用于视图操作的Undo/Redo wjh-2016.6.9
public:
	CCirBufferStack();
	CCirBufferStack(long nCount);
	~CCirBufferStack();
	long SetSize(long nCount);
	void Empty();
	long UndoRecordLength();				//获取栈顶元素所占内存大小
	long UndoRecordType();					//获取栈顶元素所存储的操作类型
	long RedoRecordLength();				//获取栈顶元素所占内存大小
	long RedoRecordType();					//获取栈顶元素所存储的操作类型
	long UndoRecordBuffer(void* pBuffer);	//获取栈顶元素所占内存大小
	long RedoRecordBuffer(void* pBuffer);	//获取栈顶元素所占内存大小
	void push(void* pBuffer,long bufferLen,int type=0);
	void TerminateLaterRedo();
	long popUndo(void* pBuffer);
	long popRedo(void* pBuffer);
public:	//视图操作
	long CurrentRecordSerial(){return m_iCursor;}
	long LastRecordSerial(){return m_iLastStackCursor;}
	void UpdateLastRecord(void* pBuffer,long bufferLen,int type=0);
	long LastRecordLength();				//获取最近一次压/出栈操作元素所占内存大小
	long LastRecordType();					//获取最近一次压/出栈操作元素所存储的操作类型
	long LastRecordBuffer(void* pBuffer);	//获取最近一次压/出栈操作元素所占内存大小
	const char* LastRecordBuffer();			//获取最近一次压/出栈操作元素所占内存大小
};

#endif

