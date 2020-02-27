// CDragListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DragListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDragListCtrl

CDragListCtrl::CDragListCtrl()
{
	m_pDragImage=NULL;
}

CDragListCtrl::~CDragListCtrl()
{
	m_pDragImage=NULL;
}


BEGIN_MESSAGE_MAP(CDragListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CDragListCtrl)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDragListCtrl message handlers
CImageList* CDragListCtrl::CreateDragImage(const CPoint& ptMouse)  
{  
	CRect rcClient, rcOne, rcAll( 32000, 32000, -32000, -32000 );  
	int nIndex;  
	if ( GetSelectedCount() == 0 ) return NULL;  
	SetFocus();  
	GetClientRect( &rcClient );  
	int nCount = 0;  
	for ( nIndex = -1 ; ( nIndex = GetNextItem( nIndex, LVNI_SELECTED ) ) >= 0 && MAX_DRAG_SIZE >= nCount; )  
	{  
		m_nDrag[ nCount++ ] = nIndex;  
		GetItemRect( nIndex, rcOne, LVIR_BOUNDS );  
		if ( rcOne.IntersectRect( &rcClient, &rcOne ) )  
		{  
			rcAll.left   = min( rcAll.left, rcOne.left );  
			rcAll.top   = min( rcAll.top, rcOne.top );  
			rcAll.right   = max( rcAll.right, rcOne.right );  
			rcAll.bottom = max( rcAll.bottom, rcOne.bottom );  
		}  
		SetItemState( nIndex, 0, LVIS_FOCUSED );  
	}  
	BOOL bClipped = rcAll.Height() > MAX_DRAG_SIZE;  
	if ( bClipped )  
	{  
		rcAll.left   = max( rcAll.left, ptMouse.x - MAX_DRAG_SIZE_2 );  
		rcAll.right   = max( rcAll.right, ptMouse.x + MAX_DRAG_SIZE_2 );  
		rcAll.top   = max( rcAll.top, ptMouse.y - MAX_DRAG_SIZE_2 );  
		rcAll.bottom = max( rcAll.bottom, ptMouse.y + MAX_DRAG_SIZE_2 );  
	}  
	CClientDC dcClient( this );  
	CBitmap bmAll, bmDrag;  
	CDC dcAll, dcDrag;  
	if ( ! dcAll.CreateCompatibleDC( &dcClient ) )  
		return NULL;  
	if ( ! bmAll.CreateCompatibleBitmap( &dcClient, rcClient.Width(), rcClient.Height() ) )  
		return NULL;  
	if ( ! dcDrag.CreateCompatibleDC( &dcClient ) )  
		return NULL;  
	if ( ! bmDrag.CreateCompatibleBitmap( &dcClient, rcAll.Width(), rcAll.Height() ) )  
		return NULL;  
	CBitmap *pOldAll = dcAll.SelectObject( &bmAll );  
	COLORREF crDrag = RGB( 250, 255, 250 );  
	dcAll.FillSolidRect( &rcClient, crDrag );  
	COLORREF crBack = GetBkColor();  
	SetBkColor( crDrag );  
	SendMessage( WM_PAINT, (WPARAM)dcAll.GetSafeHdc() );  
	SetBkColor( crBack );  
	CBitmap *pOldDrag = dcDrag.SelectObject( &bmDrag );  
	dcDrag.FillSolidRect( 0, 0, rcAll.Width(), rcAll.Height(), crDrag );  
	CRgn pRgn;  
	if ( bClipped )  
	{  
		CPoint ptMiddle( ptMouse.x - rcAll.left, ptMouse.y - rcAll.top );  
		pRgn.CreateEllipticRgn( ptMiddle.x - MAX_DRAG_SIZE_2, ptMiddle.y - MAX_DRAG_SIZE_2,  
			ptMiddle.x + MAX_DRAG_SIZE_2, ptMiddle.y + MAX_DRAG_SIZE_2 );  
		dcDrag.SelectClipRgn( &pRgn );  
	}  
	for ( nIndex = -1 ; ( nIndex = GetNextItem( nIndex, LVNI_SELECTED ) ) >= 0 ; )  
	{  
		GetItemRect( nIndex, rcOne, LVIR_BOUNDS );  
		if ( rcOne.IntersectRect( &rcAll, &rcOne ) )  
		{  
			dcDrag.BitBlt( rcOne.left - rcAll.left, rcOne.top - rcAll.top,  
				rcOne.Width(), rcOne.Height(), &dcAll, rcOne.left, rcOne.top, SRCCOPY );  
		}  
	}  
	dcDrag.SelectObject( pOldDrag );  
	dcAll.SelectObject( pOldAll );  
	dcDrag.DeleteDC();  
	bmAll.DeleteObject();  
	dcAll.DeleteDC();  
	CImageList* pAll = new CImageList();  
	pAll->Create( rcAll.Width(), rcAll.Height(), ILC_COLOR16|ILC_MASK, 1, 1 );  
	pAll->Add( &bmDrag, crDrag );  
	bmDrag.DeleteObject();  
	pAll->BeginDrag( 0, ptMouse - rcAll.TopLeft() );  
	return pAll;  
}  

void CDragListCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;   
	CPoint ptAction( pNMListView->ptAction );  
	for ( int i = 0; i < MAX_DRAG_SIZE; i++ )  
		m_nDrag[i] = -1;   
	m_pDragImage = CreateDragImage( ptAction );//绘制图像  
	if ( m_pDragImage == NULL ) return;  
	m_nDragDrop = -1;  
	UpdateWindow();  
	CRect rcClient;  
	GetClientRect( &rcClient );  
	ClientToScreen( &rcClient );  
	ClipCursor( &rcClient );  
	SetCapture();  
	SetFocus();  
	UpdateWindow();  
	m_pDragImage->DragEnter( this, ptAction );  
   *pResult = 0;  
}

void CDragListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	int nHit = HitTest( point );  
	if ( m_pDragImage != NULL )  
	{  
		m_pDragImage->DragMove( point );  
		if ( nHit != m_nDragDrop )  
		{    
		   if ( m_nDragDrop >= 0 ) SetItemState( m_nDragDrop, 0, LVIS_DROPHILITED );  
				m_nDragDrop = nHit;  
		   if ( m_nDragDrop >= 0 ) SetItemState( m_nDragDrop, LVIS_DROPHILITED, LVIS_DROPHILITED );  
				UpdateWindow();    
		}
	}
	CListCtrl::OnMouseMove(nFlags, point); 
}

void CDragListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( m_pDragImage == NULL)  
	{  
		CListCtrl::OnLButtonUp( nFlags, point );  
		return;  
	}  
	ClipCursor( NULL );  
	ReleaseCapture();  
	m_pDragImage->DragLeave( this );  
	m_pDragImage->EndDrag();  
	delete m_pDragImage;  
	m_pDragImage = NULL;  
	if ( m_nDragDrop >= 0 )  
	{  
		SetItemState( m_nDragDrop, 0, LVIS_DROPHILITED );  
		//由于本类中只是加了拖动的效果和记录拖动的相关信息，当拖动结束以后并没有实际效果。
		//故需要传递消息 在其父窗口中进行拖动的实际处理  
		NM_LISTVIEW pNotify;  
		pNotify.hdr.hwndFrom = GetSafeHwnd();  
		pNotify.hdr.idFrom   = GetDlgCtrlID();  
		pNotify.hdr.code   = HDN_ENDDRAG;  
		pNotify.iItem    = m_nDragDrop;//拖动到的index  
		pNotify.lParam    = (LPARAM)m_nDrag;//被拖动的Item  
		GetOwner()->SendMessage( WM_NOTIFY, pNotify.hdr.idFrom, (LPARAM)&pNotify );  
	}  
}
