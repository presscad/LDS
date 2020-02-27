#if !defined(AFX_LINKTUBEDLG_H__3B700A5B_63A8_4147_84F6_20E60BB8237C__INCLUDED_)
#define AFX_LINKTUBEDLG_H__3B700A5B_63A8_4147_84F6_20E60BB8237C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkTubeDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CLinkTubeDlg dialog

class CLinkTubeDlg : public CDialog
{
// Construction
public:
	CLinkTubeDlg(CWnd* pParent = NULL);   // standard constructor
	MIRMSG mirmsg;
	int xy1_yz2_xz3;	//1-XY�Գ��ظ� 2-YZ�Գ��ظ� 3-XZ�Գ��ظ� �����ʾ���ظ��Գ�
	CLDSNode *m_pStartNode;
	CLDSNode *m_pEndNode;
// Dialog Data
	//{{AFX_DATA(CLinkTubeDlg)
	enum { IDD = IDD_LINK_TUBE_DLG };
	int		m_iPart1stChar;
	int		m_iPart2ndChar;
	CString	m_sTubeGuige;
	int		m_iTubeMat;
	CString	m_sMirYPartNo;
	CString	m_sMirXPartNo;
	CString	m_sOriginPartNo;
	CString	m_sMirZPartNo;
	CString m_sSegNo;
	BOOL	m_bMirSideSpace;
	BOOL	m_bMirAxisX;
	BOOL	m_bMirAxisXYZ;
	BOOL	m_bMirAxisY;
	BOOL	m_bMirAxisZ;
	//}}AFX_DATA
	char sPartNoArr[4][4][16];	//[...][][]�Գ�λ��;[][...][]�ӽǸ����к�
	int	 m_iPoleType;	//�˼����� 0--�ֹ� 1--��� 2--�۸�

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkTubeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkTubeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEOriginPartNo();
	afx_msg void OnChangeEMirXOriginPartNo();
	virtual void OnOK();
	afx_msg void OnChkMirAxisXYZ();
	afx_msg void OnMirAxisX();
	afx_msg void OnMirAxisY();
	afx_msg void OnMirAxisZ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKTUBEDLG_H__3B700A5B_63A8_4147_84F6_20E60BB8237C__INCLUDED_)
