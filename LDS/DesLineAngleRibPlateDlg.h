#pragma once

#include "PropertyList.h"
#include "PropListItem.h"
#include "XhCharString.h"
#include "CallBackDialog.h"
// CDesLineAngleRibPlateDlg 对话框

class CDesLineAngleRibPlateDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesLineAngleRibPlateDlg)
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
	CBitmap m_xCurPlateBmp;
public:
	//角钢肋板参数
	struct RIBPLATE_PARAM{
		double D;	//偏移距
		double L;	//X肢上长度
		double Ly;	//Y肢上长度
		double W;	//X肢上台阶高度
		double Wy;	//Y肢上台阶高度
		double S;	//切角宽度
		double H;	//内侧X向长度
		double Hy;	//内侧Y向长度
		double R;	//圆弧半径
		long   C;	//自由边类型 0.直边|1.圆弧
		long   E;	//倒角类型：0.直角|1.圆弧
		double d;	//孔径
		double P;	//孔距
		double m_fAngleWidth;	//角钢宽度，用来判断使用静态变量时是否重新计算肋板参数 wht 18-11-15
		//
		RIBPLATE_PARAM(){memset(this,0,sizeof(RIBPLATE_PARAM));}
	}para;
	//钢管拉线板参数
	struct STAYWIRE_RIBPLATE_PARA{
		BOOL bOnRelaPlateTopFace;	//
		double rotAngle;//环向定位角的旋转起始边自上述基准边或焊道线的额外定位偏转角,与肘板‘Y’参数对应，°
		long J;			//环向定位角的旋转起始边类型：0.基准边；1.焊道线
		long N;		//拉线孔数
		double L;	//钢管轴线的长度	
		double W;	//拉线板宽度
		double H;	//在辅助板测的焊接长度
		double R;	//倒角半径
		double d;	//孔径
		double P;	//孔距
		//
		STAYWIRE_RIBPLATE_PARA(){memset(this,0,sizeof(STAYWIRE_RIBPLATE_PARA));}
	}stay_wire_para;
	//钢管角肋板
	struct TUBE_CORNER_RIBPLATE{
		int thick;
		double fRotAngle;	//定位角
		double E;	//底部宽度
		double F;	//顶部宽度
		double G;	//内侧长度
		double H;	//外侧长度
		BYTE e;		//倒角模式
		double I;	//切角宽度
		double MF;
		double MG;	
		double MH;
		//
		TUBE_CORNER_RIBPLATE(){memset(this,0,sizeof(TUBE_CORNER_RIBPLATE));}
	}ass_rib_para;	
	//基本参数
	struct BASIC_PARRM 
	{
		SEGI nSegI;
		char cMaterial;
		int nThick;
		long cPlateType;
		//
		BASIC_PARRM() { memset(this, 0, sizeof(BASIC_PARRM)); }
	};
private:
	static bool m_bValidStaticRibParam;				//角钢肋板静态参数是否可用
	static bool m_bValidStaticStayWireParam;		//拉线板静态参数是否可用
	static bool m_bValidStaticAssRibParam;			//拉线板小肋板参数是否可用
	static RIBPLATE_PARAM s_xRibPlateParam;			//肋板参数
	static STAYWIRE_RIBPLATE_PARA s_xStayWireParam;	//拉线板参数
	static TUBE_CORNER_RIBPLATE s_xAssRibParam;		//拉线板肋板参数
	static BASIC_PARRM s_xBasicParam;				//肋板基本参数
public:
	CLDSLineTube* m_pDatumTube;
	CLDSGeneralPlate* m_pAssistPlate;
	CLDSLineAngle* m_pDatumAngle;
	CLDSBolt* m_pDatumBolt;
	CLDSNode* m_pDatumNode;
	CXhChar16 m_sPartNo;
	CXhChar16 m_sAssPartNo;
	SEGI m_nSegI;
	char m_cMaterial;
	int m_nThick;
	long m_cPlateType;	//肋板类型  0.内侧肋板|1.外侧肋板|2.纵向肋板
	long m_cNormCalType;	//法向计算方式 0.智能计算|1.人为指定
	long m_cDirection;		//延伸方向方式 0.始-终|1.终-始
	BOOL m_bNewRibPlate;
	CLDSParamPlate _previewRibPlate;
	CLDSParamPlate *m_pRibPlate;
	UCS_STRU ucs;
	long m_idSubEventProp;  //记录触发中断的子属性ID,恢复窗口时使用
	BOOL m_bNewAssRibPlate;	//拉线板两侧的钢管肋板
public:
	CDesLineAngleRibPlateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDesLineAngleRibPlateDlg();
	//
	void InitRibPlatePara();
	void AutoCalRibPlateWidth();
	CLDSParamPlate *CreateRibPlate(CLDSParamPlate* pToExistPlate=NULL);
	void ReDesignPlate();
	void FinishSelectObjOper();		//完成选择对象的后续操作
	void UpdateSketchMap(CPropTreeItem *pItem=NULL);
	void ModifyPlateParam(WORD wKey,double fValue);
	void ModifyPlateParam(WORD wKey,long nValue);
	void DeletePlateParam(WORD wKey);
	//属性管理
	DECLARE_PROP_FUNC(CDesLineAngleRibPlateDlg);
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值
	void DisplayPlatePropList();
	void DisplaySubPropList(CPropTreeItem *pItem=NULL);
	// 对话框数据
	enum { IDD = IDD_DES_LINEANGLE_RIBPLATE_DLG };
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
