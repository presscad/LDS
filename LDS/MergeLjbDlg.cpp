// MergeLjbDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "tma_part.h"
#include "MergeLjbDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeLjbDlg dialog


CMergeLjbDlg::CMergeLjbDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMergeLjbDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMergeLjbDlg)
	m_iAttachFace = 0;
	m_iBaseFace = 0;
	m_hBasePlank = _T("0X0");
	m_hAttachPlank = _T("0X0");
	//}}AFX_DATA_INIT
	m_pBaseAttach=m_pViceAttach=NULL;
}


void CMergeLjbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMergeLjbDlg)
	DDX_CBIndex(pDX, IDC_CMB_ATTACH_FACE_SEL, m_iAttachFace);
	DDX_CBIndex(pDX, IDC_CMB_BASE_FACE_SEL, m_iBaseFace);
	DDX_Text(pDX, IDC_S_BASE_PLANK_HANDLE, m_hBasePlank);
	DDX_Text(pDX, IDC_S_ATTACH_PLANK_HANDLE, m_hAttachPlank);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMergeLjbDlg, CDialog)
	//{{AFX_MSG_MAP(CMergeLjbDlg)
	ON_CBN_SELCHANGE(IDC_CMB_BASE_FACE_SEL, OnSelchangeCmbBaseFaceSel)
	ON_CBN_SELCHANGE(IDC_CMB_ATTACH_FACE_SEL, OnSelchangeCmbAttachFaceSel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeLjbDlg message handlers

void CMergeLjbDlg::OnSelchangeCmbBaseFaceSel() 
{
	UpdateData();
	if(m_pBaseAttach)
		m_hBasePlank.Format("0X%X",m_pBaseAttach->lj_handle_arr[m_iBaseFace]);
	else
		m_hBasePlank="0X0";
	UpdateData(FALSE);
}

void CMergeLjbDlg::OnSelchangeCmbAttachFaceSel() 
{
	UpdateData();
	if(m_pViceAttach)
		m_hAttachPlank.Format("0X%X",m_pViceAttach->lj_handle_arr[m_iAttachFace]);
	else
		m_hAttachPlank="0X0";
	UpdateData(FALSE);
}

BOOL CMergeLjbDlg::OnInitDialog() 
{
	if(m_pBaseAttach)
		m_hBasePlank.Format("0X%X",m_pBaseAttach->lj_handle_arr[m_iBaseFace]);
	else
		m_hBasePlank="0X0";
	if(m_pViceAttach)
		m_hAttachPlank.Format("0X%X",m_pViceAttach->lj_handle_arr[m_iAttachFace]);
	else
		m_hAttachPlank="0X0";
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
