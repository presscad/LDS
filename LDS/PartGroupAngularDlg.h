#if !defined(AFX_PARTGROUPANGULARDLG_H__9171ADAD_4A9F_43B6_B3E3_C2BBE7EAE010__INCLUDED_)
#define AFX_PARTGROUPANGULARDLG_H__9171ADAD_4A9F_43B6_B3E3_C2BBE7EAE010__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

// PartGroupAngularDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartGroupAngularDlg dialog

class CPartGroupAngularDlg : public CDialog
{
// Construction
public:
	CPartGroupAngularDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPartGroupAngularDlg)
	enum { IDD = IDD_PARTGROUP_ANGULAR_DLG };
	BOOL	m_bStartLineIsWeldRoad;
	double	m_fDimR;
	CString	m_sEndPartHandle;
	CString	m_sStartPartHandle;
	//}}AFX_DATA
	CListCtrl* m_pListCtrl;
	CStringArray m_arrHeaderLabel;
	CLDSPartGroup* m_pWorkPartGroup;
	CSectionView* m_pWorkView;
	BOOL m_bPauseBreakExit;	//是否为中途暂时中断选择构件性质的退出
	BOOL m_bHasWeldRoad;	//是否有焊道线
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartGroupAngularDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartGroupAngularDlg)
	afx_msg void OnAddAngular();
	afx_msg void OnModifyAngular();
	afx_msg void OnSelectPart();
	afx_msg void OnClickListAngular(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListAngular(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListAngular(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusStartPart();
	afx_msg void OnKillfocusEndPart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ModifyItem(int iCurSel);
	void SelectPart(int nCurSel);
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
};

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTGROUPANGULARDLG_H__9171ADAD_4A9F_43B6_B3E3_C2BBE7EAE010__INCLUDED_)
