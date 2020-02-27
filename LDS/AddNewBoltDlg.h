#if !defined(AFX_ADDNEWBOLTDLG_H__85F3E347_6BC4_4855_B565_6A7B69C4CA74__INCLUDED_)
#define AFX_ADDNEWBOLTDLG_H__85F3E347_6BC4_4855_B565_6A7B69C4CA74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifdef __ANGLE_PART_INC_
// AddNewBoltDlg.h : header file
//
#include "lds_part.h"
#include "ListViewComparer.h"
/////////////////////////////////////////////////////////////////////////////
// CAddNewBoltDlg dialog

class CAddNewBoltDlg : public CDialog
{
// Construction
public:
	CAddNewBoltDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSPart* m_pWorkPart;
	CLDSLineTube *m_pDatumTube;
	CBitmap m_CurPlateBmp;
	CStringArray m_arrHeaderLabel;
	CListViewComparer listViewComparer;
	//ATOM_LIST<CDesignPlateBoltPara> designBoltList;
	//ATOM_LIST<CDesignLsPara> *m_pLsList;
// Dialog Data
	//{{AFX_DATA(CAddNewBoltDlg)
	enum { IDD = IDD_ADD_NEW_BOLT_DLG };
	CComboBox	m_cmbBoltSpec;
	double	m_fBoltPosX;
	double	m_fBoltPosY;
	double	m_fBoltPosZ;
	double	m_fBoltR;
	double	m_fStartAngle;
	double	m_fHorzSpace;
	double	m_fVertSpace;
	double	m_fScopeAngle;
	double	m_fFlWidth;
	double	m_fTubeR;
	int		m_nItemsArrayNum;
	CString	m_sBoltSpec;
	int		m_iRectArray;
	int		m_iBoltNorm;	//FL: 0.以焊道线为基准 1.以钢管基准边为基准  其他:0.螺栓朝外 1.螺栓朝里
	CString	m_sSegNo;
	int		m_iRowNum;
	int		m_iColumnNum;
	BOOL	m_bDoubleRow;
	double	m_fSpecSpaceD;
	BOOL	m_bStaggerLayout;
	BOOL	m_bMirLayoutBolt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddNewBoltDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddNewBoltDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnBoltModify();
	afx_msg void OnBnAddBolt();
	afx_msg void OnDblclkListBolt(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListBolt(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnBtnLayoutBolt();
	afx_msg void OnChangeEItemsArrayNum();
	afx_msg void OnChangeEScopeAngle();
	afx_msg void OnChangeRowNum();
	afx_msg void OnChangeColumnNum();
	afx_msg void OnRectArray();
	afx_msg void OnRoundArray();
	afx_msg void OnColumnclickListBolt(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeVertSpace();
	afx_msg void OnDoubleRow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL AddLsRefToList(CLsRef *pLsRef);
	CListCtrl *m_pListCtrl;
	void RefreshDlgCtrlState();
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
	void CreateBolt(CLDSPart *pPart=NULL);
	void DeleteItem(int nCurSel);
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDNEWBOLTDLG_H__85F3E347_6BC4_4855_B565_6A7B69C4CA74__INCLUDED_)
