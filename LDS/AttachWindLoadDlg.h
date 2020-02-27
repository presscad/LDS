#if !defined(AFX_ATTACHWINDLOADDLG_H__67F4A949_E233_4BE1_8B13_7E148F8B9CF5__INCLUDED_)
#define AFX_ATTACHWINDLOADDLG_H__67F4A949_E233_4BE1_8B13_7E148F8B9CF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(__LDS_)||defined(__TSA_)
// AttachWindLoadDlg.h : header file
//
typedef struct tagNODE_POINTI
{
	long h,point_i;
	tagNODE_POINTI(){;}
	tagNODE_POINTI(long handle,long pointI){h=handle;point_i=pointI;}
}NODE_POINTI;
/////////////////////////////////////////////////////////////////////////////
// CAttachWindLoadDlg dialog
#include "CallBackDialog.h"
class CAttachWindLoadDlg : public CCallBackDialog
{
// Construction
	CStringArray m_arrHeaderLabel;
public:
	BOOL m_bWorkStatusMode;	//TRUE:修改附加荷载在指定工况中的挡风系数模式;FALSE:修改全塔附加荷载参数模式
	CAttachWindLoadDlg(CWnd* pParent = NULL);   // standard constructor
	CArray<NODE_POINTI,NODE_POINTI>node_arr;

// Dialog Data
	//{{AFX_DATA(CAttachWindLoadDlg)
	enum { IDD = IDD_ATTACH_WIND_LOAD_DLG };
	CString	m_sAttachWindLoadName;
	double	m_fShieldArea;
	double	m_fShieldCoef;
	double	m_fProfileCoef;
	double	m_fAttachWeight;
	double	m_fBetaZ;
	CString	m_hNode;
	int		m_nPointI;
	double	m_fBetaZForFoundation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttachWindLoadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAttachWindLoadDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnAddLoadNode();
	afx_msg void OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnModifyLoadNode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl* m_pListCtrl;
	bool CheckNodeValid(long h, int point_i);
	void ModifyItem(int iCurSel);
	void DeleteItem(int iCurSel);
	void RefreshListViewHeader();
	void AppendRecord(int iItem, CStringArray &str_arr);
public:
	afx_msg void OnBnClickedBnAddItemRecord();
};
/////////////////////////////////////////////////////////////////////////////
// CSelAttachWindLoadDlg dialog

class CSelAttachWindLoadDlg : public CDialog
{
// Construction
public:
	CHashList<long> hashIncWindLoads;
	CSelAttachWindLoadDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelAttachWindLoadDlg)
	enum { IDD = IDD_SEL_ATTACH_WIND_LOAD_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelAttachWindLoadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelAttachWindLoadDlg)
	afx_msg void OnBnAddAttachWindload();
	afx_msg void OnBnDelAttachWindload();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnViewProp();
	afx_msg void OnBnDeleteItem();
	afx_msg void OnBnAddAttachWindloadBycopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTACHWINDLOADDLG_H__67F4A949_E233_4BE1_8B13_7E148F8B9CF5__INCLUDED_)
