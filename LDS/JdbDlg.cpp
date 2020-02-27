#include "stdafx.h"
#include "resource.h"
#include "JdbDlg.h"
#include "Query.h"
#include "env_def.h"
#include "GlobalFunc.h"
/////////////////////////////////////////////////////////////////////////////
// CJdbJgJointDlg dialog


CJdbJgJointDlg::CJdbJgJointDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJdbJgJointDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJdbJgJointDlg)
	m_nLsN = 0;
	m_nLsSpace = 0;
	m_fOddment = 0.0;
	m_iLsLayOutStyle = 0;
	m_nLsRowsN = 0;
	m_sRayJgGuiGe = _T("");
	m_nG = 0;
	m_nG1 = 0;
	m_nG2 = 0;
	m_nLsEndSpace = 0;
	m_sLsGuiGe = _T("6.8M20");
	//}}AFX_DATA_INIT
	m_bEnable2RowsLs = TRUE;
}


void CJdbJgJointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJdbJgJointDlg)
	DDX_Text(pDX, IDC_EDIT_LS_NUM, m_nLsN);
	DDX_Text(pDX, IDC_EDIT_LS_SPACE, m_nLsSpace);
	DDX_Text(pDX, IDC_EDIT_ODDMENT, m_fOddment);
	DDX_Radio(pDX, IDC_LS_LAYOUT_STYLE, m_iLsLayOutStyle);
	DDX_CBIndex(pDX, IDC_CMB_LS_ROWS, m_nLsRowsN);
	DDX_Text(pDX, IDC_S_RAY_JG_GUIGE, m_sRayJgGuiGe);
	DDX_Text(pDX, IDC_E_G, m_nG);
	DDV_MinMaxInt(pDX, m_nG, 0, 200);
	DDX_Text(pDX, IDC_E_G1, m_nG1);
	DDV_MinMaxInt(pDX, m_nG1, 0, 200);
	DDX_Text(pDX, IDC_E_G2, m_nG2);
	DDV_MinMaxInt(pDX, m_nG2, 0, 200);
	DDX_Text(pDX, IDC_E_LS_END_SPACE, m_nLsEndSpace);
	DDX_CBString(pDX, IDC_CMB_LS_GUIGE, m_sLsGuiGe);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJdbJgJointDlg, CDialog)
	//{{AFX_MSG_MAP(CJdbJgJointDlg)
	ON_CBN_SELCHANGE(IDC_CMB_LS_GUIGE, OnSelchangeCmbLsGuige)
	ON_CBN_SELCHANGE(IDC_CMB_LS_ROWS, OnSelchangeCmbLsGuige)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJdbJgJointDlg message handlers

BOOL CJdbJgJointDlg::OnInitDialog() 
{
	//获得基准螺栓直径
	//CLsSpace LsSpace;
	if(m_bEnable2RowsLs&&m_nLsN>4)	//允许多排螺栓
		m_nLsRowsN = 1;	//螺栓数大于4,推荐双排螺栓布孔
	else		//单排螺栓
		m_nLsRowsN = 0;
	double wing_wide,wing_thick;
	restore_JG_guige(m_sRayJgGuiGe.GetBuffer(),wing_wide,wing_thick);
	m_nG  = jgzj.g;
	m_nG1 = jgzj.g1;
	m_nG2 = jgzj.g2;
	CComboBox *pCmb = (CComboBox*)GetDlgItem(IDC_CMB_LS_ROWS);
	if(jgzj.g1<=0)
	{
		m_nLsRowsN = 0;
		m_bEnable2RowsLs = FALSE;
	}
	pCmb->EnableWindow(m_bEnable2RowsLs);
	//获得基准螺栓间距表
	int ls_d;
	sscanf(m_sLsGuiGe,"%d",&ls_d);
	if(GetLsSpace(LsSpace, ls_d))
	{
		if(m_nLsRowsN==0)
			m_nLsSpace = LsSpace.SingleRowSpace;
		else
			m_nLsSpace = LsSpace.doubleRowSpace;
		m_nLsEndSpace = LsSpace.EndSpace;
	}
	CDialog::OnInitDialog();
	CRect rect;
	GetWindowRect(rect);
	rect.right = rect.Width();
	rect.left = 0;
	rect.bottom = rect.Height();
	rect.top = 0;
	MoveWindow(rect,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CJdbJgJointDlg::OnSelchangeCmbLsGuige() 
{
	CString sText;
	CComboBox *pCmb = (CComboBox*)GetDlgItem(IDC_CMB_LS_GUIGE);
	UpdateData();
	int iCur = pCmb->GetCurSel();
	pCmb->GetLBText(iCur,m_sLsGuiGe);
	int ls_d;
	ls_d = atoi(m_sLsGuiGe);
	//1.1获得基准螺栓间距表
	if(GetLsSpace(LsSpace, ls_d))
	{
		if(m_nLsRowsN==0)
			m_nLsSpace = LsSpace.SingleRowSpace;
		else
			m_nLsSpace = LsSpace.doubleRowSpace;
		m_nLsEndSpace = LsSpace.EndSpace;
		sText.Format("%d",m_nLsSpace);
		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_LS_SPACE);
		pEdit->SetWindowText(sText);
		pEdit = (CEdit*)GetDlgItem(IDC_E_LS_END_SPACE);
		sText.Format("%d",m_nLsEndSpace);
		pEdit->SetWindowText(sText);
	}
}


void CJdbJgJointDlg::OnOK() 
{
	UpdateData();
	if(m_nLsRowsN==0)
		LsSpace.SingleRowSpace = m_nLsSpace;
	else
		LsSpace.doubleRowSpace = m_nLsSpace;
	LsSpace.EndSpace = m_nLsEndSpace;

	jgzj.g  = m_nG;
	jgzj.g1 = m_nG1;
	jgzj.g2 = m_nG2;
	CDialog::OnOK();
}
