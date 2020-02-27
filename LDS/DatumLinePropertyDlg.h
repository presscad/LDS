#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "NativePropertyList.h"

// CDatumLinePropertyDlg �Ի���
#include "PropertyList.h"
#include "CallBackDialog.h"
class CDatumLinePropertyDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDatumLinePropertyDlg)
public:
	CLDSLine m_datumLine;
	CLDSPoint m_startPoint;
	BOOL m_bNewStartPoint;
	CLDSPoint m_endPoint;
	BOOL m_bNewEndPoint;
	CLDSPoint* m_pActivePt;
public:
	CDatumLinePropertyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDatumLinePropertyDlg();
	

// �Ի�������
	enum { IDD = IDD_DATUM_LINE_PROPERTY_DLG };
	void UpdateEditPoint();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_ctrlPropGroup;
	CParaPointPropertyList m_propList;
	afx_msg void OnTcnSelchangeTabGroup(NMHDR *pNMHDR, LRESULT *pResult);
public:
	void DisplayDatumLineProperty();
	//���Թ���
	static CStringKeyHashTable<long>propHashtable;	//�����ַ�����ϣ��
	int GetPropValueStr(long id,char *valueStr);	//��������ID�õ�����ֵ
	int InitPropHashtable();
	static long GetPropID(char *sProp);			//�õ�����ID
	double m_fPointX;
	double m_fPointY;
	double m_fPointZ;
	afx_msg void OnEnChangeEdtX();
	afx_msg void OnEnChangeEdtY();
	afx_msg void OnEnChangeEdtZ();
};
