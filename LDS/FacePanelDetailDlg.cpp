// FacePanelDetailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "FacePanelDetailDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFacePanelDetailDlg dialog


CFacePanelDetailDlg::CFacePanelDetailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFacePanelDetailDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFacePanelDetailDlg)
	//}}AFX_DATA_INIT
	rgn_scale=1.0;
}


void CFacePanelDetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFacePanelDetailDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFacePanelDetailDlg, CDialog)
	//{{AFX_MSG_MAP(CFacePanelDetailDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFacePanelDetailDlg message handlers

BOOL CFacePanelDetailDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CPoint pos(0,0);	//客户区左上角坐标
	CWnd *pWnd=GetDlgItem(IDC_S_MAP);
	pWnd->GetWindowRect(&m_rectValidRgn);	//绘图有效区的屏坐标区域
	ClientToScreen(&pos);	//对话框客户区左上角坐标转为屏幕坐标
	m_rectValidRgn.left		-=pos.x;
	m_rectValidRgn.right	-=pos.x;
	m_rectValidRgn.top		-=pos.y;
	m_rectValidRgn.bottom	-=pos.y;
	scope.ClearScope();
	for(CFacePanelNode *pNode=m_xFacePanel.node_list.GetFirst();pNode;
						pNode=m_xFacePanel.node_list.GetNext())
		scope.VerifyVertex(f3dPoint(pNode->pos.x,pNode->pos.y));
	int wide=m_rectValidRgn.Width();
	int high=m_rectValidRgn.Height();
	rgn_scale=min((wide-14)/scope.wide(),(high-14)/scope.high());
	//确定绘图区相对对话框客户区的相对坐标系
	scope.centre(&rgn_ucs.origin.x,&rgn_ucs.origin.y,&rgn_ucs.origin.z);
	rgn_ucs.origin*=rgn_scale;
	rgn_ucs.origin+=f3dPoint(-wide/2-m_rectValidRgn.left,high/2+m_rectValidRgn.top);
	rgn_ucs.axis_x.Set(1,0,0);
	rgn_ucs.axis_y.Set(0,-1,0);
	rgn_ucs.axis_z.Set(0,0,-1);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFacePanelDetailDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	dc.SelectStockObject(BLACK_BRUSH); // TODO: Add your message handler code here
	dc.Rectangle(m_rectValidRgn);
	dc.SelectStockObject(HOLLOW_BRUSH);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,0,0));
	CPen pen;
	pen.CreatePen(PS_SOLID,1,RGB(0,255,0));
	CPen *pOldPen=dc.SelectObject(&pen);
	for(CFacePanelNode *pNode=m_xFacePanel.node_list.GetFirst();pNode;
						pNode=m_xFacePanel.node_list.GetNext())
	{
		f3dPoint pos(pNode->pos.x*rgn_scale,pNode->pos.y*rgn_scale);
		coord_trans(pos,rgn_ucs,FALSE);
		f3dPoint start(pos.x-6,pos.y);
		f3dPoint end(pos.x+6,pos.y);
		dc.MoveTo(ftoi(start.x),ftoi(start.y));
		dc.LineTo(ftoi(end.x),ftoi(end.y));
		start.Set(pos.x,pos.y+6);
		end.Set(pos.x,pos.y-6);
		dc.MoveTo(ftoi(start.x),ftoi(start.y));
		dc.LineTo(ftoi(end.x),ftoi(end.y));
		start.Set(pos.x-4,pos.y-4);
		end.Set(pos.x+4,pos.y+4);
		dc.MoveTo(ftoi(start.x),ftoi(start.y));
		dc.LineTo(ftoi(end.x),ftoi(end.y));
		start.Set(pos.x-4,pos.y+4);
		end.Set(pos.x+4,pos.y-4);
		dc.MoveTo(ftoi(start.x),ftoi(start.y));
		dc.LineTo(ftoi(end.x),ftoi(end.y));
		dc.TextOut((int)pos.x,(int)pos.y,pNode->sTitle,strlen(pNode->sTitle));
	}
	for(CFacePanelPole *pPole=m_xFacePanel.pole_list.GetFirst();pPole;
						pPole=m_xFacePanel.pole_list.GetNext())
	{
		f2dPoint start2d=m_xFacePanel.node_list[pPole->start_i].pos;
		f2dPoint end2d  =m_xFacePanel.node_list[pPole->end_i].pos;
		f3dPoint start(start2d.x,start2d.y),end(end2d.x,end2d.y);
		start*=rgn_scale;
		end  *=rgn_scale;
		coord_trans(start,rgn_ucs,FALSE);
		coord_trans(end,rgn_ucs,FALSE);
		dc.MoveTo(ftoi(start.x),ftoi(start.y));
		dc.LineTo(ftoi(end.x),ftoi(end.y));
	}
	dc.SelectObject(pOldPen);
}

void CFacePanelDetailDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CFacePanelDetailDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnMouseMove(nFlags, point);
}
