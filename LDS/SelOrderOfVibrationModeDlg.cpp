//<LOCALE_TRANSLATE Confirm by hxr/>
// SelOrderOfVibrationModeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "XhListCtrl.h"
#include "SelOrderOfVibrationModeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelOrderOfVibrationModeDlg dialog
#if defined(__LDS_)||defined(__TSA_)

CSelOrderOfVibrationModeDlg::CSelOrderOfVibrationModeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelOrderOfVibrationModeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelOrderOfVibrationModeDlg)
	m_fMaxAmplitude = 0.0;
	m_nSimulateVibraModePeriod = 1000;
	m_nSampleTimerInterval = 100;
	//}}AFX_DATA_INIT
	modes_arr=NULL;
	m_iSelOrder=0;
	m_nOrderNum=1;
#ifdef AFX_TARG_ENU_ENGLISH
	m_listVibraMode.AddColumnHeader("vibration mode",40);
	m_listVibraMode.AddColumnHeader("self-vibration frequency",80);
	m_listVibraMode.AddColumnHeader("self-vibration period",80);
#else
	m_listVibraMode.AddColumnHeader("振型",40);
	m_listVibraMode.AddColumnHeader("自振频率",80);
	m_listVibraMode.AddColumnHeader("自振周期",80);
#endif
	//m_listVibraMode.m_arrHeaderLabel.Add("X向参预系数");
	//m_listVibraMode.m_arrHeaderLabel.Add("Y向参预系数");
}


void CSelOrderOfVibrationModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelOrderOfVibrationModeDlg)
	DDX_Control(pDX, IDC_VIBRATION_MODE_INFO, m_listVibraMode);
	DDX_Text(pDX, IDC_E_MAX_AMPLITUDE, m_fMaxAmplitude);
	DDV_MinMaxDouble(pDX, m_fMaxAmplitude, 1., 100000.);
	DDX_Text(pDX, IDC_E_SIMULATE_PERIOD, m_nSimulateVibraModePeriod);
	DDV_MinMaxInt(pDX, m_nSimulateVibraModePeriod, 100, 10000);
	DDX_Text(pDX, IDC_E_SIMULATE_SAMPLE_TIMER_INTERVAL, m_nSampleTimerInterval);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelOrderOfVibrationModeDlg, CDialog)
	//{{AFX_MSG_MAP(CSelOrderOfVibrationModeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelOrderOfVibrationModeDlg message handlers

BOOL CSelOrderOfVibrationModeDlg::OnInitDialog() 
{
	if(modes_arr==NULL)
		return FALSE;
	CDialog::OnInitDialog();
	m_listVibraMode.InitListCtrl();
	CStringArray str_arr;
	str_arr.SetSize(3);
	for(int i=0;i<m_nOrderNum;i++)
	{
		modes_arr[i].period=1/modes_arr[i].freqs;
		str_arr[0].Format("%d",i+1);
		str_arr[1].Format("%.3f",modes_arr[i].freqs);
		str_arr[2].Format("%.3f",modes_arr[i].period);
		//str_arr[3].Format("%.3f",modes_arr[i].gama_x);
		//str_arr[4].Format("%.3f",modes_arr[i].gama_y);
		int iItem=m_listVibraMode.InsertItemRecord(-1,str_arr);
	}
	m_listVibraMode.SetItemState(m_iSelOrder,LVIS_SELECTED|LVIS_FOCUSED ,LVIS_SELECTED|LVIS_FOCUSED);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelOrderOfVibrationModeDlg::OnOK() 
{
	POSITION pos=m_listVibraMode.GetFirstSelectedItemPosition();
	if(pos!=NULL)
		m_iSelOrder=m_listVibraMode.GetNextSelectedItem(pos);
	
	CDialog::OnOK();
}
#endif