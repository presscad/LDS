// SuppPartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "SuppPartDlg.h"
#include "CfgPartNoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSuppPartDlg dialog


CSuppPartDlg::CSuppPartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSuppPartDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSuppPartDlg)
	m_sNotes = _T("");
	m_sPartGuige = _T("");
	m_fPartLength = 0.0;
	m_sPartNo = _T("");
	m_nPartNum = 0;
	m_fPieceWeight = 0.0;
	m_sSegNo = _T("");
	m_iMaterial = 0;
	m_iPartType = 0;
	m_sCfgPartNo = _T("");
	m_bLegPart = FALSE;
	//}}AFX_DATA_INIT
	m_cMaterial = 'S';
	cfgword.flag.word[0]=1;
}


void CSuppPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSuppPartDlg)
	DDX_Text(pDX, IDC_E_NOTES, m_sNotes);
	DDV_MaxChars(pDX, m_sNotes, 49);
	DDX_Text(pDX, IDC_E_PART_GUIGE, m_sPartGuige);
	DDV_MaxChars(pDX, m_sPartGuige, 15);
	DDX_Text(pDX, IDC_E_PART_LENGTH, m_fPartLength);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDV_MaxChars(pDX, m_sPartNo, 15);
	DDX_Text(pDX, IDC_E_PART_NUM, m_nPartNum);
	DDX_Text(pDX, IDC_E_PIECE_WEIGHT, m_fPieceWeight);
	DDX_Text(pDX, IDC_E_SEG_NO, m_sSegNo);
	DDV_MaxChars(pDX, m_sSegNo, 20);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_CBIndex(pDX, IDC_CMB_PART_TYPE, m_iPartType);
	DDX_Text(pDX, IDC_E_CFG_PART_NO, m_sCfgPartNo);
	DDX_Check(pDX, IDC_CHK_LEG_PART, m_bLegPart);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSuppPartDlg, CDialog)
	//{{AFX_MSG_MAP(CSuppPartDlg)
	ON_BN_CLICKED(IDC_BN_CFG_PART, OnBnCfgPart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuppPartDlg message handlers

BOOL CSuppPartDlg::OnInitDialog() 
{
	if(m_cMaterial==CSteelMatLibrary::Q235BriefMark())//'S')
		m_iMaterial=0;
	else if(m_cMaterial==CSteelMatLibrary::Q345BriefMark())//'H')
		m_iMaterial=1;
	else if(m_cMaterial==CSteelMatLibrary::Q390BriefMark())//'G')
		m_iMaterial=2;
	else if(m_cMaterial==CSteelMatLibrary::Q420BriefMark())//'P')
		m_iMaterial=3;
	else
		m_iMaterial=4;
	UpdateCfgPartNoString();
	
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CSuppPartDlg::UpdateCfgPartNoString()
{
	m_sCfgPartNo.Empty();
	for(int i=0;i<192;i++)
	{
		if(cfgword.IsHasNo(i+1))
		{
			CString sText;
			sText.Format("%d",i+1);
			if(m_sCfgPartNo.GetLength()<=0)
				m_sCfgPartNo=sText;
			else
				m_sCfgPartNo+=","+sText;
		}
	}
	UpdateData(FALSE);
}

void CSuppPartDlg::OnOK() 
{
	CDialog::OnOK();
	if(m_iMaterial==0)
		m_cMaterial=CSteelMatLibrary::Q235BriefMark();//'S';
	else if(m_iMaterial==1)
		m_cMaterial=CSteelMatLibrary::Q345BriefMark();//'H';
	else if(m_iMaterial==2)
		m_cMaterial=CSteelMatLibrary::Q390BriefMark();//'G';
	else if(m_iMaterial==3)
		m_cMaterial=CSteelMatLibrary::Q420BriefMark();//'P';
	else
		m_cMaterial=CSteelMatLibrary::Q390BriefMark();//'T';
}

void CSuppPartDlg::OnBnCfgPart() 
{
	CCfgPartNoDlg dlg;
	dlg.cfgword = cfgword;
	if(dlg.DoModal()==IDOK)
	{
		cfgword = dlg.cfgword;
		UpdateCfgPartNoString();
	}
	
}
