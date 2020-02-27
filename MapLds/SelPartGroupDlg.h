#if !defined(AFX_SELPARTGROUPDLG_H__9B354CBC_E46F_4D04_A2C3_DC3D4F1EBA18__INCLUDED_)
#define AFX_SELPARTGROUPDLG_H__9B354CBC_E46F_4D04_A2C3_DC3D4F1EBA18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelPartGroupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelPartGroupDlg dialog
#include "resource.h"
#include "Tower.h"
#ifndef __TSA_FILE_

struct TREEITEMPARTGROUP_INFO{
	TREEITEMPARTGROUP_INFO(){;}
	TREEITEMPARTGROUP_INFO(long type,void *ptr){itemType=type;pObj=ptr;}
	long itemType;	//0:组焊件夹1:组合焊件;2组合焊件视图
	void* pObj;
};
class CSelPartGroupDlg : public CDialog
{
	ATOM_LIST<TREEITEMPARTGROUP_INFO>itemInfoList;
// Construction
public:
	BOOL m_bPartGroupBOM;
	CSectionView* m_pSelSectionView;
	CString m_sFolderName;
	long m_hSelPartGroup;
	CString m_sSectionViewName;
	BOOL m_bIncProcessCard;
	CString m_sProcessCardPathFile;
	CTmaPtrList<CLDSPartGroup,CLDSPartGroup*>* m_pWeldPartSet;
	CSelPartGroupDlg(CWnd* pParent = NULL);   // standard constructor
	CImageList m_ModelImages;

// Dialog Data
	//{{AFX_DATA(CSelPartGroupDlg)
	enum { IDD = IDD_SEL_PART_GROUP_DLG };
	CString	m_sCurrentPartGroup;
	CString	m_sCurrentSectionView;
	BOOL	m_bIgnoreStdPartGroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelPartGroupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelPartGroupDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangedTreePartGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIgnoreStdPartgroup();
	afx_msg void OnBnSpecWeldpartCardPathfile();
	afx_msg void OnChkIncProcessCard();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void RefreshModel();
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELPARTGROUPDLG_H__9B354CBC_E46F_4D04_A2C3_DC3D4F1EBA18__INCLUDED_)
