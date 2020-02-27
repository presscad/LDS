#pragma once
#include "XhListCtrl.h"

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
// CBoltThroughThickDlg �Ի���

class CBoltThroughThickDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBoltThroughThickDlg)
public:
	CBoltThroughThickDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBoltThroughThickDlg();
	void ContextMenu(CWnd* pWnd, CPoint point);
	void OnSetToGlobal();
	void OnRevokeGlobal();
	CLDSBolt *m_pBolt;
// �Ի�������
	enum { IDD = IDD_BOLT_THROUGH_THICK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	void RefreshL0StrByListCtrl();
	CXhChar50 EmendZOffsetStr(PARTSET& partset);

	DECLARE_MESSAGE_MAP()
public:
	CXhListCtrl m_xListCtrl;
	virtual BOOL OnInitDialog();
	CString m_sL0;
	CString m_sZOffsetStr;
	BOOL m_bUpdateZOffset;
	double m_fL0;
	afx_msg void OnBnClickedBtnUpdateL0();
	afx_msg void OnEnChangeEL0Str();
	afx_msg void OnDestroy();
	afx_msg void OnNMClickListRelatedPart(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListRelatedPart(NMHDR *pNMHDR, LRESULT *pResult);
};
#endif