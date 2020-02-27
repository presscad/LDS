#pragma once

#include "XhListCtrl.h"
#include "supergridctrl.h"
// CDataCheckDlg �Ի���

class CDataCheckDlg : public CDialog
{
	DECLARE_DYNCREATE(CDataCheckDlg)

public:
	CComboBox	m_cmbCheckType;
	CSuperGridCtrl m_xDataListCtrl;
	CString		m_sSeg;
	CString		m_sHelp;
	BOOL m_bSelAngle;
	BOOL m_bSelPlate;
private:
	void RelayoutWnd();
	void RefreshList(BOOL bRefreshColumn=TRUE);
	void RefreshColumnHeader();
	void RefreshHelpStr();
	void ContextMenu(CWnd* pWnd, CPoint point);
public:
	CDataCheckDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDataCheckDlg();
	//
	void InitDataList();
	void MarkPartData(const char* sKey);
	int GetCurSelType(){return m_cmbCheckType.GetCurSel();}
// �Ի�������
	enum { IDD = IDD_DATA_CHECK_DLG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeCmbCheckType();
	afx_msg void OnEnChangeECheckIseg();
	afx_msg void OnBnClickedBnUpdate();
	afx_msg void OnNMRClickCheckDataList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMarkCheck();
	afx_msg void OnCancelMark();
	afx_msg void OnBnClickedCheckAngle();
	afx_msg void OnBnClickedCheckPlate();
};
