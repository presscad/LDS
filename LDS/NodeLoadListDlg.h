#if !defined(AFX_NODELOADLISTDLG_H__A09EAECA_4D9A_4BC4_AC8A_F40D5DDD73A7__INCLUDED_)
#define AFX_NODELOADLISTDLG_H__A09EAECA_4D9A_4BC4_AC8A_F40D5DDD73A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NodeLoadListDlg.h : header file
//
#include "SuperGridCtrl.h"
#include "CallBackDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CNodeLoadListDlg dialog

#if defined(__TSA_)||defined(__LDS_)
class CNodeLoadListDlg : public CCallBackDialog
{
// Construction
public:
	long m_iWireLayoutNo;
	CNodeLoadListDlg(CWnd* pParent = NULL);   // standard constructor
	void OnSelectObject();
	void SetPickObjTypeId(int cls_id);
	void SetCmdPickPromptStr(CString sPromptStr);
// Dialog Data
	//{{AFX_DATA(CNodeLoadListDlg)
	enum { IDD = IDD_NODE_LOAD_LIST_DLG };
	CSuperGridCtrl	m_listCtrl;
	//}}AFX_DATA
	double Hhandle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNodeLoadListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNodeLoadListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg void OnNewData();
	afx_msg void OnPasteData();
	afx_msg void OnShutAll();
	afx_msg void OnExpandAll();
	afx_msg void OnDeleteNode();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NODELOADLISTDLG_H__A09EAECA_4D9A_4BC4_AC8A_F40D5DDD73A7__INCLUDED_)
