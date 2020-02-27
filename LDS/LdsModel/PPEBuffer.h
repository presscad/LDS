#pragma once
#include "Buffer.h"
#include "Hashtable.h"
#include "Tower.h"
#include "ProcessPart.h"
class CPPEBuffer : public CBuffer
{
protected:
	DWORD version;
	CHashStrList<long> hashPartIndexPos;
	DWORD SkipProcessPartBuffer(char* sPartLabel=NULL,int nMaxBuffLen=0);	//���ص�ǰ����������Ϣ��¼���ܳ���
public:

	CPPEBuffer():CBuffer(){;}
	CPPEBuffer(char* srcBuf, DWORD buf_size);
	bool AttachPPEBuffer(char* srcBuf, DWORD buf_size);
	virtual ~CPPEBuffer(void);
	DWORD Version(){return version;}
	bool GetProcessPart(const char* sPartLabel,CProcessPart* pProcessPart);
};

