#if !defined(AFX_PLATEDESIGNPARAMDLG_H__E5203B00_AD07_4B2B_8A84_98BB79FF62E7__INCLUDED_)
#define AFX_PLATEDESIGNPARAMDLG_H__E5203B00_AD07_4B2B_8A84_98BB79FF62E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlateDesignParamDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CPlateDesignParamDlg dialog

class CPlateDesignParamDlg : public CDialog
{
// Construction
	CListViewComparer listViewComparer;
public:
	BOOL m_bAddItem;
	CPlateDesignParamDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSPart* m_pWorkPart;
	CBitmap m_CurPlateBmp;
	CStringArray m_arrHeaderLabel;
// Dialog Data
	//{{AFX_DATA(CPlateDesignParamDlg)
	enum { IDD = IDD_PLATE_DESIGN_PARAM_DLG };
	CString	m_sParamName;
	CString	m_sParamNotes;
	CString	m_sParamValue;
	int		m_iDataType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlateDesignParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPlateDesignParamDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListPlateDesignParam(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListPlateDesignParam(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnModify();
	virtual void OnOK();
	afx_msg void OnBtnAddItem();
	afx_msg void OnColumnclickListPlateDesignParam(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl* m_pListCtrl;
	void DeleteItem(int iCurSel);
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLATEDESIGNPARAMDLG_H__E5203B00_AD07_4B2B_8A84_98BB79FF62E7__INCLUDED_)
