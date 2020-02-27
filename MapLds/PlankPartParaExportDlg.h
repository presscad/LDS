#if !defined(AFX_PLANKPARTPARAEXPORTDLG_H__ECF17BC2_7529_4AB5_B1FA_777D468D2417__INCLUDED_)
#define AFX_PLANKPARTPARAEXPORTDLG_H__ECF17BC2_7529_4AB5_B1FA_777D468D2417__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlankPartParaExportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlankPartParaExportDlg dialog
#include "resource.h"
#include "XhCharString.h"
#include "f_ent_list.h"
#include "supergridctrl.h"

class CPlankPartParaExportDlg : public CDialog
{
// Construction
	void UpdateCheckStateByName(const char *sName,BOOL bValue);
	void UpdateCheckBoxEnableState();
public:
	//钢板明细显示参数
	struct TABLE_ROW{
		CXhChar100 sRowName;
		CXhChar100 sDisplayName;
	};
	static BOOL m_bMarkVertex;
	static BOOL m_bMartWeld;
	static CString m_sExportPartInfoKeyStr;
	static ATOM_LIST<TABLE_ROW> m_xTblRowList;
	//
	static BOOL SaveDimStyleToFile(const char* file_path);
	static BOOL LoadDimStyleFromFile(const char* file_path);
public:
	CPlankPartParaExportDlg(CWnd* pParent = NULL);   // standard constructor
#ifndef AFX_TARG_ENU_ENGLISH
	CSuperGridCtrl m_listCtrTblRow;
#endif
// Dialog Data
	//{{AFX_DATA(CPlankPartParaExportDlg)
	enum { IDD = IDD_PLANK_PART_PARA_EXPORT_DLG };
	BOOL	m_bPartMat;
	BOOL	m_bPartNo;
	BOOL	m_bPieceWeight;
	BOOL	m_bPlankThick;
	BOOL	m_bSumWeight;
	BOOL	m_bTaType;
	BOOL	m_bPartSize;
	BOOL	m_bVertexMark;
	BOOL	m_bWeldMark;
	BOOL	m_bPrjName;
	BOOL	m_bTowerTypeAlias;
	BOOL	m_bLsNum;
	CString	m_sEditLine;
	BOOL	m_bStampNo;
	BOOL	m_bPartNum;
	BOOL	m_bPieceArea;
	BOOL	m_bTaNum;	
	BOOL    m_bEdgeNum;
	BOOL	m_bWeldLength;
	BOOL	m_bTrialAssembly;
	BOOL	m_bTolerance;
	BOOL	m_bTaskNo;
	BOOL	m_bHuoQu;
	BOOL	m_bWeldPart;
	BOOL	m_bSumPartNum;
	BOOL	m_bSupportTable;
	BOOL	m_bPromptEveryTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlankPartParaExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPlankPartParaExportDlg)
	afx_msg void OnBnAddEditLine();
	afx_msg void OnBnDelCurrentLine();
	afx_msg void OnBnClearEditLine();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChkPrjName();
	afx_msg void OnChkPrjCode();
	afx_msg void OnChkTaType();
	afx_msg void OnChkPartNo();
	afx_msg void OnChkPartMat();
	afx_msg void OnChkPlankThick();
	afx_msg void OnChkPartSize();
	afx_msg void OnChkPieceWeight();
	afx_msg void OnChkSumWeight();
	afx_msg void OnChkLsNum();
	afx_msg void OnChkStampNo();
	afx_msg void OnChkPartNum();
	afx_msg void OnChkPieceArea();
	afx_msg void OnChkTaNum();
	afx_msg void OnChkEdgeNum();
	afx_msg void OnChkWeldLength();
#ifndef AFX_TARG_ENU_ENGLISH
	afx_msg void OnChkTrialAssembly();
	afx_msg void OnChkTolerance();
	afx_msg void OnChkTaskNo();
	afx_msg void OnChkSumPartNum();
	afx_msg void OnChkWeldPart();
	afx_msg void OnChkHuoQu();
	afx_msg void OnChkSupportTable();
	afx_msg void OnBnAddRow();
	afx_msg void OnBnDelRow();
	afx_msg void OnBnMoveUp();
	afx_msg void OnBnMoveDown();
#endif
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANKPARTPARAEXPORTDLG_H__ECF17BC2_7529_4AB5_B1FA_777D468D2417__INCLUDED_)
