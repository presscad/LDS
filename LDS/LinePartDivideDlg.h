#if !defined(AFX_JGDIVIDEDLG_H__A143108E_39A0_44A7_BE2C_CC79F2FC1767__INCLUDED_)
#define AFX_JGDIVIDEDLG_H__A143108E_39A0_44A7_BE2C_CC79F2FC1767__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JgDivideDlg.h : header file
//
#include "resource.h"       // main symbols
#include "lds_part.h"
#include "RememberLocationDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CLinePartDivideDlg dialog

class CLinePartDivideDlg : public CRememberLocationDlg
{
// Construction
public:
	CString m_sCaption;
	int m_nSegNum;
	double VertDistList[100];
	CLDSLinePart *m_pCurPart;
	void InitLinePart(CLDSLinePart *pLinePart);
	CLinePartDivideDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLinePartDivideDlg)
	enum { IDD = IDD_MAIN_LINEPART_FENG_DUAN_DLG };
	CString	m_sCurItemText;
	double	m_fVerticalDist;
	CString	m_sPrompt;
	CString	m_sValType;
	CString	m_sMutiLegMsg;
	double	m_fTotalHigh;
	double 	m_fHighRemainder;
	int		m_nMirMsg;
	int		m_iRefAxis;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinePartDivideDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinePartDivideDlg)
	afx_msg void OnBnAddItem();
	afx_msg void OnBnDeleteItem();
	afx_msg void OnBnModifyItem();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListVerticalDist();
	afx_msg void OnSelchangeCmbMirAxis();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	double StringToDouble(CString& source);
	CString DoubleToString(int nLen, double fVerticalDist);
	CListBox* m_pLB;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JGDIVIDEDLG_H__A143108E_39A0_44A7_BE2C_CC79F2FC1767__INCLUDED_)
