#pragma once
#include "LinkButton.h"

#define IDC_BUTTON_LINK		65534

class CNavigationBar : public CStatic
{
	POINT m_leftTop;	//���������Ͻ�λ��
	int m_nHeight;		//�������߶�
	int m_nWidth;		//���������
	CLinkButton *m_pHeadLinkBtn;
	int m_nLinkBtnCount;

	HCURSOR m_hHandCursor;
	//���ݰ�ť��ʾ�����ּ��Ѵ��ڰ�ť�������°�ť���ھ�������λ��
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
