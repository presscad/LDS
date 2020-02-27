#if !defined(AFX_FOOTDESIGNDLG_H__E0511EA0_DF85_48FE_B578_1400D262BA21__INCLUDED_)
#define AFX_FOOTDESIGNDLG_H__E0511EA0_DF85_48FE_B578_1400D262BA21__INCLUDED_
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FootDesignDlg.h : header file
//
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "lds_part.h"
#include "lds_co_part.h"
#include "footparasetdlg.h"
#include "EntityDataBase.h"
#include "f_ent.h"
#include "Markup.h"
//#include "Draw3d.h"
/////////////////////////////////////////////////////////////////////////////
// CFootDesignDlg dialog
struct LEI_BAN{
	char cMaterial;	//'S':Q235;'H':Q345
	double fRayDist;	//射线距离
	double wide;		//肋板宽
	//BOOL X_FALSE_Y_TRUE;		//肋板方向底座X轴向或Y轴向
	BOOL bEnableSecondDatumXueBan;
	int thick,high;
	char sPartNo[MAX_CHAR_GUIGE_16+1];
	int A,B;					//外形参数
	double water_slot_wide;		//流水槽宽
	double datumXueBanAngle;		//基准靴板角度，角度制
	double fGradientAngle;		//助板倾斜角度，弧度制
};
struct DIJIAO_LS{
	int d;
	double hole_d;
	f3dPoint pos;
};
struct TEMP_FOOT_DESIGN_PARASET{
	int C,m_iUpLsLayStyle,m_iDownLsLayStyle;
	bool bUpJg;
	double wing_wide,wing_thick;
	f3dPoint vert_arr[6],xueban_vert_arr[4];
	f3dPoint face_pick,wing_vec_x,wing_vec_y,jg_vec;
	CLDSPlate *pXUpFrontPlank,*pXUpBackPlank,*pYUpFrontPlank,*pYUpBackPlank;
	CLDSPlate *pXDownFrontPlank,*pXDownBackPlank,*pYDownFrontPlank,*pYDownBackPlank;
	CLDSPlate* upXueBan[8],*downXueBan[8];
};
class CFootDesignDlg : public CDialog
{
	//CDrawEngine *m_pDraw;
// Construction
public:
	CFootDesignDlg(CWnd* pParent = NULL);   // standard constructor
	FOOT_PARA_STRU foot_para;
// Dialog Data
	//{{AFX_DATA(CFootDesignDlg)
	enum { IDD = IDD_FOOT_PLANK_DESIGN_DLG };
	int		m_iCurOper;
	BOOL	m_bSaveData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFootDesignDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFootDesignDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnAddDijiaoLs();
	afx_msg void OnBnAddLeiBan();
	afx_msg void OnBnModifyLeiBan();
	afx_msg void OnBnModifyLs();
	afx_msg void OnBnParamSet();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClearDesResult();
	afx_msg void OnDestroy();
	afx_msg void OnBnImPortXMLFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void InitGraph();
	BOOL DesignDiBan();
	bool DesignXueBan();
	BOOL DesignLeiBan();
	void GetYInsideFace(f3dPoint *face_pick, f3dPoint *face_norm,BOOL bUpper=TRUE);
	void GetXInsideFace(f3dPoint *face_pick, f3dPoint *face_norm,BOOL bUpper=TRUE);
	void GetYOutsideFace(f3dPoint *face_pick, f3dPoint *face_norm,BOOL bUpper=TRUE);
	void GetXOutsideFace(f3dPoint *face_pick, f3dPoint *face_norm,BOOL bUpper=TRUE);
	//CLsList lslist;
	ATOM_LIST<f3dLine> leiBanDatumLineList;

	UCS_STRU rgn_ucs;
	CRect m_rectValidRgn;
	CEntityDataBase ent_base;
	ATOM_LIST<LEI_BAN>leiban_list;
	ATOM_LIST<DIJIAO_LS>dijiaols_list;
	CPoint m_oldPoint,m_curPoint;
	OPER_TYPE oper_type;
	SNAP_TYPE snap_type;
	double m_fScaleScreenToUser,m_fStaticScaleScreenToUser;
	HCURSOR m_hOldCursor,m_hCursor;
	BOOL m_bUpAxisXSameToBaseWingX,m_bDownAxisXSameToBaseWingX;
public:
	//angle是角度制
	double AngleFromFootToUpJg(double angle);
	//angle是角度制
	double AngleFromFootToDownJg(double angle);
	//angle是角度制
	f3dPoint RayVecFromUpJgAngle(double angle);
	//angle是角度制
	f3dPoint RayVecFromDownJgAngle(double angle);
	BOOL DesignFoot(CLDSNode *pMirNode=NULL,CLDSLineAngle *pMirJg=NULL);
	void RefreshRgn();
	void RefreshCurOper();
	//塔脚板数据的导入导出
	BOOL XMLDataSerialize(CString sFileName, BOOL bWrite);
	bool ImportFootDataFromXML(CMarkup xml,CString sElemName,BOOL bUpXueBan=FALSE);
	bool ExportFootDataToXML(CMarkup& xml);
	//
	CFoot *m_pFoot;
	BOOL m_bLMouseDown;
	CPen solid_pen,green_solid_pen,green_dash_pen,red_solid_pen,wide_solid_pen,dash_pen,dash_dot_pen;
private:
	// 设计底脚板过程中临时用到的一些过程参数(如:标定底座板上/下平面的点),可能随计算过程而变化
	TEMP_FOOT_DESIGN_PARASET tm;
	//  获得此底脚板(牛碲板)所连接的角钢集
	bool GetLinkJgSet(JGSET& jgset);
	// 获得上下两根基准角钢
	bool GetBaseJg(CLDSLineAngle *& pUpBaseJg, CLDSLineAngle *& pDownBaseJg);
	// 计算基准角钢的正负头及切角切肢
	void CalBaseJgOddment(CLDSLineAngle * pBaseJg);
	void CalBaseJgPlankIntVertex(CLDSLineAngle *pBaseJg, f3dPoint vert_arr[6],f3dPoint xueban_vert_arr[4]);
	// 设定靴板的定位坐标系及轮廓顶点坐标等参数
	void CalXueBanPara(CLDSLineAngle *pBaseJg);
	// 布置基准角钢上的螺栓
	void LayBaseJgLs(CLDSLineAngle * pBaseJg);
	// 预设射线角钢摆放位置
	double PresetRayJgEndPos(CLDSLineAngle *pAngle,CLDSLineAngle *pBaseAngle);
	// 布置射线角钢螺栓
	bool LayRayAngleBolt(ATOM_LIST<CDesignLsPara> *pLsList,CLDSLineAngle *pRayAngle,CLDSLineAngle *pBaseAngle,
		CLDSPlate *pPlate,double *ber_space,double *wing_space);
	// 设计底脚板(牛蹄板)处的射线材角钢
	bool DesRayJg(JGSET &jgset,CLDSLineAngle *pUpBaseJg,CLDSLineAngle *pDownBaseJg);
};

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOOTDESIGNDLG_H__E0511EA0_DF85_48FE_B578_1400D262BA21__INCLUDED_)
