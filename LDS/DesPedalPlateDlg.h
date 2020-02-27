#pragma once
#include "PropertyList.h"
#include "PropListItem.h"
#include "XhCharString.h"
#include "CallBackDialog.h"

// CDesPedalPlateDlg 对话框
class CDesPedalPlateDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesPedalPlateDlg)
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX, nScrLocationY;
	CBitmap m_xCurPlateBmp;
	long m_idSubEventProp;  //记录触发中断的子属性ID,恢复窗口时使用
	ATOM_LIST<CLDSBolt> m_xPreBoltList;
public:
	//角钢肋板参数
	struct PEDALPLATE_PARAM {
		long ciDirection;	//延伸方向方式 0.肢翼侧|1.楞线侧
		long C;		//外形类型：0.外型I|1.外形II
		double S;	//连接宽度,S=0时，自动延伸到边
		double L;	//踏板长度
		double W;	//踏板宽度
		double H;	//挡板高度
		double D;	//台口高度
		//
		PEDALPLATE_PARAM() { memset(this, 0, sizeof(PEDALPLATE_PARAM)); }
	}para;
	struct LAYOUT_PARAM {
		BYTE m_ciWorkWing;	//所在工作肢  0.X肢|1.Y肢
		BYTE m_ciLineS0E1;	//所在连接端  0.始端|1.终端
		int m_nBoltNum;		//螺栓数
		int m_nBoltD;		//螺栓直径
		int m_nBoltSpace;	//螺栓间距
		BYTE m_ciGType;		//0.g|1.g1|2.g2|3.g3
		int m_nGDist;		//
		double m_fSpace;	//踏板间距
		double m_fSumLen;	//角钢总长
		double m_fCanLen;	//剩下长度
		double m_fExtraLen;	//固定偏移
		double m_fUseLen;	//待布置螺栓纵向延伸使用距离
		double m_fPreRemainDist;	//上跟杆件预留长度
		double m_fJointGap;	//接头间隙
		//
		LAYOUT_PARAM() { memset(this, 0, sizeof(LAYOUT_PARAM)); }
	}layout_para;
	//基本参数
	struct BASIC_PARRM
	{
		SEGI nSegI;
		char cMaterial;
		int nThick;
		//
		BASIC_PARRM() { memset(this, 0, sizeof(BASIC_PARRM)); }
	};
protected:
	//处理记录上次数据
	static bool m_bValidStaticRibParam;
	static PEDALPLATE_PARAM s_xDesParam;
	static LAYOUT_PARAM s_xLayoutParam;
	static BASIC_PARRM s_xBasicParam;
public:
	CLDSLineAngle* m_pDatumAngle;
	CXhChar16 m_sPartNo;
	SEGI m_nSegI;
	char m_cMaterial;
	int m_nThick;
	BOOL m_bNewPlate;
	CLDSParamPlate _previewPlate;
	CLDSParamPlate *m_pPedalPlate;
protected:
	void ClearPreBolts();
	void InitPedalPlatePara();
public:
	CDesPedalPlateDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDesPedalPlateDlg();
	//
	CLDSParamPlate *CreatePedalPlate(CLDSParamPlate* pToExistPlate = NULL);
	double CalOffDist();
	void LayoutBolt(CLDSParamPlate* pPlate, BOOL bNewCreate = FALSE);
	void ReDesignPlate();
	void FinishSelectObjOper();		//完成选择对象的后续操作
	void UpdateSketchMap();
	void ModifyPlateParam(WORD wKey, double fValue);
	void ModifyPlateParam(WORD wKey, long nValue);
	void DeletePlateParam(WORD wKey);
	//属性管理
	DECLARE_PROP_FUNC(CDesPedalPlateDlg);
	int GetPropValueStr(long id, char *valueStr, UINT nMaxStrBufLen = 100);	//根据属性ID得到属性值
	void DisplayPlatePropList();
	void DisplaySubPropList(CPropTreeItem *pItem = NULL);
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DES_PEDAL_PLATE };
#endif
	CPropertyList	m_xSubPropList;
	CPropertyList	m_propList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnConfirm();
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnModelViewOper();
};
