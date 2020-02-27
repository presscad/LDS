#if !defined(AFX_PARTSDBDLG_H__3EAA8728_1CE8_4A56_9331_DEA738A9E8CE__INCLUDED_)
#define AFX_PARTSDBDLG_H__3EAA8728_1CE8_4A56_9331_DEA738A9E8CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartsDbDlg.h : header file
//
#include "ListViewComparer.h"
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CPartsDbDlg dialog

class CPartsDbDlg : public CDialog
{
	struct STDPART_ITEM{
		CLDSPart *pPart;
		int num;
	};
	ATOM_LIST<STDPART_ITEM> partList;
	CListViewComparer listViewComparer;
// Construction
public:
	BOOL m_bRefreshDlgCtrl;
	void RefreshPartsDb();
	DWORD m_dwSelectPartsType;
	CPartsDbDlg(CWnd* pParent = NULL);   // standard constructor
	CStringArray m_arrHeaderLabel;
	CListCtrl *m_pListCtrl;
	CLDSPart *m_pSelectPart;
	void RefreshListViewHeader();
	void RefreshDlgCtrlState();
	void ReadLocalStdPartLibary();
	int AppendRecord(int iItem, CStringArray &str_arr);

// Dialog Data
	//{{AFX_DATA(CPartsDbDlg)
	enum { IDD = IDD_PARTS_DB_DLG };
	BOOL	m_bIncPlate;
	BOOL	m_bIncParamUend;
	BOOL	m_bIncParamTubeRib;
	BOOL	m_bIncParamRollEnd;
	BOOL	m_bIncParamElbow;
	BOOL	m_bIncParamFl;
	BOOL	m_bIncParamCirRib;
	BOOL	m_bIncParamCirLid;
	BOOL	m_bIncParamXEnd;
	BOOL	m_bIncParamAngleRib;
	BOOL	m_bIncParamRibPlate;
	BOOL	m_bIncParamWaterPlate;
	CString m_sSegStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartsDbDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartsDbDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChkTypeModify();
	afx_msg void OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeSegStr();
	afx_msg void OnBnClickedBtnExportPart();	//
	afx_msg void OnBnClickedBtnImportPart();	//
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTSDBDLG_H__3EAA8728_1CE8_4A56_9331_DEA738A9E8CE__INCLUDED_)
