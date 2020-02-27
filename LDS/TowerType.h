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
//以下类主要用于存储统材信息
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
//钢管工艺类型
enum TUBE_PROCESS_TYPE
{
	PROCESSTYPE_NONE	= 0,	//无特殊工艺
	PROCESSTYPE_TRANSECT= 1,	//圆柱相贯
	PROCESSTYPE_PLANECUT= 2,	//平面相贯
	PROCESSTYPE_CSLOT	= 3,	//一型开口槽
	PROCESSTYPE_USLOT	= 4,	//U型开口槽
	PROCESSTYPE_TSLOT	= 5,	//T型开口槽
	PROCESSTYPE_XSLOT	= 6,	//十字型开口槽
	PROCESSTYPE_FL		= 7,	//法兰
	PROCESSTYPE_FLD		= 8,	//对焊法兰
	PROCESSTYPE_FLP		= 9,	//平焊法兰
	PROCESSTYPE_FLG		= 10,	//刚性法兰
	PROCESSTYPE_FLR		= 11,	//柔性法兰
};

//钢管加工工艺---相贯/开槽
struct TUBE_PROCESS
{
	TUBE_PROCESS_TYPE type;	//钢管工艺类型
	char guige[MAX_CHAR_GUIGE_16+1];//相贯时主管规格
	double L;				//开口槽长度L或相贯时支管长度
	double A;				//开口槽宽度A或相贯时主管与支管之间的夹角或钢管心线与切割面之间的夹角
	double B;				//T型槽宽度B或U型槽内圆弧半径
	double fNormOffset;		//开口槽法向偏移量
	BOOL IsFL(){return type==PROCESSTYPE_FL||type==PROCESSTYPE_FLD||
					   type==PROCESSTYPE_FLP||type==PROCESSTYPE_FLG||type==TYPE_FLR;}
};

//构件子类型
enum PART_SUB_TYPE
{
	TUBE_MAIN	=1,		//主管
	TUBE_BRANCH,		//支管
	PARAMPLATE_C,		//槽型插板
	PARAMPLATE_U,		//U型插板
	PARAMPLATE_X,		//十字插板
	PARAMPLATE_FL,		//法兰
	PARAMPLATE_WATER,	//遮水板
	PLATE_FL,			//底脚法兰
};
class PART
{
	long nPart;		//一个塔型中具有的当前编号构件数量
	char sPartNo[MAX_CHAR_GUIGE_16+1];		//构件编号
	char sPartNoIncMat[MAX_CHAR_GUIGE_16+2];//带材质的构件编号,通过GetPartNo()函数赋值并返回
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
	//获取文本形式的样杆表数据。其中：!:M12;*:M16;#:M20;$:M24;?:螺栓孔直径非常规请核实后再冲孔
	int GetLsStr(char *ls_str);
	//获取特殊工艺备注信息	bIncAngle表示工艺字符串是否包括开合角度数 wht 11-05-23
	int GetProcessNotes(char *notes,BOOL bIncKaiHeAngle=FALSE);
	//获取开口槽类型 nRuleType 开槽类型编号规则 wht 10-10-11
	int GetNotchType(char *type,int nRuleType);
	//螺栓孔字符串
	int GetLsHoleStr(char *hole_str,BOOL bIncUnit=TRUE);
	//是否割相贯线
	BOOL IsTubeTransect();
	//是否需要割开口槽
	BOOL IsTubeNotch();	
	//得到指定孔径的孔数 bSpecifyScope==TRUE; 指定范围内的孔个数
	//					 bSpecifyScope==FALSE; 直径=min_d的孔个数
	int GetLsHoleNum(double min_d,double max_d=-1,BOOL bHoleD=TRUE,BOOL bSpecifyScope=TRUE);
	long GetPartNum(){return nPart;}
	//构件编号
	void SetPartNo(const char *part_no) { strncpy(sPartNo,part_no,16);}
	char* GetPartNo(int nMatCharPosType=0,char cMatSeparator=0);
public:
	SEGI iSeg;			//段号
	char part_type;		//0.节点 1.角钢 2.螺栓 3.连板 4.钢管 5.扁钢 6.槽钢 100.其它
	short sub_type;		//  
	char cMaterial;		//材质简化字符，如：'S','H','G'
	char cQualityLevel;	//材料的质量等级，如：'B','C','D'
	double wide;		//构件宽度参数
	double thick;		//构件厚度参数
	double y_wing_wide;	//角钢Y肢宽度
	CXhChar16 sSpec;	//规格
	double length;		//构件长度参数
	double fPieceMinWeight;	//单件重(kg)（板为最小包容矩形重量）
	double fPieceNetWeight;	//单件重(kg)（板为净截面重量）
	double min_area;		//连接板最小包容矩形面积(mm2)
	double real_area;		//净连板面积(mm2)
	double wing_angle;	//角钢两肢夹角
	double fRollHeight;	//卷边高度
	BOOL bZhiWan;		//是否需要制弯
	BOOL bWeldPart;		//是否需要焊接
	BOOL bNeedFillet;	//需要坡口
	BOOL bCutAngle;		//是否切角
	BOOL bCutBer;		//是否需要铲背
	BOOL bCutRoot;		//是否需要清根
	BOOL bKaiJiao;		//是否需要开角
	BOOL bHeJiao;		//是否需要合角
	BOOL bHasFootNail;	//是否含脚钉
	BOOL bRollEdge;		//是否卷边
	BOOL bDianBan;		//是否为垫板
	//BOOL bPushFlat;		//是否压扁
	BOOL bLegPart;		//是否为腿部构件
	BOOL bHasNodePlate;	//钢管中间连有节点板		//wht 05-31
	BOOL bHasBranchTube;//钢管中间连有相贯的支管
	int nPushFlat;		//0无压扁 1一端压扁 2两端压扁 3中间压扁
	// --------- 切肢 -----------
	//标识所切角钢肢,0表示无切肢,1表示X肢,2表示Y肢
	int cut_wing[2];
	/*cut_wing_para[0]cut_wing_para[1]表示终端切角
	...[][0]表示角钢楞线切取长度
	...[][1]表示另一肢切取宽度
	...[][2]表示肢棱线切取长度
	*/
	int cut_wing_para[2][3];
    // --------- 切角 -----------
	/*
	cut_angle[0]表示起点X肢
	cut_angle[1]表示起点Y肢
	cut_angle[2]表示终点X肢
	cut_angle[3]表示终点Y肢
	...[][0]表示平行楞线方向切取量  ...[][1]表示垂直楞线方向切取量
	*/
	int cut_angle[4][2];
	short nM16Ls;	//M16螺栓孔数
	short nM20Ls;	//M20螺栓孔数
	short nM24Ls;	//M24螺栓孔数
	short nMXLs;	//其它直径螺栓孔数
	short nMSumLs;	//螺栓孔总数
	short nEdgeNum;	//钢板边数
	double fPerimeter;	//钢板周长
	double fWeldEdgeLen;//焊接边长度
	PART_LS *ls_arr;
	char sNotes[51];	//构件备注说明
	TUBE_PROCESS startProcess;	//钢管始端加工工艺	wht 09-08-31
	TUBE_PROCESS endProcess;	//钢管终端加工工艺
};
struct PARTREF
{
	//long idPart;	//映射到PART,与PART里的id相对应
	char sPartNo[MAX_CHAR_GUIGE_16+1+1];//映射到PART,与PART里的sPartNo相对应
	short num;		 //此构件在某一组焊件中的数量
	PARTREF(){num=0;}
};
//主要用于塔中若干构件需要焊接组合在一起的情况.
class CStatWeldPart
{
	ATOM_LIST<PARTREF> *partref_arr;
public:
	SEGI iSeg;		//
	short num;		//此焊接组合件在单基塔中的个数
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
// 统计用加工呼高模型类
class CStatModule
{
public:
	CHashList<SEGI> segNoHashList;//呼高内包含的段号哈希表
	long iNo;		//在塔型内部唯一
	char description[MAX_TEMP_CHAR_50+1];	//描述呼高的字符串
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
	long nPart;				//单基总数量（包括各种螺栓）
public:
	short typels0_pad1;		//0表示螺栓1表示垫圈
	long iModuleNo;
	char sModule[MAX_TEMP_CHAR_50+1];//模型简单描述
	char grade[8];			//螺栓级别
	SEGI iSeg;				//螺栓所从属的段号
	long hFamily;			//螺栓规格系列标识
	long d;
	long L,L0_uplim,L0_downlim;	//螺栓有效长，及通过厚度
	long no_thread_len;			//无扣长
	//double unit_weight;	//单螺栓重
	double sum_weight;		//总螺栓重
	long nTwoCap;			//其中含有双帽螺栓的数量
	long nBurglarproof;		//其中含有防盗螺栓的数量
	long nSpringPad;		//其中含有带弹簧垫圈的数量
	BOOL bFootNail;	//脚钉
public:
	//构件默认值修改为0，否则统计特殊类型螺栓时个数可能少1 wht 12-08-24
	CLsRecord(){memset(this,0,sizeof(CLsRecord));nPart=0;}
	~CLsRecord(){;}
	long GetLsPadCount();			//获取螺栓垫圈数量 
	long GetCommonLsCount();		//获取普通螺栓数量
	long GetTwoCapLsCount();		//获取双帽螺栓数量
	long GetCommonNailCount();		//获取普通脚钉数量
	long GetBurglarproofLsCount();	//获取防盗螺栓数量
	long GetBurglarproofNailCount();//获取防盗脚钉数量
	double GetUnitWeight(BYTE ciLsFlag=0);	//0.普通|1.防松|2.防盗
	int AddPart(BITFLAG flagBolt,int add_num=1);
	long GetPartNum(){return nPart;}
};


struct SEGPARTNO_RECORD
{
	int	   N;	 
	double length;			//构件长度	(mm)
	double unit_weight;	//单构件重量(kg)
	char sPartNo[MAX_CHAR_GUIGE_16+1];		//构件编号
	char guige[MAX_CHAR_GUIGE_16+1];		//规格
	char sNotes[MAX_TEMP_CHAR_50+1];		//备注
};
struct SEGLS_SPEC_RECORD
{
	int	   N;	 
	//double unit_weight;	//单构件重量(kg)
	int d,L;
	char grade[10];
	char guige[MAX_CHAR_GUIGE_16+1];//规格
	char sNotes[MAX_TEMP_CHAR_50+1];//备注
};
struct PARTSPEC_RECORD
{
	char guige[MAX_CHAR_GUIGE_16+1];	//规格
	ATOM_LIST<double> bodyWeightList;	//加工模型重量表
	ATOM_LIST<double> segWeightList;	//加工段号重量表
};
class CFlRecord
{
	long nPart;			//单基总数量
	CXhChar16 sPartNo;	//法兰编号
public:
	SEGI iSeg;			//段号
	double thick;		//厚度
	double D;			//法兰外径
	double d;			//法兰内径
	double K;			//布孔直径
	int nLsNum;			//螺栓数量
	int ls_d;			//螺栓直径
	char sNotes[50];	//法兰备注说明
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
	CTower *m_pTower;	//记录当前正在统计的塔型
	//为统计部件引用和统计塔型数据代码共用，记录IModel指针，仅在设计呼高时使用m_pTower
	//统计构件时统一使用m_pModel，故在统计部件内构件时仅需要切换m_pModel为当前部件指针，
	//并设置部件内构件段号及配材即可，统计完部件后再将m_pModel切换为m_pTower wht 12-09-13
	IModel *m_pModel;	
public:
	static int CustomerSerial;	//客户定制标识序号
	bool m_bStatLsBySegI;
	char m_sTypeName[MAX_TEMP_CHAR_50+1];	//塔型名称描述
	char m_sTypeCode[MAX_TEMP_CHAR_50+1];	//塔型代号
	char description[MAX_TEMP_CHAR_50+1];	//除名字以外的塔型描述
	char voltGrade[MAX_TEMP_CHAR_50+1];		//电压等级(塔型的分类依据之一），每塔型只能有一个电压等级
	char viceType[MAX_TEMP_CHAR_50+1];		//区分：转角，终端和直线塔，每塔型只属于三种之一
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
	void SummarizeSuppPart();	//统计附加构件
	void SummarizePartGroup();	//统计组焊件
	bool SummarizePartByModule(char* limit_str,SEGI iSeg);
	void SummarizeBlockRefs();
	void SummarizeOneBlockRef(CBlockReference *pBlockRef);
public:
	ATOM_LIST<SEGI>segIList;			//段号链表
	ATOM_LIST<CStatModule>moduleset;	//构件模型集合
	ATOM_LIST<PART> part_recordset;		//构件记录集合
	ATOM_LIST<CLsRecord> ls_recordset;	//螺栓记录集合
	ATOM_LIST<CStatWeldPart> partgroupset;	//组焊件集合
	ATOM_LIST<CFlRecord> fl_recordset;	//法兰记录集合
	BOOL RunSummarize(CTower* pTower,bool bIncLsSegI=false);	//进行材料统计
	BOOL SummarizeFLs(CTower* pTower);							//进行法兰统计
	void SummarizeOnePart(CLDSPart *pPart,CTower* pTower=NULL);	//输出选中构件列表时用来进行材料统计
	void SortPart(int partno0_guige1_onlyguige2=0,BOOL bDifferMat=TRUE,BOOL bAscending=TRUE);//partno0_guige1_onlyguige2=2会按规格排序但不区分段号
	void SortLs();
	void SortFL();
	PART *GetPartFromPartNo(char *sPartNo);	//主要用于通过组焊件的PARTREF查找相关PART
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
	