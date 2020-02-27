// MaterialTbl.h: interface for the CMaterialTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIALTBL_H__5DF4AC0B_CE1E_4592_8EE3_C5E71DDA0BFB__INCLUDED_)
#define AFX_MATERIALTBL_H__5DF4AC0B_CE1E_4592_8EE3_C5E71DDA0BFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "f_ent_list.h"
#include "Tower.h"
typedef struct tagBRIEF_PART_RECORD
{
	long id;	//��CPartRecord�е�id���Ӧ
	int  num;	//��ǰ������ĳһģ���е�����
}BRIEF_PART_RECORD;
class CModulePartSet
{
	ATOM_LIST<BRIEF_PART_RECORD> *pPartList;
public:
	long iNo;
	char description[50];	//ģ�ͼ�Ҫ����
	CModulePartSet(){
		iNo=0;
		pPartList=new ATOM_LIST<BRIEF_PART_RECORD>;
		strcpy(description,"");
	}
	~CModulePartSet(){pPartList->Empty();delete pPartList;}
	ATOM_LIST<BRIEF_PART_RECORD>* GetPartList(){return pPartList;}
};
class CPartRecord
{
public:
	char sPartNo[16];		//�������
	char part_type;			//1.�Ǹ�2.��˨3.����
	long id;				//��ͬһ�����ļ���Ψһ������ű�ʶ����
	int nPart;				//��������
	int iSeg;				//�����κ�
	char limit_str[301];	//��������ģ�ͺ�
	char cMaterial;			//��Ʋ���
	char sSpec[24];		//��ƹ��
	double wide;			//������(mm)
	double thick;			//������(mm)
	double length;			//������(mm)
	double fPieceMinWeight;	//������(kg)����Ϊ��С���ݾ���������
	double fPieceNetWeight;	//������(kg)����Ϊ������������
	double net_area;		//���������(mm2)
	double wing_angle;		//��֫�н�
	int nLsHole[2][4];		//��˨����
	BOOL bHuoQu;			//�Ƿ����
	BOOL bCutAngle;			//�Ƿ��нǣ�֫��
	BOOL bCutBer;			//�Ƿ����
	BOOL bCutRoot;			//�Ƿ����
	BOOL bKaiHeJiao;		//�Ƿ񿪺Ͻ�
	BOOL bPushFlat;			//�Ƿ�ѹ��
	BOOL bWeld;				//�Ƿ񺸽�
	char sNotes[50];		//��ע��Ϣ
	long nM16Ls;			//M16��˨����
	long nM20Ls;			//M20��˨����
	long nM24Ls;			//M24��˨����
	long nMXLs;				//������M��˨����

public:
	CPartRecord(){memset(this,0,sizeof(CPartRecord));nPart=1;}
	~CPartRecord(){;}
	int AddPart(int add_num=1){nPart+=add_num;return nPart;}
};
class CLsRecord
{
public:
	short typels0_pad1;		//0��ʾ��˨1��ʾ��Ȧ
	long iModuleNo;
	char sModule[50];		//ģ�ͼ�����
	char grade[8];			//��˨����
	long iSeg;				//��˨�������Ķκ�
	long d;
	long nPart;				//����������������������˨��
	long L,L0_uplim,L0_downlim;	//��˨��Ч������ͨ�����
	double unit_weight,sum_weight;	//��ͨ����˨�أ�����˨��
	long nTwoCap;			//���к���˫ñ��˨������
	long nBurglarproof;		//���к��з�����˨������
	long nSpringPad;		//���к��д����ɵ�Ȧ������
	BOOL bFootNail;	//�Ŷ�
public:
	CLsRecord(){memset(this,0,sizeof(CLsRecord));nPart=1;}
	~CLsRecord(){;}
	long GetLsPadCounta();			//��ȡ��˨��Ȧ���� 
	long GetCommonLsCount();		//��ȡ��ͨ��˨����
	long GetTwoCapLsCount();		//��ȡ˫ñ��˨����
	long GetCommonNailCount();		//��ȡ��ͨ�Ŷ�����
	long GetBurglarproofLsCount();	//��ȡ������˨����
	long GetBurglarproofNailCount();//��ȡ�����Ŷ�����
	int AddPart(BOOL bTwoCap=FALSE,BOOL bSpringPad=FALSE,BOOL bBurglarproof=FALSE,int add_num=1);
};
class CMaterialTbl  
{
	BOOL m_bIncLsSegI;
	char limit_str[301];
	void SummarizeNodeAttachPart();
	void SummarizeJg();
	void SummarizeOneJg(CTMAAngle *pJg,int iSeg,char *limit_str);
	void SummarizeOnePlank(CTMAPlank *pPlank,int iSeg,char *limit_str,BOOL bFinal=FALSE);
	void SummarizeSuppPart();	//ͳ�Ƹ��ӹ���
	void SummarizeLsList(CLsRefList *pLsRefList,int iSeg,char* limit_str);
public:
	ATOM_LIST<CModulePartSet>moduleset;		//����ģ�ͼ���
	ATOM_LIST<CPartRecord> part_recordset;	//������¼����
	ATOM_LIST<CLsRecord> ls_recordset;		//��˨��¼����
	CMaterialTbl();
	~CMaterialTbl();
	BOOL RunSummarize(BOOL bIncLsSegI=FALSE);	//���в���ͳ��
	void SortPart(int partno0_guige1=0,BOOL bDifferMat=TRUE,BOOL bAscending=TRUE);
	void SortLs();
private:
	void SummarizePartByModule(char* limit_str,long id,UINT uStatCoef);
};

#endif // !defined(AFX_MATERIALTBL_H__5DF4AC0B_CE1E_4592_8EE3_C5E71DDA0BFB__INCLUDED_)
