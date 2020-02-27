#if !defined(AFX_LINKTSAPOLEDlg_H__3B700A5B_63A8_4147_84F6_20E60BB8237C__INCLUDED_)
#define AFX_LINKTSAPOLEDLG_H__3B700A5B_63A8_4147_84F6_20E60BB8237C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if defined(__TSA_)||defined(__LDS_)
// LinkTsaPoleDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CLinkLdsTubeDlg dialog

class CLinkTsaPoleDlg : public CDialog
{
// Construction
public:
	CLinkTsaPoleDlg(CWnd* pParent = NULL);   // standard constructor
	MIRMSG mirmsg;
	int xy1_yz2_xz3;	//1-XY对称重复 2-YZ对称重复 3-XZ对称重复 其余表示无重复对称
	CLDSNode *m_pStartNode;
	CLDSNode *m_pEndNode;
// Dialog Data
	//{{AFX_DATA(CLinkTsaPoleDlg)
	enum { IDD = IDD_LINK_TSA_POLE_DLG };
	int		m_iPart1stChar;
	int		m_iPart2ndChar;
	int		m_iPoleMat;
	BOOL	m_bMirSideSpace;
	int		m_uStatMatNo;
	BOOL	m_bSelSize;
	int		m_iStartForceType;
	int		m_iStartJointType;
	int		m_iEndForceType;
	int		m_iEndJointType;
	int		m_iCalLenType;
	BOOL	m_bMirAxisX;
	BOOL	m_bMirAxisXYZ;
	BOOL	m_bMirAxisY;
	BOOL	m_bMirAxisZ;
	//}}AFX_DATA
	BOOL m_bEnableMirStyle;
	char sPartNoArr[4][4][16];	//[...][][]对称位置;[][...][]子角钢序列号
	int	 m_iPoleType;	//杆件类型 0--钢管 1--扁钢 2--槽钢

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkTsaPoleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkTsaPoleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkMirAxisXyz();
	afx_msg void OnMirAxisX();
	afx_msg void OnMirAxisY();
	afx_msg void OnMirAxisZ();
	virtual void OnOK();
	afx_msg void OnBnNewStatMatNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKTSAPOLEDLG_H__3B700A5B_63A8_4147_84F6_20E60BB8237C__INCLUDED_)
