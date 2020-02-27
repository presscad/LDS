// Sel2JgKeyPointDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "Sel2JgKeyPointDlg.h"
#include "LicFuncDef.h"
#include "env_def.h"
#include "OriginalData.h"
#include "LDSView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////
static bool ConvertToSizeCheckKeyPoint(CLDSPoint* pPoint,CDataCheckModel::KEYPOINT* pKeyPoint)
{
	if(pPoint->datum_pos_style==1)
	{
		pKeyPoint->ciPointType=0;
		pKeyPoint->angle.hAngle=pPoint->des_para.RODEND.hRod;
		SmartRodPtr pRod=Ta.FromRodHandle(pKeyPoint->angle.hAngle);
		if(pPoint->des_para.RODEND.direction==0&&pRod.IsHasPtr()&&pRod->pStart)
			pKeyPoint->angle.hNode=pRod->pStart->handle;
		else if(pPoint->des_para.RODEND.direction==1&&pRod.IsHasPtr()&&pRod->pEnd)
			pKeyPoint->angle.hNode=pRod->pEnd->handle;
		pKeyPoint->angle.offset.ciOffsetWing=(BYTE)pPoint->des_para.RODEND.offset_wing;
		pKeyPoint->angle.offset.wiLenOffset=(short)ftoi(pPoint->des_para.RODEND.len_offset_dist);
		pKeyPoint->angle.offset.ciStyleOfG=(BYTE)pPoint->des_para.RODEND.wing_offset_style;
		pKeyPoint->angle.offset.wiWingOffset=(short)ftoi(pPoint->des_para.RODEND.wing_offset_dist);
		pKeyPoint->angle.offset.ciOffsetPlane=0;
	}
	else if(pPoint->datum_pos_style==2)
	{
		pKeyPoint->ciPointType=0;
		pKeyPoint->angle.hAngle=pPoint->des_para.RODNODE.hRod;
		SmartRodPtr pRod=Ta.FromRodHandle(pKeyPoint->angle.hAngle);
		pKeyPoint->angle.hNode=pPoint->des_para.RODNODE.hNode;
		pKeyPoint->angle.offset.ciOffsetWing=(BYTE)pPoint->des_para.RODNODE.offset_wing;
		pKeyPoint->angle.offset.wiLenOffset=(short)ftoi(pPoint->des_para.RODNODE.len_offset_dist);
		pKeyPoint->angle.offset.ciStyleOfG=(BYTE)pPoint->des_para.RODNODE.wing_offset_style;
		pKeyPoint->angle.offset.wiWingOffset=(short)ftoi(pPoint->des_para.RODNODE.wing_offset_dist);
		GEPOINT vPlaneNormal=pPoint->des_para.RODNODE.vPlaneNormal;
		if(vPlaneNormal.IsZero()||pPoint->des_para.RODNODE.bFaceOffset)
			pKeyPoint->angle.offset.ciOffsetPlane=0;
		else if(fabs(vPlaneNormal.x+vPlaneNormal.y)<EPS)
			pKeyPoint->angle.offset.ciOffsetPlane=1;	//X-Y平面
		else if(fabs(vPlaneNormal.y+vPlaneNormal.z)<EPS)
			pKeyPoint->angle.offset.ciOffsetPlane=2;	//Y-Z平面
		else if(fabs(vPlaneNormal.x+vPlaneNormal.z)<EPS)
			pKeyPoint->angle.offset.ciOffsetPlane=3;	//X-Z平面
		else
			pKeyPoint->angle.offset.ciOffsetPlane=0;
	}
	else
		return false;
	return true;
}
/////////////////////////////////////////////////////////////////////////////
// CSel2JgKeyPointDlg dialog
CSel2JgKeyPointDlg::CSel2JgKeyPointDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CSel2JgKeyPointDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSel2JgKeyPointDlg)
	m_iFirstKeyPoint = 0;
	m_iFirstOffsetStyle = 0;
	m_iSecondKeyPoint = 0;
	m_iSecondOffsetStyle = 0;
	m_bChkDeitaL=TRUE;
	m_bChkDeitaX=FALSE;
	m_bChkDeitaY=FALSE;
	m_bChkDeitaZ=FALSE;
	m_fCheckDist=0;
	m_cbDimFlag=0;
	m_pFromAngle=NULL;
	m_pToAngle=NULL;
	m_pFromAngleNode=NULL;
	m_pToAngleNode=NULL;
	m_bAddCheckDim=false;
	//}}AFX_DATA_INIT
}
void CSel2JgKeyPointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSel2JgKeyPointDlg)
	DDX_CBIndex(pDX, IDC_CMB_FIRST_KEY_POINT, m_iFirstKeyPoint);
	DDX_CBIndex(pDX, IDC_CMB_FIRST_OFFSET_STYLE, m_iFirstOffsetStyle);
	DDX_CBIndex(pDX, IDC_CMB_SECOND_KEY_POINT, m_iSecondKeyPoint);
	DDX_CBIndex(pDX, IDC_CMB_SECOND_OFFSET_STYLE, m_iSecondOffsetStyle);
	DDX_Check(pDX, IDC_CHK_DEITA_L, m_bChkDeitaL);
	DDX_Check(pDX, IDC_CHK_DEITA_X, m_bChkDeitaX);
	DDX_Check(pDX, IDC_CHK_DEITA_Y, m_bChkDeitaY);
	DDX_Check(pDX, IDC_CHK_DEITA_Z, m_bChkDeitaZ);
	DDX_Check(pDX, IDC_CHK_MARK, m_bMarkCheck);
	DDX_Text(pDX, IDC_E_DIST, m_sDist);
	DDX_Text(pDX, IDC_E_CHECK_DIST, m_fCheckDist);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSel2JgKeyPointDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CSel2JgKeyPointDlg)
	ON_BN_CLICKED(IDC_CHK_DEITA_L, &CSel2JgKeyPointDlg::OnBnClickedChkDeitaL)
	ON_BN_CLICKED(IDC_CHK_DEITA_X, &CSel2JgKeyPointDlg::OnBnClickedChkDeitaX)
	ON_BN_CLICKED(IDC_CHK_DEITA_Y, &CSel2JgKeyPointDlg::OnBnClickedChkDeitaY)
	ON_BN_CLICKED(IDC_CHK_DEITA_Z, &CSel2JgKeyPointDlg::OnBnClickedChkDeitaZ)
	//}}AFX_MSG_MAP
	
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSel2JgKeyPointDlg message handlers
BOOL CSel2JgKeyPointDlg::OnInitDialog()
{
	CRememberLocationDlg::OnInitDialog();
	if(theApp.m_bManuRevisionMode)
	{	//启用校审模式
		GetDlgItem(IDC_CHK_DEITA_L)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_DEITA_X)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_DEITA_Y)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_DEITA_Z)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_S_DIST)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_DIST)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_S_CHECK_DIST)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_CHECK_DIST)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_MARK)->ShowWindow(SW_SHOW);
	}
	else
	{	//关闭校审模式
		GetDlgItem(IDC_CHK_DEITA_L)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_DEITA_X)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_DEITA_Y)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_DEITA_Z)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_DIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_DIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_CHECK_DIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_CHECK_DIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_MARK)->ShowWindow(SW_HIDE);
	}
	//对话框默认为展开模式，非校审模式时，需调整对话框大小
	CWnd* pWorkPanel=GetDlgItem(IDC_WORK_PANEL);
	pWorkPanel->ShowWindow(SW_HIDE);
	if(!theApp.m_bManuRevisionMode)
	{
		CRect panel_rc,full_rc;
		pWorkPanel->GetWindowRect(panel_rc);
		int nOffset=-panel_rc.Height();
		GetWindowRect(full_rc);
		full_rc.bottom+=nOffset;
		MoveWindow(full_rc);
		//调整按钮位置
		CWnd *pBtn=NULL;
		for(int i=0;i<2;i++)
		{
			if(i==0)
				pBtn=GetDlgItem(IDOK);
			else
				pBtn=GetDlgItem(IDCANCEL);
			CRect rcBtn;
			pBtn->GetWindowRect(rcBtn);
			ScreenToClient(rcBtn);
			rcBtn.top+=nOffset;
			rcBtn.bottom+=nOffset;
			pBtn->MoveWindow(rcBtn);
		}
	}
	//校审模式下，实时计算关键点距离
	if(theApp.m_bManuRevisionMode)
	{
		Update2JgKeyPointPos();
		Cal2KeyPointDistance();	
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CSel2JgKeyPointDlg::Update2JgKeyPointPos()
{
	m_xStartPoint.datum_pos_style=2;	//杆件节点
	m_xStartPoint.des_para.RODNODE.wing_offset_style=0;	//标准准距
	m_xStartPoint.des_para.RODNODE.vPlaneNormal.Set(0,0,0);
	m_xStartPoint.des_para.RODNODE.bFaceOffset=TRUE;
	m_xEndPoint=m_xStartPoint;
	//起始点
	m_xStartPoint.des_para.RODNODE.hRod=m_pFromAngle->handle;
	m_xStartPoint.des_para.RODNODE.hNode=m_pFromAngleNode->handle;
	m_xStartPoint.des_para.RODNODE.offset_wing=max(0,m_iFirstKeyPoint-1);
	if(m_iFirstKeyPoint==0)
	{	//楞点
		m_xStartPoint.des_para.RODNODE.wing_offset_style=4;
		m_xStartPoint.des_para.RODNODE.wing_offset_dist=0;
	}
	else 
		m_xStartPoint.des_para.RODNODE.wing_offset_style=0;
	if(m_iFirstOffsetStyle==1)	//X-Y平面平推
		m_xStartPoint.des_para.RODNODE.vPlaneNormal.Set(0,0,1);
	else if(m_iFirstOffsetStyle==2)	//Y-Z平面平推
		m_xStartPoint.des_para.RODNODE.vPlaneNormal.Set(1,0,0);
	else if(m_iFirstOffsetStyle==3)	//X-Z平面平推
		m_xStartPoint.des_para.RODNODE.vPlaneNormal.Set(0,1,0);
	else
		m_xStartPoint.des_para.RODNODE.bFaceOffset=FALSE;
	//终止点
	m_xEndPoint.des_para.RODNODE.hRod=m_pToAngle->handle;
	m_xEndPoint.des_para.RODNODE.hNode=m_pToAngleNode->handle;
	m_xEndPoint.des_para.RODNODE.offset_wing=max(0,m_iSecondKeyPoint-1);
	if(m_iSecondKeyPoint==0)
	{	//楞点
		m_xEndPoint.des_para.RODNODE.wing_offset_style=4;
		m_xEndPoint.des_para.RODNODE.wing_offset_dist=0;
	}
	else 
		m_xEndPoint.des_para.RODNODE.wing_offset_style=0;
	if(m_iSecondOffsetStyle==1)	//X-Y平面平推
		m_xEndPoint.des_para.RODNODE.vPlaneNormal.Set(0,0,1);
	else if(m_iSecondOffsetStyle==2)	//Y-Z平面平推
		m_xEndPoint.des_para.RODNODE.vPlaneNormal.Set(1,0,0);
	else if(m_iSecondOffsetStyle==3)	//X-Z平面平推
		m_xEndPoint.des_para.RODNODE.vPlaneNormal.Set(0,1,0);
	else
		m_xEndPoint.des_para.RODNODE.bFaceOffset=FALSE;
	//
	m_xStartPoint.UpdatePos(&Ta);
	m_xEndPoint.UpdatePos(&Ta);
}
void CSel2JgKeyPointDlg::Cal2KeyPointDistance()
{
	GEPOINT startPt=m_xStartPoint.Position();
	GEPOINT endPt=m_xEndPoint.Position();
	double fDist=0;
	if(m_bChkDeitaL)
		fDist=DISTANCE(startPt,endPt);
	else if(m_bChkDeitaX)
		fDist=fabs(startPt.x-endPt.x);
	else if(m_bChkDeitaY)
		fDist=fabs(startPt.y-endPt.y);
	else if(m_bChkDeitaZ)
		fDist=fabs(startPt.z-endPt.z);
	m_sDist.Format("%.2f",fDist);
	m_fCheckDist=ftoi(fDist);
	GetDlgItem(IDC_E_CHECK_DIST)->SetWindowTextA(m_sDist);
}
//
void CSel2JgKeyPointDlg::OnBnClickedChkDeitaL()
{
	UpdateData();
	m_cbDimFlag=0;
	m_bChkDeitaX=FALSE;
	m_bChkDeitaY=FALSE;
	m_bChkDeitaZ=FALSE;
	Cal2KeyPointDistance();	
	UpdateData(FALSE);
}
void CSel2JgKeyPointDlg::OnBnClickedChkDeitaX()
{
	UpdateData();
	m_cbDimFlag=0x01;
	m_bChkDeitaL=FALSE;
	m_bChkDeitaY=FALSE;
	m_bChkDeitaZ=FALSE;
	Cal2KeyPointDistance();
	UpdateData(FALSE);
}
void CSel2JgKeyPointDlg::OnBnClickedChkDeitaY()
{
	UpdateData();
	m_cbDimFlag=0x02;
	m_bChkDeitaL=FALSE;
	m_bChkDeitaX=FALSE;
	m_bChkDeitaZ=FALSE;
	Cal2KeyPointDistance();
	UpdateData(FALSE);
}
void CSel2JgKeyPointDlg::OnBnClickedChkDeitaZ()
{
	UpdateData();
	m_cbDimFlag=0x04;
	m_bChkDeitaL=FALSE;
	m_bChkDeitaX=FALSE;
	m_bChkDeitaY=FALSE;
	Cal2KeyPointDistance();
	UpdateData(FALSE);
}
void CSel2JgKeyPointDlg::OnOK()
{
	UpdateData();
	Update2JgKeyPointPos();
	if(theApp.m_bManuRevisionMode&&PRODUCT_FUNC::IsHasInternalTest())
	{
		CDataCheckModel::KEYPOINT keyPointS,keyPointE;
		if(ConvertToSizeCheckKeyPoint(&m_xStartPoint,&keyPointS)&&ConvertToSizeCheckKeyPoint(&m_xEndPoint,&keyPointE))
		{
			CDataCheckModel::SIZE_DATA* pSizeCoord=xDataCheckModel.AppendCoordSizeItem();
			pSizeCoord->ciSizeType=1;
			pSizeCoord->cbSizeDimFlag=m_cbDimFlag;
			pSizeCoord->m_bCheck=m_bMarkCheck;
			pSizeCoord->from=keyPointS;
			pSizeCoord->to=keyPointE;
			pSizeCoord->posFrom=GetStartPt();
			pSizeCoord->posTo=GetEndPt();
			if(m_bChkDeitaL)
				pSizeCoord->dist.coords.distance=(float)m_fCheckDist;//(float)DISTANCE(pSizeCoord->posFrom,pSizeCoord->posTo);
			if(m_bChkDeitaX)
				pSizeCoord->dist.coords.distInX=(float)fabs(pSizeCoord->posFrom.x-pSizeCoord->posTo.x);
			CLDSView::AppendSizeCoordCheckItem(pSizeCoord);
			m_bAddCheckDim=true;
		}
	}
	return CRememberLocationDlg::OnOK();
}
void CSel2JgKeyPointDlg::OnCancel()
{
	return CRememberLocationDlg::OnCancel();
}
