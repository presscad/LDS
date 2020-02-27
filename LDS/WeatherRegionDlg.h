#if !defined(AFX_WEATHERREGIONDLG_H__6C4BDBB7_0190_42A0_8FC5_F8EADA7E490F__INCLUDED_)
#define AFX_WEATHERREGIONDLG_H__6C4BDBB7_0190_42A0_8FC5_F8EADA7E490F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WeatherRegionDlg.h : header file
//
#include "SuperGridCtrl.h"
#include "WireLoadDatabase.h"

/////////////////////////////////////////////////////////////////////////////
// CWeatherRegionDlg dialog

class CWeatherRegionDlg : public CDialog
{
	CWireLoadDatabase db;
	BOOL UpdateWeatherInfoList(char* regionCode);
// Construction
public:
	BOOL m_bDataChange;//监控list中内容是否变化进而得知用户需要查询操作还是保存操作，若发生变化就跳过查询
	WEATHER_REGION weatherRegion;
	CWeatherRegionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWeatherRegionDlg)
	enum { IDD = IDD_WEATHER_REGION_DLG };
	CSuperGridCtrl	m_listCtrl;
	CString	m_sRegionCode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWeatherRegionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWeatherRegionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCmbRegionCode();
	afx_msg void OnEditchangeCmbRegionCode();
	afx_msg void OnBnSaveWeatherRegion();
	afx_msg void OnBnDelWeatherRegion();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEATHERREGIONDLG_H__6C4BDBB7_0190_42A0_8FC5_F8EADA7E490F__INCLUDED_)
