// DatumPointDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "Query.h"
#include "ScrTools.h"
#include "GlobalFunc.h"
#include "DatumPointDlg.h"
#include "DatumFaceOffsetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDatumPointDlg dialog


CDatumPointDlg::CDatumPointDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDatumPointDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDatumPointDlg)
	m_shDatumPoint = _T("");
	m_sPointDescription = _T("");
	m_fPosX = 0.0;
	m_fPosY = 0.0;
	m_fPosZ = 0.0;
	m_fPosXInPart = 0.0;
	m_fPosYInPart = 0.0;
	m_fPosZInPart = 0.0;
	m_shFirstHandle = _T("");
	m_shSecondHandle = _T("");
	m_iDatumPosCalStyle = 0;
	m_iSpecAxis = 0;
	m_fExtendLen = 0.0;
	m_fWingOffsetDist = 0.0;
	m_iOffsetWing = 0;
	m_iExtendDirection = 0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sFirstHandleName = _T("Datum Angle Handle:");
	m_sSecondHandleName = _T("Datum Node Handle:");
#else
	m_sFirstHandleName = _T("基准角钢句柄:");
	m_sSecondHandleName = _T("基准节点句柄:");
#endif
	m_iWingOffsetStyle = 0;
	m_bIncOddEffect = FALSE;
	m_bFaceOffset = FALSE;
	m_iWingOffsetStyle2 = 0;
	m_fWingOffsetDist2 = 0.0;
	m_iOffsetFaceType = -1;
	//}}AFX_DATA_INIT
	m_hFirstHandle=m_hSecondHandle=m_hDatumPart=0;
	g[0]=g1[0]=g2[0]=g3[0]=g[1]=g1[1]=g2[1]=g3[1]=0;
}

void CDatumPointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDatumPointDlg)
	DDX_Text(pDX, IDC_S_H_DATUM_POINT, m_shDatumPoint);
	DDX_Text(pDX, IDC_E_POINT_DESCRIP, m_sPointDescription);
	DDX_Text(pDX, IDC_E_POS_X, m_fPosX);
	DDX_Text(pDX, IDC_E_POS_Y, m_fPosY);
	DDX_Text(pDX, IDC_E_POS_Z, m_fPosZ);
	DDX_Text(pDX, IDC_E_POS_X_IN_PART, m_fPosXInPart);
	DDX_Text(pDX, IDC_E_POS_Y_IN_PART, m_fPosYInPart);
	DDX_Text(pDX, IDC_E_POS_Z_IN_PART, m_fPosZInPart);
	DDX_Text(pDX, IDC_E_FIRST_HANDLE, m_shFirstHandle);
	DDX_Text(pDX, IDC_E_SECOND_HANDLE, m_shSecondHandle);
	DDX_CBIndex(pDX, IDC_CMB_DATUM_POINT_CAL_STYLE, m_iDatumPosCalStyle);
	DDX_CBIndex(pDX, IDC_CMB_SPEC_AXIS, m_iSpecAxis);
	DDX_Text(pDX, IDC_E_EXTEND_LEN, m_fExtendLen);
	DDX_Text(pDX, IDC_E_WING_OFFSET_DIST, m_fWingOffsetDist);
	DDX_Radio(pDX, IDC_RDO_OFFSET_VEC, m_iOffsetWing);
	DDX_Radio(pDX, IDC_RDO_EXTEND_DIRECTION, m_iExtendDirection);
	DDX_Text(pDX, IDC_S_FIRST_HANDLE_NAME, m_sFirstHandleName);
	DDX_Text(pDX, IDC_S_SECOND_HANDLE_NAME, m_sSecondHandleName);
	DDX_CBIndex(pDX, IDC_CMB_WING_OFFSET_STYLE, m_iWingOffsetStyle);
	DDX_Check(pDX, IDC_CHK_LEN_OFFSET_INC_ODD, m_bIncOddEffect);
	DDX_Check(pDX, IDC_CHK_FACE_OFFSET, m_bFaceOffset);
	DDX_CBIndex(pDX, IDC_CMB_WING_OFFSET_STYLE2, m_iWingOffsetStyle2);
	DDX_Text(pDX, IDC_E_WING_OFFSET_DIST2, m_fWingOffsetDist2);
	DDX_CBIndex(pDX, IDC_CMB_OFFSET_FACE_TYPE, m_iOffsetFaceType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDatumPointDlg, CDialog)
	//{{AFX_MSG_MAP(CDatumPointDlg)
	ON_CBN_SELCHANGE(IDC_CMB_DATUM_POINT_CAL_STYLE, OnSelchangeCmbDatumPointCalStyle)
	ON_CBN_SELCHANGE(IDC_CMB_WING_OFFSET_STYLE, OnSelchangeCmbWingOffsetStyle)
	ON_CBN_SELCHANGE(IDC_CMB_SPEC_AXIS, OnSelchangeCmbSpecAxis)
	ON_BN_CLICKED(IDC_BN_NORM_OFFSET, OnBnNormOffset)
	ON_CBN_SELCHANGE(IDC_CMB_WING_OFFSET_STYLE2, OnSelchangeCmbWingOffsetStyle2)
	ON_BN_CLICKED(IDC_CHK_FACE_OFFSET, OnChkFaceOffset)
	ON_BN_CLICKED(IDC_BN_PASTE_VEC, OnBnPasteVec)
	ON_BN_CLICKED(IDC_BN_PASTE_POS_IN_PART, OnBnPastePosInPart)
	ON_BN_CLICKED(IDC_BN_COPY_POS_IN_PART, OnBnCopyPosInPart)
	ON_BN_CLICKED(IDC_BN_COPY_VEC, OnBnCopyVec)
	ON_CBN_SELCHANGE(IDC_CMB_OFFSET_FACE_TYPE, OnSelchangeCmbOffsetFaceType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatumPointDlg message handlers

BOOL CDatumPointDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_pPoint->BelongModel()==NULL)
		m_pPoint->SetBelongModel(console.GetActiveModel());
	if(strlen(m_pPoint->description)>0)
		m_sPointDescription =m_pPoint->description;
	m_pPoint->UpdatePos(console.GetActiveModel()); //更新基点位置 wht 11-01-24
	m_iDatumPosCalStyle =m_pPoint->datum_pos_style;
	m_fPosX=m_pPoint->Position().x;
	m_fPosY=m_pPoint->Position().y;
	m_fPosZ=m_pPoint->Position().z;
	m_fPosXInPart=m_pPoint->datum_pos.x;
	m_fPosYInPart=m_pPoint->datum_pos.y;
	m_fPosZInPart=m_pPoint->datum_pos.z;
	m_bIncOddEffect=m_pPoint->des_para.RODEND.bIncOddEffect;
	GetDlgItem(IDC_CHK_LEN_OFFSET_INC_ODD)->EnableWindow(FALSE);
	double x=fabs(m_pPoint->des_para.RODNODE.vPlaneNormal.x);
	double y=fabs(m_pPoint->des_para.RODNODE.vPlaneNormal.y);
	double z=fabs(m_pPoint->des_para.RODNODE.vPlaneNormal.z);
	if(x>EPS&&y<EPS&&z<EPS)
		m_iOffsetFaceType=0;
	else if(y>EPS&&x<EPS&&z<EPS)
		m_iOffsetFaceType=1;
	else if(z>EPS&&y<EPS&&x<EPS)
		m_iOffsetFaceType=2;
	else
		m_iOffsetFaceType=3;
	UpdateData(FALSE);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(m_pPoint->datum_pos_style==1||m_pPoint->datum_pos_style==7)	//角钢楞点定位
	{
		m_iExtendDirection=m_pPoint->des_para.RODEND.direction;
		m_iOffsetWing=m_pPoint->des_para.RODEND.offset_wing;
		m_fExtendLen=m_pPoint->des_para.RODEND.len_offset_dist;
		m_iWingOffsetStyle=m_pPoint->des_para.RODEND.wing_offset_style;
		m_fWingOffsetDist=m_pPoint->des_para.RODEND.wing_offset_dist;
		CDesignThickPara thick(m_pPoint->des_para.RODEND.norm_offset);
		CString ss;
		ss.Format("%d",thick.Thick(m_pPoint->BelongModel()));
		GetDlgItem(IDC_BN_NORM_OFFSET)->SetWindowText(ss);
		if(m_pPoint->datum_pos_style==1)
			GetDlgItem(IDC_CHK_LEN_OFFSET_INC_ODD)->EnableWindow(TRUE);
		else
		{
			m_bIncOddEffect=TRUE;
			GetDlgItem(IDC_CHK_LEN_OFFSET_INC_ODD)->EnableWindow(FALSE);
		}
		CLDSLineAngle *pJg=(CLDSLineAngle*)console.FromPartHandle(m_pPoint->des_para.RODEND.hRod,CLS_LINEANGLE);
		if(pJg)
		{
			JGZJ jgzj;
			if(pJg->m_bEnableTeG&&m_iOffsetWing==0)
				jgzj=pJg->xWingXZhunJu;
			else if(pJg->m_bEnableTeG&&m_iOffsetWing==1)
				jgzj=pJg->xWingYZhunJu;
			else
			{
				getjgzj(jgzj,pJg->GetWidth());
				g[0]=jgzj.g;
				g1[0]=jgzj.g1;
				g2[0]=jgzj.g2;
				g3[0]=jgzj.g3;
			}
		}
		m_hFirstHandle=m_pPoint->des_para.RODEND.hRod;
	}
	else if(m_pPoint->datum_pos_style==2)	//角钢节点定位
	{
		CLDSLineAngle *pJg=(CLDSLineAngle*)console.FromPartHandle(m_pPoint->des_para.RODNODE.hRod ,CLS_LINEANGLE);
		m_hFirstHandle=m_pPoint->des_para.RODNODE.hRod ;
		m_hSecondHandle=m_pPoint->des_para.RODNODE.hNode;
		m_iExtendDirection=m_pPoint->des_para.RODNODE.direction;
		m_iOffsetWing=m_pPoint->des_para.RODNODE.offset_wing;
		m_fExtendLen=m_pPoint->des_para.RODNODE.len_offset_dist;
		m_iWingOffsetStyle=m_pPoint->des_para.RODNODE.wing_offset_style;
		m_fWingOffsetDist=m_pPoint->des_para.RODNODE.wing_offset_dist;
		m_bFaceOffset=m_pPoint->des_para.RODNODE.bFaceOffset;
		CDesignThickPara thick(m_pPoint->des_para.RODNODE.norm_offset);
		CString ss;
		ss.Format("%d",thick.Thick(m_pPoint->BelongModel()));
		GetDlgItem(IDC_BN_NORM_OFFSET)->SetWindowText(ss);
		if(pJg)
		{
			JGZJ jgzj;
			if(pJg->m_bEnableTeG&&m_iOffsetWing==0)
				jgzj=pJg->xWingXZhunJu;
			else if(pJg->m_bEnableTeG&&m_iOffsetWing==1)
				jgzj=pJg->xWingYZhunJu;
			else
			{
				getjgzj(jgzj,pJg->GetWidth());
				g[0]=jgzj.g;
				g1[0]=jgzj.g1;
				g2[0]=jgzj.g2;
				g3[0]=jgzj.g3;
			}
		}
	}
	else if(m_pPoint->datum_pos_style==3||m_pPoint->datum_pos_style==4)
	{	//角钢心线交点或基准线空间交点
		m_hFirstHandle=m_pPoint->des_para.AXIS_INTERS.hDatum1;
		m_hSecondHandle=m_pPoint->des_para.AXIS_INTERS.hDatum2;
		CSuperSmartPtr<CLDSPart>pDatumPart1,pDatumPart2;
		if(m_pPoint->datum_pos_style==3)
		{
			pDatumPart1=console.FromPartHandle(m_hFirstHandle);
			pDatumPart2=console.FromPartHandle(m_hSecondHandle);
			if(pDatumPart1.IsHasPtr()&&pDatumPart2.IsHasPtr()&&
				pDatumPart1->IsLinePart()&&pDatumPart2->IsLinePart())
			{
				f3dPoint line_vec1=pDatumPart1.LinePartPointer()->End()-pDatumPart1.LinePartPointer()->Start();
				f3dPoint line_vec2=pDatumPart2.LinePartPointer()->End()-pDatumPart2.LinePartPointer()->Start();
				f3dPoint wing_norm=line_vec1^line_vec2;
				normalize(wing_norm);
				if(pDatumPart1->GetClassTypeId()==CLS_LINEANGLE)
				{
					JGZJ jgzj;
					int offset_wing=0;
					if( fabs(wing_norm*pDatumPart1.LineAnglePointer()->get_norm_y_wing())>
						fabs(wing_norm*pDatumPart1.LineAnglePointer()->get_norm_x_wing()))
						offset_wing=1;
					if(pDatumPart1.LineAnglePointer()->m_bEnableTeG&&offset_wing==0)	//m_iOffsetWing==0)
						jgzj=pDatumPart1.LineAnglePointer()->xWingXZhunJu;
					else if(pDatumPart1.LineAnglePointer()->m_bEnableTeG&&offset_wing==1)//m_iOffsetWing==1)
						jgzj=pDatumPart1.LineAnglePointer()->xWingYZhunJu;
					else
						getjgzj(jgzj,pDatumPart1.LineAnglePointer()->GetWidth());
					g[0]=jgzj.g;
					g1[0]=jgzj.g1;
					g2[0]=jgzj.g2;
					g3[0]=jgzj.g3;
				}
				if(pDatumPart2.IsHasPtr()&&pDatumPart2->GetClassTypeId()==CLS_LINEANGLE)
				{
					JGZJ jgzj;
					int offset_wing=0;
					if( fabs(wing_norm*pDatumPart2.LineAnglePointer()->get_norm_y_wing())>
						fabs(wing_norm*pDatumPart2.LineAnglePointer()->get_norm_x_wing()))
						offset_wing=1;
					if(pDatumPart2.LineAnglePointer()->m_bEnableTeG&&offset_wing==0)	//m_iOffsetWing==0)
						jgzj=pDatumPart2.LineAnglePointer()->xWingXZhunJu;
					else if(pDatumPart2.LineAnglePointer()->m_bEnableTeG&&offset_wing==1)//m_iOffsetWing==1)
						jgzj=pDatumPart2.LineAnglePointer()->xWingYZhunJu;
					else
						getjgzj(jgzj,pDatumPart2.LineAnglePointer()->GetWidth());
					g[1]=jgzj.g;
					g1[1]=jgzj.g1;
					g2[1]=jgzj.g2;
					g3[1]=jgzj.g3;
				}
			}
		}
		/*CDesignThickPara thick(m_pPoint->des_para.AXIS_INTERS.norm_offset);
		CString ss;
		ss.Format("%d",thick.Thick(m_pPoint->SuperiorTower()));
		GetDlgItem(IDC_BN_NORM_OFFSET)->SetWindowText(ss);*/
		m_fWingOffsetDist=m_pPoint->des_para.AXIS_INTERS.wing_offset_dist1;
		m_fWingOffsetDist2=m_pPoint->des_para.AXIS_INTERS.wing_offset_dist2;
		m_iWingOffsetStyle=m_pPoint->des_para.AXIS_INTERS.wing_offset_style1;
		m_iWingOffsetStyle2=m_pPoint->des_para.AXIS_INTERS.wing_offset_style2;
	}
	else 
#endif
		if(m_pPoint->datum_pos_style==5)	//基准线上轴向坐标不变点
	{
		m_hFirstHandle=m_pPoint->des_para.DATUMLINE.hDatumLine;
		m_iSpecAxis=m_pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2;
	}
	else if(m_pPoint->datum_pos_style==6)	//基准面上指定轴向点
	{
		m_hFirstHandle=m_pPoint->des_para.DATUMPLANE.hDatumPlane;
		m_iSpecAxis=m_pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2;
	}
	else if(m_pPoint->datum_pos_style==8)	//与节点同位
	{
		m_hFirstHandle=m_pPoint->des_para.hNode;
	}
	else if(m_pPoint->datum_pos_style==9)	//基准构件上的相对坐标
		m_hDatumPart=m_pPoint->des_para.hPart;
	UpdateData(FALSE);
	OnSelchangeCmbWingOffsetStyle();
	OnSelchangeCmbWingOffsetStyle2();
	if(m_iDatumPosCalStyle==0||m_iDatumPosCalStyle==7)
	{	//手动输入点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sFirstHandleName="Datum Angle Handle:";
		m_sSecondHandleName="Datum Node Handle:";
#else
		m_sFirstHandleName="基准角钢句柄:";
		m_sSecondHandleName="基准节点句柄:";
#endif
	}
	else if(m_iDatumPosCalStyle==1)
	{	//角钢楞点定位点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sFirstHandleName="Datum Angle Handle:";
		m_sSecondHandleName="Datum Node Handle:";
#else
		m_sFirstHandleName="基准角钢句柄:";
		m_sSecondHandleName="基准节点句柄:";
#endif
	}
	else if(m_iDatumPosCalStyle==2)
	{	//角钢节点定位点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sFirstHandleName="Datum Angle Handle:";
		m_sSecondHandleName="Datum Node Handle:";
#else
		m_sFirstHandleName="基准角钢句柄:";
		m_sSecondHandleName="基准节点句柄:";
#endif
	}
	else if(m_iDatumPosCalStyle==3)
	{	//角钢心线交点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sFirstHandleName="Datum Angle 1 Handle:";
		m_sSecondHandleName="Datum Angle 2 Handle:";
#else
		m_sFirstHandleName="基准角钢1句柄:";
		m_sSecondHandleName="基准角钢2句柄:";
#endif
	}
	else if(m_iDatumPosCalStyle==4)
	{	//基准线空间交点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sFirstHandleName="Datum Line 1 Handle:";
		m_sSecondHandleName="Datum Line 2 Handle:";
#else
		m_sFirstHandleName="基准线1句柄:";
		m_sSecondHandleName="基准线2句柄:";
#endif
	}
	else if(m_iDatumPosCalStyle==5)
	{	//基准线轴向坐标不变点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sFirstHandleName="Datum Line 1 Handle:";
		m_sSecondHandleName="Datum Node Handle:";
#else
		m_sFirstHandleName="基准线1句柄:";
		m_sSecondHandleName="基准节点句柄:";
#endif
	}
	else if(m_iDatumPosCalStyle==6)
	{	//基准面上指定轴向点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sFirstHandleName="Datum Plane Handle:";
		m_sSecondHandleName="Datum Node Handle:";
#else
		m_sFirstHandleName="基准面句柄:";
		m_sSecondHandleName="基准节点句柄:";
#endif
	}
	else if(m_pPoint->datum_pos_style==8)	//与节点同位
	{
		m_shSecondHandle.Format("0X%X",m_pPoint->des_para.hNode);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sSecondHandleName="Datum Node Handle:";
#else
		m_sSecondHandleName="基准节点句柄:";
#endif
	}
	else if(m_iDatumPosCalStyle==9)	//基准构件上的相对坐标
	{
		m_shSecondHandle.Format("0X%X",m_hDatumPart);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sSecondHandleName="Datum Part Handle:";
#else
		m_sSecondHandleName="基准构件句柄:";
#endif
	}
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDatumPointDlg::OnSelchangeCmbDatumPointCalStyle() 
{
	UpdateUIStatus();
	GetDlgItem(IDC_CHK_LEN_OFFSET_INC_ODD)->EnableWindow(FALSE);
	if(m_iDatumPosCalStyle==0)
	{	//手动输入点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sFirstHandleName="Datum Angle Handle:";
		m_sSecondHandleName="Datum Node Handle:";
#else
		m_sFirstHandleName="基准角钢句柄:";
		m_sSecondHandleName="基准节点句柄:";
#endif
	}
	else if(m_iDatumPosCalStyle==1||m_iDatumPosCalStyle==7)
	{	//角钢楞点定位点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sFirstHandleName="Datum Angle Handle:";
		m_sSecondHandleName="Datum Node Handle:";
#else
		m_sFirstHandleName="基准角钢句柄:";
		m_sSecondHandleName="基准节点句柄:";
#endif
		if(m_iDatumPosCalStyle==1)
			GetDlgItem(IDC_CHK_LEN_OFFSET_INC_ODD)->EnableWindow(TRUE);
		else
		{
			m_bIncOddEffect=TRUE;
			GetDlgItem(IDC_CHK_LEN_OFFSET_INC_ODD)->EnableWindow(FALSE);
		}
	}
	else if(m_iDatumPosCalStyle==2)
	{	//杆件节点定位点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sFirstHandleName="Datum Rod  Handle:";
		m_sSecondHandleName="Datum Node Handle:";
#else
		m_sFirstHandleName="基准杆件句柄:";
		m_sSecondHandleName="基准节点句柄:";
#endif
	}
	else if(m_iDatumPosCalStyle==3||m_iDatumPosCalStyle==4)
	{	//基准线空间交点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
		if(m_iDatumPosCalStyle==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			m_sFirstHandleName="Datum Angle 1 Handle:";
			m_sSecondHandleName="Datum Angle 2 Handle:";
#else
			m_sFirstHandleName="基准角钢1句柄:";
			m_sSecondHandleName="基准角钢2句柄:";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			m_sFirstHandleName="Datum Line 1 Handle:";
			m_sSecondHandleName="Datum Line 2 Handle:";
#else
			m_sFirstHandleName="基准线1句柄:";
			m_sSecondHandleName="基准线2句柄:";
#endif
		}
	}
	else if(m_iDatumPosCalStyle==5)
	{	//基准线轴向坐标不变点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sFirstHandleName="Datum Line 1 Handle:";
		m_sSecondHandleName="Datum Node Handle:";
#else
		m_sFirstHandleName="基准线1句柄:";
		m_sSecondHandleName="基准节点句柄:";
#endif
	}
	else if(m_iDatumPosCalStyle==6)
	{	//基准面上指定轴向点
		m_shFirstHandle.Format("0X%X",m_hFirstHandle);
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sFirstHandleName="Datum Plane Handle:";
		m_sSecondHandleName="Datum Node Handle:";
#else
		m_sFirstHandleName="基准面句柄:";
		m_sSecondHandleName="基准节点句柄:";
#endif
	}
	else if(m_iDatumPosCalStyle==8)
	{	//与节点同位
		m_shSecondHandle.Format("0X%X",m_hSecondHandle);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sSecondHandleName="Datum Node Handle:";
#else
		m_sSecondHandleName="基准节点句柄:";
#endif
	}
	else if(m_iDatumPosCalStyle==9)
	{	//基准构件上的相对坐标
		m_shSecondHandle.Format("0X%X",m_hDatumPart);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sSecondHandleName="Datum Part Handle:";
#else
		m_sSecondHandleName="基准构件句柄:";
#endif
	}
	//m_pPoint->datum_pos_style=m_iDatumPosCalStyle;
	UpdateData(FALSE);
}

void CDatumPointDlg::OnOK() 
{
	UpdateData();
	sprintf(m_pPoint->description,"%s",m_sPointDescription);
	sscanf(m_shFirstHandle,"%X",&m_hFirstHandle);
	sscanf(m_shSecondHandle,"%X",&m_hSecondHandle);
	sscanf(m_shSecondHandle,"%X",&m_hDatumPart);
	if(m_iDatumPosCalStyle==1&&console.FromPartHandle(m_hFirstHandle,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
	{	//角钢楞点定位点
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The handle of specified angle is illegal!");
#else
		AfxMessageBox("指定角钢句柄非法!");
#endif
		return;
	}
	else if(m_iDatumPosCalStyle==2&&(console.FromPartHandle(m_hFirstHandle,CLS_LINEPART)==NULL
								   ||console.FromNodeHandle(m_hSecondHandle)==NULL)	)
	{	//杆件节点定位点
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The specified datum line's handle or node's handle is illegal!");
#else
		AfxMessageBox("指定杆件或节点句柄非法!");
#endif
		return;
	}
	else if(m_iDatumPosCalStyle==3&&(console.FromPartHandle(m_hFirstHandle,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL
								   ||console.FromPartHandle(m_hSecondHandle,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)	)
	{	//角钢心线交点
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The specified rod's handle is illegal!");
#else
		AfxMessageBox("指定杆件句柄非法!");
#endif
		return;
	}
	else if(m_iDatumPosCalStyle==4&&(console.FromLineHandle(m_hFirstHandle)==NULL
								   ||console.FromLineHandle(m_hSecondHandle)==NULL)	)
	{	//基准线空间交点
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The specified datum line's handle is illegal!");
#else
		AfxMessageBox("指定基准线句柄非法!");
#endif
		return;
	}
	else if(m_iDatumPosCalStyle==5&&console.FromLineHandle(m_hFirstHandle)==NULL	)
	{	//基准线轴向坐标不变点
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The specified datum line's handle is illegal!");
#else
		AfxMessageBox("指定基准线句柄非法!");
#endif
		return;
	}
	else if(m_iDatumPosCalStyle==6&&console.FromPlaneHandle(m_hFirstHandle)==NULL	)
	{	//基准面上指定轴向点
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The specified datum plane's handle is illegal!");
#else
		AfxMessageBox("指定基准面句柄非法!");
#endif
		return;
	}
	else if(m_iDatumPosCalStyle==7&&console.FromPartHandle(m_hFirstHandle,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
	{	//7.沿角钢两肢法向偏移的包铁楞点
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The handle of specified angle is illegal!");
#else
		AfxMessageBox("指定角钢句柄非法!");
#endif
		return;
	}
	else if(m_iDatumPosCalStyle==8&&console.FromNodeHandle(m_hSecondHandle)==NULL)
	{	//8.与节点同位
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The specified datum node's handle is illegal!");
#else
		AfxMessageBox("指定基准节点句柄非法!");
#endif
		return;
	}
	else if(m_iDatumPosCalStyle==9&&console.FromPartHandle(m_hDatumPart)==NULL)
	{	//9.基准构件上的相对坐标
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The specified datum part's handle is illegal!");
#else
		AfxMessageBox("指定基准构件句柄非法!");
#endif
		return;
	}
	m_pPoint->SetPosition(f3dPoint(m_fPosX,m_fPosY,m_fPosZ));
	m_pPoint->datum_pos.Set(m_fPosXInPart,m_fPosYInPart,m_fPosZInPart);
	m_pPoint->datum_pos_style=m_iDatumPosCalStyle;
	if(m_pPoint->datum_pos_style==0)		//手动输入点坐标
		;
	else if(m_pPoint->datum_pos_style==1||m_pPoint->datum_pos_style==7)	//角钢楞点定位点
	{
		m_pPoint->des_para.RODEND.hRod=m_hFirstHandle;
		m_pPoint->des_para.RODEND.direction=m_iExtendDirection;
		m_pPoint->des_para.RODEND.offset_wing=m_iOffsetWing;
		m_pPoint->des_para.RODEND.len_offset_dist=m_fExtendLen;
		m_pPoint->des_para.RODEND.bIncOddEffect=m_bIncOddEffect;
		m_pPoint->des_para.RODEND.wing_offset_style=m_iWingOffsetStyle;
		m_pPoint->des_para.RODEND.wing_offset_dist=m_fWingOffsetDist;
	}
	else if(m_pPoint->datum_pos_style==2)	//角钢节点定位点
	{
		m_pPoint->des_para.RODNODE.hRod =m_hFirstHandle;
		m_pPoint->des_para.RODNODE.hNode=m_hSecondHandle;
		m_pPoint->des_para.RODNODE.direction=m_iExtendDirection;
		m_pPoint->des_para.RODNODE.offset_wing=m_iOffsetWing;
		m_pPoint->des_para.RODNODE.len_offset_dist=m_fExtendLen;
		m_pPoint->des_para.RODNODE.wing_offset_style=m_iWingOffsetStyle;
		m_pPoint->des_para.RODNODE.wing_offset_dist=m_fWingOffsetDist;
		m_pPoint->des_para.RODNODE.bFaceOffset=m_bFaceOffset;
	}
	else if(m_pPoint->datum_pos_style==3||m_pPoint->datum_pos_style==4)
	{	//角钢心线交点或基准线空间交点
		m_pPoint->des_para.AXIS_INTERS.hDatum1 =m_hFirstHandle;
		m_pPoint->des_para.AXIS_INTERS.hDatum2 =m_hSecondHandle;
		m_pPoint->des_para.AXIS_INTERS.wing_offset_dist1=ftoi(m_fWingOffsetDist);
		m_pPoint->des_para.AXIS_INTERS.wing_offset_dist2=ftoi(m_fWingOffsetDist2);
		m_pPoint->des_para.AXIS_INTERS.wing_offset_style1=m_iWingOffsetStyle;
		m_pPoint->des_para.AXIS_INTERS.wing_offset_style2=m_iWingOffsetStyle2;
	}
	else if(m_pPoint->datum_pos_style==5)	//基准线上轴向坐标不变点
	{
		m_pPoint->des_para.DATUMLINE.hDatumLine=m_hFirstHandle;
		m_pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=m_iSpecAxis;
	}
	else if(m_pPoint->datum_pos_style==6)	//基准面上指定轴向点
	{
		m_pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=m_iSpecAxis;
		m_pPoint->des_para.DATUMPLANE.hDatumPlane=m_hFirstHandle;
	}
	else if(m_pPoint->datum_pos_style==8)	//与节点同位
		m_pPoint->des_para.hNode=m_hSecondHandle;
	else if(m_pPoint->datum_pos_style==9)	//基准构件上的相对坐标
		m_pPoint->des_para.hPart=m_hDatumPart;
	if(m_pPoint->UpdatePos(console.GetActiveModel()))
		CDialog::OnOK();
}

void CDatumPointDlg::OnSelchangeCmbWingOffsetStyle() 
{
	UpdateData(TRUE);
	if(m_iWingOffsetStyle==0)
		m_fWingOffsetDist=g[0];
	else if(m_iWingOffsetStyle==1)
		m_fWingOffsetDist=g1[0];
	else if(m_iWingOffsetStyle==2)
		m_fWingOffsetDist=g2[0];
	else if(m_iWingOffsetStyle==3)
		m_fWingOffsetDist=g3[0];
	UpdateData(FALSE);
	UpdateUIStatus();
}

void CDatumPointDlg::OnSelchangeCmbWingOffsetStyle2() 
{
	UpdateData(TRUE);
	if(m_iWingOffsetStyle2==0)
		m_fWingOffsetDist2=g[1];
	else if(m_iWingOffsetStyle2==1)
		m_fWingOffsetDist2=g1[1];
	else if(m_iWingOffsetStyle2==2)
		m_fWingOffsetDist2=g2[1];
	else if(m_iWingOffsetStyle2==3)
		m_fWingOffsetDist2=g3[1];
	UpdateData(FALSE);
	UpdateUIStatus();
}

void CDatumPointDlg::OnSelchangeCmbSpecAxis() 
{
	UpdateUIStatus();
}

void CDatumPointDlg::UpdateUIStatus()
{
	UpdateData();
	((CEdit*)GetDlgItem(IDC_E_POS_X_IN_PART))->EnableWindow(m_iDatumPosCalStyle==9);
	((CEdit*)GetDlgItem(IDC_E_POS_Y_IN_PART))->EnableWindow(m_iDatumPosCalStyle==9);
	((CEdit*)GetDlgItem(IDC_E_POS_Z_IN_PART))->EnableWindow(m_iDatumPosCalStyle==9);
	//禁用粘贴相对坐标按钮
	GetDlgItem(IDC_BN_PASTE_POS_IN_PART)->EnableWindow(m_iDatumPosCalStyle==9);
	if( m_iDatumPosCalStyle==0||(m_iDatumPosCalStyle==5&&m_iSpecAxis==0) ||
		(m_iDatumPosCalStyle==6&&m_iSpecAxis!=0))
		((CEdit*)GetDlgItem(IDC_E_POS_X))->SetReadOnly(FALSE);
	else
		((CEdit*)GetDlgItem(IDC_E_POS_X))->SetReadOnly(TRUE);
	if( m_iDatumPosCalStyle==0||(m_iDatumPosCalStyle==5&&m_iSpecAxis==1) ||
		(m_iDatumPosCalStyle==6&&m_iSpecAxis!=1))
		((CEdit*)GetDlgItem(IDC_E_POS_Y))->SetReadOnly(FALSE);
	else
		((CEdit*)GetDlgItem(IDC_E_POS_Y))->SetReadOnly(TRUE);
	if( m_iDatumPosCalStyle==0||(m_iDatumPosCalStyle==5&&m_iSpecAxis==2) ||
		(m_iDatumPosCalStyle==6&&m_iSpecAxis!=2))
		((CEdit*)GetDlgItem(IDC_E_POS_Z))->SetReadOnly(FALSE);
	else
		((CEdit*)GetDlgItem(IDC_E_POS_Z))->SetReadOnly(TRUE);

	if( m_iDatumPosCalStyle==0) //手动输入
		GetDlgItem(IDC_BN_PASTE_VEC)->EnableWindow(TRUE);
	else 
		GetDlgItem(IDC_BN_PASTE_VEC)->EnableWindow(FALSE);//禁用粘贴按钮

	((CEdit*)GetDlgItem(IDC_E_FIRST_HANDLE))->SetReadOnly(m_iDatumPosCalStyle==0||m_iDatumPosCalStyle==8||m_iDatumPosCalStyle==9);
	if(m_iDatumPosCalStyle!=2)
	{	
		GetDlgItem(IDC_CHK_FACE_OFFSET)->ShowWindow(SW_HIDE);
		m_bFaceOffset=FALSE;	//无平推时 不需要显示定义平推面按钮
	}
	else
		GetDlgItem(IDC_CHK_FACE_OFFSET)->ShowWindow(SW_SHOW);
	CWnd *pWnd=GetDlgItem(IDC_CMB_OFFSET_FACE_TYPE);
	pWnd->EnableWindow(m_bFaceOffset);
	if(m_bFaceOffset)	//是否显示定义平推面按钮
	{
		pWnd->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_S_OFFSET_FACE)->ShowWindow(SW_SHOW);
	}
	else
	{
		pWnd->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_OFFSET_FACE)->ShowWindow(SW_HIDE);
	}
	((CEdit*)GetDlgItem(IDC_E_SECOND_HANDLE))->SetReadOnly(m_iDatumPosCalStyle<2||(m_iDatumPosCalStyle>4&&m_iDatumPosCalStyle<8));
	((CEdit*)GetDlgItem(IDC_E_EXTEND_LEN))->SetReadOnly(m_iDatumPosCalStyle!=1&&m_iDatumPosCalStyle!=2&&m_iDatumPosCalStyle!=7);
	GetDlgItem(IDC_CMB_WING_OFFSET_STYLE)->EnableWindow(m_iDatumPosCalStyle==1||m_iDatumPosCalStyle==2||m_iDatumPosCalStyle==3);
	((CEdit*)GetDlgItem(IDC_E_WING_OFFSET_DIST))->SetReadOnly(
		(m_iDatumPosCalStyle!=1&&m_iDatumPosCalStyle!=2&&m_iDatumPosCalStyle!=3)||m_iWingOffsetStyle!=4);
	GetDlgItem(IDC_CMB_WING_OFFSET_STYLE2)->EnableWindow(m_iDatumPosCalStyle==3);
	((CEdit*)GetDlgItem(IDC_E_WING_OFFSET_DIST2))->SetReadOnly(m_iDatumPosCalStyle!=3||m_iWingOffsetStyle2!=4);
	GetDlgItem(IDC_BN_NORM_OFFSET)->EnableWindow(m_iDatumPosCalStyle==1||m_iDatumPosCalStyle==2||m_iDatumPosCalStyle==7);
	GetDlgItem(IDC_RDO_EXTEND_DIRECTION)->EnableWindow(m_iDatumPosCalStyle==1||m_iDatumPosCalStyle==2||m_iDatumPosCalStyle==7);
	GetDlgItem(IDC_RADIO3)->EnableWindow(m_iDatumPosCalStyle==1||m_iDatumPosCalStyle==2||m_iDatumPosCalStyle==7);
	GetDlgItem(IDC_RDO_OFFSET_VEC)->EnableWindow(m_iDatumPosCalStyle==1||m_iDatumPosCalStyle==2);
	GetDlgItem(IDC_RADIO11)->EnableWindow(m_iDatumPosCalStyle==1||m_iDatumPosCalStyle==2);
	GetDlgItem(IDC_CMB_SPEC_AXIS)->EnableWindow(m_iDatumPosCalStyle==5||m_iDatumPosCalStyle==6);
}

void CDatumPointDlg::OnBnNormOffset() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CString sThickStr;
	if(m_iDatumPosCalStyle==1||m_iDatumPosCalStyle==7)
		sThickStr=m_pPoint->des_para.RODEND.norm_offset;
	else if(m_iDatumPosCalStyle==2)
		sThickStr=m_pPoint->des_para.RODNODE.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
	Input("Input the offset string along normal",&sThickStr,'s');
#else
	Input("输入法向偏移量字符串",&sThickStr,'s');
#endif
	if(m_iDatumPosCalStyle==1||m_iDatumPosCalStyle==7)
	{
		_snprintf(m_pPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
		CDesignThickPara thick(m_pPoint->des_para.RODEND.norm_offset);
		sThickStr.Format("%d",thick.Thick(m_pPoint->BelongModel()));
	}
	else if(m_iDatumPosCalStyle==2)
	{
		_snprintf(m_pPoint->des_para.RODNODE.norm_offset,49,"%s",sThickStr);
		CDesignThickPara thick(m_pPoint->des_para.RODNODE.norm_offset);
		sThickStr.Format("%d",thick.Thick(m_pPoint->BelongModel()));
	}
	GetDlgItem(IDC_BN_NORM_OFFSET)->SetWindowText(sThickStr);
#endif
}

void CDatumPointDlg::OnChkFaceOffset() 
{
	UpdateData();
	CWnd *pWnd=GetDlgItem(IDC_CMB_OFFSET_FACE_TYPE);
	pWnd->EnableWindow(m_bFaceOffset);
	if(m_bFaceOffset)
	{
		pWnd->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_S_OFFSET_FACE)->ShowWindow(SW_SHOW);
	}
	else
	{
		pWnd->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_OFFSET_FACE)->ShowWindow(SW_HIDE);
	}
}

void CDatumPointDlg::OnBnPasteVec() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fPosX = point.x;
		m_fPosY = point.y;
		m_fPosZ = point.z;
		UpdateData(FALSE);
	}
}

void CDatumPointDlg::OnBnPastePosInPart() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fPosXInPart = point.x;
		m_fPosYInPart = point.y;
		m_fPosZInPart = point.z;
		UpdateData(FALSE);
	}
}

void CDatumPointDlg::OnBnCopyPosInPart() 
{
	f3dPoint point;
	point.Set(m_fPosXInPart,m_fPosYInPart,m_fPosZInPart);
	WritePointToClip(point);
}

void CDatumPointDlg::OnBnCopyVec() 
{
	f3dPoint point;
	point.Set(m_fPosX,m_fPosY,m_fPosZ);
	WritePointToClip(point);
}

void CDatumPointDlg::OnSelchangeCmbOffsetFaceType() 
{
	UpdateData();
	if(m_iOffsetFaceType==3)
	{
		CDatumFaceOffsetDlg dlg;
		dlg.m_fNormX = m_pPoint->des_para.RODNODE.vPlaneNormal.x;
		dlg.m_fNormY = m_pPoint->des_para.RODNODE.vPlaneNormal.y;
		dlg.m_fNormZ = m_pPoint->des_para.RODNODE.vPlaneNormal.z;
		dlg.m_iBaseStyle = 0;
		if(dlg.DoModal()==IDOK)
		{
			m_pPoint->des_para.RODNODE.vPlaneNormal.x = dlg.m_fNormX;
			m_pPoint->des_para.RODNODE.vPlaneNormal.y = dlg.m_fNormY;
			m_pPoint->des_para.RODNODE.vPlaneNormal.z = dlg.m_fNormZ;
			//m_iBaseOffsetStyle = dlg.m_iBaseStyle;
		}	
	}
	else if(m_iOffsetFaceType==0)
		m_pPoint->des_para.RODNODE.vPlaneNormal.Set(1,0,0);
	else if(m_iOffsetFaceType==1)
		m_pPoint->des_para.RODNODE.vPlaneNormal.Set(0,1,0);
	else
		m_pPoint->des_para.RODNODE.vPlaneNormal.Set(0,0,1);
}
