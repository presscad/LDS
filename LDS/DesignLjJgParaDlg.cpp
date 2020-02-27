//<LOCALE_TRANSLATE Confirm by hxr/>
// DesignLjJgparaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "DesignLjJgParaDlg.h"
#include "ScrTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDesignLjJgParaDlg dialog


CDesignLjJgParaDlg::CDesignLjJgParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignLjJgParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignLjJgParaDlg)
	m_iCurLjEnd = 0;
	m_iCurLjWing = 0;
	m_iLjFace = 0;
	m_fBerSpace = 30.0;
	m_hCollideRefJg1 = _T("0X0");
	m_hCollideRefJg2 = _T("0X0");
	m_fEndSpace = 30;
	m_bTwoEdgeVertice = FALSE;
	m_hJg = _T("0X0");
	m_sLsStr = _T("");
	m_fWingSpace = 30;
	m_nCollideDist = 0;
	m_iOddCalStyle = 0;
	m_iAffectTubeEndPlateStyle = 0;
	//}}AFX_DATA_INIT
}


void CDesignLjJgParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignLjJgParaDlg)
	DDX_CBIndex(pDX, IDC_CMB_CURLJ_END, m_iCurLjEnd);
	DDX_CBIndex(pDX, IDC_CMB_CURLJ_WING, m_iCurLjWing);
	DDX_CBIndex(pDX, IDC_CMB_LJ_FACE, m_iLjFace);
	DDX_Text(pDX, IDC_E_BER_SPACE, m_fBerSpace);
	DDX_Text(pDX, IDC_E_COLLIDE_REF_JG1, m_hCollideRefJg1);
	DDX_Text(pDX, IDC_E_COLLIDE_REF_JG2, m_hCollideRefJg2);
	DDX_Text(pDX, IDC_E_END_SPACE, m_fEndSpace);
	DDX_Check(pDX, IDC_CHK_TWO_EDGE_VERTICE, m_bTwoEdgeVertice);
	DDX_Text(pDX, IDC_E_JG_HANDLE, m_hJg);
	DDX_Text(pDX, IDC_E_LS_STR, m_sLsStr);
	DDX_Text(pDX, IDC_E_WING_SPACE, m_fWingSpace);
	DDX_Text(pDX, IDC_E_COLLIDE_DIST, m_nCollideDist);
	DDX_CBIndex(pDX, IDC_CMB_ODD_CAL_STYLE, m_iOddCalStyle);
	DDX_CBIndex(pDX, IDC_CMB_AFFECT_PLATE_STYLE, m_iAffectTubeEndPlateStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignLjJgParaDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignLjJgParaDlg)
	ON_BN_CLICKED(IDC_BN_END_NORM_OFFSET, OnBnEndNormOffset)
	ON_BN_CLICKED(IDC_BN_END_DATUM_NORM_OFFSET, OnBnEndDatumNormOffset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignLjJgparaDlg message handlers

void CDesignLjJgParaDlg::OnBnEndNormOffset() 
{
	CString sThickStr=normoffset.key_str;
#ifdef AFX_TARG_ENU_ENGLISH
	Input("input thickness string",&sThickStr,'s');
#else
	Input("ÊäÈëºñ¶È×Ö·û´®",&sThickStr,'s');
#endif
	_snprintf(normoffset.key_str,MAX_TEMP_CHAR_50,"%s",sThickStr);
	sThickStr.Format("%d",normoffset.Thick(console.GetActiveModel()));
	GetDlgItem(IDC_BN_END_NORM_OFFSET)->SetWindowText(sThickStr);
}

void CDesignLjJgParaDlg::OnBnEndDatumNormOffset() 
{
	CString sThickStr=normoffset.key_str;
#ifdef AFX_TARG_ENU_ENGLISH
	Input("input thickness string",&sThickStr,'s');
#else
	Input("ÊäÈëºñ¶È×Ö·û´®",&sThickStr,'s');
#endif
	_snprintf(normoffset.key_str,MAX_TEMP_CHAR_50,"%s",sThickStr);
	sThickStr.Format("%d",normoffset.Thick(console.GetActiveModel()));
	GetDlgItem(IDC_BN_END_DATUM_NORM_OFFSET)->SetWindowText(sThickStr);
}

BOOL CDesignLjJgParaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	long offset=normoffset.Thick(console.GetActiveModel());
	char thick_str[20]="";
	_snprintf(thick_str,19,"%d",offset);
	GetDlgItem(IDC_BN_END_NORM_OFFSET)->SetWindowText(thick_str);
	offset=datumnormoffset.Thick(console.GetActiveModel());
	_snprintf(thick_str,19,"%d",offset);
	GetDlgItem(IDC_BN_END_DATUM_NORM_OFFSET)->SetWindowText(thick_str);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
#endif