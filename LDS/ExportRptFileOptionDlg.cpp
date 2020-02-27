// ExportRptFileOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "ExportRptFileOptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportRptFileOptionDlg dialog


CExportRptFileOptionDlg::CExportRptFileOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExportRptFileOptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExportRptFileOptionDlg)
	m_iDocFileType = 0;
	m_iFileLanguage = 0;
	m_iEfficType = 0;
	//}}AFX_DATA_INIT
}


void CExportRptFileOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportRptFileOptionDlg)
	DDX_CBIndex(pDX, IDC_CMB_FILE_TYPE, m_iDocFileType);
	DDX_CBIndex(pDX, IDC_CMB_FILE_LANGUAGE, m_iFileLanguage);
	DDX_CBIndex(pDX, IDC_CMB_FILE_EFFIC_TYPE, m_iEfficType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportRptFileOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CExportRptFileOptionDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportRptFileOptionDlg message handlers
