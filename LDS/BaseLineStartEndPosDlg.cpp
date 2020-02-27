// BaseLineStartEndPosDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "BaseLineStartEndPosDlg.h"
#include "GlobalFunc.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBaseLineStartEndPosDlg dialog


CBaseLineStartEndPosDlg::CBaseLineStartEndPosDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBaseLineStartEndPosDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBaseLineStartEndPosDlg)
	m_fPointX = 0.0;
	m_fPointY = 0.0;
	m_fPointZ = 0.0;
	//}}AFX_DATA_INIT
	m_pWorkPart=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCaption=_T("Datum Line Start Coordinate");
#else 
	m_sCaption=_T("基准线起点坐标");
#endif
}


void CBaseLineStartEndPosDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBaseLineStartEndPosDlg)
	DDX_Text(pDX, IDC_E_POINT_X, m_fPointX);
	DDX_Text(pDX, IDC_E_POINT_Y, m_fPointY);
	DDX_Text(pDX, IDC_E_POINT_Z, m_fPointZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBaseLineStartEndPosDlg, CDialog)
	//{{AFX_MSG_MAP(CBaseLineStartEndPosDlg)
	ON_BN_CLICKED(IDC_BTN_NEW_POINT, OnNewPoint)
	ON_BN_CLICKED(IDC_BNT_COPY_POINT, OnCopyPoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseLineStartEndPosDlg message handlers

void CBaseLineStartEndPosDlg::OnNewPoint() 
{
	UpdateData();
	CLDSNode *pNode=console.AppendNode();
	pNode->SetPosition(m_fPointX,m_fPointY,m_fPointZ);
	pNode->m_cPosCalType = 0;//无任何依赖
	if(m_pWorkPart)
	{
		pNode->iSeg=m_pWorkPart->iSeg;
		pNode->SetLayer(m_pWorkPart->layer());
		pNode->layer(1)='P';
		pNode->hFatherPart=m_pWorkPart->handle;
	}
	NewNode(pNode);
	g_pSolidDraw->SetEntSnapStatus(pNode->handle);
	CDialog::OnOK();
}

void CBaseLineStartEndPosDlg::OnCopyPoint() 
{
	f3dPoint point;
	UpdateData();
	point.Set(m_fPointX,m_fPointY,m_fPointZ);
	WritePointToClip(point);
}

BOOL CBaseLineStartEndPosDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(m_sCaption);
	return TRUE;
}
