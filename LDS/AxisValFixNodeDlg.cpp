// AxisValFixNodeDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "AxisValFixNodeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAxisValFixNodeDlg dialog


CAxisValFixNodeDlg::CAxisValFixNodeDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CAxisValFixNodeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAxisValFixNodeDlg)
	m_fFixValX = 0.0;
	m_fFixValY = 0.0;
	m_fFixValZ = 0.0;
	m_iFixAxis = 2;
	//}}AFX_DATA_INIT
	m_bCanFixCoordX=m_bCanFixCoordY=m_bCanFixCoordZ=TRUE;
}


void CAxisValFixNodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAxisValFixNodeDlg)
	DDX_Text(pDX, IDC_E_X_VAL, m_fFixValX);
	DDX_Text(pDX, IDC_E_Y_VAL, m_fFixValY);
	DDX_Text(pDX, IDC_E_Z_VAL, m_fFixValZ);
	DDX_Radio(pDX, IDC_RDO_AXIS, m_iFixAxis);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAxisValFixNodeDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CAxisValFixNodeDlg)
	ON_BN_CLICKED(IDC_RDO_AXIS, OnRdoAxis)
	ON_BN_CLICKED(IDC_RDO_AXIS2, OnRdoAxis)
	ON_BN_CLICKED(IDC_RDO_AXIS3, OnRdoAxis)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAxisValFixNodeDlg message handlers

BOOL CAxisValFixNodeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	OnRdoAxis();
	MoveWndToLocation();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAxisValFixNodeDlg::OnRdoAxis() 
{
	UpdateData();
	if(m_iFixAxis==0)
	{
		GetDlgItem(IDC_E_X_VAL)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_Y_VAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_Z_VAL)->EnableWindow(FALSE);
	}
	else if(m_iFixAxis==1)
	{
		GetDlgItem(IDC_E_X_VAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_Y_VAL)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_Z_VAL)->EnableWindow(FALSE);
	}
	else
	{
		m_iFixAxis=2;
		GetDlgItem(IDC_E_X_VAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_Y_VAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_Z_VAL)->EnableWindow(TRUE);
	}
}

void CAxisValFixNodeDlg::OnOK() 
{
	if( (!m_bCanFixCoordX&&m_iFixAxis==0)||
		(!m_bCanFixCoordY&&m_iFixAxis==1)||
		(!m_bCanFixCoordZ&&m_iFixAxis==2))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can't define the node of current axia component！");
#else 
		AfxMessageBox("不能定义指定当前轴向分量的节点！");
#endif
			return;
	}
	CDialog::OnOK();
}
