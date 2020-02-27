#include "stdafx.h"
#include "LDS.h"
#include "FemSolverLoop.h"

#ifdef __PART_DESIGN_INC_
bool CFemSolverLoop::m_bQuitNow;	//��ǰ�Ƿ���Ҫ�˳����߳�
CFemModel CFemSolverLoop::femmodel;
CFemSolverLoop::CFemSolverLoop(void)
{
}


CFemSolverLoop::~CFemSolverLoop(void)
{
}

DWORD WINAPI MainPipeInstanceProc(LPVOID lpParameter)
{
	HANDLE hReadPipe =GetStdHandle(STD_INPUT_HANDLE);
	//	CreateNamedPipe(PIPE_CORE_NAME,PIPE_ACCESS_DUPLEX,PIPE_TYPE_BYTE|PIPE_READMODE_BYTE,32,0,0,2000,&CWorkLoop::sa);
	DWORD nBytesRead;
	LDS_MSGHEAD msg;
	msg.msg_length=0;
	bool exitprocess=false;
	while(1)
	{
		if(!ReadFile(hReadPipe,(void*)&msg,9,&nBytesRead,NULL))
		{
			exitprocess=true;
			break;	//����������������ж�,��ʱӦ�˳�����
		}
		if(msg.command_id==CFemSolverLoop::CMD_SOLVEFEMMODEL)	//���û�����ע��
			CFemSolverLoop::SolveFemModel(msg);
		else if(msg.command_id==CFemSolverLoop::CMD_WRITERESULTDATA)
			CFemSolverLoop::WriteFemResultData();
		else if(msg.command_id==CFemSolverLoop::CMD_QUERYPROCESS)
			CFemSolverLoop::WriteFemSolveProcessPercent();
		else if(msg.command_id==CFemSolverLoop::CMD_EXITLDSPROCESS)
		{
			exitprocess=true;
			break;
		}
		if(CFemSolverLoop::m_bQuitNow)
			break;
	}
	if(exitprocess)
		exit(0);	//theApp.ExitInstance();
	return 0;
}
void CFemSolverLoop::StartLoop()
{
	DWORD idThread;
	HANDLE hThread=CreateThread(NULL,0,&MainPipeInstanceProc,NULL,0,&idThread);
	CloseHandle(hThread);
}
void CFemSolverLoop::SolveFemModel(LDS_MSGHEAD& msghead)
{
	HANDLE hReadPipe =GetStdHandle(STD_INPUT_HANDLE);
	DWORD nBytesRead;
	CBuffer buffer(msghead.msg_length);	//����10k����Ĭ�Ͽռ�
	CHAR_ARRAY memory_pool(msghead.msg_length);
	UINT least=msghead.msg_length;
	do{
		if(!ReadFile(hReadPipe,memory_pool,least,&nBytesRead,NULL))
			return;
		least-=nBytesRead;
		if(nBytesRead==0)
			return;	//�����쳣
		buffer.Write(memory_pool,nBytesRead);
	}while(least>0);
	/*BYTE memory_pool[PACK_SIZE];
	if(msghead.msg_length>0)
	{	//��ȡ��Ϣ���ݣ���ȡ���ݵ��û���Χ��
		while(ReadFile(hReadPipe,memory_pool,PACK_SIZE,&nBytesRead,NULL))
		{
			if(memory_pool[PACK_SIZE-1]==0)
				int b=3;
			buffer.Write(memory_pool,PACK_SIZE-1);
			if(memory_pool[PACK_SIZE-1]==0)
				break;	//���ݽ��ս���
		}
	}*/
	buffer.SeekToBegin();
	femmodel.ModelFromBuffer(buffer);
	femmodel.m_ciPercent=0;
	CBuffer rsltbuf;
	if(femmodel.AnalyzeFemModel())
	{
		rsltbuf.SetBlockSize(femmodel.TestRsltsBufLength());
		femmodel.RsltsToBuffer(rsltbuf);
	}
	femmodel.InitializeLdsModel();
	DWORD nBytesWritten=0,bufsize=least=rsltbuf.GetLength();
	HANDLE hWritePipe =GetStdHandle(STD_OUTPUT_HANDLE);
	WriteFile(hWritePipe,&bufsize,4,&nBytesWritten,NULL);	//д�뻺�泤��
	do{
		if(!WriteFile(hWritePipe,rsltbuf.GetBufferPtr()+(bufsize-least),least,&nBytesWritten,NULL))
			return;
		least-=nBytesWritten;
		if(nBytesWritten==0&&least>0)
			return;	//�����쳣
	}while(least>0);
}
void CFemSolverLoop::WriteFemResultData()
{
	HANDLE hWritePipe =GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwWritten;
	BYTE memory_pool[PACK_SIZE];
	DWORD msg_length=femmodel.rsltbuf.GetLength();
	WriteFile(hWritePipe,&msg_length,4,&dwWritten,NULL);
	femmodel.rsltbuf.SeekToBegin();
	while(true)
	{
		DWORD dwReadBytes=femmodel.rsltbuf.Read(memory_pool,PACK_SIZE-1);
		if(femmodel.rsltbuf.GetRemnantSize()>0)
			memory_pool[PACK_SIZE-1]=1;	//�������
		else
		{
			memory_pool[PACK_SIZE-1]=0;	//���ݽ���
			memset(&memory_pool[dwReadBytes],0,PACK_SIZE-dwReadBytes-1);
		}
		WriteFile(hWritePipe,memory_pool,PACK_SIZE,&dwWritten,NULL);
		if(memory_pool[PACK_SIZE-1]==0)
			break;
	}
}
void CFemSolverLoop::WriteFemSolveProcessPercent()
{
	DWORD dwWritten;
	HANDLE hWritePipe =GetStdHandle(STD_OUTPUT_HANDLE);
	WriteFile(hWritePipe,&femmodel.m_ciPercent,1,&dwWritten,NULL);
}
#endif