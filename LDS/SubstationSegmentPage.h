#pragma once
#include "supergridctrl.h"


// CSubstationBeamTemplate �Ի���
class CSubstationBeamSegmentPage : public CDialogEx
{
	DECLARE_DYNAMIC(CSubstationBeamSegmentPage)

public:
	struct SECTION{
		double space;	//�ڼ䵵��
		char   sLayout[16];	//������ʽ,���һ���ֽڲ�Ϊ��ʱ,��Ȼ��Ч
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
	CSubstationBeamSegmentPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSubstationBeamSegmentPage();
	//
	CSuperGridCtrl* GetFocusListCtrl();
	CSuperGridCtrl* GetActiveListCtrl();
	void RefreshRCBeamListCtrl();
	void RefreshRSBeamListCtrl();
	void RefreshListCtrl(int iActiveCtrl=0);
	void RefreshBeamLenEdit();
	void ExtractBeamSegment();
// �Ի�������
	enum { IDD = IDD_SUBSTATION_BEAM_SEGMENT };
	
	double m_fWidth1;			//���տڿ��
	double m_fWidth2;			//ֱ�ڿ��
	double m_fWidth3;			//���տڿ��
	double m_fLeftBeamLength;	//����������
	double m_fRightBeamLength;	//����������
	double m_fBeamHeight;		//�����߶�
	int m_iPoleType;		//�ڼ�˼�����
	double m_fLSBeamLen;
	double m_fSBeamLen;		//ֱ������
	double m_fSpanLen;		//�ܳ���
	CSuperGridCtrl m_listCtrlLCBeamSeg;
	CSuperGridCtrl m_listCtrlRCBeamSeg;
	CSuperGridCtrl m_listCtrlLSBeamSeg;
	CSuperGridCtrl m_listCtrlRSBeamSeg;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
