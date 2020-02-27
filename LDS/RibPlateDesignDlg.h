#if !defined(AFX_RIBPLATEDESIGNDLG_H__08D42BE1_3DBE_4AA2_A9B1_8848F8D9B20A__INCLUDED_)
#define AFX_RIBPLATEDESIGNDLG_H__08D42BE1_3DBE_4AA2_A9B1_8848F8D9B20A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RibPlateDesignDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CRibPlateDesignDlg dialog

class CRibPlateDesignDlg : public CDialog
{
// Construction
public:
	int m_iPlateType;	//0.����Ӿ��� 1.��ͷ�� 2.��ͨ�߰�
	CRibPlateDesignDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bDatumTube;	//�Ըֹ�Ϊ��λ��׼
	CBitmap m_CurPlateBmp;
	CLDSPart *m_pDatumPart;
	CLDSNode *m_pDatumNode;
// Dialog Data
	//{{AFX_DATA(CRibPlateDesignDlg)
	enum { IDD = IDD_DESIGN_RIB_PLATE_DLG };
	int		m_iMaterial;
	double	m_fEndPosX;
	double	m_fEndPosY;
	double	m_fEndPosZ;
	int		m_nHigh;
	CString	m_sPartNo;
	double	m_fPosAngle;
	CString m_sSegI;
	double	m_fStartPosX;
	double	m_fStartPosY;
	double	m_fStartPosZ;
	int		m_nThick;
	int		m_nPlateNum;
	double	m_fNormOffset;
	double	m_fStartHigh;	//��׼����Ϊ�ֹ�ʱ��ʾ��ʼ�Ƕ�
	double	m_fGradientAngle;
	int		m_iDatumLine;	//��׼��: 0.��׼�� 1.������
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRibPlateDesignDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRibPlateDesignDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEPartNo();
	afx_msg void OnChangePlateThick();
	virtual void OnOK();
	afx_msg void OnPasteEndPoint();
	afx_msg void OnCopyEndPoint();
	afx_msg void OnPasteStartPoint();
	afx_msg void OnCopyStartPoint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RIBPLATEDESIGNDLG_H__08D42BE1_3DBE_4AA2_A9B1_8848F8D9B20A__INCLUDED_)
