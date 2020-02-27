#if !defined(AFX_LAYERVIEWDLG_H__1D707C88_A346_4907_A89E_EB15DCAAB75C__INCLUDED_)
#define AFX_LAYERVIEWDLG_H__1D707C88_A346_4907_A89E_EB15DCAAB75C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayerViewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLayerViewDlg dialog

class CLayerViewDlg : public CDialog
{
// Construction
public:
	CLayerViewDlg(CWnd* pParent = NULL);   // standard constructor
	int m_nDlgType;	//0.图层视图 1.定制视图 
	CDisplayView *m_pCurView;
	UCS_STRU ucs;
// Dialog Data
	//{{AFX_DATA(CLayerViewDlg)
	enum { IDD = IDD_LAYER_VIEW_DLG };
	double	m_fAxisXX;
	double	m_fAxisXY;
	double	m_fAxisXZ;
	double	m_fAxisYX;
	double	m_fAxisYY;
	double	m_fAxisYZ;
	double	m_fAxisZX;
	double	m_fAxisZY;
	double	m_fAxisZZ;
	double	m_fOriginPosY;
	double	m_fOriginPosZ;
	double	m_fOriginPosX;
	CString	m_sFirstCharSet;
	CString	m_sSecondCharSet;
	CString	m_sThirdCharSet;
	CString	m_sViewName;
	CString	m_sDefaultLayer;
	CString m_sFolderName;
	BOOL	m_bCanModifyAxisX;
	BOOL	m_bCanModifyAxisY;
	BOOL	m_bCanModifyAxisZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLayerViewDlg)
	afx_msg void OnBnCpyUcs();
	afx_msg void OnBnPasteUcs();
	afx_msg void OnBnPasteOriginPos();
	afx_msg void OnBnPasteAxisZ();
	afx_msg void OnBnPasteAxisY();
	afx_msg void OnBnPasteAxisX();
	afx_msg void OnBnCopyOriginPos();
	afx_msg void OnBnCopyAxisZ();
	afx_msg void OnBnCopyAxisY();
	afx_msg void OnBnCopyAxisX();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnToRightHand();
	afx_msg void OnChkEnableAxisX();
	afx_msg void OnChkEnableAxisY();
	afx_msg void OnChkEnableAxisZ();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateUcs(BOOL bFlag=TRUE);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERVIEWDLG_H__1D707C88_A346_4907_A89E_EB15DCAAB75C__INCLUDED_)
