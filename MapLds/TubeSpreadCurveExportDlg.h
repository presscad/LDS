#if !defined(AFX_TUBESPREADCURVEEXPORTDLG_H__59C91B15_71C4_490E_8E95_576A7FB63C0F__INCLUDED_)
#define AFX_TUBESPREADCURVEEXPORTDLG_H__59C91B15_71C4_490E_8E95_576A7FB63C0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TubeSpreadCurveExportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTubeSpreadCurveExportDlg dialog

class CTubeSpreadCurveExportDlg : public CDialog
{
// Construction
public:
	CTubeSpreadCurveExportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTubeSpreadCurveExportDlg)
	enum { IDD = IDD_TUBE_SPREAD_CURVE_EXPORT_DLG };
	BOOL	m_bInnerSpreadCurve;
	BOOL	m_bOutterSpreadCurve;
	BOOL	m_bInnerOutterComposeCurve;
	BOOL	m_bProcessSpreadCurve;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTubeSpreadCurveExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTubeSpreadCurveExportDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TUBESPREADCURVEEXPORTDLG_H__59C91B15_71C4_490E_8E95_576A7FB63C0F__INCLUDED_)
