#if !defined(AFX_SEL2JGKEYPOINTDLG_H__4A9F8156_E524_4440_98A9_CE383001B2A2__INCLUDED_)
#define AFX_SEL2JGKEYPOINTDLG_H__4A9F8156_E524_4440_98A9_CE383001B2A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Sel2JgKeyPointDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSel2JgKeyPointDlg dialog
#include "RememberLocationDlg.h"
class CSel2JgKeyPointDlg : public CRememberLocationDlg
{
// Construction
	CLDSPoint m_xStartPoint,m_xEndPoint;
public:
	CLDSNode *m_pFromAngleNode,*m_pToAngleNode;
	CLDSLineAngle *m_pFromAngle,*m_pToAngle;
	bool m_bAddCheckDim;
	BYTE m_cbDimFlag;
public:
	CSel2JgKeyPointDlg(CWnd* pParent = NULL);   // standard constructor
	void Update2JgKeyPointPos();
	void Cal2KeyPointDistance();
	GEPOINT GetStartPt(){return m_xStartPoint.Position();}
	GEPOINT GetEndPt(){return m_xEndPoint.Position();}
// Dialog Data
	//{{AFX_DATA(CSel2JgKeyPointDlg)
	enum { IDD = IDD_SEL_2JG_KEY_POINT_DLG };
	int		m_iFirstKeyPoint;
	int		m_iFirstOffsetStyle;	//0.投影;1.X-Y平面;2.Y-Z平面;3.X-Z平面
	int		m_iSecondKeyPoint;
	int		m_iSecondOffsetStyle;	//0.投影;1.X-Y平面;2.Y-Z平面;3.X-Z平面
	BOOL	m_bChkDeitaL;
	BOOL	m_bChkDeitaX;
	BOOL	m_bChkDeitaY;
	BOOL	m_bChkDeitaZ;
	BOOL	m_bMarkCheck;
	double	m_fCheckDist;
	CString m_sDist;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSel2JgKeyPointDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSel2JgKeyPointDlg)
	afx_msg void OnBnClickedChkDeitaL();
	afx_msg void OnBnClickedChkDeitaX();
	afx_msg void OnBnClickedChkDeitaY();
	afx_msg void OnBnClickedChkDeitaZ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEL2JGKEYPOINTDLG_H__4A9F8156_E524_4440_98A9_CE383001B2A2__INCLUDED_)
