#if !defined(AFX_DESIGNRIBBEDPLATEDLG_H__581B91A2_C138_4B12_9499_64CEEBAEC22D__INCLUDED_)
#define AFX_DESIGNRIBBEDPLATEDLG_H__581B91A2_C138_4B12_9499_64CEEBAEC22D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignRibbedPlateDlg.h : header file
//

#include "resource.h"
#include "PropertyList.h"
#include "CallBackDialog.h"

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
struct ANCHOR_PARAM{
	BYTE ciType;	//0.���� 1.VecRod;2.Bolt;3.VERTEX
	long hRelaObj;	//���������˨�����ְ�
	union LOCATION{
		long hBolt;
		long hVecRod;
		BYTE ciTriggerType;	//0.�������ʰȡ;1:�Ҽ���ֹ;2:��Χ������ֹ;3:�س���ȷ���˳�;4:ESC����ֹ
		struct VERTEX{
			long idVertex;
			double x,y;
		}vertex;
	}locate;
	ANCHOR_PARAM(){ciType=0;hRelaObj=0;locate.vertex.x=locate.vertex.y=locate.vertex.idVertex=0;}
};
// CDesignRibbedPlateDlg dialog

class CDesignRibbedPlateDlg : public CCallBackDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
// Construction
	CBitmap m_xCurPlateBmp;
public:
	typedef struct tagRIB_PLATE
	{	//�ְ��������
		char sPartNo[16];
		char cMaterial;
		int nThick;
		SEGI nSegI;
		BYTE m_cPlateType;	//�߰����� 0.���߰� 1.���߰� 2.�Ǹ��߰� 3.�ְ��߰� 4.�ֹ��߰�
		BYTE m_cSubType;	//�߰������ͣ��磺1.���߰壭���棻2.���߰壭�ֹ����
		struct tagTUBE_PARA
		{	//��׼����Ϊ�ֹ�ʱʹ��
			int nPlateNum;			//�ֹ��߰����
			int iDatumEdge;			//0.��׼�� 1.������ 2.�����(�ɵ�����������)
			int iWorkNormStyle;		//0.��ֱ�ֹ����췽�� 1.�ֹ����� 2.�Զ���
			double fInitOffsetAngle;//��ʼƫ�ƽǶ� �����췽��Ϊ�������
			f3dPoint extend_vec;	//�����췽�� �ְ����췽�����ʼ�Ƕ�������ͬ����ȷ����һ���߰�����췽��
			double fOffsetAngle;	//ƫ�ƽǶ�
			double fBottomOutterR;	//�ײ��⻷�뾶(�븨���ְ�1�����ߵ����뾶)
			double fTopOutterR;		//�����⻷�뾶(�븨���ְ�2�����ߵ����뾶)
			BOOL bVerticalPlate;	//��ֱ�Ҵ��Զ���λ���߰�
			double fOffsetDistM;	//����ƫ�ƾ���M(�޸����������߰���Ҫ�ñ���)
			double fOffsetDistX;	//X����ƫ�ƾ���(�޸����������߰���Ҫ�ñ���)
			BOOL bAutoCalB;			//�Զ����쵽�װ��Ե
			int margin;				//ָ���װ��Ե�߾�
			//
			BYTE cLeftTopCornerType;	//Y+�����߽����� 0.���� 1.Բ�� 2.ֱ��
			double fLeftTopCornerWidth;	//����ʱ��ʾ�нǿ�ȣ�Բ��ʱ��ʾ�л��뾶
			double fLeftTopCornerHeight;//����ʱ��ʾ�нǿ�ȣ�Բ��ʱ��ʾ�л��뾶
			BYTE cRightTopCornerType;	//Y-�����߽����� 0.���� 1.Բ�� 2.ֱ��
			double fRightTopCornerWidth;//����ʱ��ʾ�нǿ�ȣ�Բ��ʱ��ʾ�л��뾶
			double fRightTopCornerHeight;//����ʱ��ʾ�нǿ�ȣ�Բ��ʱ��ʾ�л��뾶
			tagTUBE_PARA(){memset(this,0,sizeof(tagTUBE_PARA));}
		}tube;
		struct tagPLATE_PARA
		{	//��׼����Ϊ�ְ�ʱʹ��
			BOOL bSpecOrg;				//ָ������ԭ����ָ����λ��׼������ѡһ
			BOOL bSpecAxisX;			//�ְ��߰�ָ��X�᷽����ָ����������˼���ѡһ��
			int  iAxisXType;			//0.X+|1.X-|2.Y+|3.Y-|4.����..|5.�Զ�����
										//���߰����߰�ʱ�����뷽��ʱ���ݸ����ְ����X�᷽��
			BOOL bSpecAxisY;			//��װ���б�Ƕȣ���Y����װ����X�����ת���뷽���׼�˼���ѡһ��
			double fGradientDegAngle;	
			double fOriginX,fOriginY;	//�߰�ԭ��λ��(��׼�ְ�����ϵ�µ��������)
			double fXAxisX,fXAxisY;		//�߰�X�᷽��(��׼�ְ�����ϵ�µ��������)
			double fOffsetDistX;	//X����ƫ�ƾ���(�޸����������߰���Ҫ�ñ���)
			double fOffsetDistZ;	//����ƫ����
			BOOL bOnDatumPlateTopFace;	//��׼����Ϊ�ְ�ʱʹ��
			BOOL bAlongVecPart;		//�ְ����췽���뷽��˼�ͬ��
			//��չ���ܺ���������� wjh-2016.5.4
			ANCHOR_PARAM anchors[4];
			double fBendTipOffsetX;	//X����ƫ�ƾ���(�޸����������߰���Ҫ�ñ���)
			double fBendTipOffsetZ;	//����ƫ����
			tagPLATE_PARA(){
				memset(this,0,sizeof(tagPLATE_PARA));
				iAxisXType=0;
				fXAxisX=1;
				fXAxisY=0;
			}
		}plate;
		BOOL bOnRelaPlateTopFace;	//һ�鸨���ְ��ʾ�߰�λ�ڸְ嶥��һ�࣬�޸���������ʾ��ֹ����췽����ͬ
		BOOL bAutoExtendOutterEdge;	//�Զ�������������Ե
		double fTopWidth;		//�������
		double fBottomWidth;	//�ײ����
		double fLength;			//����(Ϊ���ְ�а�ʱ�ó����Զ�����)
		double fHeight;			//�߶�
		double fCirR;			//Բ���뾶
		long iCutAngleMode;		//����ģʽ��0.�н� 1.�л�
		double fCutAngleWidth;	//�нǿ��
		double fCutAngleWidth2;	//�нǿ��2
		double MF,MG,MH;		//�����׵Ŀ׾����F,G��׾�H
		//������������һ��ְ�ʱ�ñ�����ʾ�Ƿ�Գ����ɣ���������Ϊ����ְ�ʱ��ʾͬʱ��������߰�
		BOOL bCreateOuterPlate;
		double fOuterPlateLen;		//����߰峤��
		double fOuterPlateHeight;	//����߰�߶�
		union RIB_PARAMS{
			CLDSParamPlate::RIBPLATE_CORNER corner;	//�ֹ�Ϊ�װ�Ľ��߰����߰�
			CLDSParamPlate::RIBPLATE_ELBOW elbow;	//�ֹ�Ϊ��׼�ķ������������
			CLDSParamPlate::RIBPLATE_PLATE platerib;//�Ըְ�Ϊ�װ�(���������򿿽ӹ���)����ͨ�߰�
			CLDSParamPlate::RIBPLATE_TUBE tuberib;	//�Ըֹ�Ϊ��׼�������߰�(���������򿿽ӹ���)
			CLDSParamPlate::RIBPLATE_ANGLE anglerib;//�Ǹ��ϵĺ�����߰�
		}params;
		tagRIB_PLATE(){
			memset(this,0,sizeof(tagRIB_PLATE));
			cMaterial='S';
			nThick=6;
			tube.nPlateNum=1;
			iCutAngleMode=1;	//Ĭ���л�
			fCutAngleWidth=20;
			fCutAngleWidth2=20;
			fTopWidth=30;
			fHeight=30;
			fLength=300;
			fBottomWidth=200;
			fCirR = 30;
			MH=0;
			MF=MG=fCutAngleWidth+20;
		}
	}RIB_PLATE;
	UCS_STRU ucs;	//�ٶ����߰�����ϵ
	RIB_PLATE m_xRibPlate;
	CLDSNode *m_pDatumNode;
	CLDSBolt *m_pDatumBolt;
	CSuperSmartPtr<CLDSPart> m_pDatumPart,m_pRelaPart1,m_pRelaPart2,m_pVecPart;
	PARTSET ribPlateSet;	//��¼ͨ����ת��Գ����ɵ��߰� wht 11-06-07
	CString m_sWndTitle;	//���ô��ڱ��� wht 11-06-07
	long m_idSubEventProp;  //��¼�����жϵ�������ID,�ָ�����ʱʹ��
public:
	CDesignRibbedPlateDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL IsValidAxisX();			//�ж������X�᷽���Ƿ����
	BOOL AutoCalRibPlateWidth();	//�Զ������߰���
	void ReCalRibPlateUCS(BOOL bUpdateRibLengthParam=TRUE);		//���¼����߰�����ϵ
	void ModifyPlateParam(WORD wKey,double fValue,char *sNote=NULL);
	void ModifyPlateParam(WORD wKey,long nValue,char *sNote=NULL);
	void DeletePlateParam(WORD wKey);
	void ReDesignPlate();
	void SetRibPlate(CLDSParamPlate *pRibPlate);
	CLDSParamPlate *GetRibPlate(){return m_pRibPlate;}
	void FinishSelectObjOper();		//���ѡ�����ĺ�������
	void UpdateSketchMap();
	//���Թ���
	void InitPropHashtable();
	int GetPropValueStr(long id, char *valueStr);	//��������ID�õ�����ֵ
	int GetPropStr(long id, char *propStr);			//�õ������ַ���
	bool CorrectPropItemName(const char* propKey,CItemInfo* lpInfo);
	CPropTreeItem* InsertItem(CPropertyList* pPropList,CPropTreeItem *pParent, const char* propKey,int controlType=0,BOOL bUpdateValueText=TRUE, int iInsertIndex=-1, BOOL bUpdate=0);
	CPropTreeItem* InsertOrUpdateItem(CPropertyList* pPropList,CPropTreeItem *pParent, const char* propKey,int controlType=0,BOOL bUpdateValueText=TRUE, int iInsertIndex=-1, BOOL bUpdate=0);
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static PROPLIST_ITEM* AddPropItem(char* propKey,PROPLIST_ITEM item){
		PROPLIST_ITEM* pItem=propHashtable.SetValue(propKey,item);
		pItem->propKeyStr.Copy(propKey);
		return pItem;
	}
	static long GetPropID(char* propStr);			//�õ�����ID
private:
	GEPOINT m_xSpreadVec1,m_xSpreadVec2;
	BOOL m_bNewRibPlate;	//TRUE:�����߰� FALSE:�鿴�޸������߰�����
	CLDSParamPlate *m_pRibPlate;
	BOOL LoadParamsFrom(CLDSParamPlate* pParamPlate);
	//BOOL SaveParamsTo(CLDSParamPlate* pParamPlate);
	void DisplayPropertyList();
	void FillParamToPlate(CLDSParamPlate *pRibPlate);
public:
	static const int TYPE_PLANE_SPREAD	=1;	//��չ������
	static const int TYPE_PLANE_NORMAL	=2;	//�淨�߷���
	static const int TYPE_TUBE_SPREAD	=3;	//�ֹ��᷽��
	static const int TYPE_X_AXIS_VECTOR =4;	//X�᷽��
	static const int TYPE_Y_AXIS_VECTOR =5;	//Y�᷽��
	void WorkPlaneSketch(int iType);
	void DisplaySubPropList(CPropTreeItem *pItem,BOOL bUpdate=FALSE);
	BOOL RedrawSketchDrawing(CPropTreeItem *pItem,UCS_STRU *pObjectCS=NULL,bool removeOldEntities=true);
	bool IsElbowRibPlate();
	BOOL IsNewRibPlate(){return m_bNewRibPlate;}
	void InitRibPlatePara();
	UINT _InitWaterHoleRadiusByThick();
	void DelWorkPlaneSketch();
	CLDSParamPlate *CreateRibPlate();
// Dialog Data
	//{{AFX_DATA(CDesignRibbedPlateDlg)
	enum { IDD = IDD_DESIGN_RIBBED_PLATE_DLG };
	CPropertyList	m_propList;
	CPropertyList	m_xSubPropList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignRibbedPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignRibbedPlateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnConfirm();
	afx_msg void OnPartsDb();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnModelViewOper();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNRIBBEDPLATEDLG_H__581B91A2_C138_4B12_9499_64CEEBAEC22D__INCLUDED_)
