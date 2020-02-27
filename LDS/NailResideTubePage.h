#pragma once


// CNailResideTubePage 对话框

class CNailResideTubePage : public CDialog
{
	DECLARE_DYNAMIC(CNailResideTubePage)

public:
	CWnd* m_pParentWnd;	//父窗口指针
	CNailResideTubePage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNailResideTubePage();

// 对话框数据
	enum { IDD = IDD_NAIL_RESIDE_TUBE_PAGE };
	int m_iLaySide;
	int m_iNailType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK(){;}		//防止回车键或ESC键关闭对话框
	virtual void OnCancel(){;}
	void OnRdoLaySide();
	void OnRdoNailType();

	DECLARE_MESSAGE_MAP()
public:
};
