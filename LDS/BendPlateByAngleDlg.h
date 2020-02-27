#if !defined(AFX_BENDPLATEBYANGLEDLG_H__B405F65B_AF2A_4A0F_92B1_18C705C6119A__INCLUDED_)
#define AFX_BENDPLATEBYANGLEDLG_H__B405F65B_AF2A_4A0F_92B1_18C705C6119A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BendPlateByAngleDlg.h : header file
//
#include "propertylist.h"
#include "PropListItem.h"
#include "XhListCtrl.h"
#include "RememberLocationDlg.h"
//////////////////////////////////////////////////////////////////////////
//DESIGN_GUAXIAN_HOLE
class DESIGN_GUAXIAN_HOLE
{
public: 
	struct GUAXIANHOLE_BASE_INFO	
	{	//挂线孔信息
		DWORD serial;			//序号
		int hole_d;				//孔径
		f3dPoint ls_pos;		//挂线孔位置
		double fDistX;			//相对坐标系的X偏移量
		double fDistY;			//相对坐标下的Y偏移量
		double fRadius;			//外圆半径
		double lengthPosToEdge;	//挂线孔在轮廓边的投影位置
		BOOL bDrillHole;		//钻孔
		//
		BOOL bHasTube;			//内衬套管
		struct TUBEINFO{
			char sPartNo[16];	//套管编号
			char guige[16];		//套管规格
			char cMaterial;		//套管材质
			TUBEINFO(){
				memset(sPartNo,0,16);
				memset(guige,0,16);
				cMaterial=CSteelMatLibrary::Q235BriefMark();;
			}
		}tube;
		//
		BOOL bWeldCirClePank;	//外焊环板
		struct CIRPLATEINFO{
			char sPartNo[16];	//环板编号
			char cMaterial;		//环板材质
			double fOffsetDist;	//高度差
			CIRPLATEINFO(){
				memset(sPartNo,0,16);
				cMaterial=CSteelMatLibrary::Q235BriefMark();
				fOffsetDist=0;
			}
		}cir_plate;
	public:
		GUAXIANHOLE_BASE_INFO();
		DWORD Serial() const{return serial;}
		void SetKey(DWORD key){serial=key;}
	};
	int m_iEdgeShapeType;		//挂线孔的外边类型 0.外凸圆行|1.波浪圆形|2.内凹角圆弧
	PROFILE_VERTEX* pStartVertex,*pEndVertex;
	//ARRAY_LIST<GUAXIANHOLE_BASE_INFO> guaXianHoleInfoArr;
	CHashListEx<GUAXIANHOLE_BASE_INFO> guaXianHoleHashList;
public:
	DESIGN_GUAXIAN_HOLE();
};
/////////////////////////////////////////////////////////////////////////////
// CBendPlateByAngleDlg dialog
class CBendPlateByAngleDlg : public CRememberLocationDlg
{
// Construction
public:
	int		m_iBendLineStyle;		//火曲线定位方式：0.角钢边缘|1.手动指定
	long    m_hBendLineAngle;		//火曲线依赖角钢
	int		m_iDatumEdge;			//0.楞线侧|1.肢翼侧
	double	m_fBendAngle;			//火曲角度
	double	m_fBendClearSpace;		//火曲线间隙
	DESIGN_GUAXIAN_HOLE design_guaxian_hole;
	CBendPlateByAngleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBendPlateByAngleDlg)
	enum { IDD = IDD_BEND_PLATE_BY_ANGLE_DLG };
	double m_fOriginX;		// 定位原点X坐标
	double m_fOriginY;		// 定位原点Y坐标
	int m_iIndex;		// 挂线孔序号
	int m_iHoleD;		// 孔径
	double m_fRadius;	// 挂线孔外缘半径
	double m_fPosX;		// 相对坐标系下的挂线孔X坐标
	double m_fPosY;		// 相对坐标系下的挂线孔Y坐标
	CString m_sTubePartNo;		// 套管编号
	CString m_sCirPlatePartNo;	// 环板编号
	double m_fOffsetDist;		// 环板高差
	int m_iTubeMaterial;		//套管材质
	int m_iTubeGuiGe;			//套管规格
	int m_iPlateMaterial;		//圆板材质
	BOOL m_bDrillHole;			// 是否钻孔
	BOOL m_bHaveTube;			// 内衬套管
	BOOL m_bHaveCirPlate;		// 外焊环板
	int m_iEdgeShapeType;		//外边类型
	CPropertyList m_xPropList;
	CXhListCtrl m_listCtrlRayAngles;
	CXhListCtrl m_listCtrlGuaXianHoles;	
	//}}AFX_DATA
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBendPlateByAngleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
// Implementation
public:
	void RefreshCtrlStatue();
	void DisplayPropertyList();
	//属性管理
	void InitPropHashtable();
	PROPLIST_ITEM* AddPropItem(char* keyStr,PROPLIST_ITEM item);
	int GetPropValueStr(long id, char *valueStr);	//根据属性ID得到属性值
	int GetPropStr(long id, char *propStr);			//得到属性字符串
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static long GetPropID(char* propStr);			//得到属性ID
	CItemInfo* AppendPropItemInfo(const char* propKey,int controlType=0,long* pPropId=NULL,BOOL bUpdateValueText=TRUE);
	CPropTreeItem* InsertItem(CPropTreeItem *pParent, const char* propKey,int controlType=0,BOOL bUpdateValueText=TRUE, int iInsertIndex=-1, BOOL bUpdate=0);
	CPropTreeItem* InsertOrUpdateItem(CPropTreeItem *pParent, const char* propKey,int controlType=0,BOOL bUpdateValueText=TRUE, int iInsertIndex=-1, BOOL bUpdate=0);
protected:

	// Generated message map functions
	//{{AFX_MSG(CBendPlateByAngleDlg)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnBnClickedBnAddGuaxianHole();
	afx_msg void OnBnClickedChkHaveTube();
	afx_msg void OnBnClickedChkHaveCirplate();
	afx_msg void OnNMDblclkListGuaXianHoles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListGuaXianHoles(NMHDR *pNMHDR, LRESULT *pResult);
	void ContextMenu(CWnd *pWnd, CPoint point);
	afx_msg void OnDeleteItem();
	afx_msg void OnBnClickedBnSelPosBolt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BENDPLATEBYANGLEDLG_H__B405F65B_AF2A_4A0F_92B1_18C705C6119A__INCLUDED_)
