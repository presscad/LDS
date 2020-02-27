#if !defined(AFX_MODIFYRELATIVEOBJPARTNODLG_H__F4F9B8E8_29C2_4D2D_B1F8_18DC444AF8F9__INCLUDED_)
#define AFX_MODIFYRELATIVEOBJPARTNODLG_H__F4F9B8E8_29C2_4D2D_B1F8_18DC444AF8F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyRelativeObjPartNoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModifyRelativeObjPartNoDlg dialog
#include "XhListCtrl.h"
class CModifyRelativeObjPartNoDlg : public CDialog
{
// Construction
public:
	CModifyRelativeObjPartNoDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSPart *m_pWorkPart;
// Dialog Data
	//{{AFX_DATA(CModifyRelativeObjPartNoDlg)
	enum { IDD = IDD_MODIFY_RELATIVE_OBJ_PARTNO_DLG };
	CString	m_sCurObjPartNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyRelativeObjPartNoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModifyRelativeObjPartNoDlg)
	afx_msg void OnClickListRelativeObject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListRelativeObject(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnModifyRelativeObj();
	virtual void OnOK();
	afx_msg void OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeCurObjPartno();
	afx_msg void OnColumnClickPartNo(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CXhListCtrl m_listCtrl;
	CListViewComparer listViewComparer;
	void SelectPart(int iCurSel);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYRELATIVEOBJPARTNODLG_H__F4F9B8E8_29C2_4D2D_B1F8_18DC444AF8F9__INCLUDED_)
