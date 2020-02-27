// AddVecOffsetItem.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "AddVecOffsetItemDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddVecOffsetItemDlg dialog


CAddVecOffsetItemDlg::CAddVecOffsetItemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddVecOffsetItemDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddVecOffsetItemDlg)
	m_fVecX = 0.0;
	m_fVecY = 0.0;
	m_fVecZ = 0.0;
	m_fOffsetDist = 0.0;
	//}}AFX_DATA_INIT
	m_bModifyItem = FALSE;
	m_bDelItem = FALSE;
}


void CAddVecOffsetItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddVecOffsetItemDlg)
	DDX_Text(pDX, IDC_E_VEC_X, m_fVecX);
	DDX_Text(pDX, IDC_E_VEC_Y, m_fVecY);
	DDX_Text(pDX, IDC_E_VEC_Z, m_fVecZ);
	DDX_Text(pDX, IDC_E_OFFSET_DIST, m_fOffsetDist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddVecOffsetItemDlg, CDialog)
	//{{AFX_MSG_MAP(CAddVecOffsetItemDlg)
	ON_BN_CLICKED(IDC_BTN_COPY, OnCopy)
	ON_BN_CLICKED(IDC_BTN_PAST, OnPast)
	ON_BN_CLICKED(IDC_BTN_REVERSE, OnReverse)
	ON_BN_CLICKED(IDC_BTN_DEL, OnDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddVecOffsetItemDlg message handlers

void CAddVecOffsetItemDlg::OnCopy() 
{
	f3dPoint point;
	UpdateData();
	point.Set(m_fVecX,m_fVecY,m_fVecZ);
	WritePointToClip(point);
}

void CAddVecOffsetItemDlg::OnPast() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fVecX=point.x;
		m_fVecY=point.y;
		m_fVecZ=point.z;
		UpdateData(FALSE);
	}
}

void CAddVecOffsetItemDlg::OnReverse() 
{
	UpdateData();
	m_fVecX *= -1.0;
	m_fVecY *= -1.0;
	m_fVecZ *= -1.0;
	UpdateData(FALSE);
}

void CAddVecOffsetItemDlg::OnDel() 
{
	m_bDelItem = TRUE;
	CDialog::OnCancel();
}

BOOL CAddVecOffsetItemDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_bModifyItem)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Edit offset item");
		SetDlgItemText(IDOK,"Modification");
#else 
		SetWindowText("±à¼­Æ«ÒÆÏî");
		SetDlgItemText(IDOK,"ÐÞ¸Ä");
#endif
		GetDlgItem(IDC_BTN_DEL)->ShowWindow(SW_SHOW);
	}
	else 
	{
		OnPast();
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Add offset item");
#else 
		SetWindowText("Ìí¼ÓÆ«ÒÆÏî");
#endif
		GetDlgItem(IDC_BTN_DEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_COPY)->ShowWindow(SW_HIDE);
		//
		int offset_dist=25;
		CRect rc;
		GetWindowRect(rc);
		rc.bottom -= offset_dist;
		MoveWindow(rc);	
		//
		long id_arr[2]={IDCANCEL,IDOK};
		for(int i=0;i<2;i++)
		{
			CWnd *pSonWnd = GetDlgItem(id_arr[i]);
			if(pSonWnd==NULL)
				continue;
			CRect rcSon;
			pSonWnd->GetWindowRect(rcSon);
			ScreenToClient(rcSon);
			rcSon.top-=offset_dist;
			rcSon.bottom-=offset_dist;
			pSonWnd->MoveWindow(rcSon);
		}
	}
	return TRUE;  
}
