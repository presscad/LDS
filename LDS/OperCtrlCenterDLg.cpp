// OperCtrlCenterDLg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "OperCtrlCenterDLg.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "GlobalFunc.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COperCtrlCenterDLg dialog


COperCtrlCenterDLg::COperCtrlCenterDLg(CWnd* pParent /*=NULL*/)
	: CDialog(COperCtrlCenterDLg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COperCtrlCenterDLg)
	m_fRotOrgX = 0.0;
	m_fRotOrgY = 0.0;
	m_fRotOrgZ = 0.0;
	m_iZoomCenterStyle = 0;
	//}}AFX_DATA_INIT
}


void COperCtrlCenterDLg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COperCtrlCenterDLg)
	DDX_Text(pDX, IDC_E_ROT_ORIGIN_X, m_fRotOrgX);
	DDX_Text(pDX, IDC_E_ROT_ORIGIN_Y, m_fRotOrgY);
	DDX_Text(pDX, IDC_E_ROT_ORIGIN_Z, m_fRotOrgZ);
	DDX_Radio(pDX, IDC_RDO_ZOOM_STYLE, m_iZoomCenterStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COperCtrlCenterDLg, CDialog)
	//{{AFX_MSG_MAP(COperCtrlCenterDLg)
	ON_BN_CLICKED(IDC_BN_PASTE, OnBnPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COperCtrlCenterDLg message handlers

void COperCtrlCenterDLg::OnBnPaste() 
{
	f3dPoint point;
	UpdateData(TRUE);
	if(ReadClipPoint(point))
	{
		m_fRotOrgX = point.x;
		m_fRotOrgY = point.y;
		m_fRotOrgZ = point.z;
		UpdateData(FALSE);
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("No data can be paste");
#else 
		AfxMessageBox("无数据可粘贴");
#endif
}
