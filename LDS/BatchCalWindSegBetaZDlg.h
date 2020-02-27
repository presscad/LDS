#pragma once

#ifdef __PART_DESIGN_INC_
#include "XhListCtrl.h"
// CBatchCalWindSegBetaZDlg �Ի���

class CBatchCalWindSegBetaZDlg : public CDialog
{
	DECLARE_DYNAMIC(CBatchCalWindSegBetaZDlg)

public:
	VIBRAMODE_PROP *modes_arr;
	int m_nOrderNum;
	int m_iSelOrder;
	double m_fBetaZCalWindV;	//��������񶯷��������ϵ��ʱ�Ļ�׼���٣�һ��Ϊ�ṹ�����е������٣�
	CBatchCalWindSegBetaZDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBatchCalWindSegBetaZDlg();

// �Ի�������
	enum { IDD = IDD_CAL_WINDSEG_BETAZ_DLG };
	CXhListCtrl	m_listVibraMode;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_iBatchCalBetaZStyle;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnIntelliSelVibraMode();
	afx_msg void OnBnPreviewCurrVibraMode();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLvnItemchangedVibrationModeInfo(NMHDR *pNMHDR, LRESULT *pResult);
};
#endif