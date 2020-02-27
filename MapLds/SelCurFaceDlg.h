#if !defined(AFX_SELCURFACEDLG_H__489455B4_3EA3_4E20_A534_04BD37873EFC__INCLUDED_)
#define AFX_SELCURFACEDLG_H__489455B4_3EA3_4E20_A534_04BD37873EFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelCurFaceDlg.h : header file
//
#include "resource.h"
#include "Tower.h"

/////////////////////////////////////////////////////////////////////////////
// CSelCurFaceDlg dialog

struct TREEITEMDISPLAYVIEW_INFO{
	TREEITEMDISPLAYVIEW_INFO(){;}
	TREEITEMDISPLAYVIEW_INFO(long type,CDisplayView *ptr){itemType=type;pView=ptr;}
	long itemType;
	CDisplayView* pView;
};
class CSelCurFaceDlg : public CDialog
{
// Construction
public:
	CDisplayView *m_pSelectedView;
	CSelCurFaceDlg(CWnd* pParent = NULL);   // standard constructor
	CImageList m_ModelImages;
	ATOM_LIST<TREEITEMDISPLAYVIEW_INFO>itemInfoList;
	ATOM_LIST<SEGI> m_xSegNoList;
// Dialog Data
	//{{AFX_DATA(CSelCurFaceDlg)
	enum { IDD = IDD_SEL_CUR_FACE_DLG };
	CString	m_sSegNo;
	//}}AFX_DATA

public:
	void RefreshViewGroup(HTREEITEM hViewGroupItem,IModel* pModel);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelCurFaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelCurFaceDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkTreeDisplayView(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTreeDisplayView(NMHDR *pNMHDR, LRESULT *pResult);
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELCURFACEDLG_H__489455B4_3EA3_4E20_A534_04BD37873EFC__INCLUDED_)
