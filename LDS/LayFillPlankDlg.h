#if !defined(AFX_LAYFILLPLANKDLG_H__FD03D3AE_AF70_4914_A6A5_761ED7AADFB6__INCLUDED_)
#define AFX_LAYFILLPLANKDLG_H__FD03D3AE_AF70_4914_A6A5_761ED7AADFB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayFillPlankDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLayFillPlankDlg dialog

#ifndef __TSA_
class CLayFillPlankDlg : public CDialog
{
// Construction
public:
	CLDSGroupLineAngle *m_pGroupAngle;
	int m_iGroupStyle;
	CLayFillPlankDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLayFillPlankDlg)
	enum { IDD = IDD_LAY_FILL_PLANK_DLG };
	CString	m_sLsGuiGe;
	int		m_iLsRows;
	double	m_fEndOffset;
	int		m_nLsNum;
	int		m_nPlankNum;
	double	m_fStartOffset;
	int		m_iLsLayOutStyle;
	int		m_iWing;
	BOOL	m_bSwapWing;
	CString	m_sPartNo;
	int		m_iPlateMaterial;
	CString m_sSegI;
	int		m_nThick;
	BOOL	m_bDifferentType;
	int	m_iType;
	double	m_fDistance;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayFillPlankDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLayFillPlankDlg)
	afx_msg void OnSelchangeCmbLsRows();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEPartNo();
	afx_msg void OnSelchangeCmbType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYFILLPLANKDLG_H__FD03D3AE_AF70_4914_A6A5_761ED7AADFB6__INCLUDED_)
