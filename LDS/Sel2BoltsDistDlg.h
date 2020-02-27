#pragma once


// CSel2BoltsDistDlg 对话框

#include "RememberLocationDlg.h"
class CSel2BoltsDistDlg : public CRememberLocationDlg
{
	//DECLARE_DYNAMIC(CSel2BoltsDistDlg)
	BYTE m_cbDimFlag;
public:
	CSel2BoltsDistDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSel2BoltsDistDlg();
	CLDSBolt* m_pDatumBolt,*m_pDestBolt;
	CLDSLineAngle* m_pDatumAngle;
	GEPOINT posStart,posEnd;
// 对话框数据
	enum { IDD = IDD_SEL_2BOLT_DIST_DLG };
public:
	// Dialog Data
	//{{AFX_DATA(CSel2BoltsDistDlg)
	CString m_shStartBolt;
	CString m_shEndBolt;
	CString m_shDatumAngle;
	double m_fDeitaD;
	double m_fDeitaL;
	double m_fDeitaG;
	double m_fChkDeitaD;
	double m_fChkDeitaL;
	double m_fChkDeitaG;
	BOOL m_bAddChkItemD;
	BOOL m_bAddChkItemL;
	BOOL m_bAddChkItemG;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSel2BoltsDistDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CSel2BoltsDistDlg)
	void OnBnClickedChkDistD();
	void OnBnClickedChkDistL();
	void OnBnClickedChkDistG();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
