#pragma once

#include "afxdialogex.h"

// CCirPlateTypePage �Ի���

class CCirPlateTypePage : public CDialogEx
{
	DECLARE_DYNAMIC(CCirPlateTypePage)

	CListCtrl* m_pListCtrl;
	CImageList imageList;
	int m_indexCurrSelPlateType;
public:
	CCirPlateTypePage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCirPlateTypePage();
	int set_idCirPlateType(int idPlateType);
	int get_idCirPlateType();
	__declspec(property(put=set_idCirPlateType,get=get_idCirPlateType)) int idCirPlateType;
// �Ի�������
	enum { IDD = IDD_CIRPLATE_TYPE_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnItemchangedListTypeImage(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickListTypeImage(NMHDR *pNMHDR, LRESULT *pResult);
};
