#if !defined(AFX_MOVEBOLTDLG_H__B5C0D5F7_3FCB_4E9E_A864_5FAD6B495934__INCLUDED_)
#define AFX_MOVEBOLTDLG_H__B5C0D5F7_3FCB_4E9E_A864_5FAD6B495934__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MoveBoltDlg.h : header file
//
#include "RememberLocationDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CMoveBoltDlg dialog

class CMoveBoltDlg : public CRememberLocationDlg
{
// Construction
public:
	CMoveBoltDlg(CWnd* pParent = NULL);   // standard constructor
	CString m_sTitle;	//Ä¬ÈÏÎª"ÒÆÈ´ÂÝË¨"
// Dialog Data
	//{{AFX_DATA(CMoveBoltDlg)
	enum { IDD = IDD_MOVE_BOLT_DLG };
	double	m_fLenOffsetDist;
	double	m_fWingOffsetDist;
	long	m_nNormOffset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoveBoltDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMoveBoltDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	BOOL m_bCreateNewBolt;
public:
	bool m_bDisplayNewBoltCheckBox;
	bool IsCreateNewBolt();
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOVEBOLTDLG_H__B5C0D5F7_3FCB_4E9E_A864_5FAD6B495934__INCLUDED_)
