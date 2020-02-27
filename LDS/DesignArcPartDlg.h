#if !defined(AFX_DESIGNARCPARTDLG_H__4CA97966_B310_437E_A2FB_7319E4CBDA0B__INCLUDED_)
#define AFX_DESIGNARCPARTDLG_H__4CA97966_B310_437E_A2FB_7319E4CBDA0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignArcPartDlg.h : header file
//

#include "PropertyList.h"
#include "CallBackDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CDesignArcPartDlg dialog

class CDesignArcPartDlg : public CCallBackDialog
{
// Construction
	const static int PARA_TYPE_CENTER	= 1;	//圆心
	const static int PARA_TYPE_NORM		= 2;	//法线
	const static int PARA_TYPE_ARCSTART	= 3;	//圆弧起始点
	//
	int m_iCurDesParaType;
	void DisplaySubPropList();
	void FinishSelectObjOper();
public:
	CBitmap m_CurPlateBmp;
	CDesignArcPartDlg(CWnd* pParent = NULL);   // standard constructor
	void RefreshDlgCtrlState();	//刷新控件状态
	void RefreshEditText();		//根据基准点或基准方向类型，更新对应单元格内容及只读状态
// Dialog Data
	//{{AFX_DATA(CDesignArcPartDlg)
	enum { IDD = IDD_DESIGN_ARC_PART };
	double	m_fWorkNormX;
	double	m_fWorkNormY;
	double	m_fWorkNormZ;
	double	m_fCenterX;
	double	m_fCenterY;
	double	m_fCenterZ;
	double	m_fStartPosX;
	double	m_fStartPosY;
	double	m_fStartPosZ;
	int		m_iMaterial;
	CString	m_sPartNo;
	CString m_sSegI;
	double	m_fStartAngle;
	int		m_iPartType;
	CString	m_sPartGuiGe;
	int		m_iLayoutStyle;
	CString	m_sGuiGeName;
	double	m_fOffsetH;
	double	m_fOffsetV;
	double	m_fArcAngle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignArcPartDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RetrievePreferOffsetParams(const char* sizespec_str);
	// Generated message map functions
	//{{AFX_MSG(CDesignArcPartDlg)
	afx_msg void OnBnPasteWorkNorm();
	afx_msg void OnBnPasteCenter();
	afx_msg void OnBnPasteStartPos();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnCopyWorkNorm();
	afx_msg void OnBnCopyCenter();
	afx_msg void OnBnCopyStartPos();
	afx_msg void OnPartType();
	afx_msg void OnChangePartNo();
	afx_msg void OnSelchangePartGuige();
	afx_msg void OnKillfocusPartGuige();
	afx_msg void OnSelchangeLayoutStyle();
	afx_msg void OnChangeCenterPosZ();
	afx_msg void OnChangeStartPosZ();
	afx_msg void OnChangeArcAngle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CPropertyList m_xPropList;
	CLDSPoint m_center,m_arcStart;	//环形构件圆心及起始点
	CLDSVector m_norm;				//环形构件法线方向
	afx_msg void OnEnSetFocusWorkNorm();
	afx_msg void OnEnSetFocusCenterPos();
	afx_msg void OnEnSetFocusStartPos();
	CString m_sCurObjType;
	afx_msg void OnBnClickedBtnConfirm();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNARCPARTDLG_H__4CA97966_B310_437E_A2FB_7319E4CBDA0B__INCLUDED_)
