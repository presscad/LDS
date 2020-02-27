#if !defined(AFX_DESIGNVERTEXPARADLG_H__76D5A662_A7FC_4689_A5A3_D1A5B2E1B7BB__INCLUDED_)
#define AFX_DESIGNVERTEXPARADLG_H__76D5A662_A7FC_4689_A5A3_D1A5B2E1B7BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignVertexParaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignVertexParaDlg dialog

class CDesignVertexParaDlg : public CDialog
{
// Construction
public:
	CDesignVertexParaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignVertexParaDlg)
	enum { IDD = IDD_DESIGN_VERTEX_PARA_DLG };
	double	m_fPosX;
	double	m_fPosY;
	int		m_iFaceI;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignVertexParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignVertexParaDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNVERTEXPARADLG_H__76D5A662_A7FC_4689_A5A3_D1A5B2E1B7BB__INCLUDED_)
