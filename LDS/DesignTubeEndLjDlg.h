#if !defined(AFX_DESIGNTUBEENDLJDLG_H__EB9EA7AF_EB03_4AC8_80CB_5CD3F9584EBC__INCLUDED_)
#define AFX_DESIGNTUBEENDLJDLG_H__EB9EA7AF_EB03_4AC8_80CB_5CD3F9584EBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignTubeEndLjDlg.h : header file
//

#include "SuperGridCtrl.h"
#include "PropertyList.h"
#include "PropListItem.h"
#include "CallBackDialog.h"
#include "XhListCtrl.h"

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDesignTubeEndLjDlg dialog

class CDesignTubeEndLjDlg : public CCallBackDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
public:
	//装配位置参数
	typedef struct tagSetUpPosPara  
	{
		CLDSVector workNorm;			//插板工作面法向
		CLDSVector workNormUpVert;		//十字插板中母板上侧的子立板工作法向
		CLDSVector workNormDownVert;	//十字插板中母板下侧的子立板工作法向
		double  H1;						//平焊法兰钢管端与法兰底面的距离
		//
		double  m_fWeldHeight;			//焊缝高度 wht 11-05-09 
		double	m_fOddment;				//钢管正负头
		double	m_fNormOffset;			//插板法向偏移量
		double	m_fTubeNormOffset;		//钢管法向偏移量
		double	m_fTubeEccentricDist;	//钢管偏心距
		tagSetUpPosPara(){memset(this,0,sizeof(tagSetUpPosPara));}
	}SETUP_POS_PARA;
	//布置螺栓参数
	typedef struct tagLayoutBoltPara 
	{
		int m_iDirection;	//朝向
		int m_nBoltD;		//直径
		int m_nBoltNum;		//个数 
		//环向布置螺栓参数
		int m_iStartEdge;	//环向布置起始边
		double m_fOffsetAngle;//首螺栓相对于起始边之间的偏移角度
		double m_fBoltLayoutD;//螺栓所在直径
		//矩阵布置螺栓参数
		int m_nRowNum;		//行数
		int m_nColNum;		//列数
		double m_fRowSpace;	//行距
		double m_fColSpace;	//列距
		f3dPoint m_posFirstBolt;//首螺栓位置
		//
		double m_fSpecialSpace;		//特殊间距
		BOOL m_bStaggerLayout;		//水平面与竖直面交错布置
		BOOL m_bMirLayout;			//对称布置特殊间距两侧螺栓
		tagLayoutBoltPara(){memset(this,0,sizeof(tagLayoutBoltPara)); m_iStartEdge=0;}
	}LAYOUT_BOLT_PARA;
	//附加板参数
	typedef struct tagAttachPlatePara  
	{
		char cMaterial;		//附加板材质
		char sPartNo[16];	//编号
		double A,B,C,D,E,F,G,H;//外形参数 
		int m_iCutMode;					//肋板倒角模式：0.切角|1.切弧
		int m_nThick;		//板厚
		double m_fAngle;	//首块法兰肋板相对于基准边的偏移角度或十字插板肋板切角角度°
		int m_nPlateNum;	//法兰肋板个数
		BOOL m_bUseAngle;	//启用角度α
		BOOL m_bMirCreate;	//对称生成插板另外一次加劲板
		CLDSPlate *m_pWeldParentPlate;	//十字插板附加肋板的焊接父构件 wht 10-08-02
		tagAttachPlatePara();
	}ATTACHPLATE_PARA;
	//节点板
	typedef struct tagNodePlatePara
	{
		char cMaterial;		//材质
		char sPartNo[16];	//编号
		int m_nThick;		//板厚
		int m_nRollHeight;	//卷边高度
		tagNodePlatePara();
	}NODEPLATE_PARA;
	//填板参数
	typedef struct tagFillPlatePara{
		int nNodePlateThick;	//与十字插板相连的节点板的厚度，根据该厚度判断是否需要生成填板，外部设定
		int nThick;				//填板厚度
		char cMat;				//填板材质
		char sPartNo[16];		//填板编号
		tagFillPlatePara(){memset(this,0,sizeof(tagFillPlatePara)); cMat='S';}
	}FILLPLATE_PARA;
private:
	//生成标准构件
	BOOL NewStandardPart(CLDSStdPartModel *pStdPartModel,CLDSStdPartModel *pCoupleStdPartModel=NULL);
	//生成螺栓
	void CreateBolt(CLDSParamPlate *pParamPlate);
	//生成附加钢板
	void CreateAttachPlate(CLDSParamPlate *pParamPlate,CLDSLineTube *pDatumTube,BOOL bFlCoupleDesign=FALSE);
	//初始化标准构件参数
	void InitStdPartDesignParam();
	CString GetTubeEndLjName();
public:
	CDesignTubeEndLjDlg(CWnd* pParent = NULL);   // standard constructor
	//属性管理
	DECLARE_PROP_FUNC(CDesignTubeEndLjDlg);
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值
	void DisplayPropertyList(int iCurSel=0);
	void DisplayChildPropertyList(CPropTreeItem *pItem,BOOL bUpdate=FALSE);
	void FinishSelectObjOper();		//完成选择对象的后续操作
	void UpdatePartTypeProperty(CPropTreeItem* pParentItem);		//
	void UpdateBaseInfoProperty(CPropTreeItem* pParentItem);		//更新基本信息
	void UpdateTubeEndProperty(CPropTreeItem* pParentItem);			//更新钢管端连接
	void UpdateSetUpProperty(CPropTreeItem* pParentItem);			//更新装配参数
	void UpdateProfileProperty(CPropTreeItem* pParentItem);			//更新外形参数
	void UpdateBoltProperty(CPropTreeItem* pParentItem);			//更新螺栓参数
	void UpdateAttachPlateProperty(CPropTreeItem* pParentItem);	//更新附加板参数
	void UpdateFillPlateProperty(CPropTreeItem* pParentItem);		//更新填板参数 wht 11-05-11
	//更新控件显示状态及内容
	void Redraw();
	void RefreshTabCtrl(int iCurSel);
	void RefreshCtrlState(BOOL bShowBolt=TRUE);
	void RefreshBoltListColumn();
	void RefreshBoltParaListColumn();
	void RefreshBoltParamList();
	void RefreshBoltList();	
	//更新螺栓布置参数
	void UpdateLayoutBoltPara();
	void LayoutBolt(ATOM_LIST<f3dPoint>& bolt_pt_arr);
	//重新得到标准构件参数
	void LoadStdPlateParaByLevel();	
	//更新钢管正负头
	void UpdateTubeOddment();
	//初始化设计参数
	void UpdateDesignParam(BOOL bInitPartType=TRUE);
	//根据存在的板更新设计信息
	void UpdateDesignParamBySrcPlate(CLDSParamPlate* pSrcParamPlate);
	//更新示意图
	void UpdateSketchMap(int iCurSel=0);
	//判断是否已经存在端连接
	BOOL IsHasExistEndLj();
	//得到构件类型
	int GetPartType() const;
	//设置构件类型
	void SetPartType(int iPartType);
	//布置十字插板附加板,bIsCloneBolt表示插板上的螺栓通过以后插板克隆而来 wht 11-05-27
	void LayoutXEndSubPlate(CLDSParamPlate *pParamPlate,BOOL bIsCloneBolt=FALSE);
	//设计十字插板填板,使用包钢代替包板时暂不支持设计填板(该情况一般不会出现) wht 11-05-11
	void DesignXEndFillPlate(CLDSParamPlate *pParamPlate);
	//设计卷边插板上的加劲板
	void DesignREndAttachPlate(CLDSParamPlate *pParamPlate);
	//设计节点板
	void InitPlateBasePara(CLDSPlate* pNodePlate,CLDSLineTube *pDatumLineTube);
	void DesignLineAngle(CLDSLineAngle* pRayJg,CLDSPlate* pNodePlate);
	CLDSPlate *CreateNodePlate(CLDSLineTube *pDatumLineTube,CLDSLineAngle* pRayLineAngle);
	CLDSPlate *CreateNodePlate(CLDSLineTube *pDatumLineTube,
		CLDSLineTube *pRayLineTube=NULL,CLDSParamPlate *pParamPlate=NULL,
		CLDSLineTube *pHorizontalTube=NULL);	//pHorizontalTube横材设计异形法兰处节点板时使用 wht 11-06-07
	CLDSParamPlate *GetParamPlate() const{return m_pParamPlate;}
	//设置工作面法线
	void SetWorkNorm(f3dPoint norm){ work_norm=norm; }
	//工作面法线
	f3dPoint GetWorkNorm() const { return work_norm; }
	//根据材质选择标准插板
	INSERT_PLATE_PARAM* QueryStdInsertPlate(int iLevel,char cMat);
	INSERT_PLATE_PARAM* QueryStdInsertPlateByMat(char cMat,int* pLevelArr=NULL);
	FL_PARAM* QueryStdFlDParaByMat(char cMat,int* pLevelArr=NULL);
//
private:
	f3dPoint work_norm;					//工作面法线方向
	CBitmap m_CurPlateBmp;
	CLDSParamPlate *m_pParamPlate,*m_pSrcParamPlate;//
	BOOL m_bHasExistEndLj;	//当前钢管端已经存在端连接
	int m_iPartType;		//0.普通构件 1.标准构件(平焊法兰) 2.对焊法兰 3.刚性法兰 4.柔性法兰
	//char sCodeName[16];		//标准构件代号 记录上一次标准构件代号,更新标准构件级别
	//标准构件相关参数
	FL_PARAM *m_pFlDParam,*m_pFlPParam;
	FL_PARAM *m_pFlDParam2,*m_pFlPParam2;
	FL_PARAM2 *m_pRigidFlParam,*m_pFlexibleFlParam;
	FL_PARAM2 *m_pRigidFlParam2,*m_pFlexibleFlParam2;
	INSERT_PLATE_PARAM *m_pInsertPlateParam;
	//画笔
	CPen solid_pen,dash_pen,dash_dot_pen,green_solid_pen,green_dash_pen,
		red_solid_pen,blue_solid_pen,purple_solid_pen;
public:
	CLDSNode *m_pBaseNode;		//基准节点
	CLDSLineTube *m_pCurLineTube;//当前钢管
	CLDSLineTube *m_pRelatedTube;//对接钢管
	bool m_bCoupleFLCreate;
	BOOL m_bStartEnd;
	BOOL m_bSpecEndLjStyle;		//指定端连接类型
	BOOL m_bDesignNodePlate;	//设计节点板 (设计塔脚板时需要)

	CLDSParamPlate m_paramPlate;//存储参数化钢板的设计信息
	char sCoupleFlPlateNo[16];	//(普通法兰)不等径对接时对接法兰的编号 wht 10-09-29
	char sWeldPlatePartNo[16];	//十字插板垂板编号
	char sWrapPlatePartNo[16];	//十字插板包板编号
	char sCodeName[16];			//十字插板代号
	int m_iWrapPlateType;		//十字插板包板类型  0.钢板 1.角钢
	char m_cFlLevel1;			//主管对焊法兰级别
	char m_cFlLevel2;			//对接对焊法兰级别
	int m_iStdPartLevel;
	char m_cStdPartMat;
	//钢管相贯
	BOOL m_bFaceToFace;	//对接相贯
	long m_hWeldLinkObj;
	long m_hDatumTube1;
	long m_hDatumTube2;
	CStringArray m_arrPropGroupLabel;
	CStringArray m_arrStdPartLevel;	
	CStringArray m_arrStdFlDLevelM;
	CStringArray m_arrStdFlDLevelC;
	//平面切割
	f3dPoint cutPlaneNorm,cutPlaneOrigin;
	//布置螺栓参数	暂时最多支持十组螺栓参数
	LAYOUT_BOLT_PARA LayoutBoltPara;
	ATOM_LIST<LAYOUT_BOLT_PARA> LayoutBoltParaList;
	//装配参数
	SETUP_POS_PARA setupInfo;
	//附加板
	BOOL m_bCreateAttachPlate;
	ATTACHPLATE_PARA AttachPlatePara;
	//包板填板	wht 11-05-11
	BOOL m_bCreateFillPlate;		
	FILLPLATE_PARA FillPlatePara;
	//节点板
	NODEPLATE_PARA NodePlatePara;
	//异形法兰连接的射线杆件
	BOOL m_bSpecialFlRayPole;
public:
// Dialog Data
	//{{AFX_DATA(CDesignTubeEndLjDlg)
	enum { IDD = IDD_DESIGN_TUBE_ENDLJ_DLG };
	CPropertyList	m_xSubPropList;
	CXhListCtrl		m_listBolt;
	CSuperGridCtrl	m_listBoltParam;
	CTabCtrl	m_ctrlPropGroup;
	CPropertyList	m_propList;
	int		m_iTubeEndLjStyle;	//0:平面相贯线;1.圆柱面相贯线;2.法兰;3.U型插板;4.C型插板;5.十字插板;6.高颈法兰.(对应TUBEJOINT::type)
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignTubeEndLjDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignTubeEndLjDlg)
	
	afx_msg void OnSelchangeTubeEndljStyle();
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnConfirmCreate();
	afx_msg void OnPaint();
	afx_msg void OnDelCurEndlj();
	afx_msg void OnPartsDb();
	afx_msg void OnKeydownBoltList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickBoltList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNTUBEENDLJDLG_H__EB9EA7AF_EB03_4AC8_80CB_5CD3F9584EBC__INCLUDED_)
