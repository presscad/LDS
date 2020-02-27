#pragma once
#include "list.h"
/*　智能情景发现, 包括:
	1.智能操作经验提醒
	2.智能数据合理校验
	3.智能操作帮助提醒
	4.智能新功能发掘提示
*/
typedef struct IIntelliSightItem{	//INTELLISIGHT_ITEM
	virtual bool ProcessTimerEvent(UINT nIdTimerEvent)=0;
	virtual bool IsActivating()=0;
	virtual bool ItemSolve()=0;
}*IIntelliSightItemPtr;
class CIntelliSightAutoSaveFile : public IIntelliSightItem{
	bool m_bActivated;
	UINT m_uiVisitHits;
public:
	CIntelliSightAutoSaveFile(){m_bActivated=false;m_uiVisitHits=0;}
	virtual bool IsActivating(){return m_bActivated;}
	virtual bool ProcessTimerEvent(UINT nIdTimerEvent);
	virtual bool ItemSolve();
};
class CIntelliSight
{
	CXhSimpleList<IIntelliSightItemPtr> listObserveItems;
public:
	struct TIMER_OBJECT{
		BOOL bEnabled;
		UINT idTimer;
		UINT nTimeInterval;	//时间间隔,毫秒ms
		UINT uiHits;		//自上次清零以来的触发次数
		void (CALLBACK* lpfnTimer)(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
	};
public:
	CIntelliSight(void);
	virtual ~CIntelliSight(void);
public:
	static const UINT TIMER_BASIC	= 3;	//基础计时器
	void Start();			//启动智能情景发现
	bool Observe(UINT idTimer);	//智能发现特定情景
	bool Remind();	//针对发现的情景及待处理任务进行智能提醒
	bool Learn();	//智能学习
};
extern CIntelliSight intelli;
