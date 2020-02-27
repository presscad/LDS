#if !defined(AFX_DESIGNSPHEREDLG_H__CEB5CD37_9F5E_4258_AD18_326197EB1D11__INCLUDED_)
#define AFX_DESIGNSPHEREDLG_H__CEB5CD37_9F5E_4258_AD18_326197EB1D11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignSphereDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignSphereDlg dialog

class CDesignSphereDlg : public CDialog
{
// Construction
public:
	CDesignSphereDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignSphereDlg)
	enum { IDD = IDD_DESIGN_SPHERE_DLG };
	CString	m_sSphereGuige;
	int		m_iSphereMat;
	CString	m_sPartNo;
	CString m_sSegNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignSphereDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignSphereDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePartNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNSPHEREDLG_H__CEB5CD37_9F5E_4258_AD18_326197EB1D11__INCLUDED_)
