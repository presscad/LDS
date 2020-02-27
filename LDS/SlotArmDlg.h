#pragma once


// SlotArmDlg 对话框
#include "resource.h"

#ifdef __GGG_
class SlotArmDlg : public CDialog
{
	DECLARE_DYNAMIC(SlotArmDlg)

public:
	SlotArmDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SlotArmDlg();

// 对话框数据
	enum { IDD = IDD_SLOT_ARM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_fCoverPlateWidth;
	double m_fCoverPlateThick;
	double m_fSidePlateThick;
	double m_fRootWidth;
	double m_fRootHigh;
	double m_fEndHigh;
	double m_fInsertLen;
	double m_fSlotArmLen;
	double m_fRibThick;
	double m_fRibLen;
};
#endif