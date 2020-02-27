// CompareLinePartPosDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "CompareLinePartPosDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompareLinePartPosDlg dialog


CCompareLinePartPosDlg::CCompareLinePartPosDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompareLinePartPosDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCompareLinePartPosDlg)
	m_sLinePartHandle1 =  _T("");
	m_sLinePartHandle2 =  _T("");
	m_sLinePart2PartNo = _T("");
	m_sLinePart1PartNo = _T("");
	m_fEndOddment1 = 0.0;
	m_fEndPosX1 = 0.0;
	m_fEndPosY1 = 0.0;
	m_fEndPosZ1 = 0.0;
	m_fStartOddment1 = 0.0;
	m_fStartPosX1 = 0.0;
	m_fStartPosY1 = 0.0;
	m_fStartPosZ1 = 0.0;
	m_fEndOddment2 = 0.0;
	m_fEndPosX2 = 0.0;
	m_fEndPosY2 = 0.0;
	m_fEndPosZ2 = 0.0;
	m_fStartOddment2 = 0.0;
	m_fStartPosX2 = 0.0;
	m_fStartPosY2 = 0.0;
	m_fStartPosZ2 = 0.0;
	m_fLinePartLen1 = 0.0;
	m_fLinePartLen2 = 0.0;
	//}}AFX_DATA_INIT
	m_pDatumLinePart1=NULL;
	m_pDatumLinePart2=NULL;
	m_bPauseBreakExit1=FALSE;
	m_bPauseBreakExit2=FALSE;
}


void CCompareLinePartPosDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompareLinePartPosDlg)
	DDX_Text(pDX, IDC_E_LINEPART_HANDLE1, m_sLinePartHandle1);
	DDX_Text(pDX, IDC_E_LINEPART_HANDLE2, m_sLinePartHandle2);
	DDX_Text(pDX, IDC_E_LINEPART_SPARTNO2, m_sLinePart2PartNo);
	DDX_Text(pDX, IDC_E_LINEPART_PARTNO1, m_sLinePart1PartNo);
	DDX_Text(pDX, IDC_E_LINEPART1_END_ODDMENT, m_fEndOddment1);
	DDX_Text(pDX, IDC_E_LINEPART1_END_POS_X, m_fEndPosX1);
	DDX_Text(pDX, IDC_E_LINEPART1_END_POS_Y, m_fEndPosY1);
	DDX_Text(pDX, IDC_E_LINEPART1_END_POS_Z, m_fEndPosZ1);
	DDX_Text(pDX, IDC_E_LINEPART1_START_ODDMENT, m_fStartOddment1);
	DDX_Text(pDX, IDC_E_LINEPART1_START_POS_X, m_fStartPosX1);
	DDX_Text(pDX, IDC_E_LINEPART1_START_POS_Y, m_fStartPosY1);
	DDX_Text(pDX, IDC_E_LINEPART1_START_POS_Z, m_fStartPosZ1);
	DDX_Text(pDX, IDC_E_LINEPART2_END_ODDMENT, m_fEndOddment2);
	DDX_Text(pDX, IDC_E_LINEPART2_END_POS_X, m_fEndPosX2);
	DDX_Text(pDX, IDC_E_LINEPART2_END_POS_Y, m_fEndPosY2);
	DDX_Text(pDX, IDC_E_LINEPART2_END_POS_Z, m_fEndPosZ2);
	DDX_Text(pDX, IDC_E_LINEPART2_START_ODDMENT, m_fStartOddment2);
	DDX_Text(pDX, IDC_E_LINEPART2_START_POS_X, m_fStartPosX2);
	DDX_Text(pDX, IDC_E_LINEPART2_START_POS_Y, m_fStartPosY2);
	DDX_Text(pDX, IDC_E_LINEPART2_START_POS_Z, m_fStartPosZ2);
	DDX_Text(pDX, IDC_E_LINEPART_LEN1, m_fLinePartLen1);
	DDX_Text(pDX, IDC_E_LINEPART_LEN2, m_fLinePartLen2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompareLinePartPosDlg, CDialog)
	//{{AFX_MSG_MAP(CCompareLinePartPosDlg)
	ON_BN_CLICKED(IDC_BTN_SELECT_LINEPART1, OnSelectLinePart1)
	ON_BN_CLICKED(IDC_BTN_SELECT_LINEPART2, OnSelectLinePart2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompareLinePartPosDlg message handlers

BOOL CCompareLinePartPosDlg::OnInitDialog() 
{
	m_bPauseBreakExit1=FALSE;
	m_bPauseBreakExit2=FALSE;
	if(m_pDatumLinePart1==NULL||m_pDatumLinePart2==NULL)
		return FALSE;
	if(m_pDatumLinePart1->GetClassTypeId()!=m_pDatumLinePart2->GetClassTypeId())
		return FALSE;
	UpdateLinePartPos();
	CDialog::OnInitDialog();
	return TRUE;
}

void CCompareLinePartPosDlg::UpdateLinePartPos()
{
	m_sLinePartHandle1.Format("0X%X",m_pDatumLinePart1->handle);
	m_sLinePart1PartNo.Format("%s",(char*)m_pDatumLinePart1->GetPartNo());
	m_fStartPosX1=m_pDatumLinePart1->Start().x;
	m_fStartPosY1=m_pDatumLinePart1->Start().y;
	m_fStartPosZ1=m_pDatumLinePart1->Start().z;
	m_fEndPosX1=m_pDatumLinePart1->End().x;
	m_fEndPosY1=m_pDatumLinePart1->End().y;
	m_fEndPosZ1=m_pDatumLinePart1->End().z;
	m_fLinePartLen1=m_pDatumLinePart1->GetLength();
	m_sLinePartHandle2.Format("0X%X",m_pDatumLinePart2->handle);
	m_sLinePart2PartNo.Format("%s",(char*)m_pDatumLinePart2->GetPartNo());
	m_fStartPosX2=m_pDatumLinePart2->Start().x;
	m_fStartPosY2=m_pDatumLinePart2->Start().y;
	m_fStartPosZ2=m_pDatumLinePart2->Start().z;
	m_fEndPosX2=m_pDatumLinePart2->End().x;
	m_fEndPosY2=m_pDatumLinePart2->End().y;
	m_fEndPosZ2=m_pDatumLinePart2->End().z;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	m_fStartOddment1=m_pDatumLinePart1->startOdd();
	m_fEndOddment1=m_pDatumLinePart1->endOdd();
	m_fStartOddment2=m_pDatumLinePart2->startOdd();
	m_fEndOddment2=m_pDatumLinePart2->endOdd();
#endif
	m_fLinePartLen2=m_pDatumLinePart2->GetLength();
	UpdateData(FALSE);
}

void CCompareLinePartPosDlg::OnSelectLinePart1() 
{
	//m_bPauseBreakExit1=TRUE;
	//CDialog::OnOK();
}

void CCompareLinePartPosDlg::OnSelectLinePart2() 
{
	//m_bPauseBreakExit2=TRUE;
	//CDialog::OnOK();
}
