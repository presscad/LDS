// ConfirmArcPartStyleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "ConfirmArcPartStyleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CConfirmArcPartStyleDlg dialog


CConfirmArcPartStyleDlg::CConfirmArcPartStyleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmArcPartStyleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmArcPartStyleDlg)
	m_iArcPartLayStyle = 1;
	//}}AFX_DATA_INIT
	m_iArcPartType=1;
}


void CConfirmArcPartStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmArcPartStyleDlg)
	DDX_Radio(pDX, IDC_RDO_ARC_PART_STYLE, m_iArcPartLayStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmArcPartStyleDlg, CDialog)
	//{{AFX_MSG_MAP(CConfirmArcPartStyleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmArcPartStyleDlg message handlers

BOOL CConfirmArcPartStyleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_iArcPartType==0)
	{	//环形角钢
		//1
		CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_LAY1);
		m_CurPlateBmp.Detach();
		m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_ANGLE_LAY1);
		pPic->SetBitmap(m_CurPlateBmp);
		//2
		pPic=(CStatic*)GetDlgItem(IDC_S_LAY2);
		m_CurPlateBmp.Detach();
		m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_ANGLE_LAY2);
		pPic->SetBitmap(m_CurPlateBmp);
		//3
		pPic=(CStatic*)GetDlgItem(IDC_S_LAY3);
		m_CurPlateBmp.Detach();
		m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_ANGLE_LAY3);
		pPic->SetBitmap(m_CurPlateBmp);
		//4
		pPic=(CStatic*)GetDlgItem(IDC_S_LAY4);
		m_CurPlateBmp.Detach();
		m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_ANGLE_LAY4);
		pPic->SetBitmap(m_CurPlateBmp);
	}
	else if(m_iArcPartType==1)
	{	//环形槽钢
		//1
		CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_LAY1);
		m_CurPlateBmp.Detach();
		m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_SLOT_LAY1);
		pPic->SetBitmap(m_CurPlateBmp);
		//2
		pPic=(CStatic*)GetDlgItem(IDC_S_LAY2);
		m_CurPlateBmp.Detach();
		m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_SLOT_LAY2);
		pPic->SetBitmap(m_CurPlateBmp);
		//3
		pPic=(CStatic*)GetDlgItem(IDC_S_LAY3);
		m_CurPlateBmp.Detach();
		m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_SLOT_LAY3);
		pPic->SetBitmap(m_CurPlateBmp);
		//4
		pPic=(CStatic*)GetDlgItem(IDC_S_LAY4);
		m_CurPlateBmp.Detach();
		m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_SLOT_LAY4);
		pPic->SetBitmap(m_CurPlateBmp);
	}
	return TRUE;
}
#endif