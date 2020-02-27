#if !defined(AFX_NODEDESIGNPOSDLG_H__BFAB874D_BE5F_45FF_8DF3_EEF6AF1D22C6__INCLUDED_)
#define AFX_NODEDESIGNPOSDLG_H__BFAB874D_BE5F_45FF_8DF3_EEF6AF1D22C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NodeDesignPosDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNodeDesignPosDlg dialog

class CNodeDesignPosDlg : public CDialog
{
// Construction
public:
	JGZJ wing_x_jgzj,wing_y_jgzj;
	CNodeDesignPosDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNodeDesignPosDlg)
	enum { IDD = IDD_NODE_POS_DESIGN_DLG };
	double	m_fOffsetWingX;
	double	m_fOffsetWingY;
	int		m_iWingXOffsetStyle;
	int		m_iWingYOffsetStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNodeDesignPosDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNodeDesignPosDlg)
	afx_msg void OnRdoWingXOffset();
	afx_msg void OnRdoWingYOffset();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NODEDESIGNPOSDLG_H__BFAB874D_BE5F_45FF_8DF3_EEF6AF1D22C6__INCLUDED_)
