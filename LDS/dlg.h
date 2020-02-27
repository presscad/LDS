#ifndef __DLG_H_
#define __DLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CConfirmJgStyleDlg dialog
#include "lds_part.h"
#include "resource.h"       // main symbols
#include "CallBackDialog.h"
#include "RememberLocationDlg.h"

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
class CConfirmJgStyleDlg : public CRememberLocationDlg
{
// Construction
public:
	CConfirmJgStyleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfirmJgStyleDlg)
	enum { IDD = IDD_CONFIRM_JG_STYLE_DLG };
	int		m_nJgStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmJgStyleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmJgStyleDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinkJGDlg dialog

class CLinkJGDlg : public CRememberLocationDlg
{
// Construction
	bool m_bDisconnectSegIAndPartNo;	//断开段号与构件编号间的联动关系
	static CHashList<char> hashPreferAngleMat;	//根据规格标识(如25020)判断的优先角钢材质简化字符
	static void AddPreferAngleMat(DWORD nSpec,char cMaterial);
	static int  SelectPreferAngleMatIndex(DWORD nSpec,int iMaterial);
public:
	static void InitPreferAngleMat(IModel* pModel=NULL);
public:
	double m_fFillThick;
	WORD wSectStyle;
	BYTE ciMasterIndex;	//组合角钢专用属性
	int m_nViewFlag;
	CLinkJGDlg(CWnd* pParent = NULL);   // standard constructor
	char sPartNoArr[4][4][16];	//[...][][]对称位置;[][...][]子角钢序列号
	MIRMSG mirmsg;
	int xy1_yz2_xz3;	//1-XY对称重复 2-YZ对称重复 3-XZ对称重复 其余表示无重复对称
	CLDSNode *m_pStartNode;
	CLDSNode *m_pEndNode;
	char m_sBoltGrade[5];	//螺栓级别 wht 13-01-25
// Dialog Data
	//{{AFX_DATA(CLinkJGDlg)
	enum { IDD = IDD_LINK_TA_JG_DLG };
	int		m_nJgMaterial;
	int		m_nStartLsNum;
	int		m_nEndLsNum;
	CString	m_sEndLsGuiGe;
	CString	m_sStartLsGuiGe;
	BOOL	m_bMirSideFace;
	CString	m_sOriginPartNo;
	CString	m_sMirXOriginPartNo;
	CString	m_sMirYOriginPartNo;
	CString	m_sMirDiagOriginPartNo;
	CString	m_sJgGuiGe;
	BOOL	m_bGroupJg;
	BOOL	m_bStartHoriOffset;
	BOOL	m_bEndHoriOffset;
	BOOL	m_bStartToBer;
	BOOL	m_bEndToBer;
	CString m_sSegNo;
	int		m_iAngleGradeType;
	int		m_iAngleLocation;
	BOOL	m_bMirAxisX;
	BOOL	m_bMirAxisXYZ;
	BOOL	m_bMirAxisY;
	BOOL	m_bMirAxisZ;
	BOOL m_bConnect1Wing;
	BOOL m_bConnect2Wing;
	BOOL m_bAutoMatchMat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkJGDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FireCmbAngleSizeChanged(const char* szSizeSpec);
	// Generated message map functions
	//{{AFX_MSG(CLinkJGDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	afx_msg void OnEditChangeCmbJgGuiege();
	afx_msg void OnSelchangeCmbJgGuiege();
	afx_msg void OnChangeEOriginPartNo();
	afx_msg void OnChangeEditStartLsNum();
	afx_msg void OnChangeEditEndLsNum();
	afx_msg void OnChangeCmbStartLsGuiege();
	afx_msg void OnChangeEMirXOriginPartNo();
	afx_msg void OnBnGroupStyle();
	afx_msg void OnChkGroupJg();
	afx_msg void OnMirAxisXyz();
	afx_msg void OnMirAxisX();
	afx_msg void OnMirAxisY();
	afx_msg void OnMirAxisZ();
	afx_msg void OnBnClickedChk1WingBoltN();
	afx_msg void OnBnClickedChk2WingBoltN();
	afx_msg void OnBnClickedJgPlaceStyle();
	afx_msg void OnEnChangeESegNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_nWingG;
	int m_iJgStyle;//摆放方式1、4为外铁，2、3为里铁
	void CalculateJgStyle();
	CXhChar16 GetLayer();
	int m_iMirLayStyle;
};

/////////////////////////////////////////////////////////////////////////////
// COffsetPtDlg dialog

class COffsetPtDlg : public CDialog
{
// Construction
public:
	BOOL m_bUpOffset;
	BOOL m_bDownOffset;
	COffsetPtDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COffsetPtDlg)
	enum { IDD = IDD_OFFSET_PT_DLG };
	CEdit	m_editDown;
	CEdit	m_editUp;
	double	m_fUpOffset;
	double	m_fDownOffset;
	int		m_nMirMsg;
	BOOL	m_bOffsetByAxisZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COffsetPtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COffsetPtDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CInterJointLsDlg dialog

class CInterJointLsDlg : public CDialog
{
// Construction
public:
	CInterJointLsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInterJointLsDlg)
	enum { IDD = IDD_INTER_JOINT_LS_D_DLG };
	CString	m_sLsGuiGe;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInterJointLsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInterJointLsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSelJdbFaceNDlg dialog

class CSelJdbFaceNDlg : public CDialog
{
// Construction
public:
	CSelJdbFaceNDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelJdbFaceNDlg)
	enum { IDD = IDD_SEL_PLANK_FACE_N_DLG };
	int		m_iJdbFaceType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelJdbFaceNDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelJdbFaceNDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CSpecLsDesignSetDlg dialog

class CSpecLsDesignSetDlg : public CDialog
{
// Construction
public:
	CSpecLsDesignSetDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bNoPlank;

// Dialog Data
	//{{AFX_DATA(CSpecLsDesignSetDlg)
	enum { IDD = IDD_NO_PLANK_SET_DLG };
	int		m_nZhunJu1;
	int		m_nZhunJu2;
	int		m_nJointPos;
	int		m_nZhunJu;
	CString	m_sBaseJgGuiGe;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecLsDesignSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpecLsDesignSetDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CFindPartDlg dialog

class CFindPartDlg : public CRememberLocationDlg
{
// Construction
public:
	CFindPartDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindPartDlg)
	enum { IDD = IDD_FIND_PART_DLG };
	BOOL	m_bDisplayProperty;
	long	m_nPartHandle;
	CString	m_sHexPartHandle;
	//}}AFX_DATA
	BOOL m_bDisplayPropertyVisible;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindPartDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindPartDlg)
	afx_msg void OnSynchronizeDec();
	afx_msg void OnSynchronizeHex();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CHorizonNodeDlg dialog

class CHorizonNodeDlg : public CDialog
{
// Construction
public:
	CHorizonNodeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHorizonNodeDlg)
	enum { IDD = IDD_DEF_HORIZON_NODE };
	int		m_nMirType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHorizonNodeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHorizonNodeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CSpecViewDirectDlg dialog

class CSpecViewDirectDlg : public CDialog
{
// Construction
public:
	CSpecViewDirectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpecViewDirectDlg)
	enum { IDD = IDD_SPECIFY_VIEW_DIRECT_DLG };
	int		m_nViewDirect;
	int		m_nWingLayStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecViewDirectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpecViewDirectDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CVFaceJdbDlg dialog

class CVFaceJdbDlg : public CDialog
{
// Construction
public:
	CVFaceJdbDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVFaceJdbDlg)
	enum { IDD = IDD_V_FACE_JDB_DLG };
	int		m_nBaseLsN;
	double	m_fZhunOffset;
	int		m_nMaterial;
	CString	m_sBaseLsGuiGe;
	int		m_nPlankThick;
	int		m_nHuoQuVecStyle;
	double	m_fHuoQuOffset;
	int		m_iZhun0HuoQu1;
	CString	m_sOriginNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVFaceJdbDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVFaceJdbDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CSharePlankDesignDlg dialog

class CSharePlankDesignDlg : public CRememberLocationDlg
{
// Construction
public:
	CSharePlankDesignDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSharePlankDesignDlg)
	enum { IDD = IDD_SHARE_PLANK_DLG };
	UINT	m_nLsN;
	UINT	m_nLsSpace;
	double	m_fOddment;
	CString	m_sLsGuiGe;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSharePlankDesignDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSharePlankDesignDlg)
	afx_msg void OnChangeCmbLsGuige();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CLsGuiGeDlg dialog

class CLsGuiGeDlg : public CDialog
{
// Construction
public:
	CLsGuiGeDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CLsGuiGeDlg)
	enum { IDD = IDD_SEL_LS_GUIGE_DLG };
	CString	m_sLsGuiGe;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLsGuiGeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLsGuiGeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CModifyPartHandleDlg dialog
class CModifyPartHandleDlg : public CCallBackDialog
{
// Construction
public:
	BOOL m_bEnableMir;
	CString m_sCaption;
	long m_nHandle;
	BOOL m_bCanZero;
	CLDSNode* m_pInvalidNode;	//修改杆件端节点时,可设定另一端节点为禁止节点,以防端点重合 wjh-2015.3.4
	//LDSCLASS_TYPEID m_idInputObjClsType;
	CModifyPartHandleDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bDispSelectBtn;	//是否显示选择构件的按钮
	void SetPickObjTypeId(int cls_id);
	void SetCmdPickPromptStr(CString sPromptStr);
// Dialog Data
	//{{AFX_DATA(CModifyPartHandleDlg)
	enum { IDD = IDD_MODIFY_PART_HANDLE_DLG };
	CString	m_sTitle;
	int		m_iMirMsg;
	CString	m_sHandle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyPartHandleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModifyPartHandleDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectObject();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CShankInputStyleDlg dialog

class CShankInputStyleDlg : public CDialog
{
// Construction
public:
	CShankInputStyleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShankInputStyleDlg)
	enum { IDD = IDD_SHANK_INPUT_STYLE_DLG };
	int		m_iShankInputStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShankInputStyleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShankInputStyleDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CMirMsgDlg dialog

class CMirMsgDlg : public CRememberLocationDlg
{
// Construction
	void AdjustWndPosition();
public:
	MIRMSG mirmsg;
	CMirMsgDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL    m_bUseMirrorSymmetry;
// Dialog Data
	//{{AFX_DATA(CMirMsgDlg)
	enum { IDD = IDD_MIR_MSG_DLG };
	double	m_fOriginX;
	double	m_fOriginY;
	double	m_fOriginZ;
	int		m_nArrayNum;
	BOOL	m_bMirAxisX;
	BOOL	m_bMirAxisY;
	BOOL	m_bMirAxisZ;
	BOOL	m_bMirRotation;
	BOOL	m_bMirAxisXYZ;
	BOOL	m_bAdvancedOption;
	BOOL	m_b45MirrorsSymmetry;
	BOOL    m_b135MirrorSymmetry;
	short	m_nRotateAngle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMirMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMirMsgDlg)
	afx_msg void OnBnPasteOrigin();
	virtual BOOL OnInitDialog();
	afx_msg void OnChkMirRotate();
	afx_msg void OnChkMirAxisXyz();
	afx_msg void OnChkMirAxisXyz2();
	afx_msg void OnChkMir45Plane();
	afx_msg void OnChkMir135Plane();
	afx_msg void OnAdvancedOption();
	afx_msg void OnOk();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CHuoQuOffsetDlg dialog

class CHuoQuOffsetDlg : public CRememberLocationDlg
{
// Construction
public:
	CHuoQuOffsetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHuoQuOffsetDlg)
	enum { IDD = IDD_INPUT_HUOQU_OFFSET_DLG };
	double	m_fHuoQuOffset;
	int		m_nByStart;
	double	m_fHuoQuR;
	int		m_iHuoQuBaseWing;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHuoQuOffsetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHuoQuOffsetDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CExtendJgDlg dialog

class CExtendJgDlg : public CDialog
{
// Construction
public:
	CExtendJgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExtendJgDlg)
	enum { IDD = IDD_EXTEND_JG_DLG };
	double	m_fExtendDist;
	int		m_nExtendStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtendJgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExtendJgDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWTADLG_H__B2209861_5257_11D4_8E40_0000B4C6F04A__INCLUDED_)
