#if !defined(AFX_FLDESIGNPARADLG_H__9090FBB8_7766_437B_8A5B_48E5B276491A__INCLUDED_)
#define AFX_FLDESIGNPARADLG_H__9090FBB8_7766_437B_8A5B_48E5B276491A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FLDesignParaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFLDesignParaDlg dialog

class CFLDesignParaDlg : public CDialog
{
// Construction
public:
	CFLDesignParaDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bCanFlCoupleDesign;

// Dialog Data
	//{{AFX_DATA(CFLDesignParaDlg)
	enum { IDD = IDD_FL_DESIGN_PARA_DLG };
	BOOL	m_bFlCoupleDesign;
	BOOL	m_bLayoutElbowPlate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFLDesignParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFLDesignParaDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLDESIGNPARADLG_H__9090FBB8_7766_437B_8A5B_48E5B276491A__INCLUDED_)
