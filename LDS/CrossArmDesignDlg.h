#pragma once


// CCrossArmDesignDlg 对话框
#include "PropertyList.h"
#include "block.h"
class CCrossArmDesignDlg : public CDialog
{
	DECLARE_DYNAMIC(CCrossArmDesignDlg)
public:
	CParaBlockModel* m_pModel;
	CString	m_sKeyNodeOrder;
	long	m_nKeyNodeNum;	//关键点数量
	int		m_iKeyNodeMir;	//关键点对称
	int		m_iStdFaceMir;	//模版面对称
	CRect m_rectInitWndRgn;		//当前对话框的初始矩形（屏幕坐标）
public:
	CCrossArmDesignDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCrossArmDesignDlg();
	void DisplayPropertyList();
	void UpdateFaceDetailGraph();	//绘图区绘制示意图
	int GetPropValueStr(int idProp,char* valueStr);
	void ChangeKeyNodeNum();

// 对话框数据
	enum { IDD = IDD_CROSSARM_DESIGN_DLG };
	CPropertyList	m_propList;
	int m_iCrossArmType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeCmbCrossArmType();
	DECLARE_MESSAGE_MAP()
};
