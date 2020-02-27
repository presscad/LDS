#ifndef __SEG_MAT_SUM_H_
#define __SEG_MAT_SUM_H_
#include "Tower.h"
#include "material.h" 
#include "f_ent_list.h"
#include "TowerBom.h"

class CSegMatPolePlankRecord
{
	int		N;	 
	double	length;		//构件长度	(mm)
	double	unit_weight;//单构件重量(kg)
	double	sum_weight;	//构件总重(小计重量)(kg)
	char sPartNo[MAX_CHAR_GUIGE_16+1];		//构件编号
	char sPartNoIncMat[MAX_CHAR_GUIGE_16+1];
public:
	/*
		由于对标准构件和法兰进行特殊处理，导致分段构件明细表中法兰不显示规格和长度
		武晓提出需显示法兰规格和长度，并与Excel表格一致  wxc-2016.12.20
	*/
	BOOL bStdPart;		//标准件或法兰,需特殊处理 wht 12-03-22
	char guige[24];		//规格
	char cMaterial;     //材质
	char sNotes[100];	//备注
	char sLength[50];	//长度	为兼容法兰长度
	CSegMatPolePlankRecord();
	~CSegMatPolePlankRecord(){;}
	int	   GetPartN();		//获得构件数
	int    SetPartN(int n);
	double GetLength();
	double SetLength(double L);//mm为单位
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
	int	N;				//螺栓总数
	int nTwoCap;		//其中含有双帽螺栓的数量
	int nBurglarproof;	//其中含有防盗螺栓的数量
	int nSpringPad;		//其中含有带弹簧垫圈的数量
	double unit_weight;	//单构件重量(kg)
	double sum_weight;	//构件总重(小计重量)(kg)
	char guige[24];		//规格
public:
	long hFamily;			//螺栓规格系列标识
	char sNotes[100];	//备注
	CSegMatLsRecord();
	~CSegMatLsRecord(){;}
	void   SetGuiGe(char *sGuiGe);
	void   GetGrade(char *sGrade);
	void   GetGuiGe(char *sGuiGe,bool bIncGrade=true);
	int    SetPartN(int n);
	int	   GetPartN();		//获得构件数
	double AddPart(BITFLAG flagBolt,int n=1);
	double SetSumWeight(double Sum_wei);
	double GetSumWeight();
	long GetCommonLsCount() {return N-nTwoCap-nBurglarproof-nSpringPad;}//获取普通螺栓数量
	long GetTwoCapLsCount() {return nTwoCap;}				//获取双帽螺栓数量
	long GetBurglarproofLsCount() {return nBurglarproof;}	//获取防盗螺栓数量
	long GetSpringPadLsCount() {return nSpringPad;};		//获取带弹簧垫圈的螺栓数量
};
inline CSegMatLsRecord::CSegMatLsRecord()
{
	memset((void*)this,0,sizeof(CSegMatLsRecord));
}
class CSegMatPadRecord
{
	int	   N;	 
	double unit_weight;	//单个垫圈的重量
	double sum_weight;	//构件总重(小计重量)(kg)
	char guige[24];		//规格
public:
	char sNotes[100];	//备注
	CSegMatPadRecord();
	~CSegMatPadRecord(){;}
	//thick表示垫圈的厚度, d表示垫圈所附属螺栓的直径(规格)
	void   SetGuiGe(int thick,int d);
	void   SetGuiGe(char *sGuiGe);
	void   SetPartN(int n);
	void   SetSumWeight(double sum_wei);
	double AddPart(int n=1);
	int	   GetPartN(){return N;}		//获得构件数
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
	SEGI m_iSeg;	//分段号
	int m_nPolePlank;	//角钢钢管连接板规格数
	CSegMatPolePlankRecord* m_pPolePlankRecArr;	//角钢钢管钢板记录

	//CSegMatLsRecord* m_pTwoCapLsRecArr;	//双帽螺栓记录
	//CStringArray sTwoCapLsGuiGe;		//双帽螺栓规格库 wht 11-04-25
	CSegMatLsRecord* m_pLsRecArr;	//螺栓记录
	CStringArray sLsGuiGe;			//螺栓规格库
	CSegMatLsRecord* m_pNailRecArr;	//螺栓垫圈记录
	CStringArray sPadGuiGe;			//垫圈规格库
	CSegMatPadRecord* m_pPadRecArr;	//螺栓脚钉记录
	CStringArray sNailGuiGe;		//脚钉规格库
private:
	void RunAnalyze();			//进行材料统计前的预分析
	void AnalyzeSuppPart();		//分析附加构件
	void AnalyzeBlockRefs();	//分析部件引用
	void AnalyzeParts(IModel *pModel,BOOL bIgnoreSegI=FALSE);	//分析构件
	//void AnalyzeFootPlank();
	//void AnalyzeNodeAttachPart();
	//void AnalyzeLsList(CLsRefList *pLsRefList);
	//void SummarizeNodeAttachPart();
	BOOL CreateRecordSet(int nJgPlank,int nLs,int nNail,int nPad);//,int nTwoCapLs);
	void SummarizeBolt(IModel *pModel,BOOL bIgnoreSegI=FALSE,CBlockModel *pEmbededBlock=NULL);	//统计螺栓
	void SummarizeSuppPart();		//统计附加构件
	void SummarizeParts(IModel *pModel,BOOL bIgnoreSegI=FALSE,CBlockModel *pEmbededBlock=NULL);	//统计构件
	void SummarizeBlockRefs();		//统计部件引用
public:	
	static bool EXPORT_BY_ENGLISH;
	int FindPolePlankNoIndex(const char *sPartNo);
	int FindLsGuiGeIndex(char *spec);
	int FindNailGuiGeIndex(char *spec);
	int FindPadGuiGeIndex(char *spec);
	//int FindTwoCapLsGuiGeIndex(char *spec);
	CSegMaterialSummator();
	virtual ~CSegMaterialSummator();
	BOOL RunSummarize(SEGI iSegNo);	//进行材料统计
	void RunSummarizeFromBOM(CTowerBOM &bom, SEGI iSegNo, bool bStatPart, bool bStatBolt);	//进行材料统计
	int GetJgPlankRecCount();
	CSegMatPolePlankRecord* GetJgPlankRecAt(int iRow);
	
	int GetLsRecCount();		//获得螺栓记录数
	int GetNailRecCount();		//获得脚钉记录数
	//int GetTwoCapLsRecCount();	//获得双帽螺栓记录数
	int GetPadRecCount();		//获得垫圈记录数
	CSegMatLsRecord* GetLsRecAt(int iRow);
	CSegMatLsRecord* GetNailRecAt(int iRow);
	//CSegMatLsRecord* GetTwoCapLsRecAt(int iRow);
	CSegMatPadRecord* GetPadRecAt(int iRow);
	int GetCurSegI();

};
#endif