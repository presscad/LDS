#if !defined(AFX_BATCHCREATEPARTDLG_H__9702A992_886A_4CFF_BE81_D3D081B4BEA3__INCLUDED_)
#define AFX_BATCHCREATEPARTDLG_H__9702A992_886A_4CFF_BE81_D3D081B4BEA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BatchCreatePartDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

#include "ListViewComparer.h"
/////////////////////////////////////////////////////////////////////////////
// CBatchCreatePartDlg dialog

class CBatchCreatePartDlg : public CDialog
{
	CStringArray m_arrHeaderLabel;
	CListCtrl *m_pListCtrl;
	CListViewComparer listViewComparer;
// Construction
public:
	CBatchCreatePartDlg(CWnd* pParent = NULL);   // standard constructor
	void RefreshTubeList();
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
	CLDSParamPlate *CloneParamPlate(CLDSLineTube *pLineTube,CLDSParamPlate *pSrcParamPlate,BOOL bStartEnd,BOOL bAutoCalcOddment=FALSE);
	void ModfiyItemInfo(int iCurSel);
// Dialog Data
	//{{AFX_DATA(CBatchCreatePartDlg)
	enum { IDD = IDD_BATCH_CREATE_PART_DLG };
	BOOL	m_bUseGuige;
	BOOL	m_bUsePartNo;
	int		m_iCreatePartType;
	int		m_iEndLj;
	int		m_iTubeGuige;
	int		m_iTubeMaterial;
	double	m_fEndOddment;
	double	m_fStartOddment;
	long	m_hTubeHandle;
	CString	m_sTubePartNo;
	CString	m_sTubeSegStr;
	CString	m_sTubeGuiGe;
	CString m_sEndLj;
	BOOL	m_bAutoCalcOddment;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchCreatePartDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBatchCreatePartDlg)
	afx_msg void OnChkUsePartNo();
	afx_msg void OnChkUseGuige();
	afx_msg void OnModfiyItem();
	afx_msg void OnAddItem();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeTubeSegStr();
	afx_msg void OnChangeTubePartNo();
	afx_msg void OnSelchangeTubeGuige();
	afx_msg void OnSelchangeTubeMaterial();
	afx_msg void OnColumnclickTubeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownTubeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTubeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAutoCalcOddment();
	afx_msg void OnUpdateTubeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCHCREATEPARTDLG_H__9702A992_886A_4CFF_BE81_D3D081B4BEA3__INCLUDED_)
