// NailResideAnglePage.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "FootNailSpaceDlg.h"
#include "NailResideAnglePage.h"


// CNailResideAnglePage 对话框

IMPLEMENT_DYNAMIC(CNailResideAnglePage, CDialog)

CNailResideAnglePage::CNailResideAnglePage(CWnd* pParent /*=NULL*/)
	: CDialog(CNailResideAnglePage::IDD, pParent)
{
	m_pParentWnd=pParent;
	m_iLayWing = 0;
	m_iGType=0;
	m_nGDist=0;
}

CNailResideAnglePage::~CNailResideAnglePage()
{
}

void CNailResideAnglePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RDO_LAY_WING, m_iLayWing);
	DDX_CBIndex(pDX, IDC_CMB_G_TYPE, m_iGType);
	DDX_Text(pDX, IDC_E_G_DIST, m_nGDist);
}


BEGIN_MESSAGE_MAP(CNailResideAnglePage, CDialog)
	ON_BN_CLICKED(IDC_RDO_LAY_WING, &OnRdoLayWing)
	ON_BN_CLICKED(IDC_RDO_LAY_WING2, &OnRdoLayWing)
	ON_CBN_SELCHANGE(IDC_CMB_G_TYPE, OnCbnSelchangeCmbGType)
	ON_EN_CHANGE(IDC_E_G_DIST, OnChangeGDist)
END_MESSAGE_MAP()


// CNailResideAnglePage 消息处理程序
void CNailResideAnglePage::OnRdoLayWing()
{
	int iOldLayWing=m_iLayWing;
	UpdateData();
	((CFootnailSpaceDlg*)m_pParentWnd)->FireRdoLayWing(iOldLayWing);
}
void CNailResideAnglePage::OnCbnSelchangeCmbGType()
{
	UpdateData();
	JGZJ jgzj;
	if(m_iLayWing==0)		//X肢上布脚钉
		jgzj=xWingXZhunJu;
	else if(m_iLayWing==1)//Y肢上布脚钉
		jgzj=xWingYZhunJu;
	((CEdit*)GetDlgItem(IDC_E_G_DIST))->SetReadOnly();
	if(m_iGType==0)
		m_nGDist=jgzj.g;
	else if(m_iGType==1)
		m_nGDist=jgzj.g1;
	else if(m_iGType==2)
		m_nGDist=jgzj.g2;
	else if(m_iGType==3)
		m_nGDist=jgzj.g3;
	else if(m_iGType==4)
		((CEdit*)GetDlgItem(IDC_E_G_DIST))->SetReadOnly(FALSE);
	UpdateData(FALSE);
	((CFootnailSpaceDlg*)m_pParentWnd)->FireResideAngleDist();
}
void CNailResideAnglePage::OnChangeGDist()
{
	UpdateData();
	((CFootnailSpaceDlg*)m_pParentWnd)->FireResideAngleDist();
}