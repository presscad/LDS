// ConfirmJgDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "tower.h"
#include "ConfirmJgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfirmJgDlg dialog
CEvent UserConfirm;
CEvent UserCancel;

CConfirmJgDlg::CConfirmJgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmJgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmJgDlg)
	m_iJgExtendStyle = 2;
	m_iJgStyle = 2;
	m_iLocation = 1;
	m_iPos = 2;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sGuiGe = _T("Unknown specifications");
#else 
	m_sGuiGe = _T("未知规格");
#endif
	//}}AFX_DATA_INIT
	m_bExtStart = m_bExtEnd = TRUE;
	m_pView = NULL;
}
CConfirmJgDlg::CConfirmJgDlg(CView *pView)
{
	m_iJgExtendStyle = 2;
	m_iJgStyle = 2;
	m_iLocation = 1;
	m_iPos = 2;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sGuiGe = _T("Unknown specifications");
#else 
	m_sGuiGe = _T("未知规格");
#endif
	m_bExtStart = m_bExtEnd = TRUE;
	m_pView = pView;
}
BOOL CConfirmJgDlg::Create()
{
	return CDialog::Create(CConfirmJgDlg::IDD);
}

void CConfirmJgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmJgDlg)
	DDX_Radio(pDX, IDC_JG_EXTEND_STYLE, m_iJgExtendStyle);
	DDX_Radio(pDX, IDC_RDO_JG_STYLE, m_iJgStyle);
	DDX_Radio(pDX, IDC_RDO_LOCATION, m_iLocation);
	DDX_Radio(pDX, IDC_RDO_POS, m_iPos);
	DDX_Text(pDX, IDC_S_JG_GUIGE, m_sGuiGe);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmJgDlg, CDialog)
	//{{AFX_MSG_MAP(CConfirmJgDlg)
	ON_BN_CLICKED(IDC_BN_MODULATE_VIEW, OnBnModulateView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmJgDlg message handlers

void CConfirmJgDlg::OnOK() 
{
	// TODO: Add extra validation here

	if(m_pView)
		UserConfirm.SetEvent();
	else
		CDialog::OnOK();
}

void CConfirmJgDlg::OnCancel() 
{
	if(m_pView)
		UserCancel.SetEvent();
	else
		CDialog::OnCancel();
}

BOOL CConfirmJgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CButton *pButton = (CButton*)GetDlgItem(IDC_JG_EXTEND_STYLE);
	pButton->EnableWindow(m_bExtStart);
			 pButton = (CButton*)GetDlgItem(IDC_RDO_EXTEND_END);
	pButton->EnableWindow(m_bExtEnd);
	//----下面的代码用于将此对话框置于右下角
	CSize sizeMaxScr;
	sizeMaxScr.cx = GetSystemMetrics(SM_CXSCREEN);
	sizeMaxScr.cy = GetSystemMetrics(SM_CYSCREEN);
	CRect rect;
	CWnd::GetWindowRect(rect);
	rect.left = sizeMaxScr.cx-rect.Width();
	rect.right=sizeMaxScr.cx;
	rect.top  =sizeMaxScr.cy-rect.Height()-40;
	rect.bottom = sizeMaxScr.cy-40;
	MoveWindow(rect, TRUE);
	m_bMoudulateView=FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfirmJgDlg::OnBnModulateView() 
{
	m_bMoudulateView=TRUE;
	CDialog::OnOK();
	
}
