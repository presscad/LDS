// FootParaSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "lds_co_part.h"
#include "FootParaSetDlg.h"
#include "coorddefdlg.h"
#include "Query.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CFootParaSetDlg dialog


CFootParaSetDlg::CFootParaSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFootParaSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFootParaSetDlg)
	m_iA = 30;
	m_iB = 30;
	m_iUpC = 25;
	m_iDownC=25;
	m_iThick = 10;
	m_iWide = 300;
	m_iLjStyle = 0;
	m_iS = 0;
	m_bWeldDiZuo = FALSE;
	m_sDiBanPartNo = _T("");
	m_iUpLsLayStyle = 0;
	m_iDownLsLayStyle = 0;
	m_iUpXueBanThick = 0;
	m_iDownXueBanThick = 0;
	m_iDiBanMaterial = 0;
	m_iXueBanMaterial = 0;
	m_sUpOddment = _T("");
	m_sDownOddment = _T("");
	m_bBreakDownXueBan0C = TRUE;
	m_bBreakDownXueBan90C = FALSE;
	m_bBreakUpXueBan0C = TRUE;
	m_bBreakUpXueBan90C = FALSE;
	m_iDiBanShape = 0;
	m_sUpXueBanPartNo0C = _T("");
	m_sUpXueBanPartNo135C = _T("");
	m_sUpXueBanPartNo180C = _T("");
	m_sUpXueBanPartNo225C = _T("");
	m_sUpXueBanPartNo270C = _T("");
	m_sUpXueBanPartNo315C = _T("");
	m_sUpXueBanPartNo45C = _T("");
	m_sUpXueBanPartNo90C = _T("");
	m_sDownXueBanPartNo0C = _T("");
	m_sDownXueBanPartNo135C = _T("");
	m_sDownXueBanPartNo180C = _T("");
	m_sDownXueBanPartNo225C = _T("");
	m_sDownXueBanPartNo270C = _T("");
	m_sDownXueBanPartNo315C = _T("");
	m_sDownXueBanPartNo45C = _T("");
	m_sDownXueBanPartNo90C = _T("");
	//}}AFX_DATA_INIT
	m_pFoot = NULL;
	m_bUpAndDownSide=FALSE;
}


void CFootParaSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFootParaSetDlg)
	DDX_Text(pDX, IDC_E_A, m_iA);
	DDX_Text(pDX, IDC_E_B, m_iB);
	DDX_Text(pDX, IDC_E_UP_C, m_iUpC);
	DDX_Text(pDX, IDC_E_DOWN_C, m_iDownC);
	DDX_Text(pDX, IDC_E_THICK, m_iThick);
	DDX_Text(pDX, IDC_E_WIDE, m_iWide);
	DDX_Radio(pDX, IDC_RDO_LJ_STYLE, m_iLjStyle);
	DDX_Text(pDX, IDC_E_DIBAN_HOLE_DIST, m_iS);
	DDX_Check(pDX, IDC_CHK_WELD_DIZUO, m_bWeldDiZuo);
	DDX_Text(pDX, IDC_E_DIBAN_PARTNO, m_sDiBanPartNo);
	DDV_MaxChars(pDX, m_sDiBanPartNo, 8);
	DDX_Radio(pDX, IDC_RDO_UP_LS_LAY_STYLE, m_iUpLsLayStyle);
	DDX_Radio(pDX, IDC_RDO_DOWN_LS_LAY_STYLE, m_iDownLsLayStyle);
	DDX_Text(pDX, IDC_E_UP_XUE_BAN_THICK, m_iUpXueBanThick);
	DDX_Text(pDX, IDC_E_DOWN_XUE_BAN_THICK2, m_iDownXueBanThick);
	DDX_CBIndex(pDX, IDC_CMB_DIBAN_MATERIAL, m_iDiBanMaterial);
	DDX_CBIndex(pDX, IDC_CMB_XUEBAN_MATERIAL, m_iXueBanMaterial);
	DDX_Text(pDX, IDC_S_UP_ODD, m_sUpOddment);
	DDX_Text(pDX, IDC_S_DOWN_ODD, m_sDownOddment);
	DDX_Check(pDX, IDC_CHK_BREAK_DOWN_0C, m_bBreakDownXueBan0C);
	DDX_Check(pDX, IDC_CHK_BREAK_DOWN_90C, m_bBreakDownXueBan90C);
	DDX_Check(pDX, IDC_CHK_BREAK_UP_0C, m_bBreakUpXueBan0C);
	DDX_Check(pDX, IDC_CHK_BREAK_UP_90C, m_bBreakUpXueBan90C);
	DDX_CBIndex(pDX, IDC_CMB_DIBAN_SHAPE, m_iDiBanShape);
	DDX_Text(pDX, IDC_E_UP_XUEBAN_PARTNO_0C, m_sUpXueBanPartNo0C);
	DDV_MaxChars(pDX, m_sUpXueBanPartNo0C, 16);
	DDX_Text(pDX, IDC_E_UP_XUEBAN_PARTNO_135C, m_sUpXueBanPartNo135C);
	DDV_MaxChars(pDX, m_sUpXueBanPartNo135C, 16);
	DDX_Text(pDX, IDC_E_UP_XUEBAN_PARTNO_180C, m_sUpXueBanPartNo180C);
	DDV_MaxChars(pDX, m_sUpXueBanPartNo180C, 16);
	DDX_Text(pDX, IDC_E_UP_XUEBAN_PARTNO_225C, m_sUpXueBanPartNo225C);
	DDV_MaxChars(pDX, m_sUpXueBanPartNo225C, 16);
	DDX_Text(pDX, IDC_E_UP_XUEBAN_PARTNO_270C, m_sUpXueBanPartNo270C);
	DDV_MaxChars(pDX, m_sUpXueBanPartNo270C, 16);
	DDX_Text(pDX, IDC_E_UP_XUEBAN_PARTNO_315C, m_sUpXueBanPartNo315C);
	DDV_MaxChars(pDX, m_sUpXueBanPartNo315C, 16);
	DDX_Text(pDX, IDC_E_UP_XUEBAN_PARTNO_45C, m_sUpXueBanPartNo45C);
	DDV_MaxChars(pDX, m_sUpXueBanPartNo45C, 16);
	DDX_Text(pDX, IDC_E_UP_XUEBAN_PARTNO_90C, m_sUpXueBanPartNo90C);
	DDV_MaxChars(pDX, m_sUpXueBanPartNo90C, 16);
	DDX_Text(pDX, IDC_E_DOWN_XUEBAN_PARTNO_0C, m_sDownXueBanPartNo0C);
	DDV_MaxChars(pDX, m_sDownXueBanPartNo0C, 16);
	DDX_Text(pDX, IDC_E_DOWN_XUEBAN_PARTNO_135C, m_sDownXueBanPartNo135C);
	DDV_MaxChars(pDX, m_sDownXueBanPartNo135C, 16);
	DDX_Text(pDX, IDC_E_DOWN_XUEBAN_PARTNO_180C, m_sDownXueBanPartNo180C);
	DDV_MaxChars(pDX, m_sDownXueBanPartNo180C, 16);
	DDX_Text(pDX, IDC_E_DOWN_XUEBAN_PARTNO_225C, m_sDownXueBanPartNo225C);
	DDV_MaxChars(pDX, m_sDownXueBanPartNo225C, 16);
	DDX_Text(pDX, IDC_E_DOWN_XUEBAN_PARTNO_270C, m_sDownXueBanPartNo270C);
	DDV_MaxChars(pDX, m_sDownXueBanPartNo270C, 16);
	DDX_Text(pDX, IDC_E_DOWN_XUEBAN_PARTNO_315C, m_sDownXueBanPartNo315C);
	DDV_MaxChars(pDX, m_sDownXueBanPartNo315C, 16);
	DDX_Text(pDX, IDC_E_DOWN_XUEBAN_PARTNO_45C, m_sDownXueBanPartNo45C);
	DDV_MaxChars(pDX, m_sDownXueBanPartNo45C, 16);
	DDX_Text(pDX, IDC_E_DOWN_XUEBAN_PARTNO_90C, m_sDownXueBanPartNo90C);
	DDV_MaxChars(pDX, m_sDownXueBanPartNo90C, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFootParaSetDlg, CDialog)
	//{{AFX_MSG_MAP(CFootParaSetDlg)
	ON_BN_CLICKED(IDC_BN_HELP, OnBnHelp)
	ON_BN_CLICKED(IDC_RDO_LJ_STYLE, OnRdoLjStyle)
	ON_EN_CHANGE(IDC_E_WIDE, OnChangeEWide)
	ON_BN_CLICKED(IDC_CHK_WELD_DIZUO, OnChkWeldDizuo)
	ON_BN_CLICKED(IDC_BN_SET_UCS_ORIGIN, OnBnSetUcsOrigin)
	ON_EN_CHANGE(IDC_E_UP_C, OnChangeEUpC)
	ON_EN_CHANGE(IDC_E_DOWN_C, OnChangeEDownC)
	ON_CBN_SELCHANGE(IDC_CMB_DIBAN_SHAPE, OnSelchangeCmbDibanShape)
	ON_BN_CLICKED(IDC_CHK_BREAK_UP_0C, OnChkBreakUp0c)
	ON_BN_CLICKED(IDC_CHK_BREAK_UP_90C, OnChkBreakUp90c)
	ON_BN_CLICKED(IDC_CHK_BREAK_DOWN_0C, OnChkBreakDown0c)
	ON_BN_CLICKED(IDC_CHK_BREAK_DOWN_90C, OnChkBreakDown90c)
	ON_BN_CLICKED(IDC_RDO_LJ_STYLE2, OnRdoLjStyle)
	ON_BN_CLICKED(IDC_RDO_LJ_STYLE3, OnRdoLjStyle)
	ON_EN_CHANGE(IDC_E_UP_XUEBAN_PARTNO_270C, OnChangeEUpXuebanPartno270C)
	ON_EN_CHANGE(IDC_E_UP_XUEBAN_PARTNO_180C, OnChangeEUpXuebanPartno180C)
	ON_EN_CHANGE(IDC_E_DOWN_XUEBAN_PARTNO_180C, OnChangeEDownXuebanPartno180C)
	ON_EN_CHANGE(IDC_E_DOWN_XUEBAN_PARTNO_270C, OnChangeEDownXuebanPartno270C)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFootParaSetDlg message handlers

BOOL CFootParaSetDlg::OnInitDialog() 
{
	if(m_pFoot==NULL)
		return FALSE;	//没有底脚板对象,初始化失败
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_DIBAN_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_XUEBAN_MATERIAL);
	AddSteelMatRecord(pCMB);
	if(m_bUpAndDownSide)
	{
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_0C)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_45C)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_90C)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_135C)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_180C)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_225C)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_270C)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_315C)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_DOWN_LS_LAY_STYLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_LS_LAY_STYLE4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_BREAK_DOWN_0C)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_BREAK_DOWN_90C)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_DOWN_XUE_BAN_THICK2)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_DOWN_C)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_0C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_45C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_90C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_135C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_180C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_225C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_270C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_315C)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_DOWN_LS_LAY_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_LS_LAY_STYLE4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_BREAK_DOWN_0C)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_BREAK_DOWN_90C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_XUE_BAN_THICK2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_C)->EnableWindow(FALSE);
	}
	GetDlgItem(IDC_E_UP_C)->EnableWindow(!m_bWeldDiZuo);
	GetDlgItem(IDC_E_DOWN_C)->EnableWindow(!m_bWeldDiZuo&&m_bUpAndDownSide);
	
	CDialog::OnInitDialog();
	OnChangeEUpC();
	OnChangeEDownC();
	OnRdoLjStyle();
	OnSelchangeCmbDibanShape();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CFootParaSetDlg::OnBnSetUcsOrigin() 
{
	CCoordDefDlg dlg;
	dlg.ucs = m_pFoot->ucs;
	if(dlg.DoModal()==IDOK)
		m_pFoot->ucs=dlg.ucs;
}

void CFootParaSetDlg::OnBnHelp() 
{
	// TODO: Add your control notification handler code here
	
}

void CFootParaSetDlg::OnRdoLjStyle() 
{
	UpdateData();
	if(m_iLjStyle==1)
	{	//对焊
		m_bBreakUpXueBan0C=m_bBreakUpXueBan90C=TRUE;
		UpdateData(FALSE);
		OnChkBreakUp0c();
		if(m_bUpAndDownSide)
		{
			m_bBreakDownXueBan0C=m_bBreakDownXueBan90C=TRUE;
			UpdateData(FALSE);
			OnChkBreakDown0c();
		}
	}
	else if(m_iLjStyle==2)
	{	//搭焊
		OnChkBreakUp0c();
		if(m_bUpAndDownSide)
			OnChkBreakDown0c();
	}
	else
	{	//螺栓
		if(m_bBreakUpXueBan0C&&m_bBreakUpXueBan90C)
			m_bBreakUpXueBan0C=TRUE;
		UpdateData(FALSE);
		OnChkBreakUp0c();
		if(m_bUpAndDownSide)
		{
			if(m_bBreakDownXueBan0C&&m_bBreakDownXueBan90C)
				m_bBreakDownXueBan0C=TRUE;
			UpdateData(FALSE);
			OnChkBreakDown0c();
		}
	}
	m_iS = QueryDiBanParaByWide(m_iWide,'S',m_iLjStyle==1);
	UpdateData(FALSE);
}


void CFootParaSetDlg::OnChkBreakUp0c() 
{
	UpdateData();
	if(!m_bBreakUpXueBan0C&&!m_bBreakUpXueBan90C)
		m_bBreakUpXueBan90C=TRUE;
	else if(m_iLjStyle==0&&m_bBreakUpXueBan0C&&m_bBreakUpXueBan90C)	//螺栓连接
		m_bBreakUpXueBan90C=FALSE;
	GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_0C)->EnableWindow(m_bBreakUpXueBan0C);
	GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_90C)->EnableWindow(m_bBreakUpXueBan90C);
	UpdateData(FALSE);
}

void CFootParaSetDlg::OnChkBreakUp90c() 
{
	UpdateData();
	if(!m_bBreakUpXueBan0C&&!m_bBreakUpXueBan90C)
		m_bBreakUpXueBan0C=TRUE;
	else if(m_iLjStyle==0&&m_bBreakUpXueBan0C&&m_bBreakUpXueBan90C)	//螺栓连接
		m_bBreakUpXueBan0C=FALSE;
	GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_0C)->EnableWindow(m_bBreakUpXueBan0C);
	GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_90C)->EnableWindow(m_bBreakUpXueBan90C);
	UpdateData(FALSE);
}

void CFootParaSetDlg::OnChkBreakDown0c() 
{
	UpdateData();
	if(!m_bBreakDownXueBan0C&&!m_bBreakDownXueBan90C)
		m_bBreakDownXueBan90C=TRUE;
	else if(m_iLjStyle==0&&m_bBreakDownXueBan0C&&m_bBreakDownXueBan90C)	//螺栓连接
		m_bBreakDownXueBan90C=FALSE;
	GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_0C)->EnableWindow(m_bBreakDownXueBan0C);
	GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_90C)->EnableWindow(m_bBreakDownXueBan90C);
	UpdateData(FALSE);
}

void CFootParaSetDlg::OnChkBreakDown90c() 
{
	UpdateData();
	if(!m_bBreakDownXueBan0C&&!m_bBreakDownXueBan90C)
		m_bBreakDownXueBan0C=TRUE;
	else if(m_iLjStyle==0&&m_bBreakDownXueBan0C&&m_bBreakDownXueBan90C)	//螺栓连接
		m_bBreakDownXueBan0C=FALSE;
	GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_0C)->EnableWindow(m_bBreakDownXueBan0C);
	GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_90C)->EnableWindow(m_bBreakDownXueBan90C);
	UpdateData(FALSE);
}

void CFootParaSetDlg::OnSelchangeCmbDibanShape() 
{
	UpdateData();
	if(m_iDiBanShape==0)	//方形底板
	{
		GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_45C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_135C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_225C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_315C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_45C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_135C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_225C)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_315C)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_45C)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_135C)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_225C)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_315C)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_45C)->EnableWindow(m_bUpAndDownSide&&TRUE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_135C)->EnableWindow(m_bUpAndDownSide&&TRUE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_225C)->EnableWindow(m_bUpAndDownSide&&TRUE);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_315C)->EnableWindow(m_bUpAndDownSide&&TRUE);
	}
	
}

void CFootParaSetDlg::OnChkWeldDizuo() 
{
	UpdateData();
	GetDlgItem(IDC_E_UP_C)->EnableWindow(!m_bWeldDiZuo);
	GetDlgItem(IDC_E_DOWN_C)->EnableWindow(!m_bWeldDiZuo&&m_bUpAndDownSide);
	
}

void CFootParaSetDlg::OnChangeEWide() 
{
	CString sText;
	GetDlgItem(IDC_E_WIDE)->GetWindowText(sText);
	if(sText.CompareNoCase("-")==0&&sText.GetLength()==1)
		return;
	UpdateData();
	m_iS = QueryDiBanParaByWide(m_iWide,'S',m_iLjStyle==1);
	m_iThick=QueryDiBanParaByWide(m_iWide,'T',m_iLjStyle==1);
	UpdateData(FALSE);
}

void CFootParaSetDlg::OnChangeEUpC() 
{
	CString sText;
	GetDlgItem(IDC_E_UP_C)->GetWindowText(sText);
	if(sText.CompareNoCase("-")==0&&sText.GetLength()==1)
		return;
	UpdateData();
	if(m_iUpC<0)
	{
		m_sUpOddment.Format("%d",m_iUpC);
		UpdateData(FALSE);
		return;
	}
	CLDSPart *pPart = console.FromPartHandle(m_pFoot->attach_jg_handle[0]);
	if(pPart==NULL)
		return;
	CLDSLineAngle *pCommJg=NULL;
	f3dPoint jg_vec,vertex;
	double lowest_z=0;
	if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pPart;
		jg_vec = pGroupJg->End()-pGroupJg->Start();
		for(int i=0;i<4;i++)
		{
			if(pGroupJg->group_style<2&&i>=2)
				continue;	//个别四角钢转换为双拼角钢时，会把第三、四根子角钢转换为虚角钢存在
			if(pGroupJg->son_jg_h[i]>0x20)
			{
				CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
				if(pGroupJg->pStart->handle==m_pFoot->base_node_handle)
				{
					vertex =pSonJg->Start()+pSonJg->GetWingVecX()*pSonJg->GetWidth();
					if(vertex.z>lowest_z)
						lowest_z = vertex.z;
					vertex =pSonJg->Start()+pSonJg->GetWingVecY()*pSonJg->GetWidth();
					if(vertex.z>lowest_z)
						lowest_z = vertex.z;
				}
				else
				{
					vertex =pSonJg->End()+pSonJg->GetWingVecX()*pSonJg->GetWidth();
					if(vertex.z>lowest_z)
						lowest_z = vertex.z;
					vertex =pSonJg->End()+pSonJg->GetWingVecY()*pSonJg->GetWidth();
					if(vertex.z>lowest_z)
						lowest_z = vertex.z;
				}
			}
		}
	}
	else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		pCommJg = (CLDSLineAngle*)pPart;
		jg_vec = pCommJg->End()-pCommJg->Start();
		if(pCommJg->pStart->handle==m_pFoot->base_node_handle)
		{
			vertex =pCommJg->Start()+pCommJg->GetWingVecX()*pCommJg->GetWidth();
			if(vertex.z>lowest_z)
				lowest_z = vertex.z;
			vertex =pCommJg->Start()+pCommJg->GetWingVecY()*pCommJg->GetWidth();
			if(vertex.z>lowest_z)
				lowest_z = vertex.z;
		}
		else
		{
			vertex =pCommJg->End()+pCommJg->GetWingVecX()*pCommJg->GetWidth();
			if(vertex.z>lowest_z)
				lowest_z = vertex.z;
			vertex =pCommJg->End()+pCommJg->GetWingVecY()*pCommJg->GetWidth();
			if(vertex.z>lowest_z)
				lowest_z = vertex.z;
		}
	}
	normalize(jg_vec);
	m_sUpOddment.Format("%.0f",-(lowest_z-m_pFoot->ucs.origin.z+m_iUpC)/fabs(jg_vec.z));
	UpdateData(FALSE);
}

void CFootParaSetDlg::OnChangeEDownC() 
{
	CString sText;
	GetDlgItem(IDC_E_DOWN_C)->GetWindowText(sText);
	if(sText.CompareNoCase("-")==0&&sText.GetLength()==1)
		return;
	UpdateData();
	if(m_iUpC<0)
	{
		m_sDownOddment.Format("%d",m_iDownC);
		UpdateData(FALSE);
		return;
	}
	CLDSPart *pPart = console.FromPartHandle(m_pFoot->attach_jg_handle[0]);
	if(pPart==NULL)
		return;
	CLDSLineAngle *pCommJg=NULL;
	f3dPoint jg_vec,vertex;
	double highest_z=0;
	if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pPart;
		jg_vec = pGroupJg->End()-pGroupJg->Start();
		for(int i=0;i<4;i++)
		{
			if(pGroupJg->group_style<2&&i>=2)
				continue;	//个别四角钢转换为双拼角钢时，会把第三、四根子角钢转换为虚角钢存在
			if(pGroupJg->son_jg_h[i]>0x20)
			{
				CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
				if(pGroupJg->pStart->handle==m_pFoot->base_node_handle)
				{
					vertex =pSonJg->Start()+pSonJg->GetWingVecX()*pSonJg->GetWidth();
					if(vertex.z<highest_z)
						highest_z = vertex.z;
					vertex =pSonJg->Start()+pSonJg->GetWingVecY()*pSonJg->GetWidth();
					if(vertex.z<highest_z)
						highest_z = vertex.z;
				}
				else
				{
					vertex =pSonJg->End()+pSonJg->GetWingVecX()*pSonJg->GetWidth();
					if(vertex.z<highest_z)
						highest_z = vertex.z;
					vertex =pSonJg->End()+pSonJg->GetWingVecY()*pSonJg->GetWidth();
					if(vertex.z<highest_z)
						highest_z = vertex.z;
				}
			}
		}
	}
	else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		pCommJg = (CLDSLineAngle*)pPart;
		if(pCommJg->pStart->handle==m_pFoot->base_node_handle)
		{
			vertex =pCommJg->Start()+pCommJg->GetWingVecX()*pCommJg->GetWidth();
			if(vertex.z<highest_z)
				highest_z = vertex.z;
			vertex =pCommJg->Start()+pCommJg->GetWingVecY()*pCommJg->GetWidth();
			if(vertex.z<highest_z)
				highest_z = vertex.z;
		}
		else
		{
			vertex =pCommJg->End()+pCommJg->GetWingVecX()*pCommJg->GetWidth();
			if(vertex.z<highest_z)
				highest_z = vertex.z;
			vertex =pCommJg->End()+pCommJg->GetWingVecY()*pCommJg->GetWidth();
			if(vertex.z<highest_z)
				highest_z = vertex.z;
		}
	}
	normalize(jg_vec);
	m_sDownOddment.Format("%.0f",-(m_pFoot->ucs.origin.z+m_iThick+m_iDownC-highest_z)/fabs(jg_vec.z));
	UpdateData(FALSE);
}

void CFootParaSetDlg::OnChangeEUpXuebanPartno270C() 
{
	if(!m_bBreakUpXueBan90C)	//前后板一致
	{
		CString ss;
		GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_270C)->GetWindowText(ss);
		GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_90C)->SetWindowText(ss);
	}
}

void CFootParaSetDlg::OnChangeEUpXuebanPartno180C() 
{
	if(!m_bBreakUpXueBan0C)	//前后板一致
	{
		CString ss;
		GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_180C)->GetWindowText(ss);
		GetDlgItem(IDC_E_UP_XUEBAN_PARTNO_0C)->SetWindowText(ss);
	}
	
}

void CFootParaSetDlg::OnChangeEDownXuebanPartno180C() 
{
	if(!m_bBreakDownXueBan0C)	//前后板一致
	{
		CString ss;
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_180C)->GetWindowText(ss);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_0C)->SetWindowText(ss);
	}
	
}

void CFootParaSetDlg::OnChangeEDownXuebanPartno270C() 
{
	if(!m_bBreakDownXueBan90C)	//前后板一致
	{
		CString ss;
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_270C)->GetWindowText(ss);
		GetDlgItem(IDC_E_DOWN_XUEBAN_PARTNO_90C)->SetWindowText(ss);
	}
	
}
#endif