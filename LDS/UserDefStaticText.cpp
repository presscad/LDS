// UserDefStaticText.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "UserDefStaticText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserDefStaticText

CUserDefStaticText::CUserDefStaticText()
{
}

CUserDefStaticText::~CUserDefStaticText()
{
}


BEGIN_MESSAGE_MAP(CUserDefStaticText, CStatic)
	//{{AFX_MSG_MAP(CUserDefStaticText)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserDefStaticText message handlers

void CUserDefStaticText::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rectFull;
	GetClientRect(&rectFull);
	dc.SetBkColor (RGB(255,255,255));
	dc.SetBkMode(OPAQUE);
	dc.FillSolidRect(&rectFull,RGB(255,255,255));
	CString str;
	GetWindowText(str);
	CFont* pFont = new CFont();
	pFont->CreateFont(
		12,							// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Tahoma");                 // lpszFacename
	CFont* pOldFont = dc.SelectObject(pFont);	
	dc.DrawText(str,&rectFull,DT_LEFT | DT_SINGLELINE);
	dc.SelectObject(pOldFont);
	pFont->DeleteObject();
	// Do not call CStatic::OnPaint() for painting messages
}

void CUserDefStaticText::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
}
