// PlateAttachLaunderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "PlateAttachLaunderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlateAttachLaunderDlg dialog


CPlateAttachLaunderDlg::CPlateAttachLaunderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlateAttachLaunderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlateAttachLaunderDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPlateAttachLaunderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlateAttachLaunderDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlateAttachLaunderDlg, CDialog)
	//{{AFX_MSG_MAP(CPlateAttachLaunderDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlateAttachLaunderDlg message handlers
