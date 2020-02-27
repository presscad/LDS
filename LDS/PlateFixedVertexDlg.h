#if !defined(AFX_PLATEFIXEDVERTEXDLG_H__24B66744_00DF_4882_B8F7_5AC67A42330F__INCLUDED_)
#define AFX_PLATEFIXEDVERTEXDLG_H__24B66744_00DF_4882_B8F7_5AC67A42330F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlateFixedVertexDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CPlateFixedVertexDlg dialog

class CPlateFixedVertexDlg : public CDialog
{
// Construction
public:
	CPlateFixedVertexDlg(CWnd* pParent = NULL);   // standard constructor
	CDesignLjPara *m_pLjPara;
	CStringArray m_arrHeaderLabel;
// Dialog Data
	//{{AFX_DATA(CPlateFixedVertexDlg)
	enum { IDD = IDD_PLATE_FIXED_VERTEX_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlateFixedVertexDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPlateFixedVertexDlg)
	afx_msg void OnDblclkListPlateFixedVertex(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListPlateFixedVertex(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnAddVertice();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl* m_pListCtrl;
	void ModifyItem(int iCurSel);
	void DeleteItem(int iCurSel);
	void RefreshListViewHeader();
	void AppendRecord(int iItem, CStringArray &str_arr);
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLATEFIXEDVERTEXDLG_H__24B66744_00DF_4882_B8F7_5AC67A42330F__INCLUDED_)
