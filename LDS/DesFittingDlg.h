#pragma once

#include "PropertyList.h"
#include "PropListItem.h"
#include "CallBackDialog.h"

//////////////////////////////////////////////////////////////////////////
// CDesFittingDlg 对话框
class CDesFittingDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesFittingDlg)
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
	CBitmap m_xCurPlateBmp;
public:
	BYTE m_ciType;
	SEGI m_iSeg;
	UCS_STRU acs;
	BOOL m_bNewFitting;
	CLDSFitting xPreviewFitting;
	CLDSFitting *m_pFitting;
	CLDSBolt* m_pHole1,*m_pHole2;
	CLDSPart* m_pResidePart1,*m_pResidePart2;
public:
	CDesFittingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDesFittingDlg();
	//
	void InitFittingPara();
	void FinishSelectObjOper();		//完成选择对象的后续操作
	void UpdateSketchMap();
	CLDSFitting *CreateFitting(CLDSFitting* pToExist=NULL);
	//属性管理
	DECLARE_PROP_FUNC(CDesFittingDlg);
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值
	void DisplayPropList();
	void DisplaySubPropList(CPropTreeItem *pItem=NULL);
// 对话框数据
	enum { IDD = IDD_DES_FITTING_DLG };

public:
	CPropertyList	m_propList;
	CPropertyList	m_xSubPropList;
	//
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnConfirm();
	afx_msg void OnBnModelViewOper();
};
