// MutiThreadTask.h: interface for the CMutiThreadTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUTITHREADTASK_H__529550A0_C82C_42D5_B57C_6033D415C69C__INCLUDED_)
#define AFX_MUTITHREADTASK_H__529550A0_C82C_42D5_B57C_6033D415C69C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "afxmt.h"

class CMutiThreadTask  
{
	CEvent idleEvent;
	CCriticalSection lock;
	struct DATA_TYPE
	{
		DATA_TYPE *next;
		void *task;
	};
	DATA_TYPE *head;
	DATA_TYPE *tail;
	HANDLE m_hThread;
	bool m_bThreadRunning;
	void Start();
public:
	void *m_pContext;	//关联上下文
	bool m_bWaitOnIdle;
	int hits;
	CMutiThreadTask();
	virtual ~CMutiThreadTask();
	DWORD (WINAPI *TaskFunc)(LPVOID lpParam);	//指向任务执行函数的指针
	void Wait();	//不能在线程内调用
	void Awake();
	void OverRunningState(){m_bThreadRunning=false;}
	void AddTask(void *Task);
	void* GetHeadTask();
	HANDLE GetThreadHandle(){return m_hThread;}
	void RemoveHead();
};

#endif // !defined(AFX_MUTITHREADTASK_H__529550A0_C82C_42D5_B57C_6033D415C69C__INCLUDED_)
