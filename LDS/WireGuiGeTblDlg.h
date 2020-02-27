#if !defined(AFX_WIREGUIGETBLDLG_H__CD89C01B_E699_4FC9_B074_588D6CF0D69A__INCLUDED_)
#define AFX_WIREGUIGETBLDLG_H__CD89C01B_E699_4FC9_B074_588D6CF0D69A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WrieGuiGeTblDlg.h : header file
//
#include "afxwin.h"
#include "WireLoadDatabase.h"
#include "lds_part.h"
#include "supergridctrl.h"
/////////////////////////////////////////////////////////////////////////////
// CWireGuiGeTblDlg dialog

class CWireGuiGeTblDlg : public CDialog
{
	CToolTipCtrl m_ctrlToolTip;
// Construction
public:
	CWireGuiGeTblDlg(CWnd* pParent = NULL);   // standard constructor
	CWireLoadDatabase db;
	void UpdateWireInfoList(char* wireType);
	CWireInfo  wireInfo;
	CString m_sSpecWire;
	BOOL m_bDataChange;
// Dialog Data
	//{{AFX_DATA(CWireGuiGeTblDlg)
	enum { IDD = IDD_WIRE_LIB_DLG };
	CSuperGridCtrl	m_listCtrl;
	CString	m_sWireType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWireGuiGeTblDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWireGuiGeTblDlg)
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCancelMode();
	afx_msg void OnSelchangeCmbWireType();
	afx_msg void OnBnSaveWireType();
	afx_msg void OnBnDelWireType();
	afx_msg void OnPasteData();
	afx_msg void OnDelData();
	afx_msg void OnOk();
	afx_msg void OnEditchangeCmbWireType();
	afx_msg void OnClose();
	afx_msg void OnDeleteAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIREGUIGETBLDLG_H__CD89C01B_E699_4FC9_B074_588D6CF0D69A__INCLUDED_)
