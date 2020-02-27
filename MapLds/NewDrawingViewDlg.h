#if !defined(AFX_NEWDRAWINGVIEWDLG_H__742A745F_1EB7_4CA8_8C4A_56707C5886CF__INCLUDED_)
#define AFX_NEWDRAWINGVIEWDLG_H__742A745F_1EB7_4CA8_8C4A_56707C5886CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewDrawingViewDlg.h : header file
//
#include "f_ent_list.h"
#include "Tower.h"

#ifdef __SMART_DRAW_INC_
// CNewDrawingViewDlg dialog
#include "XhListBox.h"
class CNewDrawingViewDlg : public CDialog
{
// Construction
public:
	CNewDrawingViewDlg(CWnd* pParent = NULL);   // standard constructor
	int m_iSeg;
	void RefreshList(SEGI iSeg);

	typedef struct tagDRAWVIEW
	{
		int iType;		//0.图纸列表中已存在的视图 1.表示新加视图
		DWORD dwDate;
	}DRAW_VIEW;
	ATOM_LIST<DRAW_VIEW>m_drawViewList;
// Dialog Data
	//{{AFX_DATA(CNewDrawingViewDlg)
	enum { IDD = IDD_NEW_DRAWINGVIEW_DLG };
	CXhListBox	m_listRight;
	CXhListBox	m_listLeft;
	CComboBox	m_cmbSeg;
	int	m_iType;
	CString	m_sName;
	CString	m_sNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewDrawingViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewDrawingViewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnDelView();
	afx_msg void OnBnAddView();
	afx_msg void OnDblclkListRemainView();
	afx_msg void OnDblclkListSelectView();
	afx_msg void OnSelchangeCmbSeg();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWDRAWINGVIEWDLG_H__742A745F_1EB7_4CA8_8C4A_56707C5886CF__INCLUDED_)
