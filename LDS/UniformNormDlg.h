#pragma once


// CUpdateNormDLg 对话框

class CUniformNormDLg : public CDialog
{
	DECLARE_DYNAMIC(CUniformNormDLg)

public:
	CUniformNormDLg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUniformNormDLg();

// 对话框数据
	enum { IDD = IDD_UNIFY_NORM_DLG };
public:
	f3dPoint norm;
	CBitmap m_bitMap;
public:
	int m_iNormStyle;
	double m_fVectorX;
	double m_fVectorY;
	double m_fVectorZ;
public:
	void UpdateMap();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeCmbNormStyle();
};
