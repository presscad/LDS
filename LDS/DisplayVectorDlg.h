#if !defined(AFX_DISPLAYVECTORDLG_H__94339A05_29B5_4994_953A_39A1BDE53A0F__INCLUDED_)
#define AFX_DISPLAYVECTORDLG_H__94339A05_29B5_4994_953A_39A1BDE53A0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DisplayVectorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDisplayVectorDlg dialog

class CDisplayVectorDlg : public CDialog
{
// Construction
public:
	CDisplayVectorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDisplayVectorDlg)
	enum { IDD = IDD_DISPLAY_VECTOR_DLG };
	double	m_fVectorX;
	double	m_fVectorY;
	double	m_fVectorZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayVectorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDisplayVectorDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYVECTORDLG_H__94339A05_29B5_4994_953A_39A1BDE53A0F__INCLUDED_)
