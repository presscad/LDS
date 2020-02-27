#if !defined(AFX_DESIGNSPECIALFLDLG_H__35EF0998_6C88_43B0_B791_70B92D749DE3__INCLUDED_)
#define AFX_DESIGNSPECIALFLDLG_H__35EF0998_6C88_43B0_B791_70B92D749DE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignSpecialFlDlg.h : header file
//

#include "XhListCtrl.h"
#include "PropertyList.h"
#include "PropListItem.h"
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDesignSpecialFlDlg dialog

class CDesignSpecialFlDlg : public CDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;

	CBitmap m_CurPlateBmp;
	CStringArray m_arrPropGroupLabel;
	//画笔
	CPen solid_pen,dash_pen,dash_dot_pen,green_solid_pen,green_dash_pen,
		red_solid_pen,blue_solid_pen,purple_solid_pen;
	//显示属性
	void DisplayPropertyList(int iCurSel=0);
	//生成螺栓
	void CreateBolt(CLDSPlate *pPlate);
	//生成环向板
	CLDSPlate* CreateCircularPlate(CLDSLineTube *pDatumTube);
	//生成附加钢板
	void CreateAttachPlate(CLDSPlate *pPlate,CLDSLineTube *pDatumTube);
	//生成环向封版
	void CreateCoverPlate(CLDSPlate *pNodePlate1,CLDSPlate *pNodePlate2,
		CLDSParamPlate *&pCoverPlate1,CLDSParamPlate *&pCoverPlate2);
	//生成环向肋板
	CLDSParamPlate* CreateRibPlate(CLDSPlate *pNodePlate1,CLDSPlate *pNodePlate2);
	//平面切割钢管
	void CutDatumTubeByPlane(CLDSPlate *pPlate);
	//异形法兰
	CLDSPlate *CreateSpecialFl(LINEPARTSET *pLinePartSet,double fNormOffset=0);
public:
	int m_iNormOffsetType;
	CLDSPlate *m_pCirPlate;	//0.上侧法兰 1.下侧法兰 2.中间法兰
	//布置螺栓参数
	typedef struct tagLayoutBoltPara 
	{
		int m_iBoltSegI;	//段号
		int m_iDirection;	//朝向
		int m_nBoltD;		//直径
		int m_nBoltNum;		//个数 
		int m_iStartEdge;	//环向布置起始边
		double m_fOffsetAngle;//首螺栓相对于起始边之间的偏移角度
		double m_fBoltLayoutR;//螺栓所在半径
		tagLayoutBoltPara(){memset(this,0,sizeof(tagLayoutBoltPara));}
	}LAYOUT_BOLT_PARA;
	//肘板参数
	typedef struct tagElbowPlatePara
	{
		char cMaterial;		//附加板材质
		char sPartNo[16];	//编号
		int m_nThick;		//板厚
		double A,B,C,D,E,F,G,H;//外形参数
		int m_iCutMode;		//肋板倒角模式：0.切角|1.切弧
		double m_fAngle;	//首块法兰肋板相对于基准边的偏移角度或十字插板肋板切角角度
		int m_nPlateNum;	//法兰肋板个数
		BOOL m_bObverse;	//在正面布置钢板
		BOOL m_bVertical;	//竖直带自动正位的肘板
		tagElbowPlatePara(){memset(this,0,sizeof(tagElbowPlatePara));}
	}ELBOW_PLATE_PARA;
	//封板参数
	typedef struct tagCoverPlatePara
	{	
		char cMaterial;		//材质
		char sPartNo[16];	//编号
		int m_nThick;		//板厚
		double W,D;			//外形参数 
		double m_fAlfa,m_fBeta;	//α，β
		BOOL m_bUpPlate;	//上封板
		BOOL m_bDownPlate;	//下封板
		BYTE biHoleType;	//0.圆孔|1.半圆弧|2.无孔
		double BH;			//流水孔直径（<0.无孔;0.半圆孔;>0.圆孔）
		UINT uiAttribute1;	//节点板1流水孔信息
		UINT uiAttribute2;	//节点板2流水孔信息
		tagCoverPlatePara(){memset(this,0,sizeof(tagCoverPlatePara));biHoleType=0;BH=13.5;}
	}COVER_PLATE_PARA;
	//肋板参数
	typedef struct tagRibPlatePara
	{
		char cMaterial;		//材质
		char sPartNo[16];	//编号
		int m_nThick;		//板厚
		double S,W,D,H,N;//外形参数 
		BOOL m_bUseH;	 //启用参数H
		BOOL m_bTopCir;	 //肋板顶部圆弧连接
		BOOL m_bOutterRibPlate;	//外侧肋板
		BOOL m_bMirCreate;//对称生成
		f3dPoint norm;	 //肋板法线
		tagRibPlatePara(){memset(this,0,sizeof(tagRibPlatePara));}
	}RIB_PLATE_PARA;
public:
	CDesignSpecialFlDlg(CWnd* pParent = NULL);   // standard constructor
	//属性管理
	DECLARE_PROP_FUNC(CDesignSpecialFlDlg);
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值
	bool UpdateItemPropValue(char* propStr);
	//
	void RefreshTabCtrl(int iCurSel);
	void ReDraw();	//刷新
	int LayoutBolt();//布置螺栓,返回螺栓个数
	int LayoutAttachPlate();//布置附加板
	void UpdateTubeOddment();//更新钢管正负头信息
	void UpdateRibPlateWN();//更新肋板W、N参数
	void InitWaterleakHolePara();
	UINT _InitWaterHoleRadiusByThick(double fPlateThick);
	UINT _InitCutRadiusByTubeThick();
	void UpdateWaterHolePropItem(CPropTreeItem* pParentItem);
public:
	CLDSNode *m_pBaseNode;			//基准节点
	CLDSLineTube *m_pCurLineTube;	//当前钢管
	CLDSLineTube *m_pCoupleTube;	//对接钢管 此钢管上生成异形法兰
	CLDSLinePart *m_pRayLinePart[4];//射线杆件	0,1上侧射线杆件 2,3下侧射线杆件
	CLDSPlate *m_pNodePlate[4];		//节点板	0,1上侧节点板 2,3下侧节点板
	CLDSParamPlate *m_pCoverPlate[4];//环向封板
	LINEPARTSET *m_pLinePartSet;	//射线杆件集合
	//底板参数
	GEPOINT axis_z;	//底板工作法线
	CXhChar16 m_sPartNo;
	SEGI iSeg;
	char m_cMaterial;
	int	 m_nThick;
	double m_fNormOffset;
	double m_fExternalRadius;	//环向板外半径
	double m_fInnerRadius;		//环向板内半径
	double H;	//法兰底面与钢管端头之间的距离
	double m_fOddment;	//钢管正负头
	//螺栓参数
	LAYOUT_BOLT_PARA LayoutBoltPara;
	//肘板
	ELBOW_PLATE_PARA ElbowPlatePara;
	//封板
	COVER_PLATE_PARA CoverPlatePara;
	//肋板
	RIB_PLATE_PARA RibPlatePara;
	BOOL m_bCreateRibPlate;		//用来控制是否生成肋板	wht 11-05-26
	BOOL m_bCutDatumTube;		//基准钢管是否进行平面切割
// Dialog Data
	//{{AFX_DATA(CDesignSpecialFlDlg)
	enum { IDD = IDD_DESIGN_SPECIAL_FL_DLG };
	CXhListCtrl	m_listPlate;
	CXhListCtrl	m_listBolt;
	CTabCtrl	m_ctrlPropGroup;
	CPropertyList	m_propList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignSpecialFlDlg)
protected:
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignSpecialFlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnConfirmCreate();
	afx_msg void OnPaint();
	afx_msg void OnKeydownBoltList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickBoltList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownPlateList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickPlateList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeLjStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNSPECIALFLDLG_H__35EF0998_6C88_43B0_B791_70B92D749DE3__INCLUDED_)
