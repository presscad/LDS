#if !defined(AFX_EDITPLATEDATUMFACEDLG_H__FFE399F4_35A4_4E33_A353_ACCB85ACFAFB__INCLUDED_)
#define AFX_EDITPLATEDATUMFACEDLG_H__FFE399F4_35A4_4E33_A353_ACCB85ACFAFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditPlateDatumFaceDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CEditPlateDatumFaceDlg dialog

class CEditPlateDatumFaceDlg : public CDialog
{
// Construction
public:
	CEditPlateDatumFaceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditPlateDatumFaceDlg)
	enum { IDD = IDD_EDIT_PLATE_DATUM_FACE_DLG };
	double	m_fBaseX;
	double	m_fBaseY;
	double	m_fBaseZ;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	//}}AFX_DATA
	CLDSPoint	*m_pPoint;
	CDesignLjPara *m_pLjPara;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditPlateDatumFaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditPlateDatumFaceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnDesBasePos();
	afx_msg void OnBnDesBaseNorm();
	afx_msg void OnBnCopyBase();
	afx_msg void OnBnCopyNorm();
	afx_msg void OnBntCopyPlane2();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITPLATEDATUMFACEDLG_H__FFE399F4_35A4_4E33_A353_ACCB85ACFAFB__INCLUDED_)
