#if !defined(AFX_COMPARELINEPARTPOSDLG_H__D1EEAD76_D183_4145_B154_3EBA6271FB67__INCLUDED_)
#define AFX_COMPARELINEPARTPOSDLG_H__D1EEAD76_D183_4145_B154_3EBA6271FB67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompareLinePartPosDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompareLinePartPosDlg dialog

class CCompareLinePartPosDlg : public CDialog
{
// Construction
public:
	CCompareLinePartPosDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSLinePart *m_pDatumLinePart1,*m_pDatumLinePart2;
	BOOL m_bPauseBreakExit1;	//是否为中途暂时中断选择交叉杆件性质的退出
	BOOL m_bPauseBreakExit2;	//是否为中途暂时中断选择交叉杆件性质的退出
	void UpdateLinePartPos();
// Dialog Data
	//{{AFX_DATA(CCompareLinePartPosDlg)
	enum { IDD = IDD_COMPARE_LINEPART_POS_DLG };
	CString	m_sLinePartHandle1;
	CString	m_sLinePartHandle2;
	CString	m_sLinePart2PartNo;
	CString	m_sLinePart1PartNo;
	double	m_fEndOddment1;
	double	m_fEndPosX1;
	double	m_fEndPosY1;
	double	m_fEndPosZ1;
	double	m_fStartOddment1;
	double	m_fStartPosX1;
	double	m_fStartPosY1;
	double	m_fStartPosZ1;
	double	m_fEndOddment2;
	double	m_fEndPosX2;
	double	m_fEndPosY2;
	double	m_fEndPosZ2;
	double	m_fStartOddment2;
	double	m_fStartPosX2;
	double	m_fStartPosY2;
	double	m_fStartPosZ2;
	double	m_fLinePartLen1;
	double	m_fLinePartLen2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompareLinePartPosDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCompareLinePartPosDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectLinePart1();
	afx_msg void OnSelectLinePart2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPARELINEPARTPOSDLG_H__D1EEAD76_D183_4145_B154_3EBA6271FB67__INCLUDED_)
