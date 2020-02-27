//<LOCALE_TRANSLATE/>
// DrawSettingSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DrawSettingSheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawSettingSheet

IMPLEMENT_DYNAMIC(CDrawSettingSheet, CPropertySheet)

CDrawSettingSheet::CDrawSettingSheet(CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	AddPage(&m_StruPage);		//�ṹͼ
	AddPage(&m_PartGroupPage);	//�麸ͼ
	AddPage(&m_PartPage);		//����ͼ
	AddPage(&m_TextDimPage);	//�������ע
	AddPage(&m_DrawPenPage);	//������ɫ
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
#ifdef AFX_TARG_ENU_ENGLISH
	m_psh.pszCaption = "drawing setting";
#else
	m_psh.pszCaption = "��ͼ����";
#endif
}

CDrawSettingSheet::~CDrawSettingSheet()
{
}


BEGIN_MESSAGE_MAP(CDrawSettingSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CDrawSettingSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDrawSettingSheet message handlers


