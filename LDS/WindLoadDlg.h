#if !defined(AFX_WINDLOADDLG_H__BED7F525_CE3F_4212_9EF5_8E3A384EAA91__INCLUDED_)
#define AFX_WINDLOADDLG_H__BED7F525_CE3F_4212_9EF5_8E3A384EAA91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WindLoadDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWindLoadDlg dialog

class CWindLoadDlg : public CDialog
{
// Construction
public:
	CWindLoadDlg(CWnd* pParent = NULL);   // standard constructor
	int m_nColumnsN;
	int m_nRowsN;

// Dialog Data
	//{{AFX_DATA(CWindLoadDlg)
	enum { IDD = IDD_WIND_LOAD_WORK_COND_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	CGridCtrl m_Grid;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindLoadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWindLoadDlg)
	afx_msg void OnBnExportGFile();
	afx_msg void OnBnImportGFile();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDLOADDLG_H__BED7F525_CE3F_4212_9EF5_8E3A384EAA91__INCLUDED_)
