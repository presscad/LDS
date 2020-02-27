#pragma once


// CSelAssotiatedBoltsDlg 对话框

class CSelAssotiatedBoltsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelAssotiatedBoltsDlg)

public:
	CSelAssotiatedBoltsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelAssotiatedBoltsDlg();

// 对话框数据
	enum { IDD = IDD_SEL_ASSOTIATED_BOLTS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bOrdinaryBolt;
	BOOL m_bFootnailBolt; 
public:
	virtual BOOL OnInitDialog();
};
