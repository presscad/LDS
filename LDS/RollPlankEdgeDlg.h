#if !defined(AFX_ROLLPLANKEDGEDLG_H__2B43A84C_27C2_466F_8A7B_1DF2BF2F0502__INCLUDED_)
#define AFX_ROLLPLANKEDGEDLG_H__2B43A84C_27C2_466F_8A7B_1DF2BF2F0502__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RollPlankEdgeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRollPlankEdgeDlg dialog

class CRollPlankEdgeDlg : public CDialog
{
// Construction
public:
	CRollPlankEdgeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRollPlankEdgeDlg)
	enum { IDD = IDD_ROLL_PLANK_EDGE_DLG };
	double	m_fRollLineHeight;
	double	m_fRollLineOffsetDist;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRollPlankEdgeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRollPlankEdgeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROLLPLANKEDGEDLG_H__2B43A84C_27C2_466F_8A7B_1DF2BF2F0502__INCLUDED_)
