// ExportSetDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ExportSetDlg.h"


// CExportSetDlg 对话框

IMPLEMENT_DYNAMIC(CExportSetDlg, CDialog)

CExportSetDlg::CExportSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_EXPORT_SET_DLG, pParent)
{
	m_iLengthUnit = 0;
	m_iModelLevel = 0;
}

CExportSetDlg::~CExportSetDlg()
{
}

void CExportSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_CMB_MODEL_LEVEL, m_iModelLevel);
	DDX_CBIndex(pDX, IDC_CMB_LENGTH_UNIT, m_iLengthUnit);
}


BEGIN_MESSAGE_MAP(CExportSetDlg, CDialog)
END_MESSAGE_MAP()


// CExportSetDlg 消息处理程序
BOOL CExportSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//
	CComboBox* pCmbUnit = (CComboBox*)GetDlgItem(IDC_CMB_LENGTH_UNIT);
	pCmbUnit->ResetContent();
	pCmbUnit->AddString("毫米(mm)");
	pCmbUnit->AddString("米(m)");
	pCmbUnit->SetCurSel(0);
	//
	CComboBox* pCmbLevel = (CComboBox*)GetDlgItem(IDC_CMB_MODEL_LEVEL);
	pCmbLevel->ResetContent();
	pCmbLevel->AddString("单线");
	pCmbLevel->AddString("杆件");
	pCmbLevel->AddString("杆件+钢板");
	pCmbLevel->AddString("杆件+钢板+螺栓");
	pCmbLevel->SetCurSel(0);

	UpdateData(FALSE);
	return TRUE;
}