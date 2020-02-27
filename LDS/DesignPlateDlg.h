#if !defined(AFX_DESIGNPLATEDLG_H__4B7D0D77_B771_40D7_BDA3_B4A4C07D6C30__INCLUDED_)
#define AFX_DESIGNPLATEDLG_H__4B7D0D77_B771_40D7_BDA3_B4A4C07D6C30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignPlateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignPlateDlg dialog

class CDesignPlateDlg : public CDialog
{
// Construction
public:
	double m_fDianQuanOffset;
	bool m_bAutoCalWash;
	BOOL m_bSpreadState;
	CLDSPart* m_pDatumPart;
	CLDSPlate* m_pBoltPad;
	//
	void CalNormOffset();
	void RelayoutWnd();
	void PreviewBoltPad();
public:
	CDesignPlateDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CDesignPlateDlg)
	enum { IDD = IDD_DESIGN_PLATE_DLG };
	int		m_iMaterial;
	CString	m_sPartNo;
	int		m_iDesStyle;	//0.µÊ∞Â 1.µÊ»¶
	int		m_nThick;
	double	m_fNormOffset;
	BOOL	m_bDatumPartTop;
	int		m_nWasherNum;
	int		m_nWasherThick;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_VIRTUAL
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDesignPlateDlg)
	afx_msg void OnBnClickedDatumPartTop();
	afx_msg void OnEnChangeENormOffset();
	afx_msg void OnEnChangeEThick();
	afx_msg void OnEnChangeEWasher();
	afx_msg void OnBnClickedRdoDianBan();
	afx_msg void OnBnClickedRdoDianQuan();
	afx_msg void OnChangePartNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNPLATEDLG_H__4B7D0D77_B771_40D7_BDA3_B4A4C07D6C30__INCLUDED_)
