#if !defined(AFX_EXPORTEXCELOPTIONDLG_H__0D9D77A7_6D72_4D9C_BAB5_8DC51DF25542__INCLUDED_)
#define AFX_EXPORTEXCELOPTIONDLG_H__0D9D77A7_6D72_4D9C_BAB5_8DC51DF25542__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportExcelOptionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExportExcelOptionDlg dialog

class CExportExcelOptionDlg : public CDialog
{
// Construction
public:
	CExportExcelOptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExportExcelOptionDlg)
	enum { IDD = IDD_EXPORT_EXCEL_OPTION_DLG };
	BOOL	m_bBoltSumMatTbl;
	BOOL	m_bLineAngleSumMatTbl;
	BOOL	m_bLineFlatSumMatTbl;
	BOOL	m_bLineSlotSumMatTbl;
	BOOL	m_bLineTubeSumMatTbl;
	BOOL	m_bPlateSumMatTbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportExcelOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportExcelOptionDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTEXCELOPTIONDLG_H__0D9D77A7_6D72_4D9C_BAB5_8DC51DF25542__INCLUDED_)
