// ShortJgDefDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "ShortJgDefDlg.h"
#include "Query.h"
#include "env_def.h"
#include ".\shortjgdefdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShortJgDefDlg dialog
#ifndef __TSA_

CShortJgDefDlg::CShortJgDefDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShortJgDefDlg::IDD, pParent)
	, m_fManuSpace(1)
{
	//{{AFX_DATA_INIT(CShortJgDefDlg)
	m_sJgGuiGe = _T("");
	m_iJunctPosStyle = 0;
	m_sPartNo = _T("");
	m_iMaterial = 0;
	m_sWingXPadPartNo = _T("");
	m_sWingYPadPartNo = _T("");
	//}}AFX_DATA_INIT
	m_nThickDiff=0;
}


void CShortJgDefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShortJgDefDlg)
	DDX_CBString(pDX, IDC_CMB_SHORTJG_GUIGE, m_sJgGuiGe);
	DDX_CBIndex(pDX, IDC_CMB_JUNCTION_POS_STYLE, m_iJunctPosStyle);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDV_MaxChars(pDX, m_sPartNo, 15);
	DDX_CBIndex(pDX, IDC_CMB_JG_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_WING_X_PAD_PART_NO, m_sWingXPadPartNo);
	DDV_MaxChars(pDX, m_sWingXPadPartNo, 15);
	DDX_Text(pDX, IDC_E_WING_Y_PAD_PART_NO, m_sWingYPadPartNo);
	DDV_MaxChars(pDX, m_sWingYPadPartNo, 15);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_E_MANU_SPACE, m_fManuSpace);
	DDV_MinMaxDouble(pDX, m_fManuSpace, 0, 10);
}


BEGIN_MESSAGE_MAP(CShortJgDefDlg, CDialog)
	//{{AFX_MSG_MAP(CShortJgDefDlg)
	ON_CBN_SELCHANGE(IDC_CMB_JUNCTION_POS_STYLE, OnCbnSelchangeCmbJunctionPosStyle)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShortJgDefDlg message handlers

BOOL CShortJgDefDlg::OnInitDialog() 
{
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_SHORTJG_GUIGE);
	AddJgRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_JG_MATERIAL);
	AddSteelMatRecord(pCMB);
	for(int i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		if(CSteelMatLibrary::RecordAt(i).cBriefMark==info.cMaterial)
		{
			m_iMaterial=i;
			break;
		}
	}
	//连接角钢肢厚相同时可能也需要设置加工间隙 wht 15-09-21
	GetDlgItem(IDC_E_MANU_SPACE)->EnableWindow(m_iJunctPosStyle==0);
	if(m_nThickDiff>1&&m_iJunctPosStyle==0)
	{	
		GetDlgItem(IDC_E_WING_X_PAD_PART_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_WING_Y_PAD_PART_NO)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_E_WING_X_PAD_PART_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_WING_Y_PAD_PART_NO)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);
	CDialog::OnInitDialog();
	
	return TRUE;  
}

void CShortJgDefDlg::OnCbnSelchangeCmbJunctionPosStyle()
{
	UpdateData();
	if(m_nThickDiff>1&&m_iJunctPosStyle==0)
	{
		GetDlgItem(IDC_E_MANU_SPACE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_WING_X_PAD_PART_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_WING_Y_PAD_PART_NO)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_E_MANU_SPACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_WING_X_PAD_PART_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_WING_Y_PAD_PART_NO)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);
}

void CShortJgDefDlg::OnBnClickedOk()
{
	OnOK();

	info.cJunctionStyle=(char)m_iJunctPosStyle;
	info.cMaterial=CSteelMatLibrary::RecordAt(m_iMaterial).cBriefMark;
	info.sizeStr.Copy(m_sJgGuiGe);
	info.sPartNo.Copy(m_sPartNo);
	info.xPadPartNo.Copy(m_sWingXPadPartNo);
	info.yPadPartNo.Copy(m_sWingYPadPartNo);
	info.manu_space=m_fManuSpace;
}
#endif
