#pragma once

#include "PropertyList.h"
#include "CallBackDialog.h"
#include "afxwin.h"

// CDesignOuterAttachDlg �Ի���

struct OUTER_ATTACH_ANGLE_INFO{
	CLDSNode *pDatumNode;
	CLDSLineAngle *pDatumJg,*pDatumJg1;
	BOLTSET *pBoltSet;
	CLDSPlate *pRelaPlate;
	//
	char cMaterial;					//���ʼ��ַ�
	CXhChar16 spec;					//�̽Ǹֹ���ַ���, ��ʽ: "L140X10"
	CXhChar16 sPartNo,xPadPartNo,yPadPartNo;
	SEGI iSeg;						//�κ�
	short nRotateAngle;				//��ת�Ƕȡ�
	short iWorkWingOfDepend;//JunctionStyle;0:˫֫���������;1:X֫������;2:Y֫������
	short iWorkWingOfSelf;	//nAttachWing;	�����̽Ǹ�����֫0:˫֫����ڻ�׼�Ǹ� 1.�̽Ǹֵ�X֫�����ڻ�׼�Ǹ� 2.�̽Ǹֵ�Y֫�����ڻ�׼�Ǹ�
	BOOL bLocateOutBer;				//�Ƿ����
	static const BYTE WING_XY = 0;
	static const BYTE WING_X  = 1;
	static const BYTE WING_Y  = 2;

	OUTER_ATTACH_ANGLE_INFO(){pDatumNode=NULL;pDatumJg=pDatumJg1=NULL;pBoltSet=NULL;pRelaPlate=NULL;}
};
class CDesignOuterAttachDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesignOuterAttachDlg)

public:
	CDesignOuterAttachDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDesignOuterAttachDlg();

	void SetDatumAngle(CLDSLineAngle *pDatumAngle);
	CLDSLineAngle *GetDatumAngle(){return m_pDatumAngle;}
	
	CLDSLineAngle *m_pShortSegJg;
	bool (*DesOuterAttachShortJg)(CLDSLineAngle* &pShortSegJg,OUTER_ATTACH_ANGLE_INFO* pDesignInfo);
	void UpdateOuterAttachAngleInfo(BOOL bInernalEnable=TRUE);
// �Ի�������
	enum { IDD = IDD_DESIGN_OUTER_ATTACH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

private:
	CBitmap m_CurJointBmp;
	CLDSLineAngle *m_pDatumAngle;	//��׼�Ǹ�
public:		//�������
	OUTER_ATTACH_ANGLE_INFO info;
	int m_nOutterAttachStyle;		//������ʽ��0:����˳��;1:������;2.��������;3.��������;4.��� 5.����Ǹ�;
	int m_nWorkWingOfDepend;		//�����̽Ǹ�����֫0.����ڻ�׼�Ǹ� 1.�̽Ǹ������ڻ�׼�Ǹֵ�X֫ 2.�̽Ǹ������ڻ�׼�Ǹֵ�X֫
	CString m_sSpec;				//�̽Ǹֹ���ַ���, ��ʽ: "L140X10"
	int m_iMaterial;				//����
	CString m_sPartNo;				//�������
	CString m_sSegI;				//�κ�
	afx_msg void OnChangePartNo();
	afx_msg void OnCbnSelchangeCmbOutterAttachStyle();
	afx_msg void OnCbnSelchangeCmbAttachWing();
};
