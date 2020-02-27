#if !defined(AFX_DESIGNSIMPLELADDERDLG_H__139C3267_91F5_46E2_86B0_B1691D58D791__INCLUDED_)
#define AFX_DESIGNSIMPLELADDERDLG_H__139C3267_91F5_46E2_86B0_B1691D58D791__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignSimpleLadderDlg.h : header file
//
#include "PropertyList.h"
#include "XhCharString.h"
#include "CallBackDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CDesignSimpleLadderDlg dialog
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

class CDesignSimpleLadderDlg : public CCallBackDialog
{
	CBitmap m_CurPlateBmp;
	CStringArray m_arrPropGroupLabel;
public:
	//脚钉信息
	typedef struct tagFootNailPara{
		CXhChar16 sPartNo;		//短脚钉的编号
		CXhChar16 sLongPartNo;	//长脚钉的编号
		double fUpEndSpace;		//上端距(一般是短脚钉距端头的距离)
		double fDownEndSpace;	//下端距(一般是长脚钉距端头的距离)
		double fBetweenSpace;	//脚钉间距
		int    nNum;			//短脚钉数
		int iDirection;			//第一个短脚钉的朝向
		double short_grade,long_grade;
		double fShortLen;		//短脚钉的长度
		double fLongLen;		//长脚钉的长度
		int D;					//脚钉的直径
		tagFootNailPara(){memset(this,0,sizeof(tagFootNailPara));}
	}FOOT_NAIL_PARA;
	//连接板信息
	typedef struct tagPlatePara{
		CXhChar16 sPartNo;		//圆孔钢板的编号
		CXhChar16 sEllipsePartNo;//椭圆孔钢板的编号
		double fUpEndSpace;		//上端距(一般是圆孔钢板距上端头的距离)
		CXhChar100 sBetweenSpace;//椭圆孔钢板的间距(从上到下依次输入)
		int nPlateThick;		//连接板厚度
		double fCircinalPlateA;	//圆孔钢板的A
		double fCircinalPlateB;	//圆孔钢板的B
		double fEllipsePlateC;	//椭圆形孔钢板的C
		int iEllipseAperture;//腰圆孔长方形长度
		CXhChar16 sAnglePartNo;//连接角钢的编号
		double fLineAngleLen;	//连接角钢的长度
		double fDownEndSpace;//角钢距基准钢管开始端距离
		int iConnectType;		//连接方式
		double wide;
		double thick;
		char cMaterial;
		int iAngleDirect;//角钢法线朝向 0朝下1朝上
		int	nLSD;		//圆形孔的螺栓规格
		tagPlatePara(){memset(this,0,sizeof(tagPlatePara));}
	}PLATE_PARA;
	//钢管信息
	typedef struct tagLineTubePara{
		CXhChar16 sPartNo;
		double wide;
		double thick;
		CLDSLineTube *pBaseLineTube;
		double fDownSpace;	//距钢管开始段的距离
		double fAngle;		//和基准边之间的角度
		SEGI iSeg;
		tagLineTubePara(){memset(this,0,sizeof(tagLineTubePara));}
	}LINE_TUBE_PARA;
public:
	CDesignSimpleLadderDlg(CWnd* pParent = NULL);   // standard constructor
	//属性管理
	void InitPropHashtable();
	int GetPropValueStr(long id, char *valueStr);	//根据属性ID得到属性值
	int GetPropStr(long id, char *propStr);			//得到属性字符串
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	static long GetPropID(char* propStr);			//得到属性ID
	void SelectLineTube();//选择钢管
	void SetLineTube();//设置选中钢管
	
	void RefreshTabCtrl(int iCurSel);
	void ReDraw();	//刷新
	void SetCmdPickPromptStr(CString sPromptStr);
	//显示属性
	void DisplayPropertyList(int iCurSel=0);
	//创建爬梯
	BOOL CreateSimpleLadder();
	//创建爬梯连接板 iPlateType=0为圆孔板1为椭圆  
	BOOL CreateConnectPalte(int iPlateType,double fOffsetSpace,CLDSLineTube *pBaseLineTube,CLDSLineAngle *pLineAngle);
	//创建爬梯脚钉 iType=0向左的短脚钉=1向右的短脚钉2长脚钉
	BOOL CreateFootNail(CLDSLineTube *pLadderLineTube,double fOffsetSpace,int iType);
	//创建钢管
	BOOL CreateLineTube(double fLineTubeLen,CLDSLineTube *pLineTube,double fVectorLen);
	//创建连接角钢
	CLDSLineAngle* CreateConnectLineAngle(double fOffsetSpace);

public:
	//脚钉
	FOOT_NAIL_PARA FootNailPara;
	//连接板
	PLATE_PARA PlatePara;
	//钢管
	LINE_TUBE_PARA LineTubePara;
	//画笔
	CPen solid_pen,dash_pen,dash_dot_pen,green_solid_pen,green_dash_pen,
		red_solid_pen,blue_solid_pen,purple_solid_pen;
// Dialog Data
	//{{AFX_DATA(CDesignSimpleLadderDlg)
	enum { IDD = IDD_DESIGN_SIMPLE_LADDER_DLG };
	CPropertyList	m_propList;
	CTabCtrl	m_ctrlPropGroup;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignSimpleLadderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignSimpleLadderDlg)
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnConfirmCreate();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNSIMPLELADDERDLG_H__139C3267_91F5_46E2_86B0_B1691D58D791__INCLUDED_)
