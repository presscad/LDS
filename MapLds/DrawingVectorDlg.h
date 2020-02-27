#if !defined(AFX_DRAWINGVECTORDLG_H__9EEAA500_CAB5_4944_BBA7_B5CED88987FB__INCLUDED_)
#define AFX_DRAWINGVECTORDLG_H__9EEAA500_CAB5_4944_BBA7_B5CED88987FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DrawingVectorDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDrawingVectorDlg dialog

class CDrawingVectorDlg : public CDialog
{
// Construction
public:
	CDrawingVectorDlg(CWnd* pParent = NULL);   // standard constructor
	f3dPoint axis_x,axis_z;
// Dialog Data
	//{{AFX_DATA(CDrawingVectorDlg)
	enum { IDD = IDD_DRAWING_VECTOR_DLG };
	double	m_fAxisXX;
	double	m_fAxisXY;
	double	m_fAxisXZ;
	double	m_fAxisZX;
	double	m_fAxisZY;
	double	m_fAxisZZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawingVectorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDrawingVectorDlg)
	afx_msg void OnBnCopyAxisX();
	afx_msg void OnBnPasteAxisX();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnPasteAxisZ();
	afx_msg void OnBnCopyAxisZ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWINGVECTORDLG_H__9EEAA500_CAB5_4944_BBA7_B5CED88987FB__INCLUDED_)
