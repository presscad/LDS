#if !defined(AFX_SELPARTTYPEDLG_H__E3D7A77D_0848_4CEA_94E8_8D000D4E6F81__INCLUDED_)
#define AFX_SELPARTTYPEDLG_H__E3D7A77D_0848_4CEA_94E8_8D000D4E6F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelPartTypeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelPartTypeDlg dialog

class CSelPartTypeDlg : public CDialog
{
// Construction
public:
	CSelPartTypeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelPartTypeDlg)
	enum { IDD = IDD_SEL_PART_TYPE_DLG };
	int		m_iPartType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelPartTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelPartTypeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELPARTTYPEDLG_H__E3D7A77D_0848_4CEA_94E8_8D000D4E6F81__INCLUDED_)
