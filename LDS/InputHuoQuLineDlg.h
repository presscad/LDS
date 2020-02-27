#pragma once


// CInputHuoQuLineDlg 对话框
#include "CallBackDialog.h"
#include "NativePropertyList.h"

class CInputHuoQuLineDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CInputHuoQuLineDlg)

public:
	CInputHuoQuLineDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInputHuoQuLineDlg();

	void InitHuoquLineInfo(BOOL bInitDefaultInfo=TRUE);
	void RefreshPropertyListCtrl();
	void UpdateUIStatus();
// 对话框数据
	enum { IDD = IDD_INPUT_HUOQULINE_DLG };
	CParaPointPropertyList m_propStartPtList;
	CParaPointPropertyList m_propEndPtList;
	CParaVectorPropertyList m_propVectorList;
	CLDSLinePart *m_pCrossRod;
	CTabCtrl m_ctrlPropGroup;
	CString	m_sJgHandle;		//基准角钢句柄
	double m_fWingOffsetDist;	//肢向偏移量
	int m_iWingOffsetType;		//肢向偏移类型
	int m_iOffsetWing;			//偏移肢（X/Y）
	int m_iCurSel;
	//
	CLDSPoint m_xStartPt;
	CLDSPoint m_xEndPt;
	CLDSVector m_xVector;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTcnSelchangeTabGroup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSelectPart();
	afx_msg void OnInitHuoQuLineInfo();
};
