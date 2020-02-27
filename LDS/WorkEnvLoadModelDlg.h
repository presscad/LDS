#if !defined(AFX_CWORKENVLOADMODELDLG_H__41352632_C243_4964_B097_EDD934635F8D__INCLUDED_)
#define AFX_CWORKENVLOADMODELDLG_H__41352632_C243_4964_B097_EDD934635F8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkEnvLoadModelDlg.h : header file
//
#include "PropertyList.h"
#include "supergridctrl.h"
#include "WireLoadDatabase.h"
/////////////////////////////////////////////////////////////////////////////
// CWorkEnvLoadModelDlg dialog
#if defined(__TSA_)||defined(__LDS_)
class CWorkEnvLoadModelDlg : public CDialog
{
// Construction
public:
	BOOL m_bModifyByUser;
	void ParseLiveLoad();
	CWireLoadModel wireModel;
	CWorkEnvLoadModelDlg(CWnd* pParent = NULL);   // standard constructor
	// Ù–‘π‹¿Ì
// Dialog Data
	//{{AFX_DATA(CWorkEnvLoadModelDlg)
	enum { IDD = IDD_WORKENVLOAD_MODEL_DLG };
	CTabCtrl	m_ctrlPropGroup;
	CPropertyList	m_propList;
	CSuperGridCtrl	m_listCtrl;
	CString	m_sWireLoadName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkEnvLoadModelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorkEnvLoadModelDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnResetClassicWorkStatus();
	afx_msg void OnPasteData();
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnCalRawWireload();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void RefreshListCtrl();
	void DisplayPropertyList();
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CWORKENVLOADMODELDLG_H__41352632_C243_4964_B097_EDD934635F8D__INCLUDED_)
