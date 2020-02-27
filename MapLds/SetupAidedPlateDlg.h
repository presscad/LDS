#if !defined(AFX_SETUPAIDEDPLATEDLG_H__88C77E15_E1C2_4C07_A5C1_1D1616E3FC51__INCLUDED_)
#define AFX_SETUPAIDEDPLATEDLG_H__88C77E15_E1C2_4C07_A5C1_1D1616E3FC51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupAidedPlateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupAidedPlateDlg dialog
#include "resource.h"
#include "ListViewComparer.h"

//¹¤×°¸¨Öú¸Ö°å
struct AIDEDPLATE_ITEM{
	char sPartNo[16];	//±àºÅ
	double radius;		//¸Ö¹Ü°ë¾¶
	double width;		//¸Ö°åºñ¶È
	double angle;		//¼Ð½Ç
	double start_half_t;//ÆðÊ¼¸Ö°å°ë¸ö°åºñ
	double end_half_t;  //ÖÕÖ¹¸Ö°å°ë¸ö°åºñ
	int  profile_style;//¸Ö°åÑùÊ½
};
class CSetupAidedPlateDlg : public CDialog
{
// Construction
	CListViewComparer listViewComparer;
public:
	int m_nPlateNum;
	AIDEDPLATE_ITEM aided_plate[300];
	CSetupAidedPlateDlg(CWnd* pParent = NULL);   // standard constructor
	CStringArray m_arrHeaderLabel;
	CListCtrl *m_pListCtrl;
// Dialog Data
	//{{AFX_DATA(CSetupAidedPlateDlg)
	enum { IDD = IDD_SETUP_AIDED_PLATE_DLG };
	double	m_fRadius;
	double	m_fStartPlateHalfT;
	CString	m_sSetupAidedPlatePartNo;
	double	m_fEndPlateHalfT;
	double	m_fAngle;
	CString	m_sAidedPlateFilePath;
	int 	m_iProfileStyle;
	double	m_fWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupAidedPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupAidedPlateDlg)
	afx_msg void OnRebuildPartset();
	afx_msg void OnModifySetupAidedPlate();
	afx_msg void OnBrowseFile();
	afx_msg void OnAddSetupAidedPlate();
	virtual void OnOK();
	afx_msg void OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusAidedPlateFilePath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ModifyItem(int iCurSel);
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
	void SetupAidedPlateSerialize(CString sFileName, BOOL bWrite);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPAIDEDPLATEDLG_H__88C77E15_E1C2_4C07_A5C1_1D1616E3FC51__INCLUDED_)
