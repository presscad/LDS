#include "supergridctrl.h"
#pragma once


// displayPartInfoDlg �Ի���

class CDisplayPartInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDisplayPartInfoDlg)
public:
	 CSuperGridCtrl m_displayPartInfoListCtrl;
     PARTSET partSet;
public:
	CDisplayPartInfoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDisplayPartInfoDlg();
	CHashSet<CLDSPart*>hashDatumPartByHandle ;
// �Ի�������
	enum { IDD = IDD_DISPLAY_PART_DLG };
public:
	 virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnOK();
	afx_msg void OnClickDisplayPartInfoListCtrl();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClickDisplayPartInfoListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
};
