#if !defined(AFX_PARTGROUPCHECKLINEDLG_H__61F5B320_525B_4E13_841B_BE789E679920__INCLUDED_)
#define AFX_PARTGROUPCHECKLINEDLG_H__61F5B320_525B_4E13_841B_BE789E679920__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
// PartGroupCheckLineDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartGroupCheckLineDlg dialog

class CPartGroupCheckLineDlg : public CDialog
{
// Construction
public:
	CPartGroupCheckLineDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPartGroupCheckLineDlg)
	enum { IDD = IDD_PARTGROUP_CHECK_LINE_DLG };
	double	m_fEndPointX;
	double	m_fEndPointY;
	double	m_fEndPointZ;
	CString	m_sStartPartHandle;
	CString	m_sEndPartHandle;
	double	m_fStartPointX;
	double	m_fStartPointY;
	double	m_fStartPointZ;
	BOOL	m_bBeforeWeld;
	CString	m_sPrompt;
	//}}AFX_DATA
	CListCtrl* m_pListCtrl;
	CStringArray m_arrHeaderLabel;
	CLDSPartGroup* m_pWorkPartGroup;
	CSectionView* m_pWorkView;
	BOOL m_bPauseBreakExit;	//是否为中途暂时中断选择构件性质的退出
	BOLTSET boltList;	//螺栓列表 支持添加焊接前检测线时的多选

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartGroupCheckLineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartGroupCheckLineDlg)
	afx_msg void OnPasteStartPoint();
	afx_msg void OnSelectPart();
	afx_msg void OnPasteEndPoint();
	afx_msg void OnAddCheckLine();
	afx_msg void OnCopyEndPoint();
	afx_msg void OnCopyStartPoint();
	afx_msg void OnModifyCheckLine();
	virtual void OnOK();
	afx_msg void OnClickListCheckLine(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListCheckLine(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListCheckLine(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnBeforeWeld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void ModifyItem(int iCurSel);
	void SelectPart(int nCurSel);
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
};

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTGROUPCHECKLINEDLG_H__61F5B320_525B_4E13_841B_BE789E679920__INCLUDED_)
