#if !defined(AFX_MODIFYVERTEXDLG_H__30E4AD04_D453_415E_A6CD_97982AFCE48F__INCLUDED_)
#define AFX_MODIFYVERTEXDLG_H__30E4AD04_D453_415E_A6CD_97982AFCE48F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyVertexDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModifyVertexDlg dialog

class CModifyVertexDlg : public CDialog
{
// Construction
public:
	CString m_sTitle;
	BOOL m_bEnableModify;
	CModifyVertexDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModifyVertexDlg)
	enum { IDD = IDD_MODIFY_VERTEX_POS_DLG };
	double	m_fVertexPosX;
	double	m_fVertexPosY;
	double	m_fVertexPosZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyVertexDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModifyVertexDlg)
	afx_msg void OnBnPasteVertex();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYVERTEXDLG_H__30E4AD04_D453_415E_A6CD_97982AFCE48F__INCLUDED_)
