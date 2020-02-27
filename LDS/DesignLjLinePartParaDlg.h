#if !defined(AFX_DESIGNLJLINEPARTPARADLG_H__CC28EDDC_CF96_4889_A853_92ADCC4C3B2B__INCLUDED_)
#define AFX_DESIGNLJLINEPARTPARADLG_H__CC28EDDC_CF96_4889_A853_92ADCC4C3B2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignLjLinePartParaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignLjLinePartParaDlg dialog

class CDesignLjLinePartParaDlg : public CDialog
{
// Construction
public:
	CDesignLjLinePartParaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignLjLinePartParaDlg)
	enum { IDD = IDD_DESIGN_LJ_LINEPART_PARA_DLG };
	int		m_iCurLjEnd;
	int		m_iCurLjWing;
	int		m_iLjFace;
	double	m_fBerSpace;
	double	m_fEndSpace;
	BOOL	m_bTwoEdgeVertice;
	CString	m_hLinePart;
	double	m_fWingSpace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignLjLinePartParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignLjLinePartParaDlg)
	afx_msg void OnChangeLinepartHandle();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNLJLINEPARTPARADLG_H__CC28EDDC_CF96_4889_A853_92ADCC4C3B2B__INCLUDED_)
