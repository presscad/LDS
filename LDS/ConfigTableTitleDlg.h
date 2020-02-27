#pragma once
#include "afxcmn.h"
#include "supergridctrl.h"
#include "Variant.h"
#include "BomTblTitleCfg.h"

// CConfigTableTitleDlg �Ի���
class CConfigTableTitleDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigTableTitleDlg)
	int m_iCurTitleIndex;
	CBomTblTitleCfg m_xTblCfg;
	void InitListCtrl(CVariant2dArray *pSheetContextMap);
	void ListTitleHeaderContextMenu(CPoint point);
public:
	CConfigTableTitleDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConfigTableTitleDlg();
	CBomTblTitleCfg *GetTblTitleCfg(){return &m_xTblCfg;}
// �Ի�������
	enum { IDD = IDD_CONFIG_TABLE_TITLE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CSuperGridCtrl m_listTitle;
	CSuperGridCtrl m_listContent;
	CVariant2dArray *m_pSheetContextMap;
	//
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	int m_nStartRow;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void OnDeleteHeaderTitle();
};
