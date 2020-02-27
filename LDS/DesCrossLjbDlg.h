#if !defined(AFX_DESCROSSLJBDLG_H__3BECFC26_D9B6_4349_B008_A7492FC0A4CF__INCLUDED_)
#define AFX_DESCROSSLJBDLG_H__3BECFC26_D9B6_4349_B008_A7492FC0A4CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesCrossLjbDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesCrossLjbDlg dialog

class CDesCrossLjbDlg : public CDialog
{
// Construction
public:
	CDesCrossLjbDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesCrossLjbDlg)
	enum { IDD = IDD_DES_CROSS_LJB_DLG };
	int		m_nThick;
	int		m_nVertexDist;
	CString m_sPartSegI;
	CString	m_sPartNo;
	int		m_iProfileType;
	int		m_iMaterial;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesCrossLjbDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesCrossLjbDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePartNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESCROSSLJBDLG_H__3BECFC26_D9B6_4349_B008_A7492FC0A4CF__INCLUDED_)
