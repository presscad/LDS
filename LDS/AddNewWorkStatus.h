#if !defined(AFX_ADDNEWWORKSTATUS_H__ED078309_E833_4D2C_8020_661297840EEE__INCLUDED_)
#define AFX_ADDNEWWORKSTATUS_H__ED078309_E833_4D2C_8020_661297840EEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddNewWorkStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddNewWorkStatus dialog

class CAddNewWorkStatus : public CDialog
{
// Construction
public:
	CAddNewWorkStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddNewWorkStatus)
	enum { IDD = IDD_ADD_WORKSTATUS_DLG };
	CString	m_sWorkStatus;
	double	m_fIceThick;
	double	m_fMaxWind;
	long	m_fWindAngle;
	CString	m_sModelLimStr;
	BOOL	m_bMinVertSpace;
	CString	m_sVibrationMode;
	BYTE	m_cHoriEqAxisAzimuth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddNewWorkStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddNewWorkStatus)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDNEWWORKSTATUS_H__ED078309_E833_4D2C_8020_661297840EEE__INCLUDED_)
