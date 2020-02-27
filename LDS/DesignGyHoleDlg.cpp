// DesignGyHoleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignGyHoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignGyHoleDlg dialog


CDesignGyHoleDlg::CDesignGyHoleDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CDesignGyHoleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignGyHoleDlg)
	m_sBoltSize = _T("12");
	m_fHoleD = 13.5;
	m_fParaH = 25.0;
	m_bIncDownHole = TRUE;
	m_bIncUpHole = TRUE;
	m_fParaS = 25.0;
	m_bIncludeHoleR = TRUE;
	//}}AFX_DATA_INIT
}


void CDesignGyHoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignGyHoleDlg)
	DDX_CBString(pDX, IDC_CMB_BOLT_SIZE, m_sBoltSize);
	DDX_Text(pDX, IDC_E_HOLE_D, m_fHoleD);
	DDX_Text(pDX, IDC_E_HOLE_PARA_H, m_fParaH);
	DDX_Check(pDX, IDC_CHK_INC_DOWN, m_bIncDownHole);
	DDX_Check(pDX, IDC_CHK_INC_UP, m_bIncUpHole);
	DDX_Text(pDX, IDC_E_HOLE_PARA_S, m_fParaS);
	DDX_Check(pDX, IDC_CHK_INCLUDE_HOLE_R, m_bIncludeHoleR);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignGyHoleDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CDesignGyHoleDlg)
	ON_CBN_SELCHANGE(IDC_CMB_BOLT_SIZE, OnSelchangeBoltSize)
	ON_EN_CHANGE(IDC_E_HOLE_PARA_H, OnChangeHoleParaH)
	ON_BN_CLICKED(IDC_CHK_INCLUDE_HOLE_R, OnIncludeHoleR)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignGyHoleDlg message handlers

BOOL CDesignGyHoleDlg::OnInitDialog() 
{
	/*CRect rect;
	GetWindowRect(rect);
	rect.right = rect.Width();
	rect.left = 0;
	rect.bottom = rect.Height();
	rect.top = 0;
	MoveWindow(rect,TRUE);*/

	CDialog::OnInitDialog();
	//移动对话框到左上角
	MoveWndToLocation();
	return TRUE;
}

void CDesignGyHoleDlg::OnSelchangeBoltSize() 
{
	CString sText;
	CComboBox *pCmb = (CComboBox*)GetDlgItem(IDC_CMB_BOLT_SIZE);
	int iCur = pCmb->GetCurSel();
	UpdateData();
	pCmb->GetLBText(iCur,m_sBoltSize);
	long d=atoi(m_sBoltSize);
	if(d==12)
		m_fHoleD = 13.5;
	else if(d==16)
		m_fHoleD = 17.5;
	else if(d==20)
		m_fHoleD = 21.5;
	else if(d==24)
		m_fHoleD = 25.5;
	else if(d==27)
		m_fHoleD = 35;
	else if(d==30)
		m_fHoleD = 40;
	else if(d==36)
		m_fHoleD = 45;
	else if(d==42)
		m_fHoleD = 55;
	else if(d==45)
		m_fHoleD = 60;
	else if(d==48)
		m_fHoleD = 60;
	else if(d==52)
		m_fHoleD = 65;
	else if(d==56)
		m_fHoleD = 70;
	else if(d==60)
		m_fHoleD = 75;
	else if(d==64)
		m_fHoleD = 80;	
	UpdateData(FALSE);
}

void CDesignGyHoleDlg::OnChangeHoleParaH() 
{
	UpdateData();
	m_fParaS=m_fParaH;
	UpdateData(FALSE);
}

void CDesignGyHoleDlg::OnIncludeHoleR() 
{
	UpdateData();
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	m_bmpCirPlate.Detach();
	if(m_bIncludeHoleR)
		m_bmpCirPlate.LoadBitmap(IDB_BMP_GY_HOLE1);
	else
		m_bmpCirPlate.LoadBitmap(IDB_BMP_GY_HOLE);
	pPic->SetBitmap(m_bmpCirPlate);
	UpdateData(FALSE);
}
