// SelCurDesignFaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "SelCurDesignFaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelCurDesignFaceDlg dialog


CSelCurDesignFaceDlg::CSelCurDesignFaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelCurDesignFaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelCurDesignFaceDlg)
	m_iCurFace = 0;
	//}}AFX_DATA_INIT
}


void CSelCurDesignFaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelCurDesignFaceDlg)
	DDX_Radio(pDX, IDC_RDO_CUR_FACE, m_iCurFace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelCurDesignFaceDlg, CDialog)
	//{{AFX_MSG_MAP(CSelCurDesignFaceDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelCurDesignFaceDlg message handlers
