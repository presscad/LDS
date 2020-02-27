#if !defined(AFX_FLBILLDLG_H__039BC77A_F06A_4780_B911_6BD128119289__INCLUDED_)
#define AFX_FLBILLDLG_H__039BC77A_F06A_4780_B911_6BD128119289__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FLBillDlg.h : header file
//

#if defined(__TSA_) || defined(__LDS_)
#include "ListViewComparer.h"
#include "XhListCtrl.h"
#include "FLSizeOptimizor.h"
/////////////////////////////////////////////////////////////////////////////
// CFLBillDlg dialog

class CFLBillDlg : public CDialog
{
	CFLSizeOptimizor flOpt;
// Construction
public:
	CFLBillDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFLBillDlg)
	enum { IDD = IDD_BILL_OF_FL_DLG };
	CXhListCtrl	m_listCtrl;
	CString	m_sIncSegStr;
	int		m_iMat;
	CString	m_sTubeGuige;
	int		m_iBoltGrade;
	double	m_fAxisForce;
	double	m_fAxisMoment;
	double	m_fBoltCirDia;
	int		m_nBoltNum;
	CString	m_sFlNo;
	double	m_fFlDia;
	int		m_nFlThick;
	int		m_nRibHeight;
	int		m_iBoltD;
	int		m_nCoupleFlThick;
	CString	m_sCoupleFlNo;
	int		m_nCoupleRibHeight;
	BOOL	m_bSpecBoltNum;
	BOOL	m_bSpecFlDia;
	BOOL	m_bSpecRibHeight;
	BOOL	m_bSpecRibThick;
	BOOL	m_bSpecBoltCir;
	int		m_iFlMat;
	int		m_nRibThick;
	double	m_fCoupleSumWeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFLBillDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	void ContextMenu(CWnd *pWnd, CPoint point);
	void RefreshItemCtrlValue(int iItem);
	void RefreshListViewItem(CJointCouple* pJoint,int iItem);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFLBillDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDesignCommonFl();
	afx_msg void OnAutoMatchPWeldFl();
	afx_msg void OnAutoMatchDWeldFl();
	afx_msg void OnRclickTubeFlListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCmbTubeMaterial();
	afx_msg void OnSelchangeCmbTubeGuige();
	afx_msg void OnChangeEIncSegStr();
	afx_msg void OnItemchangedTubeFlListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnFlSave();
	afx_msg void OnBnFlRedesignHasRib();
	afx_msg void OnBnFlRedesignNoneRib();
	afx_msg void OnBnFlRedesignAll();
	afx_msg void OnChkSpecPara();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLBILLDLG_H__039BC77A_F06A_4780_B911_6BD128119289__INCLUDED_)
