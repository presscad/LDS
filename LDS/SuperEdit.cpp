// SuperEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SuperEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSuperEdit

CSuperEdit::CSuperEdit()
{
}

CSuperEdit::~CSuperEdit()
{
}


BEGIN_MESSAGE_MAP(CSuperEdit, CEdit)
	//{{AFX_MSG_MAP(CSuperEdit)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuperEdit message handlers


BOOL CSuperEdit::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )	
	{		
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{			
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			GetParent()->SetFocus();
			return 1;
		}	
	}	
	return CEdit::PreTranslateMessage(pMsg);
}
