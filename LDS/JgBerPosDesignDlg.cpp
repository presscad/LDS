// JgBerPosDesignDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "JgBerPosDesignDlg.h"
#include "NodePrjLineDefDlg.h"
#include "JgNodePrjDlg.h"
#include "GlobalFunc.h"
#include "ModifyVertexDlg.h"
#include "DatumFaceOffsetDlg.h"
#include "DatumPointDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJgBerPosDesignDlg dialog


CJgBerPosDesignDlg::CJgBerPosDesignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJgBerPosDesignDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJgBerPosDesignDlg)
	m_fOffsetWingYDist = 0.0;
	m_fOffsetWingXDist = 0.0;
	m_iWingXOffsetStyle = 0;
	m_iWingYOffsetStyle = 0;
	m_hHuoQuBaseJg = _T("");
	m_fHuoQuDist = 0.0;
	m_fJointX = 0.0;
	m_fJointY = 0.0;
	m_fJointZ = 0.0;
	m_fOffsetZhun = 0.0;
	m_hDaJiePosJg = _T("");
	m_fDatumNormOffset = 0.0;
	m_iDatumPosStyle = 0;
	m_iDatumToJgBerStyle = 0;
	m_iJgBerCalStyle = 0;
	//}}AFX_DATA_INIT
	face_offset_norm.Set(0,0,1);
	m_pPoint=NULL;
	m_bEnableDaJiePosJg=TRUE;
	//平推处理
	m_iBaseOffsetStyle=0;			//根据另一端进行平推计算
}


void CJgBerPosDesignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJgBerPosDesignDlg)
	DDX_Text(pDX, IDC_E_OFFSET_WING_Y, m_fOffsetWingYDist);
	DDX_Text(pDX, IDC_E_OFFSET_WING_X, m_fOffsetWingXDist);
	DDX_CBIndex(pDX, IDC_CMB_WING_X_OFFSET_STYLE, m_iWingXOffsetStyle);
	DDX_CBIndex(pDX, IDC_CMB_WING_Y_OFFSET_STYLE, m_iWingYOffsetStyle);
	DDX_Text(pDX, IDC_E_HUOQU_BASEJG, m_hHuoQuBaseJg);
	DDX_Text(pDX, IDC_E_HUOQU_DIST, m_fHuoQuDist);
	DDX_Text(pDX, IDC_E_HUOQU_JOINT_X, m_fJointX);
	DDX_Text(pDX, IDC_E_HUOQU_JOINT_Y, m_fJointY);
	DDX_Text(pDX, IDC_E_HUOQU_JOINT_Z, m_fJointZ);
	DDX_Text(pDX, IDC_E_OFFSET_ZHUN, m_fOffsetZhun);
	DDX_Text(pDX, IDC_E_H_DAJIE_POS_JG, m_hDaJiePosJg);
	DDX_Text(pDX, IDC_E_NORM_OFFSET, m_fDatumNormOffset);
	DDX_CBIndex(pDX, IDC_CMB_DATUM_POS_STYLE, m_iDatumPosStyle);
	DDX_CBIndex(pDX, IDC_CMB_DATUM_TO_BER_STYLE, m_iDatumToJgBerStyle);
	DDX_CBIndex(pDX, IDC_CMB_JGBER_CAL_STYLE, m_iJgBerCalStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJgBerPosDesignDlg, CDialog)
	//{{AFX_MSG_MAP(CJgBerPosDesignDlg)
	ON_CBN_SELCHANGE(IDC_CMB_WING_X_OFFSET_STYLE, OnSelchangeCmbWingXOffsetStyle)
	ON_CBN_SELCHANGE(IDC_CMB_WING_Y_OFFSET_STYLE, OnSelchangeCmbWingYOffsetStyle)
	ON_BN_CLICKED(IDC_BN_PT_LINE_DATUM, OnBnPtLineDatum)
	ON_BN_CLICKED(IDC_BN_COPY, OnBnCopy)
	ON_BN_CLICKED(IDC_BN_PASTE, OnBnPaste)
	ON_CBN_SELCHANGE(IDC_CMB_DATUM_POS_STYLE, OnSelchangeCmbDatumPosStyle)
	ON_BN_CLICKED(IDC_BN_OFFSET_FACE_PARA, OnBnOffsetFacePara)
	ON_CBN_SELCHANGE(IDC_CMB_DATUM_TO_BER_STYLE, OnSelchangeCmbDatumToBerStyle)
	ON_CBN_SELCHANGE(IDC_CMB_JGBER_CAL_STYLE, OnSelchangeCmbJgberCalStyle)
	ON_BN_CLICKED(IDC_BN_SPEC_BASE_POS, OnBnSpecBasePos)
	ON_BN_CLICKED(IDC_BN_OFFSET_FACE_PARA2, OnBnOffsetFacePara)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJgBerPosDesignDlg message handlers

BOOL CJgBerPosDesignDlg::OnInitDialog() 
{
	if(m_pPoint==NULL)
		return FALSE;
	RefreshDisplayStatus();	
	m_fJointX = m_pPoint->Position().x;
	m_fJointY = m_pPoint->Position().y;
	m_fJointZ = m_pPoint->Position().z;
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CJgBerPosDesignDlg::OnSelchangeCmbWingXOffsetStyle() 
{
	UpdateData();
	if(m_iWingXOffsetStyle!=4)
		GetDlgItem(IDC_E_OFFSET_WING_X)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_E_OFFSET_WING_X)->EnableWindow(TRUE);
	switch(m_iWingXOffsetStyle)
	{
	case 0:
		m_fOffsetWingXDist = wing_y_jgzj.g;
		break;
	case 1:
		m_fOffsetWingXDist = wing_y_jgzj.g1;
		break;
	case 2:
		m_fOffsetWingXDist = wing_y_jgzj.g2;
		break;
	case 3:
		m_fOffsetWingXDist = wing_y_jgzj.g3;
		break;
	default:
		break;
	}
	UpdateData(FALSE);
}

void CJgBerPosDesignDlg::OnSelchangeCmbWingYOffsetStyle() 
{
	UpdateData();
	if(m_iWingYOffsetStyle!=4)
		GetDlgItem(IDC_E_OFFSET_WING_Y)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_E_OFFSET_WING_Y)->EnableWindow(TRUE);
	
	switch(m_iWingYOffsetStyle)
	{
	case 0:
		m_fOffsetWingYDist = wing_x_jgzj.g;
		break;
	case 1:
		m_fOffsetWingYDist = wing_x_jgzj.g1;
		break;
	case 2:
		m_fOffsetWingYDist = wing_x_jgzj.g2;
		break;
	case 3:
		m_fOffsetWingYDist = wing_x_jgzj.g3;
		break;
	default:
		break;
	}
	UpdateData(FALSE);
}

void CJgBerPosDesignDlg::OnBnPtLineDatum() 
{
	CNodePrjLineDefDlg dlg;
	dlg.m_hStartJg.Format("0X%X",m_hStartJg);
	dlg.m_hEndJg.Format("0X%X",m_hEndJg);
	dlg.m_iStartJgBerStyle = m_iStartJgBerStyle;
	dlg.m_iEndJgBerStyle   = m_iEndJgBerStyle;
	dlg.m_fStartJgLengOffsetX=m_fStartJgLengOffsetX;
	dlg.m_fStartJgLengOffsetY=m_fStartJgLengOffsetY;
	dlg.m_fEndJgLengOffsetX=m_fEndJgLengOffsetX;
	dlg.m_fEndJgLengOffsetY=m_fEndJgLengOffsetY;
	if(dlg.DoModal()==IDOK)
	{
		sscanf(dlg.m_hStartJg,"%X",&m_hStartJg);
		sscanf(dlg.m_hEndJg,"%X",&m_hEndJg);
		m_iStartJgBerStyle = dlg.m_iStartJgBerStyle;
		m_iEndJgBerStyle   = dlg.m_iEndJgBerStyle;
		m_fStartJgLengOffsetX=dlg.m_fStartJgLengOffsetX;
		m_fStartJgLengOffsetY=dlg.m_fStartJgLengOffsetY;
		m_fEndJgLengOffsetX=dlg.m_fEndJgLengOffsetX;
		m_fEndJgLengOffsetY=dlg.m_fEndJgLengOffsetY;
	}
	/*else if(m_iOffsetStyle==0)	//普通投影
	{
		CJgNodePrjDlg dlg;
		dlg.m_iPosStyle = m_iJgNodePrjStyle;
		if(dlg.DoModal())
		{
			m_iJgNodePrjStyle = dlg.m_iPosStyle;
		}
	}*/
}

void CJgBerPosDesignDlg::OnBnCopy() 
{
	f3dPoint point;
	UpdateData();
	point.Set(m_fJointX,m_fJointY,m_fJointZ);
	WritePointToClip(point);
}

void CJgBerPosDesignDlg::OnBnPaste() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fJointX = point.x;
		m_fJointY = point.y;
		m_fJointZ = point.z;
		UpdateData(FALSE);
	}
}

void CJgBerPosDesignDlg::OnBnOffsetFacePara() 
{
	CDatumFaceOffsetDlg dlg;
	dlg.m_fNormX = face_offset_norm.x;
	dlg.m_fNormY = face_offset_norm.y;
	dlg.m_fNormZ = face_offset_norm.z;
	dlg.m_iBaseStyle = m_iBaseOffsetStyle;
	if(dlg.DoModal()==IDOK)
	{
		face_offset_norm.x = dlg.m_fNormX;
		face_offset_norm.y = dlg.m_fNormY;
		face_offset_norm.z = dlg.m_fNormZ;
		m_iBaseOffsetStyle = dlg.m_iBaseStyle;
	}
}

void CJgBerPosDesignDlg::OnSelchangeCmbJgberCalStyle() 
{
	UpdateData();
	RefreshDisplayStatus();
}

void CJgBerPosDesignDlg::OnSelchangeCmbDatumPosStyle() 
{
	UpdateData();
	RefreshDisplayStatus();
}

void CJgBerPosDesignDlg::OnSelchangeCmbDatumToBerStyle() 
{
	UpdateData();
	RefreshDisplayStatus();
}

void CJgBerPosDesignDlg::RefreshDisplayStatus()
{
	if(m_iJgBerCalStyle==0)	//分两步偏移
	{
		GetDlgItem(IDC_BN_PT_LINE_DATUM)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_DATUM_POS_STYLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CMB_DATUM_TO_BER_STYLE)->EnableWindow(FALSE);
		if(m_iDatumPosStyle==0)		//节点即基点
		{
			GetDlgItem(IDC_BN_OFFSET_FACE_PARA)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_E_H_DAJIE_POS_JG)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_NORM_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_BASEJG)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(FALSE);
			GetDlgItem(IDC_BN_SPEC_BASE_POS)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_Z)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_DATUM_TO_BER_STYLE)->EnableWindow(TRUE);
		}
		else if(m_iDatumPosStyle==1||m_iDatumPosStyle==2||m_iDatumPosStyle==3)
		{//向楞线投影|向X肢心线投影|向Y肢心线投影
			GetDlgItem(IDC_BN_OFFSET_FACE_PARA)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_E_H_DAJIE_POS_JG)->EnableWindow(m_bEnableDaJiePosJg);
			GetDlgItem(IDC_E_NORM_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_BASEJG)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(FALSE);
			GetDlgItem(IDC_BN_SPEC_BASE_POS)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_Z)->EnableWindow(FALSE);
			m_iDatumToJgBerStyle=0;
		}
		else if(m_iDatumPosStyle==4||m_iDatumPosStyle==5||m_iDatumPosStyle==6)
		{//向楞线平推|向X肢心线平推|向Y肢心线平推
			GetDlgItem(IDC_BN_OFFSET_FACE_PARA)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_E_H_DAJIE_POS_JG)->EnableWindow(m_bEnableDaJiePosJg);
			GetDlgItem(IDC_E_NORM_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_BASEJG)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(FALSE);
			GetDlgItem(IDC_BN_SPEC_BASE_POS)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_Z)->EnableWindow(FALSE);
			m_iDatumToJgBerStyle=0;
		}
		else if(m_iDatumPosStyle>=7&&m_iDatumPosStyle<=12)
		{//投影楞线火曲线|投影X肢心线火曲线|投影Y肢心线火曲线
			if(m_iDatumPosStyle>=10)
				GetDlgItem(IDC_BN_OFFSET_FACE_PARA)->ShowWindow(SW_SHOW);
			else
				GetDlgItem(IDC_BN_OFFSET_FACE_PARA)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_E_H_DAJIE_POS_JG)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_NORM_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_BASEJG)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(TRUE);
			GetDlgItem(IDC_BN_SPEC_BASE_POS)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_Z)->EnableWindow(FALSE);
			m_iDatumToJgBerStyle=0;
		}
		else if(m_iDatumPosStyle==13||m_iDatumPosStyle==14)
		{//X肢偏移准线|Y肢偏移准线
			GetDlgItem(IDC_BN_OFFSET_FACE_PARA)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_E_H_DAJIE_POS_JG)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_NORM_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_HUOQU_BASEJG)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(TRUE);
			GetDlgItem(IDC_BN_SPEC_BASE_POS)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_JOINT_Z)->EnableWindow(FALSE);
			m_iDatumToJgBerStyle=0;
		}
		else //if(m_iDatumPosStyle==15)//直接指定基点坐标
		{
			GetDlgItem(IDC_BN_OFFSET_FACE_PARA)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_E_H_DAJIE_POS_JG)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_NORM_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_BASEJG)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(FALSE);
			GetDlgItem(IDC_BN_SPEC_BASE_POS)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_HUOQU_JOINT_X)->EnableWindow(m_pPoint->datum_pos_style==0);
			GetDlgItem(IDC_E_HUOQU_JOINT_Y)->EnableWindow(m_pPoint->datum_pos_style==0);
			GetDlgItem(IDC_E_HUOQU_JOINT_Z)->EnableWindow(m_pPoint->datum_pos_style==0);
			m_iDatumToJgBerStyle=0;
		}
		if(m_iDatumToJgBerStyle==0)	//投影
			GetDlgItem(IDC_BN_OFFSET_FACE_PARA2)->ShowWindow(SW_HIDE);
		else //if(m_iDatumToJgBerStyle==1)	//平推
			GetDlgItem(IDC_BN_OFFSET_FACE_PARA2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CMB_WING_X_OFFSET_STYLE)->EnableWindow(TRUE);
		if(m_iWingXOffsetStyle==4)
			GetDlgItem(IDC_E_OFFSET_WING_X)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_E_OFFSET_WING_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_WING_Y_OFFSET_STYLE)->EnableWindow(TRUE);
		if(m_iWingYOffsetStyle==4)
			GetDlgItem(IDC_E_OFFSET_WING_Y)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_E_OFFSET_WING_Y)->EnableWindow(FALSE);
		switch(m_iWingXOffsetStyle)
		{
		case 0:
			m_fOffsetWingXDist = wing_y_jgzj.g;
			break;
		case 1:
			m_fOffsetWingXDist = wing_y_jgzj.g1;
			break;
		case 2:
			m_fOffsetWingXDist = wing_y_jgzj.g2;
			break;
		case 3:
			m_fOffsetWingXDist = wing_y_jgzj.g3;
			break;
		default:
			break;
		}
		switch(m_iWingYOffsetStyle)
		{
		case 0:
			m_fOffsetWingYDist = wing_x_jgzj.g;
			break;
		case 1:
			m_fOffsetWingYDist = wing_x_jgzj.g1;
			break;
		case 2:
			m_fOffsetWingYDist = wing_x_jgzj.g2;
			break;
		case 3:
			m_fOffsetWingYDist = wing_x_jgzj.g3;
			break;
		default:
			break;
		}
	}
	else if(m_iJgBerCalStyle==1||m_iJgBerCalStyle==2)	//点向线投影或平推
	{
		GetDlgItem(IDC_BN_PT_LINE_DATUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_CMB_DATUM_POS_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_OFFSET_FACE_PARA)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_H_DAJIE_POS_JG)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_NORM_OFFSET)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_BASEJG)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_SPEC_BASE_POS)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_JOINT_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_JOINT_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_JOINT_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_DATUM_TO_BER_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_OFFSET_FACE_PARA2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CMB_WING_X_OFFSET_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_OFFSET_WING_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_WING_Y_OFFSET_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_OFFSET_WING_Y)->EnableWindow(FALSE);
	}
	else //if(m_iJgBerCalStyle==3)	//直接指定坐标
	{
		GetDlgItem(IDC_BN_PT_LINE_DATUM)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_DATUM_POS_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_OFFSET_FACE_PARA)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_H_DAJIE_POS_JG)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_NORM_OFFSET)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_OFFSET_ZHUN)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_BASEJG)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_DIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_SPEC_BASE_POS)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_JOINT_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_JOINT_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HUOQU_JOINT_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_DATUM_TO_BER_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_OFFSET_FACE_PARA2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CMB_WING_X_OFFSET_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_OFFSET_WING_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_WING_Y_OFFSET_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_OFFSET_WING_Y)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);
}

void CJgBerPosDesignDlg::OnBnSpecBasePos() 
{
	CDatumPointDlg dlg;
	dlg.m_pPoint=m_pPoint;
#ifdef AFX_TARG_ENU_ENGLISH
	dlg.m_sPointDescription="Angle Datum Point";
	dlg.m_shDatumPoint="Handle:None";
#else 
	dlg.m_sPointDescription="角钢基点";
	dlg.m_shDatumPoint="句柄:无";
#endif
	if(dlg.DoModal()==IDOK)
	{
		m_fJointX=m_pPoint->Position().x;
		m_fJointY=m_pPoint->Position().y;
		m_fJointZ=m_pPoint->Position().z;
		UpdateData(FALSE);
		RefreshDisplayStatus();
	}
}
