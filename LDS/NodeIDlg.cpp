// NodeSegIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "NodeIDlg.h"
#include "afxdialogex.h"


// CNodeSegIDlg 对话框

IMPLEMENT_DYNAMIC(CNodeIDlg, CDialogEx)

CNodeIDlg::CNodeIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNodeIDlg::IDD, pParent)
{
	m_bInheritPracticalNode=m_bInheritDummyNode=TRUE;
	m_bRecodeAll=FALSE;
	m_iMinDummyNodeI = 0;
	m_iPriorMiddleVacancy = 0;
}

CNodeIDlg::~CNodeIDlg()
{
}

void CNodeIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_MIN_DUMMY_NODE_I, m_iMinDummyNodeI);
	DDX_Radio(pDX, IDC_R_PRIOR_MIDDLE_VACANCY, m_iPriorMiddleVacancy);
	DDX_Check(pDX, IDC_CHK_INHERIT_PRACTICAL_NODE, m_bInheritPracticalNode);
	DDX_Check(pDX, IDC_CHK_INHERIT_DUMMY_NODE, m_bInheritDummyNode);
	DDX_Check(pDX, IDC_CHK_RECODE_ALL, m_bRecodeAll);
}


BEGIN_MESSAGE_MAP(CNodeIDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHK_RECODE_ALL, &CNodeIDlg::OnBnClickedRecodeAll)
	ON_BN_CLICKED(IDC_CHK_INHERIT_DUMMY_NODE, &CNodeIDlg::OnBnClickedInheritDummyNode)
	ON_BN_CLICKED(IDC_CHK_INHERIT_PRACTICAL_NODE, &CNodeIDlg::OnBnClickedParcticalNode)
END_MESSAGE_MAP()


// CNodeSegIDlg 消息处理程序


void CNodeIDlg::OnBnClickedRecodeAll()
{
	UpdateData();
	if(m_bRecodeAll)
	{
		m_bInheritDummyNode=m_bInheritPracticalNode=FALSE;
		m_iPriorMiddleVacancy=1;
	}
	GetDlgItem(IDC_R_PRIOR_MIDDLE_VACANCY)->EnableWindow(m_bInheritDummyNode||m_bInheritPracticalNode);
	UpdateData(FALSE);
}


void CNodeIDlg::OnBnClickedInheritDummyNode()
{
	UpdateData();
	m_bRecodeAll=!(m_bInheritDummyNode||m_bInheritPracticalNode);
	GetDlgItem(IDC_R_PRIOR_MIDDLE_VACANCY)->EnableWindow(m_bInheritDummyNode||m_bInheritPracticalNode);
	UpdateData(FALSE);
}


void CNodeIDlg::OnBnClickedParcticalNode()
{
	UpdateData();
	m_bRecodeAll=!(m_bInheritDummyNode||m_bInheritPracticalNode);
	GetDlgItem(IDC_R_PRIOR_MIDDLE_VACANCY)->EnableWindow(m_bInheritDummyNode||m_bInheritPracticalNode);
	UpdateData(FALSE);
}
