// DefineOffsetPos.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "DefineOffsetPosDlg.h"
#include ".\defineoffsetposdlg.h"


// CDefineOffsetPos 对话框

IMPLEMENT_DYNAMIC(CDefineOffsetPosDlg, CDialog)
CDefineOffsetPosDlg::CDefineOffsetPosDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefineOffsetPosDlg::IDD, pParent)
	, m_fOffsetX(0)
	, m_fOffsetY(0)
	, m_fOffsetZ(0)
	, m_fPostionX(0)
	, m_fPostionY(0)
	, m_fPostionZ(0)
{
	m_bDirectDefPos=false;
}

CDefineOffsetPosDlg::~CDefineOffsetPosDlg()
{
}

void CDefineOffsetPosDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_X_OFFSET, m_fOffsetX);
	DDX_Text(pDX, IDC_E_Y_OFFSET, m_fOffsetY);
	DDX_Text(pDX, IDC_E_Z_OFFSET, m_fOffsetZ);
	DDX_Text(pDX, IDC_E_POSTION_X, m_fPostionX);
	DDX_Text(pDX, IDC_E_POSTION_Y, m_fPostionY);
	DDX_Text(pDX, IDC_E_POSTION_Z, m_fPostionZ);
}


BEGIN_MESSAGE_MAP(CDefineOffsetPosDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_EN_KILLFOCUS(IDC_E_X_OFFSET, OnEnKillfocusEXOffset)
	ON_EN_KILLFOCUS(IDC_E_Y_OFFSET, OnEnKillfocusEYOffset)
	ON_EN_KILLFOCUS(IDC_E_Z_OFFSET, OnEnKillfocusEZOffset)
END_MESSAGE_MAP()



// CDefineOffsetPos 消息处理程序
BOOL CDefineOffsetPosDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_fOffsetX=offset_vector.x;
	m_fOffsetY=offset_vector.y;
	m_fOffsetZ=offset_vector.z;
	CEdit* pEditPosX=(CEdit*)GetDlgItem(IDC_E_POSTION_X);
	CEdit* pEditPosY=(CEdit*)GetDlgItem(IDC_E_POSTION_Y);
	CEdit* pEditPosZ=(CEdit*)GetDlgItem(IDC_E_POSTION_Z);
	if(m_bDirectDefPos==false)
	{
		pEditPosX->SetReadOnly();
		pEditPosY->SetReadOnly();
		pEditPosZ->SetReadOnly();
	}
	else
	{
		pEditPosX->SetReadOnly(FALSE);
		pEditPosY->SetReadOnly(FALSE);
		pEditPosZ->SetReadOnly(FALSE);
		m_fPostionX=datum_pos.x+offset_vector.x;
		m_fPostionY=datum_pos.y+offset_vector.y;
		m_fPostionZ=datum_pos.z+offset_vector.z;
	}
	UpdateData(FALSE);
	return TRUE;
}
void CDefineOffsetPosDlg::OnOk()
{
	UpdateData();
	if(m_bDirectDefPos)
	{
		m_fOffsetX=m_fPostionX-datum_pos.x;
		m_fOffsetY=m_fPostionY-datum_pos.y;
		m_fOffsetZ=m_fPostionZ-datum_pos.z;
	}
	offset_vector.Set(m_fOffsetX,m_fOffsetY,m_fOffsetZ);
	CDialog::OnOK();
}
void CDefineOffsetPosDlg::OnCancel()
{
	CDialog::OnCancel();
}
void CDefineOffsetPosDlg::OnEnKillfocusEXOffset()
{
	UpdateData();
	if(m_bDirectDefPos)
	{
		m_fPostionX=datum_pos.x+m_fOffsetX;
		UpdateData(FALSE);
	}
}

void CDefineOffsetPosDlg::OnEnKillfocusEYOffset()
{
	UpdateData();
	if(m_bDirectDefPos)
	{
		m_fPostionY=datum_pos.y+m_fOffsetY;
		UpdateData(FALSE);
	}
}

void CDefineOffsetPosDlg::OnEnKillfocusEZOffset()
{
	UpdateData();
	if(m_bDirectDefPos)
	{
		m_fPostionZ=datum_pos.z+m_fOffsetZ;
		UpdateData(FALSE);
	}
}
