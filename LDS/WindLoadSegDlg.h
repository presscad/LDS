#if !defined(AFX_WINDLOADSEGDLG_H__FDBCE47B_B370_4B95_BF1B_D460AF023699__INCLUDED_)
#define AFX_WINDLOADSEGDLG_H__FDBCE47B_B370_4B95_BF1B_D460AF023699__INCLUDED_

#include "LDSMODEL\lds_part.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WindLoadSegDlg.h : header file
//

#include "XhListCtrl.h"
#include "lds_part.h"
#include "CallBackDialog.h"
#include "SuperGridCtrl.h"

#if defined(__LDS_)||defined(__TSA_)
/////////////////////////////////////////////////////////////////////////////
// CWindLoadSegDlg dialog
class CWindLoadSegDlg : public CCallBackDialog
{
	NODESET tempNodeSet;
// Construction
public:
	CWindSegment *m_pWindSeg;
	CWindLoadSegDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWindLoadSegDlg)
	enum { IDD = IDD_WINDLOAD_SEG_DLG };
	CXhListCtrl	m_listNodeCtrl;
	CSuperGridCtrl m_listEiffelCtrl;
	double	m_fFrontCoef;
	double	m_fSideCoef;
	double	m_fBetaZ;
	int		m_iCalStyle;
	int		m_iMirStyle;
	int		m_iSegType;
	double	m_fCoefK;
	double	m_fProfileCoef;
	BOOL	m_bSpecProfileCoef;
	double	m_fBetaZForFoundation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindLoadSegDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWindLoadSegDlg)
	afx_msg void OnBnAddItemRecord();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnKeydownListNodeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChkSpecProfileCoef();
	afx_msg void OnSelchangeCmbCalStyle();
	afx_msg void OnSelchangeCmbSegType();
	afx_msg void OnBnClickedChkSpecBtoAFront();
	afx_msg void OnBnAddEiffelItem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	double m_fBtoAFront;
	BOOL m_bSpecBtoAFront;
	double m_fPrjOrgX;
	double m_fPrjOrgY;
	double m_fPrjOrgZ;
};
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_WINDLOADSEGDLG_H__FDBCE47B_B370_4B95_BF1B_D460AF023699__INCLUDED_)
