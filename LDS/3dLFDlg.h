#if !defined(AFX_3DLFDLG_H__8F3C6A57_7DF0_413E_9453_E87AF8185777__INCLUDED_)
#define AFX_3DLFDLG_H__8F3C6A57_7DF0_413E_9453_E87AF8185777__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3dLFDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// C3dLFDlg dialog

class C3dLFDlg : public CDialog
{
// Construction
public:
	C3dLFDlg(CWnd* pWnd);   // standard constructor
	CWnd * m_pWnd;
	BOOL Create();

// Dialog Data
	//{{AFX_DATA(C3dLFDlg)
	enum { IDD = IDD_INT3D_LF_DLG };
	double	m_fBaseX;
	double	m_fBaseY;
	double	m_fBaseZ;
	double	m_fEndX;
	double	m_fEndY;
	double	m_fEndZ;
	double	m_fIntersX;
	double	m_fIntersY;
	double	m_fIntersZ;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	double	m_fStartX;
	double	m_fStartY;
	double	m_fStartZ;
	CString	m_sResultReport;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3dLFDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(C3dLFDlg)
	afx_msg void OnBnCalInters();
	afx_msg void OnBnCopyIntersPt();
	afx_msg void OnBnPasteBase();
	afx_msg void OnBnPasteEnd();
	afx_msg void OnBnPasteNorm();
	afx_msg void OnBnPasteStart();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnPastePlane();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DLFDLG_H__8F3C6A57_7DF0_413E_9453_E87AF8185777__INCLUDED_)
