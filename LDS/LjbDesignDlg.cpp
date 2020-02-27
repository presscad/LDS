//<LOCALE_TRANSLATE Confirm by hxr/>
// LjbDesignDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "LjbDesignDlg.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "DataCompare.h"
#include "LocalFeatureDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLjbDesignDlg dialog
#ifndef __TSA_

CLjbDesignDlg::CLjbDesignDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CLjbDesignDlg::IDD, pParent)
	, m_bIntersAtAngleOuter(TRUE)
	, m_bIntersAtAngleInner(FALSE)
{
	//{{AFX_DATA_INIT(CLjbDesignDlg)
	m_bHuoQuPosByCtrlPt = FALSE;
	m_iPlankMaterial = 0;
	m_iRayJointStyle = 0;
	m_nHuoQuDist = 0;
	m_nHuoQuDist2 = 0;
	m_nOffsetZhun = 0;
	m_nOffsetZhun2 = 0;
	m_sPartNo = _T("");
	m_nPlankThick = 0;
	m_nVertexDist = 5;
	m_fNormOffset = 0.0;
	m_iProfileType = 1;
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	m_sSegI = _T("");
	m_bWeldLj = FALSE;
	//}}AFX_DATA_INIT
	m_nFaceN = 1;
	m_bEnableHuoQuPos = TRUE;
	jdb_style = 0;
	m_pBaseLineAngle=NULL;
	m_nNormDirection=0;
	base_x_wing0_y_wing1=0;
	reverseDirection=false;
}


void CLjbDesignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLjbDesignDlg)
	DDX_Check(pDX, IDC_CHK_HUOQU_POS_BY_CTRL_PT, m_bHuoQuPosByCtrlPt);
	DDX_CBIndex(pDX, IDC_CMB_PLANK_MATERIAL, m_iPlankMaterial);
	DDX_CBIndex(pDX, IDC_CMB_RAY_JOINT_STYLE, m_iRayJointStyle);
	DDX_Text(pDX, IDC_E_HUOQU_DIST, m_nHuoQuDist);
	DDX_Text(pDX, IDC_E_HUOQU_DIST2, m_nHuoQuDist2);
	DDX_Text(pDX, IDC_E_OFFSET_ZHUN, m_nOffsetZhun);
	DDX_Text(pDX, IDC_E_OFFSET_ZHUN2, m_nOffsetZhun2);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDV_MaxChars(pDX, m_sPartNo, 15);
	DDX_Text(pDX, IDC_PLANK_THICK, m_nPlankThick);
	DDX_Text(pDX, IDC_E_VERTEX_DIST, m_nVertexDist);
	DDX_Text(pDX, IDC_E_NORM_OFFSET, m_fNormOffset);
	DDX_CBIndex(pDX, IDC_CMB_PROFILE_TYPE, m_iProfileType);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDX_Check(pDX, IDC_CHK_WELD_LJ, m_bWeldLj);
	DDX_Check(pDX, IDC_CHK_AT_ANGLE_OUTER, m_bIntersAtAngleOuter);
	DDX_Check(pDX, IDC_CHK_AT_ANGLE_INNER, m_bIntersAtAngleInner);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLjbDesignDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CLjbDesignDlg)
	ON_CBN_SELCHANGE(IDC_CMB_RAY_JOINT_STYLE, OnSelchangeCmbRayJointStyle)
	ON_BN_CLICKED(IDC_BN_PASTE_NORM, OnBnPasteNorm)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangeEPartNo)
	ON_BN_CLICKED(IDC_CHK_WELD_LJ, OnWeldToDatumJg)
	ON_EN_CHANGE(IDC_PLANK_THICK, OnChangePlankThick)
	ON_BN_CLICKED(IDC_CHK_AT_ANGLE_OUTER, &CLjbDesignDlg::OnBnClickedChkAtAngleOuter)
	ON_BN_CLICKED(IDC_CHK_AT_ANGLE_INNER, &CLjbDesignDlg::OnBnClickedChkAtAngleInner)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_E_NORM_OFFSET, &CLjbDesignDlg::OnEnChangeENormOffset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLjbDesignDlg message handlers

BOOL CLjbDesignDlg::OnInitDialog() 
{
	if(m_nFaceN==3)
		GetDlgItem(IDC_CMB_PROFILE_TYPE)->EnableWindow(FALSE);
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_PLANK_MATERIAL);
	AddSteelMatRecord(pCMB);
	//以下四行是为了处理火曲角钢处于内皮的情况 WJH-2003.12.18
	//添加m_nFaceN==1||项，用于处理单面板交于内皮情况  WJH-2004.9.21
	//改为所有单面板及双面板均可以  WJH-2004.11.18
	if(m_nFaceN<=2)
	{
		GetDlgItem(IDC_CHK_AT_ANGLE_OUTER)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_AT_ANGLE_INNER)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CHK_AT_ANGLE_OUTER)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_AT_ANGLE_INNER)->EnableWindow(FALSE);
	}
	GetDlgItem(IDC_PLANK_THICK)->EnableWindow(TRUE);
	GetDlgItem(IDC_E_PART_NO)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_HUOQU_POS_BY_CTRL_PT)->EnableWindow(m_bEnableHuoQuPos);
	if(m_nFaceN==1)	//单面板
	{
		m_iRayJointStyle = 0;
		GetDlgItem(IDC_CMB_RAY_JOINT_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_DIST2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_OFFSET_ZHUN2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_HUOQU_POS_BY_CTRL_PT)->EnableWindow(FALSE);
		//单面板不需要显示火曲参数
		GetDlgItem(IDC_CHK_HUOQU_POS_BY_CTRL_PT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_WELD_LJ)->ShowWindow(SW_SHOW);
		if(m_pBaseLineAngle&&m_pBaseLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			m_bWeldLj=FALSE;	//基准角钢为组合角钢时不支持焊接连接
			GetDlgItem(IDC_CHK_WELD_LJ)->EnableWindow(FALSE);
		}
		else
			GetDlgItem(IDC_CHK_WELD_LJ)->EnableWindow(TRUE);
		if(m_bWeldLj)
		{	//节点板焊接到基准角钢上
			m_nPlankThick=ftoi(m_pBaseLineAngle->GetThick());
			m_fNormOffset=-m_nPlankThick;
		}
	}
	else if(m_nFaceN==2)	//双面板
	{
		GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_HUOQU_POS_BY_CTRL_PT)->EnableWindow(TRUE);
		//双面板需要显示火曲参数
		GetDlgItem(IDC_CHK_HUOQU_POS_BY_CTRL_PT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_WELD_LJ)->ShowWindow(SW_HIDE);
		if(m_iRayJointStyle>0)
		{
			GetDlgItem(IDC_E_HUOQU_DIST2)->EnableWindow(FALSE);
			if(m_iRayJointStyle==2)
				GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_OFFSET_ZHUN2)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_E_HUOQU_DIST2)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_OFFSET_ZHUN2)->EnableWindow(FALSE);
		}
	}
	else					//三面板
	{
		GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_HUOQU_DIST2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_HUOQU_POS_BY_CTRL_PT)->EnableWindow(TRUE);
		//双面板需要显示火曲参数
		GetDlgItem(IDC_CHK_HUOQU_POS_BY_CTRL_PT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_WELD_LJ)->ShowWindow(SW_HIDE);
		if(m_iRayJointStyle>0)
		{
			if(m_iRayJointStyle==2)
			{
				GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(TRUE);
				GetDlgItem(IDC_E_OFFSET_ZHUN2)->EnableWindow(TRUE);
			}
			else
			{
				GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
				GetDlgItem(IDC_E_OFFSET_ZHUN2)->EnableWindow(FALSE);
			}
		}
		else
		{
			GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_OFFSET_ZHUN2)->EnableWindow(FALSE);
		}
	}
	CDialog::OnInitDialog();
#if defined(__TMA_)||defined(__LMA_)
	if(g_sysPara.ciLiveFocusCtrlInLinkAngleDlg==2)//::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0)
	{	//1.规格框优先;2.构件编号优先
		GetDlgItem(IDC_E_PART_NO)->SetFocus();	//启用数据对比时，件号为默认焦点 wht 18-12-04
	}
#endif
	OnBnClickedChkAtAngleInner();
	MoveWndToLocation();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLjbDesignDlg::OnSelchangeCmbRayJointStyle() 
{
	UpdateData();
	//要与OninitDialog函数中相关代码要一致　wjh-2015.11.12
	if(m_nFaceN<=2)
	{
		GetDlgItem(IDC_CHK_AT_ANGLE_OUTER)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_AT_ANGLE_INNER)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CHK_AT_ANGLE_OUTER)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_AT_ANGLE_INNER)->EnableWindow(FALSE);
	}
	if(m_nFaceN==1)	//单面板
	{
		m_iRayJointStyle = 0;
		GetDlgItem(IDC_CMB_RAY_JOINT_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_DIST2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_OFFSET_ZHUN2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_HUOQU_POS_BY_CTRL_PT)->EnableWindow(FALSE);
	}
	else if(m_nFaceN==2)	//双面板
	{
		GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_HUOQU_POS_BY_CTRL_PT)->EnableWindow(TRUE);
		if(m_iRayJointStyle>0)
		{
			GetDlgItem(IDC_E_HUOQU_DIST2)->EnableWindow(FALSE);
			if(m_iRayJointStyle==2)
				GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_OFFSET_ZHUN2)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_E_HUOQU_DIST2)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_OFFSET_ZHUN2)->EnableWindow(FALSE);
		}
	}
	else					//三面板
	{
		if(m_iRayJointStyle>0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("3-plane plate does not support this connecting type, type rollback!");
#else
			AfxMessageBox("三面板不支持此种交接方式,类型回滚!");
#endif
			m_iRayJointStyle = 0;
			UpdateData(FALSE);
			return;
		}
		GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_HUOQU_DIST2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_HUOQU_POS_BY_CTRL_PT)->EnableWindow(TRUE);
		if(m_iRayJointStyle>0)
		{
			if(m_iRayJointStyle==2)
			{
				GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(TRUE);
				GetDlgItem(IDC_E_OFFSET_ZHUN2)->EnableWindow(TRUE);
			}
			else
			{
				GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
				GetDlgItem(IDC_E_OFFSET_ZHUN2)->EnableWindow(FALSE);
			}
		}
		else
		{
			GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_OFFSET_ZHUN2)->EnableWindow(FALSE);
		}
	}
}

void CLjbDesignDlg::OnBnPasteNorm() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fNormX = point.x;
		m_fNormY = point.y;
		m_fNormZ = point.z;
		UpdateData(FALSE);
	}
}

void CLjbDesignDlg::OnChangeEPartNo() 
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNo);
	if(pPart)
	{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
		if(pPart->cPartType==BOMPART::PLATE)
		{
			m_nPlankThick=(UINT)pPart->thick;
			m_iPlankMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		}
		//else
			//AfxMessageBox(CXhChar100("Bom中没有件号#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo.GetBuffer(),&iSeg,NULL))
		m_sSegI=iSeg.ToString();
	UpdateData(FALSE);
}

void CLjbDesignDlg::OnWeldToDatumJg() 
{
	if(m_pBaseLineAngle==NULL)
		return;
	UpdateData();
	if(m_bWeldLj)
	{
		m_nPlankThick=ftoi(m_pBaseLineAngle->GetThick());
		m_fNormOffset=-m_nPlankThick;
	}
	else
		m_fNormOffset=0;
	UpdateData(FALSE);
}
void CLjbDesignDlg::OnChangePlankThick() 
{
	if(m_pBaseLineAngle==NULL)
		return;
	UpdateData();
	IntelliCalNormOffset();
	OnBnClickedChkAtAngleInner();
	UpdateData(FALSE);
}

void CLjbDesignDlg::OnEnChangeENormOffset()
{
	CString sText;
	GetDlgItem(IDC_E_NORM_OFFSET)->GetWindowText(sText);
	if(sText.CompareNoCase("-")==0&&sText.GetLength()==1)
		return;
	if(m_pBaseLineAngle==NULL)
		return;
	UpdateData();
	if(m_pBaseLineAngle->GetClassTypeId()!=CLS_GROUPLINEANGLE)
	{
		m_bIntersAtAngleOuter=FALSE;
		m_bIntersAtAngleInner=FALSE;
	}
	else if(m_fNormOffset==0)
	{
		m_bIntersAtAngleOuter=TRUE;
		m_bIntersAtAngleInner=FALSE;
	}
	else if(m_fNormOffset=-m_pBaseLineAngle->GetThick()-m_nPlankThick)
	{
		m_bIntersAtAngleOuter=FALSE;
		m_bIntersAtAngleInner=TRUE;
	}
	UpdateData(FALSE);
}

void CLjbDesignDlg::OnBnClickedChkAtAngleOuter()
{
	if(m_pBaseLineAngle==NULL)
		return;
	UpdateData();
	m_bIntersAtAngleInner=!m_bIntersAtAngleOuter;
	IntelliCalNormOffset();
	UpdateData(FALSE);
}

void CLjbDesignDlg::OnBnClickedChkAtAngleInner()
{
	if(m_pBaseLineAngle==NULL)
		return;
	UpdateData();
	m_bIntersAtAngleOuter=!m_bIntersAtAngleInner;
	IntelliCalNormOffset();
	UpdateData(FALSE);
}
void CLjbDesignDlg::IntelliCalNormOffset()
{
	if(m_bWeldLj)
		m_fNormOffset=-m_nPlankThick;
	else
	{
		m_fNormOffset=0;
		if(m_pBaseLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)m_pBaseLineAngle;
			if(pGroupAngle->size_cSubClassType!='T'||pGroupAngle->ciMasterIndex==base_x_wing0_y_wing1)
			{
				m_fNormOffset=-m_nPlankThick/2.0;
				if(reverseDirection)	//钢板法向与所在肢法向相反，调整法向偏移量
					m_fNormOffset*=-1;
			}
		}
		if(!m_bIntersAtAngleOuter&&m_pBaseLineAngle)
			m_fNormOffset-=(m_pBaseLineAngle->GetThick()+m_nPlankThick);
	}
}
#endif

