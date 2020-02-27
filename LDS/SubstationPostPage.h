#pragma once


// CSubstationPostPage 对话框

class CSubstationPostPage : public CDialogEx
{
	DECLARE_DYNAMIC(CSubstationPostPage)

public:
	CSubstationPostPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSubstationPostPage();

// 对话框数据
	enum { IDD = IDD_SUBSTATION_PANEL_POST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_fTopWidth;		//人字架顶部宽度
	int m_fBtmWidth;		//人字架底部宽度
	int m_fLinkHigh;		//人字架连接高度
	int m_fTopSideWidth;	//人字架侧撑顶部宽度
	int m_fBtmSideWidth;	//人字架侧撑底部宽度
};
