#if !defined(AFX_MODIFYJGLSDLG_H__DF662D05_0E3A_4458_940E_00658DB79582__INCLUDED_)
#define AFX_MODIFYJGLSDLG_H__DF662D05_0E3A_4458_940E_00658DB79582__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyJgLsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModifyJgLsDlg dialog

class CModifyJgLsDlg : public CDialog
{
// Construction
public:
	CModifyJgLsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModifyJgLsDlg)
	enum { IDD = IDD_MODIFY_JG_LS_POS_DLG };
	double	m_fAxisLen;
	double	m_fWingOffsetDist;
	int		m_iLsPosStyle;
	CString	m_sName1;
	CString	m_sName2;
	CString	m_hRayJg;
	CString m_sLsGuiGe;
	CString	m_sBoltNormOffset;
	int		m_nRayJgG;
	int		m_iRayDirection;
	int		m_nDatumJgG;
	//}}AFX_DATA
	f3dPoint face_norm_offset;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyJgLsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModifyJgLsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCmbLsPosStyle();
	afx_msg void OnOffsetFacePara();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYJGLSDLG_H__DF662D05_0E3A_4458_940E_00658DB79582__INCLUDED_)
