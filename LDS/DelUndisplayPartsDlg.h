#if !defined(AFX_DELUNDISPLAYPARTSDLG_H__449C964A_DE39_4B2E_8D51_3A1AB4FE5B6A__INCLUDED_)
#define AFX_DELUNDISPLAYPARTSDLG_H__449C964A_DE39_4B2E_8D51_3A1AB4FE5B6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DelUndisplayPartsDlg.h : header file
//
#include "ListViewComparer.h"
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CDelUndisplayPartsDlg dialog

class CDelUndisplayPartsDlg : public CDialog
{
// Construction
	CStringArray m_arrHeaderLabel;
	CListViewComparer listViewComparer;
public:
	CDelUndisplayPartsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDelUndisplayPartsDlg)
	enum { IDD = IDD_DEL_UNDISPLAY_PARTS_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelUndisplayPartsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDelUndisplayPartsDlg)
	afx_msg void OnClickListUndisplayParts(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListUndisplayParts(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListUndisplayParts(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListUndisplayParts(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl* m_pListCtrl;
	void SelectPart(int iCurSel);
	void DeleteItem(int iCurSel);
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELUNDISPLAYPARTSDLG_H__449C964A_DE39_4B2E_8D51_3A1AB4FE5B6A__INCLUDED_)
