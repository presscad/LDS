#if !defined(AFX_SUPPPARTDLG_H__41395797_28BB_4243_A6FA_462EFBCE9744__INCLUDED_)
#define AFX_SUPPPARTDLG_H__41395797_28BB_4243_A6FA_462EFBCE9744__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SuppPartDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSuppPartDlg dialog

class CSuppPartDlg : public CDialog
{
// Construction
public:
	char m_cMaterial;
	CSuppPartDlg(CWnd* pParent = NULL);   // standard constructor
	CFGWORD cfgword;
	void UpdateCfgPartNoString();

// Dialog Data
	//{{AFX_DATA(CSuppPartDlg)
	enum { IDD = IDD_SUPP_PART_SPEC_DLG };
	CString	m_sNotes;
	CString	m_sPartGuige;
	double	m_fPartLength;
	CString	m_sPartNo;
	int		m_nPartNum;
	double	m_fPieceWeight;
	CString	m_sSegNo;
	int		m_iMaterial;
	int		m_iPartType;
	CString	m_sCfgPartNo;
	BOOL	m_bLegPart;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSuppPartDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSuppPartDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnCfgPart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUPPPARTDLG_H__41395797_28BB_4243_A6FA_462EFBCE9744__INCLUDED_)
