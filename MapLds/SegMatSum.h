#ifndef __SEG_MAT_SUM_H_
#define __SEG_MAT_SUM_H_
#include "Tower.h"
#include "material.h" 
#include "f_ent_list.h"
#include "TowerBom.h"

class CSegMatPolePlankRecord
{
	int		N;	 
	double	length;		//��������	(mm)
	double	unit_weight;//����������(kg)
	double	sum_weight;	//��������(С������)(kg)
	char sPartNo[MAX_CHAR_GUIGE_16+1];		//�������
	char sPartNoIncMat[MAX_CHAR_GUIGE_16+1];
public:
	/*
		���ڶԱ�׼�����ͷ����������⴦�����·ֶι�����ϸ���з�������ʾ���ͳ���
		�����������ʾ�������ͳ��ȣ�����Excel���һ��  wxc-2016.12.20
	*/
	BOOL bStdPart;		//��׼������,�����⴦�� wht 12-03-22
	char guige[24];		//���
	char cMaterial;     //����
	char sNotes[100];	//��ע
	char sLength[50];	//����	Ϊ���ݷ�������
	CSegMatPolePlankRecord();
	~CSegMatPolePlankRecord(){;}
	int	   GetPartN();		//��ù�����
	int    SetPartN(int n);
	double GetLength();
	double SetLength(double L);//mmΪ��λ
	double AddPart(int n=1);
	double SetUnitWeight(double unit_wei);
	double GetUnitWeight();
	double SetSumWeitht(double sum_weight);	
	double GetSumWeight();
	void SetPartNo(char* part_no){strcpy(sPartNo,part_no);}
	char* GetPartNo(int nMatCharPosType=0);
};
inline CSegMatPolePlankRecord::CSegMatPolePlankRecord()
{
	memset((void*)this,0,sizeof(CSegMatPolePlankRecord));
}
class CSegMatLsRecord
{
	int	N;				//��˨����
	int nTwoCap;		//���к���˫ñ��˨������
	int nBurglarproof;	//���к��з�����˨������
	int nSpringPad;		//���к��д����ɵ�Ȧ������
	double unit_weight;	//����������(kg)
	double sum_weight;	//��������(С������)(kg)
	char guige[24];		//���
public:
	long hFamily;			//��˨���ϵ�б�ʶ
	char sNotes[100];	//��ע
	CSegMatLsRecord();
	~CSegMatLsRecord(){;}
	void   SetGuiGe(char *sGuiGe);
	void   GetGrade(char *sGrade);
	void   GetGuiGe(char *sGuiGe,bool bIncGrade=true);
	int    SetPartN(int n);
	int	   GetPartN();		//��ù�����
	double AddPart(BITFLAG flagBolt,int n=1);
	double SetSumWeight(double Sum_wei);
	double GetSumWeight();
	long GetCommonLsCount() {return N-nTwoCap-nBurglarproof-nSpringPad;}//��ȡ��ͨ��˨����
	long GetTwoCapLsCount() {return nTwoCap;}				//��ȡ˫ñ��˨����
	long GetBurglarproofLsCount() {return nBurglarproof;}	//��ȡ������˨����
	long GetSpringPadLsCount() {return nSpringPad;};		//��ȡ�����ɵ�Ȧ����˨����
};
inline CSegMatLsRecord::CSegMatLsRecord()
{
	memset((void*)this,0,sizeof(CSegMatLsRecord));
}
class CSegMatPadRecord
{
	int	   N;	 
	double unit_weight;	//������Ȧ������
	double sum_weight;	//��������(С������)(kg)
	char guige[24];		//���
public:
	char sNotes[100];	//��ע
	CSegMatPadRecord();
	~CSegMatPadRecord(){;}
	//thick��ʾ��Ȧ�ĺ��, d��ʾ��Ȧ��������˨��ֱ��(���)
	void   SetGuiGe(int thick,int d);
	void   SetGuiGe(char *sGuiGe);
	void   SetPartN(int n);
	void   SetSumWeight(double sum_wei);
	double AddPart(int n=1);
	int	   GetPartN(){return N;}		//��ù�����
	void   GetGuiGe(char *sGuiGe){strcpy(sGuiGe,guige);}
	double GetSumWeight(){return sum_weight;}
};
inline CSegMatPadRecord::CSegMatPadRecord()
{
	memset((void*)this,0,sizeof(CSegMatPadRecord));
}
class CSegMaterialSummator
{
	CStringArray sPolePlankNo;
	CStringArray sPolePlankGuiGe;
	CByteArray cPolePlankMat;
	SEGI m_iSeg;	//�ֶκ�
	int m_nPolePlank;	//�Ǹָֹ����Ӱ�����
	CSegMatPolePlankRecord* m_pPolePlankRecArr;	//�Ǹְָֹܸ��¼

	//CSegMatLsRecord* m_pTwoCapLsRecArr;	//˫ñ��˨��¼
	//CStringArray sTwoCapLsGuiGe;		//˫ñ��˨���� wht 11-04-25
	CSegMatLsRecord* m_pLsRecArr;	//��˨��¼
	CStringArray sLsGuiGe;			//��˨����
	CSegMatLsRecord* m_pNailRecArr;	//��˨��Ȧ��¼
	CStringArray sPadGuiGe;			//��Ȧ����
	CSegMatPadRecord* m_pPadRecArr;	//��˨�Ŷ���¼
	CStringArray sNailGuiGe;		//�Ŷ�����
private:
	void RunAnalyze();			//���в���ͳ��ǰ��Ԥ����
	void AnalyzeSuppPart();		//�������ӹ���
	void AnalyzeBlockRefs();	//������������
	void AnalyzeParts(IModel *pModel,BOOL bIgnoreSegI=FALSE);	//��������
	//void AnalyzeFootPlank();
	//void AnalyzeNodeAttachPart();
	//void AnalyzeLsList(CLsRefList *pLsRefList);
	//void SummarizeNodeAttachPart();
	BOOL CreateRecordSet(int nJgPlank,int nLs,int nNail,int nPad);//,int nTwoCapLs);
	void SummarizeBolt(IModel *pModel,BOOL bIgnoreSegI=FALSE,CBlockModel *pEmbededBlock=NULL);	//ͳ����˨
	void SummarizeSuppPart();		//ͳ�Ƹ��ӹ���
	void SummarizeParts(IModel *pModel,BOOL bIgnoreSegI=FALSE,CBlockModel *pEmbededBlock=NULL);	//ͳ�ƹ���
	void SummarizeBlockRefs();		//ͳ�Ʋ�������
public:	
	static bool EXPORT_BY_ENGLISH;
	int FindPolePlankNoIndex(const char *sPartNo);
	int FindLsGuiGeIndex(char *spec);
	int FindNailGuiGeIndex(char *spec);
	int FindPadGuiGeIndex(char *spec);
	//int FindTwoCapLsGuiGeIndex(char *spec);
	CSegMaterialSummator();
	virtual ~CSegMaterialSummator();
	BOOL RunSummarize(SEGI iSegNo);	//���в���ͳ��
	void RunSummarizeFromBOM(CTowerBOM &bom, SEGI iSegNo, bool bStatPart, bool bStatBolt);	//���в���ͳ��
	int GetJgPlankRecCount();
	CSegMatPolePlankRecord* GetJgPlankRecAt(int iRow);
	
	int GetLsRecCount();		//�����˨��¼��
	int GetNailRecCount();		//��ýŶ���¼��
	//int GetTwoCapLsRecCount();	//���˫ñ��˨��¼��
	int GetPadRecCount();		//��õ�Ȧ��¼��
	CSegMatLsRecord* GetLsRecAt(int iRow);
	CSegMatLsRecord* GetNailRecAt(int iRow);
	//CSegMatLsRecord* GetTwoCapLsRecAt(int iRow);
	CSegMatPadRecord* GetPadRecAt(int iRow);
	int GetCurSegI();

};
#endif