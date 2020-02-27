#if !defined(AFX_PARAMPLATEPARAMLISTDLG_H__50A6987C_2A5C_47FA_9668_57FE2B7183F4__INCLUDED_)
#define AFX_PARAMPLATEPARAMLISTDLG_H__50A6987C_2A5C_47FA_9668_57FE2B7183F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParamPlateParamListDlg.h : header file
//
#include "PropertyList.h"
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CParamPlateParamListDlg dialog

class CParamPlateParamListDlg : public CDialog
{
// Construction
public:
	RECT m_rcClient;
	int m_nOldHorzY;
	HICON m_hCursorArrow;
	HICON m_hCursorSize;
	BOOL m_bTracking;
	int m_nSplitterWidth;
	CLDSParamPlate *m_pParamPlate;
	CBitmap m_CurPlateBmp;
public:
	CParamPlateParamListDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL IsHasFocus();
	CPropertyList *GetParamList(){return &m_paramList;}
	void DisplayParamPlateDesignParam();
// Dialog Data
	//{{AFX_DATA(CParamPlateParamListDlg)
	enum { IDD = IDD_PARAMPLATE_PARAM_LIST_DLG };
	CPropertyList	m_paramList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParamPlateParamListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InvertLine(CDC* pDC,CPoint ptFrom,CPoint ptTo);
	// Generated message map functions
	//{{AFX_MSG(CParamPlateParamListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnBtnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAMPLATEPARAMLISTDLG_H__50A6987C_2A5C_47FA_9668_57FE2B7183F4__INCLUDED_)
