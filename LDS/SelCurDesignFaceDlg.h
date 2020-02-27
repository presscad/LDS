#if !defined(AFX_SELCURDESIGNFACEDLG_H__D04C0F47_8959_420F_9292_8741BAB01808__INCLUDED_)
#define AFX_SELCURDESIGNFACEDLG_H__D04C0F47_8959_420F_9292_8741BAB01808__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelCurDesignFaceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelCurDesignFaceDlg dialog

class CSelCurDesignFaceDlg : public CDialog
{
// Construction
public:
	CSelCurDesignFaceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelCurDesignFaceDlg)
	enum { IDD = IDD_SEL_CUR_FACE_DLG };
	int		m_iCurFace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelCurDesignFaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelCurDesignFaceDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELCURDESIGNFACEDLG_H__D04C0F47_8959_420F_9292_8741BAB01808__INCLUDED_)
