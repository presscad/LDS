#if !defined(AFX_EXCELEXPORTOPTIONDLG_H__5A535368_294E_4B4A_9101_CE794E576A99__INCLUDED_)
#define AFX_EXCELEXPORTOPTIONDLG_H__5A535368_294E_4B4A_9101_CE794E576A99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExcelExportOptionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExcelExportOptionDlg dialog

class CExcelExportOptionDlg : public CDialog
{
// Construction
public:
	CExcelExportOptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExcelExportOptionDlg)
	enum { IDD = IDD_EXCEL_EXPORT_OPTION_DLG };
	BOOL	m_bDataInterface;
	BOOL	m_bCustomTbl;
	BOOL	m_bLsSumMatTbl;
	BOOL	m_bOrgMatTbl;
	BOOL	m_bPartTbl;
	int		m_iLsTblStyle;
	int		m_iWeldPartTblStyle;
	BOOL	m_bWeldPartTbl;
	int		m_iOrgmatTblStyle;
	int		m_iPartsSortType;
	BOOL	m_bPoleExcelFile;
	int		m_iPoleSortType;
	BOOL	m_bSelectAll;
	CString	m_sSegStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExcelExportOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExcelExportOptionDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChkSelectAll();
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXCELEXPORTOPTIONDLG_H__5A535368_294E_4B4A_9101_CE794E576A99__INCLUDED_)
