#pragma once
#ifdef __COMMON_PART_INC_

#include "propertylist.h"
#include "CallBackDialog.h"
// CDesignLjPartPropDlg �Ի���

class CDesignLjPartPropDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesignLjPartPropDlg)
	long m_hCurEditLjPart;
	long m_hPlate;			//���������ʱ�����ã�
public:
	BOOL m_bNewAddRod;
	BOLTSET m_xSelLinkLs;
	CDesignLjPara *m_pLjPara;
	CDesignLjPartPara m_xLjPartPara;
public:
	CDesignLjPartPropDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDesignLjPartPropDlg();
	void DisplayLjPartProperty();
	void UpdateProfileProperty(CPropTreeItem* pGroupItem);
	void FinishSelectObjOper();
	CString MakeLinkBoltStr();
	BOOL UpdateLjRodEdgeSpace(BYTE cbEdgeFlag = 0x03);
// �Ի�������
	enum { IDD = IDD_DESIGN_LJPART_PROP_DLG };
	CPropertyList m_xPropList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnConfirm();
};
#endif