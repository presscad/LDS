#pragma once

// CInputGimInfoDlg �Ի���
#include "PropertyList.h"
#include "ElectricTower.h"

#ifdef __PART_DESIGN_INC_
class CInputGimInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInputGimInfoDlg)
private:
	CTowerGimInfo* m_pGimInfo;
public:
	void SetGimInfo(CTowerGimInfo* pGimInfo);
	void DisplayGimProperty();
public:
	CInputGimInfoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputGimInfoDlg();

// �Ի�������
	enum { IDD = IDD_INPUT_GIM_INFO_DLG };
	CPropertyList	m_xPropList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnImportFile();
	afx_msg void OnExportFile();
};
#endif
