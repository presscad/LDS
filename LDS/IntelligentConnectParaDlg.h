#if !defined(AFX_INTELLIGENTCONNECTPARADLG_H__E7815BEC_0267_4B31_AF25_920E1AE757C2__INCLUDED_)
#define AFX_INTELLIGENTCONNECTPARADLG_H__E7815BEC_0267_4B31_AF25_920E1AE757C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IntelligentConnectParaDlg.h : header file
//
#include "DragListCtrl.h"
#include "PropertyList.h"
/////////////////////////////////////////////////////////////////////////////
// CIntelligentConnectParaDlg dialog

class CIntelligentConnectParaDlg : public CDialog
{
// Construction
	typedef struct tagBITMAP_INFO
	{
		CBitmap bmp;
		CString sName;
	}BITMAP_INFO;
	BITMAP_INFO m_arrBitemapInfo[25];
	CImageList m_ImageList;
	long m_nCurItemData;
	
	CStringArray m_arrL1RayPole;	//�Ǹ�����1�����߸˼������� 
	CStringArray m_arrL2RayPole;	//�Ǹ�����2�����߸˼������� 
	CStringArray m_arrL3RayPole;	//�Ǹ�����3�����߸˼������� 
	CStringArray m_arrLnRayPole;	//�Ǹ����Ķ�����߸˼�������
	CStringArray m_arrInsertPlateType;	//�������
	CStringArray m_arrFlType;		//��������

	void RefreshListView(int iCurSel);
	void RefreshTabCtrl(int iCurSel);
public:
	int m_nRayPoleNum;
	RECT m_rcClient;
	CLDSLinePart *m_pDatumLinePart;
public:
	CIntelligentConnectParaDlg(CWnd* pParent = NULL);   // standard constructor
	static CStringKeyHashTable<long>propHashtable;	//�����ַ�����ϣ��
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	int GetPropValueStr(long id, char *valueStr);	//��������ID�õ�����ֵ
	void InitSysSettingPropHashtable();
	void DisplaySystemSetting();
	static long GetPropID(char* propStr);	//�õ�����ID
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
// Dialog Data
	//{{AFX_DATA(CIntelligentConnectParaDlg)
	enum { IDD = IDD_INTELLIGENT_CONNECT_PARA_DLG };
	CPropertyList	m_propList;
	CDragListCtrl	m_listLj;
	CTabCtrl	m_tabDesignType;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIntelligentConnectParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIntelligentConnectParaDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnItemchangedListLj(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeStypeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnddragList(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
//	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTELLIGENTCONNECTPARADLG_H__E7815BEC_0267_4B31_AF25_920E1AE757C2__INCLUDED_)
