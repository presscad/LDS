#pragma once

#include "PropertyList.h"
#include "PropListItem.h"
#include "CallBackDialog.h"

//////////////////////////////////////////////////////////////////////////
// CDesFittingDlg �Ի���
class CDesFittingDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesFittingDlg)
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
	CBitmap m_xCurPlateBmp;
public:
	BYTE m_ciType;
	SEGI m_iSeg;
	UCS_STRU acs;
	BOOL m_bNewFitting;
	CLDSFitting xPreviewFitting;
	CLDSFitting *m_pFitting;
	CLDSBolt* m_pHole1,*m_pHole2;
	CLDSPart* m_pResidePart1,*m_pResidePart2;
public:
	CDesFittingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDesFittingDlg();
	//
	void InitFittingPara();
	void FinishSelectObjOper();		//���ѡ�����ĺ�������
	void UpdateSketchMap();
	CLDSFitting *CreateFitting(CLDSFitting* pToExist=NULL);
	//���Թ���
	DECLARE_PROP_FUNC(CDesFittingDlg);
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ
	void DisplayPropList();
	void DisplaySubPropList(CPropTreeItem *pItem=NULL);
// �Ի�������
	enum { IDD = IDD_DES_FITTING_DLG };

public:
	CPropertyList	m_propList;
	CPropertyList	m_xSubPropList;
	//
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnConfirm();
	afx_msg void OnBnModelViewOper();
};
