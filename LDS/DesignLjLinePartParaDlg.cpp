// DesignLjLinePartParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignLjLinePartParaDlg.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignLjLinePartParaDlg dialog


CDesignLjLinePartParaDlg::CDesignLjLinePartParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignLjLinePartParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignLjLinePartParaDlg)
	m_iCurLjEnd = 0;
	m_iCurLjWing = 0;
	m_iLjFace = 0;
	m_fBerSpace = 30.0;
	m_fEndSpace = 30;
	m_bTwoEdgeVertice = FALSE;
	m_hLinePart = _T("0X0");
	m_fWingSpace = 30;
	//}}AFX_DATA_INIT
}


void CDesignLjLinePartParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignLjLinePartParaDlg)
	DDX_CBIndex(pDX, IDC_CMB_CURLJ_END, m_iCurLjEnd);
	DDX_CBIndex(pDX, IDC_CMB_CURLJ_WING, m_iCurLjWing);
	DDX_CBIndex(pDX, IDC_CMB_LJ_FACE, m_iLjFace);
	DDX_Text(pDX, IDC_E_BER_SPACE, m_fBerSpace);
	DDX_Text(pDX, IDC_E_END_SPACE, m_fEndSpace);
	DDX_Check(pDX, IDC_CHK_TWO_EDGE_VERTICE, m_bTwoEdgeVertice);
	DDX_Text(pDX, IDC_E_LINEPART_HANDLE, m_hLinePart);
	DDX_Text(pDX, IDC_E_WING_SPACE, m_fWingSpace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignLjLinePartParaDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignLjLinePartParaDlg)
	ON_EN_CHANGE(IDC_E_LINEPART_HANDLE, OnChangeLinepartHandle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignLjLinePartParaDlg message handlers

void CDesignLjLinePartParaDlg::OnChangeLinepartHandle() 
{
	UpdateData();
	char sText[MAX_TEMP_CHAR_100+1]="";
	long hLinePart=0;
	_snprintf(sText,MAX_TEMP_CHAR_100,"%s",m_hLinePart);
	sscanf(sText,"%X",&hLinePart);
	CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(hLinePart,CLS_LINEANGLE,CLS_LINETUBE);
	if(pLinePart&&pLinePart->GetClassTypeId()==CLS_LINETUBE)
	{
		GetDlgItem(IDC_CMB_CURLJ_WING)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_WING_SPACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_TWO_EDGE_VERTICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BER_SPACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_END_SPACE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CMB_CURLJ_WING)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_WING_SPACE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_TWO_EDGE_VERTICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BER_SPACE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_END_SPACE)->EnableWindow(TRUE);
	}
}

BOOL CDesignLjLinePartParaDlg::OnInitDialog() 
{
	char sText[MAX_TEMP_CHAR_100+1];
	long hLinePart=0;
	_snprintf(sText,MAX_TEMP_CHAR_100,"%s",m_hLinePart);
	sscanf(sText,"%X",&hLinePart);
	CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(hLinePart,CLS_LINEANGLE,CLS_LINETUBE);
	if(pLinePart&&pLinePart->GetClassTypeId()==CLS_LINEANGLE)
	{
		GetDlgItem(IDC_CMB_CURLJ_WING)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_WING_SPACE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_TWO_EDGE_VERTICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BER_SPACE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_END_SPACE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CMB_CURLJ_WING)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_WING_SPACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_TWO_EDGE_VERTICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BER_SPACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_END_SPACE)->EnableWindow(FALSE);
	}
	CDialog::OnInitDialog();
	return TRUE;
}
