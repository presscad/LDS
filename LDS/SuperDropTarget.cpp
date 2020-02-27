// SuperDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "SuperDropTarget.h"
#include "TowerTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSuperDropTarget

//IMPLEMENT_DYNCREATE(CSuperDropTarget, CCmdTarget)

CSuperDropTarget::CSuperDropTarget()
{
}

CSuperDropTarget::~CSuperDropTarget()
{
}


BEGIN_MESSAGE_MAP(CSuperDropTarget, COleDropTarget)
	//{{AFX_MSG_MAP(CSuperDropTarget)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

DROPEFFECT CSuperDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CTowerTreeCtrl* pTreeCtrl = NULL;
	
	ASSERT_VALID(this);
	ASSERT(IsWindow(pWnd->m_hWnd));
	
	if(pWnd->IsKindOf(RUNTIME_CLASS(CTowerTreeCtrl)))
	{
		pTreeCtrl = (CTowerTreeCtrl*)pWnd;
		return pTreeCtrl->OnDragEnter(pWnd, pDataObject, dwKeyState, point);
	}
	else
	{
		return COleDropTarget::OnDragEnter(pWnd, pDataObject, dwKeyState, point);      
	}
}

DROPEFFECT CSuperDropTarget::OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point)
{
	CTowerTreeCtrl* pTreeCtrl = NULL;
	
	ASSERT_VALID(this);
	ASSERT(IsWindow(pWnd->m_hWnd));
	
	if(pWnd->IsKindOf(RUNTIME_CLASS(CTowerTreeCtrl)))
	{
		pTreeCtrl = (CTowerTreeCtrl*)pWnd;
		return pTreeCtrl->OnDropEx(pWnd, pDataObject, dropDefault, dropList, point);
	}
	else
	{
		return COleDropTarget::OnDropEx(pWnd, pDataObject, dropDefault, dropList, point);      
	}
}

DROPEFFECT CSuperDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CTowerTreeCtrl* pTreeCtrl = NULL;
	
	ASSERT_VALID(this);
	ASSERT(IsWindow(pWnd->m_hWnd));
	
	if(pWnd->IsKindOf(RUNTIME_CLASS(CTowerTreeCtrl)))
	{
		pTreeCtrl = (CTowerTreeCtrl*)pWnd;
		return pTreeCtrl->OnDragOver(pWnd, pDataObject, dwKeyState, point);
	}
	else
	{
		return COleDropTarget::OnDragOver(pWnd, pDataObject, dwKeyState, point);      
	}
}
BOOL CSuperDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	CTowerTreeCtrl* pTreeCtrl = NULL;
	
	ASSERT_VALID(this);
	ASSERT(IsWindow(pWnd->m_hWnd));
	
	if(pWnd->IsKindOf(RUNTIME_CLASS(CTowerTreeCtrl)))
	{
		pTreeCtrl = (CTowerTreeCtrl*)pWnd;
		return pTreeCtrl->OnDrop(pWnd, pDataObject, dropEffect, point);
	}
	else
	{
		return COleDropTarget::OnDrop(pWnd, pDataObject, dropEffect, point);      
	}	
}
/////////////////////////////////////////////////////////////////////////////
// CSuperDropTarget message handlers
