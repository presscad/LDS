#if !defined(AFX_PARTGUIGELIBDLG_H__9E67C5D7_FE61_4704_B30D_E2728F681664__INCLUDED_)
#define AFX_PARTGUIGELIBDLG_H__9E67C5D7_FE61_4704_B30D_E2728F681664__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartGuiGeLibDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartGuiGeLibDlg dialog
#include "supergridctrl.h"
#include "PartLib.h"

class CPartGuiGeLibDlg : public CDialog
{
// Construction
	int m_nLsXingHaoCount;				
	LS_XING_HAO org_lsguige_table[150];	//��ʼ��ʱ����ԭ����˨���� wht 11-04-25 ��˨������100,��Ĭ�ϸ�Ϊ150 wjh 11-07-29
public:
	BOOL m_bModified;
	CLsFamily* m_pLsFamily;
	CPartGuiGeLibDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Refresh();
	void WriteDataBase();
	void CheckValue();
	int m_iPartType;//0.�ֹ� 1.�Ǹ� 2.���� 3.�۸� 4.��˨ 5.��˨���� 6.������˨ѡ�ı� 7.�Ǹֽ�ͷ�� 8.H�͸� 9.�ؽ���˨ 10.���ӽ��
	int m_iPartSubType;	
	/*	m_iPartType==8:		0.����ԵH�ָ˼� 1.����ԵH�ָ˼� 2.խ��ԵH�ָ˼� 3.����ԵH�ָ˼�
		m_iPartType==10:	0.EB�Ұ� 1.GD�Ұ� 2.U�ͻ�
	*/
	// Dialog Data
	//{{AFX_DATA(CPartGuiGeLibDlg)
	enum { IDD = IDD_TUBE_GUIGE_LIB_DLG };
	CButton	m_CheckBtn;
	CSuperGridCtrl	m_TubeLibList;
	CButton	m_ImportBtn;
	CButton	m_ExportBtn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartGuiGeLibDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartGuiGeLibDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPasteData();
	afx_msg void OnDelData();
	afx_msg void OnBnImport();
	afx_msg void OnBnExport();
	virtual void OnOK();
	afx_msg void OnCannel();
	afx_msg void OnCheck();
	afx_msg void OnRevisionTubePara();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTGUIGELIBDLG_H__9E67C5D7_FE61_4704_B30D_E2728F681664__INCLUDED_)
