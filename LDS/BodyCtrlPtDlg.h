#ifndef __BODY_CTRL_PT_DLG_H_
#define __BODY_CTRL_PT_DLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"       // main symbols
/////////////////////////////////////////////////////////////////////////////
// CShankCtrlPtDlg dialog

class CShankCtrlPtDlg : public CDialog
{
// Construction
public:
	CString m_sCaption;
	int OpenListNum;
	//0:垂高 1:正面开口 2:侧面开口
	double OpenList[100][3];
	CStringArray m_arrHeaderLabel;
	CShankCtrlPtDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShankCtrlPtDlg)
	enum { IDD = IDD_DEF_SHANK_JG_DLG };
	double	m_fFrontOpen;
	double	m_fSideOpen;
	double	m_fVerticalDist;
	int		m_iMainPartType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShankCtrlPtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShankCtrlPtDlg)
	afx_msg void OnBnAddItem();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl* m_pListCtrl;
	void ModifyItem(int iCurSel);
	void DeleteItem(int iCurSel);
	void RefreshListViewHeader();
	void AppendRecord(int iItem, CStringArray &str_arr);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
