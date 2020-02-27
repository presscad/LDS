#if !defined(AFX_PROCESSMSGPACKDLG_H__149E3274_850C_4149_808E_47C6A0A218EF__INCLUDED_)
#define AFX_PROCESSMSGPACKDLG_H__149E3274_850C_4149_808E_47C6A0A218EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcessMsgPackDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProcessMsgPackDlg dialog

class CProcessMsgPackDlg : public CDialog
{
// Construction
public:
	CProcessMsgPackDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProcessMsgPackDlg)
	enum { IDD = IDD_PROCESS_PACK_DLG };
	CString	m_sReadMsg;
	CString	m_sUpdateDataFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessMsgPackDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcessMsgPackDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnUpdateDataFile();
	afx_msg void OnBnReadMsg();
	afx_msg void OnBnUpdatePerm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSMSGPACKDLG_H__149E3274_850C_4149_808E_47C6A0A218EF__INCLUDED_)
