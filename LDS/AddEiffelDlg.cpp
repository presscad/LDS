// AddEiffelDlg.cpp : ʵ���ļ�
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "AddEiffelDlg.h"
#include "env_def.h"

// CAddEiffelDlg �Ի���
#ifdef __PART_DESIGN_INC_

IMPLEMENT_DYNAMIC(CAddEiffelDlg, CDialog)
CAddEiffelDlg::CAddEiffelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddEiffelDlg::IDD, pParent)
	, m_fEiffel(0)
	, m_sWorkName(_T(""))
{
	m_iWorkNo=0;
}

CAddEiffelDlg::~CAddEiffelDlg()
{
}

void CAddEiffelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_EFFIEL, m_fEiffel);
	DDX_Text(pDX, IDC_E_WORKCASES_NAME, m_sWorkName);
}


BEGIN_MESSAGE_MAP(CAddEiffelDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_CMB_WORKCASE_NO, OnCbnSelchangeCmbWorkcaseNo)
END_MESSAGE_MAP()


// CAddEiffelDlg ��Ϣ�������
BOOL CAddEiffelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CEdit* pEdit=(CEdit*)GetDlgItem(IDC_E_WORKCASES_NAME);
	pEdit->SetReadOnly(TRUE);
	CComboBox* pCMB=(CComboBox*)GetDlgItem(IDC_CMB_WORKCASE_NO);
	pCMB->ResetContent();
	CString sText;
	int index=0;
	for(CWorkStatus* pWorkCase=Ta.WorkStatus.GetFirst();pWorkCase;pWorkCase=Ta.WorkStatus.GetNext())
	{
		sText.Format("%d",pWorkCase->iNo);
		pCMB->InsertString(index,sText);
		index++;
	}
	pCMB->SetCurSel(0);
	return TRUE;
}
void CAddEiffelDlg::OnOK()
{
	UpdateData();
	if(m_fEiffel<0.3 || m_fEiffel>1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Eiffel coefficient is mistake!"");
#else
		AfxMessageBox("���ƶ�ϵ������ȷ!");
#endif
		return;
	}
	CDialog::OnOK();
}
void CAddEiffelDlg::OnCbnSelchangeCmbWorkcaseNo()
{
	UpdateData();
	CString sText;
	CComboBox* pCMB=(CComboBox*)GetDlgItem(IDC_CMB_WORKCASE_NO);
	pCMB->GetLBText(pCMB->GetCurSel(),sText);
	m_iWorkNo=atoi(sText);
	CWorkStatus* pWork=NULL;
	for(pWork=Ta.WorkStatus.GetFirst();pWork;pWork=Ta.WorkStatus.GetNext())
	{
		if(pWork->iNo==m_iWorkNo)
			break;
	}
	if(pWork==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The working condition number isn't exist��please re-input!");
#else
		AfxMessageBox("����Ĺ����Ų����ڣ�����������!");
#endif
		return;
	}
	m_sWorkName.Format(pWork->description);
	UpdateData(FALSE);
}
#endif