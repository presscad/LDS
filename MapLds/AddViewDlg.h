#if !defined(AFX_ADDVIEW_H__6D7C4FA1_C3A3_44C4_949B_FF7170C7CB71__INCLUDED_)
#define AFX_ADDVIEW_H__6D7C4FA1_C3A3_44C4_949B_FF7170C7CB71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddView.h : header file
//
#include "Drawing.h"
#ifdef __SMART_DRAW_INC_
/////////////////////////////////////////////////////////////////////////////
// CAddViewDlg dialog

class CAddViewDlg : public CDialog
{
	// Construction
	typedef struct tagDRAWVIEW
	{
		int iType;
		DWORD dwView;	// ”Õº
		DWORD dwModule;	//∫Ù∏ﬂ
		tagDRAWVIEW(){;}
		tagDRAWVIEW(int type,DWORD dw1,DWORD dw2){iType=type;dwView=dw1;dwModule=dw2;}
	}DRAW_VIEW;
	ATOM_LIST<DRAW_VIEW> itemInfoList;
public:
	CAddViewDlg(CWnd* pParent = NULL);   // standard constructor
	CDrawing *m_pDrawing;
// Dialog Data
	//{{AFX_DATA(CAddViewDlg)
	enum { IDD = IDD_ADD_VIEW };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddViewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnAddView();
	afx_msg void OnBnDelView();
	virtual void OnOK();
	afx_msg void OnDblclkListSelectView();
	afx_msg void OnDblclkListRemainView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDVIEW_H__6D7C4FA1_C3A3_44C4_949B_FF7170C7CB71__INCLUDED_)
