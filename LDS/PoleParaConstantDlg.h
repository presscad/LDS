#if !defined(AFX_POLEPARACONSTANTDLG_H__FA1223C4_90E7_4AA5_8ED0_BEAE7750CFE1__INCLUDED_)
#define AFX_POLEPARACONSTANTDLG_H__FA1223C4_90E7_4AA5_8ED0_BEAE7750CFE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PoleParaConstantDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPoleParaConstantDlg dialog

class CPoleParaConstantDlg : public CDialog
{
// Construction
public:
	CPoleParaConstantDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPoleParaConstantDlg)
	enum { IDD = IDD_POLE_PARA_CONSTANT_DLG };
	double	m_fArea;
	double	m_fPara1;
	double	m_fPara2;
	double	m_fUnitWeight;
	double	m_fYangModulus;
	double	m_fDesCompStrength;
	double	m_fDesTensStrength;
	double	m_fPreStress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPoleParaConstantDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPoleParaConstantDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POLEPARACONSTANTDLG_H__FA1223C4_90E7_4AA5_8ED0_BEAE7750CFE1__INCLUDED_)
