#if !defined(AFX_TUBEENDLJGUIGETBLDLG_H__5CD79A19_F571_42CC_AD98_32D6C4E436F0__INCLUDED_)
#define AFX_TUBEENDLJGUIGETBLDLG_H__5CD79A19_F571_42CC_AD98_32D6C4E436F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TubeEndLjGuiGeTblDlg.h : header file
//
#include "PartLib.h"

/////////////////////////////////////////////////////////////////////////////
// CTubeEndLjGuiGeTblDlg dialog

class CTubeEndLjGuiGeTblDlg : public CDialog
{
// Construction
public:
	CTubeEndLjGuiGeTblDlg(CWnd* pParent = NULL);   // standard constructor
	int AppendRecord(CListCtrl *pListCtrl, int iItem, CStringArray &str_arr);
	void RefreshListViewHeader();
	void RefreshListView();
	void DeleteItem(CListCtrl *pListCtrl,int nCurSel);
	void UpdateInsertPlateName();
	CBitmap m_CurPlateBmp;
// Dialog Data
	//{{AFX_DATA(CTubeEndLjGuiGeTblDlg)
	enum { IDD = IDD_TUBE_END_LJ_GUIGE_TBL_DLG };
	double	m_fParaA;
	double	m_fParaR;
	double	m_fParamL;
	double	m_fParamM;
	double	m_fParamH;
	double	m_fParamW;
	double	m_fParamN;
	double	m_fParamS;
	double	m_fHorizSpace;
	double	m_fVertiSpace;
	double	m_fBoltPosX;
	double	m_fBoltPosY;
	double	m_fWeight;
	CString m_sBoltD;
	CString	m_sInsertPlateName;
	CString	m_sMainTubeGuiGe;
	int		m_nParamT;
	int		m_iInsertPlateType;
	int		m_iInsertPlateMaterial;
	int		m_nBoltNum;
	int		m_nRowNum;
	int		m_nColNum;
	BOOL	m_bDoubleRow;
	int		m_iInsertPlateLevel;	//插板级别 可用来表示钢管应力系数 或用来区分同一规格的钢管上连接的不同类型的插板
	int		m_iTubeMaterial;
	CString	m_sKPlateNo;
	CString	m_sLPlateNo;
	CString	m_sZPlateNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTubeEndLjGuiGeTblDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTubeEndLjGuiGeTblDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnImportInsertPlateFile();
	afx_msg void OnExportInsertPlateFile();
	afx_msg void OnAddNewBoltItem();
	afx_msg void OnModifyBoltItem();
	afx_msg void OnAddNewItem();
	afx_msg void OnModifyItem();
	afx_msg void OnChangeParamN();
	afx_msg void OnSelchangeInsertPlateType();
	afx_msg void OnDblclkLayOutBoltParaList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownLayOutBoltParaList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkInsertPlateLibList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownInsertPlateLibList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeTubeMaterial();
	afx_msg void OnSelchangeMainTubeGuige();
	afx_msg void OnDoubleRowLs();
	afx_msg void OnKillfocusMainTubeGuige();
	afx_msg void OnSelchangeInsertPlateLevel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TUBEENDLJGUIGETBLDLG_H__5CD79A19_F571_42CC_AD98_32D6C4E436F0__INCLUDED_)
