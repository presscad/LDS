#if !defined(AFX_BENDPLANKSTYLEDLG_H__9A8804D7_F22E_434E_B7BD_590AABD1752C__INCLUDED_)
#define AFX_BENDPLANKSTYLEDLG_H__9A8804D7_F22E_434E_B7BD_590AABD1752C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BendPlankStyleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBendPlankStyleDlg dialog

class CBendPlankStyleDlg : public CDialog
{
// Construction
public:
	CBendPlankStyleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBendPlankStyleDlg)
	enum { IDD = IDD_BEND_PLANK_STYLE_DLG };
	double	m_fBendAngle;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	int		m_iBendFaceStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBendPlankStyleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBendPlankStyleDlg)
	afx_msg void OnBnPasteNorm();
	virtual BOOL OnInitDialog();
	afx_msg void OnRdoBendFaceStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BENDPLANKSTYLEDLG_H__9A8804D7_F22E_434E_B7BD_590AABD1752C__INCLUDED_)
