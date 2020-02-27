#pragma once


// CCutAngleCheckTypeDlg 对话框

class CCutAngleCheckTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CCutAngleCheckTypeDlg)

public:
	CCutAngleCheckTypeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCutAngleCheckTypeDlg();
// 对话框数据
	enum { IDD = IDD_CUT_ANGLE_TYPE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL OnInitDialog();
	afx_msg void OnBnClickedChkAngleBolt();
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bCheckRayAndRay;
	BOOL m_bCheckMainAndRay;
	BOOL m_bCheckAngleAndBolt;
	int m_iBoltDetectType;
	double m_fClearance;
};
