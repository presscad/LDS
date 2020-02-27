#if !defined(AFX_DRAGLISTCTRL_H__79B0BD3E_63DF_4863_A9C2_AFDBCEE24D09__INCLUDED_)
#define AFX_DRAGLISTCTRL_H__79B0BD3E_63DF_4863_A9C2_AFDBCEE24D09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDragListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDragListCtrl window
#define MAX_DRAG_SIZE 128
#define MAX_DRAG_SIZE_2 (MAX_DRAG_SIZE/2)
class CDragListCtrl : public CListCtrl
{
// Construction
public:
	CDragListCtrl();

// Attributes
public:
	CImageList* m_pDragImage;
	int m_nDragDrop, m_nDrag[MAX_DRAG_SIZE];

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDragListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDragListCtrl();
	CImageList* CreateDragImage(const CPoint& ptMouse);  
	// Generated message map functions
protected:
	//{{AFX_MSG(CDragListCtrl)
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAGLISTCTRL_H__79B0BD3E_63DF_4863_A9C2_AFDBCEE24D09__INCLUDED_)
