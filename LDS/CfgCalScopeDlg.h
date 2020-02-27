#if !defined(AFX_CFGCALSCOPEDLG_H__361B1087_35BE_49F1_A400_20916303FB89__INCLUDED_)
#define AFX_CFGCALSCOPEDLG_H__361B1087_35BE_49F1_A400_20916303FB89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if defined(__LDS_)||defined(__TSA_)
// CfgCalScopeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCfgCalScopeDlg dialog

class CCfgCalScopeDlg : public CDialog
{
// Construction
public:
	CCfgCalScopeDlg(CWnd* pParent = NULL);   // standard constructor
	CString m_sModuleScope;		//模型号字符串
	CString m_sWorkStatusScope;	//工况字符串
	enum CAL_MODE{
		MYL_CAL = 0,	//满应力优选
		UNI_CAL = 1,	//单向继承优选
		CHK_CAL = 2,	//模型验算
		CHK_DEFORM = 3,	//变形验算
	};
	int m_iCalMode;	//0.满应力优选；1.常规验算；2.抗震验算
	BOOL m_bUpdateBoltInfo;
	BOOL m_bVerifyFoundationMode;
	BOOL m_iFoundationByStdLoad0;

// Dialog Data
	//{{AFX_DATA(CCfgCalScopeDlg)
	enum { IDD = IDD_CFG_CAL_SCOPE_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCfgCalScopeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateFoundationVerifyModeUI();
	// Generated message map functions
	//{{AFX_MSG(CCfgCalScopeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnAddModule();
	afx_msg void OnBnDelModule();
	afx_msg void OnDblclkListAttachModule();
	afx_msg void OnDblclkListRemainModule();
	afx_msg void OnBnCalMode();
	afx_msg void OnClickedChkFoundationMode();
	afx_msg void OnBnClickedChkUpdateBoltInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFGCALSCOPEDLG_H__361B1087_35BE_49F1_A400_20916303FB89__INCLUDED_)
