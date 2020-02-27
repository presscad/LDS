#if !defined(AFX_DEFHUOQUFACEDLG_H__46AC61A6_8915_41CF_A0AC_438A670F8E57__INCLUDED_)
#define AFX_DEFHUOQUFACEDLG_H__46AC61A6_8915_41CF_A0AC_438A670F8E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefHuoquFaceDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDefHuoquFaceDlg dialog

class CDefHuoquFaceDlg : public CDialog
{
// Construction
public:
	CDefHuoquFaceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefHuoquFaceDlg)
	enum { IDD = IDD_DEF_HUOQU_FACE_DLG };
	double	m_fEndPosX;
	double	m_fEndPosY;
	double	m_fEndPosZ;
	double	m_fHuoquNormX;
	double	m_fHuoquNormY;
	double	m_fHuoquNormZ;
	double	m_fStartPosX;
	double	m_fStartPosZ;
	double	m_fStartPosY;
	//}}AFX_DATA
	CDesignLjPara *m_pLjPara;
	int		m_iHuoquFaceType;//表示是第几火曲面

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefHuoquFaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefHuoquFaceDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnCopyVecEnd();
	afx_msg void OnBnCopyVecStart();
	afx_msg void OnBNCOPYVECHUOQu();
	afx_msg void OnBnEditStartPos();
	afx_msg void OnBnEditHuoqu();
	afx_msg void OnBnEditEndPos();
	afx_msg void OnBnCopyFace();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFHUOQUFACEDLG_H__46AC61A6_8915_41CF_A0AC_438A670F8E57__INCLUDED_)
