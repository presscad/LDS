#include "stdafx.h"
#include "LDS.h"
#include "IntelliSight.h"
#include "env_def.h"
#include "LDSDoc.h"
#include "AutoSaveParamDlg.h"

CIntelliSight intelli;
bool CIntelliSightAutoSaveFile::ProcessTimerEvent(UINT nIdTimerEvent)
{
	if(nIdTimerEvent!=CIntelliSight::TIMER_BASIC)
		return false;
	if(g_sysPara.nAutoSaveTime==0)
		return false;	//����Ϊ��ͣ�Զ����̹���
	if(theApp.GetLDSDoc()->IsStartAutoSaveThread()||theApp.m_bCooperativeWork)
		return false;	//�û����������Զ����̹��ܻ�ǰ����Эͬ��ҵ�ͻ��� wjh-2016.3.17
	m_uiVisitHits++;
	if((m_uiVisitHits+1)%600==0&&Ta.CanUndoOper())	//ÿ10���Ӽ��һ��
	{
		m_bActivated=true;
		return true;
	}
	else
		return false;
}
bool CIntelliSightAutoSaveFile::ItemSolve()
{
    // �����ʾ�û������Զ��ļ����̲���        ccs 2015-12-14
	//CString sDisplayDlg=GetProfileString("Settings","displayAutoSaveParamDlg",NULL);
	//if(strcmp(sDisplayDlg,"TRUE")!=0)
	{
		CAutoSaveParamDlg dlg;
		if(dlg.DoModal()==IDOK)
		{
			g_sysPara.nAutoSaveTime=atoi(dlg.m_sAutoSaveTime)*60000;
			g_sysPara.iAutoSaveType=dlg.m_iAutoSaveType;
			theApp.GetLDSDoc()->SaveFile();
		}
		//CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd; 
		//CLDSView *pView=(CLDSView *)pMain->GetActiveView(); 
		//pView->UpdatePropertyPage();
	}
	m_bActivated=false;
	return true;
}
static CIntelliSightAutoSaveFile autosavefileItem;

CIntelliSight::CIntelliSight(void)
{
	listObserveItems.AttachObject(&autosavefileItem);
}


CIntelliSight::~CIntelliSight(void)
{
	//AfxGetMainWnd()->KillTimer(TIMER_BASIC);
}
void CALLBACK EXPORT BasicIntelliSightTimerTrigger(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	intelli.Observe(nIDEvent);
	AfxGetMainWnd()->SetTimer(CIntelliSight::TIMER_BASIC,1000,BasicIntelliSightTimerTrigger);
}
void CIntelliSight::Start()			//���������龰����
{
	AfxGetMainWnd()->SetTimer(TIMER_BASIC,1000,BasicIntelliSightTimerTrigger);
}
bool CIntelliSight::Observe(UINT idTimer)	//���ܷ����ض��龰
{
	bool observed=false;
	for(IIntelliSightItem** ppSight=listObserveItems.EnumObjectFirst();ppSight;ppSight=listObserveItems.EnumObjectNext())
		observed|=(*ppSight)->ProcessTimerEvent(idTimer);
	return observed;
}
bool CIntelliSight::Remind()	//��Է��ֵ��龰�����������������������
{
	bool remind=false;
	for(IIntelliSightItem** ppSight=listObserveItems.EnumObjectFirst();ppSight;ppSight=listObserveItems.EnumObjectNext())
	{
		if((*ppSight)->IsActivating())
			remind|=(*ppSight)->ItemSolve();
	}
	return remind;
}
bool CIntelliSight::Learn()	//����ѧϰ
{
	return false;
}
