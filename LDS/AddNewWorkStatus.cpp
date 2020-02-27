// AddNewWorkStatus.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "AddNewWorkStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddNewWorkStatus dialog


CAddNewWorkStatus::CAddNewWorkStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CAddNewWorkStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddNewWorkStatus)
	m_sWorkStatus = _T("");
	m_fIceThick = 0.0;
	m_fMaxWind = 0.0;
	m_fWindAngle = 0;
	m_sModelLimStr = _T("1");
	m_bMinVertSpace = FALSE;
	m_sVibrationMode = _T("");
	m_cHoriEqAxisAzimuth = 90;
	//}}AFX_DATA_INIT
}


void CAddNewWorkStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddNewWorkStatus)
	DDX_Text(pDX, IDC_WORKSTATUS_NAME, m_sWorkStatus);
	DDX_Text(pDX, IDC_ICE_THICK, m_fIceThick);
	DDX_Text(pDX, IDC_MAXWIND, m_fMaxWind);
	DDX_Text(pDX, IDC_WIND_ANGLE, m_fWindAngle);
	DDX_Text(pDX, IDC_MODEL_LIM_STR, m_sModelLimStr);
	DDV_MaxChars(pDX, m_sModelLimStr, 50);
	DDX_Check(pDX, IDC_CHK_MIN_VERT_SPACE, m_bMinVertSpace);
	DDX_Text(pDX, IDC_E_VIBRATION_MODE_STR, m_sVibrationMode);
	DDV_MaxChars(pDX, m_sVibrationMode, 50);
	DDX_Text(pDX, IDC_E_HORI_EQ_AXIS_AZIMUTH, m_cHoriEqAxisAzimuth);
	DDV_MinMaxByte(pDX, m_cHoriEqAxisAzimuth, 0, 180);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddNewWorkStatus, CDialog)
	//{{AFX_MSG_MAP(CAddNewWorkStatus)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddNewWorkStatus message handlers
