#if !defined(AFX_DESIGNJINBANDLG_H__8DB0F1EF_D29D_4567_90CB_08BF664E4A38__INCLUDED_)
#define AFX_DESIGNJINBANDLG_H__8DB0F1EF_D29D_4567_90CB_08BF664E4A38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignJinBanDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignJinBanDlg dialog

class CDesignJinBanDlg : public CDialog
{
// Construction
public:
	CDesignJinBanDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignJinBanDlg)
	enum { IDD = IDD_DESIGN_JIN_BAN };
	double		m_fParaA;
	double		m_fParaB;
	double		m_fParaC;
	double		m_fParaD;
	double		m_fParaE;
	double		m_fParaT;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignJinBanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignJinBanDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNJINBANDLG_H__8DB0F1EF_D29D_4567_90CB_08BF664E4A38__INCLUDED_)
