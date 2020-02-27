#pragma once
#include "afxmenubar.h"

// CLDSMFCMenuBar

class CLDSMFCMenuBar : public CMFCMenuBar
{
	DECLARE_DYNAMIC(CLDSMFCMenuBar)
	void ValidateAuthorize(UINT uiResID);
public:
	CLDSMFCMenuBar();
	virtual ~CLDSMFCMenuBar();
	virtual void CreateFromMenu(HMENU hMenu, BOOL bDefaultMenu = FALSE, BOOL bForceUpdate = FALSE);
	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
protected:
	DECLARE_MESSAGE_MAP()
};

