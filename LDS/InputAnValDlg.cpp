// InputAnValDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "InputAnValDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputAnIntegerValDlg dialog


CInputAnIntegerValDlg::CInputAnIntegerValDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputAnIntegerValDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputAnIntegerValDlg)
	m_nVal = 0;
	m_sValTitle = _T("");
	//}}AFX_DATA_INIT
}


void CInputAnIntegerValDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputAnIntegerValDlg)
	DDX_Text(pDX, IDC_E_VAL, m_nVal);
	DDX_Text(pDX, IDC_S_TITLE, m_sValTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputAnIntegerValDlg, CDialog)
	//{{AFX_MSG_MAP(CInputAnIntegerValDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputAnIntegerValDlg message handlers

BOOL CInputAnIntegerValDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
#ifdef AFX_TARG_ENU_ENGLISH
	SetWindowText("Input"+m_sValTitle);
#else 
	SetWindowText("����"+m_sValTitle);
#endif
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
// CInputAnStringValDlg dialog


CInputAnStringValDlg::CInputAnStringValDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputAnStringValDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputAnStringValDlg)
	m_sItemTitle = _T("");
	m_sItemValue = _T("");
	//}}AFX_DATA_INIT
	m_nStrMaxLen = 1000000000;	//Ĭ��һ���㹻���ֵ
	m_sTip = _T("");
}


void CInputAnStringValDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputAnStringValDlg)
	DDX_Text(pDX, IDC_S_TITLE, m_sItemTitle);
	DDX_Text(pDX, IDC_E_VAL, m_sItemValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputAnStringValDlg, CDialog)
	//{{AFX_MSG_MAP(CInputAnStringValDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputAnStringValDlg message handlers

BOOL CInputAnStringValDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_sTip.GetLength()>0)
	{
		m_toolTipCtrl.Create(this,TTS_ALWAYSTIP);
		m_toolTipCtrl.AddTool(this,m_sTip);
		m_toolTipCtrl.AddTool(GetDlgItem(IDC_S_TITLE),m_sTip);
		m_toolTipCtrl.AddTool(GetDlgItem(IDC_E_VAL),m_sTip);
		m_toolTipCtrl.SetDelayTime(100);
		m_toolTipCtrl.SetDelayTime(TTDT_AUTOPOP,30000);
		m_toolTipCtrl.SetMaxTipWidth(200);
		m_toolTipCtrl.SetTipTextColor(RGB(0,0,0)); 
		m_toolTipCtrl.SetTipBkColor( RGB(255,255,0)); 
		m_toolTipCtrl.Activate(TRUE);
	}
	return TRUE;
}
void CInputAnStringValDlg::OnOK() 
{
	UpdateData();
	if(m_sItemValue.GetLength()>m_nStrMaxLen)
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The string is too long, please input again");
#else 
		AfxMessageBox("�ַ�������������������");
#endif
	else
		CDialog::OnOK();
}

BOOL CInputAnStringValDlg::PreTranslateMessage(MSG* pMsg)
{
	if(m_toolTipCtrl.GetSafeHwnd()!=NULL)
	{
		switch(pMsg->message)
		{ 
		case WM_LBUTTONDOWN: 
		case WM_LBUTTONUP: 
		case WM_MOUSEMOVE: 
			m_toolTipCtrl.RelayEvent(pMsg); 
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
