#if !defined(AFX_FOOTPLANKWRAPLENDLG_H__638B9519_725A_4855_8774_08D1BD7A53CD__INCLUDED_)
#define AFX_FOOTPLANKWRAPLENDLG_H__638B9519_725A_4855_8774_08D1BD7A53CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FootPlankWrapLenDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFootPlankWrapLenDlg dialog

class CFootPlankWrapLenDlg : public CDialog
{
// Construction
public:
	int weld_style;
	BOOL m_bBreakWingX;
	BOOL m_bBreakWingY;
	CFootPlankWrapLenDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFootPlankWrapLenDlg)
	enum { IDD = IDD_FOOT_PLANK_WRAP_LEN_DLG };
	double	m_fRidgeWrapLen;
	double	m_fXWingBackWrapWidth;
	double	m_fXWingFrontOffset;
	double	m_fYWingBackWrapWidth;
	double	m_fYWingFrontOffset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFootPlankWrapLenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFootPlankWrapLenDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOOTPLANKWRAPLENDLG_H__638B9519_725A_4855_8774_08D1BD7A53CD__INCLUDED_)
