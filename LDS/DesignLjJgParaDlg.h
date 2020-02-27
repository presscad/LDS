#if !defined(AFX_DESIGNLJJGPARADLG_H__BC6D11B3_431C_4F4D_931C_8725BB0E2650__INCLUDED_)
#define AFX_DESIGNLJJGPARADLG_H__BC6D11B3_431C_4F4D_931C_8725BB0E2650__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignLjJgParaDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDesignLjJgParaDlg dialog

class CDesignLjJgParaDlg : public CDialog
{
// Construction
public:
	CDesignLjJgParaDlg(CWnd* pParent = NULL);   // standard constructor
	CDesignThickPara normoffset,datumnormoffset;
// Dialog Data
	//{{AFX_DATA(CDesignLjJgParaDlg)
	enum { IDD = IDD_DESIGN_LJJG_PARA_DLG };
	int		m_iCurLjEnd;
	int		m_iCurLjWing;
	int		m_iLjFace;
	double	m_fBerSpace;
	CString	m_hCollideRefJg1;
	CString	m_hCollideRefJg2;
	double	m_fEndSpace;
	BOOL	m_bTwoEdgeVertice;
	CString	m_hJg;
	CString	m_sLsStr;
	double	m_fWingSpace;
	long	m_nCollideDist;
	int		m_iOddCalStyle;
	int		m_iAffectTubeEndPlateStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignLjJgParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignLjJgParaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnEndNormOffset();
	afx_msg void OnBnEndDatumNormOffset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNLJJGPARADLG_H__BC6D11B3_431C_4F4D_931C_8725BB0E2650__INCLUDED_)
