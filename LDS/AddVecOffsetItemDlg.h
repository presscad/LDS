#if !defined(AFX_ADDVECOFFSETITEM_H__A6DCD2CF_57A2_4FA5_9E30_FACE0CC7CF0D__INCLUDED_)
#define AFX_ADDVECOFFSETITEM_H__A6DCD2CF_57A2_4FA5_9E30_FACE0CC7CF0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddVecOffsetItem.h : header file
//
#include "ToolKitDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CAddVecOffsetItemDlg dialog

class CAddVecOffsetItemDlg : public CDialog
{
// Construction
public:
	CAddVecOffsetItemDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bModifyItem,m_bDelItem;
// Dialog Data
	//{{AFX_DATA(CAddVecOffsetItemDlg)
	enum { IDD = IDD_ADD_VEC_OFFSET_ITME };
	double	m_fVecX;
	double	m_fVecY;
	double	m_fVecZ;
	double	m_fOffsetDist;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddVecOffsetItemDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddVecOffsetItemDlg)
	afx_msg void OnCopy();
	afx_msg void OnPast();
	afx_msg void OnReverse();
	afx_msg void OnDel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDVECOFFSETITEM_H__A6DCD2CF_57A2_4FA5_9E30_FACE0CC7CF0D__INCLUDED_)
