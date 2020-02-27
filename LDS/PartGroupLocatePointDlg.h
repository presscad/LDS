#if !defined(AFX_PARTGROUPLOCATEPOINTDLG_H__6484293A_649D_4DD8_A585_9DE68F991D1F__INCLUDED_)
#define AFX_PARTGROUPLOCATEPOINTDLG_H__6484293A_649D_4DD8_A585_9DE68F991D1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
// PartGroupLocatePointDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartGroupLocatePointDlg dialog

class CPartGroupLocatePointDlg : public CDialog
{
// Construction
public:
	CPartGroupLocatePointDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPartGroupLocatePointDlg)
	enum { IDD = IDD_PARTGROUP_LOCATE_POINT_DLG };
	CString	m_sDatumPart;
	double	m_fLocatePointX;
	double	m_fLocatePointY;
	double	m_fLocatePointZ;
	double	m_fStartOffset;
	double	m_fEndOffset;
	int		m_iYOrder;
	//}}AFX_DATA
	CListCtrl* m_pListCtrl;
	CStringArray m_arrHeaderLabel;
	CLDSPartGroup* m_pWorkPartGroup;
	CSectionView* m_pWorkView;
	BOOL m_bPauseBreakExit;	//是否为中途暂时中断选择构件性质的退出

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartGroupLocatePointDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartGroupLocatePointDlg)
	afx_msg void OnBtnSelectPart();
	afx_msg void OnBtnModifyLocatePoint();
	afx_msg void OnBtnAddLocatePoint();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClickListLocatePoint(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListLocatePoint(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListLocatePoint(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCopyPoint();
	afx_msg void OnPastePoint();
	afx_msg void OnKillfocusDatumPart();
	afx_msg void OnUpSide();
	afx_msg void OnDownSide();
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

#endif // !defined(AFX_PARTGROUPLOCATEPOINTDLG_H__6484293A_649D_4DD8_A585_9DE68F991D1F__INCLUDED_)
