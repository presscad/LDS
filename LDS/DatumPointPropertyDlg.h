#if !defined(AFX_DATUMPOINTPROPERTYDLG_H__BAA45DBE_0C6B_4B68_BAE1_40C82F1961F6__INCLUDED_)
#define AFX_DATUMPOINTPROPERTYDLG_H__BAA45DBE_0C6B_4B68_BAE1_40C82F1961F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DatumPointPropertyDlg.h : header file
//
#include "resource.h"
#include "NativePropertyList.h"
#include "CallBackDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CDatumPointPropertyDlg dialog


class CDatumPointPropertyDlg : public CCallBackDialog
{
public:
	CLDSPoint m_xPoint;
	BOOL m_bEnableUpdateEditPoint;
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
public:
	CLDSDbObject *m_pCurEditObj;
	CDatumPointPropertyDlg(CWnd* pParent = NULL);   // standard constructor
	void ModifyPositionFromPropList(double coord,char cCoordType);
// Dialog Data
	//{{AFX_DATA(CDatumPointPropertyDlg)
	enum { IDD = IDD_DATUM_POINT_PROPERTY_DLG };
	CParaPointPropertyList	m_propList;
	int		m_iDatumPointStyle;
	double	m_fPointX;
	double	m_fPointY;
	double	m_fPointZ;
	//}}AFX_DATA
	void UpdateEditPoint();	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatumPointPropertyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDatumPointPropertyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDatumPointStyle();
	virtual void OnOK();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnCopyPoint();
	afx_msg void OnPastePoint();
	afx_msg void OnEnChangePointCoord();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATUMPOINTPROPERTYDLG_H__BAA45DBE_0C6B_4B68_BAE1_40C82F1961F6__INCLUDED_)
