// DrawPenPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "stdarx.h"
#include "MapSetPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawPenPage property page

IMPLEMENT_DYNCREATE(CDrawPenPage, CPropertyPage)

CDrawPenPage::CDrawPenPage() : CPropertyPage(CDrawPenPage::IDD)
{
	//{{AFX_DATA_INIT(CDrawPenPage)
	m_iHuoQuLine = 0;
	m_iAxisLine = 0;
	m_iSingleLine = 0;
	m_iVisibleOutline = 0;
	m_iLsHole = 0;
	m_iDashLine = 0;
	m_iDragLine = 0;
	m_iDimSimTri = 0;
	//}}AFX_DATA_INIT
}

CDrawPenPage::~CDrawPenPage()
{
}

void CDrawPenPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawPenPage)
	DDX_Text(pDX, IDC_E_HUOQU_LINE, m_iHuoQuLine);
	DDV_MinMaxInt(pDX, m_iHuoQuLine, 1, 255);
	DDX_Text(pDX, IDC_E_AXIS_LINE, m_iAxisLine);
	DDV_MinMaxInt(pDX, m_iAxisLine, 1, 255);
	DDX_Text(pDX, IDC_E_SINGLE_LINE, m_iSingleLine);
	DDV_MinMaxInt(pDX, m_iSingleLine, 1, 255);
	DDX_Text(pDX, IDC_E_VISIBLE_OUTLINE, m_iVisibleOutline);
	DDV_MinMaxInt(pDX, m_iVisibleOutline, 1, 255);
	DDX_Text(pDX, IDC_E_LS_HOLE, m_iLsHole);
	DDV_MinMaxInt(pDX, m_iLsHole, 1, 255);
	DDX_Text(pDX, IDC_E_DASH_LINE, m_iDashLine);
	DDV_MinMaxInt(pDX, m_iDashLine, 1, 255);
	DDX_Text(pDX, IDC_E_DRAG_LINE, m_iDragLine);
	DDX_Text(pDX, IDC_E_DIM_SIMTRI, m_iDimSimTri);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDrawPenPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDrawPenPage)
	ON_EN_CHANGE(IDC_E_HUOQU_LINE, OnChangeEHuoquLine)
	ON_EN_CHANGE(IDC_E_AXIS_LINE, OnChangeEAxisLine)
	ON_EN_CHANGE(IDC_E_SINGLE_LINE, OnChangeESingleLine)
	ON_EN_CHANGE(IDC_E_VISIBLE_OUTLINE, OnChangeEVisibleOutline)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_E_LS_HOLE, OnChangeELsHole)
	ON_EN_CHANGE(IDC_E_DIM_SIMTRI, OnChangeEDim)
	ON_EN_CHANGE(IDC_E_DASH_LINE, OnChangeEDashLine)
	ON_EN_CHANGE(IDC_E_DRAG_LINE, OnChangeEDragLine)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawPenPage message handlers

void CDrawPenPage::OnChangeEHuoquLine() 
{
	UpdateData();
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_HUOQU_LINE);
	pSpin->SetPos(m_iHuoQuLine);
	
}

void CDrawPenPage::OnChangeEAxisLine() 
{
	UpdateData();
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_AXIS_LINE);
	pSpin->SetPos(m_iAxisLine);
	
}

void CDrawPenPage::OnChangeESingleLine() 
{
	UpdateData();
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_SINGLE_LINE);
	pSpin->SetPos(m_iSingleLine);
	
}

void CDrawPenPage::OnChangeEVisibleOutline() 
{
	UpdateData();
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_VISIBLE_OUTLINE);
	pSpin->SetPos(m_iVisibleOutline);
	
}

void CDrawPenPage::OnChangeEDashLine() 
{
	UpdateData();
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DASH_LINE);
	pSpin->SetPos(m_iDashLine);
	
}

void CDrawPenPage::OnChangeELsHole() 
{
	UpdateData();
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_LS_HOLE);
	pSpin->SetPos(m_iDashLine);
	
}

void CDrawPenPage::OnChangeEDim() 
{
	UpdateData();
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DIM);
	pSpin->SetPos(m_iDimSimTri);
}

void CDrawPenPage::OnChangeEDragLine() 
{
	UpdateData();
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DRAG_LINE);
	pSpin->SetPos(m_iDragLine);
}

void CDrawPenPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if (nSBCode == SB_ENDSCROLL) {
        return; // Reject spurious messages
    }
    // Process scroll messages from IDC_SPIN only
    if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_VISIBLE_OUTLINE ||
		pScrollBar->GetDlgCtrlID() == IDC_SPIN_SINGLE_LINE||
		pScrollBar->GetDlgCtrlID() == IDC_SPIN_AXIS_LINE||
		pScrollBar->GetDlgCtrlID() == IDC_SPIN_HUOQU_LINE||
		pScrollBar->GetDlgCtrlID() == IDC_SPIN_DRAG_LINE)
	{
        CString strValue;
        strValue.Format("%d", nPos);
        ((CSpinButtonCtrl*) pScrollBar)->GetBuddy()
                                       ->SetWindowText(strValue);
    }
	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CDrawPenPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_E_VISIBLE_OUTLINE); 
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_VISIBLE_OUTLINE);
	pSpin->SetBuddy(pEdit);
	pSpin->SetRange(0, 255);
	pSpin->SetPos(m_iVisibleOutline);
	
	pEdit = (CEdit*) GetDlgItem(IDC_E_SINGLE_LINE); 
	pSpin =	(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_SINGLE_LINE);
	pSpin->SetBuddy(pEdit);
	pSpin->SetRange(0, 255);
	pSpin->SetPos(m_iSingleLine);
	
	pEdit = (CEdit*) GetDlgItem(IDC_E_AXIS_LINE); 
	pSpin =	(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_AXIS_LINE);
	pSpin->SetBuddy(pEdit);
	pSpin->SetRange(0, 255);
	pSpin->SetPos(m_iAxisLine);
	
	pEdit = (CEdit*) GetDlgItem(IDC_E_HUOQU_LINE); 
	pSpin =	(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_HUOQU_LINE);
	pSpin->SetBuddy(pEdit);
	pSpin->SetRange(0, 255);
	pSpin->SetPos(m_iHuoQuLine);
	
	pEdit = (CEdit*) GetDlgItem(IDC_E_DASH_LINE); 
	pSpin =	(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DASH_LINE);
	pSpin->SetBuddy(pEdit);
	pSpin->SetRange(0, 255);
	pSpin->SetPos(m_iDashLine);
	
	pEdit = (CEdit*) GetDlgItem(IDC_E_LS_HOLE); 
	pSpin =	(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_LS_HOLE);
	pSpin->SetBuddy(pEdit);
	pSpin->SetRange(0, 255);
	pSpin->SetPos(m_iLsHole);
	
	pEdit = (CEdit*) GetDlgItem(IDC_E_DIM_SIMTRI); 
	pSpin =	(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DIM);
	pSpin->SetBuddy(pEdit);
	pSpin->SetRange(0, 255);
	pSpin->SetPos(m_iDimSimTri);
	
	pEdit = (CEdit*) GetDlgItem(IDC_E_DRAG_LINE); 
	pSpin =	(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DRAG_LINE);
	pSpin->SetBuddy(pEdit);
	pSpin->SetRange(0, 255);
	pSpin->SetPos(m_iDragLine);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
