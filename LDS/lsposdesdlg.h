#if !defined(AFX_LSPOSDESDLG_H__949F27E7_AB49_46ED_A43F_464B3E90D421__INCLUDED_)
#define AFX_LSPOSDESDLG_H__949F27E7_AB49_46ED_A43F_464B3E90D421__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LsPosDesDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CLsPosDesDlg dialog

class CLsPosDesDlg : public CDialog
{
// Construction
public:
	CLsPosDesDlg(CWnd* pParent = NULL);   // standard constructor
	CDesignThickPara norm_offset;

// Dialog Data
	//{{AFX_DATA(CLsPosDesDlg)
	enum { IDD = IDD_LS_POS_DES_DLG };
	int		m_iPosSpecStyle;
	CString	m_hJg1;
	CString	m_hJg2;
	CString	m_hNode;
	double	m_fLenDist;
	double	m_fWingDist;
	int		m_iJgOffsetVec;
	int		m_iOffsetWing;
	CString	m_sBaseName;
	double	m_fG1;
	double	m_fG2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLsPosDesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLsPosDesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCmbPosSpecStyle();
	virtual void OnOK();
	afx_msg void OnBnNormOffset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PTDESDLG_H__949F27E7_AB49_46ED_A43F_464B3E90D421__INCLUDED_)
