#if !defined(AFX_OFFSETPARTDLG_H__EBC409DE_534F_42FC_ADB0_C33272B3C9FD__INCLUDED_)
#define AFX_OFFSETPARTDLG_H__EBC409DE_534F_42FC_ADB0_C33272B3C9FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OffsetPartDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COffsetPartDlg dialog

class COffsetPartDlg : public CDialog
{
// Construction
public:
	COffsetPartDlg(CWnd* pParent = NULL);   // standard constructor
	int part_type;

// Dialog Data
	//{{AFX_DATA(COffsetPartDlg)
	enum { IDD = IDD_MODIFY_PART_POS_DLG };
	double	m_fBaseOffsetX;
	double	m_fBaseOffsetY;
	double	m_fBaseOffsetZ;
	double	m_fEndOffsetX;
	double	m_fEndOffsetY;
	double	m_fEndOffsetZ;
	CString	m_sParaName;
	BOOL	m_bRelativeUcs;
	BOOL	m_bAbsoluteUcs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COffsetPartDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COffsetPartDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkRelativeUcs();
	afx_msg void OnChkAbsoluteUcs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OFFSETPARTDLG_H__EBC409DE_534F_42FC_ADB0_C33272B3C9FD__INCLUDED_)
