#if !defined(AFX_CORRECTPLATEPARADLG_H__E7B7B4FF_EF02_4F8A_B14D_5F2F08697FB2__INCLUDED_)
#define AFX_CORRECTPLATEPARADLG_H__E7B7B4FF_EF02_4F8A_B14D_5F2F08697FB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CorrectPlateParaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCorrectPlateParaDlg dialog

class CCorrectPlateParaDlg : public CDialog
{
// Construction
public:
	CCorrectPlateParaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCorrectPlateParaDlg)
	enum { IDD = IDD_CORRECT_PLATE_PARA_DLG };
	BOOL	m_bCorrectAngleProfilePara;
	BOOL	m_bCorrectLsInfo;
	int		m_iPlateMaterial;
	int		m_iProfileStyle;
	CString	m_sPlatePartNo;
	CString m_sPlateSegI;
	int		m_nPlateThick;
	BOOL	m_bCalLinePartOddment;
	//}}AFX_DATA
	BOOL	m_bGuiGeMatch;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCorrectPlateParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCorrectPlateParaDlg)
	afx_msg void OnChangePlatePartNo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CORRECTPLATEPARADLG_H__E7B7B4FF_EF02_4F8A_B14D_5F2F08697FB2__INCLUDED_)
