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
	long pointI;		//TTA�нڵ��
	long J1,J2,J3,J4,J5;
	//0.���κ������ڵ�;1.ָ��X���������;2.ָ��Y���������;3.ָ��Z���������;4.�����ȷֵ�;5.�����;
	//����ԺTANS��������չ������ӳ��:
	//6.�ظ˼���ĳ�ڵ����������Ľڵ�;7.Ԥ��ƫ�Ƶ�;8.��׼����XY����ֵ�����;9.��׼����YZ����ֵ�����;
	//10.��׼����XZ����ֵ�����;12.ԭʼ��Գ�;13.(����յ�)������ڵ�
	long posType;
	char ciPosViceType;	//�ڵ�λ���������ͣ�'X','Y','Z'
	char cMirTransType;	//��׼��ԳƲ������ͣ�'X','Y','Z'
	int IS;				//0.�޶Գ�1.����Y��Գ�;2.ǰ��X��Գ�;3.�Խ�Z��Գ�;4.�ĶȶԳ�(X/Y/Z�Գ�)
	f3dPoint pos;
	union{
		double coefScale;	//�����ȷֵ�ı���ֵϵ��
		double offsetDist;	//ƫ�ƾ��루һ����ciPosViceTypeָ��������ƫ��һ�����룩
	};
	//long hits;			//���������ĵ�Ԫ����
	//��������������LDSת����
	CFGWORD cfgword;
	char cPosLayer;
	bool bBraceNode;
	bool bValidateCfgWord;	//�Ƿ�����֤cfgword����ĺ�Ϊ������Ч��ĺţ�����cPosLayer)
	bool bFoundationNode;
private:
	bool _blInitPosCalParams;//�Ƿ��ѳ�ʼ���ڵ�����궨λ����ֵ
	BYTE m_cbPosUpdated;
	BYTE m_cnVisitHits;
	char m_ciMirFromType;	//��pMirFromNode�ĶԳ���������,'X','Y','Z',0��ʾΪԭʼ�ڵ�
	TTA_NODE *_pMirFromNode;	//�Գ����ɽڵ��ԭʼ��������Գƽڵ㣬ΪNULL��ʾ����Ϊԭʼ����ڵ�
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
	__declspec(property(get=get_cbPosUpdated)) BYTE cbPosUpdated;	//��ȡ�ڵ㵱ǰ���������״̬
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
	TTA_NODE *pStart,*pEnd;	//��Ԫ��ʼ�����ֹ��
	int N;	//��N  �������ű�ʾ����
	int IS;
	int TYPE;
	int M;
	int L1,L2;
	int SMC;
	char cMaterial;	//'S':Q235;'H':Q345;'G':Q390;'P':Q420;'T':Q460;
	int iStatMatNo;	//LDSͳ�ĺ�
	int iSizeOrder;
	//bool bTubeRod;
	double boltgrade;
	int boltd,boltN;
	double dfHoleOutCoef;	//ָ������ϵ����0��ʾδ����ָ��
	SIZESPEC size;
	bool bReadSizeSpec;
	TTA_ELEM(){
		memset(this,0,sizeof(TTA_ELEM));
		cMaterial='S';
		iStatMatNo=0;
		boltd=20;boltN=1;	//��������˨Ĭ��Ϊ0
	}
};
struct TANS_LINE{	//����ԺTANS�������
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
	TTA_NODE *pStart,*pEnd;	//��Ԫ��ʼ�����ֹ��
	int IS;
	int M;	//���ศ����M����������TTA_ELEM::M����
	//��λ��0:ֻ��ϸ����Ҫ��ѡ��1:����ϸ����Ҫ���⣬��Ӧ���ǳе�J1�����ڸ˼�������N��
	//2:����ϸ����Ҫ����,��Ӧ���ǳе�J2�����ڸ˼�������N��
	//ʰλ����λ��Ϊ������˼��������һ�鲹���ĵĸ���
	int N;	//N�ò��ĳе�����֧�Ÿ˼�������ǧ����
	int iStatSerial;	//�൱��TTA_ELEM::Nͳ�ĺ�
	int percentOfMinRL,percentOfParaRL;
	double boltgrade;
	int boltd,boltN;
	char cSectType;		//cSectType��'R'���ֹ�;'L',�Ǹ�;'T',T��˫ƴ�Ǹ�;'D',�Խ�˫ƴ�Ǹ�;'X',��ƴ�Ǹ�
	char cBriefMaterial;
	double size_wide,size_thick;	//�����������
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
	int Jmin;	//���ϵ���С�ڵ��
	int Fmin;	//���ϵ���С�����ڵ��
	int Fmax;	//���ϵ��������ڵ��
	int MAX;	//���������ϵĲ��������˽ڵ��
	int LB;		//��λ��Ϊ���������ı��弴�߶Ⱥţ�ʮλΪ�����������Ĳ��ȸ��ȵĸ߶���
	int Var;	//һ��Var=0
	int MylBraceNodeMAX;	//������Ӧ������������Ķ˵��
	//��������������LDSת����
	int iLeg;				//���Ⱥ�
	int niLegN;	//�ú���������ĳ���������
	//��LDS����ģ�ͼ�Ĺ�������
	char ciType;	//0.��ӦTTA����;1.��ӦTTA����
	DWORD dwBodyFlag;		//��ӦTTA����
	CLDSModule *pModule;	//��������ģ��(����)
	TTA_BODYLEG();
	DWORD ApplyListTtaNodeSet(CTtaDataFile* pTtaFile);//���������������Ľڵ㼯��,�Ա��ڰ�TTA������б��
	CXhSimpleList<TTA_NODEPTR>& getListTtaNodeSet();
	_declspec(property(get=getListTtaNodeSet)) CXhSimpleList<TTA_NODEPTR>& listTtaNodeSet;
private:
	CTtaDataFile* m_pTtaFile;
	DWORD listId;
};
struct LDS_BODYLEG
{
	char ciType;	//0.��ӦTTA����;1.��ӦTTA����
	DWORD dwBodyFlag;		//��ӦTTA����
	CXhSimpleList<TTA_NODEPTR>* pRelaNodeSet;	//���������������Ľڵ㼯��,�Ա��ڰ�TTA������б��
	CLDSModule* pModule;	//��ӦTTA����
	//long Jmin;	//������TTA�ļ��е���ʼ�ڵ��
	//long Tmin,Tmax;	//LDS�б�����������С�ڵ����С����(����)�ڵ�ż������(����)�ڵ��
	//long iLeg;		//LDS�еĽ��Ⱥ�
	//long LB;	//��λ��Ϊ���������ı��弴�߶Ⱥţ�ʮλΪ�����������Ĳ��ȸ��ȵĸ߶���
};
struct TTA_WINDSEG
{
	int TYPE;
	int arrPointI[4];
	int NLG;
	int Fa,Fb;	//��������������ϵ��
	int Fw;		//��������ϵ��
	int BetaZ;	//����ϵ��
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
	SIZESPEC size;	//���
	double length;	//��Ԫ����,mm(TTA��Ϊm)
	double ulength;	//���㳤��,mm(TTA��Ϊcm)
	double gyration;//��ת�뾶,mm(TTA��Ϊcm)
	double lamda;	//��ϸ��
	double allowLamda;	//����ϸ��
	double stableFactor;//ѹ��ϵ��
	double maxTension;			//kN
	double maxCompression;		//kN
	int idMaxTensionCase;		//�������������
	int idMaxCompressionCase;	//���ѹ��������
	double strengthDeductCoef;	//ǿ���ۼ�ϵ��
	double actualStress;	//����Ӧ��,MPa
	double allowStress;		//�������Ӧ��,MPa
	double efficiency;	//��ԪЧ��
	float boltgrade;	//��˨����
	short boltd,boltN;	//��˨ֱ��������
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
	TTA_NODE* pLegTopMidNode;	//���������渨����Ƭ����м�ڵ�(
	GTM_KEYNODE_MAP() { memset(this,0,sizeof(GTM_KEYNODE_MAP)); }
};
struct GTM_FZC_FNAME {
	char ciFzcPanelType;	//0.����;1.���������渨��;2.����V�渨��
	char ciPrefixSymbol;	//�������ͷ��ţ��������帨��ȡֵ 'B'/'C'��ʾ�����ᵣ����������ȡֵ'A'/'R'��ʾ����/������
	union{long liPointA,liPointTop;};	//�ؼ��ڵ�A(����������������νӽڵ�)
	union{long liPointB,liPointBase;};	//�ؼ��ڵ�B(����������ϻ����ڵ�)
	struct {
		long liHeight,liLegDiff;
		long liVTopPointA,liVTopPointB;	//V��б�Ķ����ڵ㣨��������������˳��
	}subleg;//����(����),���ȸ߲�(��ƽ�ȸ߲����)
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
	static const BYTE FILETYPE_TANS = 3;	//����ԺTANS
	static const BYTE FILETYPE_GTOM = 4;	//�㶫ԺGTower
private:
	CTower *m_pContextModel;
	BYTE m_ciFileType;	//�ļ���ʽ
	long KIND,N10,N20,M10,M20,IE,NL,NC,NW,NSEC,WIND;	//WIND��SmartTower�з��ؼ�����Ϣ
	long K1,K2,K3,K4,K5,K6,NA,TSM,REN,NGP,VAL2;
	long NBY,NLEG;
	long IW;	//��˨�������趨����֫����ֵ
	double Wk;	//�㶫ԺGTM��ʽ���Ʋ���=���Եػ�����(kN/m^3)��
	long CODE,LOAD_GROUP;	//�㶫ԺGTM��ʽ���Ʋ������淶��𼰺��ط���
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
	CHashList<TTA_NODE>hashTtaNodesByH;	//��LDS�еĽڵ���Ϊ��ֵ,����Ӧ��ȫȡ��m_arrTtaNode
	ARRAY_LIST<TTA_NODE>m_arrTtaNode;
	ARRAY_LIST<TTA_NODEPTR>m_arrTtaNodePtr1;	//��һ�����뷽ʽ�ڵ�
	ARRAY_LIST<TTA_NODEPTR>m_arrTtaNodePtr2;	//�ڶ������뷽ʽ�ڵ�
	ARRAY_LIST<TTA_ELEM>m_arrTtaElem;
	ARRAY_LIST<TTA_BRACEELEM>m_arrTtaBraceElem;
	CXhPtrSet<CLDSNode> listIntersNodes;
	//ARRAY_LIST<LDS_BODYLEG>m_arrLdsBodyLeg;
	ARRAY_LIST<TTA_BODYLEG>m_arrBodyLeg;
	ARRAY_LIST<TTA_WINDSEG>m_arrWindSeg;
	CHashList<TTA_NODEPTR>hashTtaNodeH;		//
	CHashList<long> hashTansLegBraceNodes;	//��ֵΪ����ڵ��ŵľ���ֵ��TANS��ԭʼ���Ϊ��ʱ��ʾ�Ȳ������ֻ��ڵ㣩
protected:	//��Ÿ�λ����ĶԳƹ�ϵ����,0x01��ʾY�ᣨ���ң��Գ���Ч;0x02��ʾX�ᣨǰ�󣩶Գ���Ч;0x04��ʾZ�ᣨ�Խǣ��Գ���Ч
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
	MIRFLAG arrMirFlags[1000];		//�����Ľڵ㣬�ڵ��<10000
	CHashList<MIRFLAG>hashMirFlags;//���Ľڵ�
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
	CHashList<bool>hashParentTtaNodeI;		//һ��Գƽڵ�����ԭʼ�������ļ�¼��ڵ�
	struct MYLCFG_WIRENODE{
		char ciWireType;//���ݴ洢
		long iWirePlacePointI;
		short voltage;	//��ѹ�ȼ�
		CXhChar50 wirename;
		GEPOINT xLocation;	//��λmm
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
	void BuildLdsGeoModel(CHashSet<TTA_NODEPTR>* pHashOnlyBraceNodes=NULL,CMapList<TTA_BRACEELEM*>* pMapOnlyBraceElems=NULL,bool updateMirRelaInfo=true);	//����TTA������Ϣ����LDS����ģ��
	TTA_NODE* AddOrUpdateMirTtaNode(TTA_NODE* pSrcTtaNode, int mir_i,bool anywayReturn=true);	//mir_i��ѭTTA�ļ��Գ�Լ���淶
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
	//����Ժ&SmartTower��������
	bool ReadDatElemsSpecFile(FILE* fp,CMapList<TTA_ELEM*>* pMapElems,CMapList<TTA_BRACEELEM*>* pMapBraceElems);
	bool ReadDatBraceElemsDataFile(FILE* fp,CMapList<TTA_BRACEELEM*>& mapOnlyBraceElems,const char* sKeyText=NULL);
	//����ԺTANS��������
	bool ReadTANSBraceElemsDataFile(const char* szBraceDataFile,CLDSNode* pDependLdsNodes=NULL);
	bool ReadSubBraceElemsDataFile(const char* szBraceDataFile,GTM_KEYNODE_MAP* pxarrMapNodes=NULL,int count=1,char ciFileType=3);
	void ReadOutFile(FILE* fp,FILE* auxout_fp2=NULL,void* pRptObj=NULL,char ciFileType=0);
private:
	bool ReadSmartTowerLoadSection(FILE* fp);
	void ReadDaoHengAuxElemOutFile(FILE* fp,void* pRptObj=NULL,char ciFileType=0);	//ִ��ǰ������ִ��ReadOutFile
};
void TtaFileIn(LPCTSTR lpszPathName = NULL, char ciFileFormatType = -1);
void TtaFileExport();
#endif
#endif // !defined(AFX_TTADATAFILE_H__062EEA51_A1F6_48A9_ACBE_C091B018A0DC__INCLUDED_)
