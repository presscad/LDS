#pragma once
#include "afxwin.h"


// CSubstationGeneralPage 对话框

class CSubstationGeneralPage : public CDialogEx
{
	DECLARE_DYNAMIC(CSubstationGeneralPage)

public:
	CSubstationGeneralPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSubstationGeneralPage();

// 对话框数据
	enum { IDD = IDD_SUBSTATION_PANEL_DESCRIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nSpanNum;		// 跨数
	double m_fBeamHeight;	//梁垂高
	double m_fPostHeight;	//柱垂高
	double m_fDL;		//柱到梁的距离
	int m_iPostType;	//0.人字架
	int m_iBeamType;	//0.三角梁 1.四方两
	BOOL m_bStartSup;	//始端支撑
	BOOL m_bEndSup;		//终端支撑
	BOOL m_bDesignPost;
	BOOL m_bDesignBeam;
	afx_msg void OnBnClickedChkDesignPost();
	afx_msg void OnBnClickedChkDesignBeam();
};
