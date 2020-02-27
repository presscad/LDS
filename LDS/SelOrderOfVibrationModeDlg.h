#if !defined(AFX_SELORDEROFVIBRATIONMODEDLG_H__956588F2_E2C8_4368_A3E7_64205A8D53B3__INCLUDED_)
#define AFX_SELORDEROFVIBRATIONMODEDLG_H__956588F2_E2C8_4368_A3E7_64205A8D53B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelOrderOfVibrationModeDlg.h : header file
//
#if defined(__LDS_)||defined(__TSA_)
/////////////////////////////////////////////////////////////////////////////
// CSelOrderOfVibrationModeDlg dialog
class CSelOrderOfVibrationModeDlg : public CDialog
{
// Construction
public:
	VIBRAMODE_PROP *modes_arr;
	int m_iSelOrder;
	int m_nOrderNum;
	CSelOrderOfVibrationModeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelOrderOfVibrationModeDlg)
	enum { IDD = IDD_SEL_ORDER_OF_VIBRATION_MODE_DLG };
	CXhListCtrl	m_listVibraMode;
	double	m_fMaxAmplitude;
	int m_nSimulateVibraModePeriod;
	int m_nSampleTimerInterval;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelOrderOfVibrationModeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelOrderOfVibrationModeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELORDEROFVIBRATIONMODEDLG_H__956588F2_E2C8_4368_A3E7_64205A8D53B3__INCLUDED_)
