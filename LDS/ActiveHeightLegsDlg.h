#pragma once
#include "LinkLabel.h"

// CActiveHeightLegsDlg 对话框

class CActiveHeightLegsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CActiveHeightLegsDlg)

public:
	CActiveHeightLegsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CActiveHeightLegsDlg();
public:	//启动对话框前的呼高输入参数
	short m_siInitLegHeight;	//A级减腿的相对于基准呼高的高差(单位mm)，一般为0，极少存在+1.5m情况
	short m_siLegLevelDiff;		//相邻等级减腿的高差(单位mm, >0)
	DWORD m_dwIncLegItemFlag;	//当前呼高包含的减腿范围
public:	//对话框关闭后从窗口上获取的用户设定参数（同时也为输入参数）
	char xarrActiveQuadLegs[4];	//1,2,3,4象限的激活减腿简化字符

// 对话框数据
	enum { IDD = IDD_ACTIVE_HEIGHT_LEGS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnActivate();
public:
	virtual BOOL OnInitDialog();
public:
	CColorLabel m_ctrlMasterLegLabel;
	int m_iOrderOfLegQuadA2;
	int m_iOrderOfLegQuadB4;
	int m_iOrderOfLegQuadC3;
	int m_iOrderOfLegQuadD1;
	double m_fLegLevelDiff;	//相邻减腿高度级差
	CString m_sHeightName;
	CString m_sLegItems;
	CString m_sLegLevelDiff;
	afx_msg void OnCbnSelchangeCmbLegQuadA2();
};
