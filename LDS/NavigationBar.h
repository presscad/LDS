#pragma once
#include "LinkButton.h"

#define IDC_BUTTON_LINK		65534

class CNavigationBar : public CStatic
{
	POINT m_leftTop;	//导航栏左上角位置
	int m_nHeight;		//导航栏高度
	int m_nWidth;		//导航栏宽度
	CLinkButton *m_pHeadLinkBtn;
	int m_nLinkBtnCount;

	HCURSOR m_hHandCursor;
	//根据按钮显示的文字及已存在按钮，计算新按钮所在矩形区域位置
	CRect GetNewBtnRect(int nTextLen);
	BOOL (*FireButtonClick)(CLinkButton *pBtn);
public:
	CNavigationBar(void);
	~CNavigationBar(void);
	
	void SetButtonHandleCursor(HCURSOR hHandCursor);
	CLinkButton *AddLinkButton(char *sText,DWORD dwTag=0);
	BOOL RemoveBtnsAfterCurBtn(CLinkButton *pCurBtn);
	void Empty();
	void SetLeftTop(int nLeft,int nTop){m_leftTop.x=nLeft;m_leftTop.y=nTop;}
	void SetWidth(int nWidth){m_nWidth=nWidth;}
	void SetHeight(int nHeight){m_nHeight=nHeight;}
	void SetFireButtonClick(BOOL (*func)(CLinkButton *)){FireButtonClick=func;}
	//
	void OnClickLinkButton();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
