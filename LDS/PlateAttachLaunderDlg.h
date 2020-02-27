#if !defined(AFX_PLATEATTACHLAUNDERDLG_H__7031BBC1_DE46_4F29_AB5F_74BC6124DD61__INCLUDED_)
#define AFX_PLATEATTACHLAUNDERDLG_H__7031BBC1_DE46_4F29_AB5F_74BC6124DD61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlateAttachLaunderDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlateAttachLaunderDlg dialog

class CPlateAttachLaunderDlg : public CDialog
{
// Construction
public:
	CPlateAttachLaunderDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPlateAttachLaunderDlg)
	enum { IDD = IDD_PLATE_ATTACH_LAUNDER_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlateAttachLaunderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPlateAttachLaunderDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLATEATTACHLAUNDERDLG_H__7031BBC1_DE46_4F29_AB5F_74BC6124DD61__INCLUDED_)
