#pragma once


// CSubstationSquareBeamPage 对话框

class CSubstationSquareBeamPage : public CDialogEx
{
	DECLARE_DYNAMIC(CSubstationSquareBeamPage)

public:
	CSubstationSquareBeamPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSubstationSquareBeamPage();

// 对话框数据
	enum { IDD = IDD_SUBSTATION_PANEL_SQUARE_BEAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_fBeamWidth;			//方形梁宽度
	int m_fBeamHeight;			//方形梁高度
	int m_nInternodeNum;		//方形梁节间数
	int m_iPoleType;			//节间杆件类型
};
