#if !defined(AFX_DEFFACEDLG_H__A6E4323C_497D_44C7_81B8_4D36FA8A2C2D__INCLUDED_)
#define AFX_DEFFACEDLG_H__A6E4323C_497D_44C7_81B8_4D36FA8A2C2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefFaceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDefFaceDlg dialog

class CDefFaceDlg : public CDialog
{
// Construction
public:
	CDefFaceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefFaceDlg)
	enum { IDD = IDD_DEF_CUT_FACE_DLG };
	double	m_fBasePosX;
	double	m_fBasePosY;
	double	m_fBasePosZ;
	double	m_fFaceNormX;
	double	m_fFaceNormY;
	double	m_fFaceNormZ;
	double	m_fBaseNormOffsetDist;
	BOOL	m_bSaveAnotherPlate;
	//}}AFX_DATA
	BOOL	m_bCutPlate;	//切割钢板时用于定义切割面 wht 11-05-30
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefFaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefFaceDlg)
	afx_msg void OnBnPasteBasePoint();
	afx_msg void OnBnPasteFaceNorm();
	afx_msg void OnBnPastePlane();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFFACEDLG_H__A6E4323C_497D_44C7_81B8_4D36FA8A2C2D__INCLUDED_)
