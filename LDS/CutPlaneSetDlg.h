#if !defined(AFX_CUTPLANESETDLG_H__04C79FFA_BFCC_4961_A5FD_9C99250F085A__INCLUDED_)
#define AFX_CUTPLANESETDLG_H__04C79FFA_BFCC_4961_A5FD_9C99250F085A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CutPlaneSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCutPlaneSetDlg dialog
typedef struct tagWORKPLANE{
	f3dPoint planeOrigin;
	f3dPoint planeNorm;
}WORKPLANE;
class CCutPlaneSetDlg : public CDialog
{
// Construction
public:
	CCutPlaneSetDlg(CWnd* pParent = NULL);   // standard constructor
	ATOM_LIST<WORKPLANE>planeList;

	CStringArray m_arrHeaderLabel;
// Dialog Data
	//{{AFX_DATA(CCutPlaneSetDlg)
	enum { IDD = IDD_CUT_PLANE_SET_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCutPlaneSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCutPlaneSetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListCutPlane(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListCutPlane(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnAddItem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl* m_pListCtrl;
	void ModifyItem(int iCurSel);
	void DeleteItem(int iCurSel);
	void RefreshListViewHeader();
	void AppendRecord(int iItem, CStringArray &str_arr);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUTPLANESETDLG_H__04C79FFA_BFCC_4961_A5FD_9C99250F085A__INCLUDED_)
