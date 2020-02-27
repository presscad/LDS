// DesignJdb.h: interface for the CDesignJdb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DESIGNJDB_H__22B6EB81_E612_11D4_895A_00E098212149__INCLUDED_)
#define AFX_DESIGNJDB_H__22B6EB81_E612_11D4_895A_00E098212149__INCLUDED_
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tower.h"

enum RAY_JOINT_STYLE{
	JOINT_HUOQU_LINE=0,
	JOINT_PRESET_ZHUN=1,
};
struct JG_STATE_STRU
{
	f3dPoint start_ber,end_ber;
	THANDLE handle;
	double start_oddment,end_oddment;
	// --------- 1.2. ��֫ -----------
	double cut_wing_para[2][3];
	//��ʶ���нǸ�֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
	int cut_wing[2];
    // --------- 1.3. �н� -----------
	double cut_angle[4][2];
	CJgBerPosPara start_pos,end_pos;
};	//�Ǹ�״̬������ȡ�Ǹ��м�����Ҫ��Ա������ɵĽṹ��
struct LJB_DESIGN_PARA_STRU
{
	long plank_thick;		//���,���Ϊ0ʱ��ʾ���Ӳ���Ҫ��
	char cMaterial;			//'S'��ʾQ235;'H'��ʾQ345
	int iRayIntersStyle;	//0��ʾ����Ԥ������;1��ʾ���ڻ�������;2��ʾ����ƫ��׼��
	BOOL bHuoQuPosByCtrlPt;	//������λ�ø����û�����Ԥ������Ŀ��Ƶ㶨λ
	BOOL bHuoQuNearBer[2];
	char sPartNo[MAX_CHAR_GUIGE_16+1];	//�������
	int nHuoQuDist[2];					//�����߼�϶����Ǹ�����
	int nOffsetZhun[2];					//ƫ��׼��ָ�ɻ�����ƫ�ƾ���
	int nVertexDist;					//�Ǹֶ�������϶
	BOOL iProfileCalType01;				//�����μ�������(0��������1��������)
	double fNormOffset;					//���Ӱ��ط���ƫ����
	BOOL bWeldToDatumJg;				//�ڵ�庸�ӵ���׼�Ǹ�(�����ڵ����) wht 10-06-06
	BOOL bAtAngleOuter;					//�ڵ�彻�ڻ�׼�Ǹ���Ƥ  wxc-17.3.3
	double fInsertPlateEdgeSpace;				//���߾�
};
//�ԽǸ��ϵĵ�����˨�����ṹ
typedef struct tagLS_AT_JG_STRU
{
	tagLS_AT_JG_STRU(){memset(this,0,sizeof(tagLS_AT_JG_STRU));}
	char guige[MAX_CHAR_GUIGE_16+1];
	int pos_para_style;	//0.��ʾ����˨����Ծ�������;1.��ʾ�ɽ�����������;2��ʾ�����˨
	double len_dist;	//�Ǹֳ��ȷ�������Ӷ˵ľ�������ڻ�׼�Ǹ������ľ�
	double ber_dist;	//�Ǹ�֫����������ߵľ�������߽Ǹ��ľ�
	double L0;			//ͨ���ĺ��
	double depth_offset;//����Ƥ�ؽǸֹ���֫���ߵ��ƶ���
	BOOL bBaseLs;
	DWORD dwRayNo;		//��˨���ߺ�
	THANDLE hCrossJg;
}LS_AT_JG_STRU;
//�ԽǸ�����ʱ�Ľ�ͷ����˨��������ṹ
struct JGLJ_ODDMENT_LS_STRU
{
	double oddment;
	BOOL m_bAutoCorrect;		//�Զ���������ͷ
	ATOM_LIST<LS_AT_JG_STRU>ls_list;
};
typedef struct tagHUOQU_LINE_STRU	//������λ��
{
	CSuperSmartPtr<CLDSLineAngle> pBaseJg;
	BOOL bOffsetWingX;
	long offset_dist;				//������ƫ�ƾ��� 
}HUOQU_LINE_STRU;
class CDesignJdb
{
	CLDSLineAngle _delegateAngle;
	BOOL m_bTerminateByUser;	//���û���ֹ
	int m_nViewFlag;			//����������
	BOOL m_bRayEndFaceOffset;	//���߲Ķ�ͷƽ�ƴ�����ʱ��׼����˨Ĭ��ҲӦΪƽ��
	const static BYTE TYPE_COMMON_PLATE		= 0;	//��ͨ��
	const static BYTE TYPE_TOP_SPLAY_PLATE	= 1;	//��繹�������տڶ������ְ�
	const static BYTE TYPE_TOP_FOLD_PLATE	= 2;	//��繹�������տڶ����۵���(�������߸˼�������һ��ʼ�ն�̧��߶�һ�²�����MIN_NORM_OFFSET)
	const static BYTE TYPE_BTM_FOLD_PLATE1	= 3;	//��繹�������տڵײ��۵���(�������߸˼�,����������)
	const static BYTE TYPE_BTM_FOLD_PLATE2	= 4;	//��繹�������տڵײ��۵���(�������߸˼�,����������)
	BYTE m_cPlateType;

	LJB_DESIGN_PARA_STRU xDesLjbPara;	//���Ӱ���Ʋ���
	JGLJ_ODDMENT_LS_STRU xDesJgResult;	//�Ǹ���ƽ��(������˨������ͷ)
	HUOQU_LINE_STRU design_huoqu[2];	//�����ߵ��������
	f3dPoint HuoQuJoint[2];				
	
	SmartRodPtr GetBaseRodPart(){
		return (m_pBasePart.IsHasPtr()&&m_pBasePart->IsLinePart())?m_pBasePart.pRod:NULL;
	}
	SmartPartPtr m_pBasePart;
	__declspec( property(get=GetBaseRodPart)) SmartRodPtr m_pBaseRod;

	SmartRodPtr m_pPrevPart;
	CSmartPtr<CLDSNode> m_pBaseNode;
	CDesignLsPara m_xBaseLs;			//�����ж����ӵ����������(��¼��׼���ϵ���˨)
	BOOL m_bAdjustWeldLen;	//�Ƿ����ͬһ�ڵ��ϵ��������Ӱ庸�쳤��һ�£���׼��λ�˼�Ϊ�ֹ�ʱ�˱�����������
	
	BOOL DesignLineFlatOrSlot(int iFaceNo,CLDSLinePart* pFlatOrSlot,BOLTSET *pLegacyBoltSet=NULL,BOOL bBasePart=FALSE);
	BOOL DesignLineTube(int iFaceNo,CLDSLineTube* pTube,CLDSLinePart* pNextPart=NULL);
	void DesLjbPara();
	void DesJgLjParaSet(BOOL bBaseJg=FALSE);
	bool CreateArcBasePlateAcs(CLDSLinePart* pRayRod);
	void CreateRefAxis();
	void Create3FaceRefAxis();
	void DesignHuoQu();
	BOOL DesignCommonPlank_Internal(BOOL bDisplayDlg);
// Operations
public:
	BOOL DesignCrossPlank(CLDSPlate* pPlank);
	bool AssureNormDirect(double* vNormCoords);	//����ֵtrue:ȷ��������������н����˷���;false:���򱣳�δ��
	BOOL DesignFoldPlank(CLDSPlate* pPlank);
	LINEPARTSET sum_partset,partset;
	TUBESET	face2_tubeset;//����ڶ����иֹ����
	JGSET face2_jgset,face3_jgset,end_lj_jgset;
	BOOL DesignOneBoltJdb();
	CLDSPlate * m_pPlank;
	//��������ӽǸ�������׸���׼�Ǹ���˨λ��
	void FirstMirLsPos(int ls_d, int initBoltSpace, double *head_offset_p, double *tail_offset_p);
	BOOL IsBerHuoQuJg(int i);
	void DesRayJgEndPos(CLDSLineAngle *pCurAngle, double fNormOffset);
	double PresetRayJgEndPos(CLDSLineAngle *pCurAngle);
	void QuitJgEndLsGroup(CLDSLineAngle *pJg, int quit_dist);
	void QuitJgStartLsGroup(CLDSLineAngle *pJg, int quit_dist);
	f3dPoint MutiFacePntToDatumFace(f3dPoint pt,int face_i,BOOL bFromMuti=TRUE);
	int GetJgInFaceI(CLDSLineAngle *pJg);
	double PresetRayFlatEndPos(CLDSLinePart* pCurFlatOrSlot);
	CDesignJdb();
	BOOL InputJdbLinePart();
	void SetFeatureJg();
	BOOL IsOneLsDesignValid();
	BOOL IsInsideJg(CLDSLineAngle *pJg,short *x_wing0_y_wing1=NULL);
	BOOL IsInsideJg(CLDSLineAngle *pJg,BYTE *x_wing0_y_wing1=NULL);
	//0��ʾʼ�����ӣ�1��ʾ�ն����ӣ�2��ʾ�м�����
	int LjPosInPart(CLDSLinePart *pLinePart);
	static int LjPosInPart(CLDSLinePart *pDatumLinePart,CLDSNode *pDatumNode,CLDSLinePart *pLinePart);
	//�����߶�λ��׼��ʽ��
	//0:�ɻ�׼�Ǹ��Ƴ�1:�ɿ��Ƶ�ȷ��
	void SetViewFlag(int nViewFlag){m_nViewFlag = nViewFlag;}
	//void CreateRayJg2Vertex(BOOL bBaseJg=FALSE);
	BOOL DesignCommonPlank(CLDSPlate* pPlank);		//�������V��ڵ��
	//װ�䶨λ�˼�Ϊ�ֹ�ʱ,�����붨λ�˼�����Ҫ����,����ͬһ�ڵ��ϵĺ������Ӱ庸���߳���һ��
	static BOOL AdjustPlateWeldLenOnSameNode(CLDSLinePart *pBaseLinePart,CLDSNode *pBaseNode);
private:
	void DesignLineAngle(int iFaceNo,CLDSLineAngle* pJg,CLDSLinePart* pNextPart=NULL,BOOL bBasePart=FALSE,
						CLDSLineAngle *pBackToBackJg=NULL,BOLTSET *pLegacyBoltSet=NULL);
	long CalJdbThick();
	void CalCurJgPreSetLsN(CLDSLineAngle *pCurAngle,BOOL bBasePart);
	void SortLinePartSet();
	void DesignHuoQuVertex(CLDSLineAngle *pBaseHuoQuJg, f3dPoint &v1, f3dPoint &v2);
};

/////////////////////////////////////////////////////////////////////////////

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNJDB_H__22B6EB81_E612_11D4_895A_00E098212149__INCLUDED_)
