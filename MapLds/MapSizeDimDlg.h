#if !defined(AFX_MAPSIZEDIMDLG_H__F97A8A26_7738_496B_B822_8C2DE798A097__INCLUDED_)
#define AFX_MAPSIZEDIMDLG_H__F97A8A26_7738_496B_B822_8C2DE798A097__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapSizeDimDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapSizeDimDlg dialog
#include "StdArx.h"
#include "resource.h"
#include "CADCallBackDlg.h"
class CMapSizeDimDlg : public CCADCallBackDlg
{
// Construction
public:
	CMapSizeDimDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMapSizeDimDlg)
	enum { IDD = IDD_MAP_SIZE_DIM_DLG };
	int		m_iDirection;
	double	m_fMapScale;
	int		m_iPrecision;
	double	m_fActualDist;
	//}}AFX_DATA
public:
	void FinishSelectObjOper();		//完成选择对象的后续操作
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapSizeDimDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMapSizeDimDlg)
	afx_msg void OnBnClickedBtnSnapObj();
	afx_msg void OnBnClickedDimensionDriection();
	afx_msg void OnBnClickedVertical();
	afx_msg void OnBnClickedExtend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPSIZEDIMDLG_H__F97A8A26_7738_496B_B822_8C2DE798A097__INCLUDED_)
