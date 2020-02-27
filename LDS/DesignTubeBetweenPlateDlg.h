#if !defined(AFX_DESIGNTUBEBETWEENPLATEDLG_H__1DA4C3B2_4492_49C8_BC8A_CD8947F52CB8__INCLUDED_)
#define AFX_DESIGNTUBEBETWEENPLATEDLG_H__1DA4C3B2_4492_49C8_BC8A_CD8947F52CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignTubeBetweenPlateDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
#include "CallBackDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CDesignTubeBetweenPlateDlg dialog

class CDesignTubeBetweenPlateDlg : public CCallBackDialog
{
// Construction
public:
	CDesignTubeBetweenPlateDlg(CWnd* pParent = NULL);   // standard constructor
	void RefreshDlgCtrlState();
// Dialog Data
	//{{AFX_DATA(CDesignTubeBetweenPlateDlg)
	enum { IDD = ID_DESIGN_TUBE_BETWEEN_PLATE_DLG };
	BOOL	m_bAutoCalcFirst;
	BOOL	m_bHasInterPos;
	BOOL	m_bLinkLinePart;
	BOOL	m_bMirCreate;
	double	m_fFirstEdgeLen;
	double	m_fFirstEdgeWidth;
	double	m_fSecondEdgeLen;
	double	m_fSecondEdgeWidth;
	double	m_fTangentWidth;
	double	m_fPlateThick;
	int		m_iOnlyOnePlate;
	CString	m_sPartName;
	CString m_sSegI;
	CString	m_sPartNo;
	int		m_iMaterial;
	CString	m_sTransectPartHandle;
	BOOL	m_bUseTangWidth;
	BOOL	m_bAutoCalcSecond;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignTubeBetweenPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignTubeBetweenPlateDlg)
	afx_msg void OnSelectPart();
	afx_msg void OnHasInterPos();
	afx_msg void OnAutoCalc();
	afx_msg void OnOnlyOnePlate();
	afx_msg void OnChangeTransectPartHandle();
	afx_msg void OnChangePartNo();
	afx_msg void OnUseTangWidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNTUBEBETWEENPLATEDLG_H__1DA4C3B2_4492_49C8_BC8A_CD8947F52CB8__INCLUDED_)
