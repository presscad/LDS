#if !defined(AFX_SPECIALPROCESSSTATCARDDLG_H__5FC6AB3F_A0C2_4854_8DE5_6ADB35362F74__INCLUDED_)
#define AFX_SPECIALPROCESSSTATCARDDLG_H__5FC6AB3F_A0C2_4854_8DE5_6ADB35362F74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpecialProcessStatCard.h : header file
//
#include "resource.h"
#include "ListViewComparer.h"

/////////////////////////////////////////////////////////////////////////////
// CSpecialProcessStatCardDlg dialog

enum SKETCH_TYPE{
	SKETCH_RSLOT	= 1,	//һ�Ͳ�
	SKETCH_USLOT	= 2,	//U�Ͳ�
	SKETCH_XSLOT	= 3,	//ʮ���Ͳ�
	SKETCH_TSLOT	= 4,	//T�Ͳ�
	SKETCH_RTSLOT	= 5,	//һ�Ͳ�/T�Ͳ�
	SKETCH_RUSLOT	= 6,	//һ�Ͳ�/U�Ͳ�
	SKETCH_RXSLOT	= 7,	//һ�Ͳ�/ʮ���Ͳ�
	SKETCH_TUSLOT	= 7,	//T�Ͳ�/U�Ͳ�
	SKETCH_TXSLOT	= 9,	//T�Ͳ�/ʮ���Ͳ�
	SKETCH_UXSLOT	= 10,	//U�Ͳ�/ʮ���Ͳ�
	SKETCH_TUBETRANS= 11,	//�ֹ����
	SKETCH_OTHER	=100,	//����
};

class CSpecialProcessStatCardDlg : public CDialog
{
// Construction
	CListViewComparer listViewComparer;
public:
	struct STDPART_ITEM{
		CLDSPart *pPart;
		int num;
		int nSketchType;	//ʾ��ͼ����
	};
	ATOM_LIST<STDPART_ITEM> partList;
	CSpecialProcessStatCardDlg(CWnd* pParent = NULL);   // standard constructor
	CStringArray m_arrHeaderLabel;
	CListCtrl *m_pListCtrl;
	STDPART_ITEM *m_pCurSelItem;
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);

// Dialog Data
	//{{AFX_DATA(CSpecialProcessStatCardDlg)
	enum { IDD = IDD_SPECIAL_PROCESS_STAT_CARD_DLG };
	CString	m_sSegStr;
	int		m_iSpecialProcessType;
	int		m_iStartType;
	int		m_iEndType;
	BOOL	m_bAllSlot;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecialProcessStatCardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpecialProcessStatCardDlg)
	afx_msg void OnBnRebuildPartset();
	virtual BOOL OnInitDialog();
	afx_msg void OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnSpecialProcessType();
	afx_msg void OnSelchangeSlotType();
	afx_msg void OnAllSlot();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECIALPROCESSSTATCARDDLG_H__5FC6AB3F_A0C2_4854_8DE5_6ADB35362F74__INCLUDED_)
