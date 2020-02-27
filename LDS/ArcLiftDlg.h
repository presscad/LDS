#pragma once
#include "afxwin.h"
#include "PropertyList.h"
#include "CallBackDialog.h"
// CArcLiftDlg 对话框

#ifdef __ANGLE_PART_INC_
class CArcLiftDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CArcLiftDlg)
	CPropertyList m_propList;
public:
	CLDSArcLift m_xArcLift;
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
public:
	CArcLiftDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CArcLiftDlg();

	void DisplayArcLiftProperty();
	void FinishSelectObjOper();		//完成选择对象的后续操作
	void CopyCoordinate(long idProp);
	//属性管理
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	CString GetPropValueStr(long id);		//根据属性ID得到属性值
	void InitArcLiftPropHashtable();
	static long GetPropID(char* propStr);	//得到属性ID
// 对话框数据
	enum { IDD = IDD_ARC_LIFT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnConfirm();
};
#endif