#if !defined(AFX_NEWPARALLELPLATEDLG_H__1CBF076A_5E6D_48EA_BDFC_8B098B185528__INCLUDED_)
#define AFX_NEWPARALLELPLATEDLG_H__1CBF076A_5E6D_48EA_BDFC_8B098B185528__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewParallelPlateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewParallelPlateDlg dialog

class CNewParallelPlateDlg : public CDialog
{
// Construction
public:
	CNewParallelPlateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewParallelPlateDlg)
	enum { IDD = IDD_NEW_PARALLEL_PLATE_DLG };
	int		m_iMaterial;
	long	m_nPlankThick;
	double	m_fVertOffsetDist;
	CString	m_sPartNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewParallelPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewParallelPlateDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWPARALLELPLATEDLG_H__1CBF076A_5E6D_48EA_BDFC_8B098B185528__INCLUDED_)
