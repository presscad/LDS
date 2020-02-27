#pragma once

#include <stddef.h>
#include <WinDef.h>
#include "XhCharString.h"
#include "ArrayList.h"
#include "List.h"
#include "Buffer.h"

typedef struct BOMBOLT_RECORD
{
	static const DWORD TWO_CAP;		//˫ñ��˨��ʶλ
	static const DWORD ANTI_THEFT;	//������˨��ʶλ
	static const DWORD ANTI_LOOSE;	//�����ɵ�Ȧ��ʶλ
	static const DWORD FOOT_NAIL;	//�Ŷ���˨��ʶλ
	static const DWORD REVERSE_NORM;//��ת��˨�����ʶλ
	//
	static const BYTE FUNC_COMMON	= 0;//0.������˨
	static const BYTE FUNC_FOOTNAIL	= 1;//1.�Ŷ����۸֡��Ǹֱ�ʾ�Ŷ��������ְ��ʾ�Ŷ���;�ĸְ�
	static const BYTE FUNC_WIREHOLE	= 2;//2.���߿�;
	static const BYTE FUNC_EARTHHOLE= 3;//3.�ӵؿ�;
	static const BYTE FUNC_SETUPHOLE= 4;//4.װ���;
	static const BYTE FUNC_BRANDHOLE= 5;//5.���ƿ�;
	static const BYTE FUNC_WATERHOLE= 6;//6.������
	static const BYTE FUNC_FOOTNAIL2= 7;//7.���׽Ŷ�

	//HIBERID hiberId;			//���洢,ʹ��+1��ռλ�洢��˨id
	float x,y;
	short d;
	float hole_d_increment;		//��˨��ֱ������
	short waistLen;				// ��Բ������
	BYTE  cFuncType;			// ���߿�
	BYTE  cFlag;				//��˨�׼ӹ����յ�������Ϣ�ı�ʶϸ�ڣ����->�ߵ�1λΪ0x00��ʾ��ף�Ϊ0x01��ʾ���
	//COORD3D waistVec;			// ��Բ�׷���
	//DWORD dwRayNo;				//��˨���������ߺ�
	DWORD m_dwFlag;			// ������Ҫ��ı�ʶλ����ANTITHEFT_BOLT(1),ANTILOOSE_BOLT(2),FOOTNAIL_BOLT(4)

	BOMBOLT_RECORD(){ memset(this,0,sizeof(BOMBOLT_RECORD)); }
	//void SetKey(DWORD key){hiberId.SetHiberarchy(0,0,2,key);}
	bool IsFootNail(){ return (m_dwFlag&FOOT_NAIL)!=0;}
	bool IsAntiTheft(){ return (m_dwFlag&ANTI_THEFT)!=0;}
	bool IsTwoCap(){ return (m_dwFlag&TWO_CAP)!=0;}
	bool IsAntiLoose(){ return (m_dwFlag&ANTI_LOOSE)!=0;}
	BOOL CloneBolt(BOMBOLT_RECORD *pNewBoltInfo);
	void FromBuffer(CBuffer &buffer,long version=NULL);
	void ToBuffer(CBuffer &buffer,long version=NULL);

}*BOMBOLT_RECORDPTR;
typedef struct BOMBOLT
{
	bool bFootNail;	//�Ŷ�
	bool bTwoCap;	//˫ñ��˨
	bool bAntiTheft;//������˨
	bool bAnitLoose;//������˨
	bool bUFootNail;//U�ͽŶ�
	short d;		//��˨ֱ��
	int nNoThreadlen;//�޿۳�
	short L;		//��˨��Ч�����Ȧ���,mm
	short L0;		//ͨ�����
	short hFamily;			//��˨������ϵ�б�ʶ��>=0��ʾ��˨��<0��ʾ��Ȧ
	float grade;			//��˨����
	long iSeg;				//��˨�������Ķκ�
	float fUnitWeight;		//����˨��
	CXhChar16 sMaterial;	//����
	//
	BYTE nCommonNut;	//��ͨ��ĸ
	BYTE nFastenNut;	//�۽���ĸ
	BYTE nAntiTheftNut;	//������ĸ
	BYTE nThinNut;		//����ĸ
	BYTE nCommonGasket;	//ƽ��Ȧ
	BYTE nSpringGasket;	//���ɵ�Ȧ
	BYTE nLockClasp;	//���ɿ�
private:
	long nPart;				//����������������������˨��
public:
	//����Ĭ��ֵ�޸�Ϊ0������ͳ������������˨ʱ����������1 wht 12-08-24
	BOMBOLT(){memset(this,0,sizeof(BOMBOLT));nPart=0;}
	CXhChar50 CombinedKey(bool bIncLsSegI=false);
	long AddPart(int add_num=1){nPart+=add_num;return nPart;}
	long GetPartNum(){return nPart;}
	void FromBuffer(CBuffer &buffer,long version=NULL);
	void ToBuffer(CBuffer &buffer,long version=NULL);
	BOOL IsDianQuan() const{return hFamily<0;}
}*BOMBOLT_PTR;

class BOMPART
{
protected:
	long nPart;		//һ�������о��еĵ�ǰ��ŵ��Σ�������������
public:
	static const BYTE OTHER = 0;//����ǳ��湹��
	static const BYTE BOLT  = 1;//��˨
	static const BYTE ANGLE = 2;//�Ǹ�
	static const BYTE PLATE = 3;//�ְ�
	static const BYTE TUBE  = 4;//�ֹ�
	static const BYTE FLAT  = 5;//���
	static const BYTE SLOT  = 6;//�۸�
	static const BYTE ROUND = 7;//Բ��
	static const BYTE ACCESSORY = 8;//����
	//����������
	const static BYTE SUB_TYPE_TUBE_MAIN	= 1;	//����
	const static BYTE SUB_TYPE_TUBE_BRANCH	= 2;	//֧��
	const static BYTE SUB_TYPE_PLATE_C		= 3;	//���Ͳ��
	const static BYTE SUB_TYPE_PLATE_U		= 4;	//U�Ͳ��
	const static BYTE SUB_TYPE_PLATE_X		= 5;	//ʮ�ֲ��
	const static BYTE SUB_TYPE_PLATE_FL		= 6;	//����
	const static BYTE SUB_TYPE_PLATE_WATER	= 7;	//��ˮ��
	const static BYTE SUB_TYPE_FOOT_FL		= 8;	//�׽ŷ���
	const static BYTE SUB_TYPE_ROD_Z		= 9;	//����
	const static BYTE SUB_TYPE_ROD_F		= 10;	//����
	const static BYTE SUB_TYPE_ANGLE_SHORT	= 11;	//�̽Ǹ�
	const static BYTE SUB_TYPE_PLATE_FLD	= 12;	//�Ժ�����
	const static BYTE SUB_TYPE_PLATE_FLP	= 13;	//ƽ������
	const static BYTE SUB_TYPE_PLATE_FLG	= 14;	//���Է���
	const static BYTE SUB_TYPE_PLATE_FLR	= 15;	//���Է���
	const static BYTE SUB_TYPE_BOLT_PAD		= 16;	//��˨���
	const static BYTE SUB_TYPE_TUBE_WIRE	= 17;	//�׹�
	const static BYTE SUB_TYPE_STEEL_GRATING= 18;	//�ָ�դ
	const static BYTE SUB_TYPE_STAY_ROPE	= 19;	//��������
	const static BYTE SUB_TYPE_LADDER		= 20;	//����

	char cPartType;
	CXhChar16 sMappingDrwLabel;	//�������ݱȶ�ʱ����ʾ��ǰ������ԭͼֽ��ϸ�е�ƥ���Ӧ�Ĺ������
public:
	CXhChar16 sPartNo;	//�������
	int iSeg;			//�κ�
	short siSubType;	//
	char cMaterial;		//���ʼ��ַ����磺'S','H','G'
	CXhChar16 sMaterial;//���ò����ַ������ڴ����¼Ӳ���,�磺Q355 wht 19-08-25
	char cQualityLevel;	//���������ȼ�
	double wide;		//������Ȳ���
	double thick;		//������Ȳ���
	double wingWideY;	//�Ǹ�Y֫���
	CXhChar16 sSpec;	//���
	double length;		//�������Ȳ���
	double fPieceWeight;//������(kg)�����ڸְ��Ϊ��С���ݾ�������
	double fSumWeight;	//����
	short siZhiWan;		//>0�����������<=0����Ҫ����
	BOOL bWeldPart;		//�Ƿ���Ҫ����
	BOOL bLegPart;		//�Ƿ�Ϊ�Ȳ�����
	short nM16Ls;	//M16��˨����
	short nM20Ls;	//M20��˨����
	short nM24Ls;	//M24��˨����
	short nMXLs;	//����ֱ����˨����
	short nMSumLs;	//��˨������
	char sNotes[50];	//������ע˵��
	long feature1;		//��������1����ʱʹ��
	long feature2;		//��������2����ʱʹ��
	//��˨��Ϣ
	ARRAY_LIST<BOMBOLT_RECORD> m_arrBoltRecs;
public:
	BOMPART();
	~BOMPART(){;}
	long AddPart(int add_num=1){nPart+=add_num;return nPart;}
	long GetPartNum(){return nPart;}
	int GetBoltOrHoleNum(double min_d,double max_d=-1,BOOL bHoleD=TRUE);
	BOOL IsHasLinearHole();
	BOOL IsHasSpecFuncHole();
	BOOL IsDrillHole();
	BOOL IsExpandHole();
	bool get_bRevMappingPart(){return sMappingDrwLabel.At(0)!=0;}
	__declspec(property(get=get_bRevMappingPart)) bool bRevMappingPart;
	void set_szMappingDrwLabel(const char* _szMappingDrwLabel){sMappingDrwLabel=_szMappingDrwLabel;}
	const char* get_szMappingDrwLabel(){return bRevMappingPart?sMappingDrwLabel:sPartNo;}
	__declspec(property(put=set_szMappingDrwLabel,get=get_szMappingDrwLabel)) char* szMappingDrwLabel;
	virtual double CalZincCoatedArea(BOOL bSwitchToM=TRUE){return 0;}	//��п���
	virtual short CutAngleN(){return 0;}
	virtual CXhChar16 GetSpec(){ return sSpec;}
	virtual short GetHuoquLineCount(BOOL bIncRollEdge=TRUE);
	virtual double GetHuoquDegree(int index){return 0;}
	virtual int GetLsHoleStr(char *hole_str,BOOL bIncUnit=TRUE);	//��˨���ַ���
	virtual CXhChar16 GetPartTypeName(BOOL bSymbol=FALSE);
	virtual CXhChar16 GetPartSubTypeName();
	virtual char* GetPartNo(int nMatCharPosType=0,char cMatSeparator=0,bool bIncQualityLevel=false);
	virtual void FromBuffer(CBuffer &buffer,long version=NULL);
	virtual void ToBuffer(CBuffer &buffer,long version=NULL);
	virtual double GetWeldLength(){return 0;}
};
struct BOMPROFILE_VERTEX{
	//HIBERID hiberId;				//���洢,ʹ��+2��ռλ�洢�ְ�������
	struct COORD2D{
		double x,y;
		COORD2D(double* coord=NULL, int dimension=2);
		operator double*(){return &x;}
		void Set(double coordX=0,double coordY=0){x=coordX; y=coordY;}
	};
	struct COORD3D{
		double x,y,z;
		COORD3D(double* coord=NULL, int dimension=3);
		operator double*(){return &x;}
		void Set(double coordX=0,double coordY=0,double coordZ=0){x=coordX; y=coordY; z=coordZ;}
	};
	BYTE type;						//1:��ֱͨ�� 2:Բ�� 3:��Բ��
	bool m_bWeldEdge;				//�Ƿ񺸷��
	bool m_bRollEdge;				//�˱�����
	short roll_edge_offset_dist;	//���ƫ�ƾ���
	double radius;					//��Բ������Ҫ�Ĳ���
	double sector_angle;			//��׼Բ������Ҫ�Ĳ���
	short manu_space;				//����ӹ�Ԥ����϶���߸߶�(mm)
	COORD2D vertex;					//�����λ������
	COORD3D center,column_norm;	//��Բ������Ҫ�Ĳ�������Զ�����������),center�����ʾԲ��Բ��(ʰȡ��)ʱ���������������
	COORD3D work_norm;				//Բ�����߷���
	//double local_point_y;			//�Ի��λ�ã��ڸõ������ע
	//int local_point_vec;			//0.��ʾ�޶Ի�㣬1.��ʾ�Ի�������ע50-->100�ķ����뺸�Ӹ��˼���start-->end������ͬ��2.��ʾ�෴
	CXhChar16 sWeldMotherPart;		//���ӷ����ڵĺ���ĸ����
	double edge_len;				//�߳�
public:	//�������
	static const char STATE_NEED_SLOPECUT  = 0x01;	//��Ҫ�����¿�
	static const char STATE_FSQUARE_WELD   = 0x02;	//����ƽ������ͻ�����죩������Ĭ��Ϊ����
	static const char STATE_BSQUARE_WELD   = 0x04;	//����ƽ������ͻ�����죩������Ĭ��Ϊ����
	char cStateFlag;	//0.�¿ڣ�2.����ƽ(��)����4.����ƽ(��)��
	short height;		//����߶�,mm
	short length;		//���쳤��,mm
	short lenpos;		//�������λ��(�Ա�ʼ�˵��ƫ����), mm
public:
	BOMPROFILE_VERTEX(const double* coord2d=NULL, int dimension=2);
	BOMPROFILE_VERTEX(double x,double y);
	//void SetKey(DWORD key){hiberId.SetHiberarchy(0,0,0,key);}
	virtual void FromBuffer(CBuffer &buffer,long version=NULL);
	virtual void ToBuffer(CBuffer &buffer,long version=NULL);
};
class PART_PLATE : public BOMPART
{
public:
	BYTE m_cFaceN;			//����(�������)
	BOOL bNeedFillet;		//��Ҫ�¿�
	static const int TYPE_TOWER_FOOT	  = 1;	//���Ű�
	static const int TYPE_FOLD_3FACE	  = 2;	//�۵���
	static const int TYPE_CUT_MOUTH_3FACE = 3;	//��������
	int m_iPlateType;		//BOOL bTowerFootPlate;		//���Ű�
	double fPieceNetWeight;	//������(kg)����Ϊ������������
	double min_area;		//���Ӱ���С���ݾ������(mm2)
	double real_area;		//���������(mm2)
	double fPerimeter;		//�ְ��ܳ�
	double fCutMouthLen;	//��ڳ��� wht 19-06-27
	double fWeldEdgeLen;	//���ӱ߳���
	double fWeldLineLen;	//�����߳��ȣ����ӱ߳��� X ����ϵ��
	double fFlInnerD;		//������Բֱ��
	double fFlOutterD;		//������Բֱ��
	double fFlBoltLayoutD;	//������˨����ֱ��
	double huoQuAngleArr[2];	//faceAngleArr[0] ����1�����2��Ƕ�
							    //faceAngleArr[1] ����2�����3��Ƕ�
 	BOMPROFILE_VERTEX::COORD3D huoquFaceNormArr[2];
	//��������Ϣ
	CXhSimpleList<BOMPROFILE_VERTEX> listVertex;
public:
	BOOL IsTowerFootPlate() { return m_iPlateType == TYPE_TOWER_FOOT; }
	__declspec(property(get = IsTowerFootPlate)) BOOL bTowerFootPlate;
public:
	PART_PLATE();
	long GetEdgeCount();
	long GetArcEdgeCount();
	BOOL IsFL();
	BOOL IsRect();	
	virtual double CalZincCoatedArea(BOOL bSwitchToM=TRUE);
	virtual short CutAngleN();
	virtual short GetHuoquLineCount(BOOL bIncRollEdge=TRUE);
	virtual double GetHuoquDegree(int index);
	virtual CXhChar16 GetSpec();
	virtual void FromBuffer(CBuffer &buffer,long version=NULL);
	virtual void ToBuffer(CBuffer &buffer,long version=NULL);
	virtual double GetWeldLength(){return fWeldLineLen;}
};
class PART_TUBE : public BOMPART
{	//�ֹܼӹ�����---���/����
public:
	struct TUBE_PROCESS
	{
		static const BYTE PROCESSTYPE_NONE		= 0;	//�����⹤��
		static const BYTE PROCESSTYPE_TRANSECT	= 1;	//Բ�����
		static const BYTE PROCESSTYPE_PLANECUT	= 2;	//ƽ�����
		static const BYTE PROCESSTYPE_CSLOT		= 3;	//һ�Ϳ��ڲ�
		static const BYTE PROCESSTYPE_USLOT		= 4;	//U�Ϳ��ڲ�
		static const BYTE PROCESSTYPE_TSLOT		= 5;	//T�Ϳ��ڲ�
		static const BYTE PROCESSTYPE_XSLOT		= 6;	//ʮ���Ϳ��ڲ�
		static const BYTE PROCESSTYPE_FL		= 7;	//����
		static const BYTE PROCESSTYPE_FLD		= 8;	//�Ժ�����
		static const BYTE PROCESSTYPE_FLP		= 9;	//ƽ������
		static const BYTE PROCESSTYPE_FLG		= 10;	//���Է���
		static const BYTE PROCESSTYPE_FLR		= 11;	//���Է���
		BYTE type;				//�ֹܹ�������
		CXhChar16 sSpec;		//���ʱ���ܹ��
		double L;				//���ڲ۳���L�����ʱ֧�ܳ���
		double A;				//���ڲۿ��A�����ʱ������֧��֮��ļнǻ�ֹ��������и���֮��ļн�
		double B;				//T�Ͳۿ��B��U�Ͳ���Բ���뾶
		double fNormOffset;		//���ڲ۷���ƫ����
		BOOL IsFL(){return PROCESSTYPE_FL==type||PROCESSTYPE_FLD==type||PROCESSTYPE_FLP==type||
						   PROCESSTYPE_FLR==type||PROCESSTYPE_FLG==type;}
		TUBE_PROCESS(){memset(this,0,sizeof(TUBE_PROCESS));}
	};
	TUBE_PROCESS startProcess;	//ʼ�˹�������
	TUBE_PROCESS endProcess;	//�ն˹�������
	BOOL bHasNodePlate;			//�ֹ��м����нڵ��
	BOOL bHasBranchTube;		//�ֹ��м���������֧��
	BOOL bHasFootNail;			//�Ƿ���Ҫ�Ŷ�
	BOOL bHasWeldRoad;			//�Ƿ��к�����
public:
	PART_TUBE();
	int GetNotchType(char *type,int nRuleType);
	virtual double CalZincCoatedArea(BOOL bSwitchToM=TRUE);
	virtual CXhChar16 GetPartTypeName(BOOL bSymbol=FALSE);
	virtual void FromBuffer(CBuffer &buffer,long version=NULL);
	virtual void ToBuffer(CBuffer &buffer,long version=NULL);
};
#ifndef __DEF_KAIHEJIAO_STRUC__
#define __DEF_KAIHEJIAO_STRUC__
struct KAI_HE_JIAO
{
	float decWingAngle;		//��֫�н�
	short position;			//�궨λ��
	short startLength;		//ʼ�˿��ϳ���
	short endLength;		//�ն˿��ϳ���
	KAI_HE_JIAO() {memset(this,0,sizeof(KAI_HE_JIAO));}
};
#endif
#ifndef __DEF_ANGLE_PUSH_FLAT_STRUC__
#define __DEF_ANGLE_PUSH_FLAT_STRUC__
struct ANGLE_PUSH_FLAT
{
	short push_flat_x1_y2;	//X֫��1��Y֫��2
	short flat_start_pos;
	short flat_end_pos;
	ANGLE_PUSH_FLAT() { memset(this, 0, sizeof(ANGLE_PUSH_FLAT)); }
};
#endif
class PART_ANGLE : public BOMPART
{
public:
	float wing_angle;	//�Ǹ���֫�н�
	BOOL bCutAngle;		//�Ƿ��н�
	BOOL bCutBer;		//�Ƿ���Ҫ����
	BOOL bCutRoot;		//�Ƿ���Ҫ���
	BOOL bKaiJiao;		//�Ƿ���Ҫ����
	BOOL bHeJiao;		//�Ƿ���Ҫ�Ͻ�
	BOOL bHasFootNail;	//�Ƿ���Ҫ�Ŷ�
	int nPushFlat;		//0x01:ʼ��ѹ�� 0x02:�м�ѹ�� 0x04:�ն�ѹ��
	// --------- ��֫ -----------
	//��ʶ���нǸ�֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
	int cut_wing[2];
	/*cut_wing_para[0]cut_wing_para[1]��ʾ�ն��н�
	...[][0]��ʾ�Ǹ�������ȡ����
	...[][1]��ʾ��һ֫��ȡ���
	...[][2]��ʾ֫������ȡ����
	*/
	int cut_wing_para[2][3];
    // --------- �н� -----------
	/*
	cut_angle[0]��ʾ���X֫
	cut_angle[1]��ʾ���Y֫
	cut_angle[2]��ʾ�յ�X֫
	cut_angle[3]��ʾ�յ�Y֫
	...[][0]��ʾƽ�����߷�����ȡ��  ...[][1]��ʾ��ֱ���߷�����ȡ��
	*/
	int cut_angle[4][2];
	double fWeldLineLen;	//���쳤��
	CXhSimpleList<KAI_HE_JIAO> listKaihe;		//���Ͻ�
	CXhSimpleList<ANGLE_PUSH_FLAT> listPushFlat;//ѹ��
	struct POLY_ANGLE_INFO
	{
		int nIndex;				//��������
		BOMPROFILE_VERTEX::COORD3D norm_x_wing;	//X֫չ������
		BOMPROFILE_VERTEX::COORD3D norm_y_wing;	//Y֫չ������
		BOMPROFILE_VERTEX::COORD3D len_vec;		//���췽��
	};
	CXhSimpleList<POLY_ANGLE_INFO> listPolyAngle;	//����Ǹ��ӽǸ�
public:
	PART_ANGLE();
	bool IsCutAngleOrWing(); 
	bool IsStartCutAngleOrWing();
	bool IsEndCutAngleOrWing();
	BYTE GetHuoquType(int *huoqu_wing_x0_y1_all2=NULL);	//0.������ 1.���� 2.���� 3.������ 4.������
	virtual double GetHuoquDegree(int index);
	virtual double CalZincCoatedArea(BOOL bSwitchToM=TRUE);
	virtual short CutAngleN();
	virtual short KaiHeJiaoN();
	virtual CXhChar16 GetSpec();
	virtual void FromBuffer(CBuffer &buffer,long version=NULL);
	virtual void ToBuffer(CBuffer &buffer,long version=NULL);
	virtual double GetWeldLength(){return fWeldLineLen;}
};
class PART_SLOT : public BOMPART
{
public:
	PART_SLOT();
	virtual void FromBuffer(CBuffer &buffer,long version=NULL);
	virtual void ToBuffer(CBuffer &buffer,long version=NULL);
};
class BOM_PARTREF
{
public:
	CXhChar16 sPartNo;
	int nPartNum;
	BOM_PARTREF(){nPartNum=0;}
	void ToBuffer(CBuffer &buffer,long version=NULL);
	void FromBuffer(CBuffer &buffer,long version=NULL);
};
class BOM_WELDPART
{
public:
	static const BYTE TYPE_COMMON		= 0;	//��ͨ�麸��
	static const BYTE TYPE_ANGLEFOOT	= 1;	//�Ǹ�����
	static const BYTE TYPE_TUBEFOOT		= 2;	//�ֹ�����
	static const BYTE TYPE_PLATES		= 3;	//���庸��
	static const BYTE TYPE_ANGLERIBPLATE= 4;	//�Ǹ��߰�
	static const BYTE TYPE_ANGLELJB		= 5;	//�Ǹ�����
	BYTE cWeldPartType;
	int iSeg;
	CXhChar16 sName;
	CXhChar16 sPartNo;
	int nWeldPartNum;
	CXhSimpleList<BOM_PARTREF> listPartRef;
	BOM_WELDPART(){iSeg=0;nWeldPartNum=0;cWeldPartType=0;}
	void ToBuffer(CBuffer &buffer,long version=NULL);
	void FromBuffer(CBuffer &buffer,long version=NULL);
};
class CModelBOM
{
public:
	CXhChar100 m_sPrjCode;		//���̱��
	CXhChar100 m_sCompanyName;	//��Ƶ�λ
	CXhChar50 m_sPrjName;		//��������
	CXhChar100 m_sTowerTypeName;//������
	CXhChar50 m_sTaAlias;		//���ţ������ţ�
	CXhChar50 m_sTaStampNo;		//��ӡ��(��������)
	CXhChar50 m_sOperator;		//����Ա���Ʊ��ˣ�
	CXhChar50 m_sAuditor;		//�����
	CXhChar50 m_sCritic;		//������
	int m_nTaNum;				//������
	int m_iCirclePlankAreaType;	//1.��ʾ��С���ݾ���ͳ��;2.��ʾ�����ͳ��(ȥ���ڻ�)
	int m_iPlankWeightStatType;	//0.��ʾ��������ͳ��;1.��ʾ��С���ݾ���ͳ��;2.��ʾ�����ͳ��
	BYTE m_biMatCharPosType;	//������ŵĲ����ַ�λ������
	char m_cMatCharSeparator;	//������ŵĲ����ַ��ָ���: ��0  �ո�' '
	//
	CXhChar50 m_sSegStr;		//�κ�
	CXhChar100 m_sInstanceName;	//��ʱ�洢�������ţ�
	CXhChar16 m_sInstanceNo;	//��ʱ�洢����ţ�
	//
	int m_nPartsN,m_nBoltsN,m_nWeldPartsN;
	CXhSuperList<BOMPART> listParts;	//������¼����
	BOMBOLT* m_pBoltRecsArr;			//��˨��¼����
	CXhSimpleList<BOM_WELDPART> listWeldParts;	//�麸��
public:
	CModelBOM();
	~CModelBOM();
	bool ToBuffer(CBuffer &buffer,long version=NULL);
	bool FromBuffer(CBuffer &buffer,long version=NULL);
};