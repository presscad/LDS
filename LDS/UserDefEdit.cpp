// UserDefEdit.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "UserDefEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserDefEdit

CUserDefEdit::CUserDefEdit()
{
}

CUserDefEdit::~CUserDefEdit()
{
}


BEGIN_MESSAGE_MAP(CUserDefEdit, CEdit)
	//{{AFX_MSG_MAP(CUserDefEdit)
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserDefEdit message handlers

void CUserDefEdit::OnNcPaint() 
{
	// TODO: Add your message handler code here
	
	// Do not call CEdit::OnNcPaint() for painting messages
}

void CUserDefEdit::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	
	
	// Do not call CEdit::OnPaint() for painting messages
}

void CUserDefEdit::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	CEdit::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

HBRUSH CUserDefEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return NULL;
}
