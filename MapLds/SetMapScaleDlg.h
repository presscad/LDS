#if !defined(AFX_SETMAPSCALEDLG_H__3AF18EB2_B913_429D_B779_3893B67A4EDF__INCLUDED_)
#define AFX_SETMAPSCALEDLG_H__3AF18EB2_B913_429D_B779_3893B67A4EDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetMapScaleDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CSetMapScaleDlg dialog

class CSetMapScaleDlg : public CDialog
{
// Construction
public:
	CSetMapScaleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetMapScaleDlg)
	enum { IDD = IDD_SET_MAP_SCALE_DLG };
	int		m_nLineMapScale;
	int		m_nPartMapScale;
	int		m_nStruMapScale;
	int		m_nForceLineScale;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetMapScaleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetMapScaleDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETMAPSCALEDLG_H__3AF18EB2_B913_429D_B779_3893B67A4EDF__INCLUDED_)
