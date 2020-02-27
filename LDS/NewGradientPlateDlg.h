#if !defined(AFX_NEWGRADIENTPLATEDLG_H__014C6A07_9BCF_46F2_B405_9F8CAFBB5E38__INCLUDED_)
#define AFX_NEWGRADIENTPLATEDLG_H__014C6A07_9BCF_46F2_B405_9F8CAFBB5E38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewGradientPlateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewGradientPlateDlg dialog

class CNewGradientPlateDlg : public CDialog
{
// Construction
public:
	CNewGradientPlateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewGradientPlateDlg)
	enum { IDD = IDD_NEW_GRADIENT_PLATE_DLG };
	int		m_iMaterial;
	double	m_fGradientAngle;
	CString	m_sPartNo;
	long	m_nPlankThick;
	double	m_fStartX;
	double	m_fStartY;
	double	m_fEndX;
	double	m_fEndY;
	double	m_fHigh;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewGradientPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewGradientPlateDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWGRADIENTPLATEDLG_H__014C6A07_9BCF_46F2_B405_9F8CAFBB5E38__INCLUDED_)
