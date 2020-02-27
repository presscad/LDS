// PartTablePreviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "PartTablePreviewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartTablePreviewDlg dialog


CPartTablePreviewDlg::CPartTablePreviewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartTablePreviewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartTablePreviewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPartTablePreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartTablePreviewDlg)
	DDX_Control(pDX, IDC_PART_LIST, m_listPart);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartTablePreviewDlg, CDialog)
	//{{AFX_MSG_MAP(CPartTablePreviewDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartTablePreviewDlg message handlers

void CPartTablePreviewDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	CWnd *pPartListWnd=GetDlgItem(IDC_PART_LIST);
	if(pPartListWnd==NULL)
		return;
	RECT rcList;
	pPartListWnd->GetClientRect(&rcList);
	rcList.left+=10;
	rcList.top+=40;
	rcList.right=cx-10;
	rcList.bottom=cy-10;
	pPartListWnd->MoveWindow(&rcList);
}

BOOL CPartTablePreviewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;
}
