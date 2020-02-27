#if !defined(AFX_TUBESPECIALPROCESSDLG_H__F319F0DA_0FC6_4FF6_BFD4_D7D3B6D8A9CC__INCLUDED_)
#define AFX_TUBESPECIALPROCESSDLG_H__F319F0DA_0FC6_4FF6_BFD4_D7D3B6D8A9CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TubeSpecialProcessDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTubeSpecialProcessDlg dialog

class CTubeSpecialProcessDlg : public CDialog
{
// Construction
public:
	CTubeSpecialProcessDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTubeSpecialProcessDlg)
	enum { IDD = IDD_TUBE_SPECIAL_PROCESS_DLG };
	BOOL	m_bSlotC;
	BOOL	m_bPlaneCut;
	BOOL	m_bTwoSlot;
	BOOL	m_bTransect;
	BOOL	m_bSlotT;
	BOOL	m_bSlotU;
	BOOL	m_bSlotX;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTubeSpecialProcessDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTubeSpecialProcessDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TUBESPECIALPROCESSDLG_H__F319F0DA_0FC6_4FF6_BFD4_D7D3B6D8A9CC__INCLUDED_)
