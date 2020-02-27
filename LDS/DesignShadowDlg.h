#pragma once

#include "PropertyList.h"
#include "CallBackDialog.h"
#include "afxwin.h"

// CDesignShadowDlg 对话框

class CDesignShadowDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesignShadowDlg)

public:
	CDesignShadowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDesignShadowDlg();
	void DisplayShadowDesignPropList();
	void DisplaySubPropList(CPropTreeItem *pItem,BOOL bUpdate=FALSE);
	void FinishSelectObjOper();						//完成选择对象的后续操作
	//属性管理
	void InitPropHashtable();
	int GetPropValueStr(long id, char *valueStr);	//根据属性ID得到属性值
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	static long GetPropID(char* propStr);			//得到属性ID
	void SetShadowObj(CLDSObject *pShadowObj){m_pShadowObj=pShadowObj;}
	CLDSObject *GetShadowObj(){return m_pShadowObj;}
	void ReDesignShadowObj();	//重新设计影射构件
	CPropTreeItem* SetDesignInfoPropListCurSel();	//设置设计信息属性栏当前选中项
	void RefreshShadowDesignInfo();		//更新影射构件设计信息

// 对话框数据
	enum { IDD = IDD_DESIGN_SHADOW_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnCbnSelchangeCmbShadowCreateStyle();
	DECLARE_MESSAGE_MAP()

public:
	CPropertyList m_xDesignInfoPropList;
	CPropertyList m_xSubPropList;
	int m_iShadowCreateStyle;	//影射构件生成方式0.对称生成 1.指定偏移量生成 2指定方位生成 3指定面镜像生成
	long m_idParentEventProp;	//记录触发中断的子属性ID,恢复窗口时使用
private:
	CLDSObject *m_pShadowObj;	//NULL:生成影射构件 !NULL:查看修改已有影射构件设计信息
public:		//输入参数	
	BOOL m_bMirAxisX;			//X轴对称
	BOOL m_bMirAxisY;			//Y轴对称
	BOOL m_bMirAxisZ;			//Z轴对称
	BOOL m_bMirRotation;		//旋转对称
	BOOL m_bAdvancedMir;		//高级对称

	double m_fOffsetDist;		//指定偏移量生成影射构件时的偏移量
	int m_nOffsetNum;			//指定偏移量生成影射构件时的偏移数量

	int m_iCurDesParaType;		//0.datumPoint(点设计) 1.datumVector1(方向设计/基准轴设计) 2.datumVector2(辅助轴设计)
	CLDSPoint datumPoint;		
	CLDSVector datumVector1;
	CLDSVector datumVector2;
	
public:		//输出参数
	CParaCS des_cs;				//生成影射构件的参数化坐标系
};
