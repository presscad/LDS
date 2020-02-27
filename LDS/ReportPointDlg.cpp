//<LOCALE_TRANSLATE Confirm by hxr/>
// ReportPointDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "ReportPointDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportPointDlg dialog


CReportPointDlg::CReportPointDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReportPointDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReportPointDlg)
	m_fPointX = 0.0;
	m_fPointY = 0.0;
	m_fPointZ = 0.0;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCaption = "point coordinate";
#else
	m_sCaption = "点坐标";
#endif
	m_bLockPoint = FALSE;
	m_bReadOnly = FALSE;
	m_bEnableLock = TRUE;
}


void CReportPointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReportPointDlg)
	DDX_Text(pDX, IDC_E_POINT_X, m_fPointX);
	DDX_Text(pDX, IDC_E_POINT_Y, m_fPointY);
	DDX_Text(pDX, IDC_E_POINT_Z, m_fPointZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReportPointDlg, CDialog)
	//{{AFX_MSG_MAP(CReportPointDlg)
	ON_BN_CLICKED(IDC_BN_COPY_POINT, OnBnCopyPoint)
	ON_BN_CLICKED(IDC_BN_PASTE_POINT, OnBnPastePoint)
	ON_BN_CLICKED(IDC_BN_LOCK_POINT, OnBnLockPoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportPointDlg message handlers

BOOL CReportPointDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(m_sCaption);	
	GetDlgItem(IDC_BN_LOCK_POINT)->EnableWindow(m_bEnableLock);
	if(m_bEnableLock&&m_bLockPoint)
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_BN_LOCK_POINT)->SetWindowText("unlock");
	else
		GetDlgItem(IDC_BN_LOCK_POINT)->SetWindowText("lock");
#else
		GetDlgItem(IDC_BN_LOCK_POINT)->SetWindowText("解锁");
	else
		GetDlgItem(IDC_BN_LOCK_POINT)->SetWindowText("锁定");
#endif
	GetDlgItem(IDC_BN_PASTE_POINT)->EnableWindow(!((m_bEnableLock&&m_bLockPoint)||m_bReadOnly));
	((CEdit*)GetDlgItem(IDC_E_POINT_X))->SetReadOnly((m_bEnableLock&&m_bLockPoint)||m_bReadOnly);
	((CEdit*)GetDlgItem(IDC_E_POINT_Y))->SetReadOnly((m_bEnableLock&&m_bLockPoint)||m_bReadOnly);
	((CEdit*)GetDlgItem(IDC_E_POINT_Z))->SetReadOnly((m_bEnableLock&&m_bLockPoint)||m_bReadOnly);
	return TRUE;
}

void CReportPointDlg::OnBnCopyPoint() 
{
	f3dPoint point;
	UpdateData();
	point.Set(m_fPointX,m_fPointY,m_fPointZ);
	WritePointToClip(point);
}

void CReportPointDlg::OnBnPastePoint() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fPointX=point.x;
		m_fPointY=point.y;
		m_fPointZ=point.z;
		UpdateData(FALSE);
	}
}

void CReportPointDlg::OnBnLockPoint() 
{
	UpdateData();
	m_bLockPoint=!m_bLockPoint;
	if(m_bLockPoint)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_BN_LOCK_POINT)->SetWindowText("unlock");
#else
		GetDlgItem(IDC_BN_LOCK_POINT)->SetWindowText("解锁");
#endif
		GetDlgItem(IDC_BN_PASTE_POINT)->EnableWindow(FALSE);
		((CEdit*)GetDlgItem(IDC_E_POINT_X))->SetReadOnly(m_bLockPoint);
		((CEdit*)GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(m_bLockPoint);
		((CEdit*)GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(m_bLockPoint);
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_BN_LOCK_POINT)->SetWindowText("lock");
#else
		GetDlgItem(IDC_BN_LOCK_POINT)->SetWindowText("锁定");
#endif
		GetDlgItem(IDC_BN_PASTE_POINT)->EnableWindow(!m_bReadOnly);
		((CEdit*)GetDlgItem(IDC_E_POINT_X))->SetReadOnly(m_bReadOnly);
		((CEdit*)GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(m_bReadOnly);
		((CEdit*)GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(m_bReadOnly);
	}
	UpdateData(FALSE);
}
