#if !defined(AFX_TABLEGRIDDLG_H__D6D73C69_E332_488A_A090_D0EAC154685C__INCLUDED_)
#define AFX_TABLEGRIDDLG_H__D6D73C69_E332_488A_A090_D0EAC154685C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TableGridDlg.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTableGridDlg dialog

class CTableGridDlg : public CDialog
{
// Construction
public:
	int content_id;
	BOOL SetCMBData(DWORD content_id);
	int GetCMBData(int index);
	CTableGridDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTableGridDlg)
	enum { IDD = IDD_SET_BASE_GRID_STYLE_DLG };
	int		m_iContent;
	int		m_iDataType;
	double	m_fHighToWide;
	double	m_fTextHigh;
	CString	m_sDisplayContent;
	CString m_sDisplayPrefix;
	CString m_sDisplaySuffix;
	int		m_iAlignType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTableGridDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTableGridDlg)
	afx_msg void OnSelchangeCmbDataContent();
	afx_msg void OnSelchangeCmbDataType();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLEGRIDDLG_H__D6D73C69_E332_488A_A090_D0EAC154685C__INCLUDED_)
