#pragma once
#include "afxwin.h"

// CRsltBufferDlg 对话框
#if defined(__LDS_)||defined(__TSA_)

class CRsltBufferDlg : public CDialog
{
	DECLARE_DYNAMIC(CRsltBufferDlg)

public:
	CString m_cmbRsltItems;
	CRsltBufferDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRsltBufferDlg();
	CBitmap m_curBitmap1,m_curBitmap2,m_curBitmap3;
// 对话框数据
	enum { IDD = IDD_RSLT_BUFFER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	void InitQuadLegs(CLDSModule* pModule);
public:
	CString m_sActiveRsltItemKeyStr;
	CString m_sCompareRsltItemKeyStr;
	CString m_sCtrlCaseStr;
	long m_nPointI;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSaveNewItem();
	afx_msg void OnBnClickedDelRsltItem();
	afx_msg void OnBnNodeEquation();
	afx_msg void OnBnClickedExportRsltRptFile();
	afx_msg void OnBnClickedCmpRsltRpt();
	afx_msg void OnBnClickedBnRetrieveCtrlCases();
	afx_msg void OnBnClickedBnExportAnalyzeProcFile();
	afx_msg void OnCbnSelchangeCmbModuleNo();
	afx_msg void OnCbnSelchangeCmbLegQuad1();
	afx_msg void OnBnClickedReadRsltItem();
};
#endif