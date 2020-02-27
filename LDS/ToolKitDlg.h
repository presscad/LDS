//<LOCALE_TRANSLATE Confirm by hxr/>
#if !defined(AFX_TOOLKITDLG_H__D5082355_DE9E_4696_BA19_107853B8815F__INCLUDED_)
#define AFX_TOOLKITDLG_H__D5082355_DE9E_4696_BA19_107853B8815F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolKitDlg.h : header file
//

#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "PropertyList.h"
#include "env_def.h"
#include "CallBackDialog.h"
#include "GlobalFunc.h"
/////////////////////////////////////////////////////////////////////////////
// CToolKitDlg dialog

#define VEC_OFFSET_ID_COEF		100000	//矢量偏移ID系数

//圆弧预拱
struct OPER_PRE_ARCLIFT
{
	char rot_axis;		//'X'或'Y'
	double lift_height;	//预拱高度
	f3dPoint rot_center;//旋转基点
	f3dPoint lift_pos;	//预拱基准位置(该位置预拱lift_height值)
	f3dPoint pick,result;	//预拱点预拱前后位置
	void Cal();
};
//平台计算(常用于平台预拱计算)
#ifndef __TSA_
struct OPER_BOARDCAL
{
	long hPoleGrad,hPoleHori;		//斜拉杆件,水平杆件句柄
	double vert_high,hori_space;	//横向间隙,垂直高度
	double grad_offset,left_hori_offset,board_len;	//斜杆偏移量,水平杆偏移量,平台长度(计算结果)
	f3dPoint corner_up;				//角钢右上角点位置(工作肢心线基点,计算结果)
	f3dPoint corner_down;			//右下角角钢楞点位置(计算结果)
	//
	BOOL Cal(char *error_info);
};
#endif
struct TOOLKIT_OPER_DATA
{
	//空间两点距离公式
	struct OPER_VEC_LEN{
		f3dPoint start,end;
		double result;
		double CalLen()
		{
			result = DISTANCE(start,end);
			return result;
		}
	}vec_len;
	//点到点矢量方向
	struct OPER_VEC_PtoP{
		f3dPoint start,end,result;
		f3dPoint CalVec()
		{
			result=end-start;
			normalize(result);
			return result;
		}
	}vec_p2p;
	//矢量夹角操作
	struct OPER_VEC_ANGLE{
		f3dPoint vec1,vec2;
		double rad_angle;
		double CalAngle()
		{
			f3dPoint vecStart=vec1,vecEnd=vec2;
			normalize(vecStart);
			normalize(vecEnd);
			rad_angle = cal_angle_of_2vec(vecStart,vecEnd);
			return rad_angle;
		}
	}vec_ang;
	//矢量偏移起始点
	f3dPoint offset_origin;
	//计算结果
	f3dPoint result;
	struct OPER_VEC_OFFSET{
		int iNo;
		double offset;
		f3dPoint vec;
	}vec_offset;	
	//坐标系装换
	struct OPER_CS_TRANS{
		UCS_STRU cs;
		f3dPoint src,dest;
		void CoordTrans(BOOL bUcsToWcs)
		{
			dest=src;
			coord_trans(dest,cs,bUcsToWcs);
		}
	}trans_cs;
	//三点共面
	struct OPER_3D_FACE3P{
		f3dPoint p[3],face_norm;
		void CopyPlane()
		{
			WritePlaneToClip(p[0],face_norm);
		}
		void CalPlane()
		{
			f3dPoint vec1,vec2;
			vec1=p[1]-p[0];
			normalize(vec1);
			vec2=p[2]-p[0];
			normalize(vec2);
			face_norm=vec1^vec2;
			normalize(face_norm);
		}
	}face3p;
	//两直线交点
	struct OPER_3D_INT_LL{
		f3dLine line1,line2;
		f3dPoint result;
		BOOL CalIntersPt(char *error_info)
		{
			int nRetCode = Int3dll(line1,line2,result);
			if(nRetCode==-2||nRetCode==1||nRetCode==2)	//直线外交点、直线内交点、直线段断点
				return TRUE;
			else 
			{
				if(error_info)
				{
					if(nRetCode==-1)
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(error_info,"Straight line end point overlap, the definition is wrong");
					else if(nRetCode==0)
						strcpy(error_info,"Straight line parallel,overlap or different plane");
					else 
						strcpy(error_info,"calculate result abnormal");
#else
						strcpy(error_info,"直线端点重合，定义有误");
					else if(nRetCode==0)
						strcpy(error_info,"直线平行、重合或异面");
					else 
						strcpy(error_info,"计算结果异常");
#endif
				}
				return FALSE;
			}
		}
	}inters_ll;
	//线面交点
	struct OPER_3D_INT_LF{
		f3dLine line;
		f3dPoint face_pos,face_norm;
		f3dPoint result;
		void PastPlane()
		{
			ReadClipPlane(face_pos,face_norm);
		}
		BOOL CalIntersPt(char *error_info)
		{
			int nRetCode = Int3dlf(result,line,face_pos,face_norm);
			if(nRetCode==1)	//得到有效交点
				return TRUE;
			else 
			{
				if(error_info)
				{
					if(nRetCode==-1)
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(error_info,"the definition of line or plane is wrong0");
					else if(nRetCode==0)
						strcpy(error_info,"line and plane are parallel or overlap");
					else 
						strcpy(error_info,"calculate result abnormal");
#else
						strcpy(error_info,"直线或面定义有误0");
					else if(nRetCode==0)
						strcpy(error_info,"线与面平行或重合");
					else 
						strcpy(error_info,"计算结果异常");
#endif
				}
				return FALSE;
			}
		}
	}inters_lf;
	//点到线的距离
	struct OPER_3D_DIST_PL
	{
		f3dPoint pt;			//拾取点
		f3dPoint line_start;	//线起点
		f3dPoint line_end;		//线终点
		double result;
		double Cal()
		{
			result=DistOf3dPtLine(pt,line_start,line_end);
			return result;
		}
	}dist_pl;
	//点到面的距离
	struct OPER_3D_DIST_PF
	{
		f3dPoint pt;		//拾取点
		f3dPoint face_pos;	//面基点
		f3dPoint face_norm;	//面法线
		double result;
		void PastPlane()
		{
			ReadClipPlane(face_pos,face_norm);
		}
		double Cal()
		{
			f3dPoint prj_pt=pt;
			project_point(prj_pt,face_pos,face_norm);
			result=DISTANCE(prj_pt,pt);
			return result;
		}
	}dist_pf;
	//圆弧预拱
	OPER_PRE_ARCLIFT arc_lift;
#ifndef __TSA_
	//平台计算(常用于平台预拱计算)
	OPER_BOARDCAL board;
#endif
};
class CToolKitDlg : public CCallBackDialog
{
	// Construction
	CBitmap m_xCurPlateBmp;
	int m_iMinAvailable;	//偏移方向最小可用编号
public:
	TOOLKIT_OPER_DATA m_xOperData;
	ATOM_LIST<TOOLKIT_OPER_DATA::OPER_VEC_OFFSET>listOffset;//矢量偏移时的偏移过程
	CToolKitDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Create();
	TOOLKIT_OPER_DATA::OPER_VEC_OFFSET *AddVecOffsetItem(f3dPoint vec,double offse_dist);
	f3dPoint CalVecOffsetResult();
	void UpdateSketchMap(CPropTreeItem *pItem);
// Dialog Data
	//{{AFX_DATA(CToolKitDlg)
	enum { IDD = IDD_TOOLKIT_DLG };
	CPropertyList	m_propList;
	CTabCtrl	m_tabCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolKitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InsertPointPropItem(CPropTreeItem *pParentItem,char *sIdProp,char *sPropName,BOOL bHideChild=FALSE);
	void DisplayItemList();
	void RefreshTabCtrl(int iCurSel);
	void ResetDlgSize(int iCurSel);
	void FinishSelectObjOper();

	// Generated message map functions
	//{{AFX_MSG(CToolKitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//属性管理
	static int m_iCurGroup;				//记录属性栏当前显示组的ID
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	CString GetPropValueStr(long id);				//根据属性ID得到属性值
	static void InitToolkitsPropHashtable();
	static long GetPropID(char* propStr);			//得到属性ID
	static int GetPropStatus(long id);				//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);	//设置属性状态
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLKITDLG_H__D5082355_DE9E_4696_BA19_107853B8815F__INCLUDED_)
