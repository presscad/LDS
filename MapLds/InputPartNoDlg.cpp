//<LOCALE_TRANSLATE/>
// InputPartNoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "InputPartNoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputPartNoDlg dialog


CInputPartNoDlg::CInputPartNoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputPartNoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputPartNoDlg)
	m_sPartNo = _T("");
#ifdef AFX_TARG_ENU_ENGLISH
	m_sValName = _T("part number");
	//}}AFX_DATA_INIT
	m_sDlgTitle="input part number";
#else
	m_sValName = _T("构件编号");
	//}}AFX_DATA_INIT
	m_sDlgTitle="输入构件编号";
#endif
}


void CInputPartNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputPartNoDlg)
	DDX_Text(pDX, IDC_E_PART_CODE, m_sPartNo);
	DDX_Text(pDX, IDC_S_VAL_NAME, m_sValName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputPartNoDlg, CDialog)
	//{{AFX_MSG_MAP(CInputPartNoDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputPartNoDlg message handlers
