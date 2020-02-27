#if !defined(AFX_FACEOFFSETDLG_H__9F02306E_21F6_40C3_99A6_687AAE6C54DA__INCLUDED_)
#define AFX_FACEOFFSETDLG_H__9F02306E_21F6_40C3_99A6_687AAE6C54DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FaceOffsetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFaceOffsetDlg dialog

class CFaceOffsetDlg : public CDialog
{
// Construction
public:
	CFaceOffsetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFaceOffsetDlg)
	enum { IDD = IDD_FACE_OFFSET_DLG };
	double	m_fFaceOffsetNormX;
	double	m_fFaceOffsetNormY;
	int		m_iFaceOffsetDatumLine;
	int		m_iFaceOffsetDatumPosStyle;
	double	m_fFaceOffsetNormZ;
	int		m_iType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFaceOffsetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFaceOffsetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCmbType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FACEOFFSETDLG_H__9F02306E_21F6_40C3_99A6_687AAE6C54DA__INCLUDED_)
