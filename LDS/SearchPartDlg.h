#pragma once
#include "afxwin.h"
#include "XhListCtrl.h"
#include "afxcmn.h"

// CSearchPartDlg 对话框

class CSearchPartDlg : public CDialog
{
	DECLARE_DYNAMIC(CSearchPartDlg)
	bool m_bEnableWindowMoveListen;
	bool m_bHideListCtrl;
	long m_hDatumSelPart;
	CTabCtrl m_tabSearchType;
	static int nScrLocationX,nScrLocationY;
public:
	CString m_sSearchText;		//
	BOOL m_bSearchResult;		//
	CXhListCtrl m_listCtrl;
	CString m_sPromptInfo;
public:
	void ResetDlgSize();
	void InitListCtrl(long hDatumSelPart=0);
	void HideListCtrl();
	void SpreadListCtrl();
	void SearchPart(BOOL bColseDialog=TRUE,int bySpecMode=-1);
	void SelectPart(int nCurSel);
	void ContextMenu(CWnd *pWnd, CPoint point);
	BOOL Create();
	void InitSearchPartDlg();
	void SetPromptInfo(const char* sValue);
	//void InitLsRefDlg(CLDSPart *pWorkPart,int s1_e2_m0=0,BOOL bPlateAttachPart=FALSE);
public:
	CSearchPartDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSearchPartDlg();

// 对话框数据
	enum { IDD = IDD_PART_SEARCH_DLG };

protected:
	void AddSearchResultObjToListCtrl(CLDSDbObject* pDbObj);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSearchPart();
	afx_msg void OnExpandSearchResult();
	afx_msg void OnNMRClickListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelectItem();
	afx_msg void OnNMClickListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnCheckPartno();
	afx_msg void OnBnClickedBnUpdateSameLabelPartsRelaProperty();
	afx_msg void OnTcnSelchangeTabSearchType(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) ;
	afx_msg void OnEnChangeEPartSearchText();
	afx_msg void OnBnClickedBnSelectAll();
	afx_msg void OnNMClickTabSearchType(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};
extern CSearchPartDlg *g_pSearchPartDlg;
