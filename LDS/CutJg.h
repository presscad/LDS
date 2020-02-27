#pragma once
#ifdef __COMMON_PART_INC_

#include "InputAnValDlg.h"
struct ANGLECUT_ADVICE{
	struct ANGLECUT_PARA{
		char cCutWing;	//'X'����X֫��'Y'����Y֫��������ʾ����֫
		struct CUTWING{
			double fCutWingLength;	//��Ӧ...[][2]��ʾ֫������ȡ����
			double fCutBerLength;	//��Ӧ...[][0]��ʾ�Ǹ�������ȡ����
			double fCutWidth;		//��Ӧ...[][1]��ʾ��һ֫��ȡ���
		}wing;
		struct CUTCORNER{
			double fLength;	//�и�ȣ��Ǹ����췽�򣩣���cut_angle[0][...]��Ӧ
			double fWidth;	//�и��ȣ��Ǹ�֫���򣩣���cut_angle[1][...]��Ӧ
		}corner[2];//0.X;1.Y
	}start,end;
	BOOL bCutStart;			//�Ƿ������߽Ǹ�ͷ��
	void Init(CLDSLineAngle* pAngle);
	void MergeCutting(const ANGLECUT_ADVICE& pAdvice,CLDSLineAngle* pAngle=NULL);
	void RetriveValidCutData();
	void Clear(){memset(this,0,sizeof(ANGLECUT_ADVICE));}
};
struct ANGLE_CUT_INFO 
{
public:
	static const BYTE RELATION_MAINJG_RAYJG			 =1;	//���߽Ǹ��������Ǹ�	
	static const BYTE RELATION_RAYJG_RAYJG			 =2;	//���߽Ǹ������߽Ǹ�
	static const BYTE RELATION_ANGLE_BOLT			 =3;	//�Ǹ�����˨�����ײ���
	//
	static const BYTE CUT_TWORAYJG_BY_PLANE 		 =1;	//�ǹ������߽Ǹ֣��������ǸֵĽ�ƽ�����и����߽Ǹ�
	static const BYTE CUT_TWORAYJG_BY_45DU			 =2;	//����ԵȽǸ�(֫��֫������ײ)��ͨ��45���нǽ����и�
	static const BYTE CUT_RAYJG_BY_DATUMJG			 =3;	//����Ǹֻ���ǶԵȽǸַ���֫�������ײ�����ݻ�׼�Ǹ��и����߽Ǹ�
	static const BYTE CUT_RAYJG_BY_PLANE			 =4;	//����Ǹַ���֫��֫��ײ��ͨ�����Ǹ�֫���ȷ���и��棬�и����߽Ǹ�
	static const BYTE CUT_RAYJG_BY_BOLT				 =5;	//����Ǹַ���֫��֫��ײ��ͨ�����Ǹ�֫���ȷ���и��棬�и����߽Ǹ�
public:
	SEGI iSeg;
	THANDLE hPart1,hPart2;
	f3dPoint face_pick,face_norm;
	BYTE cFlag;					//&0x01��ͬ�˵����߽Ǹ��н� &0x02��������ӽǸ��н� &0x04���Ǹ�����˨�н�
	BYTE cRelationType;			//���Ǹֹ�ϵ�� 1��׼�Ǹ������߽Ǹ�  2���߽Ǹ������߽Ǹ�
	union RELATION{
		struct{	//
			THANDLE hMainJg;
			THANDLE hRayJg;
			BOOL bCutStart;			//�Ƿ������߽Ǹ�ͷ��
			BOOL bInMainWingX;		//�Ƿ������Ǹ�X֫
			BOOL bInRayWingX;		//�Ƿ������߽Ǹ�X֫
			BOOL bInboard;			//�Ƿ������Ǹ��ڲ�
		}MAIN_RAY;
		struct{	//
			THANDLE hMainJg;
			THANDLE hRayJg[2];
			BOOL bCutStart[2];		//�Ƿ������߽Ǹ�ͷ��
			BOOL bInMainWingX[2];	//�Ƿ������Ǹ�X֫
			BOOL bInRayWingX[2];	//�Ƿ������߽Ǹ�X֫
			BOOL bInboard[2];		//�Ƿ������Ǹ��ڲ�
		}RAY_RAY;
		struct{	//
			THANDLE hAngle;
			THANDLE hBolt;
			BOOL bCutStart;			//�Ǹ�ʼ���н�
			BOOL bCutWingX;			//�Ǹ֣�֫�н�
			BOOL bCutBer;			//����֫
		}ANGLE_BOLT;
	}relation;
	BYTE cCutMode;	//�Ǹ��иʽ��
	union CUT_PARA{
		struct{
			THANDLE hDatumJg;
			THANDLE hRayJg;
			BOOL bCutStart;
		}CUT_ONE_BY_DATUM;
		struct{
			THANDLE hDatumJg;
			THANDLE hRayJg;
			BOOL bCutStart;
		}CUT_ONE_BY_PLANE;
		struct{
			THANDLE hRayJg1;
			BOOL bCutStart1;
			THANDLE hRayJg2;
			BOOL bCutStart2;
		}CUT_TWO_BY_PLANE;
		struct{
			THANDLE hRayJg1;
			BOOL bCutStart1;
			BOOL bInRayWingX1;
			THANDLE hRayJg2;
			BOOL bCutStart2;
			BOOL bInRayWingX2;
		}CUT_TWO_BY_45DU;
		ANGLECUT_ADVICE CUT_BY_BOLT;
	}cut_para;
};
class CCutJg
{	//
public:
	CHashList<ANGLE_CUT_INFO> hashAngleCuts;
public:
	static BOOL IsMainAndRayRelation(CLDSLineAngle* pJg1,CLDSLineAngle* pJg2,BOOL* pCutStart=NULL,BOOL* pMainFirst=NULL);
	//��ʼ���Ǹ��и���Ϣ
	static BOOL InitJgAndJgCutInfo(ANGLE_CUT_INFO* pJgCutInfo,BYTE cFlag=0);
	//���нǸ��нǼ��
	static BOOL CollisionCheck(ANGLE_CUT_INFO* pJgCutInfo,BYTE cFlag=0);
	static BOOL CutDataCheck(ANGLE_CUT_INFO* pJgCutInfo);
	static int FinishCheckCutAngle(ANGLE_CUT_INFO* pJgCutInfo);
	//�Ǹ��нǲ���
	static BOOL FinishCutAngle(ANGLE_CUT_INFO* pJgCutInfo,int (*InputCollideDist)(int dist),int nCutDist=0,BOOL bCheck=FALSE);
	//
	void Empty(){hashAngleCuts.Empty();}
	~CCutJg(){Empty();}
};
struct ANGLE_CUTDATA
{
	/*��֫����˵��
	 *cut_wing[0]��ʾʼ��
	 *cut_wing[1]��ʾ�ն�
	 *cut_wing_para[0]��ʾʼ�˱���֫����
	 *cut_wing_para[1]��ʾ�ն˱���֫����
	 *cut_wing_para[][0]��ʾ�Ǹ�������ȡ����
	 *cut_wing_para[][1]��ʾ��һ֫��ȡ���
	 *cut_wing_para[][2]��ʾ֫������ȡ����
	*/
	long cut_wing[2];	//��ʶ���нǸ�֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
	double cut_wing_para[2][3];
	/*�нǲ���˵��
	 *cut_angle[0]��ʾ���X֫
	 *cut_angle[1]��ʾ���Y֫
	 *cut_angle[2]��ʾ�յ�X֫
	 *cut_angle[3]��ʾ�յ�Y֫
	 *cut_angle[][0]��ʾƽ�����߷�����ȡ��  
	 *cut_angle[][1]��ʾ��ֱ���߷�����ȡ��
	*/
	double cut_angle[4][2];
	BOOL m_bUserSpecStartCutAngle;
	BOOL m_bUserSpecEndCutAngle;
	ANGLE_CUTDATA(){memset(cut_angle,0,sizeof(cut_angle));memset(cut_wing_para,0,sizeof(cut_wing_para));memset(cut_wing,0,sizeof(cut_wing));}
};
struct BOLTSOLID_PARA{
	short d;	// ��˨ֱ��
  	double D;   // ��ñֱ��.
  	double H;	// ��˨ͷ�߶�.
  	double H2;	// ��ñ�߶�.
	BOLTSOLID_PARA(short boltd=0,double _D=0,double _H=0,double _H2=0);
public:
	static ARRAY_LIST<BOLTSOLID_PARA> BOLT_LIBRARY;
	static bool QuerySolidParams(short boltd,BOLTSOLID_PARA* pBoltSolidParams);
};

struct BOLTSOLID_COLUMN
{
	double radius,height;
	GEPOINT start,end;	//��ĩ��
	GEPOINT vStdAxis;	//start->end�ĵ�λ����
	BOLTSOLID_COLUMN(double r){radius=r;height=0;}
};
extern CCutJg cutJg;

//�и��϶
extern int InputCutAngleCollideDist(int dist);

#endif