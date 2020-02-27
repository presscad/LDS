#if !defined(AFX_FOOTNAILSPACEDLG_H__9FCCC071_8378_4A9F_A126_A64982A77FDB__INCLUDED_)
#define AFX_FOOTNAILSPACEDLG_H__9FCCC071_8378_4A9F_A126_A64982A77FDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddDianBanDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFootnailSpaceDlg dialog
#include "lds_part.h"
#include "CallBackDialog.h"
#include "NailResideAnglePage.h"
#include "NailResideTubePage.h"
#include "MemberProperty.h"
class CFootnailSpaceDlg : public CCallBackDialog
{
	CLDSBolt xNailBolt,xNailBolt2;	//xNailBolt2为双侧布置时，另一侧槽钢脚钉的预览显示
	//CLDSLineSlot xNailSlot;
	CLDSBolt* m_pReplacerBolt;
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
	CNailResideAnglePage pageOnAngle;
	CNailResideTubePage  pageOnTube;
	int m_iOldLayWing,m_iOldNailType;
// Construction
public:
	void InitState(int hCurRod, double CanUseLen,double SumLen,double lenOffset,double fAngle=0,double fAngle2=120);
	CFootnailSpaceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFootnailSpaceDlg)
	enum { IDD = IDD_FOOTNAIL_SPACE_DLG };
	int		m_nNailSpace;
	int		m_nDefaultNailSpace;
	double	m_fNailAngle;
	double	m_fNailAngle2;
	double	m_fCurNailAngle,m_fCurNailAngle2;
	CString	m_sCanUseLen;
	CString	m_sCurJg;
	CString	m_sSumLen;
	CString	m_sNailGuiGe;
	BOOL	m_bShiftWing;
	//int		m_iLayWing;
	int		m_iDatumLine;	//0--以基准边为基准 1--以焊道线为基准
	//int		m_iGType;	// 所在准距的选择类型:0,标准准距g,1,2,3,分别表示g1,g2,g3,4表示自定义
	//long	m_nGDist;	// 用户自定义的脚钉准距
	BOOL m_bSearchReplaceBolt;
	BOOL m_bHorizonSlotNail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFootnailSpaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CLDSBolt* SearchReplaceBolt(double preferStepSpace=420,bool bUpdateNailSpace=true);
	// Generated message map functions
	//{{AFX_MSG(CFootnailSpaceDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChkShiftWing();
	afx_msg void OnChkDualSide();
	afx_msg void OnChangeNailAngle();
	afx_msg void OnChangeNailAngle2();
	afx_msg void OnChangeCurNailAngle();
	afx_msg void OnRdoDatumLine();
	afx_msg void OnBnPickReplaceNailbolt();
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnChangeENailSpace();
	afx_msg void OnEnChangeDefaultNailSpace();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClickedChkSearchReplaceBolt();
	afx_msg void OnChkHorizonNail();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void PreviewNailBolt(BOOL bReFocus=TRUE);
public:
	void FireRdoLayWing(int iOldLayWing);
	void FireResideAngleDist();
	void FireRdoNailType(int iOldNailType);
public:
	// 角钢肢宽
	long wing_wide;
	JGZJ xWingXZhunJu,xWingYZhunJu;
	BOOL m_bEnableTeG;
	BOOL m_bHasWeldRoad;//是否存在焊道线
	int	 m_iLineType;	//基准构件类型 0--角钢 1--钢管 
	int  m_bDualSide;
	void RefreshDlgCtrlState();
	void AddNailGuigeRecord();
	void InitLineTubeFootNail(BOOL bNewCreate=FALSE);
	void InitLineAngleFootNail();
public:
	f3dPoint m_xDatumStart,m_xDatumLenVec;
	CSuperSmartPtr<CLDSLinePart> m_pRod;
	CLDSBolt* m_pMergeBolt;
	double m_fPreRodRemainingLen;	//前一杆件的剩余长度,跨杆件布置第一个螺栓时脚钉间距需要减去前一杆件剩余长度 wht 15-08-13
	long hFirstLineSlot;		//标准槽钢底座句柄
	CLDSLineSlot xNailSlot;
	CLDSLineAngle xNailAngle;
private:
	CLDSLineSlot xNailSlot2;	//双侧布置时，另一侧槽钢脚钉座的预览显示
public: //属性
	PROPERTY(int,m_iLayWing);
	GET(m_iLayWing){ return m_iLineType==0?pageOnAngle.m_iLayWing:pageOnTube.m_iLaySide; }
	SET(m_iLayWing){
		pageOnAngle.m_iLayWing=value;
		pageOnTube.m_iLaySide =value;
	}
	PROPERTY(int,m_iNailType);
	GET(m_iNailType){ return pageOnTube.m_iNailType; }
	SET(m_iNailType){ pageOnTube.m_iNailType =value; }
	//角钢上脚钉的心距
	PROPERTY(int,m_nGDist);
	GET(m_nGDist){return pageOnAngle.m_nGDist;}
	SET(m_nGDist){pageOnAngle.m_nGDist=value;}
private:
	double m_fLenUseDist;	//当前待布置螺栓自m_xDatumStart延m_xDatumLenVec的纵向已使用距离
	double m_fFocusZoomCoef;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOOTNAILSPACEDLG_H__9FCCC071_8378_4A9F_A126_A64982A77FDB__INCLUDED_)
