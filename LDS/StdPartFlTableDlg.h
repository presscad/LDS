#if !defined(AFX_STDPARTFLTABLEDLG_H__111AA9DE_0005_4A18_B198_6A291DDA637B__INCLUDED_)
#define AFX_STDPARTFLTABLEDLG_H__111AA9DE_0005_4A18_B198_6A291DDA637B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StdPartFlTableDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStdPartFlTableDlg dialog

class CStdPartFlTableDlg : public CDialog
{
// Construction
	BOOL m_bModify;	//记录数据是否被修改
public:
	CStdPartFlTableDlg(CWnd* pParent = NULL);   // standard constructor
	int AppendRecord(CListCtrl *pListCtrl, int iItem, CStringArray &str_arr);
	void RefreshListViewHeader();
	void RefreshListView();
	void DeleteItem(CListCtrl *pListCtrl,int nCurSel);
	void UpdateFlName();
	BOOL DoSave();
	CBitmap m_CurPlateBmp;
// Dialog Data
	//{{AFX_DATA(CStdPartFlTableDlg)
	enum { IDD = IDD_STD_PART_FL_TABLE_DLG };
	double	m_fFlParaA;
	double	m_fFlParaB;
	double	m_fFlParaK;
	double	m_fFlParaD;
	double	m_fFlParaN;
	double	m_fFlParaS;
	double	m_fFlParaR;
	double	m_fFlParaC;
	double	m_fFlParaH;
	double	m_fFlParaH1;
	double	m_fFlParaL;
	double	m_fWeight;
	CString	m_sBoltD;
	CString	m_sFlName;
	CString m_sFlLevel;
	CString	m_sMainTubeGuiGe;
	int		m_iPartType;
	int		m_iFlMaterial;
	int		m_iFlBoltNum;
	double	m_fFlParaSIncrement;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStdPartFlTableDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStdPartFlTableDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnImportFlParamLibFile();
	afx_msg void OnExportFlParamLibFile();
	afx_msg void OnFlCoupleLevel();
	afx_msg void OnAddItem();
	afx_msg void OnModifyItem();
	afx_msg void OnSelchangePartType();
	afx_msg void OnSelchangeFlLevel();
	afx_msg void OnSelchangeMainTubeGuige();
	afx_msg void OnDblclkStdPartFlLibList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownStdPartFlLibList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusMainTubeGuige();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDPARTFLTABLEDLG_H__111AA9DE_0005_4A18_B198_6A291DDA637B__INCLUDED_)
