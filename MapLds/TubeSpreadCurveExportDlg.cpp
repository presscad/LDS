// TubeSpreadCurveExportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TubeSpreadCurveExportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTubeSpreadCurveExportDlg dialog


CTubeSpreadCurveExportDlg::CTubeSpreadCurveExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTubeSpreadCurveExportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTubeSpreadCurveExportDlg)
	m_bInnerSpreadCurve = FALSE;
	m_bOutterSpreadCurve = FALSE;
	m_bInnerOutterComposeCurve = TRUE;
	m_bProcessSpreadCurve = FALSE;
	//}}AFX_DATA_INIT
}


void CTubeSpreadCurveExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTubeSpreadCurveExportDlg)
	DDX_Check(pDX, IDC_CHK_INNER_SPREAD_CURVE, m_bInnerSpreadCurve);
	DDX_Check(pDX, IDC_CHK_OUTTER_SPREAD_CURVE, m_bOutterSpreadCurve);
	DDX_Check(pDX, IDC_CHK_INNER_OUTTER_COMPOSE_SPREAD_CURVE, m_bInnerOutterComposeCurve);
	DDX_Check(pDX, IDC_CHK_PROCESS_SPREAD_CURVE, m_bProcessSpreadCurve);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTubeSpreadCurveExportDlg, CDialog)
	//{{AFX_MSG_MAP(CTubeSpreadCurveExportDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTubeSpreadCurveExportDlg message handlers
