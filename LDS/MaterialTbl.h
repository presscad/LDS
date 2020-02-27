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
	long id;	//与CPartRecord中的id相对应
	int  num;	//当前构件在某一模型中的数量
}BRIEF_PART_RECORD;
class CModulePartSet
{
	ATOM_LIST<BRIEF_PART_RECORD> *pPartList;
public:
	long iNo;
	char description[50];	//模型简要描述
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
	char sPartNo[16];		//构件编号
	char part_type;			//1.角钢2.螺栓3.连板
	long id;				//在同一塔型文件内唯一代替件号标识构件
	int nPart;				//单基件数
	int iSeg;				//从属段号
	char limit_str[301];	//所从属的模型号
	char cMaterial;			//设计材质
	char sSpec[24];		//设计规格
	double wide;			//构件宽(mm)
	double thick;			//构件厚(mm)
	double length;			//构件长(mm)
	double fPieceMinWeight;	//单件重(kg)（板为最小包容矩形重量）
	double fPieceNetWeight;	//单件重(kg)（板为净截面重量）
	double net_area;		//净连板面积(mm2)
	double wing_angle;		//两肢夹角
	int nLsHole[2][4];		//螺栓孔数
	BOOL bHuoQu;			//是否火曲
	BOOL bCutAngle;			//是否切角（肢）
	BOOL bCutBer;			//是否铲背
	BOOL bCutRoot;			//是否清根
	BOOL bKaiHeJiao;		//是否开合角
	BOOL bPushFlat;			//是否压扁
	BOOL bWeld;				//是否焊接
	char sNotes[50];		//备注信息
	long nM16Ls;			//M16螺栓孔数
	long nM20Ls;			//M20螺栓孔数
	long nM24Ls;			//M24螺栓孔数
	long nMXLs;				//其余规格M螺栓孔数

public:
	CPartRecord(){memset(this,0,sizeof(CPartRecord));nPart=1;}
	~CPartRecord(){;}
	int AddPart(int add_num=1){nPart+=add_num;return nPart;}
};
class CLsRecord
{
public:
	short typels0_pad1;		//0表示螺栓1表示垫圈
	long iModuleNo;
	char sModule[50];		//模型简单描述
	char grade[8];			//螺栓级别
	long iSeg;				//螺栓所从属的段号
	long d;
	long nPart;				//单基总数量（包括各种螺栓）
	long L,L0_uplim,L0_downlim;	//螺栓有效长，及通过厚度
	double unit_weight,sum_weight;	//普通单螺栓重，总螺栓重
	long nTwoCap;			//其中含有双帽螺栓的数量
	long nBurglarproof;		//其中含有防盗螺栓的数量
	long nSpringPad;		//其中含有带弹簧垫圈的数量
	BOOL bFootNail;	//脚钉
public:
	CLsRecord(){memset(this,0,sizeof(CLsRecord));nPart=1;}
	~CLsRecord(){;}
	long GetLsPadCounta();			//获取螺栓垫圈数量 
	long GetCommonLsCount();		//获取普通螺栓数量
	long GetTwoCapLsCount();		//获取双帽螺栓数量
	long GetCommonNailCount();		//获取普通脚钉数量
	long GetBurglarproofLsCount();	//获取防盗螺栓数量
	long GetBurglarproofNailCount();//获取防盗脚钉数量
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
	void SummarizeSuppPart();	//统计附加构件
	void SummarizeLsList(CLsRefList *pLsRefList,int iSeg,char* limit_str);
public:
	ATOM_LIST<CModulePartSet>moduleset;		//构件模型集合
	ATOM_LIST<CPartRecord> part_recordset;	//构件记录集合
	ATOM_LIST<CLsRecord> ls_recordset;		//螺栓记录集合
	CMaterialTbl();
	~CMaterialTbl();
	BOOL RunSummarize(BOOL bIncLsSegI=FALSE);	//进行材料统计
	void SortPart(int partno0_guige1=0,BOOL bDifferMat=TRUE,BOOL bAscending=TRUE);
	void SortLs();
private:
	void SummarizePartByModule(char* limit_str,long id,UINT uStatCoef);
};

#endif // !defined(AFX_MATERIALTBL_H__5DF4AC0B_CE1E_4592_8EE3_C5E71DDA0BFB__INCLUDED_)
