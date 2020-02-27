#if !defined(AFX_FACEPANELDETAILDLG_H__A94C6C02_1D83_459C_9E2C_8B9BDEC6CC0D__INCLUDED_)
#define AFX_FACEPANELDETAILDLG_H__A94C6C02_1D83_459C_9E2C_8B9BDEC6CC0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FacePanelDetailDlg.h : header file
//
#include "StdFacePanel.h"
#include "BitmapGraph.h"
#include "EntityDataBase.h"

/////////////////////////////////////////////////////////////////////////////
// CFacePanelDetailDlg dialog

class CFacePanelDetailDlg : public CDialog
{
// Construction
public:
	CFacePanelDetailDlg(CWnd* pParent = NULL);   // standard constructor
	CFacePanel m_xFacePanel;
	UCS_STRU rgn_ucs;
	CRect m_rectValidRgn;
	CEntityDataBase ent_base;
	SCOPE_STRU scope;
	double rgn_scale;
// Dialog Data
	//{{AFX_DATA(CFacePanelDetailDlg)
	enum { IDD = IDD_FACE_PANEL_DETAIL_DLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFacePanelDetailDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFacePanelDetailDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FACEPANELDETAILDLG_H__A94C6C02_1D83_459C_9E2C_8B9BDEC6CC0D__INCLUDED_)
