#if !defined(AFX_CFGWORKSTATUSDLG_H__72069E84_1130_49E1_8A36_F88323C3AF72__INCLUDED_)
#define AFX_CFGWORKSTATUSDLG_H__72069E84_1130_49E1_8A36_F88323C3AF72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if defined(__TSA_)||defined(__LDS_)
// CfgWorkStatusDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCfgWorkStatusDlg dialog
class CCfgWorkStatusDlg : public CDialog
{
// Construction
public:
	CListBox* m_pListBoxAttach;
	CListBox* m_pListBoxRemain;
	ATOM_LIST<THANDLE>list;
	CCfgWorkStatusDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCfgWorkStatusDlg)
	enum { IDD = IDD_CFG_WORK_STATUS_DLG };
	CString	m_sLeftStr;
	CString	m_sRightStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCfgWorkStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCfgWorkStatusDlg)
	afx_msg void OnBnAddWorkStatus();
	afx_msg void OnBnDelWorkStatus();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFGWORKSTATUSDLG_H__72069E84_1130_49E1_8A36_F88323C3AF72__INCLUDED_)
