//<LOCALE_TRANSLATE Confirm by hxr/>
// PtDesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "LsPosDesDlg.h"
#include "ScrTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CLsPosDesDlg dialog
CLsPosDesDlg::CLsPosDesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLsPosDesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLsPosDesDlg)
	m_iPosSpecStyle = 0;
	m_hJg1 = _T("");
	m_hJg2 = _T("");
	m_hNode = _T("");
	m_fLenDist = 0.0;
	m_fWingDist = 0.0;
	m_iJgOffsetVec = 0;
	m_iOffsetWing = 0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sBaseName = _T("datum node");
#else
	m_sBaseName = _T("基准节点");
#endif
	m_fG1 = 0.0;
	m_fG2 = 0.0;
	//}}AFX_DATA_INIT
}


void CLsPosDesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLsPosDesDlg)
	DDX_CBIndex(pDX, IDC_CMB_POS_SPEC_STYLE, m_iPosSpecStyle);
	DDX_Text(pDX, IDC_E_BASE_JG1, m_hJg1);
	DDX_Text(pDX, IDC_E_BASE_JG2, m_hJg2);
	DDX_Text(pDX, IDC_E_BASE_NODE, m_hNode);
	DDX_Text(pDX, IDC_E_LEN_OFFSET_DIST, m_fLenDist);
	DDX_Text(pDX, IDC_E_WING_OFFSET_DIST, m_fWingDist);
	DDX_Radio(pDX, IDC_RDO_JG_OFFSET_VEC, m_iJgOffsetVec);
	DDX_Radio(pDX, IDC_RDO_OFFSET_WING, m_iOffsetWing);
	DDX_Text(pDX, IDC_S_BASE_NAME, m_sBaseName);
	DDX_Text(pDX, IDC_E_BASE_G1, m_fG1);
	DDX_Text(pDX, IDC_E_BASE_G2, m_fG2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLsPosDesDlg, CDialog)
	//{{AFX_MSG_MAP(CLsPosDesDlg)
	ON_CBN_SELCHANGE(IDC_CMB_POS_SPEC_STYLE, OnSelchangeCmbPosSpecStyle)
	ON_BN_CLICKED(IDC_BN_NORM_OFFSET, OnBnNormOffset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLsPosDesDlg message handlers

BOOL CLsPosDesDlg::OnInitDialog() 
{
	if(m_iPosSpecStyle==4)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sBaseName="datum bolt";
	else
		m_sBaseName="datum node";
#else
		m_sBaseName="基准螺栓";
	else
		m_sBaseName="基准节点";
#endif
	CDialog::OnInitDialog();
	CString ss;
	ss.Format("%d",norm_offset.Thick(&Ta));
	GetDlgItem(IDC_BN_NORM_OFFSET)->SetWindowText(ss);
	OnSelchangeCmbPosSpecStyle();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLsPosDesDlg::OnSelchangeCmbPosSpecStyle() 
{
	UpdateData();
	if(m_iPosSpecStyle==4)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sBaseName="datum bolt";
	else
		m_sBaseName="datum node";
#else
		m_sBaseName="基准螺栓";
	else
		m_sBaseName="基准节点";
#endif
	if(m_iPosSpecStyle==0)
	{
		GetDlgItem(IDC_E_BASE_NODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_OFFSET_WING)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_OFFSET_WING2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_JG_OFFSET_VEC)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_JG_OFFSET_VEC2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_JG1)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_JG2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_G1)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_G2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_LEN_OFFSET_DIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_WING_OFFSET_DIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_NORM_OFFSET)->EnableWindow(FALSE);
	}
	else if(m_iPosSpecStyle==1)
	{	//心线交叉点
		GetDlgItem(IDC_E_BASE_NODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_OFFSET_WING)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_OFFSET_WING2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_JG_OFFSET_VEC)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_JG_OFFSET_VEC2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_JG1)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BASE_JG2)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BASE_G1)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BASE_G2)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_LEN_OFFSET_DIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_WING_OFFSET_DIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_NORM_OFFSET)->EnableWindow(TRUE);
	}
	else if(m_iPosSpecStyle==2)
	{	//角钢端楞点
		GetDlgItem(IDC_E_BASE_NODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_OFFSET_WING)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_OFFSET_WING2)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_JG_OFFSET_VEC)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_JG_OFFSET_VEC2)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BASE_JG1)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BASE_JG2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_G1)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_G2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_LEN_OFFSET_DIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_WING_OFFSET_DIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_BN_NORM_OFFSET)->EnableWindow(TRUE);
	}
	else if(m_iPosSpecStyle==3||m_iPosSpecStyle==5)
	{	//基准节点
		GetDlgItem(IDC_E_BASE_NODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_OFFSET_WING)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_OFFSET_WING2)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_JG_OFFSET_VEC)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_JG_OFFSET_VEC2)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BASE_JG1)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BASE_JG2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_G1)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_G2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_LEN_OFFSET_DIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_WING_OFFSET_DIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_BN_NORM_OFFSET)->EnableWindow(TRUE);
	}
	else if(m_iPosSpecStyle==4)
	{	//螺栓相对螺栓
		GetDlgItem(IDC_E_BASE_NODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_OFFSET_WING)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_OFFSET_WING2)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_JG_OFFSET_VEC)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_JG_OFFSET_VEC2)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BASE_JG1)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BASE_JG2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_G1)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_G2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_LEN_OFFSET_DIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_WING_OFFSET_DIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_BN_NORM_OFFSET)->EnableWindow(TRUE);
	}
	UpdateData(FALSE);
}

void CLsPosDesDlg::OnOK() 
{
	UpdateData();
	long hPart;
	sscanf(m_hNode,"%X",&hPart);
	if(m_iPosSpecStyle==4)
	{	//基准螺栓
		if(hPart>0x20&&(CLDSBolt*)console.FromPartHandle(hPart,CLS_BOLT)==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("You maybe input a handle of non-empty and non-bolt in the datum bolt!");
#else
			AfxMessageBox("你可能在基准螺栓处输入了一个非空且非螺栓的句柄值!");
#endif
			return;
		}
	}
	else
	{	//基准节点
		if(hPart>0x20&&console.FromNodeHandle(hPart)==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("You maybe input a handle of non-empty and non-node in the datum node!");
#else
			AfxMessageBox("你可能在基准节点处输入了一个非空且非节点的句柄值!");
#endif
			return;
		}
	}
	
	CDialog::OnOK();
}

void CLsPosDesDlg::OnBnNormOffset() 
{
	CString sThickStr=norm_offset.key_str;
#ifdef AFX_TARG_ENU_ENGLISH
	Input("Input normal offset the string",&sThickStr,'s');
#else
	Input("输入法向偏移字符串",&sThickStr,'s');
#endif
	_snprintf(norm_offset.key_str,MAX_TEMP_CHAR_50,"%s",sThickStr);
	sThickStr.Format("%d",norm_offset.Thick(&Ta));
	GetDlgItem(IDC_BN_NORM_OFFSET)->SetWindowText(sThickStr);
}
#endif