#if !defined(AFX_LINEPARTEXTENDVECDLG_H__85B403D7_0C33_4E3D_8702_4D7FBDF46B56__INCLUDED_)
#define AFX_LINEPARTEXTENDVECDLG_H__85B403D7_0C33_4E3D_8702_4D7FBDF46B56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinePartExtendVecDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLinePartExtendVecDlg dialog

class CLinePartExtendVecDlg : public CDialog
{
	void CalLinePartExtendVec();
// Construction
public:
	CLinePartExtendVecDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSLinePart *m_pLinePart;
// Dialog Data
	//{{AFX_DATA(CLinePartExtendVecDlg)
	enum { IDD = IDD_LINEPART_EXTEND_VEC_DLG };
	double	m_fVecX;
	double	m_fVecY;
	double	m_fVecZ;
	BOOL	m_bLinePartSolidVec;
	BOOL	m_bLinePartLineVec;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinePartExtendVecDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinePartExtendVecDlg)
	afx_msg void OnLinePartSolidVec();
	afx_msg void OnLinePartLineVec();
	afx_msg void OnReverseVec();
	afx_msg void OnCopyVec();
	afx_msg void OnPasteVec();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINEPARTEXTENDVECDLG_H__85B403D7_0C33_4E3D_8702_4D7FBDF46B56__INCLUDED_)
