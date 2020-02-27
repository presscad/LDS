#if !defined(AFX_MXBDLG_H__EB475D48_67A4_4605_B651_9E292FD7DFDC__INCLUDED_)
#define AFX_MXBDLG_H__EB475D48_67A4_4605_B651_9E292FD7DFDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MxbDlg.h : header file
//
#include "resource.h"
#include "Tower.h"
/////////////////////////////////////////////////////////////////////////////
// CMxbDlg dialog

class CMxbDlg : public CDialog
{
// Construction
public:
	ATOM_LIST<SEGI> m_xSegNoList;
public:
	CMxbDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMxbDlg)
	enum { IDD = IDD_MXB_DLG };
	int		m_nLocatePos;
	CString	m_sSegNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMxbDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMxbDlg)

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MXBDLG_H__EB475D48_67A4_4605_B651_9E292FD7DFDC__INCLUDED_)
