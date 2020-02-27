#pragma once

#include "PropertyList.h"
#include "CallBackDialog.h"
#include "afxwin.h"

// CDesignOuterAttachDlg 对话框

struct OUTER_ATTACH_ANGLE_INFO{
	CLDSNode *pDatumNode;
	CLDSLineAngle *pDatumJg,*pDatumJg1;
	BOLTSET *pBoltSet;
	CLDSPlate *pRelaPlate;
	//
	char cMaterial;					//材质简化字符
	CXhChar16 spec;					//短角钢规格字符串, 格式: "L140X10"
	CXhChar16 sPartNo,xPadPartNo,yPadPartNo;
	SEGI iSeg;						//段号
	short nRotateAngle;				//旋转角度°
	short iWorkWingOfDepend;//JunctionStyle;0:双肢连接外包铁;1:X肢外贴材;2:Y肢外贴材
	short iWorkWingOfSelf;	//nAttachWing;	外贴短角钢依附肢0:双肢外包于基准角钢 1.短角钢的X肢依附于基准角钢 2.短角钢的Y肢依附于基准角钢
	BOOL bLocateOutBer;				//是否外包
	static const BYTE WING_XY = 0;
	static const BYTE WING_X  = 1;
	static const BYTE WING_Y  = 2;

	OUTER_ATTACH_ANGLE_INFO(){pDatumNode=NULL;pDatumJg=pDatumJg1=NULL;pBoltSet=NULL;pRelaPlate=NULL;}
};
class CDesignOuterAttachDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesignOuterAttachDlg)

public:
	CDesignOuterAttachDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDesignOuterAttachDlg();

	void SetDatumAngle(CLDSLineAngle *pDatumAngle);
	CLDSLineAngle *GetDatumAngle(){return m_pDatumAngle;}
	
	CLDSLineAngle *m_pShortSegJg;
	bool (*DesOuterAttachShortJg)(CLDSLineAngle* &pShortSegJg,OUTER_ATTACH_ANGLE_INFO* pDesignInfo);
	void UpdateOuterAttachAngleInfo(BOOL bInernalEnable=TRUE);
// 对话框数据
	enum { IDD = IDD_DESIGN_OUTER_ATTACH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

private:
	CBitmap m_CurJointBmp;
	CLDSLineAngle *m_pDatumAngle;	//基准角钢
public:		//输入参数
	OUTER_ATTACH_ANGLE_INFO info;
	int m_nOutterAttachStyle;		//外贴形式：0:沿向顺贴;1:沿向倒贴;2.垂向朝外贴;3.垂向朝里贴;4.外包 5.交叉角钢;
	int m_nWorkWingOfDepend;		//外贴短角钢依附肢0.外包于基准角钢 1.短角钢依附于基准角钢的X肢 2.短角钢依附于基准角钢的X肢
	CString m_sSpec;				//短角钢规格字符串, 格式: "L140X10"
	int m_iMaterial;				//材质
	CString m_sPartNo;				//构件编号
	CString m_sSegI;				//段号
	afx_msg void OnChangePartNo();
	afx_msg void OnCbnSelchangeCmbOutterAttachStyle();
	afx_msg void OnCbnSelchangeCmbAttachWing();
};
