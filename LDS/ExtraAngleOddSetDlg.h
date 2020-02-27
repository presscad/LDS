#if !defined(AFX_EXTRAANGLEODDSETDLG_H__A39ACBD4_D451_4AEB_BC4F_D571442D58BD__INCLUDED_)
#define AFX_EXTRAANGLEODDSETDLG_H__A39ACBD4_D451_4AEB_BC4F_D571442D58BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtraAngleOddSetDlg.h : header file
//
#include "PropertyList.h"
#include "CallBackDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CExtraAngleOddSetDlg dialog
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
class CExtraAngleOddSetDlg : public CCallBackDialog
{
	EXTRA_ANGLEODD* m_pExtraOdd;
	EXTRA_ANGLEODD oldExtraOdd;
// Construction
public:
	void UpdateDatumPointStyle(CPropTreeItem *pParent,CLDSPoint *pPoint,BOOL bUpdate);
	void UpdateExtraOddDisplayValue();
	EXTRA_ODDINFO* GetCurExtraOddInfo();
	CLDSLineAngle *m_pAngle;
	int start0_end1;
	CExtraAngleOddSetDlg(CWnd* pParent = NULL);   // standard constructor
	void DisplayPropertyList();
	//
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	static long GetPropID(char* propStr);			//得到属性ID
// Dialog Data
	//{{AFX_DATA(CExtraAngleOddSetDlg)
	enum { IDD = IDD_EXTRA_ANGLEODD_SET_DLG };
	CTabCtrl	m_tabHeader;
	CPropertyList	m_propList;
	int		m_iOddCalStyle;
	double	m_fExtraOdd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtraAngleOddSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions...
	//{{AFX_MSG(CExtraAngleOddSetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTabHeader(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCmbOddCalStyle();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateOddStyle(int iTabSel);
	void FinishSelectObjOper();
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTRAANGLEODDSETDLG_H__A39ACBD4_D451_4AEB_BC4F_D571442D58BD__INCLUDED_)
