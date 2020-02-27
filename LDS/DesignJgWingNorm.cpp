//<LOCALE_TRANSLATE Confirm by hxr/>
// DesignJgWingNorm.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "math.h"
#include "f_ent.h"
#include "env_def.h"
#include "DesignJgWingNorm.h"
#include "DesignVectorPropDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignJgWingNorm dialog


CDesignJgWingNorm::CDesignJgWingNorm(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignJgWingNorm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignJgWingNorm)
	m_bCalByOtherWing = FALSE;
	m_bSpecificNorm = FALSE;
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	m_hRefJg = _T("");
	m_fNearNormX = 0.0;
	m_fNearNormY = 0.0;
	m_fNearNormZ = 0.0;
	//}}AFX_DATA_INIT
}


void CDesignJgWingNorm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignJgWingNorm)
	DDX_Check(pDX, IDC_CHK_CAL_BY_OTHER_WING, m_bCalByOtherWing);
	DDX_Check(pDX, IDC_CHK_SPECIFIC_NORM, m_bSpecificNorm);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_Text(pDX, IDC_E_REF_JG_HANDLE, m_hRefJg);
	DDX_Text(pDX, IDC_E_NEAR_NORM_X, m_fNearNormX);
	DDX_Text(pDX, IDC_E_NEAR_NORM_Y, m_fNearNormY);
	DDX_Text(pDX, IDC_E_NEAR_NORM_Z, m_fNearNormZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignJgWingNorm, CDialog)
	//{{AFX_MSG_MAP(CDesignJgWingNorm)
	ON_BN_CLICKED(IDC_BN_DES_JG_WING_NORM, OnBnDesJgWingNorm)
	ON_BN_CLICKED(IDC_CHK_SPECIFIC_NORM, RefreshDisplayStatus)
	ON_BN_CLICKED(ID_BN_COPY, OnBnCopy)
	ON_BN_CLICKED(IDC_CHK_CAL_BY_OTHER_WING, RefreshDisplayStatus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignJgWingNorm message handlers

void CDesignJgWingNorm::OnOK() 
{
	UpdateData();
	if(!m_bCalByOtherWing&&!m_bSpecificNorm)	//不是由另一肢法线确定,时必须指定一根参考角钢和近似法线
	{
		if(fabs(m_fNormX)+fabs(m_fNormY)+fabs(m_fNormZ)<EPS)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("current leg's near normal direction should be confirmed!");
#else
			AfxMessageBox("你必须指定一个当前肢的近似法线方向!");
#endif
			return;
		}
		THANDLE hRefPart;
		sscanf(m_hRefJg,"%X",&hRefPart);
		CLDSPart *pPart=console.FromPartHandle(hRefPart);
		if(pPart==NULL||!pPart->IsLinePart())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Specify a leg normal reference rod which not collinear with current angle!");
#else
			AfxMessageBox("你必须指定一个与当前角钢不共线的肢法线参考杆件!");
#endif
			return;
		}
	}
	
	CDialog::OnOK();
}

void CDesignJgWingNorm::OnBnDesJgWingNorm() 
{
	CDesignVectorPropDlg vector_dlg;
	vector_dlg.m_xVector=spec_norm;
	if(vector_dlg.DoModal()==IDOK)
	{
		spec_norm=vector_dlg.m_xVector;
		spec_norm.UpdateVector(console.GetActiveModel());
	}
}

BOOL CDesignJgWingNorm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RefreshDisplayStatus();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDesignJgWingNorm::RefreshDisplayStatus() 
{
	UpdateData();
	GetDlgItem(IDC_BN_DES_JG_WING_NORM)->EnableWindow(m_bSpecificNorm);
	GetDlgItem(IDC_CHK_CAL_BY_OTHER_WING)->EnableWindow(!m_bSpecificNorm);
	((CEdit*)GetDlgItem(IDC_E_REF_JG_HANDLE))->SetReadOnly(m_bSpecificNorm||m_bCalByOtherWing);
	m_bCalByOtherWing=FALSE;
}

void CDesignJgWingNorm::OnBnCopy() 
{
	f3dPoint point;
	UpdateData();
	point.Set(m_fNormX,m_fNormY,m_fNormZ);
	WritePointToClip(point);
}
