#include "stdafx.h"
#include ".\navigationbar.h"

CNavigationBar::CNavigationBar(void)
{
	m_pHeadLinkBtn=NULL;
	m_leftTop.x=0;
	m_leftTop.y=0;
	m_nHeight=15;
	m_nWidth=100;
	m_nLinkBtnCount=0;
	FireButtonClick=NULL;
}

CNavigationBar::~CNavigationBar(void)
{
	Empty();
}

//根据按钮显示的文字及已存在按钮，计算新按钮所在矩形区域位置
CRect CNavigationBar::GetNewBtnRect(int nTextLen)
{
	CRect rect;
	rect.top=0;
	rect.left=0;
	CLinkButton *pBtn=m_pHeadLinkBtn;
	while(pBtn)
	{
		CRect btnRect;
		pBtn->GetWindowRect(btnRect);
		rect.left+=btnRect.Width();
		if(rect.left>(m_leftTop.x+m_nWidth))
		{	//换行
			rect.left=0;
			rect.top+=m_nHeight;
		}
		pBtn=pBtn->m_pNextBtn;
	}
	rect.right=rect.left+nTextLen;
	rect.bottom=rect.top+m_nHeight;
	return rect;
}

CLinkButton *CNavigationBar::AddLinkButton(char *sText,DWORD dwTag/*=0*/)
{
	CLinkButton *pButton = new CLinkButton();
	m_nLinkBtnCount++;
	ASSERT_VALID(pButton);
	strcpy(pButton->m_sName,sText);
	pButton->m_dwTag=dwTag;
	pButton->m_hHandCursor=m_hHandCursor;
	CRect rect=GetNewBtnRect(strlen(pButton->m_sName)*8);
	pButton->Create(pButton->m_sName, BS_OWNERDRAW|WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON , rect, this, IDC_BUTTON_LINK); //创建按钮
	CFont *pFont=pButton->GetFont();
	if(m_pHeadLinkBtn==NULL)
		m_pHeadLinkBtn=pButton;
	else 
	{
		CLinkButton *pTailBtn=m_pHeadLinkBtn;
		while (pTailBtn)
		{
			if(pTailBtn->m_pNextBtn==NULL)
				break;
			pTailBtn=pTailBtn->m_pNextBtn;
		}
		pTailBtn->m_pNextBtn=pButton;
	}
	return pButton;
}

BOOL CNavigationBar::RemoveBtnsAfterCurBtn(CLinkButton *pCurBtn)
{
	if(pCurBtn==NULL||pCurBtn->m_pNextBtn==NULL)
		return FALSE;
	CLinkButton *pNextBtn=pCurBtn->m_pNextBtn;
	while (pNextBtn)
	{
		CLinkButton *pDelBtn=pNextBtn;
		pNextBtn=pNextBtn->m_pNextBtn;
		delete pDelBtn;
	}
	pCurBtn->m_pNextBtn=NULL;
	return TRUE;
}

void CNavigationBar::SetButtonHandleCursor(HCURSOR hHandCursor)
{
	m_hHandCursor=hHandCursor;
}

void CNavigationBar::Empty()
{
	while (m_pHeadLinkBtn)
	{
		CLinkButton *pDelBtn=m_pHeadLinkBtn;
		m_pHeadLinkBtn=m_pHeadLinkBtn->m_pNextBtn;
		delete pDelBtn;
	}
}
void CNavigationBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	/*pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(255,255,255));
	CBrush brush;
	brush.CreateSolidBrush(RGB(255,255,255));
	CBrush *pOldBrush = pDC->SelectObject(&brush); 
	CRect rect;
	GetClientRect(&rect); 
	pDC->Rectangle(&rect);  
	if(pOldBrush)
		pDC->SelectObject(pOldBrush);*/
}
BEGIN_MESSAGE_MAP(CNavigationBar, CStatic)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_LINK, OnClickLinkButton)
END_MESSAGE_MAP()

HBRUSH CNavigationBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CStatic::OnCtlColor(pDC, pWnd, nCtlColor);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(255,255,255));	
	return hbr;
}

void CNavigationBar::OnClickLinkButton()
{
	CPoint pos;
	GetCursorPos(&pos);
	CLinkButton *pCurBtn=NULL,*pBtn=m_pHeadLinkBtn;
	while(pBtn)
	{
		CRect btnRect;
		pBtn->GetWindowRect(btnRect);
		if(btnRect.PtInRect(pos))
		{
			pCurBtn=pBtn;
			break;
		}
		pBtn=pBtn->m_pNextBtn;
	}
	if(pCurBtn&&pCurBtn->m_pNextBtn)
	{
		RemoveBtnsAfterCurBtn(pCurBtn);
		if(FireButtonClick)	//通过回调函数刷新显示数据
			FireButtonClick(pCurBtn);
	}

}

