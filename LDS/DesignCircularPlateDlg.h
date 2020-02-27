#if !defined(AFX_DESIGNCIRCULARPLATEDLG_H__3ECCA5E9_8DB1_4E12_93A3_B8C2CFBAA131__INCLUDED_)
#define AFX_DESIGNCIRCULARPLATEDLG_H__3ECCA5E9_8DB1_4E12_93A3_B8C2CFBAA131__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignCircularPlateDlg.h : header file
//

#include "PropertyList.h"
#include "PropListItem.h"
#include "XhCharString.h"
#include "CallBackDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CDesignCircularPlateDlg dialog
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

class CConnVectorList
{
	ATOM_LIST<CONNECT_DIRECTION> vecList;	//���ӷ���
public:
	CONNECT_DIRECTION *Append();
	CONNECT_DIRECTION *Append(CONNECT_DIRECTION &vec);
	long GetRelaConnVecLinePart(int serial,BOOL bProfilePole=FALSE);
	CONNECT_DIRECTION *GetRelaConnInfo(int serial,BOOL bProfilePole=FALSE);
	CONNECT_DIRECTION *GetFirst(BOOL bProfilePole=FALSE);
	CONNECT_DIRECTION *GetNext(BOOL bProfilePole=FALSE);
	int GetCount(BOOL bProfilePole=FALSE);
	void Empty(BOOL bProfilePole=FALSE);
	//
	CONNECT_DIRECTION *GetRelaConnVecByFeature(long feature);
	BYTE DeleteRelaConnVecByFeature(long feature);
	//�����Ƿ����serial==1��serial==2�����ӷ����ж�
	BOOL IsNeedBend();
};

class CDesignCircularPlateDlg : public CCallBackDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
private:
	GEPOINT m_xSpreadVec1,m_xSpreadVec2;
	CBitmap m_xCurPlateBmp;
	CLDSParamPlate _previewParamPlate;
	CLDSParamPlate *CreateWaterShieldedPlate(CLDSParamPlate* pToExistPlate=NULL);	//������ˮ�� 
	int m_idPlateType;
	int _IntelliRecogPlateTypeId();
	UINT _InitWaterHoleRadiusByThick(double fPlateThick);
public:
	CDesignCircularPlateDlg(CWnd* pParent = NULL);
	
	BOOL IsValidParam(BOOL bCheckPropfileParam=TRUE);
	void UpdateSketchMap(BOOL bShowWaterHole=FALSE);
	void InitCirPlatePara();//��ʼ�����������,Ŀǰ�����滹�п��ܵ��õ���δ��Ӧȥ�� wjh-2018l3.7
	void InitProfilePara();	//��ʼ���ְ����β���
	void InitWaterleakHolePara(CLDSPart* pSpecRelyVertPlate=NULL);
	int SideOfCirRibPlateOnOutwardToBasicPlate();	//�����߰��Ƿ��ڻ������,���(>0),�ڲ�(<0)���������Ϸ�(0)
	BOOL AutoCalWN();		//�Զ�����W,N����
	BOOL AutoCalDH();		//�Զ�����D,H����
	void ModifyPlateParam(WORD wKey,double fValue,BOOL bReDesign=TRUE);
	void ModifyPlateParam(WORD wKey,long nValue,BOOL bReDesign=TRUE);
	void ModifyPlateParam(WORD wKey,long hPart,UINT uiValue,BOOL bReDesign=TRUE);
	void ReDesignPlate();
	//���㻷����ϲ೤�ȣ��²೤�ȣ������������Ʒ��Ļ���� wht 11-05-31
	void CalMaxMinOffsetDist(double &up_len,double &down_len);
	//�ְ����ɺ���
	void UpdateCircularPlateConnVecList(CLDSParamPlate *pCirPlate);
	void PreviewCircularPlate();
	CLDSParamPlate *CreateCircularPlate(CLDSParamPlate* pToExistPlate=NULL);
	//��ӹ����˼�
	void AddRelaPole(CLDSLinePart *pLinePart);
	//������˨ wht 11-05-16
	void LayoutBolt(CLDSParamPlate *pParamPlate);
	//
	static const int TYPE_DATUM_VECTOR	=1;	//��λ��׼����
	static const int TYPE_TUBE_SPREAD	=2;	//�ֹ��᷽��
	static const int TYPE_Z_AXIS_VECTOR =3;	//Z�᷽��
	static const int TYPE_X_AXIS_VECTOR =4;	//X�᷽��
	static const int TYPE_Y_AXIS_VECTOR =5;	//Y�᷽��
	static const int TYPE_SPREAD_VECTOR =6;	//���巽λ
 	void WorkPlaneSketch(int iType);
	void DelWorkPlaneSketch();
	void ClearCirPlate();
	void RedrawSketchDrawing(CPropTreeItem *pItem);
	//���Թ���
	DECLARE_PROP_FUNC(CDesignCircularPlateDlg);
	static void InitEngPropHashtable();
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ
	void DisplayPlatePropList();
	void DisplaySubPropList(CPropTreeItem *pItem=NULL);
	void UpdateCirPlateBasicProp(CPropTreeItem *pBasicItem=NULL);
	void UpdateCirPlateRelaProp(CPropTreeItem *pRelaInfoItem=NULL);
	void UpdateCirPlateLocationProp(CPropTreeItem *pLocationItem=NULL);
	void UpdatePlateProfileParamProp(CPropTreeItem *pItem);
	void UpdateVectorPropList(CPropTreeItem *pParentItem,char* sPrefix,CLDSVector xVector);
public:
	long m_idSubEventProp;			//��¼�����жϵ�������ID,�ָ�����ʱʹ��
	void FinishSelectObjOper();		//���ѡ�����ĺ�������
public:	//������������
	static const int TYPE_COMMON_PLATE	/*= 1*/;	//0.��ͨ�����
	static const int TYPE_SPECIAL_FL	/*= 2*/;	//0.���ͷ���
	static const int TYPE_COVERPLATE2	/*= 3*/;	//1.������
	static const int TYPE_COVERPLATE3	/*= 4*/;	//1.������(����򻯷��)
	static const int TYPE_COVERPLATE4	/*= 5*/;	//1.������(���ɱ�Ϊֱ���͵ķ��)
	static const int TYPE_3WL_1			/*= 6*/;	//2.3WL(����֧��ֱ����)IDB_BMP_CIRCULAR_RIBPLATE5��Խ����
	static const int TYPE_3WL_2			/*= 7*/;	//2.3WL(����֧��ֱ����)IDB_BMP_CIRCULAR_RIBPLATE1
	static const int TYPE_3WA_1			/*= 8*/;	//3.3WA(����֧��Բ����)IDB_BMP_CIRCULAR_RIBPLATE5��Խ����
	static const int TYPE_3WA_2			/*= 9*/;	//3.3WA(����֧��Բ����)IDB_BMP_CIRCULAR_RIBPLATE2
	static const int TYPE_2WL_1			/*= 10*/;	//4.�����߰�2WL(����֧��ֱ����-ָ���߰��D )IDB_BMP_CIRCULAR_RIBPLATE3
	static const int TYPE_2WL_2			/*= 11*/;	//4.�����߰�2WL(����֧��ֱ����-ָ�����νǦ�)IDB_BMP_CIRCULAR_RIBPLATE7
	static const int TYPE_2WA			/*= 12*/;	//5.�����߰�2WA(��֧֫��Բ����)IDB_BMP_CIRCULAR_RIBPLATE4
	static const int TYPE_SHIELD_WATER	/*= 13*/;	//6.��ˮ��IDB_BMP_SHIELDEDWATER_PLATE
	static const int TYPE_CIR_BASE		/*= 14*/;	//6.Բ����
	static const int TYPE_AISLE_PLATE	/*= 15*/;	//7.�ߵ���

	int get_idPlateType();
	int set_idPlateType(int idType);
	__declspec(property(get=get_idPlateType,put=set_idPlateType)) int idPlateType;
public:
	CLDSParamPlate *m_pCirPlate;	//��ǰ���ڱ༭״̬���߰�,Ŀǰ�����ڲ鿴�޸����� wht 11-05-12
	BOOL m_bNewCirPlate;			//TRUE:���������µĻ���� FALSE:�鿴�޸�����
	BOOL m_bDisplayDlg;		//��Ʒ��ʱ���Ƿ񵯳����׶Ի���
	//0.��ͨ�����1.������;2.�����߰�3WL(����֧��ֱ����)3.�����߰�3WA(����֧��Բ����)
	//4.�����߰�2WL(����֧��ֱ����)5.�����߰�2WA(��֧֫��Բ����) 6.��ˮ�� 7.�����ߵ���
	int m_iPlateType;
	BOOL m_bReadOnlyPlateType;		//���øְ����Ͳ��ɱ༭�����ָ�����������η���ʱʹ�� wht 11-06-07
	//��������
	CLDSNode *m_pDatumNode;
	CLDSSphere *m_pDatumSphere;
	CLDSLineTube *m_pDatumTube;				//��׼�ֹ�
	CLDSPart *m_pRelaPart1, *m_pRelaPart2;	//��������
	CLDSPart *m_pCoverPart0,*m_pCoverPart1,*m_pCoverPart2,*m_pCoverPart3;
	CLDSVector design_axis_z;
	CLsRefList LsRefList;			//������˨
	CConnVectorList ConnVecList;	//���ӷ���
	CONNECT_DIRECTION *m_pCurSelConnVec;	//���ڼ�¼��ǰѡ�����ӷ���
	CXhChar16 m_sPartNo,m_sMirPartLabel;
	CString  m_sSeg;
	char m_cMaterial;
	long thick;
	UCS_STRU plate_ucs;	//���������ϵ wht 11-06-03
	f3dPoint connVec1,connVec2; 
	CString m_sWndTitle;//���ô��ڱ��� wht 11-06-07
	//
	struct CIRCULAR_PARAM{
		double A;	//��,����λ��(��λ����),��λ��
		double B;	//��,�������ν�(���β���),��λ��
		long   C;	//��λ��׼�ֹ�(����)���(��λ����)
		double D;	//��׼�ֹ���ڵ����ɱ߾��룬������ɱ߾�
		double d;	//��׼�ֹ���ڵ����ͷ����ھ��ľ��룬����ڲ�߾�
		long   E;	//��һ�������׼�ְ���
		long   F;	//�ڶ��������׼�ְ������򺸽ӹ����ֹܾ��(�����ֹ���ָ�뻷����к���ĸֹܣ���������Ĳ����˨���Ӹֹܣ���˲����ܳ�������)
		long   G;	//���ɱ�����(>0ֱ�ߣ�<=0Բ��)
		double H,h;	//W��N��̨�ڸ߶�
		long   I;	//��һ�����ӷ���ֹܾ��(ָ���д˸ֹܷ����������ͼ���������˨���ӻ�����������)
		long   J;	//�ڶ������ӷ���ֹܾ��(ָ���д˸ֹܷ����������ͼ���������˨���ӻ�����������)
		long   P;	//��λ��׼�ڵ���
		double R;	//�������Բ���뾶
		double r;	//r:�������Բ�뾶(���η�����360�Ļ����ʹ�� r<0���ݸֹ�������Բ r=0����Բ r>0��ָ���뾶������Բ) wht 11-05-27
		double S;	//�Զ�λ��׼�ڵ���λԭ�������ƫ�ƾ���(S<0�Ϸ��;S>0�·��)
		double T;	//�Զ�λ��׼�ڵ���λԭ�������ƫ�ƾ����ڻ���巨���ϵ�ͶӰ����
		double V;	//�ӹ���϶(��׼�ֹ���ڵ�������ڻ���Ԥ����϶)
		double Y;	//��Բ�ͻ�����λ��(>0��׼�����;=0�Զ�;<0��׼���ڲ�)
		double gama;//y:��,2WA�ͻ����߰����ɲ�ĵ��ǽǶ�,��λ��
		double W;	//��һ������ְ�ĺ��쳤
		double N;	//�ڶ�������ְ��ֹܵĺ��쳤
		double j;	//��ʼ�ߣ�0.��׼��|1.������ 
		BOOL bWeldSemicircleMode; //�������Ƿ�����Ӱ�Բ
		double R1,R2; //��׼��1��ˮ�װ뾶����׼����ˮ�װ뾶
		int nOffsetType;	//0.����ƫ�� 1.����ƫ��
		//����D��HΪһ���������,D��H�����������Ϊ��Խ���� 
		//3WL:0.�������� 1.����������D 2.����������H 
		//3WA:0.������DH 1.����DH
		int nDHRelativeType;	
		//BOOL bNeedBend;		//��Ҫ������߰�,m_iPlateType==0ʱ��ʾ�Ƿ�Ϊ���η���
		BOOL bCreateMirPlate;	//���ɶԳƲ�ְ� wht 11-06-07
		/*��Ӧ�����б��е�Y���� wht 12-03-20
		Y==0��ʾ������ͨ���η��;
		Y==1��ʾ�����νǦ���Y��������ļ򻯷��;
		Y==-1��ʾ�����νǦ���Y�Ḻ�����ļ򻯷��;
		Y==2����򻯷��;
		Y==3��ʾֱ�Ƿ��;
		*/
		long nCirCoverPlateType;
		//��ˮ�ײ���
		BYTE biHoleType;	//0.Բ��|1.��Բ��|2.�޿�
		double BH;			//��ˮ��ֱ����<0.�޿�;0.��Բ��;>0.Բ�ף�
		double fEdgeSpace;		//��˨�߾�
	}para;
	struct PARAM_WATER
	{
		double D;	//�ڻ�ֱ��
		double W;	//�⻷ֱ��
		double S;	//����ƫ�ƾ���
		double K;	//��ˮ�ۿ��
		double H;	//��ˮ�����
		double L;	//��ˮ�۳���
		double A;	//����ˮ��֮��ļн�
		double O;	//����ˮ�۽�ƽ�������׼��֮���ƫ�ƽ�
		double E;	//��λ�ǻ�׼��:0.��׼��1.������
	}water;
	struct LAYOUT_BOLT
	{
		SEGI m_iBoltSegI;	//�κ�
		int m_iDirection;	//����
		int m_nBoltD;		//ֱ��
		int m_nBoltNum;		//���� 
		//��������˨����
		int m_iStartEdge;		//��������ʼ��,0.��׼��,1.������
		double m_fOffsetAngle;	//����˨�������ʼ��֮���ƫ�ƽǶ�
		double m_fBoltLayoutR;	//��˨���ڰ뾶
		LAYOUT_BOLT(){memset(this,0,sizeof(LAYOUT_BOLT));}
	}m_xLayoutBolt;

// Dialog Data
	//{{AFX_DATA(CDesignCircularPlateDlg)
	enum { IDD = IDD_DESIGN_CIRCULAR_PLATE_DLG };
	CPropertyList	m_xSubPropList;
	CPropertyList	m_propList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignCircularPlateDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignCircularPlateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnConfirm();
	afx_msg void OnPartsDb();
	afx_msg void OnClose();
	afx_msg void OnBnModelViewOper();
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNCIRCULARPLATEDLG_H__3ECCA5E9_8DB1_4E12_93A3_B8C2CFBAA131__INCLUDED_)
