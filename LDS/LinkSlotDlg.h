#if !defined(AFX_LINKSLOTDLG_H__5D8D44F4_BA34_44B3_A99F_C7499D04565D__INCLUDED_)
#define AFX_LINKSLOTDLG_H__5D8D44F4_BA34_44B3_A99F_C7499D04565D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkSlotDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLinkSlotDlg dialog

class CLinkSlotDlg : public CDialog
{
// Construction
public:
	CLinkSlotDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLinkSlotDlg)
	enum { IDD = IDD_LINK_SLOT_DLG };
	int		m_iSlotLayStyle;
	int		m_iPart1stChar;
	int		m_iPart2ndChar;
	CString	m_sSlotGuige;
	int		m_iSlotMat;
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
	MIRMSG mirmsg;
	int xy1_yz2_xz3;	//1-XY�Գ��ظ� 2-YZ�Գ��ظ� 3-XZ�Գ��ظ� �����ʾ���ظ��Գ�
	CLDSNode *m_pStartNode;
	CLDSNode *m_pEndNode;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkSlotDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkSlotDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEOriginPartNo();
	afx_msg void OnChangeEMirXOriginPartNo();
	afx_msg void OnMirAxisX();
	afx_msg void OnMirAxisXyz();
	afx_msg void OnMirAxisY();
	afx_msg void OnMirAxisZ();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKSLOTDLG_H__5D8D44F4_BA34_44B3_A99F_C7499D04565D__INCLUDED_)
