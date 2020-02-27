//<LOCALE_TRANSLATE Confirm by hxr/>
// ModifyJgLsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "ModifyJgLsDlg.h"
#include "Query.h"
#include "env_def.h"
#include "LmaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyJgLsDlg dialog


CModifyJgLsDlg::CModifyJgLsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyJgLsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyJgLsDlg)
	m_fAxisLen = 0.0;
	m_fWingOffsetDist = 0.0;
	m_iLsPosStyle = 0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sName1 = _T("vertical offset:");
	m_sName2 = _T("leg direction offset:");
#else
	m_sName1 = _T("纵向方向偏移:");
	m_sName2 = _T("肢翼方向偏移:");
#endif
	m_hRayJg = _T("");
	m_sBoltNormOffset = _T("");
	m_nRayJgG = 0;
	m_iRayDirection = 0;
	m_nDatumJgG = 0;
	//}}AFX_DATA_INIT
	face_norm_offset.Set(0,0,1);
}


void CModifyJgLsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyJgLsDlg)
	DDX_Text(pDX, IDC_E_AXIS_LEN, m_fAxisLen);
	DDX_Text(pDX, IDC_E_WING_OFFSET_DIST, m_fWingOffsetDist);
	DDX_CBIndex(pDX, IDC_CMB_LS_POS_STYLE, m_iLsPosStyle);
	DDX_Text(pDX, IDC_S_NAME1, m_sName1);
	DDX_Text(pDX, IDC_S_NAME2, m_sName2);
	DDX_Text(pDX, IDC_E_RAY_HANDLE, m_hRayJg);
	DDX_CBString(pDX, IDC_CMB_LS_GUIGE, m_sLsGuiGe);
	DDX_Text(pDX, IDC_E_BOLT_NORM_OFFSET, m_sBoltNormOffset);
	DDX_Text(pDX, IDC_E_RAY_JG_G, m_nRayJgG);
	DDX_CBIndex(pDX, IDC_CMB_RAY_DIRECTION, m_iRayDirection);
	DDX_Text(pDX, IDC_E_DATUM_JG_G, m_nDatumJgG);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyJgLsDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyJgLsDlg)
	ON_CBN_SELCHANGE(IDC_CMB_LS_POS_STYLE, OnSelchangeCmbLsPosStyle)
	ON_BN_CLICKED(IDC_BTN_OFFSET_FACE_PARA, OnOffsetFacePara)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyJgLsDlg message handlers

BOOL CModifyJgLsDlg::OnInitDialog() 
{
	//0.角钢楞点定位 1.节点向角钢心线投影 2.节点向角钢心线平推 3.心线交点
// 	if(m_iLsPosStyle==3)
// 	{
// 		m_sName1 = "基准心距:";
// 		m_sName2 = "射线心距:";
// 	}
// 	else 
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		m_sName1 = "vertical offset:";
		m_sName2 = "leg direction offset:";
#else
		m_sName1 = "纵向方向偏移:";
		m_sName2 = "肢翼方向偏移:";
#endif
	}
	GetDlgItem(IDC_E_RAY_HANDLE)->EnableWindow(m_iLsPosStyle==3);
	GetDlgItem(IDC_E_RAY_JG_G)->EnableWindow(m_iLsPosStyle==3);
	GetDlgItem(IDC_E_DATUM_JG_G)->EnableWindow(m_iLsPosStyle==3);
	GetDlgItem(IDC_BTN_OFFSET_FACE_PARA)->EnableWindow(m_iLsPosStyle==2);
	CDialog::OnInitDialog();
	CRect rect;
	GetWindowRect(rect);
	rect.right = rect.Width();
	rect.left = 0;
	rect.bottom = rect.Height();
	rect.top = 0;
	MoveWindow(rect,TRUE);
	return TRUE;
}

void CModifyJgLsDlg::OnSelchangeCmbLsPosStyle() 
{
	UpdateData();
	//0.角钢楞点定位 1.节点向角钢心线投影 2.节点向角钢心线平推 3.心线交点
// 	if(m_iLsPosStyle==3)
// 	{
// 		m_sName1 = "基准心距:";
// 		m_sName2 = "射线心距:";
// 	}
// 	else 
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		m_sName1 = "vertical offset:";
		m_sName2 = "leg direction offset:";
#else
		m_sName1 = "纵向方向偏移:";
		m_sName2 = "肢翼方向偏移:";
#endif
	}
	GetDlgItem(IDC_E_RAY_HANDLE)->EnableWindow(m_iLsPosStyle==3);
	GetDlgItem(IDC_E_RAY_JG_G)->EnableWindow(m_iLsPosStyle==3);
	GetDlgItem(IDC_E_DATUM_JG_G)->EnableWindow(m_iLsPosStyle==3);
	GetDlgItem(IDC_BTN_OFFSET_FACE_PARA)->EnableWindow(m_iLsPosStyle==2);
	UpdateData(FALSE);
}

void CModifyJgLsDlg::OnOffsetFacePara() 
{
	CReportVectorDlg dlg;
#ifdef AFX_TARG_ENU_ENGLISH
	dlg.m_sCaption="input datum plane normal line";
#else
	dlg.m_sCaption="输入平推面法线";
#endif
	dlg.m_fVectorX = face_norm_offset.x;
	dlg.m_fVectorY = face_norm_offset.y;
	dlg.m_fVectorZ = face_norm_offset.z;
	if(dlg.DoModal()==IDOK)
	{
		face_norm_offset.x = dlg.m_fVectorX;
		face_norm_offset.y = dlg.m_fVectorY;
		face_norm_offset.z = dlg.m_fVectorZ;
	}
}
