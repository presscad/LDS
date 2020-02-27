#if !defined(AFX_ADDCIRCULARPLATEDLG_H__7C3CB6A6_C47E_452F_B3B1_BAF239A2F38E__INCLUDED_)
#define AFX_ADDCIRCULARPLATEDLG_H__7C3CB6A6_C47E_452F_B3B1_BAF239A2F38E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddCircularPlateDlg.h : header file
//
#ifdef __ANGLE_PART_INC_

/////////////////////////////////////////////////////////////////////////////
// CAddCircularPlateDlg dialog

class CAddCircularPlateDlg : public CDialog
{
// Construction
public:
	CAddCircularPlateDlg(CWnd* pParent = NULL);   // standard constructor
	//CBitmap m_CurPlateBmp;
// Dialog Data
	//{{AFX_DATA(CAddCircularPlateDlg)
	enum { IDD = IDD_ADD_CIRCULAR_PLATE_DLG };
	int		m_iMaterial;
	double	m_fParaD;
	double	m_fParaW;
	double	m_fPlateT;
	CString	m_sPartNo;
	CString m_sSegI;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fParaS;
	double	m_fNormZ;
	double	m_fParaH;
	BOOL	m_bUseH;
	double	m_fParaN;
	BOOL	m_bMirCreate;
	BOOL	m_bDesignOuterPlate;
	BOOL	m_bCir;
	//}}AFX_DATA
	CBitmap m_CurPlateBmp;
	CLDSSphere *m_pDatumSphere;
	CLDSLineTube *m_pDatumTube;
	CLDSNode *m_pDatumNode;
	CLDSPlate *m_pDatumPlate1, *m_pDatumPlate2;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddCircularPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddCircularPlateDlg)
	afx_msg void OnBnPartsDb();
	afx_msg void OnChangeEPlatePartNo();
	afx_msg void OnChangeS();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCir();
	afx_msg void OnPasteNorm();
	afx_msg void OnChangeParaW();
	//}}AFX_MSG
	BOOL AutoCalWN();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnChkUseH();
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDCIRCULARPLATEDLG_H__7C3CB6A6_C47E_452F_B3B1_BAF239A2F38E__INCLUDED_)
