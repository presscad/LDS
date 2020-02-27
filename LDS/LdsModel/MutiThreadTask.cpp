// MutiThreadTask.cpp: implementation of the CMutiThreadTask class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MutiThreadTask.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMutiThreadTask::CMutiThreadTask()
{
	head=tail=NULL;
	m_hThread=0;
	TaskFunc=NULL;
	hits=0;
	m_pContext=NULL;
	m_bThreadRunning=false;
	m_bWaitOnIdle=true;
}
CMutiThreadTask::~CMutiThreadTask()
{
	while(head!=NULL)
		RemoveHead();
}
void* CMutiThreadTask::GetHeadTask()
{
	if(head)
		return head->task;
	else
		return NULL;
}

void CMutiThreadTask::RemoveHead()
{
	if(lock.Lock())
	{
		if(head!=NULL)
		{
			DATA_TYPE *next=head->next;
			delete head;
			head=next;
			if(head==NULL)
			{
				tail=NULL;
				//CloseHandle(m_hThread);
				//m_hThread=0;
			}
		}
		lock.Unlock();
	}
}
void CMutiThreadTask::Wait()
{
	idleEvent.ResetEvent();
	DWORD result=WaitForSingleObject(idleEvent,INFINITE);
}
void CMutiThreadTask::Awake()
{
	idleEvent.SetEvent();
}

void CMutiThreadTask::AddTask(void *task)
{
	if(lock.Lock())
	{
		if(tail==NULL)
			head = tail = new DATA_TYPE;
		else
		{
			tail->next = new DATA_TYPE;
			tail=tail->next;
		}
		tail->task=task;
		tail->next=NULL;
		lock.Unlock();
	}
	Start();
}

void CMutiThreadTask::Start()
{
	if(!m_bThreadRunning&&TaskFunc!=NULL)
	{
		hits++;
		if(m_hThread!=0)
			CloseHandle(m_hThread);
		DWORD dwThreadId; 
		m_hThread = CreateThread(NULL,0,TaskFunc,this,0,&dwThreadId);
		m_bThreadRunning=true;
	}
	else
		Awake();
}
