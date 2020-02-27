// XhListBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "XhListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXhListBox

CXhListBox::CXhListBox()
{
	crColor=RGB(0,0,0);
}

CXhListBox::~CXhListBox()
{
}


BEGIN_MESSAGE_MAP(CXhListBox, CListBox)
	//{{AFX_MSG_MAP(CXhListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXhListBox message handlers

void CXhListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC); 
	CRect rect;
	rect.CopyRect(&lpDrawItemStruct-> rcItem); 
	pDC-> SetBkMode(TRANSPARENT); 
	if(lpDrawItemStruct-> itemState&ODS_SELECTED) 
	{ 
		pDC-> FillSolidRect(rect,GetSysColor(COLOR_HIGHLIGHT)); 
		pDC-> SetTextColor(crColor); 
	} 
	else 
	{ 
		pDC-> FillSolidRect(rect,GetSysColor(COLOR_WINDOW)); 
		pDC-> SetTextColor(crColor); 
	}
	CString sText; 
	GetText(lpDrawItemStruct->itemID,sText);
	GetItemData(lpDrawItemStruct-> itemID);
	pDC-> DrawText(sText,rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE); 
}
