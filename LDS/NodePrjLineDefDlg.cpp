//<LOCALE_TRANSLATE BY wbt />
// NodePrjLineDefDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "NodePrjLineDefDlg.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNodePrjLineDefDlg dialog


CNodePrjLineDefDlg::CNodePrjLineDefDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNodePrjLineDefDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNodePrjLineDefDlg)
	m_hEndJg = _T("");
	m_hStartJg = _T("");
	m_iStartJgBerStyle = 0;
	m_iEndJgBerStyle = 0;
	m_fStartJgLengOffsetX = 0.0;
	m_fStartJgLengOffsetY = 0.0;
	m_fEndJgLengOffsetX = 0.0;
	m_fEndJgLengOffsetY = 0.0;;
	//}}AFX_DATA_INIT
}


void CNodePrjLineDefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNodePrjLineDefDlg)
	DDX_Text(pDX, IDC_E_END_JG, m_hEndJg);
	DDX_Text(pDX, IDC_E_START_JG, m_hStartJg);
	DDX_Radio(pDX, IDC_RDO_START_JG_BASE_BER, m_iStartJgBerStyle);
	DDX_Radio(pDX, IDC_RDO_END_JG_BASE_BER, m_iEndJgBerStyle);
	DDX_Text(pDX, IDC_E_START_JG_LENG_OFFSET_X, m_fStartJgLengOffsetX);
	DDX_Text(pDX, IDC_E_START_JG_LENG_OFFSET_Y, m_fStartJgLengOffsetY);
	DDX_Text(pDX, IDC_E_END_JG_LENG_OFFSET_X, m_fEndJgLengOffsetX);
	DDX_Text(pDX, IDC_E_END_JG_LENG_OFFSET_Y, m_fEndJgLengOffsetY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNodePrjLineDefDlg, CDialog)
	//{{AFX_MSG_MAP(CNodePrjLineDefDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNodePrjLineDefDlg message handlers

void CNodePrjLineDefDlg::OnOK() 
{
	long handle;
	UpdateData();
	sscanf(m_hStartJg,"%X",&handle);
	if(console.FromPartHandle(handle,CLS_LINEANGLE)==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can't find the start of the specified Angle!");
#else 
		AfxMessageBox("找不到指定的始端角钢!");
#endif
		return;
	}
	sscanf(m_hEndJg,"%X",&handle);
	if(console.FromPartHandle(handle,CLS_LINEANGLE)==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can't find the end of the specified Angle!");
#else 
		AfxMessageBox("找不到指定的终端角钢!");
#endif
		return;
	}
	
	CDialog::OnOK();
}
