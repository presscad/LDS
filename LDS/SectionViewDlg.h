#if !defined(AFX_SECTIONVIEWDLG_H__C3269121_5711_46FC_8C3F_547D389A3153__INCLUDED_)
#define AFX_SectionViewDlg_H__C3269121_5711_46FC_8C3F_547D389A3153__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
// SectionViewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSectionViewDlg dialog

class CSectionViewDlg : public CDialog
{
// Construction
public:
	CLDSPartGroup *m_pWorkPartGroup;
	CSectionView *m_pWorkView, *m_pSrcView;
	long m_nPartNum;
	CSectionViewDlg(CWnd* pParent = NULL);   // standard constructor
	UCS_STRU ucs;

// Dialog Data
	//{{AFX_DATA(CSectionViewDlg)
	enum { IDD = IDD_DEF_SECTION_VIEW_DLG };
	CString	m_sSectionViewName;
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
	BOOL	m_bLockedView;
	CString	m_sPartNum;
	//}}AFX_DATA
	CListCtrl* m_pListCtrl;
	CStringArray m_arrHeaderLabel;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSectionViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSectionViewDlg)
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
	afx_msg void OnClickListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnDrawBySelPartUcs();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateUcs(BOOL bFlag=TRUE);
	void SelectPart(int iCurSel);
	void RefreshListViewHeader();
	void ModifyItem(int iCurSel);
	int AppendRecord(int iItem, CStringArray &str_arr);
};

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECTIONVIEWDLG_H__C3269121_5711_46FC_8C3F_547D389A3153__INCLUDED_)
