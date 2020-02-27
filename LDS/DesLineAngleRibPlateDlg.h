#pragma once

#include "PropertyList.h"
#include "PropListItem.h"
#include "XhCharString.h"
#include "CallBackDialog.h"
// CDesLineAngleRibPlateDlg �Ի���

class CDesLineAngleRibPlateDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesLineAngleRibPlateDlg)
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
	CBitmap m_xCurPlateBmp;
public:
	//�Ǹ��߰����
	struct RIBPLATE_PARAM{
		double D;	//ƫ�ƾ�
		double L;	//X֫�ϳ���
		double Ly;	//Y֫�ϳ���
		double W;	//X֫��̨�׸߶�
		double Wy;	//Y֫��̨�׸߶�
		double S;	//�нǿ��
		double H;	//�ڲ�X�򳤶�
		double Hy;	//�ڲ�Y�򳤶�
		double R;	//Բ���뾶
		long   C;	//���ɱ����� 0.ֱ��|1.Բ��
		long   E;	//�������ͣ�0.ֱ��|1.Բ��
		double d;	//�׾�
		double P;	//�׾�
		double m_fAngleWidth;	//�Ǹֿ�ȣ������ж�ʹ�þ�̬����ʱ�Ƿ����¼����߰���� wht 18-11-15
		//
		RIBPLATE_PARAM(){memset(this,0,sizeof(RIBPLATE_PARAM));}
	}para;
	//�ֹ����߰����
	struct STAYWIRE_RIBPLATE_PARA{
		BOOL bOnRelaPlateTopFace;	//
		double rotAngle;//����λ�ǵ���ת��ʼ����������׼�߻򺸵��ߵĶ��ⶨλƫת��,����塮Y��������Ӧ����
		long J;			//����λ�ǵ���ת��ʼ�����ͣ�0.��׼�ߣ�1.������
		long N;		//���߿���
		double L;	//�ֹ����ߵĳ���	
		double W;	//���߰���
		double H;	//�ڸ������ĺ��ӳ���
		double R;	//���ǰ뾶
		double d;	//�׾�
		double P;	//�׾�
		//
		STAYWIRE_RIBPLATE_PARA(){memset(this,0,sizeof(STAYWIRE_RIBPLATE_PARA));}
	}stay_wire_para;
	//�ֹܽ��߰�
	struct TUBE_CORNER_RIBPLATE{
		int thick;
		double fRotAngle;	//��λ��
		double E;	//�ײ����
		double F;	//�������
		double G;	//�ڲ೤��
		double H;	//��೤��
		BYTE e;		//����ģʽ
		double I;	//�нǿ��
		double MF;
		double MG;	
		double MH;
		//
		TUBE_CORNER_RIBPLATE(){memset(this,0,sizeof(TUBE_CORNER_RIBPLATE));}
	}ass_rib_para;	
	//��������
	struct BASIC_PARRM 
	{
		SEGI nSegI;
		char cMaterial;
		int nThick;
		long cPlateType;
		//
		BASIC_PARRM() { memset(this, 0, sizeof(BASIC_PARRM)); }
	};
private:
	static bool m_bValidStaticRibParam;				//�Ǹ��߰徲̬�����Ƿ����
	static bool m_bValidStaticStayWireParam;		//���߰徲̬�����Ƿ����
	static bool m_bValidStaticAssRibParam;			//���߰�С�߰�����Ƿ����
	static RIBPLATE_PARAM s_xRibPlateParam;			//�߰����
	static STAYWIRE_RIBPLATE_PARA s_xStayWireParam;	//���߰����
	static TUBE_CORNER_RIBPLATE s_xAssRibParam;		//���߰��߰����
	static BASIC_PARRM s_xBasicParam;				//�߰��������
public:
	CLDSLineTube* m_pDatumTube;
	CLDSGeneralPlate* m_pAssistPlate;
	CLDSLineAngle* m_pDatumAngle;
	CLDSBolt* m_pDatumBolt;
	CLDSNode* m_pDatumNode;
	CXhChar16 m_sPartNo;
	CXhChar16 m_sAssPartNo;
	SEGI m_nSegI;
	char m_cMaterial;
	int m_nThick;
	long m_cPlateType;	//�߰�����  0.�ڲ��߰�|1.����߰�|2.�����߰�
	long m_cNormCalType;	//������㷽ʽ 0.���ܼ���|1.��Ϊָ��
	long m_cDirection;		//���췽��ʽ 0.ʼ-��|1.��-ʼ
	BOOL m_bNewRibPlate;
	CLDSParamPlate _previewRibPlate;
	CLDSParamPlate *m_pRibPlate;
	UCS_STRU ucs;
	long m_idSubEventProp;  //��¼�����жϵ�������ID,�ָ�����ʱʹ��
	BOOL m_bNewAssRibPlate;	//���߰�����ĸֹ��߰�
public:
	CDesLineAngleRibPlateDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDesLineAngleRibPlateDlg();
	//
	void InitRibPlatePara();
	void AutoCalRibPlateWidth();
	CLDSParamPlate *CreateRibPlate(CLDSParamPlate* pToExistPlate=NULL);
	void ReDesignPlate();
	void FinishSelectObjOper();		//���ѡ�����ĺ�������
	void UpdateSketchMap(CPropTreeItem *pItem=NULL);
	void ModifyPlateParam(WORD wKey,double fValue);
	void ModifyPlateParam(WORD wKey,long nValue);
	void DeletePlateParam(WORD wKey);
	//���Թ���
	DECLARE_PROP_FUNC(CDesLineAngleRibPlateDlg);
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ
	void DisplayPlatePropList();
	void DisplaySubPropList(CPropTreeItem *pItem=NULL);
	// �Ի�������
	enum { IDD = IDD_DES_LINEANGLE_RIBPLATE_DLG };
	CPropertyList	m_xSubPropList;
	CPropertyList	m_propList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnConfirm();
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnModelViewOper();
};
