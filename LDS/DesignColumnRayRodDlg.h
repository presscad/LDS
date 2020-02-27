#pragma once
#include "propertylist.h"
#include "CallBackDialog.h"

// CDesignColumnRayRodDlg 对话框

class CDesignColumnRayRodDlg : public CCallBackDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
public:
	const static int ROD_TYPE_ANGLE = 0;
	const static int ROD_TYPE_SLOT	= 1;
	const static int ROD_TYPE_FLAT	= 2;
	const static int ROD_TYPE_TUBE	= 3;

	const static int ROD_ENDPOSTYPE_AXISVEC	= 0;
	const static int ROD_ENDPOSTYPE_RAYVEC	= 1;
	const static int ROD_ENDPOSTYPE_USERDEF = 2;

	DECLARE_DYNAMIC(CDesignColumnRayRodDlg)
	void DisplayRayRodPropList();
	void DisplaySubObjPropList(CPropTreeItem *pItem,BOOL bUpdate=FALSE);
	void FinishSelectObjOper();
	void InitRayRodDesginPara();
	CLDSLinePart *CreateRayRod();
	int m_idParentEventProp;
	//射线杆件基本信息
	CXhChar16 m_sPartNo;	//件号
	SEGI  m_xSegI;			//段号
	char m_cMaterial;		//材质
	int m_iRodType;			//杆件类型 0.角钢 1.槽钢 2.扁铁 3.钢管
	CXhChar16 m_sSpec;		//规格
	CLDSPoint m_xRodStartPos;	//杆件始端位置(默认为柱面定位点)
	int m_iEndPosType;			//0.沿始端柱面轴向延伸 1.沿始端柱面射线延伸 2.自定义
	int m_nRotateArrayN;		//多柱面射线杆件阵列的数量
	double m_fSectorAngle;		//多柱面射线杆件阵列时邻近两根间的扇形夹角，°
	CLDSPoint m_xRodEndPos;		//杆件终端位置(默认为柱面定位点)
	int m_iRodLenDefType;		//杆件长度定义类型: 0.指定杆件长度 1.指定末端辅射半径
	double m_fRayRodLen;		//杆件长度/末端辅射半径
	//始端柱面定位点
	CLDSVector m_xStartAxisVec;		//柱面定位点:轴线正方向
	CLDSVector m_xStartRayVec;		//柱面定位点:射线方向
	CLDSPoint m_xStartSectCenter;	//柱面定位点:截面圆心
	CLDSVector m_xStartSectNorm;	//柱面定位点:截面法线方向
	//终端柱面定位点
	CLDSVector m_xEndAxisVec;		//柱面定位点:轴线正方向
	CLDSVector m_xEndRayVec;		//柱面定位点:射线方向
	CLDSPoint m_xEndSectCenter;		//柱面定位点:截面圆心
	CLDSVector m_xEndSectNorm;		//柱面定位点:截面法线方向
	//钢管、槽钢、扁铁工作面法线参数
	f3dPoint m_xWorkPlaneNorm;		//工作面法线
	short  m_iNormCalStyle;			//基准面法线计算方式 0.直接指定 1.使用参照杆件计算
	f3dPoint m_xNearWorkPlaneNorm;	//nearWorkPlaneNorm是最终工作面法线的确定依据
	long m_hWorkNormRefPart;		//计算工作面法线时的参照杆件，如果为0，则以始端节点或终端节点的父杆件为参照杆件
	//角钢工作面法线参数
	CLDSVector m_xDesVectorX,m_xDesVectorY;		//子属性栏中编辑角钢参数时使用
	f3dPoint m_xNormX,m_xNormY;					//记录X、Y肢法线
	DESIGN_JGWING_NORM m_xDesNormX,m_xDesNormY;
public:
	CDesignColumnRayRodDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDesignColumnRayRodDlg();
	//
	void UpdateParentEventPropItem();
	//属性管理
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	int GetPropValueStr(long id, char *valueStr);	//根据属性ID得到属性值
	void InitPropHashtable();
	static long GetPropID(char *sProp,...);			//得到属性ID
// 对话框数据
	enum { IDD = IDD_DESIGN_COLUMN_RAY_ROD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedBtnConfirm();
public:
	CLDSLineTube *m_pBaseLineTube;
	CPropertyList m_xPropList;
	CPropertyList m_xSubPropList;
	CString m_sSubObjName;
};
