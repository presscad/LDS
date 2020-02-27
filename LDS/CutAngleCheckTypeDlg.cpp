// CutAngleCheckTypeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "CutAngleCheckTypeDlg.h"

// CCutAngleCheckTypeDlg 对话框

IMPLEMENT_DYNAMIC(CCutAngleCheckTypeDlg, CDialog)
CCutAngleCheckTypeDlg::CCutAngleCheckTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCutAngleCheckTypeDlg::IDD, pParent)
	, m_bCheckRayAndRay(TRUE)
	, m_bCheckMainAndRay(TRUE)
	, m_bCheckAngleAndBolt(TRUE)
	, m_iBoltDetectType(1)
{
	m_fClearance=g_sysPara.VertexDist;
}

CCutAngleCheckTypeDlg::~CCutAngleCheckTypeDlg()
{
}

void CCutAngleCheckTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_RAY_RAY, m_bCheckRayAndRay);
	DDX_Check(pDX, IDC_CHK_MAIN_RAY, m_bCheckMainAndRay);
	DDX_Check(pDX, IDC_CHK_ANGLE_BOLT, m_bCheckAngleAndBolt);
	DDX_Radio(pDX, IDC_RDO_BY_COLLIDE_WITH_BOLT, m_iBoltDetectType);
	DDX_Text(pDX, IDC_E_CLEARANCE, m_fClearance);
}


BEGIN_MESSAGE_MAP(CCutAngleCheckTypeDlg, CDialog)
	ON_BN_CLICKED(IDC_CHK_ANGLE_BOLT, &CCutAngleCheckTypeDlg::OnBnClickedChkAngleBolt)
END_MESSAGE_MAP()

// CCutAngleCheckTypeDlg 消息处理程序
BOOL CCutAngleCheckTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	OnBnClickedChkAngleBolt();
	return TRUE;
}
void CCutAngleCheckTypeDlg::OnBnClickedChkAngleBolt()
{
	UpdateData();
	GetDlgItem(IDC_RDO_BY_COLLIDE_WITH_BOLT)->EnableWindow(m_bCheckAngleAndBolt);
	GetDlgItem(IDC_RDO_INTERFERE_WITH_BOLT)->EnableWindow(m_bCheckAngleAndBolt);
	GetDlgItem(IDC_E_CLEARANCE)->EnableWindow(m_bCheckAngleAndBolt);
}
