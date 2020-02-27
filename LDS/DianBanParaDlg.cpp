//<LOCALE_TRANSLATE Confirm by hxr/>
// DianBanParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DianBanParaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDianBanParaDlg dialog

CDianBanParaDlg::CDianBanParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDianBanParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDianBanParaDlg)
	m_sPartNo = _T("");
	m_nThick = 0;
	m_nSpaceDist = 0;
	m_nNum = 0;
	m_nPadThick=0;
	m_nWasherThick=0;
	m_bCreateSurplusPlate = FALSE;
	m_sPartNoSurplus = _T("");
#ifdef AFX_TARG_ENU_ENGLISH
	m_sNoSurplus = _T("Number:");
	m_sNo = _T("Gasket number:");
	m_iPartType = 0;
	m_sThick = _T("thickness of gasket:");
#else
	m_sNoSurplus = _T("编号:");
	m_sNo = _T("垫板编号:");
	m_iPartType = 0;
	m_sThick = _T("垫板厚度:");
#endif
	m_bAutoJudgePad = TRUE;
	m_fPadLen = 0.0;
	m_fPadWidth = 0.0;
	m_sLsGuiGe = _T("20");
	m_bAutoJudgeLs = TRUE;
	//}}AFX_DATA_INIT
	m_bMirCreate=FALSE;
}


void CDianBanParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDianBanParaDlg)
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_THICK, m_nThick);
	DDX_Text(pDX, IDC_S_SPACE_DIST, m_nSpaceDist);
	DDX_Text(pDX, IDC_E_NUM, m_nNum);
	DDX_Check(pDX, IDC_CHK_CREATE_SURPLUS_PLATE, m_bCreateSurplusPlate);
	DDX_Text(pDX, IDC_E_PART_NO_SURPLUS, m_sPartNoSurplus);
	DDX_Text(pDX, IDC_S_NO_SURPLUS, m_sNoSurplus);
	DDX_Text(pDX, IDC_S_NO, m_sNo);
	DDX_Radio(pDX, IDC_RDO_PLATE_TYPE, m_iPartType);
	DDX_Text(pDX, IDC_S_THICK, m_sThick);
	DDX_Check(pDX, IDC_CHK_AUTO_JUDGE_PAD, m_bAutoJudgePad);
	DDX_Text(pDX, IDC_E_PAD_LEN, m_fPadLen);
	DDX_Text(pDX, IDC_E_PAD_WIDTH, m_fPadWidth);
	DDX_CBString(pDX, IDC_CMB_LS_GUIGE, m_sLsGuiGe);
	DDX_Check(pDX, IDC_CHK_AUTO_JUDGE_LS, m_bAutoJudgeLs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDianBanParaDlg, CDialog)
	//{{AFX_MSG_MAP(CDianBanParaDlg)
	ON_EN_CHANGE(IDC_E_THICK, OnChangeThick)
	ON_BN_CLICKED(IDC_CHK_CREATE_SURPLUS_PLATE, OnChkCreateSurplusPlate)
	ON_BN_CLICKED(IDC_RDO_PLATE_TYPE, OnRdoPlateType)
	ON_BN_CLICKED(IDC_CHK_AUTO_JUDGE_LS, OnChkAutoJudgeLs)
	ON_BN_CLICKED(IDC_CHK_AUTO_JUDGE_PAD, OnChkAutoJudgePad)
	ON_BN_CLICKED(IDC_RDO_WASHER, OnRdoPlateType)
	ON_CBN_SELCHANGE(IDC_CMB_LS_GUIGE, OnSelchangeLsGuige)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDianBanParaDlg message handlers
BOOL CDianBanParaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	OnChangeThick();
	m_bCreateSurplusPlate=FALSE;
	if(m_nThick==0)
		return FALSE;
	CComboBox* pLsGuigeCmb=(CComboBox*)GetDlgItem(IDC_CMB_LS_GUIGE);
	pLsGuigeCmb->ResetContent();
	pLsGuigeCmb->AddString("12");
	pLsGuigeCmb->AddString("16");
	pLsGuigeCmb->AddString("20");
	pLsGuigeCmb->AddString("24");
	//
	m_nPadThick=m_nThick;
	m_nNum=m_nSpaceDist/m_nPadThick;
	if(m_nNum==0)
		m_nNum=1;	//应保证垫板数最小为1 wht 16-01-05
	if(m_nSpaceDist==m_nNum*m_nPadThick)
		GetDlgItem(IDC_CHK_CREATE_SURPLUS_PLATE)->EnableWindow(FALSE);
	else if(m_iPartType == 0)
		GetDlgItem(IDC_CHK_CREATE_SURPLUS_PLATE)->EnableWindow(TRUE);
	GetDlgItem(IDC_E_PART_NO_SURPLUS)->EnableWindow(m_bCreateSurplusPlate);
	GetDlgItem(IDC_E_PART_NO)->EnableWindow(m_iPartType==0);
	int nLsD = atoi(m_sLsGuiGe);
	LSSPACEEX space;
	if(IBoltLibrary::GetLsSpaceEx(nLsD,&space)&&space.WasherEdgeSpace>0)
		m_fPadLen=m_fPadWidth=space.WasherEdgeSpace*2;
	else
		m_fPadLen=m_fPadWidth=30*2;
	UpdateData(FALSE);
	GetDlgItem(IDC_CMB_LS_GUIGE)->EnableWindow(!m_bAutoJudgeLs);
	GetDlgItem(IDC_E_PAD_LEN)->EnableWindow(!m_bAutoJudgePad);	
	GetDlgItem(IDC_E_PAD_WIDTH)->EnableWindow(!m_bAutoJudgePad);
	OnRdoPlateType(); 
	return TRUE;
}
void CDianBanParaDlg::OnChangeThick() 
{
	UpdateData();
	//有时临时计算的通厚间隙不准，需要手动给大一点的垫圈厚度 wjh-2015.5.19
	if(m_nThick==0)
		return;
	if(m_iPartType==0)	//垫板
	{
		m_nPadThick=m_nThick;
		m_nNum=m_nSpaceDist/m_nPadThick;
		if(m_nNum==0)
			m_nNum=1;	//应保证垫板数最小为1 wht 16-01-05
	}
	else
	{
		m_nWasherThick=m_nThick;
		m_nNum=m_nSpaceDist/m_nWasherThick;
	}
	if(m_nSpaceDist==m_nNum*m_nThick)
	{
		GetDlgItem(IDC_CHK_CREATE_SURPLUS_PLATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_PART_NO_SURPLUS)->EnableWindow(FALSE);
	}
	else if(m_iPartType == 0)
	{
		GetDlgItem(IDC_CHK_CREATE_SURPLUS_PLATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_PART_NO_SURPLUS)->EnableWindow(m_bCreateSurplusPlate);
	}
	UpdateData(FALSE);
}

void CDianBanParaDlg::OnChkCreateSurplusPlate() 
{
	UpdateData();
	GetDlgItem(IDC_E_PART_NO_SURPLUS)->EnableWindow(m_bCreateSurplusPlate);
	UpdateData(FALSE);
}

void CDianBanParaDlg::OnRdoPlateType() 
{
	UpdateData();
	if(m_iPartType == 1)
	{
		m_nWasherThick=CLDSPart::library->GetBestMatchWasherThick(atoi(m_sLsGuiGe),m_nSpaceDist);
		m_nThick=m_nWasherThick>0? m_nWasherThick: m_nSpaceDist;	//否则m_nThick有作为除数为零的情况 wjh-2019.5.19
		m_nNum=m_nSpaceDist/m_nThick;
		//
		GetDlgItem(IDC_E_PART_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_CREATE_SURPLUS_PLATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_PART_NO_SURPLUS)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_AUTO_JUDGE_PAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_PAD_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_PAD_WIDTH)->EnableWindow(FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sThick = "thickness of washer:";
#else
		m_sThick = "垫圈厚度:";
#endif
	}
	else
	{
		m_nThick=m_nPadThick;
		m_nNum=m_nSpaceDist/m_nThick;
		if(m_nNum==0)
			m_nNum=1;	//应保证垫板数最小为1 wht 16-01-05
		if(m_nSpaceDist==m_nNum*m_nThick)
		{
			GetDlgItem(IDC_CHK_CREATE_SURPLUS_PLATE)->EnableWindow(FALSE);
			m_bCreateSurplusPlate=FALSE;
		}
		else
			GetDlgItem(IDC_CHK_CREATE_SURPLUS_PLATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_PART_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_AUTO_JUDGE_PAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_PAD_LEN)->EnableWindow(!m_bAutoJudgePad);	
		GetDlgItem(IDC_E_PAD_WIDTH)->EnableWindow(!m_bAutoJudgePad);
		GetDlgItem(IDC_E_PART_NO_SURPLUS)->EnableWindow(m_bCreateSurplusPlate);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sThick = "thickness of gasket:";
#else
		m_sThick = "垫板厚度:";
#endif
	}
	UpdateData(FALSE);
}

void CDianBanParaDlg::OnChkAutoJudgeLs() 
{
	UpdateData();
	GetDlgItem(IDC_CMB_LS_GUIGE)->EnableWindow(!m_bAutoJudgeLs);
	UpdateData(FALSE);
}

void CDianBanParaDlg::OnChkAutoJudgePad() 
{
	UpdateData();
	GetDlgItem(IDC_E_PAD_LEN)->EnableWindow(!m_bAutoJudgePad);	
	GetDlgItem(IDC_E_PAD_WIDTH)->EnableWindow(!m_bAutoJudgePad);	
	UpdateData(FALSE);
}

void CDianBanParaDlg::OnSelchangeLsGuige() 
{
	UpdateData();
	CComboBox *pCMB = (	CComboBox*)GetDlgItem(IDC_CMB_LS_GUIGE);
	int ii=pCMB->GetCurSel();
	pCMB->GetLBText(ii,m_sLsGuiGe);
	int nLsD = atoi(m_sLsGuiGe);
	LSSPACEEX space;
	IBoltLibrary::GetLsSpaceEx(nLsD,&space);
	int end_space=space.WasherEdgeSpace>0?space.WasherEdgeSpace:space.EndSpace;
	m_fPadLen=m_fPadWidth=end_space*2;
	if(m_iPartType==1)
	{
		m_nWasherThick=CLDSPart::library->GetBestMatchWasherThick(nLsD,m_nSpaceDist);
		if(m_nWasherThick==0)
			m_nWasherThick=3;
		m_nThick=m_nWasherThick;
		m_nNum=m_nSpaceDist/m_nThick;
	}
	UpdateData(FALSE);
}

void CDianBanParaDlg::CopyProperty(CDianBanParaDlg &dlg)
{
	m_fPadLen=dlg.m_fPadLen;
	m_fPadWidth=dlg.m_fPadWidth;
	m_sPartNo.Format("%s",dlg.m_sPartNo);
	m_iPartType=dlg.m_iPartType;
	m_fPadLen=dlg.m_fPadLen;
	m_fPadWidth=dlg.m_fPadWidth;
	m_nThick=dlg.m_nThick;
	m_nSpaceDist=dlg.m_nSpaceDist;
	m_nNum=dlg.m_nNum;
	m_sLsGuiGe.Format("%s",dlg.m_sLsGuiGe);
	m_bCreateSurplusPlate=dlg.m_bCreateSurplusPlate;
	m_bAutoJudgeLs=dlg.m_bAutoJudgeLs;
	m_bAutoJudgePad=dlg.m_bAutoJudgePad;
	m_sPartNoSurplus.Format("%s",dlg.m_sPartNoSurplus);
	m_sNoSurplus.Format("%s",dlg.m_sNoSurplus);
	m_sNo.Format("%s",dlg.m_sNo);
	m_sThick.Format("%s",dlg.m_sThick);
}