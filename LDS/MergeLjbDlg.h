#if !defined(AFX_MERGELJBDLG_H__D3BDB8DB_9909_4E1C_85B1_B433EC889E64__INCLUDED_)
#define AFX_MERGELJBDLG_H__D3BDB8DB_9909_4E1C_85B1_B433EC889E64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergeLjbDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMergeLjbDlg dialog

class CMergeLjbDlg : public CDialog
{
// Construction
public:
	CMergeLjbDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMergeLjbDlg)
	enum { IDD = IDD_MERGE_LJB_DLG };
	int		m_iAttachFace;
	int		m_iBaseFace;
	CString	m_hBasePlank;
	CString	m_hAttachPlank;
	//}}AFX_DATA
	CTMANode::ATTACH_PART *m_pBaseAttach,*m_pViceAttach;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeLjbDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMergeLjbDlg)
	afx_msg void OnSelchangeCmbBaseFaceSel();
	afx_msg void OnSelchangeCmbAttachFaceSel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGELJBDLG_H__D3BDB8DB_9909_4E1C_85B1_B433EC889E64__INCLUDED_)
