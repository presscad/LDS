// ModDataFile.h: interface for the CModDataFile class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __PART_DESIGN_INC_
#include "Tower.h"
struct JG_NORM_INFO
{
	f3dPoint vNormX;
	f3dPoint vNormY;
};
struct HANG_NODE_GROUP
{
	BYTE ciType;
	f3dPoint tag_pos;
	ARRAY_LIST<HANG_NODE_INFO> hangNodeArr;
};
struct NODE_DATA
{
	int iPointI;	//点序号
	BYTE ciLevel;	//点级别（0.body|1.leg|2.subleg）
	NODE_DATA(){iPointI=0;ciLevel=0;}
	NODE_DATA(int iKey){iPointI=iKey;ciLevel=0;}
};
struct ROD_DATA
{
	int iPointS;	//始端点序号
	int iPointE;	//始端点序号
	ROD_DATA(){iPointS=0;iPointE=0;}
};
struct MODULE_ITEM
{
	int m_iBody;
	int m_iLeg;
	double m_fNamedHeight;	//呼称高度
	PRESET_ARRAY32<int> m_arrBody;
	MODULE_ITEM(){m_iBody=0;m_iLeg=0;m_fNamedHeight=0;}
};
class CBasicSegment
{
public:
	virtual bool ValidateRodLiveScope(CLDSLinePart* pRod,CBasicSegment* pParentSegment){return true;}
	virtual bool IsBodyType(){return false;}
	virtual bool IsLegType(){return false;}
	virtual bool IsSubLegType(){return false;}
};
class SUB_LEG_INFO : public CBasicSegment{
public:
	double m_fLegH;		//减腿高度
	NODESET legNodeSet;	//腿部节点集合
	LINEPARTSET legRodSet;//
	SUB_LEG_INFO(){m_fLegH=0;}
	virtual bool IsSubLegType(){return true;}
	virtual bool ValidateRodLiveScope(CLDSLinePart* pRod,CBasicSegment* pParentSegment);
};
class CLegGroupItem : public SUB_LEG_INFO
{	//与杆塔呼高(即CLDSModule)一一对应
public:
	long m_idHeight;			//呼高号
	CLDSNode* m_pTopTagNode;	//呼高组顶部标签节点（本体与呼高专属部分的过渡节点）
	double m_fExclusiveBodyH;	//专属塔身部分段高(mm)
	double m_fMaxLegH;		//最长接腿高(mm)
	GEPOINT m_xLowestLegPt;	//最长腿坐标
	struct NODEREF_OBJ{	//为满足道亨杆件不跨段要求，而临时在MOD文件中添加的冗余节点
		long iPointI;
		CLDSNode* pNode;
		GEPOINT pos;
		NODEREF_OBJ(CLDSNode* _pNode=NULL,long iNewPointI=0)
		{
			pNode=_pNode;
			if(pNode!=NULL)
				pos=pNode->xOriginalPos;
			iPointI=iNewPointI;
		}
	};
	struct RODREF_OBJ{
		long iStartPointI,iEndPointI;
		CLDSLinePart* pRod;
		RODREF_OBJ(CLDSLinePart* _pRod=NULL){
			pRod=_pRod;
			iStartPointI=iEndPointI=0;
		}
		operator CLDSLinePart*(){return pRod;}
	};
	NODESET segmentNodeSet;	//共有分段节点
	LINEPARTSET segmentRodSet;	//
	CHashList<RODREF_OBJ> hashRodRefObjs;	//一般仅在需要变更杆件始末端点号时才赋值
	ATOM_LIST<SUB_LEG_INFO> subLegInfoList;
	CXhSimpleList<NODEREF_OBJ> listRedundantNodes;
public:
	CLegGroupItem();
	~CLegGroupItem();
	//
	virtual bool IsLegType(){return true;}
	void InitLegItemByModule(CLDSModule* pModulePtr,CHashSet<CLDSNode*>& hashLinkNode);
	CLDSNode* FindMidNode(CLDSLinePart* pRod); 
};
class CBodyItem : public CBasicSegment
{
public:
	CLDSNode* m_pBtmTagNode;//共用本体段的底部特征节点(第二象限Z值最大节点)
	double m_fBodyH;		//本体高度
	NODESET bodyNodeSet;	//
	LINEPARTSET bodyRodSet;
	int m_iLegS;			//本体下开始接腿序号
public:
	CBodyItem();
	~CBodyItem();
	//
	virtual bool IsBodyType(){return true;}
	bool IsIncludeLegItem(CLegGroupItem* pLegItem);
	void InitBodyItem(CLDSNode* &pTopTagNode,NODESET& selNodeSet,LINEPARTSET& selRodSet);
};
class CModDataFile
{
	double m_fTowerHeight;
	//
	struct HEIGHT_JOINT {
		BYTE idHeightSerial;
		bool blJointed;
		HEIGHT_JOINT(BYTE _idHeightSerial=0) { 
			idHeightSerial=_idHeightSerial;
			blJointed=false; 
		}
	};
	BOOL RetrieveBodyInfo(HEIGHT_JOINT xarrHeightConns[24],
		NODESET& validNodeSet,NODESET& selNodeSet,LINEPARTSET& validRodSet,LINEPARTSET& selRodSet);
	CLegGroupItem* FindLegGroupItem(int module_no);
	BOOL PrepareParaData();	//Ta数据-->mod数据
	BOOL AmendData();	//mod数据-->Ta数据
	BOOL IsHasValidPointI();
	BOOL IsUTF8File(const char* mod_file);
protected:
	struct HEIGHT_HEADER{
		UINT idJointShareBody;
		float fNamedHeight;
	};
	CLdsList<CLDSNode>  m_xAssistNode;	//过渡辅助节点集合
	CHashSet<CLDSNode*> m_hashLinkNode;
	ATOM_LIST<CLegGroupItem> m_listHeightItem;
	ATOM_LIST<CBodyItem> m_listBodyItem;
	CHashList<NODE_DATA> m_hashNodeData;
	CHashList<ROD_DATA> m_hashRodData;
	ARRAY_LIST<MODULE_ITEM> m_listModuleItem;
	ARRAY_LIST<HANG_NODE_INFO> m_listGuaNode;
	CHashList<JG_NORM_INFO> m_hashJgNorm;
	GECS BuildUcsByModCS();	//建立LDS模型坐标系（以MOD模型坐标系为基底参照）
	GECS BuildRotateCS();	//
public:
	BOOL m_bTurnLeft;		//杆塔模型是否左转
public:
	CModDataFile();
	~CModDataFile();
	//
	void WriteToFileByUtf8(FILE* fp);
	void WriteToFileByAnsi(FILE* fp);
	void ReadModFile(FILE* fp,BOOL bUtf8=FALSE);
	void ImportModFile(const char* szTowerModFile);
	void ExportModFile(const char* szTowerModFile);
};
//////////////////////////////////////////////////////////////////////////
#include "GimDef.h"
class CGimDataFile
{
	CXhChar500 m_sPhmGuid;		//杆塔组合模型文件.phm
	CXhChar500 m_sDevGuid;		//杆塔物理模型文件.dev
	CXhChar500 m_sFamGuid;		//杆塔属性文件.fam
	CXhChar500 m_sCbmGuid;		//杆塔设备工程文件.cbm
	CXhChar500 m_sOutPath;		//GIM输出路径
public:
	static const BYTE ALL_FILE = 0;
	static const BYTE CBM_FILE = 1;
	static const BYTE DEV_FILE = 2;
	static const BYTE PHM_FILE = 3;
	static const BYTE FAM_FILE = 4;
	static const BYTE MOD_FILE = 5;
	//
	static const BYTE UTF8_CODING	= 0;
	static const BYTE ANSI_CODING	= 1;
	BYTE m_ciCodingType;	//编码格式:0.ANSI|1.UTF-8
	CXhChar50 m_sName;
	CXhChar500 m_sFolder;
	CXhChar500 m_sModFile;		//
	GIM_HEAD_PROP_ITEM m_xGimHeadProp;
	TOWER_PRPERTY_ITEM m_xTowerProp;
public:
	void WriteModFile(const char* sModFilePath);
	void WritePhmFile(const char* sPhmFile);
	void WriteDevFile(const char* sDevFile);
	void WriteFamFile(const char* sFamFile);
	void WriteCbmFile(const char* sCbmFile);
	BOOL ReadDevFile(const char* sDevFile);
	BOOL ReadFamFile(const char* sFamFile);
	BOOL ReadPhmFile(const char* sPhmFile);
	BOOL ReadCbmFile(const char* sCbmFile);
	void ToBuffer(CBuffer &buffer, CBuffer &zipBuffer);
	void FromBuffer(CBuffer &buffer);
	BOOL IsTowerDevFile(const char* sDevFile);
	void ReadGimHeadInfo(CBuffer &buffer);
	void SearchFile(ATOM_LIST<CString>& xFileList, CXhChar200 sFilePath, BYTE ciFileType = 0);
public:
	CGimDataFile();
	~CGimDataFile();
	//
	void CreateGuidFile();
	void DeleteGuidFile();
	BOOL PackGimFile();
	BOOL UpdateGimFile();
	BOOL UnpackGimFile(const char* gim_path);
	BOOL ParseGimFile(const char* gim_path);
	BOOL ParseGimFileEx(const char* gim_path);
	static BOOL Run7zCmd(char* sCmd);
};
#endif