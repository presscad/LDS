#if !defined(AFX_DEFPARTGROUPDLG_H__5DFE0C22_E13A_41BA_A84C_0978B88BCCDB__INCLUDED_)
#define AFX_DEFPARTGROUPDLG_H__5DFE0C22_E13A_41BA_A84C_0978B88BCCDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
// PartGroupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartGroupDlg dialog


class CPartGroupDlg : public CDialog
{
// Construction
public:
	CPartGroupDlg(CWnd* pParent = NULL);   // standard constructor
	UCS_STRU ucs;
	CLDSPartGroup *m_pPartGroup;	//NULL表示为新建 !NULL表示编辑已存在的组焊件
	long m_hOldRootPart;	//记录基准构件句柄 若基准构件句柄发生变化需要更新组焊件视图
	BOOL m_bCopyAndNew;		//表示当前操作为克隆当前选中的组焊件并新建组焊件 wht 10-11-08
// Dialog Data
	//{{AFX_DATA(CPartGroupDlg)
	enum { IDD = IDD_DEF_PARTGROUP_DLG };
	CString	m_sPartGroupName;
	double	m_fAxisX_X;
	double	m_fAxisX_Y;
	double	m_fAxisX_Z;
	double	m_fAxisY_X;
	double	m_fAxisY_Y;
	double	m_fAxisY_Z;
	double	m_fAxisZ_X;
	double	m_fAxisZ_Y;
	double	m_fAxisZ_Z;
	double	m_fOriginPosX;
	double	m_fOriginPosY;
	double	m_fOriginPosZ;
	BOOL	m_bCanModifyAxisX;
	BOOL	m_bCanModifyAxisY;
	BOOL	m_bCanModifyAxisZ;
	UINT	m_uStatNum;
	CString	m_sFolderName;
	CString	m_sRootPartHandle;
	BOOL	m_bTowerFoot;
	CString m_sPartGroupNum;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartGroupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartGroupDlg)
	afx_msg void OnBnPasteAxisX();
	afx_msg void OnBnPasteAxisY();
	afx_msg void OnBnPasteAxisZ();
	afx_msg void OnBnPasteOriginPos();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnCopyAxisX();
	afx_msg void OnBnCopyAxisY();
	afx_msg void OnBnCopyAxisZ();
	afx_msg void OnBnCopyOriginPos();
	afx_msg void OnBnCpyUcs();
	afx_msg void OnBnPasteUcs();
	afx_msg void OnBnToRightHand();
	afx_msg void OnChkEnableAxisX();
	afx_msg void OnChkEnableAxisY();
	afx_msg void OnChkEnableAxisZ();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelchangeRootPartHandle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateUcs(BOOL bFlag=TRUE);
};

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFPARTGROUPDLG_H__5DFE0C22_E13A_41BA_A84C_0978B88BCCDB__INCLUDED_)
