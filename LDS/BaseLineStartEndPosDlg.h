#if !defined(AFX_BASELINESTARTENDPOSDLG_H__7D1C3985_1B97_4A96_9BCA_36C7D6CBE408__INCLUDED_)
#define AFX_BASELINESTARTENDPOSDLG_H__7D1C3985_1B97_4A96_9BCA_36C7D6CBE408__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseLineStartEndPosDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBaseLineStartEndPosDlg dialog

class CBaseLineStartEndPosDlg : public CDialog
{
// Construction
public:
	CBaseLineStartEndPosDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSPart *m_pWorkPart;
	CString m_sCaption;
// Dialog Data
	//{{AFX_DATA(CBaseLineStartEndPosDlg)
	enum { IDD = IDD_BASELINE_START_END_POS };
	double	m_fPointX;
	double	m_fPointY;
	double	m_fPointZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseLineStartEndPosDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBaseLineStartEndPosDlg)
	afx_msg void OnNewPoint();
	afx_msg void OnCopyPoint();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASELINESTARTENDPOSDLG_H__7D1C3985_1B97_4A96_9BCA_36C7D6CBE408__INCLUDED_)
