#if !defined(AFX_SELJGPOINTKEYDLG_H__077EE543_FD28_498E_885A_F7C89325F6AA__INCLUDED_)
#define AFX_SELJGPOINTKEYDLG_H__077EE543_FD28_498E_885A_F7C89325F6AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelJgPointKeyDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
#include "RememberLocationDlg.h"
#include "KeypointLifeObj.h"
#include "OriginalData.h"
/////////////////////////////////////////////////////////////////////////////
// CSelJgPointKeyDlg dialog

class CSelJgPointKeyDlg : public CRememberLocationDlg
{
	CLDSPoint m_xInterPoint;
	CAngleDisplayLifeObject display;
// Construction
public:
	CSelJgPointKeyDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSLineAngle *m_pJg;
	CLDSNode *m_pNode;

// Dialog Data
	//{{AFX_DATA(CSelJgPointKeyDlg)
	enum { IDD = IDD_SEL_JG_KEY_POINT_DLG };
	int		m_iKeyPoint;
	int		m_iOffsetStyle;
	double	m_fPosX;
	double	m_fPosY;
	double	m_fPosZ;
	double	m_fLenOffset;
	double	m_fSpecGVal;
	//}}AFX_DATA
	BOOL Create();
	bool UpdateAndShowDialog(CLDSPoint* pPoint=NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelJgPointKeyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	void InitDefaultSizeCheckItem();
	bool UpdateDefaultAngleKeyPoint();
	bool UpdateAngleKeyPoint(CLDSPoint* pPoint);
	void RedrawAngleSectionKeyPointDrawing();

// Implementation
protected:
	CDataCheckModel::SIZE_DATA defaultSizeItem;
	// Generated message map functions
	//{{AFX_MSG(CSelJgPointKeyDlg)
	afx_msg void OnSelchangeCmbKeyPoint();
	afx_msg void OnSelchangeCmbOffsetStyle();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnCopyPnt();
	afx_msg void OnChangeELenOffset();
	afx_msg void OnChangeESpecGValue();
	afx_msg void OnBnChkCoordX();
	afx_msg void OnBnChkCoordY();
	afx_msg void OnBnChkCoordZ();
	afx_msg void OnEnChangeEChkCoordY();
	afx_msg void OnEnChangeEChkCoordZ();
	afx_msg void OnEnChangeEChkCoordX();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CDataCheckModel::KEYPOINT RetrieveKeyPoint(BOOL bUpdateData=FALSE);
	virtual void OnCancel();
	virtual void OnOK();
public:
	BOOL m_bChkCoordX;
	BOOL m_bChkCoordY;
	BOOL m_bChkCoordZ;
	double m_fChkCoordX;
	double m_fChkCoordY;
	double m_fChkCoordZ;
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELJGPOINTKEYDLG_H__077EE543_FD28_498E_885A_F7C89325F6AA__INCLUDED_)
