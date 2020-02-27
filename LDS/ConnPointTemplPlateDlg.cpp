// ConnPointTemplPlateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "afxdialogex.h"
#include "ConnPointTemplPlateDlg.h"
#include "StdComTempl/NodeStdComTempl.h"

//////////////////////////////////////////////////////////////////////////
// CConnPointTemplPlateDlg 对话框
static void LoadDesignParamItemInfo(DESIGN_PARAM_ITEM *pItem,DWORD key){pItem->key=key;}
CConnPointTemplPlateDlg::CConnPointTemplPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConnPointTemplPlateDlg::IDD, pParent)
{
	m_idTemplPlate=0;
	m_dwIconWidth=100;
	m_dwIconHeight=100;
}

CConnPointTemplPlateDlg::~CConnPointTemplPlateDlg()
{
}

void CConnPointTemplPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CTRL, m_xIconListCtrl);
	DDX_Control(pDX, IDC_LIST_BOX, m_xPropList);
}


BEGIN_MESSAGE_MAP(CConnPointTemplPlateDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CConnPointTemplPlateDlg::OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CTRL, &CConnPointTemplPlateDlg::OnLvnItemchangedListCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCANCEL, &CConnPointTemplPlateDlg::OnBnClickedCancel)
	ON_BN_CLICKED(ID_BY_GENERAL_POLICY, &CConnPointTemplPlateDlg::OnBnByGeneralPolicy)
END_MESSAGE_MAP()

// CConnPointTemplPlateDlg 消息处理程序
BOOL CConnPointTemplPlateDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GetWindowRect(&m_rectInitWndRgn);
	CWnd *pWnd = GetDlgItem(IDC_S_COM_TMPL_SKETCH);
	pWnd->GetWindowRect(&m_rectInitDrawRgn);	//绘图有效区的屏坐标区域
	//暂时标准节点板还不需要自定义属性
	if((pWnd=GetDlgItem(IDC_LIST_BOX))!=NULL)
		pWnd->ShowWindow(SW_HIDE);
	if((pWnd=GetDlgItem(IDC_E_PROP_HELP_STR))!=NULL)
		pWnd->ShowWindow(SW_HIDE);
	//
	if(imageList.GetSafeHandle()!=NULL)
		imageList.DeleteImageList();
	imageList.Create(m_dwIconWidth, m_dwIconHeight, ILC_COLOR32, 2, 4);
	//初始化ListCtrol
	m_xIconListCtrl.SetExtendedStyle(LVS_ICON);
	m_xIconListCtrl.SetBkColor(RGB(255,255,255));
	m_xIconListCtrl.SetImageList(&imageList,LVSIL_NORMAL);
	m_xIconListCtrl.SetIconSpacing(CSize(m_dwIconWidth+2, m_dwIconHeight+20));	//设置图片间隔
	m_xIconListCtrl.Arrange(LVA_ALIGNLEFT);
	RefreshImageLictCtr();
	UpdateData(FALSE);
	return TRUE;  
}
void CConnPointTemplPlateDlg::SetCurrConnPoint(CStdComTemplConnPoint* pStdConnPoint)
{
	m_pStdConnPoint = pStdConnPoint;
}

void CConnPointTemplPlateDlg::RefreshImageLictCtr()
{
	while (imageList.GetImageCount() > 0)
		imageList.Remove(0);		//清空图标
	m_xIconListCtrl.DeleteAllItems();
	//
	CClientDC dc(this);
	CStdComTemplConnPoint::CMatchStdPlate *pMatchStdPlate = NULL;
	int indexOfImg=0, indexOfItem = 0;
	for (pMatchStdPlate = m_pStdConnPoint->hashMaybeTmplPlates.GetFirst();pMatchStdPlate;pMatchStdPlate = m_pStdConnPoint->hashMaybeTmplPlates.GetNext())
	{
		if (!pMatchStdPlate->blTmpPassedVerify)
			continue;
		CStdComTempl* pStdNodePlate = globalStdComTmplLibrary.LoadStdComTempl(pMatchStdPlate->idStdNodePlate);
		if (pStdNodePlate == NULL)
			continue;
		//在源内存设备上加载位图
		CDC dcSrcBmp, dcDestBmp;
		CBitmap  srcbmp, destbmp;
		if (pStdNodePlate->LoadSketchImage())
		{
			dcSrcBmp.CreateCompatibleDC(NULL);
			dcDestBmp.CreateCompatibleDC(&dc);
			//将源内存设备上的位图进行缩放，然后复制到目标内存设备上
			srcbmp.CreateBitmapIndirect(&pStdNodePlate->sketchimg);
			dcSrcBmp.SelectObject(&srcbmp);
			destbmp.CreateCompatibleBitmap(&dc, m_dwIconWidth, m_dwIconHeight);
			CBitmap* pOldObj = dcDestBmp.SelectObject(&destbmp);
			dcDestBmp.StretchBlt(0, 0, m_dwIconWidth, m_dwIconHeight, &dcSrcBmp, 0, 0, pStdNodePlate->sketchimg.bmWidth, pStdNodePlate->sketchimg.bmHeight, SRCCOPY);
			dcDestBmp.SelectObject(pOldObj);	//使位图脱离内存设备
		}
		//将位图添加到位图列表
		indexOfImg = imageList.Add(&destbmp, RGB(0, 0, 0));
		indexOfItem=m_xIconListCtrl.InsertItem(m_xIconListCtrl.GetItemCount(), pStdNodePlate->name, indexOfImg);
		m_xIconListCtrl.SetItemData(indexOfItem,(DWORD)pStdNodePlate);
	}
	//设置默认选中项
	if(m_xIconListCtrl.GetItemCount()>0)
	{
		m_xIconListCtrl.SetSelectionMark(0);
		m_xIconListCtrl.SetItemState(0,LVIS_SELECTED|LVNI_FOCUSED,LVIS_SELECTED|LVNI_FOCUSED);
	}
}
void CConnPointTemplPlateDlg::RelayoutWnd()
{
	return;
	CRect rect;
	if (m_xSketchImg.GetSafeHandle() != NULL)
	{
		BITMAP bitmap;
		m_xSketchImg.GetBitmap(&bitmap);
		int width = max(bitmap.bmWidth, m_rectInitDrawRgn.Width());
		int height = max(bitmap.bmHeight, m_rectInitDrawRgn.Height());
		CWnd *pWnd = GetDlgItem(IDC_S_COM_TMPL_SKETCH);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->MoveWindow(rect.left, rect.top, width, height);
		//
		int wndW = m_rectInitWndRgn.Width();
		int wndH = m_rectInitWndRgn.Height();
		GetWindowRect(&rect);
		MoveWindow(rect.left, rect.top, wndW+width- m_rectInitDrawRgn.Width(),wndH+height- m_rectInitDrawRgn.Height());
	}
	else
	{
		GetWindowRect(&rect);
		if (rect.Width() != m_rectInitWndRgn.Width() || rect.Height() != m_rectInitWndRgn.Height())
			MoveWindow(rect.left, rect.top, m_rectInitWndRgn.Width(), m_rectInitWndRgn.Height());
		CWnd *pWnd = GetDlgItem(IDC_S_COM_TMPL_SKETCH);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		if (rect.Width() != m_rectInitDrawRgn.Width() || rect.Height() != m_rectInitDrawRgn.Height())
			pWnd->MoveWindow(rect.left, rect.top, m_rectInitDrawRgn.Width(), m_rectInitDrawRgn.Height());
	}
}
void CConnPointTemplPlateDlg::OnLvnItemchangedListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	POSITION pos=m_xIconListCtrl.GetFirstSelectedItemPosition();
	int iCurSel=m_xIconListCtrl.GetNextSelectedItem(pos);
	if (iCurSel == -1)
		return;
	CStdComTempl* pStdNodePlate = (CStdComTempl*)m_xIconListCtrl.GetItemData(iCurSel);
	if (pStdNodePlate == NULL)
		return;
	CStatic *pSketchPanel = (CStatic*)GetDlgItem(IDC_S_COM_TMPL_SKETCH);
	m_idTemplPlate = pStdNodePlate->Id;
	if (m_xSketchImg.GetSafeHandle() != NULL)
		m_xSketchImg.Detach();
	m_xSketchImg.CreateBitmapIndirect(&pStdNodePlate->sketchimg);
	HBITMAP hBitmap = (HBITMAP)m_xSketchImg.GetSafeHandle();
	pSketchPanel->SetBitmap(hBitmap);
	RelayoutWnd();
	//Invalidate();
	*pResult = 0;
};

void CConnPointTemplPlateDlg::OnBnClickedOk()
{
	if (m_idTemplPlate==0)
	{
		AfxMessageBox("");
		return CDialogEx::OnCancel();
	}
	CDialogEx::OnOK();
}

void CConnPointTemplPlateDlg::InvertLine(CDC* pDC,CPoint ptFrom,CPoint ptTo)
{
	int nOldMode = pDC->SetROP2(R2_NOT);
	pDC->MoveTo(ptFrom);
	pDC->LineTo(ptTo);
	pDC->SetROP2(nOldMode);
}
void CConnPointTemplPlateDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	m_rcClient.bottom = cy;
	m_rcClient.right = cx;	
}
void CConnPointTemplPlateDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ((point.y>=m_nOldHorzY-5) && (point.y<=m_nOldHorzY+5))
	{
		::SetCursor(m_hCursorSize);
		CRect windowRect;
		GetWindowRect(windowRect);
		windowRect.left += 10; windowRect.right -= 10;
		::ClipCursor(windowRect);
		m_nOldHorzY = point.y;

		CClientDC dc(this);
		InvertLine(&dc,CPoint(m_rcClient.left,m_nOldHorzY),CPoint(m_rcClient.right,m_nOldHorzY));
		SetCapture();
		m_bTracking = TRUE;
	}
	else
	{
		m_bTracking = FALSE;
		CDialogEx::OnLButtonDown(nFlags, point);
	}
}
void CConnPointTemplPlateDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bTracking)
	{
		m_bTracking = FALSE;
		if (GetCapture()==this)
			::ReleaseCapture();
		::ClipCursor(NULL);
		CClientDC dc(this);
		InvertLine(&dc,CPoint(m_rcClient.left,point.y),CPoint(m_rcClient.right,point.y));
		m_nOldHorzY = point.y;

		RECT rcTop,rcBottom;
		CWnd *pTopWnd = GetDlgItem(IDC_LIST_BOX);
		CWnd *pBtmWnd = GetDlgItem(IDC_E_PROP_HELP_STR);
		if(pTopWnd)
			pTopWnd->GetWindowRect(&rcTop);
		if(pBtmWnd)
			pBtmWnd->GetWindowRect(&rcBottom);
		ScreenToClient(&rcTop);
		ScreenToClient(&rcBottom);
		rcBottom.top = m_nOldHorzY+m_nSplitterWidth/2;
		rcTop.bottom = rcBottom.top-m_nSplitterWidth-1;
		pTopWnd->MoveWindow(&rcTop);
		pBtmWnd->MoveWindow(&rcBottom);
		Invalidate();
	}
	else
		CDialogEx::OnLButtonUp(nFlags, point);
}
void CConnPointTemplPlateDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	/*if (m_bTracking)
	{
		CClientDC dc(this);
		InvertLine(&dc,CPoint(m_rcClient.left,m_nOldHorzY),CPoint(m_rcClient.right,m_nOldHorzY));
		InvertLine(&dc,CPoint(m_rcClient.left,point.y),CPoint(m_rcClient.right,point.y));
		m_nOldHorzY = point.y;
	}
	else if ((point.y >= m_nOldHorzY-5) && (point.y <= m_nOldHorzY+5))
		::SetCursor(m_hCursorSize);
	else*/
		CDialogEx::OnMouseMove(nFlags, point);
}
void CConnPointTemplPlateDlg::OnClose()
{
	CDialogEx::OnClose();
}
void CConnPointTemplPlateDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

#include "PropertyListOper.h"
void CConnPointTemplPlateDlg::DisplayParaProperty()
{
	/*if (!pPlate->m_bParams)
		return;
	CPropertyList *pPropList=(CPropertyList*)GetDlgItem(IDC_LIST_BOX);
	pPropList->CleanCallBackFunc();
	pPropList->CleanTree();
	pPropList->m_nObjClassTypeId = 0;
	CPropertyListOper<CTemplPlate> oper(pPropList,pPlate);
	CPropTreeItem* pRootItem=pPropList->GetRootItem();
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL;
	if (m_idTemplPlate==STDTEMPL_NODEPLATE_HD7)
	{

	} 
	else if (m_idTemplPlate==STDTEMPL_NODEPLATE_HD7_YS)
	{

	}
	pPropList->Redraw();*/
}

void CConnPointTemplPlateDlg::OnBnByGeneralPolicy()
{
	m_idTemplPlate=0;
	CDialogEx::OnOK();
}
