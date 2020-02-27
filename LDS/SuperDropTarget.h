#if !defined(AFX_SUPERDROPTARGET_H__4BEACB07_6C08_4996_B474_D7E59AEE9E5D__INCLUDED_)
#define AFX_SUPERDROPTARGET_H__4BEACB07_6C08_4996_B474_D7E59AEE9E5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SuperDropTarget.h : header file
//


#include <afxole.h>
/////////////////////////////////////////////////////////////////////////////
// CSuperDropTarget command target

class CSuperDropTarget : public COleDropTarget
{
	//DECLARE_DYNCREATE(CSuperDropTarget)
public:
	CSuperDropTarget();
	DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	DROPEFFECT OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point);
	//void OnDragLeave(CWnd* pWnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSuperDropTarget)
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CSuperDropTarget();

	// Generated message map functions
	//{{AFX_MSG(CSuperDropTarget)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUPERDROPTARGET_H__4BEACB07_6C08_4996_B474_D7E59AEE9E5D__INCLUDED_)
