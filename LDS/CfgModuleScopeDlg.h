#if !defined(AFX_CFGMODULESCOPEDlg_H__361B1087_35BE_49F1_A400_20916303FB89__INCLUDED_)
#define AFX_CFGMODULESCOPEDLG_H__361B1087_35BE_49F1_A400_20916303FB89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CfgModuleScopeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCfgModuleScopeDlg dialog

class CCfgModuleScopeDlg : public CDialog
{
// Construction
public:
	CCfgModuleScopeDlg(CWnd* pParent = NULL);   // standard constructor
	CString m_sModuleScope;		//Ä£ÐÍºÅ×Ö·û´®

// Dialog Data
	//{{AFX_DATA(CCfgModuleScopeDlg)
	enum { IDD = IDD_CFG_MODULE_SCOPE_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCfgModuleScopeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCfgModuleScopeDlg)
	afx_msg void OnBnAddModule();
	afx_msg void OnBnDelModule();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkListAttachModule();
	afx_msg void OnDblclkListRemainModule();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CfgModuleScopeDlg_H__361B1087_35BE_49F1_A400_20916303FB89__INCLUDED_)
