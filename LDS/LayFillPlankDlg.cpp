// LayFillPlankDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "LayFillPlankDlg.h"
#include "Query.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayFillPlankDlg dialog

#ifndef __TSA_
CLayFillPlankDlg::CLayFillPlankDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLayFillPlankDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLayFillPlankDlg)
	m_sLsGuiGe = _T("16");
	m_iLsRows = 0;
	m_fEndOffset = 0.0;
	m_nLsNum = 2;
	m_nPlankNum = 2;
	m_fStartOffset = 0.0;
	m_iLsLayOutStyle = 0;
	m_iWing = 0;
	m_bSwapWing = FALSE;
	m_sPartNo = _T("");
	m_iPlateMaterial = 0;
	m_sSegI = _T("");
	m_nThick = 0;
	m_bDifferentType = FALSE;
	m_iType = 0;
	m_fDistance = 0.0;
	//}}AFX_DATA_INIT
	m_iGroupStyle=0;
	m_pGroupAngle=NULL;
}


void CLayFillPlankDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayFillPlankDlg)
	DDX_CBString(pDX, IDC_CMB_LS_GUIGE, m_sLsGuiGe);
	DDV_MaxChars(pDX, m_sLsGuiGe, 2);
	DDX_CBIndex(pDX, IDC_CMB_LS_ROWS, m_iLsRows);
	DDX_Text(pDX, IDC_E_END_OFFSET, m_fEndOffset);
	DDX_Text(pDX, IDC_E_LS_NUM, m_nLsNum);
	DDX_Text(pDX, IDC_E_PLANK_NUM, m_nPlankNum);
	DDV_MinMaxInt(pDX, m_nPlankNum, 0, 10000);
	DDX_Text(pDX, IDC_E_START_OFFSET, m_fStartOffset);
	DDX_Radio(pDX, IDC_LS_LAYOUT_STYLE, m_iLsLayOutStyle);
	DDX_Radio(pDX, IDC_RDO_WING, m_iWing);
	DDX_Check(pDX, IDC_CHK_SWAP, m_bSwapWing);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDV_MaxChars(pDX, m_sPartNo, 16);
	DDX_CBIndex(pDX, IDC_CMB_PLATE_MATERIAL, m_iPlateMaterial);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_THICK, m_nThick);
	DDX_Check(pDX, IDC_CHK_DIFFERENT_TYPE, m_bDifferentType);
	DDX_CBIndex(pDX, IDC_CMB_TYPE, m_iType);
	DDX_Text(pDX, IDC_E_DISTANCE, m_fDistance);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLayFillPlankDlg, CDialog)
	//{{AFX_MSG_MAP(CLayFillPlankDlg)
	ON_CBN_SELCHANGE(IDC_CMB_LS_ROWS, OnSelchangeCmbLsRows)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangeEPartNo)
	ON_CBN_SELCHANGE(IDC_CMB_TYPE, OnSelchangeCmbType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayFillPlankDlg message handlers

void CLayFillPlankDlg::OnSelchangeCmbLsRows() 
{
	UpdateData();
	GetDlgItem(IDC_LS_LAYOUT_STYLE)->EnableWindow(m_iLsRows!=0);
	GetDlgItem(IDC_LS_LAYOUT_STYLE2)->EnableWindow(m_iLsRows!=0);
	GetDlgItem(IDC_CHK_DIFFERENT_TYPE)->EnableWindow(m_iLsRows!=0);
	UpdateData(FALSE);
}

BOOL CLayFillPlankDlg::OnInitDialog() 
{
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_PLATE_MATERIAL);
	AddSteelMatRecord(pCMB);
	CDialog::OnInitDialog();
	if(m_pGroupAngle==NULL)
		return FALSE;
	m_iGroupStyle=m_pGroupAngle->group_style;
	m_sSegI=m_pGroupAngle->iSeg.ToString();
	m_sLsGuiGe.Format("%d",m_pGroupAngle->connectStart.d);
	m_nLsNum=ftoi(m_pGroupAngle->connectStart.wnConnBoltN*0.5);
	if(m_nLsNum>5)	//螺栓数大于5时启用双排螺栓
		m_iLsRows=1;
	m_nThick=ftoi(m_pGroupAngle->jg_space-g_sysPara.m_nGroupJgMachiningGap);
	m_iPlateMaterial=QuerySteelMatIndex(m_pGroupAngle->cMaterial);
	//
	GetDlgItem(IDC_LS_LAYOUT_STYLE)->EnableWindow(m_iLsRows!=0);
	GetDlgItem(IDC_LS_LAYOUT_STYLE2)->EnableWindow(m_iLsRows!=0);
	GetDlgItem(IDC_CHK_DIFFERENT_TYPE)->EnableWindow(m_iLsRows!=0);
	//
	GetDlgItem(IDC_CMB_TYPE)->EnableWindow(m_iGroupStyle!=1);
	GetDlgItem(IDC_RDO_WING)->EnableWindow(m_iGroupStyle!=1&&m_iType==0);
	GetDlgItem(IDC_RDO_WING2)->EnableWindow(m_iGroupStyle!=1&&m_iType==0);
	GetDlgItem(IDC_CHK_SWAP)->EnableWindow(m_iGroupStyle!=1&&m_iType==0);
	GetDlgItem(IDC_E_DISTANCE)->EnableWindow(m_iGroupStyle!=1&&m_iType!=0);
	if(m_iGroupStyle==1)	//T型组合
	{
		SetDlgItemInt(IDC_CMB_TYPE,0);
		m_iType = 0;
		m_iWing=0;
		m_bSwapWing=FALSE;
	}
	UpdateData(FALSE);
	return TRUE;
}

void CLayFillPlankDlg::OnChangeEPartNo() 
{
	UpdateData();
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo.GetBuffer(),&iSeg,NULL))
		m_sSegI=iSeg.ToString();
	UpdateData(FALSE);
}

void CLayFillPlankDlg::OnSelchangeCmbType() 
{
	UpdateData();
	GetDlgItem(IDC_RDO_WING)->EnableWindow(m_iGroupStyle!=1&&m_iType==0);
	GetDlgItem(IDC_RDO_WING2)->EnableWindow(m_iGroupStyle!=1&&m_iType==0);
	GetDlgItem(IDC_CHK_SWAP)->EnableWindow(m_iGroupStyle!=1&&m_iType==0);
	GetDlgItem(IDC_E_DISTANCE)->EnableWindow(m_iGroupStyle!=1&&m_iType!=0);
}

void CLayFillPlankDlg::OnOK() 
{
	JGZJ jgzj;
	getjgzj(jgzj,m_pGroupAngle->GetWidth());
	if(m_iLsRows&&(jgzj.g1<EPS||jgzj.g2<EPS))
	{
		char sError[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(sError,"L%.fX%.f angle can't support double row of bolts!",m_pGroupAngle->GetWidth(),m_pGroupAngle->GetThick());
#else 
		sprintf(sError,"L%.fX%.f角钢不支持布置双排螺栓!",m_pGroupAngle->GetWidth(),m_pGroupAngle->GetThick());
#endif
		AfxMessageBox(sError);
		return;
	}
	CDialog::OnOK();
}
#endif