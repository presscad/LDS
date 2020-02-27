#pragma once


// CPlateNcSetDlg 对话框

class CPlateNcSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CPlateNcSetDlg)

public:
	CPlateNcSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlateNcSetDlg();

// 对话框数据
	enum { IDD = IDD_PLATE_NC_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	int m_iRollOffsetType;
	int m_iDeformType;
};
