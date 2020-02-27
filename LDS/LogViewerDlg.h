#ifdef __PART_DESIGN_INC_
#pragma once
#include "afxcmn.h"
#include "XhListCtrl.h"

// CLogViewerDlg �Ի���
class CLogViewerDlg : public CDialog
{
	DECLARE_DYNCREATE(CLogViewerDlg)
	void OnViewLogVersion();
	void RefreshLogVersionList();
public:
	CLogViewerDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLogViewerDlg();

// �Ի�������
	enum { IDD = IDD_LOG_VIEWER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
private:
	CXhListCtrl m_xLogVersionList;
	afx_msg void OnBnClickedBtnView();
	afx_msg void OnNMDblclkListLogVersion(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedBtnEmptyLog();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
#endif
