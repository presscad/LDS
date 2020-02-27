#if !defined(AFX_EXPORTRPTFILEOPTIONDLG_H__FC763DD2_476C_4042_8CA3_1DE96075863B__INCLUDED_)
#define AFX_EXPORTRPTFILEOPTIONDLG_H__FC763DD2_476C_4042_8CA3_1DE96075863B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportRptFileOptionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExportRptFileOptionDlg dialog

class CExportRptFileOptionDlg : public CDialog
{
// Construction
public:
	CExportRptFileOptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExportRptFileOptionDlg)
	enum { IDD = IDD_EXPORT_RPT_FILE_OPTION_DLG };
	int		m_iDocFileType;
	int		m_iFileLanguage;
	int		m_iEfficType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportRptFileOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportRptFileOptionDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTRPTFILEOPTIONDLG_H__FC763DD2_476C_4042_8CA3_1DE96075863B__INCLUDED_)
