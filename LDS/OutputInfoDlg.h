#if !defined(AFX_TASKLISTDLG_H__451DED6C_C5A6_4835_9FDA_C8E2FDF38938__INCLUDED_)
#define AFX_TASKLISTDLG_H__451DED6C_C5A6_4835_9FDA_C8E2FDF38938__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COutputInfoDlg dialog
#include "XhListCtrl.h"
#include "BasicOutput.h"

class COutputInfoDlg : public CDialog
{
// Construction
	CTaskOutputList m_xOutputInfoList;
	void ContextMenu(CWnd* pWnd, CPoint point);
	CString m_sPaneTopTitle;
public:
	COutputInfoDlg(CWnd* pParent = NULL);   // standard constructor
	
	void RelayoutWnd();
	void SetInfoTitle(CString sTitle);
	CString GetInfoTitle(){return m_sPaneTopTitle;}
	void AddOutputInfo(IBasicOutput* pItemInfo);
	void InitOutputInfoList(CTaskOutputList &itemList);
	void UpdateOutputInfoList();
	void ClearOutputInfoList();

	DECLARE_DYNCREATE(COutputInfoDlg)
// Dialog Data
	//{{AFX_DATA(COutputInfoDlg)
	enum { IDD = IDD_OUTPUT_INFO_DLG };
	CXhListCtrl	m_listOutputInfoCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputInfoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COutputInfoDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRecheck();
	afx_msg void OnMarkNormal();
	afx_msg void OnCancelMark();
	afx_msg void OnAutoCutJg();
	afx_msg void OnExportToTextFile();
	afx_msg void OnExportToExcelFile();
	afx_msg void OnCorrectErrorItem();
	afx_msg void OnClickOutputInfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkOutputInfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickOutputInfoList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKLISTDLG_H__451DED6C_C5A6_4835_9FDA_C8E2FDF38938__INCLUDED_)
