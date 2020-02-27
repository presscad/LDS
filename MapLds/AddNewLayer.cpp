// AddNewLayer.cpp : implementation file
//

#include "stdafx.h"
#include "maplds.h"
#include "AddNewLayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AddNewLayer dialog


AddNewLayer::AddNewLayer(CWnd* pParent /*=NULL*/)
	: CDialog(AddNewLayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(AddNewLayer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AddNewLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddNewLayer)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddNewLayer, CDialog)
	//{{AFX_MSG_MAP(AddNewLayer)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddNewLayer message handlers
