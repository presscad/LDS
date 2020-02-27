#pragma once

#include "PropertyList.h"
#include "CallBackDialog.h"
#include "afxwin.h"

// CDesignShadowDlg �Ի���

class CDesignShadowDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesignShadowDlg)

public:
	CDesignShadowDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDesignShadowDlg();
	void DisplayShadowDesignPropList();
	void DisplaySubPropList(CPropTreeItem *pItem,BOOL bUpdate=FALSE);
	void FinishSelectObjOper();						//���ѡ�����ĺ�������
	//���Թ���
	void InitPropHashtable();
	int GetPropValueStr(long id, char *valueStr);	//��������ID�õ�����ֵ
	static CStringKeyHashTable<long>propHashtable;	//�����ַ�����ϣ��
	static long GetPropID(char* propStr);			//�õ�����ID
	void SetShadowObj(CLDSObject *pShadowObj){m_pShadowObj=pShadowObj;}
	CLDSObject *GetShadowObj(){return m_pShadowObj;}
	void ReDesignShadowObj();	//�������Ӱ�乹��
	CPropTreeItem* SetDesignInfoPropListCurSel();	//���������Ϣ��������ǰѡ����
	void RefreshShadowDesignInfo();		//����Ӱ�乹�������Ϣ

// �Ի�������
	enum { IDD = IDD_DESIGN_SHADOW_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnCbnSelchangeCmbShadowCreateStyle();
	DECLARE_MESSAGE_MAP()

public:
	CPropertyList m_xDesignInfoPropList;
	CPropertyList m_xSubPropList;
	int m_iShadowCreateStyle;	//Ӱ�乹�����ɷ�ʽ0.�Գ����� 1.ָ��ƫ�������� 2ָ����λ���� 3ָ���澵������
	long m_idParentEventProp;	//��¼�����жϵ�������ID,�ָ�����ʱʹ��
private:
	CLDSObject *m_pShadowObj;	//NULL:����Ӱ�乹�� !NULL:�鿴�޸�����Ӱ�乹�������Ϣ
public:		//�������	
	BOOL m_bMirAxisX;			//X��Գ�
	BOOL m_bMirAxisY;			//Y��Գ�
	BOOL m_bMirAxisZ;			//Z��Գ�
	BOOL m_bMirRotation;		//��ת�Գ�
	BOOL m_bAdvancedMir;		//�߼��Գ�

	double m_fOffsetDist;		//ָ��ƫ��������Ӱ�乹��ʱ��ƫ����
	int m_nOffsetNum;			//ָ��ƫ��������Ӱ�乹��ʱ��ƫ������

	int m_iCurDesParaType;		//0.datumPoint(�����) 1.datumVector1(�������/��׼�����) 2.datumVector2(���������)
	CLDSPoint datumPoint;		
	CLDSVector datumVector1;
	CLDSVector datumVector2;
	
public:		//�������
	CParaCS des_cs;				//����Ӱ�乹���Ĳ���������ϵ
};
