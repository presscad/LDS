#ifndef __CIRCULAR_BUFFER_STACK_H_
#define __CIRCULAR_BUFFER_STACK_H_
//������ѭ����ջ
class CCirBufferStack
{
	typedef struct
	{
		long nBufferSize;
		char *data;
		int  type;	//��Ҫ���ڴ洢Undo��¼�Ĳ������� wjh-2015.7.22
		bool bStateUsed;
	}BUFFER_RECORD;
	BUFFER_RECORD *m_arrBufferRecord;
	long m_iCursor,m_nCount,m_nMaxCount;
	long m_iLastStackCursor;	//���һ�ε�ѹ��ջ�������α�������������ͼ������Undo/Redo wjh-2016.6.9
public:
	CCirBufferStack();
	CCirBufferStack(long nCount);
	~CCirBufferStack();
	long SetSize(long nCount);
	void Empty();
	long UndoRecordLength();				//��ȡջ��Ԫ����ռ�ڴ��С
	long UndoRecordType();					//��ȡջ��Ԫ�����洢�Ĳ�������
	long RedoRecordLength();				//��ȡջ��Ԫ����ռ�ڴ��С
	long RedoRecordType();					//��ȡջ��Ԫ�����洢�Ĳ�������
	long UndoRecordBuffer(void* pBuffer);	//��ȡջ��Ԫ����ռ�ڴ��С
	long RedoRecordBuffer(void* pBuffer);	//��ȡջ��Ԫ����ռ�ڴ��С
	void push(void* pBuffer,long bufferLen,int type=0);
	void TerminateLaterRedo();
	long popUndo(void* pBuffer);
	long popRedo(void* pBuffer);
public:	//��ͼ����
	long CurrentRecordSerial(){return m_iCursor;}
	long LastRecordSerial(){return m_iLastStackCursor;}
	void UpdateLastRecord(void* pBuffer,long bufferLen,int type=0);
	long LastRecordLength();				//��ȡ���һ��ѹ/��ջ����Ԫ����ռ�ڴ��С
	long LastRecordType();					//��ȡ���һ��ѹ/��ջ����Ԫ�����洢�Ĳ�������
	long LastRecordBuffer(void* pBuffer);	//��ȡ���һ��ѹ/��ջ����Ԫ����ռ�ڴ��С
	const char* LastRecordBuffer();			//��ȡ���һ��ѹ/��ջ����Ԫ����ռ�ڴ��С
};

#endif

