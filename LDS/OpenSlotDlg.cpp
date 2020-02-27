//DesHangFittingDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "afxdialogex.h"
#include "OpenSlotDlg.h"

//////////////////////////////////////////////////////////////////////////
// COpenSlotDlg �Ի���
IMPLEMENT_DYNAMIC(COpenSlotDlg, CRememberLocationDlg)
COpenSlotDlg::COpenSlotDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(COpenSlotDlg::IDD, pParent)
{
	m_fSlotH=0;
	m_fSlotW=0;
	m_fOffLen=0;
	m_fDeltaS=0;
	m_fDeltaE=0;
	m_iCutWing=0;
	m_pCurJg=NULL;
}

COpenSlotDlg::~COpenSlotDlg()
{
}

void COpenSlotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_SLOT_H , m_fSlotH);
	DDX_Text(pDX, IDC_E_SLOT_W,  m_fSlotW);
	DDX_Text(pDX, IDC_E_OFF_LEN, m_fOffLen);
	DDX_Text(pDX, IDC_E_DELTA_S, m_fDeltaS);
	DDX_Text(pDX, IDC_E_DELTA_E, m_fDeltaE);
	DDX_CBIndex(pDX, IDC_CMB_OPEN_WING, m_iCutWing);	
}


BEGIN_MESSAGE_MAP(COpenSlotDlg, CRememberLocationDlg)
	
END_MESSAGE_MAP()


// COpenSlotDlg ��Ϣ�������
BOOL COpenSlotDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//�ƶ��Ի������Ͻ�
	MoveWndToLocation();
	return TRUE;
}
void COpenSlotDlg::OnOK()
{
	if(m_pCurJg&&m_fSlotH>(m_pCurJg->size_wide-m_pCurJg->size_thick))
	{
		AfxMessageBox("������ȹ��󣬲�����!");
		return;
	}
	return CDialog::OnOK();
}
