// LinkButton.cpp : implementation file
//

#include "stdafx.h"
#include ".\linkbutton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinkButton

CLinkButton::CLinkButton()
{
	m_bHighlight = m_bLBtnDown = false;
	m_hHandCursor = 0;
	m_pPrevBtn=NULL;
	m_pNextBtn=NULL;
	strcpy(m_sName,"");
}

CLinkButton::~CLinkButton()
{
	if (m_fontUnderline.GetSafeHandle()) 
		m_fontUnderline.DeleteObject();
}


BEGIN_MESSAGE_MAP(CLinkButton, CButton)
	//{{AFX_MSG_MAP(CLinkButton)
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkButton message handlers

void CLinkButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// 获取一个CDC指针
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	//定义按钮区域并初始化
	CRect rect(lpDrawItemStruct->rcItem);
	//设置背景模式
	COLORREF oc = pDC->GetTextColor();
	int iObk = pDC->SetBkMode(TRANSPARENT);
	//初始化按钮状态
	UINT state = lpDrawItemStruct->itemState;

	CFont * pOldFont = NULL;
	int iYOffset = 0, iXOffset = 0;
	CString strText;
	GetWindowText(strText);

	rect.top  += iYOffset;
	rect.left += iXOffset;

	if (state & ODS_DISABLED)
	{		
		//按钮置灰（DISABLED）
		CBrush grayBrush;
		grayBrush.CreateSolidBrush (GetSysColor (COLOR_GRAYTEXT));
		CSize sz = pDC->GetTextExtent(strText);
		int x = rect.left + (rect.Width() - sz.cx)/2;
		int y = rect.top + (rect.Height() - sz.cy)/2;
		rect.top  += 2;
		rect.left += 2;
		pDC->SetTextColor(GetSysColor(COLOR_3DHIGHLIGHT));
		pDC->DrawText(strText, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		rect.top  -= 2;
		rect.left -= 2;
		pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		pDC->DrawText(strText, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	else
	{
		if (m_bHighlight)//光标在按钮上
		{
			if(state & ODS_SELECTED)	//按下按钮
				pDC->Draw3dRect(rect,GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
			else//未按下按钮
				pDC->Draw3dRect(rect,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_3DSHADOW));
			//字体颜色
			pDC->SetTextColor(RGB(0,0,255));
			//加下画线（也可以用其他字体）
			if(m_fontUnderline.GetSafeHandle())
				pOldFont = pDC->SelectObject(&m_fontUnderline);
		}
		else pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));

		pDC->DrawText(strText, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		if (pOldFont) 
			pDC->SelectObject(pOldFont);
	}
}


void CLinkButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	//设置一个定时器
	SetTimer(1,10,NULL);
	
	CButton::OnMouseMove(nFlags, point);
}

BOOL CLinkButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_bHighlight) 
	{
		::SetCursor(m_hHandCursor);
		return true;
	}
	
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

void CLinkButton::OnTimer(UINT nIDEvent) 
{
	static bool pPainted = false;
	POINT pt;
	GetCursorPos(&pt);
	CRect rect;
	GetWindowRect (rect);
	if (m_bLBtnDown)	
	{		
		KillTimer (1);
		if (pPainted) InvalidateRect (NULL);		
		pPainted = FALSE;		
		return;	
	}

	if (!rect.PtInRect (pt))	
	{		
		m_bHighlight = false;
		KillTimer (1);

		if (pPainted)			
			InvalidateRect(NULL);

		pPainted = false;
		return;	
	}
	else
	{
		m_bHighlight = true;
		if (!pPainted)
		{
			pPainted = true;
			InvalidateRect(NULL);
		}
	}

	CButton::OnTimer(nIDEvent);
}

int CLinkButton::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CLinkButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = false;
	if (m_bHighlight)	
	{
		m_bHighlight = false;
		InvalidateRect(NULL);
	}
	
	CButton::OnLButtonUp(nFlags, point);
}

void CLinkButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = true;
	
	CButton::OnLButtonDown(nFlags, point);
}

BOOL CLinkButton::OnEraseBkgnd(CDC* pDC) 
{
	COLORREF cr = GetSysColor(COLOR_3DFACE);
	int r = GetRValue(cr);
	int g = GetGValue(cr);
	int b = GetBValue(cr);
	if (r > 1) r -= 2;
	if (g > 1) g -= 2;
	if (r < 3 && g < 3 && b < 253) b += 2;
	COLORREF cr1 = RGB(r,g,b);
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect(rc, cr1);
	
	return CButton::OnEraseBkgnd(pDC);
}

void CLinkButton::PreSubclassWindow()
{
	if(m_fontUnderline.GetSafeHandle()==NULL)
	{
		CFont * pFont = GetFont();
		if(pFont)
		{
			ASSERT(pFont);
			LOGFONT lf;
			pFont->GetLogFont(&lf);
			lf.lfUnderline = TRUE;
			m_fontUnderline.CreateFontIndirect(&lf);
		}
	}
	CButton::PreSubclassWindow();
}
