#pragma once
#include "supergridctrl.h"


// CSubstationBeamTemplate 对话框
class CSubstationBeamSegmentPage : public CDialogEx
{
	DECLARE_DYNAMIC(CSubstationBeamSegmentPage)

public:
	struct SECTION{
		double space;	//节间档距
		char   sLayout[16];	//布材型式,最后一个字节不为零时,依然有效
		SECTION(){memset(this,0,sizeof(SECTION));}
		SECTION(double _space,const char* layout_str);
		const char* LayoutStr();
	};
	static const BYTE SECT_LENGTH;
	static const BYTE FRONT_PANEL;
	static const BYTE BOTTOM_PANEL;
	ARRAY_LIST<SECTION> m_xLCBeamSegArr;
	ARRAY_LIST<SECTION> m_xRCBeamSegArr;
	ARRAY_LIST<SECTION> m_xLSBeamSegArr;
	ARRAY_LIST<SECTION> m_xRSBeamSegArr;
	ARRAY_LIST<SECTION> m_xLTBeamSegArr;
	ARRAY_LIST<SECTION> m_xRTBeamSegArr;
	int m_nCBeamNum;
	int m_nSBeamNum;
	int m_nRightMargin;
	int m_iActiveListCtrl;
	CRect m_xDrawRect;
public:
	CSubstationBeamSegmentPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSubstationBeamSegmentPage();
	//
	CSuperGridCtrl* GetFocusListCtrl();
	CSuperGridCtrl* GetActiveListCtrl();
	void RefreshRCBeamListCtrl();
	void RefreshRSBeamListCtrl();
	void RefreshListCtrl(int iActiveCtrl=0);
	void RefreshBeamLenEdit();
	void ExtractBeamSegment();
// 对话框数据
	enum { IDD = IDD_SUBSTATION_BEAM_SEGMENT };
	
	double m_fWidth1;			//左收口宽度
	double m_fWidth2;			//直口宽度
	double m_fWidth3;			//右收口宽度
	double m_fLeftBeamLength;	//左曲梁长度
	double m_fRightBeamLength;	//右曲梁长度
	double m_fBeamHeight;		//顶梁高度
	int m_iPoleType;		//节间杆件类型
	double m_fLSBeamLen;
	double m_fSBeamLen;		//直梁长度
	double m_fSpanLen;		//总长度
	CSuperGridCtrl m_listCtrlLCBeamSeg;
	CSuperGridCtrl m_listCtrlRCBeamSeg;
	CSuperGridCtrl m_listCtrlLSBeamSeg;
	CSuperGridCtrl m_listCtrlRSBeamSeg;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNewItem();
	afx_msg void OnDeleteItem();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnEnChangeEWidth1();
	afx_msg void OnEnChangeEWidth2();
	afx_msg void OnEnChangeEWidth3();
	afx_msg void OnEnChangeETopbeamH();
	afx_msg void OnEnChangeOtherParam();
};
