// TowerType.h: interface for the CMaterialTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOWERTYPE_H__5DF4AC0B_CE1E_4592_8EE3_C5E71DDA0BFB__INCLUDED_)
#define AFX_TOWERTYPE_H__5DF4AC0B_CE1E_4592_8EE3_C5E71DDA0BFB__INCLUDED_
#include "f_ent_list.h"
#include "Tower.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
//��������Ҫ���ڴ洢ͳ����Ϣ
//
struct PART_LS
{
	int d;
	double hole_d_increment;
	double x,y;
};
typedef PART_LS* PART_LSPTR;
class CLsLine
{
	ATOM_LIST<PART_LSPTR>* pLsList;
public:
	CLsLine();
	~CLsLine();
	double g;
	PART_LS* AppendPartLsAndSort(PART_LS* pPartLs);
	PART_LS* GetFirstPartLs();
	PART_LS* GetNextPartLs();
	PART_LS* GetPrevPartLs();
	PART_LS* GetTailPartLs();
};
class CLsLineSet
{
public:
	CLsLineSet(){;}
	ATOM_LIST<CLsLine> lslineset;
	PART_LS* AppendPartLsAndSort(PART_LS* pPartLs,double epsilon);
};
//�ֹܹ�������
enum TUBE_PROCESS_TYPE
{
	PROCESSTYPE_NONE	= 0,	//�����⹤��
	PROCESSTYPE_TRANSECT= 1,	//Բ�����
	PROCESSTYPE_PLANECUT= 2,	//ƽ�����
	PROCESSTYPE_CSLOT	= 3,	//һ�Ϳ��ڲ�
	PROCESSTYPE_USLOT	= 4,	//U�Ϳ��ڲ�
	PROCESSTYPE_TSLOT	= 5,	//T�Ϳ��ڲ�
	PROCESSTYPE_XSLOT	= 6,	//ʮ���Ϳ��ڲ�
	PROCESSTYPE_FL		= 7,	//����
	PROCESSTYPE_FLD		= 8,	//�Ժ�����
	PROCESSTYPE_FLP		= 9,	//ƽ������
	PROCESSTYPE_FLG		= 10,	//���Է���
	PROCESSTYPE_FLR		= 11,	//���Է���
};

//�ֹܼӹ�����---���/����
struct TUBE_PROCESS
{
	TUBE_PROCESS_TYPE type;	//�ֹܹ�������
	char guige[MAX_CHAR_GUIGE_16+1];//���ʱ���ܹ��
	double L;				//���ڲ۳���L�����ʱ֧�ܳ���
	double A;				//���ڲۿ��A�����ʱ������֧��֮��ļнǻ�ֹ��������и���֮��ļн�
	double B;				//T�Ͳۿ��B��U�Ͳ���Բ���뾶
	double fNormOffset;		//���ڲ۷���ƫ����
	BOOL IsFL(){return type==PROCESSTYPE_FL||type==PROCESSTYPE_FLD||
					   type==PROCESSTYPE_FLP||type==PROCESSTYPE_FLG||type==TYPE_FLR;}
};

//����������
enum PART_SUB_TYPE
{
	TUBE_MAIN	=1,		//����
	TUBE_BRANCH,		//֧��
	PARAMPLATE_C,		//���Ͳ��
	PARAMPLATE_U,		//U�Ͳ��
	PARAMPLATE_X,		//ʮ�ֲ��
	PARAMPLATE_FL,		//����
	PARAMPLATE_WATER,	//��ˮ��
	PLATE_FL,			//�׽ŷ���
};
class PART
{
	long nPart;		//һ�������о��еĵ�ǰ��Ź�������
	char sPartNo[MAX_CHAR_GUIGE_16+1];		//�������
	char sPartNoIncMat[MAX_CHAR_GUIGE_16+2];//�����ʵĹ������,ͨ��GetPartNo()������ֵ������
public:
	PART(){memset(this,0,sizeof(PART));nPart=0;}
	PART(PART &part){*this = part;}
	~PART(){if(ls_arr) delete []ls_arr;}
	int AddPart(int add_num=1){nPart+=add_num;return nPart;}
	void operator =(PART &part)
	{
		if(ls_arr)
			delete[] ls_arr;
		memcpy(this,&part,sizeof(PART));
		if(part.nMSumLs>0)
		{
			ls_arr = new PART_LS[part.nMSumLs];
			memcpy(ls_arr,part.ls_arr,sizeof(PART_LS)*part.nMSumLs);
		}
		else
			ls_arr = NULL;
	}
	//��ȡ�ı���ʽ�����˱����ݡ����У�!:M12;*:M16;#:M20;$:M24;?:��˨��ֱ���ǳ������ʵ���ٳ��
	int GetLsStr(char *ls_str);
	//��ȡ���⹤�ձ�ע��Ϣ	bIncAngle��ʾ�����ַ����Ƿ�������ϽǶ��� wht 11-05-23
	int GetProcessNotes(char *notes,BOOL bIncKaiHeAngle=FALSE);
	//��ȡ���ڲ����� nRuleType �������ͱ�Ź��� wht 10-10-11
	int GetNotchType(char *type,int nRuleType);
	//��˨���ַ���
	int GetLsHoleStr(char *hole_str,BOOL bIncUnit=TRUE);
	//�Ƿ�������
	BOOL IsTubeTransect();
	//�Ƿ���Ҫ��ڲ�
	BOOL IsTubeNotch();	
	//�õ�ָ���׾��Ŀ��� bSpecifyScope==TRUE; ָ����Χ�ڵĿ׸���
	//					 bSpecifyScope==FALSE; ֱ��=min_d�Ŀ׸���
	int GetLsHoleNum(double min_d,double max_d=-1,BOOL bHoleD=TRUE,BOOL bSpecifyScope=TRUE);
	long GetPartNum(){return nPart;}
	//�������
	void SetPartNo(const char *part_no) { strncpy(sPartNo,part_no,16);}
	char* GetPartNo(int nMatCharPosType=0,char cMatSeparator=0);
public:
	SEGI iSeg;			//�κ�
	char part_type;		//0.�ڵ� 1.�Ǹ� 2.��˨ 3.���� 4.�ֹ� 5.��� 6.�۸� 100.����
	short sub_type;		//  
	char cMaterial;		//���ʼ��ַ����磺'S','H','G'
	char cQualityLevel;	//���ϵ������ȼ����磺'B','C','D'
	double wide;		//������Ȳ���
	double thick;		//������Ȳ���
	double y_wing_wide;	//�Ǹ�Y֫���
	CXhChar16 sSpec;	//���
	double length;		//�������Ȳ���
	double fPieceMinWeight;	//������(kg)����Ϊ��С���ݾ���������
	double fPieceNetWeight;	//������(kg)����Ϊ������������
	double min_area;		//���Ӱ���С���ݾ������(mm2)
	double real_area;		//���������(mm2)
	double wing_angle;	//�Ǹ���֫�н�
	double fRollHeight;	//��߸߶�
	BOOL bZhiWan;		//�Ƿ���Ҫ����
	BOOL bWeldPart;		//�Ƿ���Ҫ����
	BOOL bNeedFillet;	//��Ҫ�¿�
	BOOL bCutAngle;		//�Ƿ��н�
	BOOL bCutBer;		//�Ƿ���Ҫ����
	BOOL bCutRoot;		//�Ƿ���Ҫ���
	BOOL bKaiJiao;		//�Ƿ���Ҫ����
	BOOL bHeJiao;		//�Ƿ���Ҫ�Ͻ�
	BOOL bHasFootNail;	//�Ƿ񺬽Ŷ�
	BOOL bRollEdge;		//�Ƿ���
	BOOL bDianBan;		//�Ƿ�Ϊ���
	//BOOL bPushFlat;		//�Ƿ�ѹ��
	BOOL bLegPart;		//�Ƿ�Ϊ�Ȳ�����
	BOOL bHasNodePlate;	//�ֹ��м����нڵ��		//wht 05-31
	BOOL bHasBranchTube;//�ֹ��м���������֧��
	int nPushFlat;		//0��ѹ�� 1һ��ѹ�� 2����ѹ�� 3�м�ѹ��
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
	short nM16Ls;	//M16��˨����
	short nM20Ls;	//M20��˨����
	short nM24Ls;	//M24��˨����
	short nMXLs;	//����ֱ����˨����
	short nMSumLs;	//��˨������
	short nEdgeNum;	//�ְ����
	double fPerimeter;	//�ְ��ܳ�
	double fWeldEdgeLen;//���ӱ߳���
	PART_LS *ls_arr;
	char sNotes[51];	//������ע˵��
	TUBE_PROCESS startProcess;	//�ֹ�ʼ�˼ӹ�����	wht 09-08-31
	TUBE_PROCESS endProcess;	//�ֹ��ն˼ӹ�����
};
struct PARTREF
{
	//long idPart;	//ӳ�䵽PART,��PART���id���Ӧ
	char sPartNo[MAX_CHAR_GUIGE_16+1+1];//ӳ�䵽PART,��PART���sPartNo���Ӧ
	short num;		 //�˹�����ĳһ�麸���е�����
	PARTREF(){num=0;}
};
//��Ҫ�����������ɹ�����Ҫ���������һ������.
class CStatWeldPart
{
	ATOM_LIST<PARTREF> *partref_arr;
public:
	SEGI iSeg;		//
	short num;		//�˺�����ϼ��ڵ������еĸ���
	char sPartNo[MAX_CHAR_GUIGE_16+1],sName[MAX_TEMP_CHAR_50+1];
	CStatWeldPart(){
		num = 0;
		strcpy(sPartNo,"");
		strcpy(sName,"");
		partref_arr = new ATOM_LIST<PARTREF>;
	}
	~CStatWeldPart(){
		if(partref_arr!=NULL)
		{
			delete partref_arr;
			partref_arr=NULL;
		}
	}
	ATOM_LIST<PARTREF>* GetPartRefArrPtr(){return partref_arr;}
};
// ͳ���üӹ�����ģ����
class CStatModule
{
public:
	CHashList<SEGI> segNoHashList;//�����ڰ����ĶκŹ�ϣ��
	long iNo;		//�������ڲ�Ψһ
	char description[MAX_TEMP_CHAR_50+1];	//�������ߵ��ַ���
	double m_fBurglarLsStartZ,m_fBurglarLsEndZ;
	CStatModule(){
		iNo=0;
		strcpy(description,"");
		m_fBurglarLsStartZ=m_fBurglarLsEndZ=0;
		segNoHashList.Empty();
	}
	void GetIncSegStr(CString &ss);
};
class CLsRecord
{
	long nPart;				//����������������������˨��
public:
	short typels0_pad1;		//0��ʾ��˨1��ʾ��Ȧ
	long iModuleNo;
	char sModule[MAX_TEMP_CHAR_50+1];//ģ�ͼ�����
	char grade[8];			//��˨����
	SEGI iSeg;				//��˨�������Ķκ�
	long hFamily;			//��˨���ϵ�б�ʶ
	long d;
	long L,L0_uplim,L0_downlim;	//��˨��Ч������ͨ�����
	long no_thread_len;			//�޿۳�
	//double unit_weight;	//����˨��
	double sum_weight;		//����˨��
	long nTwoCap;			//���к���˫ñ��˨������
	long nBurglarproof;		//���к��з�����˨������
	long nSpringPad;		//���к��д����ɵ�Ȧ������
	BOOL bFootNail;	//�Ŷ�
public:
	//����Ĭ��ֵ�޸�Ϊ0������ͳ������������˨ʱ����������1 wht 12-08-24
	CLsRecord(){memset(this,0,sizeof(CLsRecord));nPart=0;}
	~CLsRecord(){;}
	long GetLsPadCount();			//��ȡ��˨��Ȧ���� 
	long GetCommonLsCount();		//��ȡ��ͨ��˨����
	long GetTwoCapLsCount();		//��ȡ˫ñ��˨����
	long GetCommonNailCount();		//��ȡ��ͨ�Ŷ�����
	long GetBurglarproofLsCount();	//��ȡ������˨����
	long GetBurglarproofNailCount();//��ȡ�����Ŷ�����
	double GetUnitWeight(BYTE ciLsFlag=0);	//0.��ͨ|1.����|2.����
	int AddPart(BITFLAG flagBolt,int add_num=1);
	long GetPartNum(){return nPart;}
};


struct SEGPARTNO_RECORD
{
	int	   N;	 
	double length;			//��������	(mm)
	double unit_weight;	//����������(kg)
	char sPartNo[MAX_CHAR_GUIGE_16+1];		//�������
	char guige[MAX_CHAR_GUIGE_16+1];		//���
	char sNotes[MAX_TEMP_CHAR_50+1];		//��ע
};
struct SEGLS_SPEC_RECORD
{
	int	   N;	 
	//double unit_weight;	//����������(kg)
	int d,L;
	char grade[10];
	char guige[MAX_CHAR_GUIGE_16+1];//���
	char sNotes[MAX_TEMP_CHAR_50+1];//��ע
};
struct PARTSPEC_RECORD
{
	char guige[MAX_CHAR_GUIGE_16+1];	//���
	ATOM_LIST<double> bodyWeightList;	//�ӹ�ģ��������
	ATOM_LIST<double> segWeightList;	//�ӹ��κ�������
};
class CFlRecord
{
	long nPart;			//����������
	CXhChar16 sPartNo;	//�������
public:
	SEGI iSeg;			//�κ�
	double thick;		//���
	double D;			//�����⾶
	double d;			//�����ھ�
	double K;			//����ֱ��
	int nLsNum;			//��˨����
	int ls_d;			//��˨ֱ��
	char sNotes[50];	//������ע˵��
public:
	CFlRecord(){memset(this,0,sizeof(CFlRecord));nPart=0;}
	~CFlRecord(){;}
	int AddPart(int add_num=1){nPart+=add_num;return nPart;}
	long GetFLNum(){return nPart;}
	void SetPartNo(const char *part_no) {strncpy(sPartNo,part_no,16);}
	char* GetPartNo(){return sPartNo;}
	char* GetSpec(){return CXhChar100("D%g*K%g*d%g",D,K,d);}
};
class CTowerType
{
	char limit_str[MAX_TEMP_CHAR_500+1];
	CTower *m_pTower;	//��¼��ǰ����ͳ�Ƶ�����
	//Ϊͳ�Ʋ������ú�ͳ���������ݴ��빲�ã���¼IModelָ�룬������ƺ���ʱʹ��m_pTower
	//ͳ�ƹ���ʱͳһʹ��m_pModel������ͳ�Ʋ����ڹ���ʱ����Ҫ�л�m_pModelΪ��ǰ����ָ�룬
	//�����ò����ڹ����κż���ļ��ɣ�ͳ���겿�����ٽ�m_pModel�л�Ϊm_pTower wht 12-09-13
	IModel *m_pModel;	
public:
	static int CustomerSerial;	//�ͻ����Ʊ�ʶ���
	bool m_bStatLsBySegI;
	char m_sTypeName[MAX_TEMP_CHAR_50+1];	//������������
	char m_sTypeCode[MAX_TEMP_CHAR_50+1];	//���ʹ���
	char description[MAX_TEMP_CHAR_50+1];	//�������������������
	char voltGrade[MAX_TEMP_CHAR_50+1];		//��ѹ�ȼ�(���͵ķ�������֮һ����ÿ����ֻ����һ����ѹ�ȼ�
	char viceType[MAX_TEMP_CHAR_50+1];		//���֣�ת�ǣ��ն˺�ֱ������ÿ����ֻ��������֮һ
	CTowerType(){
		m_pTower=NULL;
		m_pModel=NULL;
		m_bStatLsBySegI=false;
		strcpy(m_sTypeName,"");
		strcpy(m_sTypeCode,"");
		strcpy(description,"");
		strcpy(voltGrade,"");
		strcpy(viceType,"");
	}
private:
	void SummarizeParts();
	//void SummarizeOnePart(CLDSPart *pPart);
	void SummarizeOneLineAngle(CLDSLineAngle *pLineAngle,char *limit_str);
	void SummarizeOneLineTube(CLDSLineTube *pTube,char *limit_str);
	void SummarizeOneLineSlot(CLDSLineSlot *pLineSlot,char *limit_str);
	void SummarizeOneLineFlat(CLDSLineFlat *pLineFlat,char *limit_str);
	void SummarizeOneBolt(CLDSBolt *pBolt,char *limit_str);
	void SummarizeOnePlate(CLDSPlate *pPlate,char *limit_str);
	void SummarizeOneParamPlate(CLDSParamPlate *pParamPlate,char *limit_str);
	void SummarizeOnePolyPart(CLDSPolyPart *pPolyPart,char *limit_str);
	void SummarizeSuppPart();	//ͳ�Ƹ��ӹ���
	void SummarizePartGroup();	//ͳ���麸��
	bool SummarizePartByModule(char* limit_str,SEGI iSeg);
	void SummarizeBlockRefs();
	void SummarizeOneBlockRef(CBlockReference *pBlockRef);
public:
	ATOM_LIST<SEGI>segIList;			//�κ�����
	ATOM_LIST<CStatModule>moduleset;	//����ģ�ͼ���
	ATOM_LIST<PART> part_recordset;		//������¼����
	ATOM_LIST<CLsRecord> ls_recordset;	//��˨��¼����
	ATOM_LIST<CStatWeldPart> partgroupset;	//�麸������
	ATOM_LIST<CFlRecord> fl_recordset;	//������¼����
	BOOL RunSummarize(CTower* pTower,bool bIncLsSegI=false);	//���в���ͳ��
	BOOL SummarizeFLs(CTower* pTower);							//���з���ͳ��
	void SummarizeOnePart(CLDSPart *pPart,CTower* pTower=NULL);	//���ѡ�й����б�ʱ�������в���ͳ��
	void SortPart(int partno0_guige1_onlyguige2=0,BOOL bDifferMat=TRUE,BOOL bAscending=TRUE);//partno0_guige1_onlyguige2=2�ᰴ������򵫲����ֶκ�
	void SortLs();
	void SortFL();
	PART *GetPartFromPartNo(char *sPartNo);	//��Ҫ����ͨ���麸����PARTREF�������PART
public:
	bool GetStatMatRecordJg(char cMaterial,ATOM_LIST<PARTSPEC_RECORD>& matrecList);
	bool GetStatMatRecordPlate(char cMaterial,ATOM_LIST<PARTSPEC_RECORD>& matrecList);
	bool GetStatMatRecordLs(char* grade,ATOM_LIST<PARTSPEC_RECORD>& matrecList);
	bool GetStatMatRecordNail(char* grade,ATOM_LIST<PARTSPEC_RECORD>& matrecList);
	bool GetStatMatRecordWasher(ATOM_LIST<PARTSPEC_RECORD>& matrecList);

	bool GetSegStatMatRecordPart(SEGI iSeg,ATOM_LIST<SEGPARTNO_RECORD>& matrecList);
	bool GetSegStatMatRecordLs(SEGI iSeg,ATOM_LIST<SEGLS_SPEC_RECORD>& matrecList);
	bool GetSegStatMatRecordNail(SEGI iSeg,ATOM_LIST<SEGLS_SPEC_RECORD>& matrecList);
	bool GetSegStatMatRecordWasher(SEGI iSeg,ATOM_LIST<SEGLS_SPEC_RECORD>& matrecList);
};
#endif
#endif // !defined(AFX_TOWERTYPE_H__5DF4AC0B_CE1E_4592_8EE3_C5E71DDA0BFB__INCLUDED_)
	