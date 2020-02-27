#if !defined(AFX_LOADGRAPHDLG_H__6AF25C3B_D1B1_4F37_8F7E_A4C3163CCD03__INCLUDED_)
#define AFX_LOADGRAPHDLG_H__6AF25C3B_D1B1_4F37_8F7E_A4C3163CCD03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadGraphDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoadGraphDlg dialog

class CLoadGraphDlg : public CDialog
{
// Construction
public:
	CLoadGraphDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoadGraphDlg)
	enum { IDD = IDD_LOAD_GRAPH_DLG };
	int		m_nRowsOrColumns;
	int		m_nRowGap;
	int		m_nColumnGap;
	int		m_iRowFirst;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadGraphDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoadGraphDlg)
	afx_msg void OnRdoRowFirst();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADGRAPHDLG_H__6AF25C3B_D1B1_4F37_8F7E_A4C3163CCD03__INCLUDED_)
