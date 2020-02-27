//<LOCALE_TRANSLATE Confirm by hxr/>
// ModifyVertexDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "ModifyVertexDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyVertexDlg dialog


CModifyVertexDlg::CModifyVertexDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyVertexDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyVertexDlg)
	m_fVertexPosX = 0.0;
	m_fVertexPosY = 0.0;
	m_fVertexPosZ = 0.0;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_sTitle = "modify vertex coordinates";
#else
	m_sTitle = "ÐÞ¸Ä¶¥µã×ø±ê";
#endif
	m_bEnableModify = TRUE;
}


void CModifyVertexDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyVertexDlg)
	DDX_Text(pDX, IDC_E_VERTEX_POS_X, m_fVertexPosX);
	DDX_Text(pDX, IDC_E_VERTEX_POS_Y, m_fVertexPosY);
	DDX_Text(pDX, IDC_E_VERTEX_POS_Z, m_fVertexPosZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyVertexDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyVertexDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_VERTEX, OnBnPasteVertex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyVertexDlg message handlers

void CModifyVertexDlg::OnBnPasteVertex() 
{
	f3dPoint point;
	if(m_bEnableModify&&ReadClipPoint(point))
	{
		m_fVertexPosX = point.x;
		m_fVertexPosY = point.y;
		m_fVertexPosZ = point.z;
		UpdateData(FALSE);
	}
	else if(!m_bEnableModify)
	{
		point.Set(m_fVertexPosX,m_fVertexPosY,m_fVertexPosZ);
		WritePointToClip(point);
	}
}

BOOL CModifyVertexDlg::OnInitDialog() 
{
	if(!m_bEnableModify)
	{
		CEdit* pEdit =(CEdit*)GetDlgItem(IDC_E_VERTEX_POS_X);
		pEdit->SetReadOnly();
		pEdit =(CEdit*)GetDlgItem(IDC_E_VERTEX_POS_Y);
		pEdit->SetReadOnly();
		pEdit =(CEdit*)GetDlgItem(IDC_E_VERTEX_POS_Z);
		pEdit->SetReadOnly();
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_BN_PASTE_VERTEX)->SetWindowText("copy");
#else
		GetDlgItem(IDC_BN_PASTE_VERTEX)->SetWindowText("¿½±´");
#endif
	}
	CDialog::OnInitDialog();
	
	SetWindowText(m_sTitle);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
