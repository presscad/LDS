// ModifyStdPartNoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "ModifyStdPartNODlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyStdPartNoDlg dialog


CModifyStdPartNoDlg::CModifyStdPartNoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyStdPartNoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyStdPartNoDlg)
	m_iPartType = -1;
	m_sPartNo = _T("");
	m_iPartMaterial = -1;
	m_iPartLevel = -1;
	m_iMainTubeMaterial = -1;
	m_iMainTubeGuige = -1;
	//}}AFX_DATA_INIT
}


void CModifyStdPartNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyStdPartNoDlg)
	DDX_CBIndex(pDX, IDC_CMB_PART_TYPE, m_iPartType);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_CBIndex(pDX, IDC_CMB_PART_MATERIAL, m_iPartMaterial);
	DDX_CBIndex(pDX, IDC_CMB_PART_LEVEL, m_iPartLevel);
	DDX_CBIndex(pDX, IDC_CMB_MAIN_TUBE_MATERIAL, m_iMainTubeMaterial);
	DDX_CBIndex(pDX, IDC_CMB_MAIN_TUBE_GUIGE, m_iMainTubeGuige);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyStdPartNoDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyStdPartNoDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyStdPartNoDlg message handlers
