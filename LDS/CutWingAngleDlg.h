#if !defined(AFX_CUTWINGANGLEDLG_H__0BA03884_219E_4A77_A4EC_61C9BFA98841__INCLUDED_)
#define AFX_CUTWINGANGLEDLG_H__0BA03884_219E_4A77_A4EC_61C9BFA98841__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CutWingAngleDlg.h : header file
//

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CCutWingAngleDlg dialog

class CCutWingAngleDlg : public CDialog
{
// Construction
public:
	void RefreshReadOnlyState();
	CCutWingAngleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCutWingAngleDlg)
	enum { IDD = IDD_CUT_WING_ANGLE_DLG };
	double	m_fCutAngleXLen;
	double	m_fCutAngleXWide;
	double	m_fCutAngleYLen;
	double	m_fCutAngleYWide;
	int     m_iCutXWingAngle;
	double	m_fCutWingLen;
	double	m_fCutWingLen2;
	double	m_fCutWingWide;
	BOOL	m_bUserSpecCutInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCutWingAngleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCutWingAngleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkUserSpecCutInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUTWINGANGLEDLG_H__0BA03884_219E_4A77_A4EC_61C9BFA98841__INCLUDED_)
