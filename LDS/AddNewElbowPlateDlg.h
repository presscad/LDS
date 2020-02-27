#if !defined(AFX_ADDNEWELBOWPLATEDLG_H__CC848E42_05B9_42FE_8C48_72C95B59224F__INCLUDED_)
#define AFX_ADDNEWELBOWPLATEDLG_H__CC848E42_05B9_42FE_8C48_72C95B59224F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddNewElbowPlateDlg.h : header file
//
#ifdef __ANGLE_PART_INC_

/////////////////////////////////////////////////////////////////////////////
// CAddNewElbowPlateDlg dialog

class CAddNewElbowPlateDlg : public CDialog
{
// Construction
public:
	CAddNewElbowPlateDlg(CWnd* pParent = NULL);   // standard constructor
	CSuperSmartPtr<CLDSPart> m_pDatumPlate;
	CLDSLineTube *m_pDatumTube;
	CBitmap m_CurPlateBmp;
	CStringArray m_arrHeaderLabel;
// Dialog Data
	//{{AFX_DATA(CAddNewElbowPlateDlg)
	enum { IDD = IDD_ADD_NEW_ELBOW_PLATE_DLG };
	double	m_fParaA;
	double	m_fParaB;
	double	m_fParaC;
	double	m_fParaD;
	double	m_fParaE;
	double	m_fParaF;
	double	m_fParaG;
	double	m_fParaH;
	double	m_fParaT;
	double	m_fScopeAngle;
	int		m_nItemsArrayNum;
	int		m_iMaterial;
	double	m_fPosAngle;
	CString	m_sPartNo;
	CString m_sSegI;
	double	m_fTubeOutterR;
	double	m_fFlWidth;
	int		m_iByWeld;	//0.以焊道线为基准 1.以钢管基准边为基准
	BOOL	m_bVertical;
	BOOL	m_bObverse;
	BOOL	m_bInverse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddNewElbowPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddNewElbowPlateDlg)
	afx_msg void OnBtnElbowPlateModify();
	afx_msg void OnBtnAddElbowPlate();
	afx_msg void OnBtnLayoutElbowPlate();
	afx_msg void OnDblclkListElbowPlate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListElbowPlate(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeEElbowPlatePartNo();
	afx_msg void OnBnPartsDb();
	afx_msg void OnChangeItemsArrayNum();
	afx_msg void OnInverse();
	afx_msg void OnObverse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:		
	CListCtrl *m_pListCtrl;
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDNEWELBOWPLATEDLG_H__CC848E42_05B9_42FE_8C48_72C95B59224F__INCLUDED_)
