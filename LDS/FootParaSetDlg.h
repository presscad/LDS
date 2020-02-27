#if !defined(AFX_FOOTPARASETDLG_H__C4DA9304_F2FE_4042_AB6B_CA562E7E1C3C__INCLUDED_)
#define AFX_FOOTPARASETDLG_H__C4DA9304_F2FE_4042_AB6B_CA562E7E1C3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FootParaSetDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CFootParaSetDlg dialog
typedef struct tagFOOT_PARA_STRU{
	int m_iA;			//靴板台阶高
	int	m_iB;			//靴板前面平台宽
	int	m_iUpC;			//上侧角钢端棱点到底板距离(负值表指定负头)
	int m_iDownC;		//下侧角钢端棱点到底板距离(负值表指定负头)
	int m_iS;			//螺栓到中心线的距离
	int	m_iThick;		//底板厚
	int	m_iWide;		//底板宽
	int m_iUpXueBanThick,m_iDownXueBanThick;	//靴板厚
	int m_iUpLsLayStyle,m_iDownLsLayStyle;	//基准材布孔方式
	int m_iLjStyle;		//连接形式
	int m_iDiBanShape;	//0:表示为方板1:表示为圆板
	char m_cDiBanMat,m_cXueBanMat;	//'S':Q235;'H':Q345
	BOOL m_bWeldDiZuo;

	BOOL m_bBreakDownXueBan0C;
	BOOL m_bBreakDownXueBan90C;
	BOOL m_bBreakUpXueBan0C;
	BOOL m_bBreakUpXueBan90C;
	char sDiBanPartNo[MAX_CHAR_GUIGE_16+1];	//底板编号
	char sUpXueBanPartNo0C[MAX_CHAR_GUIGE_16+1];
	char sUpXueBanPartNo45C[MAX_CHAR_GUIGE_16+1];
	char sUpXueBanPartNo90C[MAX_CHAR_GUIGE_16+1];
	char sUpXueBanPartNo135C[MAX_CHAR_GUIGE_16+1];
	char sUpXueBanPartNo180C[MAX_CHAR_GUIGE_16+1];
	char sUpXueBanPartNo225C[MAX_CHAR_GUIGE_16+1];
	char sUpXueBanPartNo270C[MAX_CHAR_GUIGE_16+1];
	char sUpXueBanPartNo315C[MAX_CHAR_GUIGE_16+1];
	char sDownXueBanPartNo0C[MAX_CHAR_GUIGE_16+1];
	char sDownXueBanPartNo45C[MAX_CHAR_GUIGE_16+1];
	char sDownXueBanPartNo90C[MAX_CHAR_GUIGE_16+1];
	char sDownXueBanPartNo135C[MAX_CHAR_GUIGE_16+1];
	char sDownXueBanPartNo180C[MAX_CHAR_GUIGE_16+1];
	char sDownXueBanPartNo225C[MAX_CHAR_GUIGE_16+1];
	char sDownXueBanPartNo270C[MAX_CHAR_GUIGE_16+1];
	char sDownXueBanPartNo315C[MAX_CHAR_GUIGE_16+1];
}FOOT_PARA_STRU;

class CFootParaSetDlg : public CDialog
{
// Construction
public:
	CFoot* m_pFoot;
	CFootParaSetDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bUpAndDownSide;	//上下两侧都有靴板

// Dialog Data
	//{{AFX_DATA(CFootParaSetDlg)
	enum { IDD = IDD_FOOT_PLANK_PARA_SET_DLG };
	int		m_iA;
	int		m_iB;
	int		m_iUpC;
	int		m_iDownC;
	int		m_iThick;
	int		m_iWide;
	int		m_iLjStyle;
	int		m_iS;
	BOOL	m_bWeldDiZuo;
	CString	m_sDiBanPartNo;
	int		m_iUpLsLayStyle;
	int		m_iDownLsLayStyle;
	int		m_iUpXueBanThick;
	int		m_iDownXueBanThick;
	int		m_iDiBanMaterial;
	int		m_iXueBanMaterial;
	CString	m_sUpOddment;
	CString	m_sDownOddment;
	BOOL	m_bBreakDownXueBan0C;
	BOOL	m_bBreakDownXueBan90C;
	BOOL	m_bBreakUpXueBan0C;
	BOOL	m_bBreakUpXueBan90C;
	int		m_iDiBanShape;
	CString	m_sUpXueBanPartNo0C;
	CString	m_sUpXueBanPartNo135C;
	CString	m_sUpXueBanPartNo180C;
	CString	m_sUpXueBanPartNo225C;
	CString	m_sUpXueBanPartNo270C;
	CString	m_sUpXueBanPartNo315C;
	CString	m_sUpXueBanPartNo45C;
	CString	m_sUpXueBanPartNo90C;
	CString	m_sDownXueBanPartNo0C;
	CString	m_sDownXueBanPartNo135C;
	CString	m_sDownXueBanPartNo180C;
	CString	m_sDownXueBanPartNo225C;
	CString	m_sDownXueBanPartNo270C;
	CString	m_sDownXueBanPartNo315C;
	CString	m_sDownXueBanPartNo45C;
	CString	m_sDownXueBanPartNo90C;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFootParaSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFootParaSetDlg)
	afx_msg void OnBnHelp();
	virtual BOOL OnInitDialog();
	afx_msg void OnRdoLjStyle();
	afx_msg void OnChangeEWide();
	afx_msg void OnChkWeldDizuo();
	afx_msg void OnBnSetUcsOrigin();
	afx_msg void OnChangeEUpC();
	afx_msg void OnChangeEDownC();
	afx_msg void OnSelchangeCmbDibanShape();
	afx_msg void OnChkBreakUp0c();
	afx_msg void OnChkBreakUp90c();
	afx_msg void OnChkBreakDown0c();
	afx_msg void OnChkBreakDown90c();
	afx_msg void OnChangeEUpXuebanPartno270C();
	afx_msg void OnChangeEUpXuebanPartno180C();
	afx_msg void OnChangeEDownXuebanPartno180C();
	afx_msg void OnChangeEDownXuebanPartno270C();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOOTPARASETDLG_H__C4DA9304_F2FE_4042_AB6B_CA562E7E1C3C__INCLUDED_)
