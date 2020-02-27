//<LOCALE_TRANSLATE Confirm by hxr/>
// MakePlateBendDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "MakePlateBendDlg.h"
#include "GlobalFunc.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef __TSA_
/////////////////////////////////////////////////////////////////////////////
// CMakePlateBendDlg dialog
CMakePlateBendDlg::CMakePlateBendDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CMakePlateBendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMakePlateBendDlg)
	m_fBaseX = 0.0;
	m_fBaseY = 0.0;
	m_fBaseZ = 0.0;
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	m_iHuoquType = 0;
	//}}AFX_DATA_INIT
	m_pPlate=NULL;
}


void CMakePlateBendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMakePlateBendDlg)
	DDX_Text(pDX, IDC_E_BASE_X, m_fBaseX);
	DDX_Text(pDX, IDC_E_BASE_Y, m_fBaseY);
	DDX_Text(pDX, IDC_E_BASE_Z, m_fBaseZ);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMakePlateBendDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CMakePlateBendDlg)
	ON_BN_CLICKED(IDC_BNT_PASTE_PLANE, OnPastePlane)
	ON_BN_CLICKED(IDC_BNT_PASTE_NORM, OnPasteNorm)
	ON_BN_CLICKED(IDC_BNT_PASTE_BASE, OnPasteBase)
	ON_BN_CLICKED(IDCANCEL, OnCancle)
	ON_BN_CLICKED(IDC_BNT_REVERSE_BASE, OnReverseBase)
	ON_BN_CLICKED(IDC_BNT_REVERSE_NORM, OnReverseNorm)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_REVERSE_BASE_NORM, OnReverseBaseNorm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMakePlateBendDlg message handlers

void CMakePlateBendDlg::OnPastePlane() 
{
	f3dPoint face_base,face_norm;
	if(ReadClipPlane(face_base,face_norm))
	{
		m_fNormX=face_norm.x;
		m_fNormY=face_norm.y;
		m_fNormZ=face_norm.z;
		m_fBaseX=face_base.x;
		m_fBaseY=face_base.y;
		m_fBaseZ=face_base.z;
		UpdateData(FALSE);
	}
}

void CMakePlateBendDlg::OnPasteNorm() 
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

void CMakePlateBendDlg::OnPasteBase() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fBaseX = point.x;
		m_fBaseY = point.y;
		m_fBaseZ = point.z;
		UpdateData(FALSE);
	}
}

BOOL CMakePlateBendDlg::OnInitDialog() 
{
	if(m_pPlate==NULL)
		return FALSE;
	//
	OnPastePlane(); //读取剪贴板中数据
	datum_plane_pos=m_pPlate->ucs.origin+m_pPlate->ucs.axis_z*m_pPlate->m_fNormOffset;
	datum_plane_norm=m_pPlate->ucs.axis_z;
	//
	m_curBitmap1.Detach();
	m_curBitmap2.Detach();
	m_curBitmap1.LoadBitmap(IDB_BMP_HUOQU_TYPE);
	m_curBitmap2.LoadBitmap(IDB_BMP_HUOQU_TYPE2);
	CStatic *pPic1=(CStatic*)GetDlgItem(IDC_S_TYPE1);
	CStatic *pPic2=(CStatic*)GetDlgItem(IDC_S_TYPE2);
	pPic1->SetBitmap(m_curBitmap1);
	pPic2->SetBitmap(m_curBitmap2);
	CDialog::OnInitDialog();
	MoveWndToLocation();
	return TRUE;
}

void CMakePlateBendDlg::OnCancle() 
{
	CDialog::OnCancel();	
}

void CMakePlateBendDlg::OnOK() 
{
	UpdateData();
	huoqu_norm.Set(m_fNormX,m_fNormY,m_fNormZ);
	huoqu_pos.Set(m_fBaseX,m_fBaseY,m_fBaseZ);
	CDialog::OnOK();
}

void CMakePlateBendDlg::OnReverseBase() 
{
	UpdateData();
	m_fBaseX*=-1.0;
	m_fBaseY*=-1.0;
	m_fBaseZ*=-1.0;
	UpdateData(FALSE);
}

void CMakePlateBendDlg::OnReverseNorm() 
{
	UpdateData();
	m_fNormX*=-1.0;
	m_fNormY*=-1.0;
	m_fNormZ*=-1.0;
	UpdateData(FALSE);
	Invalidate(FALSE);
}

void CMakePlateBendDlg::OnPaint() 
{
	CPaintDC dc(this); 

	int i=0;
	CRect rectDrawRgn; //当前绘图区域
	CWnd *pWnd = GetDlgItem(IDC_S_MAP);
	pWnd->GetWindowRect(&rectDrawRgn);
	ScreenToClient(&rectDrawRgn);
	int width=rectDrawRgn.Width();
	int height=rectDrawRgn.Height();
	CPoint topLeft=rectDrawRgn.TopLeft();
	//
	dc.SelectStockObject(LTGRAY_BRUSH); 
	dc.Rectangle(rectDrawRgn);
	dc.SelectStockObject(HOLLOW_BRUSH);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0,0,0));
	//
	UCS_2dSTRU draw_ucs;	//绘图坐标系
	/*f3dPoint axis_x=f3dPoint(topLeft.x+width,topLeft.y)-f3dPoint(topLeft.x,topLeft.y+height);
	normalize(axis_x);
	draw_ucs.axis_x.Set(axis_x.x,axis_x.y);
	draw_ucs.axis_y.Set(-axis_x.y,axis_x.x);
	double axis_x_len=ftoi(sqrt(width*width+height*height));
	f3dPoint draw_origin=f3dPoint(topLeft.x,topLeft.y+height)+axis_x*0.2*axis_x_len;
	draw_ucs.origin.Set(draw_origin.x,draw_origin.y);*/
	draw_ucs.origin.Set(topLeft.x+10,topLeft.y+0.5*height);
	draw_ucs.axis_x.Set(1,0);
	//根据当前活动视图定制示意图绘制坐标系
	/*CDisplayView *pActiveView=Ta.GetActiveView();
	if(pActiveView->ucs.axis_z*m_pPlate->ucs.axis_z>0)
		draw_ucs.axis_y.Set(0,1);
	else*/
	draw_ucs.axis_y.Set(0,-1);
	double axis_x_len=width-20;
	
	CPen solid_pen,purple_solid_pen,blue_solid_pen,red_solid_pen;
	solid_pen.CreatePen(PS_SOLID,2,RGB(0,0,0));
	blue_solid_pen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	purple_solid_pen.CreatePen(PS_SOLID,2,RGB(255,0,255));
	red_solid_pen.CreatePen(PS_SOLID,1,RGB(128,0,64));

	f2dPoint origin=draw_ucs.origin;
	int nFlag=1;	
	if(m_pPlate->ucs.axis_z*datum_plane_norm<0)
		nFlag=-1;
	int axis_z_len=ftoi(0.15*height);
	//绘制Z轴
	CPen *pOldPen=dc.SelectObject(&blue_solid_pen);
	for(i=0;i<2;i++)
	{
		double offset_dist=0;
		if(i==1)
			offset_dist=axis_x_len;
		//Z轴
		dc.MoveTo(ftoi(origin.x+offset_dist),ftoi(origin.y));
		dc.LineTo(ftoi(origin.x+offset_dist),ftoi(origin.y-nFlag*axis_z_len));
		//绘制箭头
		dc.LineTo(ftoi(origin.x+offset_dist-3),ftoi(origin.y-nFlag*(axis_z_len-6)));
		dc.MoveTo(ftoi(origin.x+offset_dist),ftoi(origin.y-nFlag*axis_z_len));
		dc.LineTo(ftoi(origin.x+offset_dist+3),ftoi(origin.y-nFlag*(axis_z_len-6)));
	}
	dc.SelectObject(pOldPen);
	
	//绘制钢板截面示意图
	CPoint pt_arr[4];
	f3dPoint vertex_arr[4];
	double draw_thick=6;
	vertex_arr[0].Set(0,draw_thick*0.5);
	vertex_arr[1].Set(0,-draw_thick*0.5);
	vertex_arr[2].Set(axis_x_len,-draw_thick*0.5);
	vertex_arr[3].Set(axis_x_len,draw_thick*0.5);
	for(i=0;i<4;i++)
	{
		f2dPoint vertex(vertex_arr[i].x,vertex_arr[i].y);
		coord_trans(vertex,draw_ucs,TRUE);
		vertex_arr[i].Set(vertex.x,vertex.y);
		pt_arr[i].x=ftoi(vertex_arr[i].x);
		pt_arr[i].y=ftoi(vertex_arr[i].y);
	}
	pOldPen=dc.SelectObject(&solid_pen);
	for(i=0;i<4;i++)
	{
		dc.MoveTo(ftoi(vertex_arr[i].x),ftoi(vertex_arr[i].y));
		dc.LineTo(ftoi(vertex_arr[(i+1)%4].x),ftoi(vertex_arr[(i+1)%4].y));
	}
	CRgn rgn;
	rgn.CreatePolygonRgn(pt_arr,4,ALTERNATE);
	CBrush curBrush;
	curBrush.CreateSolidBrush(RGB(0,220,0));
	dc.FillRgn(&rgn,&curBrush);
	dc.SelectObject(pOldPen);
	//火曲面延伸方向,火曲法线
	f3dPoint draw_len_vec,norm,draw_norm;
	UpdateData();
	norm.Set(m_fNormX,m_fNormY,m_fNormZ);
	normalize(norm);
	vector_trans(norm,m_pPlate->ucs,FALSE);
	normalize(norm);
	if(fabs(norm.x)<fabs(norm.y))
		draw_norm.Set(-norm.y,-norm.z);
	else
		draw_norm.Set(-norm.x,-norm.z);
	vector_trans(f2dPoint(draw_norm.x,draw_norm.y),draw_ucs,TRUE);
	draw_len_vec.Set(-draw_norm.y,draw_norm.x);
	f3dPoint start_pt,end_pt;
	start_pt=f3dPoint(origin.x+0.5*width,origin.y)+draw_len_vec*0.4*height;
	end_pt=f3dPoint(origin.x+0.5*width,origin.y)-draw_len_vec*0.4*height;
	//
	pOldPen=dc.SelectObject(&purple_solid_pen);
	dc.MoveTo(ftoi(start_pt.x),ftoi(start_pt.y));
	dc.LineTo(ftoi(end_pt.x),ftoi(end_pt.y));
	dc.SelectObject(pOldPen);
	//绘制火曲面法线方向
	pOldPen=dc.SelectObject(&red_solid_pen);
	for(i=0;i<2;i++)
	{
		f3dPoint datum_pt=start_pt;
		if(i==1)
			datum_pt=end_pt;	
		vertex_arr[0]=datum_pt;
		vertex_arr[1]=datum_pt+draw_norm*axis_z_len;
		f3dPoint temp_pt=datum_pt+draw_norm*(axis_z_len-6);
		vertex_arr[2]=temp_pt+draw_len_vec*3;
		vertex_arr[3]=temp_pt-draw_len_vec*3;
		dc.MoveTo(ftoi(vertex_arr[0].x),ftoi(vertex_arr[0].y));
		dc.LineTo(ftoi(vertex_arr[1].x),ftoi(vertex_arr[1].y));
		dc.LineTo(ftoi(vertex_arr[2].x),ftoi(vertex_arr[2].y));
		dc.MoveTo(ftoi(vertex_arr[1].x),ftoi(vertex_arr[1].y));
		dc.LineTo(ftoi(vertex_arr[3].x),ftoi(vertex_arr[3].y));
	}
	dc.SelectObject(pOldPen);
}

void CMakePlateBendDlg::OnReverseBaseNorm() 
{
	UpdateData();
	datum_plane_norm*=-1.0;
	//
	m_curBitmap1.Detach();
	m_curBitmap2.Detach();
	if(datum_plane_norm*m_pPlate->ucs.axis_z>0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetDlgItemText(IDC_BTN_REVERSE_BASE_NORM,"reverse datum plane normal line (positive direction)");
#else
		SetDlgItemText(IDC_BTN_REVERSE_BASE_NORM,"反转基准面法线(正向)");
#endif
		m_curBitmap1.LoadBitmap(IDB_BMP_HUOQU_TYPE);
		m_curBitmap2.LoadBitmap(IDB_BMP_HUOQU_TYPE2);
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetDlgItemText(IDC_BTN_REVERSE_BASE_NORM,"reverse datum plane normal line (reverse direction)");
#else
		SetDlgItemText(IDC_BTN_REVERSE_BASE_NORM,"反转基准面法线(反向)");
#endif
		m_curBitmap1.LoadBitmap(IDB_BMP_HUOQU_TYPE1);
		m_curBitmap2.LoadBitmap(IDB_BMP_HUOQU_TYPE3);
	}
	CStatic *pPic1=(CStatic*)GetDlgItem(IDC_S_TYPE1);
	CStatic *pPic2=(CStatic*)GetDlgItem(IDC_S_TYPE2);
	pPic1->SetBitmap(m_curBitmap1);
	pPic2->SetBitmap(m_curBitmap2);
	Invalidate(FALSE);
	UpdateData(FALSE);
}
#endif

