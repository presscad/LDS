#if !defined(AFX_LINKLINEPARTDLG_H__F78C0C1B_6F5F_411E_A1AA_F12B4A4ABA4C__INCLUDED_)
#define AFX_LINKLINEPARTDLG_H__F78C0C1B_6F5F_411E_A1AA_F12B4A4ABA4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkLinePartDlg.h : header file
//
#include "CallBackDialog.h"
#include "lds_part.h"
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CLinkLinePartDlg dialog

class CLinkLinePartDlg : public CCallBackDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX, nScrLocationY;
public:
	CLinkLinePartDlg(CWnd* pParent = NULL);   // standard constructor
	ATOM_LIST<CDesignLjPartPara>part_list;
	CDesignLjPara *m_pLjPara;
	CStringArray m_arrHeaderLabel;
	long	m_hPlate;			//Á¬½Ó°å
// Dialog Data
	//{{AFX_DATA(CLinkLinePartDlg)
	enum { IDD = IDD_LINK_LINEPART_DLG };
	//CListCtrl	m_pListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkLinePartDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkLinePartDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclkListLinkLinepart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListLinkLinepart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnAddLinePart();
	afx_msg void OnClickListLinePartset(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl* m_pListCtrl;
	void ModifyItem(int iCurSel);
	void DeleteItem(int iCurSel);
	void RefreshListViewHeader();
	void AppendRecord(int iItem, CStringArray &str_arr);
	void SelectPart(int nCurSel);
	BOOL FinishSelectObjOper(long hPickObj, long idEventProp);
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKLINEPARTDLG_H__F78C0C1B_6F5F_411E_A1AA_F12B4A4ABA4C__INCLUDED_)
