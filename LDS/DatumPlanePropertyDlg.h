#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDatumPlanePropertyDlg 对话框
#include "PropertyList.h"
#include "CallBackDialog.h"
class CDatumPlanePropertyDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDatumPlanePropertyDlg)
public:
	CLDSPlane m_datumPlane;
	CLDSPoint m_datumPoint1;
	BOOL m_bNewPoint1;
	CLDSPoint m_datumPoint2;
	BOOL m_bNewPoint2;
	CLDSPoint m_datumPoint3;
	BOOL m_bNewPoint3;
	CLDSPoint* m_pActivePt;
	HANDLEPOINT ToHandlePoint(CLDSPoint* pDatumPoint);
public:
	CDatumPlanePropertyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDatumPlanePropertyDlg();
	void UpdateEditPoint(CLDSPoint* pPt);
// 对话框数据
	enum { IDD = IDD_DATUM_PLANE_PROPERTY_DLG };
	double m_fPointX;
	double m_fPointY;
	double m_fPointZ;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_ctrlPropGroup;
	CPropertyList m_propList;
	afx_msg void OnTcnSelchangeTabGroup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnUpdatePlane();
	afx_msg void OnBnCopyPlane();
	afx_msg void OnEnChangeEdtX();
	afx_msg void OnEnChangeEdtY();
	afx_msg void OnEnChangeEdtZ();
	afx_msg void OnBnCopyNorm();
public:
	void FinishSelectObjOper();		//完成选择对象的后续操作
	void DisplayDatumPlaneProperty();
	//属性管理
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	int GetPropValueStr(long id,char *valueStr);	//根据属性ID得到属性值
	void InitPropHashtable();
	static long GetPropID(char *sProp);			//得到属性ID
	BOOL UpdatePlane();
};
