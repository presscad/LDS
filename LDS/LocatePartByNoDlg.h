#if !defined(AFX_LOCATEPARTBYNODLG_H__21507D7B_A2E2_4FCC_BFB1_AEBFF7303A1B__INCLUDED_)
#define AFX_LOCATEPARTBYNODLG_H__21507D7B_A2E2_4FCC_BFB1_AEBFF7303A1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LocatePartByNoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLocatePartByNoDlg dialog

class CLocatePartByNoDlg : public CDialog
{
// Construction
public:
	CLocatePartByNoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLocatePartByNoDlg)
	enum { IDD = IDD_LOCATE_PART_BY_NO };
	CComboBox	m_cmbPartHandle;
	CString	m_sPartNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocatePartByNoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLocatePartByNoDlg)
	afx_msg void OnFindPart();
	afx_msg void OnSelchangePartHandle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBtnClickedSelectAll();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCATEPARTBYNODLG_H__21507D7B_A2E2_4FCC_BFB1_AEBFF7303A1B__INCLUDED_)
