//<LOCALE_TRANSLATE BY hxr /> 2015-04-24
// AdjustPartDrawingSerialNoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AdjustPartDrawingSerialNoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdjustPartDrawingSerialNoDlg dialog


CAdjustPartDrawingSerialNoDlg::CAdjustPartDrawingSerialNoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdjustPartDrawingSerialNoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdjustPartDrawingSerialNoDlg)
	m_iCmdSerialNo = -1;
	m_sSpecPartSerialNo = _T("");
	//}}AFX_DATA_INIT
	m_nMaxSerialNo=-1;
	m_nDestSerialNo=-1;
	m_nCurSerialNo=-1;
}


void CAdjustPartDrawingSerialNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdjustPartDrawingSerialNoDlg)
	DDX_CBIndex(pDX, IDC_CMB_SERIALNO, m_iCmdSerialNo);
	DDX_Text(pDX, IDC_E_SPEC_PART_SERIALNO, m_sSpecPartSerialNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdjustPartDrawingSerialNoDlg, CDialog)
	//{{AFX_MSG_MAP(CAdjustPartDrawingSerialNoDlg)
	ON_BN_CLICKED(IDC_BTN_MOVE_AFTER, OnBtnMoveAfter)
	ON_BN_CLICKED(IDC_BTN_MOVE_BEFORE, OnBtnMoveBefore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdjustPartDrawingSerialNoDlg message handlers

void CAdjustPartDrawingSerialNoDlg::OnBtnMoveAfter() 
{
	UpdateData();
	m_nDestSerialNo=m_iCmdSerialNo+1;
	if(m_nDestSerialNo==m_nCurSerialNo)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Current serial number is already after reference serial number,please choose again!");
#else
		AfxMessageBox("当前顺序号已在参照顺序号之后,请重新选择!");
#endif
		return;
	}
	CDialog::OnOK();
}

void CAdjustPartDrawingSerialNoDlg::OnBtnMoveBefore() 
{
	UpdateData();
	m_nDestSerialNo=m_iCmdSerialNo+1-1;
	if(m_nDestSerialNo==m_nCurSerialNo)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Current serial number is already before reference serial number,please choose again!");
#else
		AfxMessageBox("当前顺序号已在参照顺序号之前,请重新选择!");
#endif
		return;
	}
	CDialog::OnOK();
}

BOOL CAdjustPartDrawingSerialNoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_nCurSerialNo<0||m_nMaxSerialNo<0)
		return FALSE;
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_SERIALNO);
	ASSERT(pCMB);
	for(int i=1;i<=m_nMaxSerialNo;i++)
	{
		CXhChar16 ss("%d",i);
		pCMB->AddString(ss);
	}
	m_sSpecPartSerialNo.Format("%d",m_nCurSerialNo);
	UpdateData(FALSE);
	return TRUE;
}
