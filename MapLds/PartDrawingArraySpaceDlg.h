#if !defined(AFX_PARTDRAWINGARRAYSPACEDLG_H__D041F76B_A8A8_4CD5_B798_B50C59F761B9__INCLUDED_)
#define AFX_PARTDRAWINGARRAYSPACE_HDLG__D041F76B_A8A8_4CD5_B798_B50C59F761B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartDrawingArraySpaceDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CPartDrawingArraySpaceDlg dialog

class CPartDrawingArraySpaceDlg : public CDialog
{
// Construction
public:
	int m_iSpaceMode;	//0.显示阵列单元格宽与高;1.显示阵列单元格横向及纵向间隙
	CPartDrawingArraySpaceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPartDrawingArraySpaceDlg)
	enum { IDD = IDD_PART_DRAWING_ARRAY_DLG };
	int		m_nRowHigh;
	int		m_nColumnWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartDrawingArraySpaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartDrawingArraySpaceDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTDRAWINGARRAYSPACEDLG_H__D041F76B_A8A8_4CD5_B798_B50C59F761B9__INCLUDED_)
