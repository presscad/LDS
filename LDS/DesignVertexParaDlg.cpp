// DesignVertexParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignVertexParaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignVertexParaDlg dialog


CDesignVertexParaDlg::CDesignVertexParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignVertexParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignVertexParaDlg)
	m_fPosX = 0.0;
	m_fPosY = 0.0;
	m_iFaceI = 0;
	//}}AFX_DATA_INIT
}


void CDesignVertexParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignVertexParaDlg)
	DDX_Text(pDX, IDC_E_PT_X, m_fPosX);
	DDX_Text(pDX, IDC_E_PT_Y, m_fPosY);
	DDX_CBIndex(pDX, IDC_CMB_FACE_I, m_iFaceI);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignVertexParaDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignVertexParaDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignVertexParaDlg message handlers
