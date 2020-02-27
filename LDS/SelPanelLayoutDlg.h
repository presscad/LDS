#if !defined(AFX_SELPANELLAYOUTDLG_H__16048B3F_4C84_4D8E_AD23_455203325279__INCLUDED_)
#define AFX_SELPANELLAYOUTDLG_H__16048B3F_4C84_4D8E_AD23_455203325279__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelPanelLayoutDlg.h : header file
//
#include "BitmapGraph.h"
#include "StdFacePanel.h"

/////////////////////////////////////////////////////////////////////////////
// CSelPanelLayoutDlg dialog

class CSelPanelLayoutDlg : public CDialog
{
	BOOL m_bFirstPopup;
	UINT m_uIconWidth,m_uIconHeight;	//当前显示模板示意图像的大小
	UINT m_dwIconWidth,m_dwIconHeight;
	CListCtrl* m_pListCtrl;
	CImageList imageList;
	CBitmapGraph bitmap;
// Construction
public:
	int m_iPanelType;	//0.头部标准模板;1.塔身标准面;2.横隔面标准面(即孤立面);3.节间面;4.K面(接腿正侧面);5.V面
	CString m_sFacePanel;
	CSelPanelLayoutDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelPanelLayoutDlg)
	enum { IDD = IDD_SEL_PANEL_LAYOUT_DLG };
	BOOL	m_bShankPanel;
	BOOL	m_bKPanel;
	BOOL	m_bLFrontPanel;	//接腿正侧面
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelPanelLayoutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelPanelLayoutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkShankPanel();
	afx_msg void OnChkKfacePanel();
	virtual void OnOK();
	afx_msg void OnDblclkListCtrlStdFace(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void AddFacePanel(CFacePanel &panel);
	void UpdateFaceSet();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELPANELLAYOUTDLG_H__16048B3F_4C84_4D8E_AD23_455203325279__INCLUDED_)
