#pragma once
#ifdef __COMMON_PART_INC_

#include "propertylist.h"
#include "CallBackDialog.h"
// CDesignLjPartPropDlg 对话框

class CDesignLjPartPropDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesignLjPartPropDlg)
	long m_hCurEditLjPart;
	long m_hPlate;			//当与板连接时候启用，
public:
	BOOL m_bNewAddRod;
	BOLTSET m_xSelLinkLs;
	CDesignLjPara *m_pLjPara;
	CDesignLjPartPara m_xLjPartPara;
public:
	CDesignLjPartPropDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDesignLjPartPropDlg();
	void DisplayLjPartProperty();
	void UpdateProfileProperty(CPropTreeItem* pGroupItem);
	void FinishSelectObjOper();
	CString MakeLinkBoltStr();
	BOOL UpdateLjRodEdgeSpace(BYTE cbEdgeFlag = 0x03);
// 对话框数据
	enum { IDD = IDD_DESIGN_LJPART_PROP_DLG };
	CPropertyList m_xPropList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnConfirm();
};
#endif