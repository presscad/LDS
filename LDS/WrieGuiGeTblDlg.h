#if !defined(AFX_WRIEGUIGETBLDLG_H__CD89C01B_E699_4FC9_B074_588D6CF0D69A__INCLUDED_)
#define AFX_WRIEGUIGETBLDLG_H__CD89C01B_E699_4FC9_B074_588D6CF0D69A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WrieGuiGeTblDlg.h : header file
//
#include "WireLoadDatabase.h"
#include "lds_part.h"
#include "supergridctrl.h"
/////////////////////////////////////////////////////////////////////////////
// CWrieGuiGeTblDlg dialog

class CWrieGuiGeTblDlg : public CDialog
{
// Construction
public:
	CWrieGuiGeTblDlg(CWnd* pParent = NULL);   // standard constructor
	CWireLoadDatabase db;
	void UpdateWireInfoList(char* wireType);
	CWireInfo  wireInfo;
	CString m_sSpecWire;
	BOOL m_bDataChange;
// Dialog Data
	//{{AFX_DATA(CWrieGuiGeTblDlg)
	enum { IDD = IDD_WIRE_LIB_DLG };
	CSuperGridCtrl	m_listCtrl;
	CString	m_sWireType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWrieGuiGeTblDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWrieGuiGeTblDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	afx_msg void OnSelchangeCmbWireType();
	afx_msg void OnBnSaveWireType();
	afx_msg void OnBnDelWireType();
	afx_msg void OnPasteData();
	afx_msg void OnDelData();
	afx_msg void OnOk2();
	afx_msg void OnEditchangeCmbWireType();
	afx_msg void OnClose();
	afx_msg void OnDeleteall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WRIEGUIGETBLDLG_H__CD89C01B_E699_4FC9_B074_588D6CF0D69A__INCLUDED_)
