#pragma once

#include "Resource.h"
#include "supergridctrl.h"
#include "PropertyList.h"
//////////////////////////////////////////////////////////////////////////
// CDesHangFittingDlg �Ի���
class CDesHangFittingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDesHangFittingDlg)
	int m_iCurSel;
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
public:
	CDesHangFittingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDesHangFittingDlg();

// �Ի�������
	enum { IDD = IDD_DES_HANG_FITTING_DLG };

public:
	CSuperGridCtrl m_xWirePtListCtrl;
	CSuperGridCtrl m_xConnPtListCtrl;
	CPropertyList m_xPropertyList;
	//
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnExtractWirePoints();
public:
	void RefreshListCtrl();
	BOOL FinishSelectObjOper(long hPickObj,long idEventProp);
	void SetSelectIndex(int iCurSel){m_iCurSel=iCurSel;}
	//
};
