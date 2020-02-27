#pragma once

#include "BoltLibrary.h"
#include "XhListCtrl.h"
#include "RememberLocationDlg.h"
// CSysLsFamilyDlg �Ի���

class CSysLsFamilyDlg : public CRememberLocationDlg
{
	DECLARE_DYNAMIC(CSysLsFamilyDlg)
public:
	IBoltLibrary* m_pBoltLib;
public:
	CSysLsFamilyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSysLsFamilyDlg();
	
	void RefreshLsSeriesListCtrl();
	void RefreshListCtrl(int iCurSelLsSeries=0);
// �Ի�������
	enum { IDD = IDD_SYS_LS_FAMILY_DLG };
	CXhListCtrl		m_xListCtrl;
	CXhListCtrl		m_xLsFamilyListCtrl;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMDblclkLsFamilyListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickLsFamilyListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
};
