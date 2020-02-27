#pragma once

#include "CallBackDialog.h"
#include "PropertyList.h"
#include "XhCharString.h"
#include ".\StdComTempl\TubeWalkWayTempl.h"
//////////////////////////////////////////////////////////////////////////
// CDesWireNodeDlg 对话框
class CDesignTubeWalkWayDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesignTubeWalkWayDlg)
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
	CBitmap m_xWalkWayBmp;
	long m_hBaseTube,m_hBaseNode;	//防止undo时取消节点导致指针失效 wjh-2018.10.8
	long m_hOuterBeamDatumNode,m_hInnerBeamDatumNode;
	long m_hOuterAngle,m_hInnerAngle;
public:
	BYTE m_ciWalkWayType;		//0.钢板支架走道|1.三角支架走道|2.台型支架走道
	BOOL m_bCreateWalkWay;		//是否生成走道角钢
	double m_fYA;				//基准钢管到走道外侧角钢的横向投影距离
	double m_fZH;				//基准钢管到走道角钢平面的竖向投影距离
	double m_fWidth;			//走道宽度
	double m_fLength;			//走道长度
	double m_fZh;				//斜撑下侧竖向偏移
	double m_fD1;				//斜撑下侧横向偏移
	double m_fD2;				//斜撑上侧端点沿水平杆向内的横向偏移
	double m_fYB;				//竖撑横向偏移距
	double m_fBoltSpace;		//螺栓间距
	int m_nBoltMd;				//螺栓直径
	int m_nBoltNum;				//螺栓数量
	CXhChar16 m_sWalkWayJgSpec;	//走道角钢规格
	CXhChar16 m_sHolderJgSpec;	//支架角钢规格
	char m_cWalkWayJgMat;		//支架角钢材质
	char m_cHolderJgMat;		//支架角钢材质
	char m_ciHolderAngleLayoutStyle;	//0.朝外;1.朝内
	char m_cPlateMat;			//钢板材质
	int m_nThick;				//钢板厚度
	struct PLATE_PARA{
		double A;	//走道焊接板扇形夹角
		double B;	//螺栓连接板扇形夹角
		double H;	//台口高度
		double W;	//
	}para;
	//
	CLDSLineTube* m_pBaseTube;
	CLDSNode* m_pBaseNode;
	CLDSLineAngle* m_pOuterJg;	//走道外侧角钢
	CLDSLineAngle* m_pInnerJg;	//走道内侧角钢
	double m_fZjOffDist;			//支架位置偏移距离
	double m_fZdOffDist;			//走道始端偏移距离
	//
	bool VerifyRelaRodAndNodes();
	CTubeWalkWayStdComTempl m_xWalkWayTempl;
public:
	CDesignTubeWalkWayDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDesignTubeWalkWayDlg();

	// 对话框数据
	enum { IDD = IDD_DESIGN_WALKWAY_DLG };
	CPropertyList m_propList;
	CPropertyList m_propSubList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	void UpdateSketchMap();
	void InitTemplParams();
	GEPOINT  CalNearVecX();
	void WorkPlaneSketch();
	void DelWorkPlaneSketch();
	BOOL CreateTubeWalkWay();
	//
	DECLARE_PROP_FUNC(CDesignTubeWalkWayDlg);
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	void DisplayPropList();
	void DisplaySubPropList();
	void UpdateWalkWayProp(CPropTreeItem* pParentItem);
	void UpdateHolderProp(CPropTreeItem* pParentItem);
	void UpdateAssemblyProp(CPropTreeItem* pParentItem);
	BOOL FinishSelectObjOper(long hPickObj,long idEventProp);
};
