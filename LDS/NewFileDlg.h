#if !defined(AFX_NEWFILEDLG_H__275F1E01_5AB4_4DE1_9A12_F2D160B1FA53__INCLUDED_)
#define AFX_NEWFILEDLG_H__275F1E01_5AB4_4DE1_9A12_F2D160B1FA53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewFileDlgTest.h : header file
//
#include "ShankDefCls.h"
/////////////////////////////////////////////////////////////////////////////
// CNewFileDlg dialog

class CNewFileDlg : public CDialog
{
// Construction
	int		m_iOldUnit;	//记录前一次使用的单位索引
public:
	CString m_sCaption;
	CFrameDefine m_xFrameDef;
	CStringArray m_arrHeaderLabel;
	CNewFileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewFileDlg)
	enum { IDD = IDD_NEW_FILE_DLG };
	double	m_fFrontOpen;
	double	m_fSideOpen;
	double	m_fVerticalDist;
	int		m_iMainPartType;
	int		m_iSideNum;
	int		m_iUnit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewFileDlg)
	afx_msg void OnBnAddItem();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeTowerSideNum();
	afx_msg void OnSelchangeUnit();
	afx_msg void OnChangeEditFrontOpening();
	afx_msg void OnBtnSwitchToStandardUi();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl* m_pListCtrl;
	void ModifyItem(int iCurSel);
	void DeleteItem(int iCurSel);
	void RefreshListViewHeader();
	void AppendRecord(int iItem, CStringArray &str_arr);
	void UpdateFrameDef();
	void RefreshListCtrl();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWFILEDLGTEST_H__275F1E01_5AB4_4DE1_9A12_F2D160B1FA53__INCLUDED_)
