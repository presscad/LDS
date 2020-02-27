// SlotArmDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SlotArmDlg.h"


// SlotArmDlg 对话框
#ifdef __GGG_

IMPLEMENT_DYNAMIC(SlotArmDlg, CDialog)
SlotArmDlg::SlotArmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SlotArmDlg::IDD, pParent)
	, m_fCoverPlateWidth(0)
	, m_fCoverPlateThick(0)
	, m_fSidePlateThick(0)
	, m_fRootWidth(0)
	, m_fRootHigh(0)
	, m_fEndHigh(0)
	, m_fInsertLen(0)
	, m_fSlotArmLen(0)
	, m_fRibThick(0)
	, m_fRibLen(0)
{
}

SlotArmDlg::~SlotArmDlg()
{
}

void SlotArmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_COVER_BOAD_WIDTH, m_fCoverPlateWidth);
	DDX_Text(pDX, IDC_E_COVER_THICK, m_fCoverPlateThick);
	DDX_Text(pDX, IDC_E_SIDE_THICK, m_fSidePlateThick);
	DDX_Text(pDX, IDC_E_ROOT_WID, m_fRootWidth);
	DDX_Text(pDX, IDC_E_ROOT_HIG, m_fRootHigh);
	DDX_Text(pDX, IDC_E_END_HIG, m_fEndHigh);
	DDX_Text(pDX, IDC_E_INNER_LEN, m_fInsertLen);
	DDX_Text(pDX, IDC_E_ARM_LEN, m_fSlotArmLen);
	DDX_Text(pDX, IDC_E_RIB_THICK, m_fRibThick);
	DDX_Text(pDX, IDC_E_RIB_LEN, m_fRibLen);
}


BEGIN_MESSAGE_MAP(SlotArmDlg, CDialog)
END_MESSAGE_MAP()


// SlotArmDlg 消息处理程序
#endif