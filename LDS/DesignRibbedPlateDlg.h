#if !defined(AFX_DESIGNRIBBEDPLATEDLG_H__581B91A2_C138_4B12_9499_64CEEBAEC22D__INCLUDED_)
#define AFX_DESIGNRIBBEDPLATEDLG_H__581B91A2_C138_4B12_9499_64CEEBAEC22D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignRibbedPlateDlg.h : header file
//

#include "resource.h"
#include "PropertyList.h"
#include "CallBackDialog.h"

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
struct ANCHOR_PARAM{
	BYTE ciType;	//0.按键 1.VecRod;2.Bolt;3.VERTEX
	long hRelaObj;	//轮廓点或螺栓关联钢板
	union LOCATION{
		long hBolt;
		long hVecRod;
		BYTE ciTriggerType;	//0.左键单击拾取;1:右键中止;2:外围进程中止;3:回车键确认退出;4:ESC键中止
		struct VERTEX{
			long idVertex;
			double x,y;
		}vertex;
	}locate;
	ANCHOR_PARAM(){ciType=0;hRelaObj=0;locate.vertex.x=locate.vertex.y=locate.vertex.idVertex=0;}
};
// CDesignRibbedPlateDlg dialog

class CDesignRibbedPlateDlg : public CCallBackDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
// Construction
	CBitmap m_xCurPlateBmp;
public:
	typedef struct tagRIB_PLATE
	{	//钢板基本属性
		char sPartNo[16];
		char cMaterial;
		int nThick;
		SEGI nSegI;
		BYTE m_cPlateType;	//肋板类型 0.角肋板 1.槽肋板 2.角钢肋板 3.钢板肋板 4.钢管肋板
		BYTE m_cSubType;	//肋板子类型，如：1.角肋板－常规；2.角肋板－钢管肘板
		struct tagTUBE_PARA
		{	//基准构件为钢管时使用
			int nPlateNum;			//钢管肋板个数
			int iDatumEdge;			//0.基准边 1.焊道线 2.任意边(可调整工作法线)
			int iWorkNormStyle;		//0.垂直钢管延伸方向 1.钢管轴向 2.自定义
			double fInitOffsetAngle;//初始偏移角度 与延伸方向为互斥变量
			f3dPoint extend_vec;	//板延伸方向 钢板延伸方向与初始角度作用相同用来确定第一个肋板的延伸方向
			double fOffsetAngle;	//偏移角度
			double fBottomOutterR;	//底部外环半径(与辅助钢板1相连边的外侧半径)
			double fTopOutterR;		//顶部外环半径(与辅助钢板2相连边的外侧半径)
			BOOL bVerticalPlate;	//竖直且带自动正位的肋板
			double fOffsetDistM;	//轴向偏移距离M(无辅助构件的肋板需要该变量)
			double fOffsetDistX;	//X方向偏移距离(无辅助构件的肋板需要该变量)
			BOOL bAutoCalB;			//自动延伸到底板边缘
			int margin;				//指定底板边缘边距
			//
			BYTE cLeftTopCornerType;	//Y+顶部边角类型 0.倒角 1.圆弧 2.直角
			double fLeftTopCornerWidth;	//倒角时表示切角宽度，圆弧时表示切弧半径
			double fLeftTopCornerHeight;//倒角时表示切角宽度，圆弧时表示切弧半径
			BYTE cRightTopCornerType;	//Y-顶部边角类型 0.倒角 1.圆弧 2.直角
			double fRightTopCornerWidth;//倒角时表示切角宽度，圆弧时表示切弧半径
			double fRightTopCornerHeight;//倒角时表示切角宽度，圆弧时表示切弧半径
			tagTUBE_PARA(){memset(this,0,sizeof(tagTUBE_PARA));}
		}tube;
		struct tagPLATE_PARA
		{	//基准构件为钢板时使用
			BOOL bSpecOrg;				//指定坐标原点与指定定位基准构件二选一
			BOOL bSpecAxisX;			//钢板肋板指定X轴方向与指定构件方向杆件二选一，
			int  iAxisXType;			//0.X+|1.X-|2.Y+|3.Y-|4.输入..|5.自动计算
										//角肋板或槽肋板时无输入方向时根据辅助钢板计算X轴方向
			BOOL bSpecAxisY;			//与底板倾斜角度（即Y轴与底板间绕X轴的旋转角与方向基准杆件二选一）
			double fGradientDegAngle;	
			double fOriginX,fOriginY;	//肋板原点位置(基准钢板坐标系下的相对坐标)
			double fXAxisX,fXAxisY;		//肋板X轴方向(基准钢板坐标系下的相对坐标)
			double fOffsetDistX;	//X方向偏移距离(无辅助构件的肋板需要该变量)
			double fOffsetDistZ;	//方向偏移量
			BOOL bOnDatumPlateTopFace;	//基准构件为钢板时使用
			BOOL bAlongVecPart;		//钢板延伸方向与方向杆件同向
			//扩展功能后的新增参数 wjh-2016.5.4
			ANCHOR_PARAM anchors[4];
			double fBendTipOffsetX;	//X方向偏移距离(无辅助构件的肋板需要该变量)
			double fBendTipOffsetZ;	//方向偏移量
			tagPLATE_PARA(){
				memset(this,0,sizeof(tagPLATE_PARA));
				iAxisXType=0;
				fXAxisX=1;
				fXAxisY=0;
			}
		}plate;
		BOOL bOnRelaPlateTopFace;	//一块辅助钢板表示肋板位于钢板顶面一侧，无辅助构件表示与钢管延伸方向相同
		BOOL bAutoExtendOutterEdge;	//自动延伸至板外侧边缘
		double fTopWidth;		//顶部宽度
		double fBottomWidth;	//底部宽度
		double fLength;			//长度(为两钢板夹板时该长度自动计算)
		double fHeight;			//高度
		double fCirR;			//圆弧半径
		long iCutAngleMode;		//倒角模式：0.切角 1.切弧
		double fCutAngleWidth;	//切角宽度
		double fCutAngleWidth2;	//切角宽度2
		double MF,MG,MH;		//引流孔的孔距参数F,G与孔径H
		//辅助构件仅有一块钢板时该变量表示是否对称生成，辅助构件为两块钢板时表示同时生成外侧肋板
		BOOL bCreateOuterPlate;
		double fOuterPlateLen;		//外侧肋板长度
		double fOuterPlateHeight;	//外侧肋板高度
		union RIB_PARAMS{
			CLDSParamPlate::RIBPLATE_CORNER corner;	//钢管为底板的角肋板或槽肋板
			CLDSParamPlate::RIBPLATE_ELBOW elbow;	//钢管为基准的法兰或塔脚肘板
			CLDSParamPlate::RIBPLATE_PLATE platerib;//以钢板为底板(无其它侧向靠接构件)的普通肋板
			CLDSParamPlate::RIBPLATE_TUBE tuberib;	//以钢管为基准的轴向肋板(无其它侧向靠接构件)
			CLDSParamPlate::RIBPLATE_ANGLE anglerib;//角钢上的横断面肋板
		}params;
		tagRIB_PLATE(){
			memset(this,0,sizeof(tagRIB_PLATE));
			cMaterial='S';
			nThick=6;
			tube.nPlateNum=1;
			iCutAngleMode=1;	//默认切弧
			fCutAngleWidth=20;
			fCutAngleWidth2=20;
			fTopWidth=30;
			fHeight=30;
			fLength=300;
			fBottomWidth=200;
			fCirR = 30;
			MH=0;
			MF=MG=fCutAngleWidth+20;
		}
	}RIB_PLATE;
	UCS_STRU ucs;	//假定的肋板坐标系
	RIB_PLATE m_xRibPlate;
	CLDSNode *m_pDatumNode;
	CLDSBolt *m_pDatumBolt;
	CSuperSmartPtr<CLDSPart> m_pDatumPart,m_pRelaPart1,m_pRelaPart2,m_pVecPart;
	PARTSET ribPlateSet;	//记录通过旋转或对称生成的肋板 wht 11-06-07
	CString m_sWndTitle;	//设置窗口标题 wht 11-06-07
	long m_idSubEventProp;  //记录触发中断的子属性ID,恢复窗口时使用
public:
	CDesignRibbedPlateDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL IsValidAxisX();			//判断输入的X轴方向是否合理
	BOOL AutoCalRibPlateWidth();	//自动计算肋板宽度
	void ReCalRibPlateUCS(BOOL bUpdateRibLengthParam=TRUE);		//重新计算肋板坐标系
	void ModifyPlateParam(WORD wKey,double fValue,char *sNote=NULL);
	void ModifyPlateParam(WORD wKey,long nValue,char *sNote=NULL);
	void DeletePlateParam(WORD wKey);
	void ReDesignPlate();
	void SetRibPlate(CLDSParamPlate *pRibPlate);
	CLDSParamPlate *GetRibPlate(){return m_pRibPlate;}
	void FinishSelectObjOper();		//完成选择对象的后续操作
	void UpdateSketchMap();
	//属性管理
	void InitPropHashtable();
	int GetPropValueStr(long id, char *valueStr);	//根据属性ID得到属性值
	int GetPropStr(long id, char *propStr);			//得到属性字符串
	bool CorrectPropItemName(const char* propKey,CItemInfo* lpInfo);
	CPropTreeItem* InsertItem(CPropertyList* pPropList,CPropTreeItem *pParent, const char* propKey,int controlType=0,BOOL bUpdateValueText=TRUE, int iInsertIndex=-1, BOOL bUpdate=0);
	CPropTreeItem* InsertOrUpdateItem(CPropertyList* pPropList,CPropTreeItem *pParent, const char* propKey,int controlType=0,BOOL bUpdateValueText=TRUE, int iInsertIndex=-1, BOOL bUpdate=0);
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static PROPLIST_ITEM* AddPropItem(char* propKey,PROPLIST_ITEM item){
		PROPLIST_ITEM* pItem=propHashtable.SetValue(propKey,item);
		pItem->propKeyStr.Copy(propKey);
		return pItem;
	}
	static long GetPropID(char* propStr);			//得到属性ID
private:
	GEPOINT m_xSpreadVec1,m_xSpreadVec2;
	BOOL m_bNewRibPlate;	//TRUE:生成肋板 FALSE:查看修改以有肋板属性
	CLDSParamPlate *m_pRibPlate;
	BOOL LoadParamsFrom(CLDSParamPlate* pParamPlate);
	//BOOL SaveParamsTo(CLDSParamPlate* pParamPlate);
	void DisplayPropertyList();
	void FillParamToPlate(CLDSParamPlate *pRibPlate);
public:
	static const int TYPE_PLANE_SPREAD	=1;	//面展开方向
	static const int TYPE_PLANE_NORMAL	=2;	//面法线方向
	static const int TYPE_TUBE_SPREAD	=3;	//钢管轴方向
	static const int TYPE_X_AXIS_VECTOR =4;	//X轴方向
	static const int TYPE_Y_AXIS_VECTOR =5;	//Y轴方向
	void WorkPlaneSketch(int iType);
	void DisplaySubPropList(CPropTreeItem *pItem,BOOL bUpdate=FALSE);
	BOOL RedrawSketchDrawing(CPropTreeItem *pItem,UCS_STRU *pObjectCS=NULL,bool removeOldEntities=true);
	bool IsElbowRibPlate();
	BOOL IsNewRibPlate(){return m_bNewRibPlate;}
	void InitRibPlatePara();
	UINT _InitWaterHoleRadiusByThick();
	void DelWorkPlaneSketch();
	CLDSParamPlate *CreateRibPlate();
// Dialog Data
	//{{AFX_DATA(CDesignRibbedPlateDlg)
	enum { IDD = IDD_DESIGN_RIBBED_PLATE_DLG };
	CPropertyList	m_propList;
	CPropertyList	m_xSubPropList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignRibbedPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignRibbedPlateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnConfirm();
	afx_msg void OnPartsDb();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnModelViewOper();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNRIBBEDPLATEDLG_H__581B91A2_C138_4B12_9499_64CEEBAEC22D__INCLUDED_)
