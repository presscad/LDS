#pragma once


// CInputHuoQuLineDlg �Ի���
#include "CallBackDialog.h"
#include "NativePropertyList.h"

class CInputHuoQuLineDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CInputHuoQuLineDlg)

public:
	CInputHuoQuLineDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputHuoQuLineDlg();

	void InitHuoquLineInfo(BOOL bInitDefaultInfo=TRUE);
	void RefreshPropertyListCtrl();
	void UpdateUIStatus();
// �Ի�������
	enum { IDD = IDD_INPUT_HUOQULINE_DLG };
	CParaPointPropertyList m_propStartPtList;
	CParaPointPropertyList m_propEndPtList;
	CParaVectorPropertyList m_propVectorList;
	CLDSLinePart *m_pCrossRod;
	CTabCtrl m_ctrlPropGroup;
	CString	m_sJgHandle;		//��׼�Ǹ־��
	double m_fWingOffsetDist;	//֫��ƫ����
	int m_iWingOffsetType;		//֫��ƫ������
	int m_iOffsetWing;			//ƫ��֫��X/Y��
	int m_iCurSel;
	//
	CLDSPoint m_xStartPt;
	CLDSPoint m_xEndPt;
	CLDSVector m_xVector;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTcnSelchangeTabGroup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSelectPart();
	afx_msg void OnInitHuoQuLineInfo();
};
