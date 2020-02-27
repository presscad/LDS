#if !defined(AFX_POLEHORIZONANGLEDLG_H__E2BDC460_2539_4286_8A0F_B3EA30C4BB8D__INCLUDED_)
#define AFX_POLEHORIZONANGLEDLG_H__E2BDC460_2539_4286_8A0F_B3EA30C4BB8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PoleHorizonAngleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPoleHorizonAngleDlg dialog

class CPoleHorizonAngleDlg : public CDialog
{
// Construction
public:
	CPoleHorizonAngleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPoleHorizonAngleDlg)
	enum { IDD = IDD_LINE_VEC_ANGLE_DLG };
	double	m_fVecY;
	double	m_fVecX;
	double	m_fVecZ;
	CString	m_sAngle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPoleHorizonAngleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPoleHorizonAngleDlg)
	afx_msg void OnChangeVec();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POLEHORIZONANGLEDLG_H__E2BDC460_2539_4286_8A0F_B3EA30C4BB8D__INCLUDED_)
