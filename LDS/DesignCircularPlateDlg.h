#if !defined(AFX_DESIGNCIRCULARPLATEDLG_H__3ECCA5E9_8DB1_4E12_93A3_B8C2CFBAA131__INCLUDED_)
#define AFX_DESIGNCIRCULARPLATEDLG_H__3ECCA5E9_8DB1_4E12_93A3_B8C2CFBAA131__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignCircularPlateDlg.h : header file
//

#include "PropertyList.h"
#include "PropListItem.h"
#include "XhCharString.h"
#include "CallBackDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CDesignCircularPlateDlg dialog
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

class CConnVectorList
{
	ATOM_LIST<CONNECT_DIRECTION> vecList;	//连接方向
public:
	CONNECT_DIRECTION *Append();
	CONNECT_DIRECTION *Append(CONNECT_DIRECTION &vec);
	long GetRelaConnVecLinePart(int serial,BOOL bProfilePole=FALSE);
	CONNECT_DIRECTION *GetRelaConnInfo(int serial,BOOL bProfilePole=FALSE);
	CONNECT_DIRECTION *GetFirst(BOOL bProfilePole=FALSE);
	CONNECT_DIRECTION *GetNext(BOOL bProfilePole=FALSE);
	int GetCount(BOOL bProfilePole=FALSE);
	void Empty(BOOL bProfilePole=FALSE);
	//
	CONNECT_DIRECTION *GetRelaConnVecByFeature(long feature);
	BYTE DeleteRelaConnVecByFeature(long feature);
	//根据是否存在serial==1和serial==2的连接方向判断
	BOOL IsNeedBend();
};

class CDesignCircularPlateDlg : public CCallBackDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
private:
	GEPOINT m_xSpreadVec1,m_xSpreadVec2;
	CBitmap m_xCurPlateBmp;
	CLDSParamPlate _previewParamPlate;
	CLDSParamPlate *CreateWaterShieldedPlate(CLDSParamPlate* pToExistPlate=NULL);	//生成遮水板 
	int m_idPlateType;
	int _IntelliRecogPlateTypeId();
	UINT _InitWaterHoleRadiusByThick(double fPlateThick);
public:
	CDesignCircularPlateDlg(CWnd* pParent = NULL);
	
	BOOL IsValidParam(BOOL bCheckPropfileParam=TRUE);
	void UpdateSketchMap(BOOL bShowWaterHole=FALSE);
	void InitCirPlatePara();//初始化环向板板参数,目前在外面还有可能调用到，未来应去掉 wjh-2018l3.7
	void InitProfilePara();	//初始化钢板外形参数
	void InitWaterleakHolePara(CLDSPart* pSpecRelyVertPlate=NULL);
	int SideOfCirRibPlateOnOutwardToBasicPlate();	//环向肋板是否在基板外侧,外侧(>0),内侧(<0)，其它不合法(0)
	BOOL AutoCalWN();		//自动计算W,N参数
	BOOL AutoCalDH();		//自动计算D,H参数
	void ModifyPlateParam(WORD wKey,double fValue,BOOL bReDesign=TRUE);
	void ModifyPlateParam(WORD wKey,long nValue,BOOL bReDesign=TRUE);
	void ModifyPlateParam(WORD wKey,long hPart,UINT uiValue,BOOL bReDesign=TRUE);
	void ReDesignPlate();
	//计算环向板上侧长度，下侧长度，便于生成类似封板的环向板 wht 11-05-31
	void CalMaxMinOffsetDist(double &up_len,double &down_len);
	//钢板生成函数
	void UpdateCircularPlateConnVecList(CLDSParamPlate *pCirPlate);
	void PreviewCircularPlate();
	CLDSParamPlate *CreateCircularPlate(CLDSParamPlate* pToExistPlate=NULL);
	//添加关联杆件
	void AddRelaPole(CLDSLinePart *pLinePart);
	//布置螺栓 wht 11-05-16
	void LayoutBolt(CLDSParamPlate *pParamPlate);
	//
	static const int TYPE_DATUM_VECTOR	=1;	//定位基准方向
	static const int TYPE_TUBE_SPREAD	=2;	//钢管轴方向
	static const int TYPE_Z_AXIS_VECTOR =3;	//Z轴方向
	static const int TYPE_X_AXIS_VECTOR =4;	//X轴方向
	static const int TYPE_Y_AXIS_VECTOR =5;	//Y轴方向
	static const int TYPE_SPREAD_VECTOR =6;	//环板方位
 	void WorkPlaneSketch(int iType);
	void DelWorkPlaneSketch();
	void ClearCirPlate();
	void RedrawSketchDrawing(CPropTreeItem *pItem);
	//属性管理
	DECLARE_PROP_FUNC(CDesignCircularPlateDlg);
	static void InitEngPropHashtable();
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值
	void DisplayPlatePropList();
	void DisplaySubPropList(CPropTreeItem *pItem=NULL);
	void UpdateCirPlateBasicProp(CPropTreeItem *pBasicItem=NULL);
	void UpdateCirPlateRelaProp(CPropTreeItem *pRelaInfoItem=NULL);
	void UpdateCirPlateLocationProp(CPropTreeItem *pLocationItem=NULL);
	void UpdatePlateProfileParamProp(CPropTreeItem *pItem);
	void UpdateVectorPropList(CPropTreeItem *pParentItem,char* sPrefix,CLDSVector xVector);
public:
	long m_idSubEventProp;			//记录触发中断的子属性ID,恢复窗口时使用
	void FinishSelectObjOper();		//完成选择对象的后续操作
public:	//环向板的子类型
	static const int TYPE_COMMON_PLATE	/*= 1*/;	//0.普通环向板
	static const int TYPE_SPECIAL_FL	/*= 2*/;	//0.异型法兰
	static const int TYPE_COVERPLATE2	/*= 3*/;	//1.环向封板
	static const int TYPE_COVERPLATE3	/*= 4*/;	//1.环向封板(两块简化封板)
	static const int TYPE_COVERPLATE4	/*= 5*/;	//1.环向封板(自由边为直线型的封板)
	static const int TYPE_3WL_1			/*= 6*/;	//2.3WL(三边支撑直线型)IDB_BMP_CIRCULAR_RIBPLATE5跨越链板
	static const int TYPE_3WL_2			/*= 7*/;	//2.3WL(三边支撑直线型)IDB_BMP_CIRCULAR_RIBPLATE1
	static const int TYPE_3WA_1			/*= 8*/;	//3.3WA(三边支撑圆弧型)IDB_BMP_CIRCULAR_RIBPLATE5跨越链板
	static const int TYPE_3WA_2			/*= 9*/;	//3.3WA(三边支撑圆弧型)IDB_BMP_CIRCULAR_RIBPLATE2
	static const int TYPE_2WL_1			/*= 10*/;	//4.环向肋板2WL(两边支撑直线型-指定肋板高D )IDB_BMP_CIRCULAR_RIBPLATE3
	static const int TYPE_2WL_2			/*= 11*/;	//4.环向肋板2WL(两边支撑直线型-指定扇形角β)IDB_BMP_CIRCULAR_RIBPLATE7
	static const int TYPE_2WA			/*= 12*/;	//5.环向肋板2WA(两肢支撑圆弧型)IDB_BMP_CIRCULAR_RIBPLATE4
	static const int TYPE_SHIELD_WATER	/*= 13*/;	//6.遮水板IDB_BMP_SHIELDEDWATER_PLATE
	static const int TYPE_CIR_BASE		/*= 14*/;	//6.圆底座
	static const int TYPE_AISLE_PLATE	/*= 15*/;	//7.走道板

	int get_idPlateType();
	int set_idPlateType(int idType);
	__declspec(property(get=get_idPlateType,put=set_idPlateType)) int idPlateType;
public:
	CLDSParamPlate *m_pCirPlate;	//当前处于编辑状态的肋板,目前仅用于查看修改属性 wht 11-05-12
	BOOL m_bNewCirPlate;			//TRUE:用于生成新的环向板 FALSE:查看修改属性
	BOOL m_bDisplayDlg;		//设计封板时，是否弹出布孔对话框
	//0.普通环向板1.环向封板;2.环向肋板3WL(三边支撑直线型)3.环向肋板3WA(三边支撑圆弧型)
	//4.环向肋板2WL(两边支撑直线型)5.环向肋板2WA(两肢支撑圆弧型) 6.遮水板 7.环向走道板
	int m_iPlateType;
	BOOL m_bReadOnlyPlateType;		//设置钢板类型不可编辑由外界指定，生成异形法兰时使用 wht 11-06-07
	//关联参数
	CLDSNode *m_pDatumNode;
	CLDSSphere *m_pDatumSphere;
	CLDSLineTube *m_pDatumTube;				//基准钢管
	CLDSPart *m_pRelaPart1, *m_pRelaPart2;	//关联构件
	CLDSPart *m_pCoverPart0,*m_pCoverPart1,*m_pCoverPart2,*m_pCoverPart3;
	CLDSVector design_axis_z;
	CLsRefList LsRefList;			//关联螺栓
	CConnVectorList ConnVecList;	//连接方向
	CONNECT_DIRECTION *m_pCurSelConnVec;	//用于记录当前选中连接方向
	CXhChar16 m_sPartNo,m_sMirPartLabel;
	CString  m_sSeg;
	char m_cMaterial;
	long thick;
	UCS_STRU plate_ucs;	//环向板坐标系 wht 11-06-03
	f3dPoint connVec1,connVec2; 
	CString m_sWndTitle;//设置窗口标题 wht 11-06-07
	//
	struct CIRCULAR_PARAM{
		double A;	//α,环向定位角(定位参数),单位°
		double B;	//β,环向扇形角(外形参数),单位°
		long   C;	//定位基准钢管(球体)句柄(定位参数)
		double D;	//基准钢管外壁到自由边距离，简称自由边距
		double d;	//基准钢管外壁到异型法兰内径的距离，简称内侧边距
		long   E;	//第一块关联基准钢板句柄
		long   F;	//第二块关联基准钢板句柄，或焊接关联钢管句柄(关联钢管是指与环向板有焊缝的钢管，不含上面的插板螺栓连接钢管，因此不可能出现两根)
		long   G;	//自由边类型(>0直线，<=0圆弧)
		double H,h;	//W和N侧台口高度
		long   I;	//第一根连接方向钢管句柄(指明有此钢管方向的连接意图，即插板螺栓连接或无连接制弯)
		long   J;	//第二根连接方向钢管句柄(指明有此钢管方向的连接意图，即插板螺栓连接或无连接制弯)
		long   P;	//定位基准节点句柄
		double R;	//环向板外圆弧半径
		double r;	//r:环向板内圆半径(异形法兰或360的环向板使用 r<0根据钢管生成内圆 r=0无内圆 r>0按指定半径生成内圆) wht 11-05-27
		double S;	//自定位基准节点向定位原点的轴向偏移距离(S<0上封板;S>0下封板)
		double T;	//自定位基准节点向定位原点的轴向偏移距离在环向板法线上的投影距离
		double V;	//加工间隙(基准钢管外壁到环向板内环的预留间隙)
		double Y;	//半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)
		double gama;//y:γ,2WA型环向肋板自由侧的倒角角度,单位°
		double W;	//第一块关联钢板的焊缝长
		double N;	//第二块关联钢板或钢管的焊缝长
		double j;	//起始边：0.基准边|1.焊道线 
		BOOL bWeldSemicircleMode; //环向封板是否带焊接半圆
		double R1,R2; //基准板1流水孔半径，基准板流水孔半径
		int nOffsetType;	//0.纵向偏移 1.法线偏移
		//参数D和H为一组关联参数,D和H独立的情况即为跨越连板 
		//3WL:0.独立参数 1.关联且启用D 2.关联且启用H 
		//3WA:0.不启用DH 1.启用DH
		int nDHRelativeType;	
		//BOOL bNeedBend;		//需要制弯的肋板,m_iPlateType==0时表示是否为异形法兰
		BOOL bCreateMirPlate;	//生成对称侧钢板 wht 11-06-07
		/*对应参数列表中的Y参数 wht 12-03-20
		Y==0表示生成普通环形封板;
		Y==1表示主扇形角α在Y轴正方侧的简化封板;
		Y==-1表示主扇形角α在Y轴负方向侧的简化封板;
		Y==2两块简化封板;
		Y==3表示直角封板;
		*/
		long nCirCoverPlateType;
		//流水孔参数
		BYTE biHoleType;	//0.圆孔|1.半圆弧|2.无孔
		double BH;			//流水孔直径（<0.无孔;0.半圆孔;>0.圆孔）
		double fEdgeSpace;		//螺栓边距
	}para;
	struct PARAM_WATER
	{
		double D;	//内环直径
		double W;	//外环直径
		double S;	//法向偏移距离
		double K;	//流水槽宽度
		double H;	//流水槽深度
		double L;	//流水槽长度
		double A;	//两流水槽之间的夹角
		double O;	//两流水槽角平分线与基准边之间的偏移角
		double E;	//定位角基准边:0.基准边1.焊道线
	}water;
	struct LAYOUT_BOLT
	{
		SEGI m_iBoltSegI;	//段号
		int m_iDirection;	//朝向
		int m_nBoltD;		//直径
		int m_nBoltNum;		//个数 
		//环向布置螺栓参数
		int m_iStartEdge;		//环向布置起始边,0.基准边,1.焊道线
		double m_fOffsetAngle;	//首螺栓相对于起始边之间的偏移角度
		double m_fBoltLayoutR;	//螺栓所在半径
		LAYOUT_BOLT(){memset(this,0,sizeof(LAYOUT_BOLT));}
	}m_xLayoutBolt;

// Dialog Data
	//{{AFX_DATA(CDesignCircularPlateDlg)
	enum { IDD = IDD_DESIGN_CIRCULAR_PLATE_DLG };
	CPropertyList	m_xSubPropList;
	CPropertyList	m_propList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignCircularPlateDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignCircularPlateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnConfirm();
	afx_msg void OnPartsDb();
	afx_msg void OnClose();
	afx_msg void OnBnModelViewOper();
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNCIRCULARPLATEDLG_H__3ECCA5E9_8DB1_4E12_93A3_B8C2CFBAA131__INCLUDED_)
