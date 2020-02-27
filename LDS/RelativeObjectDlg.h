#include "afxcmn.h"
#if !defined(AFX_RELATIVEOBJECTDLG_H__689D3328_1F7D_437B_A48B_0ADE2A753B6E__INCLUDED_)
#define AFX_RELATIVEOBJECTDLG_H__689D3328_1F7D_437B_A48B_0ADE2A753B6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RelativeObjectDlg.h : header file
//

#include "XhListCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CRelativeObjectDlg dialog

class CRelativeObjectDlg : public CDialog
{
	CRect m_rcOriginal;	//窗口原始大小
// Construction
public:
	CRelativeObjectDlg(CWnd* pParent = NULL);   // standard constructor
	MEMORY_BLOCK mpcl;
	CLDSDbObject *m_pWorkObj;
	CStringArray m_arrHeaderLabel;
// Dialog Data
	//{{AFX_DATA(CRelativeObjectDlg)
	enum { IDD = IDD_RELATIVE_OBJECT_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRelativeObjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRelativeObjectDlg)
	afx_msg void OnClickListRelativeObject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListRelativeObject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListRelativeObject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedSynchronizeRelativeObj();
	afx_msg void OnSetSyncItem();
	afx_msg void OnRevokeSyncItem();
	afx_msg void OnSetSyncDefault();
	afx_msg void OnEliminateItem();
	afx_msg void OnCancelEliminateItem();
	afx_msg void OnBnChkSyncMirPropDetails();
	afx_msg void OnNMRClickListSyncPropDetails(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnNMRClickListRelativeObject(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CXhListCtrl m_listCtrl;
	CXhListCtrl m_listSyncDetails;
	BOOL m_bSyncMirPropDetails;
	void SelectPart(int iCurSel);
	void ModifyMirInfo(int iCurSel);
	void DeleteItem(int iCurSel);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RELATIVEOBJECTDLG_H__689D3328_1F7D_437B_A48B_0ADE2A753B6E__INCLUDED_)
