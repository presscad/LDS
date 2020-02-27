#pragma once


// CWireLoadCalRunDlg
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
class CWireLoadCalRunDlg : public CDialog
{
	CWinThread *m_pThread;
public:
	static HANDLE ghWireLoadProcess;
	CWireLoadCalRunDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWireLoadCalRunDlg();
	void StartWireLoadCalProcess();	//÷¥––∫…‘ÿº∆À„
	int m_eProcessExecuteState;	//0:Running;1.OK;2.Cancel
	void WireLoadCalFinished(int finishMode=TRUE);
// Dialog Data
	//{{AFX_DATA(CWireLoadCalRunDlg)
	enum { IDD = IDD_WIRELOAD_RUNNING_DLG };
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWireLoadCalRunDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWireLoadCalRunDlg)
	afx_msg void OnBnOkCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

#endif
