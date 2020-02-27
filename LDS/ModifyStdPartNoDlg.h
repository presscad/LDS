#if !defined(AFX_MODIFYSTDPARTNODLG_H__E8D4CA59_D0E0_49D1_BA76_0E94625188D3__INCLUDED_)
#define AFX_MODIFYSTDPARTNODLG_H__E8D4CA59_D0E0_49D1_BA76_0E94625188D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyStdPartNODlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModifyStdPartNoDlg dialog

class CModifyStdPartNoDlg : public CDialog
{
// Construction
public:
	CModifyStdPartNoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModifyStdPartNoDlg)
	enum { IDD = IDD_MODIFY_STD_PART_NO_DLG };
	int		m_iPartType;
	CString	m_sPartNo;
	int		m_iPartMaterial;
	int		m_iPartLevel;
	int		m_iMainTubeMaterial;
	int		m_iMainTubeGuige;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyStdPartNoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModifyStdPartNoDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYSTDPARTNODLG_H__E8D4CA59_D0E0_49D1_BA76_0E94625188D3__INCLUDED_)
