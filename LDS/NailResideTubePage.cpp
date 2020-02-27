// NailResideTubePage.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "FootNailSpaceDlg.h"
#include "NailResideTubePage.h"


// CNailResideTubePage 对话框

IMPLEMENT_DYNAMIC(CNailResideTubePage, CDialog)

CNailResideTubePage::CNailResideTubePage(CWnd* pParent /*=NULL*/)
	: CDialog(CNailResideTubePage::IDD, pParent)
{
	m_iLaySide=m_iNailType=0;
	m_pParentWnd=pParent;
}

CNailResideTubePage::~CNailResideTubePage()
{
}

void CNailResideTubePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RDO_SIDE_A, m_iLaySide);
	DDX_Radio(pDX, IDC_RDO_SLOT_NAIL, m_iNailType);
}


BEGIN_MESSAGE_MAP(CNailResideTubePage, CDialog)
	ON_BN_CLICKED(IDC_RDO_SIDE_A, &OnRdoLaySide)
	ON_BN_CLICKED(IDC_RDO_SIDE_B, &OnRdoLaySide)
	ON_BN_CLICKED(IDC_RDO_SLOT_NAIL, &OnRdoNailType)
	ON_BN_CLICKED(IDC_RDO_ANGLE_NAIL, &OnRdoNailType)
END_MESSAGE_MAP()

// CNailResideTubePage 消息处理程序
void CNailResideTubePage::OnRdoLaySide()
{
	int iOldLaySide=m_iLaySide;
	UpdateData();
	((CFootnailSpaceDlg*)m_pParentWnd)->FireRdoLayWing(iOldLaySide);
}
void CNailResideTubePage::OnRdoNailType()
{
	int iOldNailType=m_iNailType;
	UpdateData();
	((CFootnailSpaceDlg*)m_pParentWnd)->FireRdoNailType(iOldNailType);
}
