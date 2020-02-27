#if !defined(AFX_DESIGNVERTPLATEDLG_H__CE856846_52A9_4D8E_BF89_4F1E3B69F2DA__INCLUDED_)
#define AFX_DESIGNVERTPLATEDLG_H__CE856846_52A9_4D8E_BF89_4F1E3B69F2DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignVertPlateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignVertPlateDlg dialog

class CDesignVertPlateDlg : public CDialog
{
// Construction
public:
	CDesignVertPlateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignVertPlateDlg)
	enum { IDD = IDD_DESIGN_VERT_PLATE_DLG };
	CComboBox	m_listMaterial;
	int			m_iMaterial;
	CString		m_sSegI;
	int			m_nThick;
	double		m_fWidth;
	double		m_fLength;
	CString		m_sPartNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignVertPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignVertPlateDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNVERTPLATEDLG_H__CE856846_52A9_4D8E_BF89_4F1E3B69F2DA__INCLUDED_)
