#if !defined(AFX_PARTNOLISTDLG_H__6D466A9F_E33D_4715_B3E7_04549D2554E3__INCLUDED_)
#define AFX_PARTNOLISTDLG_H__6D466A9F_E33D_4715_B3E7_04549D2554E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartNoListDlg.h : header file
//
#include "supergridctrl.h"
/////////////////////////////////////////////////////////////////////////////
// CPartNoListDlg dialog

#ifdef __SMART_DRAW_INC_
struct PART_NO
{
	long h;
	SEGI iSeg;
	char cPartType;
	char cNoType;	//'N':编新号;'R':重复前一号;'S':指定编号
	char sPartNo[50];
};
class CPartNoListDlg : public CDialog
{
	// Construction
	char m_sLastPartNo[50];
	char file_name[MAX_PATH];
	//得到当前选中项索引范围，包括选中单个构件或成组选中
	BOOL GetCurSelectedItemIndexScope(int iCurSel,int &iStart,int &iEnd);
	//根据当前编号得到该编号组的起始索引及结束索引
	BOOL GetPartNoGroupScope(CString sCurPartNo,int &iStart,int &iEnd);
	void SelectMul(int iCurSel);
	void ContextMenu(class CWnd *,class CPoint);
	void UpdateNoList(CString str);
	void UpdatePartNo();
	int GetCurSelByPartHandle(THANDLE hPart);
	int GetSelectedItemList(ARRAY_LIST<CSuperGridCtrl::CTreeItem*>* pListItems,int* pStartIndex=NULL,int* pEndIndex=NULL);
	void MoveItemListBefore(ARRAY_LIST<CSuperGridCtrl::CTreeItem*>* pListItems,int iInsertBefore);
	CBitmap m_curBitmap1,m_curBitmap2;
public:
	CHashList<PART_NO>m_PartNoRecordList;//临时存储
	void EmptyPartNoList();
	void SelectPart(long hPart);
public:
	CPartNoListDlg(CWnd* pParent = NULL);   // standard constructor
	DECLARE_DYNCREATE(CPartNoListDlg)
// Dialog Data
	//{{AFX_DATA(CPartNoListDlg)
	enum { IDD = IDD_PARTNO_LIST_DLG };
	CButton	m_btnSelect;
	CSuperGridCtrl m_partNoList;
	CString	m_sSegNo;
	BOOL	m_bGroupSel;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartNoListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartNoListDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelectBtn();
	afx_msg void OnRetrievePartNo();
	afx_msg void OnUpdateBtn();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeydownPartnoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickPartnoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickPartnoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnImportPartNo();
	afx_msg void OnBnExportPartNo();
	afx_msg void OnMove();
	afx_msg void OnBnMoveTop();
	afx_msg void OnBnUpMove();
	afx_msg void OnBnDownMove();
	afx_msg void OnBnMoveBottom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTNOLISTDLG_H__6D466A9F_E33D_4715_B3E7_04549D2554E3__INCLUDED_)
