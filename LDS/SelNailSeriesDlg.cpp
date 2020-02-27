// SelNailSeriesDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SelNailSeriesDlg.h"
#include "afxdialogex.h"


// CSelNailSeriesDlg 对话框

IMPLEMENT_DYNAMIC(CSelNailSeriesDlg, CDialogEx)

CSelNailSeriesDlg::CSelNailSeriesDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelNailSeriesDlg::IDD, pParent)
	, m_iCommonNailSeries(0)
	, m_iAntiThelftNailSeries(0)
{
	m_idCommonNailSeries=m_idAntiThelftNailSeries=0;
}

CSelNailSeriesDlg::~CSelNailSeriesDlg()
{
}

void CSelNailSeriesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_LBIndex(pDX, IDC_LIST_COMMON_NAIL_SERIES, m_iCommonNailSeries);
	DDX_LBIndex(pDX, IDC_LIST_ANTITHELFT_NAIL_SERIES, m_iAntiThelftNailSeries);
}


BEGIN_MESSAGE_MAP(CSelNailSeriesDlg, CDialogEx)
END_MESSAGE_MAP()


// CSelNailSeriesDlg 消息处理程序


BOOL CSelNailSeriesDlg::OnInitDialog()
{
	m_idCommonNailSeries=CLsLibrary::GetFootNailFamily();
	CLsFamily* pNailSeries=CLsLibrary::FromHandle(m_idCommonNailSeries);
	if (pNailSeries)
		m_idAntiThelftNailSeries=pNailSeries->m_hMappingAntiTheftFamily;
	else
		m_idAntiThelftNailSeries=0;
	CDialogEx::OnInitDialog();

	CListBox* pListBox=(CListBox*)GetDlgItem(IDC_LIST_COMMON_NAIL_SERIES);
	CListBox* pThelftListBox=(CListBox*)GetDlgItem(IDC_LIST_ANTITHELFT_NAIL_SERIES);
	pListBox->ResetContent();
	pThelftListBox->ResetContent();
	pListBox->AddString("未设定");
	pThelftListBox->AddString("未设定");
	for(CLsFamily* pBoltSeries=CLsLibrary::GetFirstFamily();pBoltSeries!=NULL;pBoltSeries=CLsLibrary::GetNextFamily())
	{
		int index=pListBox->AddString(pBoltSeries->name);
		pThelftListBox->AddString(pBoltSeries->name);
		pListBox->SetItemData(index,pBoltSeries->Id);
		pThelftListBox->SetItemData(index,pBoltSeries->Id);
		if (pBoltSeries->Id==m_idCommonNailSeries)
			pListBox->SetCurSel(index);
		if (pBoltSeries->Id==m_idAntiThelftNailSeries)
			pThelftListBox->SetCurSel(index);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CSelNailSeriesDlg::OnOK()
{
	UpdateData();
	CListBox* pListBox=(CListBox*)GetDlgItem(IDC_LIST_COMMON_NAIL_SERIES);
	CListBox* pThelftListBox=(CListBox*)GetDlgItem(IDC_LIST_ANTITHELFT_NAIL_SERIES);
	CLsFamily* pPreferNailSeries=NULL,*pThelftNailSeries=NULL;
	int index=pListBox->GetCurSel();
	if (index==0)
		CLsLibrary::SetFootNailFamily(0);
	else if(index>0)
	{
		this->m_idCommonNailSeries=pListBox->GetItemData(index);
		CLsLibrary::SetFootNailFamily(m_idCommonNailSeries);
		pPreferNailSeries=CLsLibrary::FromHandle(m_idCommonNailSeries);
		pPreferNailSeries->m_bFootNail=true;	//默认脚钉之所以为空一般是因为真实的脚钉系列缺少该属性所致 wjh-2019.9.21
	}
	if (pPreferNailSeries)
	{
		index=pThelftListBox->GetCurSel();
		if (index==0)
			pPreferNailSeries->m_hMappingAntiTheftFamily=0;
		else if(index>0)
		{
			pPreferNailSeries->m_hMappingAntiTheftFamily=this->m_idAntiThelftNailSeries=pListBox->GetItemData(index);
			if ((pThelftNailSeries=CLsLibrary::FromHandle(m_idAntiThelftNailSeries))!=NULL&&
				strstr(pThelftNailSeries->name,"脚钉")!=NULL)	//添加脚钉中的名称限制，主要是为了防误操作 wjh-2019.9.21
			{	//即然设为配套的防盗脚钉系列，那这个系列如下两属性必须为true; wjh-2019.9.21
				pThelftNailSeries->m_bFootNail=true;
				pThelftNailSeries->m_bBurglarproof=true;
			}
		}
	}

	CDialogEx::OnOK();
}
