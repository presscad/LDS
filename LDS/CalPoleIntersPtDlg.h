#if !defined(AFX_CALPOLEINTERSPTDLG_H__E0E46058_9874_4B78_96D4_C07DBC31CF29__INCLUDED_)
#define AFX_CALPOLEINTERSPTDLG_H__E0E46058_9874_4B78_96D4_C07DBC31CF29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalPoleIntersPtDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CCalPoleIntersPtDlg dialog

class CCalPoleIntersPtDlg : public CDialog
{
// Construction
public:
	CCalPoleIntersPtDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSLinePart *m_pBasePole,*m_pOtherPole;
	JGZJ BaseZhunWingX,BaseZhunWingY,OtherZhunWingX,OtherZhunWingY;

// Dialog Data
	//{{AFX_DATA(CCalPoleIntersPtDlg)
	enum { IDD = IDD_CAL_POLE_INTERS_PT_DLG };
	int		m_iBaseWingOffsetStyle;
	int		m_iCurBaseWing;
	int		m_iCurOtherWing;
	int		m_iOtherWingOffsetStyle;
	double	m_fBaseWingOffsetDist;
	double	m_fOtherWingOffsetDist;
	CString	m_sBasePoleInfo;
	CString	m_sOtherPoleInfo;
	double	m_fIntersX;
	double	m_fIntersY;
	double	m_fIntersZ;
	CString	m_sCalResult;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalPoleIntersPtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCalPoleIntersPtDlg)
	afx_msg void OnBnCalInters();
	afx_msg void OnBnCopyIntersPt();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCmbCurBaseWing();
	afx_msg void OnSelchangeCmbCurOtherWing();
	afx_msg void OnSelchangeCmbBaseWingOffsetStyle();
	afx_msg void OnSelchangeCmbOtherWingOffsetStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALPOLEINTERSPTDLG_H__E0E46058_9874_4B78_96D4_C07DBC31CF29__INCLUDED_)
