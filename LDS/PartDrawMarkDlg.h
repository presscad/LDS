#if !defined(AFX_PARTDRAWMARKDLG_H__18ECD4F3_5DC0_4ABD_96BF_EB4B043E7E47__INCLUDED_)
#define AFX_PARTDRAWMARKDLG_H__18ECD4F3_5DC0_4ABD_96BF_EB4B043E7E47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartDrawMarkDlg.h : header file
//

#include "DefinedView.h"
/////////////////////////////////////////////////////////////////////////////
// CPartDrawMarkDlg dialog

class CPartDrawMarkDlg : public CDialog
{
// Construction
public:
	CViewPart *m_pViewPart;
	CString m_cmbItemStrList;
	CDisplayView *m_pRelaView;
	BOOL m_bOnlySetSpreadFace;	//仅设置绘图平面
	CPartDrawMarkDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CPartDrawMarkDlg)
	enum { IDD = IDD_PART_DRAW_MARK_DLG };
	int		m_iDrawMark;
	int		m_iResidePlane;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartDrawMarkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartDrawMarkDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTDRAWMARKDLG_H__18ECD4F3_5DC0_4ABD_96BF_EB4B043E7E47__INCLUDED_)
