#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "NativePropertyList.h"

// CDatumLinePropertyDlg 对话框
#include "PropertyList.h"
#include "CallBackDialog.h"
class CDatumLinePropertyDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDatumLinePropertyDlg)
public:
	CLDSLine m_datumLine;
	CLDSPoint m_startPoint;
	BOOL m_bNewStartPoint;
	CLDSPoint m_endPoint;
	BOOL m_bNewEndPoint;
	CLDSPoint* m_pActivePt;
public:
	CDatumLinePropertyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDatumLinePropertyDlg();
	

// 对话框数据
	enum { IDD = IDD_DATUM_LINE_PROPERTY_DLG };
	void UpdateEditPoint();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_ctrlPropGroup;
	CParaPointPropertyList m_propList;
	afx_msg void OnTcnSelchangeTabGroup(NMHDR *pNMHDR, LRESULT *pResult);
public:
	void DisplayDatumLineProperty();
	//属性管理
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	int GetPropValueStr(long id,char *valueStr);	//根据属性ID得到属性值
	int InitPropHashtable();
	static long GetPropID(char *sProp);			//得到属性ID
	double m_fPointX;
	double m_fPointY;
	double m_fPointZ;
	afx_msg void OnEnChangeEdtX();
	afx_msg void OnEnChangeEdtY();
	afx_msg void OnEnChangeEdtZ();
};
