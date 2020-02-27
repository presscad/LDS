// CG: This file was added by the Splash Screen component.
// Splash.cpp : implementation file
//

#include "stdafx.h"  // e. g. stdafx.h
#include "LDS.h"
#include "XhBitMap.h"
#include "Splash.h"  // e.g. splash.h
#include "XhCharString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

BOOL CSplashWnd::c_bShowSplashWnd;
CSplashWnd* CSplashWnd::c_pSplashWnd;
CSplashWnd::CSplashWnd()
{
}

CSplashWnd::~CSplashWnd()
{
	// Clear the static window pointer.
	ASSERT(c_pSplashWnd == this);
	c_pSplashWnd = NULL;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
	c_bShowSplashWnd = bEnable;
}

void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd /*= NULL*/)
{
	if (!c_bShowSplashWnd || c_pSplashWnd != NULL)
		return;

	// Allocate a new splash screen, and create the window.
	c_pSplashWnd = new CSplashWnd;
	if (!c_pSplashWnd->Create(pParentWnd))
		delete c_pSplashWnd;
	else
		c_pSplashWnd->UpdateWindow();
}
void CSplashWnd::RevokeHideSplashScreen()
{
	if (c_bShowSplashWnd && c_pSplashWnd != NULL)
		c_pSplashWnd->HideSplashScreen();
}

BOOL CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
	if (c_pSplashWnd == NULL)
		return FALSE;

	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
		c_pSplashWnd->HideSplashScreen();
		return TRUE;	// message handled here
	}

	return FALSE;	// message not handled
}

BOOL CSplashWnd::Create(CWnd* pParentWnd /*= NULL*/)
{
	XHBITMAP bm;
	CXhChar50 xarrCompanyName[2];
	int index = 0;
	bool blUdfImageLoaded=false;
	CXhChar200 szWelcomeImageFile,szWelcomeIniFile("%sWelcome.ini",(char*)theApp.szExecPath);
	FILE*  fp=fopen(szWelcomeIniFile,"rt");
	if(fp!=NULL)
	{
		CSelfCloseFile close(fp);
		CXhChar100 szLineText;
		while (fgets(szLineText,100,fp)!=NULL)
		{
			szLineText.Remove(' ');
			szLineText.Remove('\n');
			szLineText.Remove('\t');
			if (szLineText.Length==0)
				continue;
			CXhChar50 xarrWords[2];
			if (DELIMITER_STR::ParseWordsFromStr(szLineText,"=;",xarrWords,2)<2)
				continue;
			if (xarrWords[0].EqualNoCase("image"))
				szWelcomeImageFile.Printf("%s%s",(char*)theApp.szExecPath,(char*)xarrWords[1]);
			else if (xarrWords[0].EqualNoCase("cusotmername"))
			{
				xarrCompanyName[index] = xarrWords[1];
				index++;
			}
		}
		if(szWelcomeImageFile.Length>0&&(blUdfImageLoaded=bm.LoadImgFile(szWelcomeImageFile)))
			m_bitmap.CreateBitmapIndirect(&bm);
	}
	if(!blUdfImageLoaded)
	{
#ifdef __TMA_
		if (!m_bitmap.LoadBitmap(IDB_SPLASH_TMA))
			return FALSE;
#elif defined(__LMA_)
		if (!m_bitmap.LoadBitmap(IDB_SPLASH_LMA))
			return FALSE;
#elif defined(__LDS_)
		if (!m_bitmap.LoadBitmap(IDB_SPLASH_LDS))
			return FALSE;
#elif defined(__TDA_)
		if (!m_bitmap.LoadBitmap(IDB_SPLASH_TDA))
			return FALSE;
#else
		if (!m_bitmap.LoadBitmap(IDB_SPLASH_LMA))
			return FALSE;
#endif
		m_bitmap.GetBitmap(&bm);
	}
#ifdef __LDS_
	if (!blUdfImageLoaded)
	{
		CDC dc;
		dc.CreateCompatibleDC(NULL);
		CBitmap* pOldObj=dc.SelectObject(&m_bitmap);
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(255,255,255));

		LOGFONT logfont;
		logfont.lfHeight = 12;
		logfont.lfWidth = 0;
		logfont.lfEscapement = 0;
		logfont.lfOrientation = 0;
		logfont.lfWeight = 500;
		logfont.lfItalic = 0;
		logfont.lfUnderline=0;
		logfont.lfStrikeOut = 0;
		logfont.lfCharSet = 0;
		logfont.lfOutPrecision = 0;
		logfont.lfClipPrecision = 0;
		logfont.lfQuality = 0;
		logfont.lfPitchAndFamily = 34;
		StrCopy(logfont.lfFaceName,"宋体",30);
		//font.l
		CFont font;
		font.CreateFontIndirect(&logfont);
		dc.SelectObject(&font);
		xarrCompanyName[index] = "北京信狐天诚软件科技有限公司";
		POINT xarrLocation[2] ={ {308,215},{308,230} };
		for (index = 0; index < 2; index++)
		{
			if (xarrCompanyName[index].Length > 0)
				dc.TextOut(xarrLocation[index].x,xarrLocation[index].y,CString(xarrCompanyName[index]));
		}
		dc.SelectObject(pOldObj);
	}
#endif
	return CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), NULL);
}

void CSplashWnd::HideSplashScreen()
{
	// Destroy the window, and update the mainframe.
	KillTimer(1);
	DestroyWindow();
	CWnd* pMainWnd=AfxGetMainWnd();
	if(pMainWnd)
		pMainWnd->UpdateWindow();
}

void CSplashWnd::PostNcDestroy()
{
	// Free the C++ class.
	delete this;
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Center the window.
	CenterWindow();

	// Set a timer to destroy the splash screen.
	SetTimer(1, 750, NULL);

	return 0;
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);

	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(&dc))
		return;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);
}

void CSplashWnd::OnTimer(UINT nIDEvent)
{
	// Destroy the splash screen window.
	HideSplashScreen();
}
