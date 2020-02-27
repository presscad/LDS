// NodeDesignPosDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "NodeDesignPosDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNodeDesignPosDlg dialog


CNodeDesignPosDlg::CNodeDesignPosDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNodeDesignPosDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNodeDesignPosDlg)
	m_fOffsetWingX = 0.0;
	m_fOffsetWingY = 0.0;
	m_iWingXOffsetStyle = 0;
	m_iWingYOffsetStyle = 0;
	//}}AFX_DATA_INIT
	memset(&wing_x_jgzj,0,sizeof(JGZJ));
	memset(&wing_y_jgzj,0,sizeof(JGZJ));
}


void CNodeDesignPosDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNodeDesignPosDlg)
	DDX_Text(pDX, IDC_E_OFFSET_WING_X, m_fOffsetWingX);
	DDX_Text(pDX, IDC_E_OFFSET_WING_Y, m_fOffsetWingY);
	DDX_Radio(pDX, IDC_RDO_WING_X_OFFSET, m_iWingXOffsetStyle);
	DDX_Radio(pDX, IDC_RDO_WING_Y_OFFSET, m_iWingYOffsetStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNodeDesignPosDlg, CDialog)
	//{{AFX_MSG_MAP(CNodeDesignPosDlg)
	ON_BN_CLICKED(IDC_RDO_WING_X_OFFSET, OnRdoWingXOffset)
	ON_BN_CLICKED(IDC_RDO_WING_Y_OFFSET, OnRdoWingYOffset)
	ON_BN_CLICKED(IDC_RDO_WING_X_OFFSET2, OnRdoWingXOffset)
	ON_BN_CLICKED(IDC_RDO_WING_X_OFFSET3, OnRdoWingXOffset)
	ON_BN_CLICKED(IDC_RDO_WING_X_OFFSET4, OnRdoWingXOffset)
	ON_BN_CLICKED(IDC_RDO_WING_X_OFFSET5, OnRdoWingXOffset)
	ON_BN_CLICKED(IDC_RDO_WING_Y_OFFSET2, OnRdoWingYOffset)
	ON_BN_CLICKED(IDC_RDO_WING_Y_OFFSET3, OnRdoWingYOffset)
	ON_BN_CLICKED(IDC_RDO_WING_Y_OFFSET4, OnRdoWingYOffset)
	ON_BN_CLICKED(IDC_RDO_WING_Y_OFFSET5, OnRdoWingYOffset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNodeDesignPosDlg message handlers

BOOL CNodeDesignPosDlg::OnInitDialog() 
{
	if(m_iWingXOffsetStyle!=4)
	{
		switch(m_iWingXOffsetStyle)
		{
		case 0:
			m_fOffsetWingX = wing_x_jgzj.g;
			break;
		case 1:
			m_fOffsetWingX = wing_x_jgzj.g1;
			break;
		case 2:
			m_fOffsetWingX = wing_x_jgzj.g2;
			break;
		case 3:
			m_fOffsetWingX = wing_x_jgzj.g3;
		default:
			m_fOffsetWingX = 0;
			break;
		}
		GetDlgItem(IDC_E_OFFSET_WING_X)->EnableWindow(FALSE);
	}
	else
		GetDlgItem(IDC_E_OFFSET_WING_X)->EnableWindow(TRUE);
	if(m_iWingYOffsetStyle!=4)
	{
		switch(m_iWingYOffsetStyle)
		{
		case 0:
			m_fOffsetWingY = wing_y_jgzj.g;
			break;
		case 1:
			m_fOffsetWingY = wing_y_jgzj.g1;
			break;
		case 2:
			m_fOffsetWingY = wing_y_jgzj.g2;
			break;
		case 3:
			m_fOffsetWingY = wing_y_jgzj.g3;
		default:
			m_fOffsetWingY = 0;
			break;
		}
		GetDlgItem(IDC_E_OFFSET_WING_Y)->EnableWindow(FALSE);
	}
	else
		GetDlgItem(IDC_E_OFFSET_WING_Y)->EnableWindow(TRUE);
	
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNodeDesignPosDlg::OnRdoWingXOffset() 
{
	UpdateData();
	if(m_iWingXOffsetStyle!=4)
	{
		switch(m_iWingXOffsetStyle)
		{
		case 0:
			m_fOffsetWingX = wing_x_jgzj.g;
			break;
		case 1:
			m_fOffsetWingX = wing_x_jgzj.g1;
			break;
		case 2:
			m_fOffsetWingX = wing_x_jgzj.g2;
			break;
		case 3:
			m_fOffsetWingX = wing_x_jgzj.g3;
			break;
		default:
			m_fOffsetWingX = 0;
			break;
		}
		GetDlgItem(IDC_E_OFFSET_WING_X)->EnableWindow(FALSE);
	}
	else
		GetDlgItem(IDC_E_OFFSET_WING_X)->EnableWindow(TRUE);
	UpdateData(FALSE);
}

void CNodeDesignPosDlg::OnRdoWingYOffset() 
{
	UpdateData();
	if(m_iWingYOffsetStyle!=4)
	{
		switch(m_iWingYOffsetStyle)
		{
		case 0:
			m_fOffsetWingY = wing_y_jgzj.g;
			break;
		case 1:
			m_fOffsetWingY = wing_y_jgzj.g1;
			break;
		case 2:
			m_fOffsetWingY = wing_y_jgzj.g2;
			break;
		case 3:
			m_fOffsetWingY = wing_y_jgzj.g3;
			break;
		default:
			m_fOffsetWingY = 0;
			break;
		}
		GetDlgItem(IDC_E_OFFSET_WING_Y)->EnableWindow(FALSE);
	}
	else
		GetDlgItem(IDC_E_OFFSET_WING_Y)->EnableWindow(TRUE);
	UpdateData(FALSE);
}
