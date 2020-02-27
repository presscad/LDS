#pragma once

#include "CallBackDialog.h"
#include "supergridctrl.h"
#include "PropertyList.h"
#include "ElectricTower.h"

// CInputWirePointDlg 对话框
class CInputWirePointDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CInputWirePointDlg)
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX, nScrLocationY;
public:
	CInputWirePointDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CInputWirePointDlg();
	//
	void DisplayProperty();
	void FinishSelectObjOper();		//完成选择对象的后续操作
	void RefreshListCtrl();
	void UpdateUniWireModel();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT_WP_DLG };
#endif
	CSuperGridCtrl m_listCtrl;
	CPropertyList	m_propList;
public:
	CXhWirePoint m_curWriePt;
	CXhWirePoint* m_pSelWirePt;
	CHashListEx<CXhWirePoint> m_hashWirePoint;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnImportWireModel();
	afx_msg void OnBnExportWireModel();
};
