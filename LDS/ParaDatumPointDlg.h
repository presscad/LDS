 #pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "PropertyList.h"
#include "CallBackDialog.h"

// CParaDatumPointDlg 对话框

class CParaDatumPointDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CParaDatumPointDlg)
public:
	CLDSPoint m_xPoint;
	double m_fPointX;
	double m_fPointY;
	double m_fPointZ;
public:
	CPropertyList m_propList;
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	int GetPropValueStr(long id,char *valueStr);	//根据属性ID得到属性值
	static long GetPropID(char *sProp);			//得到属性ID
	void InitPropHashtable();
	void FinishSelectObjOper();		//完成选择对象的后续操作
	void DisplayParaPointProperty();
public:
	CParaDatumPointDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParaDatumPointDlg();

// 对话框数据
	enum { IDD = IDD_PARA_DATUM_POINT };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdtX();
	afx_msg void OnEnChangeEdtY();
	afx_msg void OnEnChangeEdtZ();
};
