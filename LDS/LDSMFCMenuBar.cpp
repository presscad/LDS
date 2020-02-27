// LDSMFCToolBar.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "LicFuncDef.h"
#include "LDSMFCMenuBar.h"
#include "env_def.h"

// CLDSMFCMenuBar

IMPLEMENT_DYNAMIC(CLDSMFCMenuBar, CMFCMenuBar)

CLDSMFCMenuBar::CLDSMFCMenuBar()
{
}

CLDSMFCMenuBar::~CLDSMFCMenuBar()
{
}


BEGIN_MESSAGE_MAP(CLDSMFCMenuBar, CMFCMenuBar)
END_MESSAGE_MAP()



// CLDSMFCMenuBar 消息处理程序
void CLDSMFCMenuBar::ValidateAuthorize(UINT uiResID)
{
	if(m_uiDefMenuResId!=theApp.m_nMainMenuID)
		return;
	//主菜单
	if(theApp.m_nMainMenuID==IDR_MAINFRAME_TDA)
	{
		CMFCToolBarButton* pButton=GetButton(7);
		CString btnStr=pButton?pButton->m_strText:"";
#ifdef __LDS_
		if(pButton!=NULL&&!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_PROCESS_EXPORT)&&strstr(btnStr,"NC加工")!=NULL)
#endif
			RemoveButton(7);//::DeleteMenu(m_hMenu,7,MF_BYPOSITION);
	}
#ifdef __LDS_
	if(theApp.m_nMainMenuID==IDR_MAINFRAME)
	{
		CMFCToolBarButton* pButton=GetButton(8);
		CString btnStr=pButton?pButton->m_strText:"";
		if(pButton!=NULL&&!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_PROCESS_EXPORT)&&strstr(btnStr,"NC加工")!=NULL)
			RemoveButton(8);//::DeleteMenu(m_hMenu,8,MF_BYPOSITION);
	}
#endif
#if !defined(__COMPLEX_PART_INC_)||defined(__TMA_)
	::DeleteMenu(m_hMenu,ID_CUT_PLATE_BY_COLUMN_FACE,MF_BYCOMMAND);	//圆柱面切割钢板
#endif
}
void CLDSMFCMenuBar::CreateFromMenu(HMENU hMenu, BOOL bDefaultMenu/*= FALSE*/, BOOL bForceUpdate/*= FALSE*/)
{
	CMFCMenuBar::CreateFromMenu(hMenu,bDefaultMenu,bForceUpdate);
	ValidateAuthorize(m_uiDefMenuResId);
}
BOOL CLDSMFCMenuBar::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	BOOL retcode=CMFCMenuBar::LoadState(lpszProfileName, nIndex, uiID);
	ValidateAuthorize(m_uiDefMenuResId);
	return retcode;
}

