#if !defined(AFX_DIANBANPARADLG_H__5C479F11_A2A3_4172_8540_F22DABAE1CC7__INCLUDED_)
#define AFX_DIANBANPARADLG_H__5C479F11_A2A3_4172_8540_F22DABAE1CC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DianBanParaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDianBanParaDlg dialog

class CDianBanParaDlg : public CDialog
{
// Construction
	int m_nPadThick;
	int m_nWasherThick;
public:
	CDianBanParaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDianBanParaDlg)
	enum { IDD = IDD_DIANBAN_PARA_DLG };
	CString	m_sPartNo;
	int		m_nThick;
	int		m_nSpaceDist;
	int		m_nNum;
	BOOL	m_bCreateSurplusPlate;
	CString	m_sPartNoSurplus;
	CString	m_sNoSurplus;
	CString	m_sNo;
	int		m_iPartType;
	CString	m_sThick;
	BOOL	m_bAutoJudgePad;
	double	m_fPadLen;
	double	m_fPadWidth;
	CString	m_sLsGuiGe;
	BOOL	m_bAutoJudgeLs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDianBanParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	BOOL m_bMirCreate;	//¶Ô³ÆÉú³É
	void CopyProperty(CDianBanParaDlg &dlg);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDianBanParaDlg)
	afx_msg void OnChangeThick();
	afx_msg void OnChkCreateSurplusPlate();
	afx_msg void OnRdoPlateType();
	virtual BOOL OnInitDialog();
	afx_msg void OnChkAutoJudgeLs();
	afx_msg void OnChkAutoJudgePad();
	afx_msg void OnSelchangeLsGuige();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIANBANPARADLG_H__5C479F11_A2A3_4172_8540_F22DABAE1CC7__INCLUDED_)
