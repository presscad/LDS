// LDSMFCToolBar.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "LicFuncDef.h"
#include "LDSMFCToolBar.h"
#include "env_def.h"

// CLDSMFCToolBar

IMPLEMENT_DYNAMIC(CLDSMFCToolBar, CMFCToolBar)

CLDSMFCToolBar::CLDSMFCToolBar()
{

}

CLDSMFCToolBar::~CLDSMFCToolBar()
{
}


BEGIN_MESSAGE_MAP(CLDSMFCToolBar, CMFCToolBar)
END_MESSAGE_MAP()



// CLDSMFCToolBar ��Ϣ�������

int ToolbarButtonIndexFromID(CMFCToolBar* pToolbar,DWORD btnResID)
{
	CMFCToolBarButton* pBtn;
	for(int i=0;pToolbar->GetAllButtons().GetSize()>0;i++){
		if((pBtn=pToolbar->GetButton(i))==NULL)
			break;
		if(pBtn->m_nID==btnResID)
			return i;
	};
	return -1;
}
void CLDSMFCToolBar::ValidateAuthorize(UINT uiResID)
{
	int index;
	UINT TOOLKIT_ID=theApp.m_bAnalysisWorkEnvOnly?IDR_BAR_TOOLKIT_TSA:IDR_BAR_TOOLKIT;
	if(uiResID==theApp.m_nMainMenuID)
	{	//m_wndToolBar ��������
#if !defined(__LDS_)&&!defined(__TSA_)
		if(true)
#else
		if(theApp.m_bDrawingWorkEnvOnly)
#endif
		{
			if((index=ToolbarButtonIndexFromID(this,ID_DISPLAY_DEFORM_TOWER))>=0)
				RemoveButton(index);	//�Ƴ���Ԥ�����κ��������ť
			if((index=ToolbarButtonIndexFromID(this,ID_DISPLAY_VIBRATION_MODE))>=0)
				RemoveButton(index);	//�Ƴ�������ģ�����ͣ���ť
		}
	}
	else if(uiResID==TOOLKIT_ID)
	{	//m_wndToolKit �����༭�޸Ĺ�����
		if(!PRODUCT_FUNC::IsHasInternalTest())
		{
			if((index=ToolbarButtonIndexFromID(this,ID_REVISION_MODE))>=0)
				RemoveButton(index);
			if((index=ToolbarButtonIndexFromID(this,ID_SIZE_DIMENSION))>=0)
				RemoveButton(index);
		}
#ifndef __COMPLEX_PART_INC_
		if((index=ToolbarButtonIndexFromID(this,ID_CUT_PLATE_BY_COLUMN_FACE))>=0)
			RemoveButton(index);	//Բ�����и�ְ�
#endif
	}
}
BOOL CLDSMFCToolBar::LoadToolBar(UINT uiResID, UINT uiColdResID, UINT uiMenuResID, BOOL bLocked, UINT uiDisabledResID, UINT uiMenuDisabledResID, UINT uiHotResID)
{
	BOOL retcode=CMFCToolBar::LoadToolBar(uiResID,uiColdResID,uiMenuResID,bLocked,uiDisabledResID,uiMenuDisabledResID,uiHotResID);
	ValidateAuthorize(uiResID);
	return retcode;
}
BOOL CLDSMFCToolBar::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	BOOL retcode=CMFCToolBar::LoadState(lpszProfileName, nIndex, uiID);
	ValidateAuthorize(m_uiOriginalResID);
	return retcode;
}

