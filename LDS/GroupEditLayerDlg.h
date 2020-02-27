#if !defined(AFX_GROUPEDITLAYERDLG_H__A002C6D5_833E_4190_8040_3126CEFB4910__INCLUDED_)
#define AFX_GROUPEDITLAYERDLG_H__A002C6D5_833E_4190_8040_3126CEFB4910__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupEditLayerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupEditLayerDlg dialog

class CGroupEditLayerDlg : public CDialog
{
// Construction
public:
	char layer[4];
	CGroupEditLayerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGroupEditLayerDlg)
	enum { IDD = IDD_GROUP_SET_LAYER_DLG };
	BOOL	m_bFirstChar;
	BOOL	m_bSecChar;
	BOOL	m_bThirdChar;
	int		m_iFirstChar;
	int		m_iSecChar;
	int		m_iThirdChar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupEditLayerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGroupEditLayerDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChkFirstChar();
	afx_msg void OnChkSecChar();
	afx_msg void OnChkThirdChar();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPEDITLAYERDLG_H__A002C6D5_833E_4190_8040_3126CEFB4910__INCLUDED_)
