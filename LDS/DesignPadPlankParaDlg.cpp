// DesignPadPlankParaDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "DesignPadPlankParaDlg.h"
#include "ScrTools.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignPadPlankParaDlg dialog


CDesignPadPlankParaDlg::CDesignPadPlankParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignPadPlankParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignPadPlankParaDlg)
	m_hBaseJg = _T("0X0");
	m_hCutBaseJg = _T("0X0");
	m_sIncLsStr = _T("");
	m_sPartNo = _T("");
	//}}AFX_DATA_INIT
	m_pPadPlankPara=NULL;
}


void CDesignPadPlankParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignPadPlankParaDlg)
	DDX_Text(pDX, IDC_E_BASE_JG, m_hBaseJg);
	DDX_Text(pDX, IDC_E_CUT_BASE_JG, m_hCutBaseJg);
	DDX_Text(pDX, IDC_E_INCLUDE_LS_STR, m_sIncLsStr);
	DDV_MaxChars(pDX, m_sIncLsStr, 49);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDV_MaxChars(pDX, m_sPartNo, 24);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignPadPlankParaDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignPadPlankParaDlg)
	ON_BN_CLICKED(IDC_BN_THICK_STR, OnBnThickStr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignPadPlankParaDlg message handlers

BOOL CDesignPadPlankParaDlg::OnInitDialog() 
{
	if(m_pPadPlankPara==NULL)
		return FALSE;
	CString ss;
	ss.Format("%d",m_pPadPlankPara->thick.Thick(&Ta));
	m_sPartNo=m_pPadPlankPara->sPartNo;
	GetDlgItem(IDC_BN_THICK_STR)->SetWindowText(ss);
	m_hBaseJg.Format("0X%X",m_pPadPlankPara->hBaseJg);
	m_hCutBaseJg.Format("0X%X",m_pPadPlankPara->hCutRefJg);
	m_sIncLsStr=m_pPadPlankPara->ls_str;
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDesignPadPlankParaDlg::OnBnThickStr() 
{
	CString sThickStr=m_pPadPlankPara->thick.key_str;
#ifdef AFX_TARG_ENU_ENGLISH
	Input("Please input the string of gasket's thick.",&sThickStr,'s');
#else 
	Input("ÊäÈëµæ°åºñ¶È×Ö·û´®",&sThickStr,'s');
#endif
	_snprintf(m_pPadPlankPara->thick.key_str,49,"%s",sThickStr);
	sThickStr.Format("%d",m_pPadPlankPara->thick.Thick(&Ta));
	GetDlgItem(IDC_BN_THICK_STR)->SetWindowText(sThickStr);
}

void CDesignPadPlankParaDlg::OnOK() 
{
	CDialog::OnOK();
	char h[MAX_TEMP_CHAR_100+1];
	_snprintf(m_pPadPlankPara->sPartNo,MAX_CHAR_GUIGE_16,"%s",m_sPartNo);
	_snprintf(h,MAX_TEMP_CHAR_100,"%s",m_hBaseJg);
	sscanf(h,"%X",&m_pPadPlankPara->hBaseJg);
	_snprintf(h,MAX_TEMP_CHAR_100,"%s",m_hCutBaseJg);
	sscanf(h,"%X",&m_pPadPlankPara->hCutRefJg);
	_snprintf(m_pPadPlankPara->ls_str,MAX_TEMP_CHAR_200,"%s",m_sIncLsStr);
}
