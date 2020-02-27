// TtaDataFile.h: interface for the CTtaDataFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TTADATAFILE_H__062EEA51_A1F6_48A9_ACBE_C091B018A0DC__INCLUDED_)
#define AFX_TTADATAFILE_H__062EEA51_A1F6_48A9_ACBE_C091B018A0DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __PART_DESIGN_INC_
#include "ArrayList.h"

class CTtaDataFile;
typedef struct TTA_NODE
{
	CLDSNode *pNode;
	TTA_NODE *pAttachStart,*pAttachEnd;
	long pointI;		//TTA中节点号
	long J1,J2,J3,J4,J5;
	//0.无任何依赖节点;1.指定X坐标分量点;2.指定Y坐标分量点;3.指定Z坐标分量点;4.比例等分点;5.交叉点;
	//河南院TANS程序新扩展点类型映射:
	//6.沿杆件与某节点等坐标分量的节点;7.预留偏移点;8.基准面上XY坐标值不变点;9.基准面上YZ坐标值不变点;
	//10.基准面上XZ坐标值不变点;12.原始点对称;13.(与参照点)等坐标节点
	long posType;
	char ciPosViceType;	//节点位计算子类型，'X','Y','Z'
	char cMirTransType;	//基准面对称参照类型，'X','Y','Z'
	int IS;				//0.无对称1.左右Y轴对称;2.前后X轴对称;3.对角Z轴对称;4.四度对称(X/Y/Z对称)
	f3dPoint pos;
	union{
		double coefScale;	//比例等分点的比例值系数
		double offsetDist;	//偏移距离（一般沿ciPosViceType指定坐标轴偏移一定距离）
	};
	//long hits;			//连接受力材单元数量
	//以下数据用于向LDS转换用
	CFGWORD cfgword;
	char cPosLayer;
	bool bBraceNode;
	bool bValidateCfgWord;	//是否已验证cfgword的配材号为最终有效配材号（包括cPosLayer)
	bool bFoundationNode;
private:
	bool _blInitPosCalParams;//是否已初始化节点的坐标定位参数值
	BYTE m_cbPosUpdated;
	BYTE m_cnVisitHits;
	char m_ciMirFromType;	//自pMirFromNode的对称生成类型,'X','Y','Z',0表示为原始节点
	TTA_NODE *_pMirFromNode;	//对称生成节点的原始定义关联对称节点，为NULL表示本身为原始定义节点
public:
	bool set_blInitPosCalParams(bool blValue);
	bool get_blInitPosCalParams();
	__declspec(property(put=set_blInitPosCalParams,get=get_blInitPosCalParams)) bool blInitPosCalParams;
	TTA_NODE* get_pMirFromNode();
	__declspec(property(get=get_pMirFromNode)) TTA_NODE* pMirFromNode;
	char get_ciMirFromType();
	__declspec(property(get=get_ciMirFromType)) char ciMirFromType;
	WORD SetVisitHits(BYTE cnVisitHits=1){return m_cnVisitHits=cnVisitHits;}
	BYTE get_cbPosUpdated() { return m_cbPosUpdated; }
	__declspec(property(get=get_cbPosUpdated)) BYTE cbPosUpdated;	//获取节点当前的坐标更新状态
	BYTE IsPosUpdated(BYTE cbCoordMaskFlag=0x07){return (m_cbPosUpdated&cbCoordMaskFlag)>0;}
	BYTE SetPosUpdateState(BYTE cbCoordUpdateFlag=0x07){return m_cbPosUpdated=cbCoordUpdateFlag;}
	void ClearVisitHits(){m_cbPosUpdated=0x07;m_cnVisitHits=0;}
	bool UpdatePosition(CTtaDataFile* pTtaModel=NULL);
	void MirBasicPropFrom(TTA_NODE* pSrcNode,int mir_x1y2z3);
	bool UpdateBasicPropFromParentNode();
	void CopyFrom(TTA_NODE* pSrcNode);
	TTA_NODE(TTA_NODE* pSrcNode=NULL);
}*TTA_NODEPTR;
struct TTA_ELEM
{
	TTA_NODE *pStart,*pEnd;	//单元起始点和终止点
	int N;	//±N  正负符号表示材质
	int IS;
	int TYPE;
	int M;
	int L1,L2;
	int SMC;
	char cMaterial;	//'S':Q235;'H':Q345;'G':Q390;'P':Q420;'T':Q460;
	int iStatMatNo;	//LDS统材号
	int iSizeOrder;
	//bool bTubeRod;
	double boltgrade;
	int boltd,boltN;
	double dfHoleOutCoef;	//指定减孔系数，0表示未特殊指定
	SIZESPEC size;
	bool bReadSizeSpec;
	TTA_ELEM(){
		memset(this,0,sizeof(TTA_ELEM));
		cMaterial='S';
		iStatMatNo=0;
		boltd=20;boltN=1;	//不能让螺栓默认为0
	}
};
struct TANS_LINE{	//河南院TANS计算程序
	int idLine;
	int iStartPointI,iEndPointI;
	CLDSLine* pRelaDatumLine;
	GEPOINT posStart,posEnd;
	TANS_LINE(){iStartPointI=iEndPointI=idLine=0;pRelaDatumLine=NULL;}
	void SetKey(DWORD key){idLine=key;}
};
struct TANS_PLANE{
private:
	bool m_bUpdatePlane;
public:
	int idPlane;
	int iPoint1stI,iPoint2ndI,iPoint3rdI;
	CLDSPlane* pRelaDatumPlane;
	GEPOINT xPlaneOrg,vPlaneNormal;
	TANS_PLANE(){iPoint1stI=iPoint2ndI=iPoint3rdI=idPlane=0;pRelaDatumPlane=NULL;m_bUpdatePlane=false;}
	bool UpdatePlane(CTtaDataFile* pTtaModel=NULL);
	void SetKey(DWORD key){idPlane=key;}
};
struct TTA_BRACEELEM
{
	TTA_NODE *pStart,*pEnd;	//单元起始点和终止点
	int IS;
	int M;	//道亨辅助材M，意义类似TTA_ELEM::M参数
	//百位数0:只按细长比要求选材1:除按细长比要求外，尚应考虑承担J1端所在杆件内力的N‰
	//2:除按细长比要求外,尚应考虑承担J2端所在杆件内力的N‰
	//拾位及个位数为该输入杆件所代表的一组补助材的根数
	int N;	//N该补材承担的所支撑杆件内力的千分数
	int iStatSerial;	//相当于TTA_ELEM::N统材号
	int percentOfMinRL,percentOfParaRL;
	double boltgrade;
	int boltd,boltN;
	char cSectType;		//cSectType：'R'，钢管;'L',角钢;'T',T型双拼角钢;'D',对角双拼角钢;'X',四拼角钢
	char cBriefMaterial;
	double size_wide,size_thick;	//构件规格类型
	CLDSLinePart* pRod;
	bool bReadSizeSpec;
	TTA_BRACEELEM(){memset(this,0,sizeof(TTA_BRACEELEM));M=20;boltd=16;boltN=1;}
};
struct TTA_ASSISTELEM{
	int index;
	int iKeyNode[4];
	TTA_ASSISTELEM(){memset(this,0,sizeof(TTA_ASSISTELEM));}
};
struct LIST_TTANODE_SET{
	DWORD id;
	CXhSimpleList<TTA_NODEPTR> listTtaNodeSet;
	LIST_TTANODE_SET(DWORD key){id=key;}
};
struct TTA_BODYLEG
{
	int Jmin;	//腿上的最小节点号
	int Fmin;	//腿上的最小基础节点号
	int Fmax;	//腿上的最大基础节点号
	int MAX;	//本体或接腿上的补助材最大端节点号
	int LB;		//个位数为该腿所属的本体即高度号；十位为该腿所包含的不等高腿的高度数
	int Var;	//一般Var=0
	int MylBraceNodeMAX;	//道亨满应力程序最大辅助材端点号
	//以下数据用于向LDS转换用
	int iLeg;				//接腿号
	int niLegN;	//该呼高组包含的长短腿数量
	//与LDS数据模型间的关联属性
	char ciType;	//0.对应TTA本体;1.对应TTA接腿
	DWORD dwBodyFlag;		//对应TTA本体
	CLDSModule *pModule;	//关联呼高模型(本体)
	TTA_BODYLEG();
	DWORD ApplyListTtaNodeSet(CTtaDataFile* pTtaFile);//本体或接腿所关联的节点集合,以便于按TTA规则进行编号
	CXhSimpleList<TTA_NODEPTR>& getListTtaNodeSet();
	_declspec(property(get=getListTtaNodeSet)) CXhSimpleList<TTA_NODEPTR>& listTtaNodeSet;
private:
	CTtaDataFile* m_pTtaFile;
	DWORD listId;
};
struct LDS_BODYLEG
{
	char ciType;	//0.对应TTA本体;1.对应TTA接腿
	DWORD dwBodyFlag;		//对应TTA本体
	CXhSimpleList<TTA_NODEPTR>* pRelaNodeSet;	//本体或接腿所关联的节点集合,以便于按TTA规则进行编号
	CLDSModule* pModule;	//对应TTA接腿
	//long Jmin;	//接腿在TTA文件中的起始节点号
	//long Tmin,Tmax;	//LDS中本体或接腿中最小节点号最小本体(基础)节点号及最大本体(基础)节点号
	//long iLeg;		//LDS中的接腿号
	//long LB;	//个位数为该腿所属的本体即高度号；十位为该腿所包含的不等高腿的高度数
};
struct TTA_WINDSEG
{
	int TYPE;
	int arrPointI[4];
	int NLG;
	int Fa,Fb;	//正侧面风荷载增大系数
	int Fw;		//自重增大系数
	int BetaZ;	//风振系数
	TTA_WINDSEG(){memset(this,0,sizeof(TTA_WINDSEG));}
};
struct TTA_RODSPEC
{
	int iSizeSpecType;
	double size,area,minR,paraR,interR,Wx;
	TTA_RODSPEC() { memset(this,0,sizeof(TTA_RODSPEC)); }
};
union BODYLEG_I{
	DWORD idBodyLeg;
	struct{
		WORD wBody;
		WORD wLegPointI;
	};
	BODYLEG_I(DWORD id){idBodyLeg=id;}
	BODYLEG_I(WORD iBody,WORD iLegPointI){
		wBody=iBody;
		wLegPointI=iLegPointI;
	}
};
struct TTAELEM_DESIGN_RSLT{
	int startPointI,endPointI;
	char cMaterial;
	SIZESPEC size;	//规格
	double length;	//单元长度,mm(TTA中为m)
	double ulength;	//计算长度,mm(TTA中为cm)
	double gyration;//回转半径,mm(TTA中为cm)
	double lamda;	//长细比
	double allowLamda;	//允许长细比
	double stableFactor;//压稳系数
	double maxTension;			//kN
	double maxCompression;		//kN
	int idMaxTensionCase;		//最大拉力工况号
	int idMaxCompressionCase;	//最大压力工况号
	double strengthDeductCoef;	//强度折减系数
	double actualStress;	//计算应力,MPa
	double allowStress;		//设计允许应力,MPa
	double efficiency;	//单元效率
	float boltgrade;	//螺栓级别
	short boltd,boltN;	//螺栓直径及数量
	TTAELEM_DESIGN_RSLT(){
		memset(this,0,sizeof(TTAELEM_DESIGN_RSLT));
	}
	bool InitFromElemRsltStr(char* rsltstr,char ciFileType=0);
	bool InitFromTANSElemRsltStr(char* rsltstr);
	bool InitFromDaoHengAuxElemRsltStr(char* rsltstr,char ciFileType=0);
};
class CTower;
struct GTM_KEYNODE_MAP {
	CLDSNode* pMainModelNode;
	CLDSNode* pModelPanelNode;
	TTA_NODE* pTtaPanelNode;
	TTA_NODE* pLegTopMidNode;	//接腿正侧面辅材面片横材中间节点(
	GTM_KEYNODE_MAP() { memset(this,0,sizeof(GTM_KEYNODE_MAP)); }
};
struct GTM_FZC_FNAME {
	char ciFzcPanelType;	//0.塔身;1.减腿正侧面辅材;2.减腿V面辅材
	char ciPrefixSymbol;	//附加类型符号，对于塔体辅材取值 'B'/'C'表示塔身及横担；对于塔腿取值'A'/'R'表示绝对/虚拟点号
	union{long liPointA,liPointTop;};	//关键节点A(或接腿主材上腿身衔接节点)
	union{long liPointB,liPointBase;};	//关键节点B(或接腿主材上基础节点)
	struct {
		long liHeight,liLegDiff;
		long liVTopPointA,liVTopPointB;	//V面斜材顶部节点（不分正侧面优先顺序）
	}subleg;//呼高(分米),减腿高差(与平腿高差，分米)
	CXhChar500 fname;
	GTM_FZC_FNAME(const char* pcszFileName=NULL,char _ciFzcPanelType=0);
	void Clear();
};
class CTtaDataFile  
{
public:
	static const BYTE FILETYPE_TTA	= 0;
	static const BYTE FILETYPE_MYL	= 1;
	static const BYTE FILETYPE_ST	= 2;	//SmartTower
	static const BYTE FILETYPE_TANS = 3;	//河南院TANS
	static const BYTE FILETYPE_GTOM = 4;	//广东院GTower
private:
	CTower *m_pContextModel;
	BYTE m_ciFileType;	//文件格式
	long KIND,N10,N20,M10,M20,IE,NL,NC,NW,NSEC,WIND;	//WIND是SmartTower中风载计算信息
	long K1,K2,K3,K4,K5,K6,NA,TSM,REN,NGP,VAL2;
	long NBY,NLEG;
	long IW;	//螺栓减孔数设定主材肢宽阈值
	double Wk;	//广东院GTM格式控制参数=弹性地基参数(kN/m^3)，
	long CODE,LOAD_GROUP;	//广东院GTM格式控制参数，规范类别及荷载分组
	double HOLECOEF_ARR[4];
	struct IBOLT{
		float fM12Grade,fM16Grade,fM20Grade,fM24Grade;
		IBOLT(DWORD uiBoltGrade=0);
		void ParseFromDword(DWORD uiBoltGrade);
		float GradeOfMd(short d);
	}ibolt;
	PRESET_ARRAY16<char> xarrSteelSymbols;
	ATOM_LIST<TTA_NODE>listTtaNodes;
	ATOM_LIST<TTA_ELEM>listTtaElems;
	ATOM_LIST<TTA_BRACEELEM>listTtaBraceElems;
	ARRAY_LIST<TTA_RODSPEC>m_arrRodSpec;
	CHashList<TTA_NODE>hashTtaNodesByH;	//以LDS中的节点句柄为键值,将来应完全取代m_arrTtaNode
	ARRAY_LIST<TTA_NODE>m_arrTtaNode;
	ARRAY_LIST<TTA_NODEPTR>m_arrTtaNodePtr1;	//第一类输入方式节点
	ARRAY_LIST<TTA_NODEPTR>m_arrTtaNodePtr2;	//第二类输入方式节点
	ARRAY_LIST<TTA_ELEM>m_arrTtaElem;
	ARRAY_LIST<TTA_BRACEELEM>m_arrTtaBraceElem;
	CXhPtrSet<CLDSNode> listIntersNodes;
	//ARRAY_LIST<LDS_BODYLEG>m_arrLdsBodyLeg;
	ARRAY_LIST<TTA_BODYLEG>m_arrBodyLeg;
	ARRAY_LIST<TTA_WINDSEG>m_arrWindSeg;
	CHashList<TTA_NODEPTR>hashTtaNodeH;		//
	CHashList<long> hashTansLegBraceNodes;	//键值为导入节点编号的绝对值（TANS中原始点号为负时表示腿部辅材轮换节点）
protected:	//点号个位数间的对称关系开关,0x01表示Y轴（左右）对称有效;0x02表示X轴（前后）对称有效;0x04表示Z轴（对角）对称有效
	struct MIRFLAG{
		BYTE cbFlag;
		MIRFLAG(BYTE _cbFlag=0x8f){cbFlag=_cbFlag;}
		bool IsEnableMirX(){return (cbFlag&0x02)>0;}
		bool IsEnableMirY(){return (cbFlag&0x01)>0;}
		bool IsEnableMirZ(){return (cbFlag&0x04)>0;}
		bool EnableMirX(bool enabled=true);
		bool EnableMirY(bool enabled=true);
		bool EnableMirZ(bool enabled=true);
	};
	MIRFLAG arrMirFlags[1000];		//受力材节点，节点号<10000
	CHashList<MIRFLAG>hashMirFlags;//辅材节点
	bool IsEnableMirX(long pointI);
	bool IsEnableMirY(long pointI);
	bool IsEnableMirZ(long pointI);
	bool EnableMirX(long pointI,bool enabled=true);
	bool EnableMirY(long pointI,bool enabled=true);
	bool EnableMirZ(long pointI,bool enabled=true);
	MIRFLAG* GetMirFlag(long pointI);
	MIRFLAG* AddMirFlag(long pointI,BYTE cbMirFlag=0x8f);
public:
	CHashListEx<TANS_LINE> hashLines;
	CHashListEx<TANS_PLANE> hashPlanes;
	CHashSet<TTA_NODEPTR> hashTtaNodeI;
	CHashSet<TTA_NODEPTR> hashOnlyBraceNodes;
	CHashList<bool>hashParentTtaNodeI;		//一组对称节点中最原始的数据文件录入节点
	struct MYLCFG_WIRENODE{
		char ciWireType;//备份存储
		long iWirePlacePointI;
		short voltage;	//电压等级
		CXhChar50 wirename;
		GEPOINT xLocation;	//单位mm
		CLDSNode* pRelaNode;
		MYLCFG_WIRENODE(){iWirePlacePointI=voltage=ciWireType=0;pRelaNode=NULL;}
	};
private:
	bool m_bLoadDataIniFile;
	CHashSet<CLDSNodePtr>hashLdsNodeI;
	int JustifyCtrlInfoFormatType(BYTE ciFileType,CXhChar50* pxarrWords1,int count1,CXhChar50* pxarrWords2,int count2);
	bool AppendTtaNodePtr(ARRAY_LIST<TTA_NODEPTR>& arrNodePtr,TTA_NODE* pTtaNode);
	bool PrepareTtaModel();
	bool UpdateTtaNodeCfgWord(TTA_NODE* pTtaNode);
	long m_nOverlapBraceNodes;
	void BuildLdsGeoModel(CHashSet<TTA_NODEPTR>* pHashOnlyBraceNodes=NULL,CMapList<TTA_BRACEELEM*>* pMapOnlyBraceElems=NULL,bool updateMirRelaInfo=true);	//根据TTA几何信息建立LDS几何模型
	TTA_NODE* AddOrUpdateMirTtaNode(TTA_NODE* pSrcTtaNode, int mir_i,bool anywayReturn=true);	//mir_i遵循TTA文件对称约定规范
	void Empty();
	bool UpdateDatumLine(TANS_LINE* pLine);
	bool UpdateDatumPlane(TANS_PLANE* pPlane);
public:
	CTower* get_pModel()const {return m_pContextModel;}
	__declspec(property(get=get_pModel)) CTower* pModel;
	CHashPtrList<LIST_TTANODE_SET> localTtaNodeSets;
	ATOM_LIST<TTA_ASSISTELEM> listTtaAssElems;
public:
	CTtaDataFile(CTower* pBelongModel=NULL);
	virtual ~CTtaDataFile();
	//
	void ExtractWireNode();
	CFGWORD ValidateBraceNodeCfgWord(TTA_NODE* pTtaNode,bool bOnlyByDependency=false);
	bool ReadTowerTypeCfgFile(FILE* fp, BYTE ciFileType = 0);
	bool ReadTansNewFormatIniFile(FILE* fp);
	bool ClassifyRodSizeSpecs(int iDefaultSizeSpecType=1,BYTE ciFileType=0);
	void WriteDataFile(FILE* fp);
	void ReadDataFile(FILE* fp,BYTE ciFileType=0,const char* szDataFilePathName=NULL);
	//西北院&SmartTower程序辅助材
	bool ReadDatElemsSpecFile(FILE* fp,CMapList<TTA_ELEM*>* pMapElems,CMapList<TTA_BRACEELEM*>* pMapBraceElems);
	bool ReadDatBraceElemsDataFile(FILE* fp,CMapList<TTA_BRACEELEM*>& mapOnlyBraceElems,const char* sKeyText=NULL);
	//河南院TANS程序辅助材
	bool ReadTANSBraceElemsDataFile(const char* szBraceDataFile,CLDSNode* pDependLdsNodes=NULL);
	bool ReadSubBraceElemsDataFile(const char* szBraceDataFile,GTM_KEYNODE_MAP* pxarrMapNodes=NULL,int count=1,char ciFileType=3);
	void ReadOutFile(FILE* fp,FILE* auxout_fp2=NULL,void* pRptObj=NULL,char ciFileType=0);
private:
	bool ReadSmartTowerLoadSection(FILE* fp);
	void ReadDaoHengAuxElemOutFile(FILE* fp,void* pRptObj=NULL,char ciFileType=0);	//执行前必须先执行ReadOutFile
};
void TtaFileIn(LPCTSTR lpszPathName = NULL, char ciFileFormatType = -1);
void TtaFileExport();
#endif
#endif // !defined(AFX_TTADATAFILE_H__062EEA51_A1F6_48A9_ACBE_C091B018A0DC__INCLUDED_)
