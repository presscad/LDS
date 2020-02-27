#if !defined(AFX_PARTUCSDLG_H__29FE68C5_B903_4C19_B7EE_FFD7EBABFDF3__INCLUDED_)
#define AFX_PARTUCSDLG_H__29FE68C5_B903_4C19_B7EE_FFD7EBABFDF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartUcsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartUcsDlg dialog

class CPartUcsDlg : public CDialog
{
// Construction
public:
	CPartUcsDlg(CWnd* pParent = NULL);   // standard constructor
	UCS_STRU ucs;
// Dialog Data
	//{{AFX_DATA(CPartUcsDlg)
	enum { IDD = IDD_PART_UCS_DLG };
	double	m_fAxisX_X;
	double	m_fAxisX_Y;
	double	m_fAxisX_Z;
	double	m_fAxisY_X;
	double	m_fAxisY_Y;
	double	m_fAxisY_Z;
	double	m_fAxisZ_X;
	double	m_fAxisZ_Y;
	double	m_fAxisZ_Z;
	double	m_fOriginPosX;
	double	m_fOriginPosY;
	double	m_fOriginPosZ;
	BOOL	m_bCanModifyAxisX;
	BOOL	m_bCanModifyAxisY;
	BOOL	m_bCanModifyAxisZ;
	//}}AFX_DATA
	CString	m_sCaption;
	BOOL	m_bCanModifyOrigin;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartUcsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPartUcsDlg)
	afx_msg void OnBnPasteAxisX();
	afx_msg void OnBnPasteAxisY();
	afx_msg void OnBnPasteAxisZ();
	afx_msg void OnBnPasteOriginPos();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnCopyAxisX();
	afx_msg void OnBnCopyAxisY();
	afx_msg void OnBnCopyAxisZ();
	afx_msg void OnBnCopyOriginPos();
	afx_msg void OnBnCpyUcs();
	afx_msg void OnBnPasteUcs();
	afx_msg void OnBnToRightHand();
	afx_msg void OnChkEnableAxisX();
	afx_msg void OnChkEnableAxisY();
	afx_msg void OnChkEnableAxisZ();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateUcs(BOOL bFlag=TRUE);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTUCSDLG_H__29FE68C5_B903_4C19_B7EE_FFD7EBABFDF3__INCLUDED_)
