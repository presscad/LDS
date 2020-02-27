#if !defined(AFX_MOVEPARTNODLG_H__513372B0_C78B_49ED_BA7D_3E87B35965A0__INCLUDED_)
#define AFX_MOVEPARTNODLG_H__513372B0_C78B_49ED_BA7D_3E87B35965A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MovePartNoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMovePartNoDlg dialog

#include "CallBackDialog.h"

class CMovePartNoDlg : public CCallBackDialog
{
// Construction
	CLDSPart *m_pPickPart;
	void InitPartByPartLable();
public:
	CMovePartNoDlg(CWnd* pParent = NULL);   // standard constructor
	CString m_sPartNo;
	THANDLE m_hPart;
	int GetMoveType() { return m_iMoveType; }
protected:
// Dialog Data
	//{{AFX_DATA(CMovePartNoDlg)
	enum { IDD = IDD_MOVE_PARTNO_DLG };
	CString	m_sPartLable;
	int		m_iMoveType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMovePartNoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMovePartNoDlg)
	afx_msg void OnMoveType();
	afx_msg void OnSelectPart();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOVEPARTNODLG_H__513372B0_C78B_49ED_BA7D_3E87B35965A0__INCLUDED_)
