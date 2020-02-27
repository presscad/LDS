#ifndef __ENV_DEF_H_
#define __ENV_DEF_H_

#define MAX_CHAR_GUIGE_16 16
#ifndef THANDLE_DEF
typedef long THANDLE;
#endif
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "Buffer.h"
#include "Tower.h"
#include "Console.h"
#ifndef __DRAWING_CONTEXT_
#include "I_DrawSolid.h"
#endif
#include ".\XhCharString.h"
#include "ArrayList.h"
//============================================================================
const int TREE_ITEM_GRADE=100000;
const int TREE_GROUP_GRADE=1000000;
enum APPLICATION_TYPE{TA_ASSEMBLE_ENV,TA_PART_EDITOR,};
struct TREEITEM_INFO{
	TREEITEM_INFO(){;}
	TREEITEM_INFO(long type,DWORD dw){itemType=type;dwRefData=dw;}
	long itemType;
	DWORD dwRefData;
};
enum PART_CODE_ORDER_RULE
{DOWN_UP,LEFT_RIGHT,MAIN_OTHER,FRONT_SIDE,};
struct RULE_STRU
{
	bool bActive;
	PART_CODE_ORDER_RULE name;
	int iOrder;
};
//�������������������������������˳�򼰼���״̬
typedef struct tagCMD_GROUP_PARA{
	int nMaxWarnings;
	int nCmdCount;
	BOOL bOutputToNotepad;	//��������±�
	BYTE orders[192];
	BOOL actives[192];
	tagCMD_GROUP_PARA(){memset(this,0,sizeof(tagCMD_GROUP_PARA));}
}CMD_GROUP_PARA;
enum SHANK_CTRL_PT_DAO_WEI_STYLE{HORIZON_OFFSET=0,PROJECT_OFFSET};
enum VIEW_DIRECT{  
	PERSPECTIVE = 0, FRONT_VIEW = 1,
	BACK_VIEW = 2,LEFT_VIEW = 3,RIGHT_VIEW = 4, RANDOM_VIEW = 5};
//#-------------------������ݽṹ--------------
typedef struct tagLS_POS_IN_JG_STRU{
	double dist_to_end;
	BOOL bInWingX;
	double dist_to_ber;
}LS_POS_IN_JG_STRU;
typedef struct tagJGGUIGE
{
	char cMaterial;		//����
	double wing_wide,wing_thick;	//֫��֫��
}JGGUIGE;
typedef struct tagTUBEGUIGE
{
	char cMaterial;		//����
	double d,t;	//ֱ�������
}TUBEGUIGE;

//#-------------------������ݽṹ-------------
//��ͬ���ʵ����Ӱ����ɫ���÷���
class CPlateColorRef
{
public:
	CHashStrList<COLORREF>colorHashList;
public:
	CPlateColorRef();
	void SetPlateColor(char* keyStr,COLORREF &textProp);
	COLORREF GetPlateColor(char* keyStr);
	COLORREF GetPlateColor(BYTE cMaterial);
};
//ϵͳ�����ṹ
class CSysParaLibrary : public ISysPara
{
public:
	CSysParaLibrary();
	void InitSysPara();
	//��ʾ����ѡ��
	CString char0_arr[6];	//��ʾ����1
	CString char1_arr[6];	//��ʾ����2
	CString char2_arr[6];	//��ʾ����3
	BOOL	bNodeArr[6];	//����ͬʱ��ʾ�ڵ�
	BOOL	bMoveDispFrame;	//ʵ���ƶ�ʱ��ʾ�߿�ͼ
	long iLanguage;				//0.���ģ�1.Ӣ��
	long iDetailDocType;		//0.�ı��ĵ���1.EXCEL�ĵ�
	BOOL bMergeRptFileMirObj;	//�ϲ���ѧ�����������еĶԳƽڵ�͸˼�
	long reactionExportType;	//0.���ȫ��;1.�ϲ��ߵ���;2.�ϲ�����3.�����(����)��ֵ����
	int nM;						//M��ֵ
	int nSigemaM;
	int nPrecentSigemaM;
	long iEfficacyCeofType;		//��Чϵ������0.���ԣ��ϵ��|1.����ϵ��
	long iShowDeformedMapPlan;	//����ͼԤ������0.λ�Ʊ���|1.Ч��ϵ��|2.����ϵ��|3.������
	BOOL bCalStdFoundationForce;//���������������׼ֵ
	BOOL bUpdateRodDesignForce;	//�˼�������������ֵ
	char ciLinkAngleDlgFocusCtrlMode;	//0.��̬�趨;1.��������;2.�����������
	char get_ciLiveFocusCtrlInLinkAngleDlg();
	__declspec(property(get=get_ciLiveFocusCtrlInLinkAngleDlg)) char ciLiveFocusCtrlInLinkAngleDlg;
	long iLegShiftType;			//0:�������κν����ֻ�;1:���еȸ����ֻ�;2:���иߵ����ֻ�
	BOOL bRecNodeOffset;		//��ѧ����ʱ��¼�ڵ���ĳ���ߵ�ĳ�����µ�λ��
	BOOL bCreateAnalFile;		//���ɷ��������ļ�
	BOOL bCreateAnsysCmdFile;	//���ɷ�������ANSYS�������ļ�
	BOOL bCreateDetailRptFile;	//������ϸ������
	BOOL bDisplayAllHole;		//��ʱ��ʾ���п�
	BYTE m_ciLowestWarningLevel;
	struct DOCKPAGEPOS
	{
		BOOL bDisplay;
		UINT uDockBarID;
	};
	struct DOCK_ENV{
		DOCKPAGEPOS pageModel,pageProp,pageCmd,pageOutput,pageRevision,pageLogger,pagePartLabel,pageWeldPart;
		short m_nLeftPageWidth,m_nRightPageWidth,m_nBottomPageHeight;
	}dock;
	union POINTSTYLE{
		WORD wPointMode;
		struct{
			BYTE cbPointSize;
			BYTE ciPointStyle;
		};
	};
	struct DISPLAY_TYPE{
		BOOL	bPartSketch;//��ʾ�����������Լ����� wht 10-09-01
		BOOL	bNodePnt;	//��ʾ���߽ڵ�
		BOOL	bPoleLine;	//��ʾ���߸˼���
		BOOL	bLoadArrow;	//��ʾ���ؼ�ͷ
		BOOL	bWirePoint;	//��ʾ�ҵ���߷���
		int		nLoadArrowScale;//���ؼ�ͷ��ʾ��С
		POINTSTYLE	xPointStyle;	//�ڵ���ʾ��С
		BOOL	bDimNodeLabel;	//��ע�ڵ���
		int		nNodeLabelTextHeight;	//�ڵ����ָ�
		BOOL	bDimRodDesignInfo;			//��ע�˼����
		int		nRodDesignInfoTextHeight;//�˼������Ϣ�������ԣ�ȣ��ָ�
		//BOOL	bDimMemberEffic;		//��ע��Ԫ���ԣ��
		//int		nMemberEfficTextHeight;	//��Ԫ���ԣ���ָ�
		BOOL	bSolidJg;	//��ʾʵ��Ǹ�
		BOOL	bSolidTube;	//��ʾʵ��ֹ�
		BOOL	bSolidJoint;//��ʾʵ���ͷ
		BOOL	bSolidLjb;	//��ʾʵ�����Ӱ�
		BOOL	bSolidLs;	//��ʾʵ����˨
		BOOL	bSolidSphere;//��ʾ����
		BOOL	bSolidSlot;	//��ʾʵ��۸�
		BOOL	bSolidFlat;	//��ʾʵ����
		BOOL	bSolidArcAngle;	//��ʾʵ�廷�νǸ�
		BOOL	bSolidArcFlat;	//��ʾʵ�廷�α��
		BOOL	bSolidArcSlot;	//��ʾʵ�廷�β۸�
		BOOL	bSolidStdPart;	//��ʾʵ���׼����
		BOOL    bSolidTransectFace;	//��ʾʵ������棬������ָ�ֹ������
		BOOL    bSolidLine;	//��ʾʵ��ͬʱ��ʾ����
		BOOL	bSolidFoundation;	//��ʾ��������
		BOOL    bDispVirtualPart;	//��ʾ�飨�������Ǹ�    
		int     nSmoothness;//��ʾʵ��ʱ��ϸ��̶�
		BYTE    cHoleWallMode;	//0.����ʾ;1.����ʾ������˨��;2.��ʾȫ���ױ�
	}display;
	struct BRUSH_PROPERTY_TYPE{
		BOOL bMaterial;
		BOOL bLayer;
		BOOL bGuige;
		BOOL bSeg;
		BOOL bCfgword;
		BOOL bDisplayHole;
		BOOL bVirtualPart;
		BOOL bColour;
	}brush;
	//
	typedef struct tagFLSELECTLSTLB
	{
		double grade,d,disTube,disFL;//����ֱ������ܱ���С���룬�෨�������С����
		tagFLSELECTLSTLB(){memset(this,0,sizeof(tagFLSELECTLSTLB));}
		void SetProPorty(double gradeTemp,double dTemp,double disTubeTemp,double disFLTemp)
		{grade = gradeTemp;d=dTemp;disTube=disTubeTemp;disFL=disFLTemp;}
	}FL_SELECTLS_TLB;
	FL_SELECTLS_TLB flSelectLs_table[50];
	int flSelectLsTlb_N;
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	//int  m_iLoadExecuteCode;	//0:������·�淶1:���ʽṹ�淶2:�����ṹ���ع淶3:�û�ָ��;���ؼ���ִ�й淶������ϵ������������ϵ�����ɱ�������ϵ��������ϵ��ȡֵ����ѭ�Ĺ淶
	//int m_iMemberSizeCheckCode;	//�����������ѡ�Ĺ淶0:������·�淶1:���ʽṹ�淶2:����ASCE10-97
	//int m_iSlendernessLimCode;	//�˼���ϸ�����ƹ淶0:������·�淶1:���ʽṹ�淶2:����ASCE10-97��3:�û�ָ��
	//int GetExecuteLoadCode(){return m_iLoadExecuteCode;}
	//enum COEF_TYPE{Bw_I,GamaQ,GamaQ_W,GamaQ_I,GamaQ_A,GamaQ_U,GamaQ_T,GamaQKesai,Kesai_W,Kesai_I,Kesai_A,Kesai_U,Kesai_T};
	//bool IsReadOnly(int coefType);
	//double gamaG();				//���ú��أ��������أ�����ϵ��
	double Beta_z_InterpByHigh(double highpos,bool bForFoundation);	//���ݸ߶Ȳ�ֵ�������ϵ��ֵ
	double IceLoadCoefInterpByThick(double ice_thick);	//���ݱ�������������������������ϵ��
	struct INTERP_BETAZ{
		double pivot_key;	//��ֵ��׼����ֵ
		double val_des;	//���ֵ(��Ƹ�����)
		double val_std;	//��׼ֵ(��ƻ�����)
	};
	DYN_ARRAY<INTERP_BETAZ> InterpBetaZArr;	//10����ֵ��:[][0]�߶�;[][1]������Ʋ�ֵ;[][2]�������ֵ
	DYN_ARRAY<INTERP_BETAZ> InterpIceLoadCoefArr;	//3����ֵ��:[][0]�������mm;[][1]����������������ϵ��;
	DYN_ARRAY<double> efficColorBarScaleArr;//ͨ����ɫ��ʾ��Ԫ���Ч�ñ�ʱ����ɫ����ʾ�̶�,����0,100%,>100%������
	struct{
		double m_fConvergeLim;//Ӧ����������������ֵ(N)
		double m_fBeamLateralStifReductCoef;	//����Ԫ����ն��ۼ�ϵ��
		long m_iNonlinearStyle;		//0:���ԣ������Է����Է�ʽ����
		long m_iResolverStyle;		//�����ѡ��0����˹��Ԫ��1����ǰ��
		long m_nLoadStepCounts;		//�ּ��μ���
		long m_nIterationCounts;	//ÿ����������������
		long m_nSubSpaceIterCounts;	//�������Է������ӿռ��������
		double m_fFreqsTolerance;	//�������Է�����Ƶ����������
	}AnalysisPara;
#endif
	//����˨��Ȧ��� wht 10-12-24
	struct WASHERTHICK_ITEMS{
		BYTE cnMaxWasherN;		//��Ȧ����������
		BYTE xarrM12Thicks[4];
		BYTE xarrM16Thicks[4];
		BYTE xarrM20Thicks[4];
		BYTE xarrM24Thicks[4];
		WASHERTHICK_ITEMS();
		CXhChar16 ToThickStr(BYTE thicks[4]);
		int FromThickStr(const char* thickstr,BYTE thicks[4]);
		CXhChar16 ToThickStr(int d);
		int FromThickStr(const char* thickstr,int d);
	}BoltPadThick;
	double fRotateVelocity;		//���ڿ�����ת�ٶ�
	BOOL m_bDispPartACS;	//�Ƿ���ѡ�й���ʱ��ʾ������װ������ϵ�������߿�(������)
	BOOL m_bDispWorkUcs;	//�Ƿ���ʾ����������ϵ(��������ϵ)
	BOOL m_bDispBlockLCS;	//�Ƿ���ʾ�����Ķ�λ����ϵ
	BOOL m_bEnablePrompt;	//�Ƿ�������ʾ��
	bool m_bDragOpenWndSel;	//��ק�������ܿ�����
	bool m_bLineDepthTest;	//�����������ܣ�false:����ģ������ǰ̨��ʾ������ʵ���ڵ���
	bool m_bDblClkStartRotateOper;	//˫��������ת����
	//��Ʋ���ѡ��
	//�����߼�϶: �����ߵ��Ǹֱ�Ե����һ��Ϊ5mm~10mm
	long HuoQuDist;
	//�Ǹּ�϶: �Ǹ���ǸֶԽ�ʱ֮��ľ���һ��Ϊ8mm
	long JgEndDist;
	long JointPlankDist;
	long VertexDist;
	long FootNailDist;
	//��ײ��϶
	long CollideDist;
	//�޸ĶԽӷ����ֹ�����ͷ
	BOOL m_bModifyCoupledFLTubeOddment;
	//������ֹܼ�϶:������װ�������ĸֹ�֮��ľ���һ��Ϊ1mm
	long TubeFlDist;
	long m_nPlankEdgeAngleThreshold;
	double fStressDiffuseAngle;	//�Ƕ��Ƶ�λ��һ��ȡ30��
	double fInsertPlateEdgeSpace;  //���߾�
	double fNailRepScope;	//��˨���׷�Χ
	BOOL m_bDelBoltsResideOnPlate;	//ͬ��ɾ���ְ���˨
	//�ֹ�ֱ�����ڸ�ֵʱ�øֹܴ��ں�����
	double fTubeWeldRoadMinD;
	DWORD m_dwSelectPartsType;
	double m_fMaxScaleScr2User;//�ֲ��Ŵ�λʱ�����������ʾ����
	BOOL m_bCanSelectEdgeLine;//�����㹹��ѡ���Ƿ�ѡ��߽���
	UINT m_uDisplaySlices;
	UINT nAutoSaveTime;	//��λ��ms
	UINT iAutoSaveType;
	UINT uFileSaveVersion;		//��ǰ�ļ���ʽ����汾��0��ʾ��ǰ����汾
	UINT iFileSaveDocType;		//��ǰ�ļ���ʽ����汾��0��ʾ��ǰ��ƷĬ�ϸ�ʽ;1.*.tma;2.*.lma;3.*.tsa;4.*.lds;5.*.tda
	BOOL m_bSaveLogVersion;		//����汾��־ wht 12-11-16
	UINT m_iModFileCodeType;	//mod�ļ��ı��뷽ʽ 0.UTF-8|1.ANSI
	//ʹ�����Ӱ��������ɸְ�ʱ�Զ������ְ����Ͳ��λ��
	UINT nAutoAdjustTubePos;	//0.������ 1.���ƫ�� 2.�ֹ�ƫ��
	double m_fKaiHeJiaoThreshold;	//���Ͻ���ֵĬ��Ϊ2��wht 11-05-06
	double m_fKaiHeJiaoMinDeita;	//���Ͻ������С��϶ wht 16-02-01
	double m_fMaxKaiHeJiaoClearance;//���Ͻ������С��϶ wht 16-02-01
	int m_nGroupJgMachiningGap;	//��ϽǸּӹ���϶,���ڳ�ʼ����ϽǸ������Ӱ�ĺ��
	int m_nMatCharPosType;       //������ŵĲ����ַ�λ������ 0.�����   1.�������ǰ    2  ������ź�
	char m_cMatCharSeparator;	//������ŵĲ����ַ��ָ���: ��0  �ո�' '
	double m_fMinCutAngleLen;          //��С�н�ֵ
	double m_fMaxIgnoreCutAngleLen;	   //�������н�ֵ
	BOOL m_b45CutAngleNeedCoefR;	//�Ǹ�45���н�ʱ���Ƿ���Ҫ������Բ���뾶ϵ��
	double m_fCutAngleCoefR;	//�Ǹ��н�ʱ����ȥ����Բ���뾶ϵ��
	BOOL m_bAnglePreferSimplePosMode;
	//����˨���ǰ��ʾ�û�
	BOOL b1BoltDesignPrompt;
	RULE_STRU arrRule[4];	//����
	BOOL bFristJgCodeNo;
	int iColorSchema;//1:crTsaMode,2:crMaterialMode,3:crPartMode
	struct CRTSA
	{
		COLORREF crBackground;
		COLORREF crDimText;
		COLORREF crPostLine;	//���Ĳĵ���
		COLORREF crPrimaryLine;	//��ͨ������
		COLORREF crAuxLine;
		COLORREF crNode;
		COLORREF crPole;
		COLORREF crDeformedPoleLine;
		COLORREF crLoadArrow;
		bool bEfficColorLinearVary;	//true:��ԪЧ����ɫԤ������Ϊ����ɫ;false:�ֶ���ɢ��ɫ
		COLORREF crZeroStressElem;	//��˵�Ԫ
		COLORREF crOverflowElem;	//Խ��Ӧ����Ԫ ��Ʋ���>�ٽ��������
		struct ELEMCLR_DIV{
			BYTE efficPercent;		//�ֽ��߰ٷֱ���
			COLORREF crElemDivision;//Ĭ������֧����Ʋ��������ֽ���
		}crElemDivArr[6];
	}crTsaMode;
	struct CRMATERIAL
	{
		COLORREF crBackground;
		COLORREF crLine;
		COLORREF crNode;
		COLORREF crPole;
		COLORREF crQ235;
		COLORREF crQ345;
		COLORREF crQ390;
		COLORREF crQ420;
		COLORREF crQ460;
		COLORREF crOther;
	}crMaterialMode;		
	struct CRPARTCOMPLEX
	{
		COLORREF crBackground;
		COLORREF crPartBackground;	//�����༭������ɫ
		COLORREF crLine;			//�����༭���ڵ���
		COLORREF crNode;			//��ƻ����ڵ�
		COLORREF crControlNode;		//���ƽڵ�
		COLORREF crDivideScaleNode; //�ȷֽڵ�
		COLORREF crOffsetNode;		//ƫ�ƽڵ�
		COLORREF crAxisValFixNode;	//ָ����������ڵ�
		COLORREF crPole;			//��ƻ����˼�������ʾ
		COLORREF crAngleEdge;
		COLORREF crAngleWingX;
		COLORREF crAngleWingY;
		COLORREF crSonAngle1Edge;	//1���ӽǸ�֫��			
		COLORREF crSonAngle1WingX;	//1���ӽǸ�X֫��ɫ	
		COLORREF crSonAngle1WingY;	//1���ӽǸ�X֫��ɫ wht 11-05-20
		COLORREF crBolt12;
		COLORREF crBolt16;
		COLORREF crBolt20;
		COLORREF crBolt24;
		COLORREF crBolt27;
		COLORREF crBolt30;
		COLORREF crBolt36;
		COLORREF crBolt39;
		COLORREF crBolt42;
		COLORREF crBolt48;
		COLORREF crOtherBolt;
		COLORREF crAntiTheftNut;
		COLORREF crAntiLoosenNut;
		COLORREF crPlate;
		COLORREF crCutFaceOrPolyJoint;
		COLORREF crStdPart;
		COLORREF crShortAngle;
	}crPartMode;
	
	//����������Ʋ���	wht 10-01-14
	BOOL m_bDisplayConnectDlg;		//��ƹ�������ʾ���öԻ���
	struct{
		int	 m_nPlateThick;			//�ְ���
		char m_cMaterial;			//�ְ����
		BOOL m_bAutoThick;			//�Զ�����ְ���
		BOOL m_bAutoMat;			//�Զ����øְ����
		BOOL m_bOnlyDesignCurView;	//����Ƶ�ǰ�����Ӱ�
		BOOL m_bDesignCommonConnect;//���������ͨ����
		BOOL m_bDesignBendConnect;	//����������������
		BOOL m_bDesignAngleJoint;	//������ƽǸֽ�ͷ
		BOOL m_bDesignFL;			//������Ʒ���
		BOOL m_bDesignTubeCoverPlate;//������Ƹֹ��ϵĻ�����
		int m_iPlateProfileStyle;	//�ְ����μ��㷽ʽ
		int m_iSingleAngleJointStyle;//���Ǹֽ�ͷ��ʽ
		int m_iTwoAngleJointStyle;	//˫���ĽǸֽ�ͷ��ʽ
		int m_iFourAngleJointStyle;	//�����ĽǸֽ�ͷ��ʽ
		int m_iBoltLayoutStyle;		//˫����˨���÷�ʽ
		int m_nOffset;              //�������߶����ӻ�׼ƫ�ƾ���
		int m_nOffsetAdd;			//�������߶����ӻ�׼ƫ�Ʋ���
		bool m_bPriorOffsetGradientRod;	//���ȶ�б��������������Ϊ�㣩�Ĺ������߶����ӽ��л�׼ƫ��
		//��˨��׼�϶A(�������)
		int m_n12LsOutterSpace;
		int m_n16LsOutterSpace;
		int m_n20LsOutterSpace;
		int m_n24LsOutterSpace;
		//��˨��׼�϶(�����)
		int m_n12LsOutterSpaceHasWasher;
		int m_n16LsOutterSpaceHasWasher;
		int m_n20LsOutterSpaceHasWasher;
		int m_n24LsOutterSpaceHasWasher;
		//��˨֫��߾�B
		int m_n12LsAngleEdgeSpace;		//
		int m_n16LsAngleEdgeSpace;
		int m_n20LsAngleEdgeSpace;
		int m_n24LsAngleEdgeSpace;
		//�����˨��׼��C
		int m_n12LsOutterStaggerSpace;		//
		int m_n16LsOutterStaggerSpace;
		int m_n20LsOutterStaggerSpace;
		int m_n24LsOutterStaggerSpace;
		//������˨��׼��S
		int m_n12LsShareStaggerSpace;		//
		int m_n16LsShareStaggerSpace;
		int m_n20LsShareStaggerSpace;
		int m_n24LsShareStaggerSpace;
	}intelligentConnect;
	BOOL m_bModifyRelationPart; //�����޸�ͼ����ʱ�Ƿ���Ĺ�������
	//���ӽṹ���ȼ��趨
	CStringArray m_arrL1RayPole;	//�Ǹ�����1�����߸˼�������
	CStringArray m_arrL2RayPole;	//�Ǹ�����2�����߸˼�������
	CStringArray m_arrL3RayPole;	//�Ǹ�����3�����߸˼�������
	CStringArray m_arrLnRayPole;	//�Ǹ����Ķ�����߸˼�������
	//������ȼ�
	CStringArray m_arrInsertPlateType;//����������ȼ�
	CStringArray m_arrFlType;		//��������
	//���Ӱ���ɫ
	CPlateColorRef plateColRef;
	//��������� �ֱ��ӦcmdRoutineChk,cmdPartNoChk,cmdCollisionChk,cmdCorrectErros������ wht 12-04-24
	CMD_GROUP_PARA cmdGroupParaArr[4];
	
	BOOL Read(CString file_path);	//�������ļ�
	BOOL Write(CString file_path);	//д�����ļ�
	void GetPublicSysParaFromReg(LPCTSTR lpszSection, LPCTSTR lpszEntry);//��ע����л�ȡ���ò��� wht 11-06-10
	void WritePublicSysParaToReg();	//���湲�ò�����ע���
	COLORREF CalEfficColor(double efficCoef);
	virtual int GetBendClearance(){return HuoQuDist;}
	virtual BOOL IsPreferAngleSimplePosMode(){return m_bAnglePreferSimplePosMode;}
	virtual BOOL IsDisplayAllHole(){return bDisplayAllHole;}
	virtual BYTE HoleWallDisplayMode(){return display.cHoleWallMode;}
	virtual BOOL IsDelBoltsResideOnPlate(){return m_bDelBoltsResideOnPlate;}
};
struct PRODUCT_FUNC{
	static bool m_bDisableInternalTest;	//ִ��ʱ��������ֹ�ڲ�������Ȩ
	static BOOL IsHasBasicPortal();	//�Ƿ���л���������ܹ���
	static BOOL IsHasRoofDesign();	//�Ƿ�������΢��ƽ̨
	static BOOL IsHasPPE();			//�Ƿ���й��ձ༭������
	static BOOL IsHasStdTemplate();	//�Ƿ���в����׼�湦��
	static BOOL IsHasCopyLeg();		//�Ƿ���н��ȿ�������
	static BOOL IsHasGrxModule();	//�Ƿ�֧��𩳿��ͼģ��
	static BOOL IsHasZrxModule();	//�Ƿ�֧��������ͼģ��
	static BOOL IsHasAngleFoot();	//�Ƿ�֧�ֽǸ�����
	static BOOL IsHasColumnRayRod();//�Ƿ�֧���������߹���
	static BOOL IsHasSphereDesign();//�Ƿ�֧����ڵ�
	static BOOL IsHasAutoArcLift();	//�Ƿ��Զ�Բ��Ԥ��
	static BOOL IsHasShadowObject();//�Ƿ�֧��Ӱ�����
	static BOOL IsHasBlockSetup();	//�Ƿ�֧�ֲ�����װ
	static BOOL IsHasTidExport();	//�Ƿ�֧�ֵ���Tid�ļ�
	static BOOL IsHasInternalTest();//�Ƿ�֧���ڲ����Թ���
	static BOOL IsHasNonlinearAnalysis();	//�Ƿ�֧�ַ��������˽ṹ����
	static BOOL IsHasDynamicAnalysis();		//�Ƿ�֧�ֶ������Է���
	static BOOL IsHasCableAnalysis();		//�Ƿ�֧�ַ���������������
};
extern CTower Ta;
extern CSysParaLibrary g_sysPara;
extern LPCLASSFACTORY solid3dClf;
#ifndef __DRAWING_CONTEXT_
extern ISolidDraw *g_pSolidDraw;
extern ISolidSet *g_pSolidSet;
extern ISolidSnap *g_pSolidSnap;
extern ISolidOper *g_pSolidOper;
GEPOINT GetCurrViewNormal();	//���ص�ǰ��ʾ�����µ���ƽ�淨��(��Ļ���ⷽ��) wjh-2017.5.25
#endif
extern CEvent LButtonDown,RButtonDown;
void NewNode(CLDSNode *pNode);
void NewLinePart(CLDSLinePart *pJg,bool updatesolid=true);
void NewPlate(CLDSGeneralPlate* pParamPlate);
SOLIDMODEL_DISPLAY_PARA GetCurrSolidModelDisplayPara();
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
void NewArcLinePart(CLDSArcPart *pArcPart);
void RefLsToPart(CLsRefList *pLsRefList,CSuperSmartPtr<CLDSPart> pDestPart,BOOL bUpdateL0=TRUE);
//���ƹ����������Լ�������,��ָ����ǰ���������� wht 10-09-02
void DrawPartLineMap(CLDSPart *pPart,int iActiveVertex=-1);
#endif

#endif