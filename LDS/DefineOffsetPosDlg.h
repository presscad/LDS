#pragma once
#include "afxwin.h"


// CDefineOffsetPos 对话框

class CDefineOffsetPosDlg : public CDialog
{
	DECLARE_DYNAMIC(CDefineOffsetPosDlg)

public:
	CDefineOffsetPosDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDefineOffsetPosDlg();

// 对话框数据
	enum { IDD = IDD_DEFINE_OFFSET_POS_DLG };
public:
	f3dPoint offset_vector;
	f3dPoint offset_pos;
	f3dPoint datum_pos;
	bool m_bDirectDefPos;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnOk();
	afx_msg void OnCancel();
private:
	double m_fPostionX;
	double m_fPostionY;
	double m_fPostionZ;
	double m_fOffsetX;
	double m_fOffsetY;
	double m_fOffsetZ;
public:
	afx_msg void OnEnKillfocusEXOffset();
	afx_msg void OnEnKillfocusEYOffset();
	afx_msg void OnEnKillfocusEZOffset();
};
