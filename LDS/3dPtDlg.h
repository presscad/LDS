#if !defined(AFX_3DPTDLG_H__0ABCDC8D_566D_4D66_A918_B798BD387DF6__INCLUDED_)
#define AFX_3DPTDLG_H__0ABCDC8D_566D_4D66_A918_B798BD387DF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3dPtDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// C3dPtDlg dialog

class C3dPtDlg : public CDialog
{
// Construction
public:
	BOOL m_bCanModify;
	C3dPtDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(C3dPtDlg)
	enum { IDD = IDD_3D_VERTEX_DLG };
	double	m_fPtX;
	double	m_fPtY;
	double	m_fPtZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3dPtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(C3dPtDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DPTDLG_H__0ABCDC8D_566D_4D66_A918_B798BD387DF6__INCLUDED_)
