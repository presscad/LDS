#pragma once


// CSubstationBeamPage 对话框

class CSubstationBeamPage : public CDialogEx
{
	DECLARE_DYNAMIC(CSubstationBeamPage)

public:
	CSubstationBeamPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSubstationBeamPage();

// 对话框数据
	enum { IDD = IDD_SUBSTATION_PANEL_BEAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_fWidth1;			//弯口宽度
	int m_fWidth2;			//直口宽度
	int m_fLeftBeamLength;	//左曲梁长度
	int m_fRightBeamLength;	//右曲梁长度
	int m_fBeamHeight;		//顶梁高度
	int m_nInternodeNum1;	//曲梁节间数
	int m_nInternodeNum2;	//直梁节间数
	int m_iPoleType;		//节间杆件类型
	int m_nRightBeamInternodeCount;
	double m_fDlWidth;
	afx_msg void OnEnChangeELeftBeamLen();
};
