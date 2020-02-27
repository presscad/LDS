#if !defined(AFX_STDPARTSETDLG_H__4A468C70_D687_4AFA_9C74_4F20F554ACC9__INCLUDED_)
#define AFX_STDPARTSETDLG_H__4A468C70_D687_4AFA_9C74_4F20F554ACC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StdPartSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStdPartSetDlg dialog
#include "resource.h"
#include "XhListCtrl.h"
#include "PartLib.h"
#include "Tower.h"

#ifndef __TSA_FILE_
class CStdPartSetDlg : public CDialog
{
// Construction
	int m_iStdPartType;	//标准构件类型
	void UpdatePartList();
public:
	CStdPartSetDlg(CWnd* pParent = NULL);   // standard constructor
	int GetStdPartType() const {return m_iStdPartType;}
	//
	struct STDPART_ITEM{
		FL_PARAM *pFlParam;
		FL_PARAM2 *pFlParam2;
		INSERT_PLATE_PARAM *pInsertPlate;
		PARAM_PLATE_TYPE iStdType;
		STDPART_ITEM(){memset(this,0,sizeof(STDPART_ITEM));}
	};
	ATOM_LIST<STDPART_ITEM> partList;
// Dialog Data
	//{{AFX_DATA(CStdPartSetDlg)
	enum { IDD = IDD_STD_PART_SET_DLG };
	CXhListCtrl	m_listCtrl;
	CString	m_sLibFilePath;
	CString	m_sPrompt;
	CString	m_sLibName;
	CString	m_sCurPartName;
	int m_iRollType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStdPartSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStdPartSetDlg)
	afx_msg void OnBrowseFile();
	afx_msg void OnBatchCreate();
	afx_msg void OnCreateCurPart();
	afx_msg void OnChangeLibFilePath();
	virtual BOOL OnInitDialog();
	afx_msg void OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCbnSelchangeCmbRollType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDPARTSETDLG_H__4A468C70_D687_4AFA_9C74_4F20F554ACC9__INCLUDED_)
