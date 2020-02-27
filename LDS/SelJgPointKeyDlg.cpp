//<LOCALE_TRANSLATE Confirm by hxr/>
// SelJgPointKeyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "lds_part.h"
#include "LicFuncDef.h"
#include "SelJgPointKeyDlg.h"
#include "GlobalFunc.h"
#include "LmaDlg.h"
#include "LDSView.h"
#include "OriginalData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CSelJgPointKeyDlg dialog

CSelJgPointKeyDlg::CSelJgPointKeyDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CSelJgPointKeyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelJgPointKeyDlg)
	m_iKeyPoint = 0;
	m_iOffsetStyle = 0;
	m_fPosX = 0.0;
	m_fPosY = 0.0;
	m_fPosZ = 0.0;
	m_fLenOffset = 0.0;
	m_fSpecGVal = 0.0;
	//}}AFX_DATA_INIT
	m_pJg=NULL;
	m_pNode=NULL;
	m_xInterPoint.SetBelongModel(&Ta);
	m_fChkCoordX = 0.0;
	m_fChkCoordY = 0.0;
	m_fChkCoordZ = 0.0;
	m_bChkCoordX=m_bChkCoordY=m_bChkCoordZ=FALSE;
}


void CSelJgPointKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelJgPointKeyDlg)
	DDX_CBIndex(pDX, IDC_CMB_KEY_POINT, m_iKeyPoint);
	DDX_CBIndex(pDX, IDC_CMB_OFFSET_STYLE, m_iOffsetStyle);
	DDX_Text(pDX, IDC_E_POS_X, m_fPosX);
	DDX_Text(pDX, IDC_E_POS_Y, m_fPosY);
	DDX_Text(pDX, IDC_E_POS_Z, m_fPosZ);
	DDX_Text(pDX, IDC_E_LEN_OFFSET, m_fLenOffset);
	DDX_Text(pDX, IDC_E_SPEC_G_VALUE, m_fSpecGVal);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHK_COORD_X, m_bChkCoordX);
	DDX_Check(pDX, IDC_CHK_COORD_Y, m_bChkCoordY);
	DDX_Check(pDX, IDC_CHK_COORD_Z, m_bChkCoordZ);
	DDX_Text(pDX, IDC_E_CHK_COORD_X, m_fChkCoordX);
	DDX_Text(pDX, IDC_E_CHK_COORD_Y, m_fChkCoordY);
	DDX_Text(pDX, IDC_E_CHK_COORD_Z, m_fChkCoordZ);
}


BEGIN_MESSAGE_MAP(CSelJgPointKeyDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CSelJgPointKeyDlg)
	ON_CBN_SELCHANGE(IDC_CMB_KEY_POINT, OnSelchangeCmbKeyPoint)
	ON_CBN_SELCHANGE(IDC_CMB_OFFSET_STYLE, OnSelchangeCmbOffsetStyle)
	ON_BN_CLICKED(ID_BN_COPY_PNT, OnBnCopyPnt)
	ON_EN_CHANGE(IDC_E_LEN_OFFSET, OnChangeELenOffset)
	ON_EN_CHANGE(IDC_E_SPEC_G_VALUE, OnChangeESpecGValue)
	ON_BN_CLICKED(IDC_CHK_COORD_X, &CSelJgPointKeyDlg::OnBnChkCoordX)
	ON_BN_CLICKED(IDC_CHK_COORD_Y, &CSelJgPointKeyDlg::OnBnChkCoordY)
	ON_BN_CLICKED(IDC_CHK_COORD_Z, &CSelJgPointKeyDlg::OnBnChkCoordZ)
	ON_EN_CHANGE(IDC_E_CHK_COORD_Y, &CSelJgPointKeyDlg::OnEnChangeEChkCoordY)
	ON_EN_CHANGE(IDC_E_CHK_COORD_Z, &CSelJgPointKeyDlg::OnEnChangeEChkCoordZ)
	ON_EN_CHANGE(IDC_E_CHK_COORD_X, &CSelJgPointKeyDlg::OnEnChangeEChkCoordX)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelJgPointKeyDlg message handlers
BOOL CSelJgPointKeyDlg::Create()
{
	return CDialog::Create(CSelJgPointKeyDlg::IDD);
}

bool CSelJgPointKeyDlg::UpdateDefaultAngleKeyPoint()
{
	if(m_pJg==NULL||m_pNode==NULL)
		return false;
	f3dPoint pos;
	if(m_iKeyPoint==1)
		m_fSpecGVal = m_pJg->GetNodeWingXG(m_pNode);
	else if(m_iKeyPoint==2)
		m_fSpecGVal = m_pJg->GetNodeWingYG(m_pNode);
	else
		m_fSpecGVal = 0;
	if(m_iOffsetStyle==0)	//投影
	{
		if(m_iKeyPoint==0)
			pos=m_pJg->GetDatumPosBer(m_pNode);
		else if(m_iKeyPoint==1)
			pos=m_pJg->GetDatumPosWingX(m_pNode,m_fSpecGVal);
		else
			pos=m_pJg->GetDatumPosWingY(m_pNode,m_fSpecGVal);
	}
	else
	{
		if(m_iOffsetStyle==1)
			m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(0,0,1);
		else if(m_iOffsetStyle==2)
			m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(1,0,0);
		else if(m_iOffsetStyle==3)
			m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(0,1,0);
		if(m_iKeyPoint==0)
			pos=m_pJg->GetDatumPosBerByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal);
		else if(m_iKeyPoint==1)
			pos=m_pJg->GetDatumPosWingXByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal,NULL,m_fSpecGVal);
		else
			pos=m_pJg->GetDatumPosWingYByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal,NULL,m_fSpecGVal);
	}
	f3dPoint vec;
	if(m_pJg->pEnd==m_pNode)
		vec=m_pJg->Start()-m_pJg->End();
	else
		vec=m_pJg->End()-m_pJg->Start();
	normalize(vec);
	pos+=vec*m_fLenOffset;
	m_fPosX=pos.x;
	m_fPosY=pos.y;
	m_fPosZ=pos.z;
	m_xInterPoint.datum_pos_style=2;	//杆件(角钢)上节点为基准
	m_xInterPoint.des_para.RODNODE.hRod=m_pJg->handle;
	m_xInterPoint.des_para.RODNODE.hNode=m_pNode->handle;
	if(m_iKeyPoint<=1)
	{
		m_xInterPoint.des_para.RODNODE.offset_wing=0;
		m_xInterPoint.des_para.RODNODE.wing_offset_dist=m_fSpecGVal;
		if(m_iKeyPoint==0)	//楞点
		{
			m_xInterPoint.des_para.RODNODE.wing_offset_style=4;
			m_xInterPoint.des_para.RODNODE.wing_offset_dist=0;
		}
		m_xInterPoint.des_para.RODNODE.len_offset_dist=m_fLenOffset;
		if(m_pJg->pEnd==m_pNode)	//终->始
			m_xInterPoint.des_para.RODNODE.direction=1;
		else
			m_xInterPoint.des_para.RODNODE.direction=0;
	}
	else
		m_xInterPoint.des_para.RODNODE.offset_wing=1;
	m_xInterPoint.des_para.RODNODE.bFaceOffset=m_iOffsetStyle!=0;
	if(m_iOffsetStyle==1)	//X-Y平面
		m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(0,0,1);
	else if(m_iOffsetStyle==1)	//Y-Z平面
		m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(1,0,0);
	else if(m_iOffsetStyle==2)	//X-Z平面
		m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(0,1,0);
	return true;
}

BOOL CSelJgPointKeyDlg::OnInitDialog() 
{
	//if(UpdateDefaultAngleKeyPoint())
	//	return FALSE;
	
#ifndef AFX_TARG_ENU_ENGLISH
	if(!PRODUCT_FUNC::IsHasInternalTest()||	!theApp.m_bManuRevisionMode)
	{
		GetDlgItem(IDC_CHK_COORD_X)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_COORD_Y)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_COORD_Z)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_CHK_COORD_X)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_CHK_COORD_Y)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_CHK_COORD_Z)->ShowWindow(SW_HIDE);
	}
	else if(PRODUCT_FUNC::IsHasInternalTest()&&	theApp.m_bManuRevisionMode)
	{
		GetDlgItem(IDC_CHK_COORD_X)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_COORD_Y)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_COORD_Z)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_CHK_COORD_X)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_CHK_COORD_Y)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_CHK_COORD_Z)->ShowWindow(SW_SHOW);
	}
#endif
	MoveWndToLocation();
	CDialog::OnInitDialog();
	OnSelchangeCmbKeyPoint();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool CSelJgPointKeyDlg::UpdateAndShowDialog(CLDSPoint* pPoint/*=NULL*/)
{
	if(GetSafeHwnd()==NULL)
		Create();
	if(pPoint==NULL&&!UpdateDefaultAngleKeyPoint())
		return false;
	else if(pPoint!=NULL&&!UpdateAngleKeyPoint(pPoint))
		return false;
#ifndef AFX_TARG_ENU_ENGLISH
	if(!PRODUCT_FUNC::IsHasInternalTest()||	!theApp.m_bManuRevisionMode)
	{
		GetDlgItem(IDC_CHK_COORD_X)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_COORD_Y)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_COORD_Z)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_CHK_COORD_X)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_CHK_COORD_Y)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_CHK_COORD_Z)->ShowWindow(SW_HIDE);
	}
	else if(PRODUCT_FUNC::IsHasInternalTest()&&	theApp.m_bManuRevisionMode)
	{
		GetDlgItem(IDC_CHK_COORD_X)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_COORD_Y)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_COORD_Z)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_CHK_COORD_X)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_CHK_COORD_Y)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_CHK_COORD_Z)->ShowWindow(SW_SHOW);
	}
#endif
	UpdateData(FALSE);
	display.AttachDisplayState(m_pJg,0);
	RedrawAngleSectionKeyPointDrawing();
	if(!IsWindowVisible())
	{
		MoveWndToLocation();
		ShowWindow(SW_SHOW);
	}
	return true;
}
bool CSelJgPointKeyDlg::UpdateAngleKeyPoint(CLDSPoint* pPoint)
{
	if( pPoint==NULL||(
		pPoint->datum_pos_style!=1&&pPoint->datum_pos_style!=2&&pPoint->datum_pos_style!=7))
		return false;
	CWnd* pCurrNodeLabel=GetDlgItem(IDC_S_CURR_NODE);
	CLDSNode* pSectNode=NULL;
	CLDSLineAngle* pAngle=NULL;
	if(pPoint->datum_pos_style==1||pPoint->datum_pos_style==7)
	{
		pAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pPoint->des_para.RODEND.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pPoint->des_para.RODEND.direction==0)
		{
			pSectNode=pAngle->pStart;
			pCurrNodeLabel->SetWindowText("当前节点：始端");
		}
		else if(pPoint->des_para.RODEND.direction==1)
		{
			pSectNode=pAngle->pEnd;
			pCurrNodeLabel->SetWindowText("当前节点：终端");
		}
		if(pAngle==NULL||pSectNode==NULL)
			return false;
		m_pJg=pAngle;
		m_pNode=pSectNode;
		JGZJ jgzj;
		if(pPoint->des_para.RODEND.wing_offset_style==4&&pPoint->des_para.RODEND.wing_offset_dist==0)
			m_iKeyPoint=0;	//楞点
		else if(pPoint->des_para.RODEND.offset_wing==0)
		{
			m_iKeyPoint=1;	//X肢心线点
			jgzj=pAngle->GetZhunJu('X');
		}
		else if(pPoint->des_para.RODEND.offset_wing==1)
		{
			m_iKeyPoint=2;	//Y肢心线点
			jgzj=pAngle->GetZhunJu('Y');
		}
		else
			getjgzj(jgzj,pAngle->size_wide);
		if(pPoint->des_para.RODEND.wing_offset_style==0)
			m_fSpecGVal=jgzj.g;
		else if(pPoint->des_para.RODEND.wing_offset_style==1)
			m_fSpecGVal=jgzj.g1;
		else if(pPoint->des_para.RODEND.wing_offset_style==2)
			m_fSpecGVal=jgzj.g2;
		else if(pPoint->des_para.RODEND.wing_offset_style==3)
			m_fSpecGVal=jgzj.g3;
		else
			m_fSpecGVal=pPoint->des_para.RODEND.wing_offset_dist;
		m_fLenOffset=pPoint->des_para.RODEND.len_offset_dist;
	}
	else if(pPoint->datum_pos_style==2)
	{
		pAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pPoint->des_para.RODNODE.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		pSectNode=Ta.Node.FromHandle(pPoint->des_para.RODNODE.hNode);
		if(pAngle==NULL||pSectNode==NULL)
			return false;
		m_pJg=pAngle;
		m_pNode=pSectNode;
		if(pSectNode==pAngle->pStart)
			pCurrNodeLabel->SetWindowText("当前节点：始端");
		else if(pSectNode==pAngle->pEnd)
			pCurrNodeLabel->SetWindowText("当前节点：末端");
		else
			pCurrNodeLabel->SetWindowText(CXhChar50("当前节点：0x%X",pSectNode->handle));

		JGZJ jgzj;
		if(pPoint->des_para.RODNODE.wing_offset_style==4&&pPoint->des_para.RODNODE.wing_offset_dist==0)
			m_iKeyPoint=0;	//楞点
		else if(pPoint->des_para.RODNODE.offset_wing==0)
		{
			m_iKeyPoint=1;	//X肢心线点
			jgzj=pAngle->GetZhunJu('X');
		}
		else if(pPoint->des_para.RODNODE.offset_wing==1)
		{
			m_iKeyPoint=2;	//Y肢心线点
			jgzj=pAngle->GetZhunJu('Y');
		}
		else
			getjgzj(jgzj,pAngle->size_wide);
		if(pPoint->des_para.RODNODE.wing_offset_style==0)
			m_fSpecGVal=jgzj.g;
		else if(pPoint->des_para.RODNODE.wing_offset_style==1)
			m_fSpecGVal=jgzj.g1;
		else if(pPoint->des_para.RODNODE.wing_offset_style==2)
			m_fSpecGVal=jgzj.g2;
		else if(pPoint->des_para.RODNODE.wing_offset_style==3)
			m_fSpecGVal=jgzj.g3;
		else
			m_fSpecGVal=pPoint->des_para.RODNODE.wing_offset_dist;
		m_fLenOffset=pPoint->des_para.RODNODE.len_offset_dist;
	}
	else
		return false;
	pPoint->UpdatePos(&Ta);
	if(pPoint!=&m_xInterPoint)
	{
		char pool[1024]={0};
		CBuffer buffer(pool,1024);
		pPoint->ToBuffer(buffer);
		m_xInterPoint.FromBuffer(buffer);
	}
	m_fPosX=pPoint->Position().x;
	m_fPosY=pPoint->Position().y;
	m_fPosZ=pPoint->Position().z;
	UpdateData(FALSE);
	return true;
}
//返回值为角钢在节点处的:0.垂直投影截面;1.X-Y平面平推;2.Y-Z平面平推;3.X-Z平面平推
int IntelliRecogWorkPlane(CLDSLineAngle* pAngle,CLDSNode* pNode,char ciWorkWingX0Y1);
void AppendAngleSectionFrameDrawing(CLDSLineAngle* pAngle,CLDSNode* pNode,int gWingX,int gWingY,BYTE ciIntersPlane=0,UCS_STRU *pObjectCS=NULL,BYTE cbHighlightFlag=0);
void CSelJgPointKeyDlg::RedrawAngleSectionKeyPointDrawing()
{
	BYTE cbDrawMode=0;
	CLDSView::RedrawAngleFrameDrawing(m_pJg,cbDrawMode);
	if(m_pNode)
	{
		int iPlaneType=IntelliRecogWorkPlane(m_pJg,m_pNode,0);
		char ciPlaneType='Z';
		//1.X-Y平面平推;2.Y-Z平面平推;3.X-Z平面平推
		if(m_iOffsetStyle==1)	//X-Y平面
			ciPlaneType='Z';
		else if(m_iOffsetStyle==2)	//Y-Z平面
			ciPlaneType='X';
		else if(m_iOffsetStyle==3)	//X-Z平面
			ciPlaneType='Y';
		else if(m_iOffsetStyle==0)
			ciPlaneType=0;
		BYTE cbHighlightFlag=0;
		if( m_iKeyPoint==0||(
			m_xInterPoint.des_para.RODNODE.wing_offset_style==4&&m_xInterPoint.des_para.RODNODE.wing_offset_dist==0))
			cbHighlightFlag=0x01;
		else if(m_xInterPoint.des_para.RODNODE.offset_wing==0)
			cbHighlightFlag=0x02;
		else //if(m_xInterPoint.des_para.RODNODE.offset_wing==1)
			cbHighlightFlag=0x04;
		if(m_iOffsetStyle>0)
			cbHighlightFlag=cbHighlightFlag<<3;
		int gx=0,gy=0;
		if(m_iKeyPoint>0&&m_xInterPoint.des_para.RODNODE.offset_wing==0)
			gx=ftoi(m_fSpecGVal);
		else if(m_iKeyPoint>0&&m_xInterPoint.des_para.RODNODE.offset_wing==1)
			gy=ftoi(m_fSpecGVal);
		AppendAngleSectionFrameDrawing(m_pJg,m_pNode,gx,gy,ciPlaneType,NULL,cbHighlightFlag);
		g_pSolidDraw->Draw();
	}
}
void CSelJgPointKeyDlg::InitDefaultSizeCheckItem()
{
	CDataCheckModel::KEYPOINT point=RetrieveKeyPoint();
	CXhChar50 idstr=CDataCheckModel::SIZE_DATA::CodingIdStr(0,point,point);
	CDataCheckModel::SIZE_DATA* pSizeItem=xDataCheckModel.GetCoordSizeItem(idstr);
	if(pSizeItem)
	{
		if(pSizeItem->cbSizeDimFlag&0x01)
			defaultSizeItem.dist.coords.distInX=pSizeItem->dist.coords.distInX;
		if(pSizeItem->cbSizeDimFlag&0x02)
			defaultSizeItem.dist.coords.distInY=pSizeItem->dist.coords.distInY;
		if(pSizeItem->cbSizeDimFlag&0x04)
			defaultSizeItem.dist.coords.distInZ=pSizeItem->dist.coords.distInZ;
	}
}
void CSelJgPointKeyDlg::OnSelchangeCmbKeyPoint() 
{
	UpdateData();
	f3dPoint pos;
	if(m_iKeyPoint==1&&m_fSpecGVal<=0)
		m_fSpecGVal = m_pJg->GetNodeWingXG(m_pNode);
	else if(m_iKeyPoint==2&&m_fSpecGVal<=0)
		m_fSpecGVal = m_pJg->GetNodeWingYG(m_pNode);
	//else
	//	m_fSpecGVal = 0;
	if(m_iOffsetStyle==0)	//投影
	{
		if(m_iKeyPoint==0)
			pos=m_pJg->GetDatumPosBer(m_pNode);
		else if(m_iKeyPoint==1)
			pos=m_pJg->GetDatumPosWingX(m_pNode,m_fSpecGVal);
		else
			pos=m_pJg->GetDatumPosWingY(m_pNode,m_fSpecGVal);
	}
	else
	{
		if(m_pNode==m_pJg->pStart&&m_pJg->desStartPos.IsFaceOffset())
			m_xInterPoint.des_para.RODNODE.vPlaneNormal=m_pJg->desStartPos.face_offset_norm;
		else if(m_pNode==m_pJg->pEnd&&m_pJg->desEndPos.IsFaceOffset())
			m_xInterPoint.des_para.RODNODE.vPlaneNormal=m_pJg->desEndPos.face_offset_norm;
		else if(m_iOffsetStyle==1)
			m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(0,0,1);
		else if(m_iOffsetStyle==2)
			m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(1,0,0);
		else if(m_iOffsetStyle==3)
			m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(0,1,0);
		if(m_iKeyPoint==0)
			pos=m_pJg->GetDatumPosBerByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal);
		else if(m_iKeyPoint==1)
			pos=m_pJg->GetDatumPosWingXByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal,NULL,m_fSpecGVal);
		else
			pos=m_pJg->GetDatumPosWingYByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal,NULL,m_fSpecGVal);
	}
	f3dPoint vec;
	if(m_pJg->pEnd==m_pNode)
		vec=m_pJg->Start()-m_pJg->End();
	else
		vec=m_pJg->End()-m_pJg->Start();
	normalize(vec);
	pos+=vec*m_fLenOffset;
	m_fPosX=pos.x;
	m_fPosY=pos.y;
	m_fPosZ=pos.z;
	if(m_iKeyPoint>0)
	{
		m_xInterPoint.des_para.RODNODE.offset_wing=m_iKeyPoint-1;
		m_xInterPoint.des_para.RODNODE.wing_offset_style=4;
		m_xInterPoint.des_para.RODNODE.wing_offset_dist=m_fSpecGVal;
	}
	else
	{
		m_xInterPoint.des_para.RODNODE.wing_offset_style=4;
		m_xInterPoint.des_para.RODNODE.wing_offset_dist=0;
	}
	RedrawAngleSectionKeyPointDrawing();
	UpdateData(FALSE);
	InitDefaultSizeCheckItem();
}

void CSelJgPointKeyDlg::OnSelchangeCmbOffsetStyle() 
{
	UpdateData();
	f3dPoint pos;
	if(m_iOffsetStyle==0)	//投影
	{
		if(m_iKeyPoint==0)
			pos=m_pJg->GetDatumPosBer(m_pNode);
		else if(m_iKeyPoint==1)
			pos=m_pJg->GetDatumPosWingX(m_pNode,m_fSpecGVal);
		else
			pos=m_pJg->GetDatumPosWingY(m_pNode,m_fSpecGVal);
		m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(0,0,0);
		m_xInterPoint.des_para.RODNODE.bFaceOffset=false;
	}
	else
	{
		if(m_iOffsetStyle==1)
			m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(0,0,1);
		else if(m_iOffsetStyle==2)
			m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(1,0,0);
		else if(m_iOffsetStyle==3)
			m_xInterPoint.des_para.RODNODE.vPlaneNormal.Set(0,1,0);
		m_xInterPoint.des_para.RODNODE.bFaceOffset=true;
		if(m_pNode==m_pJg->pStart&&m_pJg->desStartPos.IsFaceOffset())
			m_xInterPoint.des_para.RODNODE.vPlaneNormal=m_pJg->desStartPos.face_offset_norm;
		else if(m_pNode==m_pJg->pEnd&&m_pJg->desEndPos.IsFaceOffset())
			m_xInterPoint.des_para.RODNODE.vPlaneNormal=m_pJg->desEndPos.face_offset_norm;
		if(m_iKeyPoint==0)
			pos=m_pJg->GetDatumPosBerByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal);
		else if(m_iKeyPoint==1)
			pos=m_pJg->GetDatumPosWingXByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal,NULL,m_fSpecGVal);
		else
			pos=m_pJg->GetDatumPosWingYByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal,NULL,m_fSpecGVal);
	}
	f3dPoint vec;
	if(m_pJg->pEnd==m_pNode)
		vec=m_pJg->Start()-m_pJg->End();
	else
		vec=m_pJg->End()-m_pJg->Start();
	normalize(vec);
	pos+=vec*m_fLenOffset;
	m_fPosX=pos.x;
	m_fPosY=pos.y;
	m_fPosZ=pos.z;
	RedrawAngleSectionKeyPointDrawing();
	InitDefaultSizeCheckItem();
	UpdateData(FALSE);
}

void CSelJgPointKeyDlg::OnBnCopyPnt() 
{
	f3dPoint point;
	point.Set(m_fPosX,m_fPosY,m_fPosZ);
	WritePointToClip(point);
}

void CSelJgPointKeyDlg::OnChangeELenOffset() 
{
	double old_offset=m_fLenOffset;
	UpdateData();
	if(old_offset==m_fLenOffset)
		return;
	f3dPoint pos;
	if(m_iOffsetStyle==0)	//投影
	{
		if(m_iKeyPoint==0)
			pos=m_pJg->GetDatumPosBer(m_pNode);
		else if(m_iKeyPoint==1)
			pos=m_pJg->GetDatumPosWingX(m_pNode,m_fSpecGVal);
		else
			pos=m_pJg->GetDatumPosWingY(m_pNode,m_fSpecGVal);
	}
	else
	{
		if(m_iKeyPoint==0)
			pos=m_pJg->GetDatumPosBerByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal);
		else if(m_iKeyPoint==1)
			pos=m_pJg->GetDatumPosWingXByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal,NULL,m_fSpecGVal);
		else
			pos=m_pJg->GetDatumPosWingYByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal,NULL,m_fSpecGVal);
	}
	f3dPoint vec;
	if(m_pJg->pEnd==m_pNode)
		vec=m_pJg->Start()-m_pJg->End();
	else
		vec=m_pJg->End()-m_pJg->Start();
	normalize(vec);
	pos+=vec*m_fLenOffset;
	m_fPosX=pos.x;
	m_fPosY=pos.y;
	m_fPosZ=pos.z;
	UpdateData(FALSE);
}

void CSelJgPointKeyDlg::OnChangeESpecGValue() 
{
	double old_g=m_fSpecGVal;
	UpdateData();
	if(old_g==m_fSpecGVal)
		return;
	f3dPoint pos;
	if(m_iKeyPoint==1)
		m_pJg->GetDatumPosWingX(m_pNode);
	else if(m_iKeyPoint==2)
		pos=m_pJg->GetDatumPosWingY(m_pNode);
	if(m_iOffsetStyle==0)	//投影
	{
		if(m_iKeyPoint==0)
			pos=m_pJg->GetDatumPosBer(m_pNode);
		else if(m_iKeyPoint==1)
			pos=m_pJg->GetDatumPosWingX(m_pNode,m_fSpecGVal);
		else
			pos=m_pJg->GetDatumPosWingY(m_pNode,m_fSpecGVal);
	}
	else
	{
		if(m_iKeyPoint==0)
			pos=m_pJg->GetDatumPosBerByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal);
		else if(m_iKeyPoint==1)
			pos=m_pJg->GetDatumPosWingXByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal,NULL,m_fSpecGVal);
		else
			pos=m_pJg->GetDatumPosWingYByFaceOffset(m_pNode,m_xInterPoint.des_para.RODNODE.vPlaneNormal,NULL,m_fSpecGVal);
	}
	f3dPoint vec;
	if(m_pJg->pEnd==m_pNode)
		vec=m_pJg->Start()-m_pJg->End();
	else
		vec=m_pJg->End()-m_pJg->Start();
	normalize(vec);
	pos+=vec*m_fLenOffset;
	m_fPosX=pos.x;
	m_fPosY=pos.y;
	m_fPosZ=pos.z;
	m_xInterPoint.des_para.RODNODE.wing_offset_dist=m_fSpecGVal;
	RedrawAngleSectionKeyPointDrawing();
	UpdateData(FALSE);
}

void CSelJgPointKeyDlg::OnCancel()
{
	display.RestoreDisplayMode();
	display.DetachDisplayState();
	CRememberLocationDlg::OnCancel();
}

CDataCheckModel::KEYPOINT CSelJgPointKeyDlg::RetrieveKeyPoint(BOOL bUpdateData/*=FALSE*/)
{
	if(bUpdateData)
		UpdateData();
	CDataCheckModel::KEYPOINT point;
	point.ciPointType=0;
	point.angle.hAngle=m_pJg->handle;
	point.angle.hNode=m_pNode->handle;
	memset(&point.angle.offset,0,sizeof(ANGLEOFFSET));
	point.angle.offset.ciOffsetWing=2;
	if(m_iKeyPoint==0)	//楞点
		point.angle.offset.ciOffsetWing=2;
	else if(m_iKeyPoint==1)	//X肢心线点
		point.angle.offset.ciOffsetWing=0;
	else if(m_iKeyPoint==2)	//Y肢心线点
		point.angle.offset.ciOffsetWing=1;
	point.angle.offset.ciOffsetPlane=m_iOffsetStyle;
	point.angle.offset.ciStyleOfG=4;
	point.angle.offset.wiWingOffset=(short)ftoi(m_fSpecGVal);
	point.angle.offset.wiLenOffset=(short)ftoi(m_fLenOffset);
	return point;
}
void CSelJgPointKeyDlg::OnOK()
{
	display.RestoreDisplayMode();
	display.DetachDisplayState();
	CRememberLocationDlg::OnOK();
	if(PRODUCT_FUNC::IsHasInternalTest()&&(m_bChkCoordX||m_bChkCoordY||m_bChkCoordZ))
	{
		CDataCheckModel::KEYPOINT point=RetrieveKeyPoint();
		CXhChar50 idstr=CDataCheckModel::SIZE_DATA::CodingIdStr(0,point,point);
		CDataCheckModel::SIZE_DATA* pSizeItem=xDataCheckModel.AppendCoordSizeItem(idstr);
		pSizeItem->ciSizeType=0;
		pSizeItem->from=pSizeItem->to=point;
		pSizeItem->cbSizeDimFlag=0;
		if(m_bChkCoordX)
		{
			pSizeItem->cbSizeDimFlag|=0x01;
			pSizeItem->dist.coords.distInX=(float)m_fChkCoordX;
		}
		if(m_bChkCoordY)
		{
			pSizeItem->cbSizeDimFlag|=0x02;
			pSizeItem->dist.coords.distInY=(float)m_fChkCoordY;
		}
		if(m_bChkCoordZ)
		{
			pSizeItem->cbSizeDimFlag|=0x04;
			pSizeItem->dist.coords.distInZ=(float)m_fChkCoordZ;
		}
		CLDSView::AppendSizeCoordCheckItem(pSizeItem);
		g_pSolidDraw->Draw();
	}
}

void CSelJgPointKeyDlg::OnBnChkCoordX()
{
	UpdateData();
	if(m_bChkCoordX)
	{
		if(defaultSizeItem.cbSizeDimFlag&0x04)
			m_fChkCoordX=defaultSizeItem.dist.coords.distInX;
		else
			m_fChkCoordX=defaultSizeItem.dist.coords.distInX=(float)fto_halfi(m_fPosX);
		UpdateData(FALSE);
		GetDlgItem(IDC_E_CHK_COORD_X)->EnableWindow(TRUE);
	}
	else
		GetDlgItem(IDC_E_CHK_COORD_X)->EnableWindow(FALSE);
}


void CSelJgPointKeyDlg::OnBnChkCoordY()
{
	UpdateData();
	if(m_bChkCoordY)
	{
		if(defaultSizeItem.cbSizeDimFlag&0x04)
			m_fChkCoordY=defaultSizeItem.dist.coords.distInY;
		else
			m_fChkCoordY=defaultSizeItem.dist.coords.distInY=(float)fto_halfi(m_fPosY);
		UpdateData(FALSE);
		GetDlgItem(IDC_E_CHK_COORD_Y)->EnableWindow(TRUE);
	}
	else
		GetDlgItem(IDC_E_CHK_COORD_Y)->EnableWindow(FALSE);
}


void CSelJgPointKeyDlg::OnBnChkCoordZ()
{
	UpdateData();
	if(m_bChkCoordZ)
	{
		CDataCheckModel::SIZE_DATA* pSizeItem=&defaultSizeItem;
		if(pSizeItem->cbSizeDimFlag&0x04)
			m_fChkCoordZ=pSizeItem->dist.coords.distInZ;
		else
			m_fChkCoordZ=defaultSizeItem.dist.coords.distInZ=(float)fto_halfi(m_fPosZ);
		UpdateData(FALSE);
		GetDlgItem(IDC_E_CHK_COORD_Z)->EnableWindow(TRUE);
	}
	else
		GetDlgItem(IDC_E_CHK_COORD_Z)->EnableWindow(FALSE);
}

void CSelJgPointKeyDlg::OnEnChangeEChkCoordX()
{
	UpdateData();
	defaultSizeItem.dist.coords.distInX=(float)m_fChkCoordX;
}

void CSelJgPointKeyDlg::OnEnChangeEChkCoordY()
{
	UpdateData();
	defaultSizeItem.dist.coords.distInY=(float)m_fChkCoordY;
}


void CSelJgPointKeyDlg::OnEnChangeEChkCoordZ()
{
	UpdateData();
	defaultSizeItem.dist.coords.distInZ=(float)m_fChkCoordZ;
}

#endif
