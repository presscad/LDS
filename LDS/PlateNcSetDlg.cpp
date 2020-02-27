// PlateNcSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "PlateNcSetDlg.h"
#include "afxdialogex.h"
#include "env_def.h"

// CPlateNcSetDlg �Ի���

IMPLEMENT_DYNAMIC(CPlateNcSetDlg, CDialog)

CPlateNcSetDlg::CPlateNcSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlateNcSetDlg::IDD, pParent)
	, m_iRollOffsetType(0)
	, m_iDeformType(0)
{

}

CPlateNcSetDlg::~CPlateNcSetDlg()
{
}

void CPlateNcSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_CMB_ROLL_OFFSET_TYPE, m_iRollOffsetType);
	DDX_CBIndex(pDX, IDC_CMB_DEFORM_TYPE, m_iDeformType);
}


BEGIN_MESSAGE_MAP(CPlateNcSetDlg, CDialog)
END_MESSAGE_MAP()


// CPlateNcSetDlg ��Ϣ�������
BOOL CPlateNcSetDlg::OnInitDialog()
{
	m_iRollOffsetType=Ta.m_ciRollOffsetType;
	m_iDeformType=Ta.m_ciDeformType;
	//
	UpdateData(FALSE);
	return CDialog::OnInitDialog();
}