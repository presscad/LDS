#pragma once


// CNailResideAnglePage 对话框

class CNailResideAnglePage : public CDialog
{
	DECLARE_DYNAMIC(CNailResideAnglePage)

public:
	CWnd* m_pParentWnd;	//父窗口指针
	JGZJ xWingXZhunJu,xWingYZhunJu;
	CNailResideAnglePage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNailResideAnglePage();

// 对话框数据
	enum { IDD = IDD_NAIL_RESIDE_ANGLE_PAGE };
	int		m_iLayWing;
	int		m_iGType;	// 所在准距的选择类型:0,标准准距g,1,2,3,分别表示g1,g2,g3,4表示自定义
	long	m_nGDist;	// 用户自定义的脚钉准距
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK(){;}		//防止回车键或ESC键关闭对话框
	virtual void OnCancel(){;}
public:
	void OnRdoLayWing();
	void OnCbnSelchangeCmbGType();
	void OnChangeGDist();

	DECLARE_MESSAGE_MAP()
};
