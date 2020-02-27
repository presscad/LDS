// SetMapScaleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SetMapScaleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetMapScaleDlg dialog


CSetMapScaleDlg::CSetMapScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetMapScaleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetMapScaleDlg)
	m_nLineMapScale = 200;
	m_nForceLineScale = 50;
	m_nPartMapScale = 20;
	m_nStruMapScale = 5;
	//}}AFX_DATA_INIT
}


void CSetMapScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetMapScaleDlg)
	DDX_Text(pDX, IDC_E_LINE_MAP_SCALE, m_nLineMapScale);
	DDV_MinMaxInt(pDX, m_nLineMapScale, 1, 500);
	DDX_Text(pDX, IDC_E_LINE_MAP_SCALE, m_nForceLineScale);
	DDV_MinMaxInt(pDX, m_nLineMapScale, 1, 500);
	DDX_Text(pDX, IDC_E_PART_MAP_SCALE, m_nPartMapScale);
	DDV_MinMaxInt(pDX, m_nPartMapScale, 1, 500);
	DDX_Text(pDX, IDC_E_STRU_MAP_SCALE, m_nStruMapScale);
	DDV_MinMaxInt(pDX, m_nStruMapScale, 1, 500);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetMapScaleDlg, CDialog)
	//{{AFX_MSG_MAP(CSetMapScaleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetMapScaleDlg message handlers
