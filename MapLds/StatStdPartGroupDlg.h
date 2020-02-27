#if !defined(AFX_STATSTDPARTGROUPDLG_H__B8E2197A_4B47_4944_A392_FED27CF91D72__INCLUDED_)
#define AFX_STATSTDPARTGROUPDLG_H__B8E2197A_4B47_4944_A392_FED27CF91D72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatStdPartGroupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatStdPartGroupDlg dialog
#include "resource.h"
#include "ListViewComparer.h"

class CStatStdPartGroupDlg : public CDialog
{
// Construction
CListViewComparer listViewComparer;
public:
	struct STDPART_ITEM{
		CLDSPart *pPart;
		int num;
	};
	ATOM_LIST<STDPART_ITEM> partList;
	CStatStdPartGroupDlg(CWnd* pParent = NULL);   // standard constructor
	CStringArray m_arrHeaderLabel;
	CListCtrl *m_pListCtrl;
	STDPART_ITEM *m_pCurSelItem;
// Dialog Data
	//{{AFX_DATA(CStatStdPartGroupDlg)
	enum { IDD = IDD_STAT_STD_PARTGROUP_DLG };
	CString	m_sSegStr;
	int		m_iStartPartType;
	int		m_iEndPartType;
	BOOL	m_bStatAllType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatStdPartGroupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStatStdPartGroupDlg)
	afx_msg void OnRebuildPartset();
	virtual BOOL OnInitDialog();
	afx_msg void OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnSelChangeStartPartType();
	afx_msg void OnSelChangeEndPartType();
	afx_msg void OnStatAllType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void RefreshListView();
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATSTDPARTGROUPDLG_H__B8E2197A_4B47_4944_A392_FED27CF91D72__INCLUDED_)
