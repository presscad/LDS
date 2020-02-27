// SelNailSeriesDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "SelNailSeriesDlg.h"
#include "afxdialogex.h"


// CSelNailSeriesDlg �Ի���

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


// CSelNailSeriesDlg ��Ϣ�������


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
	pListBox->AddString("δ�趨");
	pThelftListBox->AddString("δ�趨");
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
	// �쳣: OCX ����ҳӦ���� FALSE
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
		pPreferNailSeries->m_bFootNail=true;	//Ĭ�ϽŶ�֮����Ϊ��һ������Ϊ��ʵ�ĽŶ�ϵ��ȱ�ٸ��������� wjh-2019.9.21
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
				strstr(pThelftNailSeries->name,"�Ŷ�")!=NULL)	//��ӽŶ��е��������ƣ���Ҫ��Ϊ�˷������ wjh-2019.9.21
			{	//��Ȼ��Ϊ���׵ķ����Ŷ�ϵ�У������ϵ�����������Ա���Ϊtrue; wjh-2019.9.21
				pThelftNailSeries->m_bFootNail=true;
				pThelftNailSeries->m_bBurglarproof=true;
			}
		}
	}

	CDialogEx::OnOK();
}
