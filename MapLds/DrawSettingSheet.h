// DrawSettingSheet.h : header file
//
// This class defines custom modal property sheet 
// CDrawSettingSheet.
 
#ifndef __DRAWSETTINGSHEET_H__
#define __DRAWSETTINGSHEET_H__

#include "MapSetPage.h"

/////////////////////////////////////////////////////////////////////////////
// CDrawSettingSheet

class CDrawSettingSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDrawSettingSheet)

// Construction
public:
	CDrawSettingSheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CStruMapPage m_StruPage;
	CPartMapPage m_PartPage;
	CDimPage m_TextDimPage;
	CDrawPenPage m_DrawPenPage;
	CPartGroupPage m_PartGroupPage;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawSettingSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDrawSettingSheet();

// Generated message map functions
protected:
	//{{AFX_MSG(CDrawSettingSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __DRAWSETTINGSHEET_H__
