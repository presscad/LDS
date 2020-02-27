#if !defined(AFX_DESIGNLJPARADLG_H__1FE52398_AF3C_4E9D_9FDE_09C00FA5F2B2__INCLUDED_)
#define AFX_DESIGNLJPARADLG_H__1FE52398_AF3C_4E9D_9FDE_09C00FA5F2B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignLjParaDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDesignLjParaDlg dialog

#include "XhListCtrl.h"
class CDesignLjParaDlg : public CDialog
{
// Construction
	CDesignLjPara m_xOldLjPara;
	CXhListCtrl	m_xListVertexes;
	void DesignPoint(CLDSPoint &point,char *sDescription);
	void SelectPart(long hCurPart);
	BOOL IsModifyDesignPara();
public:
	CDesignLjParaDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSPlate *m_pPlate;

// Dialog Data
	//{{AFX_DATA(CDesignLjParaDlg)
	enum { IDD = IDD_DESIGN_LJ_PARA_DLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignLjParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignLjParaDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListVertex(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBnMoveUp();
	afx_msg void OnBnClickedBnMoveDwon();
	afx_msg void OnBnClickedBnEnable();
	afx_msg void OnBnClickedBnDisable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNLJPARADLG_H__1FE52398_AF3C_4E9D_9FDE_09C00FA5F2B2__INCLUDED_)
