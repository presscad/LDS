#if !defined(AFX_CUTPLATEOPTIONDLG_H__E8AC0C83_B79C_49AF_BBE3_FB99739CA011__INCLUDED_)
#define AFX_CUTPLATEOPTIONDLG_H__E8AC0C83_B79C_49AF_BBE3_FB99739CA011__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CutPlateOptionDlg.h : header file
//

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CCutPlateOptionDlg dialog

class CCutPlateOptionDlg : public CDialog
{
// Construction
public:
	double m_fTopBtmSpace;
	void UpdateCutFaceNorm();
	CCutPlateOptionDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSPart *m_pDatumPart;
// Dialog Data
	//{{AFX_DATA(CCutPlateOptionDlg)
	enum { IDD = IDD_CUT_PLATE_OPTION_DLG };
	BOOL	m_bSaveAnotherPlate;
	int		m_iFaceNo;
	double	m_fOffsetDist;
	int		m_iTopFace;
	double	m_fCutFaceNormX;
	double	m_fCutFaceNormY;
	double	m_fCutFaceNormZ;
	double	m_fCutFacePickX;
	double	m_fCutFacePickY;
	double	m_fCutFacePickZ;
	BOOL	m_bWeld;
	BOOL	m_bMirCut;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCutPlateOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCutPlateOptionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSaveAnotherPlate();
	afx_msg void OnSelchangeCmbFaceNo();
	afx_msg void OnRdoTopBtmFace();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUTPLATEOPTIONDLG_H__E8AC0C83_B79C_49AF_BBE3_FB99739CA011__INCLUDED_)
