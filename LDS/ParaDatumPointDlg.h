 #pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "PropertyList.h"
#include "CallBackDialog.h"

// CParaDatumPointDlg �Ի���

class CParaDatumPointDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CParaDatumPointDlg)
public:
	CLDSPoint m_xPoint;
	double m_fPointX;
	double m_fPointY;
	double m_fPointZ;
public:
	CPropertyList m_propList;
	static CStringKeyHashTable<long>propHashtable;	//�����ַ�����ϣ��
	int GetPropValueStr(long id,char *valueStr);	//��������ID�õ�����ֵ
	static long GetPropID(char *sProp);			//�õ�����ID
	void InitPropHashtable();
	void FinishSelectObjOper();		//���ѡ�����ĺ�������
	void DisplayParaPointProperty();
public:
	CParaDatumPointDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParaDatumPointDlg();

// �Ի�������
	enum { IDD = IDD_PARA_DATUM_POINT };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdtX();
	afx_msg void OnEnChangeEdtY();
	afx_msg void OnEnChangeEdtZ();
};
