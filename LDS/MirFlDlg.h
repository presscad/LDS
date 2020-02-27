#if !defined(AFX_MIRFLDLG_H__C2FC76F8_588D_4B08_BF93_F4A5BBFA3622__INCLUDED_)
#define AFX_MIRFLDLG_H__C2FC76F8_588D_4B08_BF93_F4A5BBFA3622__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MirFlDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMirFlDlg dialog

class CMirFlDlg : public CDialog
{
// Construction
public:
	CMirFlDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMirFlDlg)
	enum { IDD = IDD_MIR_FL_DLG };
	BOOL	m_bCreateElbowPlate;
	BOOL	m_bMirSideFace;
	int		m_iMirMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMirFlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMirFlDlg)
	afx_msg void OnChkTypeModify();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIRFLDLG_H__C2FC76F8_588D_4B08_BF93_F4A5BBFA3622__INCLUDED_)
