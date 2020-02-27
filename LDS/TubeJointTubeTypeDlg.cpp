// TubeJointTubeTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "TubeJointTubeTypeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTubeJointTubeTypeDlg dialog


CTubeJointTubeTypeDlg::CTubeJointTubeTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTubeJointTubeTypeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTubeJointTubeTypeDlg)
	m_iFl = 0;
	//}}AFX_DATA_INIT
}


void CTubeJointTubeTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTubeJointTubeTypeDlg)
	DDX_Radio(pDX, IDC_RDO_FL, m_iFl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTubeJointTubeTypeDlg, CDialog)
	//{{AFX_MSG_MAP(CTubeJointTubeTypeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTubeJointTubeTypeDlg message handlers
