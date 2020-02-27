#if !defined(AFX_LAYTUBEBOLTDLG_H__BE061B03_6422_4F58_B3E5_AE196B27C900__INCLUDED_)
#define AFX_LAYTUBEBOLTDLG_H__BE061B03_6422_4F58_B3E5_AE196B27C900__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayAngleBoltDlg.h : header file
//
#include "CallBackDialog.h"
#include "afxcmn.h"
#include "XhListCtrl.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
#ifdef __COMPLEX_PART_INC_

// CLayTubeBoltDlg 对话框

class CLayTubeBoltDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CLayTubeBoltDlg)

public:
	CLayTubeBoltDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLayTubeBoltDlg();

	// Construction
public:	//相关设计参数
	long m_hPlate;
	CLDSLineTube *m_pTube;
	CLDSNode *m_pNode;
	f3dPoint viewNorm;
	BOOL m_bRayEndFaceOffset;	//射线材端头平推处理，此时基准材螺栓默认也应为平推
public:	//列表框相关属性及成员函数
	CStringArray m_arrLsSetHeader;
	BOLTSET *m_pLegacyLsList;
	ATOM_LIST<CDesignLsPara> *m_pLsList;
	void DeleteItem(int iCurSel);
	void DisplayItemInfo(int iCurSel);
	void LayoutBolts(BOOL bUpdateData=TRUE);
public:
	f3dPoint face_norm_offset;
	BOOL m_bIncPlateProfilePara;
	CDesignThickPara m_tBoltNormOffset;

	// Dialog Data
	//{{AFX_DATA(CLayTubeBoltDlg)
	// 对话框数据
	enum { IDD = IDD_LAY_TUBE_BOLT_DLG };
	int     m_nLsNum;
	int     m_nLsEndSpace;
	int     m_nLsSpace;
	int		m_iRayDirection;
	CString m_hBaseNode;
	CString m_sRayTubeSize;
	int		m_iDatumPointStyle;
	CString	m_sBoltSize;
	CString m_sSegNo;
	int     m_iType;
	BOOL    m_bVirtualPart;
	double  m_fOffsetAngle;
	int     m_iStartEdge;
	int     m_iLayoutBoltStyle;
	double  m_fLenOffsetDist;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CLayTubeBoltDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CLayTubeBoltDlg)
	afx_msg void OnChangeCmbBoltSize();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeCmbDatumPointStyle();
	afx_msg void OnSelchangeCmbType();
	afx_msg void OnBnLayoutBolts();
	afx_msg void OnBnBoltNormOffset();
	afx_msg void OnLvnKeydownListLsSet(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CXhListCtrl m_xBoltListCtrl;
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYTUBEBOLTDLG_H__BE061B03_6422_4F58_B3E5_AE196B27C900__INCLUDED_)
