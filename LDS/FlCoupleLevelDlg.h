#if !defined(AFX_FLCOUPLELEVELDLG_H__D6509031_08E7_4B8C_B0BC_FA3F67E2D011__INCLUDED_)
#define AFX_FLCOUPLELEVELDLG_H__D6509031_08E7_4B8C_B0BC_FA3F67E2D011__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlCoupleLevelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlCoupleLevelDlg dialog

class CFlCoupleLevelDlg : public CDialog
{
// Construction
public:
	CFlCoupleLevelDlg(CWnd* pParent = NULL);   // standard constructor
	int AppendRecord(CListCtrl *pListCtrl, int iItem, CStringArray &str_arr);
	void RefreshListViewHeader();
	void RefreshListView();
	void DeleteItem(CListCtrl *pListCtrl,int nCurSel);
// Dialog Data
	//{{AFX_DATA(CFlCoupleLevelDlg)
	enum { IDD = IDD_FL_COUPLE_LEVEL_DLG };
	double	m_fParamA;
	double	m_fParamB;
	double	m_fParamC;
	double	m_fParamD;
	double	m_fMainTubeD;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlCoupleLevelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFlCoupleLevelDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnImportLibFile();
	afx_msg void OnExportLibFile();
	afx_msg void OnAddItem();
	afx_msg void OnModifyItem();
	afx_msg void OnDblclkFlCoupleLevelList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownFlCoupleLevelList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeParamA();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLCOUPLELEVELDLG_H__D6509031_08E7_4B8C_B0BC_FA3F67E2D011__INCLUDED_)
