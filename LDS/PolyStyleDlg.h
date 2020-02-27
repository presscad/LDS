#if !defined(AFX_POLYSTYLEDLG_H__95F9C59F_BCBA_4F34_A099_19C0E345A8CA__INCLUDED_)
#define AFX_POLYSTYLEDLG_H__95F9C59F_BCBA_4F34_A099_19C0E345A8CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PolyStyleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPolyStyleDlg dialog

class CPolyStyleDlg : public CDialog
{
// Construction
public:
	CPolyStyleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPolyStyleDlg)
	enum { IDD = IDD_CONFIRM_POLY_STYLE_DLG };
	int		m_iPolyStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPolyStyleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPolyStyleDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POLYSTYLEDLG_H__95F9C59F_BCBA_4F34_A099_19C0E345A8CA__INCLUDED_)
