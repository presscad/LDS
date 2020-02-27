#pragma once
#include "afxtoolbar.h"

// CLDSMFCToolBar

class CLDSMFCToolBar : public CMFCToolBar
{
	DECLARE_DYNAMIC(CLDSMFCToolBar)
	void ValidateAuthorize(UINT uiResID);
public:
	CLDSMFCToolBar();
	virtual ~CLDSMFCToolBar();
	virtual BOOL LoadToolBar(UINT uiResID, UINT uiColdResID = 0, UINT uiMenuResID = 0, BOOL bLocked = FALSE,
		UINT uiDisabledResID = 0, UINT uiMenuDisabledResID = 0,  UINT uiHotResID = 0);
	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
protected:
	DECLARE_MESSAGE_MAP()
};


