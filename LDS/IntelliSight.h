#pragma once
#include "list.h"
/*�������龰����, ����:
	1.���ܲ�����������
	2.�������ݺ���У��
	3.���ܲ�����������
	4.�����¹��ܷ�����ʾ
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
		UINT nTimeInterval;	//ʱ����,����ms
		UINT uiHits;		//���ϴ����������Ĵ�������
		void (CALLBACK* lpfnTimer)(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
	};
public:
	CIntelliSight(void);
	virtual ~CIntelliSight(void);
public:
	static const UINT TIMER_BASIC	= 3;	//������ʱ��
	void Start();			//���������龰����
	bool Observe(UINT idTimer);	//���ܷ����ض��龰
	bool Remind();	//��Է��ֵ��龰�����������������������
	bool Learn();	//����ѧϰ
};
extern CIntelliSight intelli;
