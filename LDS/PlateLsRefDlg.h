#if !defined(AFX_PLATELSREFDLG_H__5CA096AF_B1F2_4942_8C69_F92B1CDC8E62__INCLUDED_)
#define AFX_PLATELSREFDLG_H__5CA096AF_B1F2_4942_8C69_F92B1CDC8E62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlateLsRefDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlateLsRefDlg dialog
#ifndef __TSA_

class CPlateLsRefDlg : public CDialog
{
// Construction
	PARTSET oldSelectPartSet;	//上次所选中的螺栓以及螺栓所连接的构件
public:
	CPlateLsRefDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Create();
	void InitLsRefDlg(CLDSPart *pWorkPart,int s1_e2_m0=0,BOOL bPlateAttachPart=FALSE);
	BOOL m_bPlateAttachPart;
	CLDSPart *m_pWorkPart;
	CStringArray m_arrHeaderLabel;
	int start1_end2_mid0;
	CWnd* m_pWnd;
// Dialog Data
	//{{AFX_DATA(CPlateLsRefDlg)
	enum { IDD = IDD_PLATE_LS_REF_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlateLsRefDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPlateLsRefDlg)
	afx_msg void OnClickListPlateLsRef(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListPlateLsRef(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListPlateLsRef(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnModifyLsRefRayNo();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl* m_pListCtrl;
	void SelectPart(int iCurSel);
	BOOL DeleteItem(int iCurSel);
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
};
extern CPlateLsRefDlg *g_pLsRefDlg;
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLATELSREFDLG_H__5CA096AF_B1F2_4942_8C69_F92B1CDC8E62__INCLUDED_)
