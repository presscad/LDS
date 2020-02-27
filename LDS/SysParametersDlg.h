#if !defined(AFX_SYSPARAMETERSDLG_H__0419DE9B_C857_4213_9B11_CA50673B20B3__INCLUDED_)
#define AFX_SYSPARAMETERSDLG_H__0419DE9B_C857_4213_9B11_CA50673B20B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SysParametersDlg.h : header file
//
#if defined(__TSA_)||defined(__LDS_)||defined(__INTERNAL_VERSION)
#include "SuperGridCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CSysParametersDlg dialog

class CSysParametersDlg : public CDialog
{
// Construction
public:
	CSysParametersDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSysParametersDlg)
	enum { IDD = IDD_SYSTEM_PARAMETER_DLG };
	CSuperGridCtrl	m_listCtrlSysParams;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysParametersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSysParametersDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSPARAMETERSDLG_H__0419DE9B_C857_4213_9B11_CA50673B20B3__INCLUDED_)
