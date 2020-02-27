#if !defined(AFX_ADDSETUPAIDEDPLATEDLG_H__5EF1619E_6786_4A42_8990_995B2F40CDB6__INCLUDED_)
#define AFX_ADDSETUPAIDEDPLATEDLG_H__5EF1619E_6786_4A42_8990_995B2F40CDB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddSetupAidedPlateDlg.h : header file
//
#include "ListViewComparer.h"
#ifdef __ANGLE_PART_INC_

/////////////////////////////////////////////////////////////////////////////
// CAddSetupAidedPlateDlg dialog

struct SETUPAIDED_PLATE_STRU{
	char sPartNo[16];	//钢板编号
	double radius;		//钢管的半径
	double width;		//钢板的宽度
	double angle;		//钢板圆弧夹角
	double start_half_t;//起始钢板半个板厚
	double end_half_t;	//终止钢板半个板厚
	int    profile_style;//钢板外型1为内侧直线2为内侧圆弧
};

class CAddSetupAidedPlateDlg : public CDialog
{
// Construction
	CListViewComparer listViewComparer;
public:
	CAddSetupAidedPlateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddSetupAidedPlateDlg)
	enum { IDD = IDD_ADD_SETUP_AIDED_PLATE_DLG };
	double	m_fAngle;
	CString	m_sEndPlateHandle;
	double	m_fStartPlateHalfT;
	double	m_fEndPlateHalfT;
	CString	m_sStartPlateHandle;
	CString	m_sPlatePartNo;
	int		m_iProfileStyle;
	int		m_tmpHandle;
	double	m_fWidth;
	double	m_fRadius;
	//}}AFX_DATA
	CListCtrl* m_pListCtrl;
	CStringArray m_arrHeaderLabel;
	BOOL m_bPauseBreakExit;	//是否为中途暂时中断选择构件性质的退出
	int m_nPlateNum;
	SETUPAIDED_PLATE_STRU aided_plate[300];
	CLdsPartList DisplayTmpParts;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddSetupAidedPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddSetupAidedPlateDlg)
	afx_msg void OnAddSetupAidedPlate();
	afx_msg void OnAutoStat();
	afx_msg void OnExportList();
	afx_msg void OnImportList();
	afx_msg void OnSelectPart();
	afx_msg void OnModifySetupAidedPlate();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnColumnclickListSetupAidedPlate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListSetupAidedPlate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListSetupAidedPlate(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void ModifyItem(int iCurSel);
	void SelectPart(int nCurSel);
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
	void SetupAidedPlateSerialize(CString sFileName, BOOL bWrite);
public:
	void DisplayTmpModel(CLDSPlate* pStartPlate,CLDSPlate* pEndPlate,CLDSLineTube* pBaseTube,int TmpHandle);
};

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDSETUPAIDEDPLATEDLG_H__5EF1619E_6786_4A42_8990_995B2F40CDB6__INCLUDED_)
