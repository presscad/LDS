#if !defined(AFX_JGBERPOSDESIGNDLG_H__B8EB0AB4_1A82_491C_BEEB_4C7573DB4B24__INCLUDED_)
#define AFX_JGBERPOSDESIGNDLG_H__B8EB0AB4_1A82_491C_BEEB_4C7573DB4B24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JgBerPosDesignDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJgBerPosDesignDlg dialog

class CJgBerPosDesignDlg : public CDialog
{
// Construction
public:
	void RefreshDisplayStatus();
	BOOL m_bEnableDaJiePosJg;	//是否允许手动指定基点定位角钢
	bool m_bDatumPosEnd;		//是否处于基准位置
	JGZJ wing_x_jgzj,wing_y_jgzj;
	double m_fStartJgLengOffsetX;
	double m_fStartJgLengOffsetY;
	double m_fEndJgLengOffsetX;
	double m_fEndJgLengOffsetY;
	CJgBerPosDesignDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CJgBerPosDesignDlg)
	enum { IDD = IDD_DESIGN_JGBER_POS_DLG };
	double	m_fOffsetWingYDist;
	double	m_fOffsetWingXDist;
	int		m_iWingXOffsetStyle;
	int		m_iWingYOffsetStyle;
	CString	m_hHuoQuBaseJg;
	double	m_fHuoQuDist;
	double	m_fJointX;
	double	m_fJointY;
	double	m_fJointZ;
	double	m_fOffsetZhun;
	CString	m_hDaJiePosJg;
	double	m_fDatumNormOffset;
	int		m_iDatumPosStyle;
	int		m_iDatumToJgBerStyle;
	int		m_iJgBerCalStyle;
	//}}AFX_DATA
	int m_iBaseOffsetStyle;			//根据另一端进行平推计算
	CLDSPoint *m_pPoint;
	f3dPoint face_offset_norm;
	THANDLE m_hStartJg,m_hEndJg;
	BOOL m_iStartJgBerStyle,m_iEndJgBerStyle;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJgBerPosDesignDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJgBerPosDesignDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCmbWingXOffsetStyle();
	afx_msg void OnSelchangeCmbWingYOffsetStyle();
	afx_msg void OnBnPtLineDatum();
	afx_msg void OnBnCopy();
	afx_msg void OnBnPaste();
	afx_msg void OnSelchangeCmbDatumPosStyle();
	afx_msg void OnBnOffsetFacePara();
	afx_msg void OnSelchangeCmbDatumToBerStyle();
	afx_msg void OnSelchangeCmbJgberCalStyle();
	afx_msg void OnBnSpecBasePos();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JGBERPOSDESIGNDLG_H__B8EB0AB4_1A82_491C_BEEB_4C7573DB4B24__INCLUDED_)
