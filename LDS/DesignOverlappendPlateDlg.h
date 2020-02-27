#if !defined(AFX_DESIGNOVERLAPPENDPLATEDLG_H__B8C51938_E837_4788_8974_37B46AAB8A58__INCLUDED_)
#define AFX_DESIGNOVERLAPPENDPLATEDLG_H__B8C51938_E837_4788_8974_37B46AAB8A58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignOverlappendPlateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignOverlappendPlateDlg dialog

class CDesignOverlappendPlateDlg : public CDialog
{
// Construction
public:
	CDesignOverlappendPlateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignOverlappendPlateDlg)
	enum { IDD = IDD_DESIGN_OVERLAPPEND_PLATE_DLG };
	double	m_fBaseOffsetX;
	double	m_fBaseOffsetY;
	double	m_fBaseOffsetZ;
	int		m_iMaterial;
	CString	m_sPartNo;
	CString m_sSegI;
	int		m_nThick;
	BOOL    m_bChooseWCS;
	BOOL    m_bChooseUCS;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignOverlappendPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignOverlappendPlateDlg)
	afx_msg void OnChangeEPartNo();
	virtual BOOL OnInitDialog();
	afx_msg void OnChkEnableWCS();
	afx_msg void OnChkEnableUCS();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNOVERLAPPENDPLATEDLG_H__B8C51938_E837_4788_8974_37B46AAB8A58__INCLUDED_)
