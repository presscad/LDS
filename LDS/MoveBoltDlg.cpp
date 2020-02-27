// MoveBoltDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "MoveBoltDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoveBoltDlg dialog

//TODO:�Զ���¼λ���㷨��������,��λ���ٴε����Ի���ʱZ�����һ���������߶ȵ�λ��,���Ҽ�ͷ����̫�ѿ��������Զ��ر�
CMoveBoltDlg::CMoveBoltDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CMoveBoltDlg::IDD, pParent)
	, m_bCreateNewBolt(FALSE)
{
	//{{AFX_DATA_INIT(CMoveBoltDlg)
	m_fLenOffsetDist = 0.0;
	m_fWingOffsetDist = 0.0;
	m_nNormOffset = 0;
	m_bDisplayNewBoltCheckBox = false;
	m_sTitle="�ƶ���˨";
	//}}AFX_DATA_INIT
}


void CMoveBoltDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoveBoltDlg)
	DDX_Text(pDX, IDC_E_LEN_OFFSET_DIST, m_fLenOffsetDist);
	DDX_Text(pDX, IDC_E_WING_OFFSET_DIST, m_fWingOffsetDist);
	DDX_Text(pDX, IDC_E_NORM_OFFSET_DIST, m_nNormOffset);
	DDX_Check(pDX, IDC_CHK_CREATE_NEW_BOLT, m_bCreateNewBolt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMoveBoltDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CMoveBoltDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoveBoltDlg message handlers
BOOL CMoveBoltDlg::OnInitDialog()
{
	if(m_bDisplayNewBoltCheckBox)
		GetDlgItem(IDC_CHK_CREATE_NEW_BOLT)->ShowWindow(SW_SHOW);
	else 
		GetDlgItem(IDC_CHK_CREATE_NEW_BOLT)->ShowWindow(SW_HIDE);
	MoveWndToLocation();
	SetWindowText(m_sTitle);
	return CDialog::OnInitDialog();
}

bool CMoveBoltDlg::IsCreateNewBolt()
{
	if(m_bDisplayNewBoltCheckBox&&m_bCreateNewBolt)
		return true;
	else 
		return false;
}