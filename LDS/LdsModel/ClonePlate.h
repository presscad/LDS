#pragma once
#include "MemberProperty.h"
#include "lds_part.h"
#include "Markup.h"
#include "AngleLayout.h"


struct LINKPOLEINFO{
protected:
	UINT id;
public:
	CLDSLinePart *pLinePart;//��¼���Ӹ˼�ָ��
	CLDSLinePart *pMatchLinePart;//��¼ƥ��˼�ָ��
	long hLinePart;			//���Ӹ˼����
	long hMatchPole;		//��¼�뵱ǰ�˼�ƥ������߸˼����
	double offset_angle;	//��¼���߸˼����׼�˼�֮��ļн�
	int vec_type;			//��¼���߸˼����췽�� 1��ʾ���׼�Ǹֵ�ǰ֫���췽����ͬ -1��ʾ���׼�Ǹֵ�ǰ֫���췽���෴
	void SetKey(DWORD key){id=key;}
	ADD_READONLY_PROPERTY(UINT,Id){return id;}
	LINKPOLEINFO()
	{
		pLinePart=NULL;
		pMatchLinePart=NULL;
		hLinePart=0;
		offset_angle=0;
		hMatchPole=0;
		vec_type=0;
	}
};
class ANGLE_MAPPING{
	ANGLE_LAYOUT xSrcLayout;
	bool Match(CLDSLineAngle* pDestAngle,char _ciDestRayOrgTipS0E1,GECS& acsMapTo);
public:
	char ciSrcRayOrgTipS0E1;
	char ciLegX2DestLeg;	//Դ�Ǹֵ�X֫��Ŀ��Ǹ��е�ƥ��ӳ��֫��'X/x','Y/y'��д��ĸΪ������Сд��ĸΪ����
	char ciLegY2DestLeg;	//Դ�Ǹֵ�Y֫��Ŀ��Ǹ��е�ƥ��ӳ��֫��'X/x','Y/y'��д��ĸΪ������Сд��ĸΪ����
	//bool blReverseTipSnE;
	//char ciDestRayOrgTipS0E1
	ANGLE_MAPPING(){
		//blReverseTipSnE=false;
		ciLegX2DestLeg='X';
		ciLegY2DestLeg='Y';
		ciSrcRayOrgTipS0E1=0;
	}
	void InitSrcAngle(CLDSLineAngle* pSrcAngle,char _ciSrcRayOrgTipS0E1,GECS& lcsSrc);
	bool Mapping(CLDSLineAngle* pDestAngle,char _ciDestRayOrgTipS0E1,GECS& lcsMapTo);
};
class CSchemaRod : public LINKPOLEINFO
{
protected:
	bool m_bInitSchemaInfo;
	bool m_bUpdateMatchRodInfo;
public:
	bool get_blInitSchemaInfo(){return m_bInitSchemaInfo;}
	__declspec(property(get=get_blInitSchemaInfo)) bool blInitSchemaInfo;
	bool get_blUpdateMatchRodInfo(){return m_bUpdateMatchRodInfo;}
	__declspec(property(get=get_blUpdateMatchRodInfo)) bool blUpdateMatchRodInfo;

	long m_idClassType;
	struct DESIGNLJ_ANGLEPARA
	{
		BYTE ciSonAngleQuad;	//��׼�Ǹ���Ϊ��ϽǸ��ҵ�ǰ�Ǹ�Ϊ��׼��ϽǸֵ��ӽǸ֣�����X-Zƽ���ڵ����޺�,ȡֵ:1-4��0��ʾ����Ǹ�
		BYTE ciWorkWingX0Y1;	//��ǰ����֫,δ�����Ӧ�Ѵ�������ȫ��xWingLayoutȡ���� wjh-2019.3.29
		ANGLE_LAYOUT xWingLayout;//����֫���÷�ʽ
		ANGLE_SPACE_FLAG cbSpaceFlag;		//
		BOOL bEndLjAngle;		//�����ӽǸ�
		BOOL bDualEdge;			//�Ƿ���˨�����඼����������
	}angle;
	struct DESIGN_ODD
	{
		long hOddRefRod1,hOddRefRod2;	//����ͷУ����չ���
		double fCollideDist;
		BYTE ciOddCalStyle;	//0.��ʾ������ײ���ո˼�����;1.������˨λ�ü���;2.��ʾ����������ͷ
	}odd;
	BYTE ciConnectTipS0E1;	//ģ���и˼�ԭʼ������Ӷˣ�ʼ��0���ն�1;
	short siPlaneNo;		//��ǰ�˼����ڵ�������,0:��׼ƽ��;1.��һ������;2.�ڶ�������
	short siEndSpace,siWingSpace,siBerSpace;	//���������ʱ�Ĳ���
	//struct MAPPING{	// MAPPING
		//��ǰӳ��Ǹ��Ƿ���Ҫ�ߵ�X/Y֫ wjh-2019.3.29
		bool blReverseTipSnE;
		//��ǰӳ��Ǹ���ģ�������ӽǸּ��֫ӳ���ϵ
		char ciLegX2DestLeg;	//Դ�Ǹֵ�X֫��Ŀ��Ǹ��е�ƥ��ӳ��֫��'X/x','Y/y'��д��ĸΪ������Сд��ĸΪ����
		char ciLegY2DestLeg;	//Դ�Ǹֵ�Y֫��Ŀ��Ǹ��е�ƥ��ӳ��֫��'X/x','Y/y'��д��ĸΪ������Сд��ĸΪ����
		//��ǰӳ��Ǹ��Ƿ���Ҫ�ߵ�X/Y֫ wjh-2019.3.29
		bool get_blReverseLegXnY(){return ciLegX2DestLeg=='Y'||ciLegX2DestLeg=='y';}
		__declspec(property(get=get_blReverseLegXnY)) bool blReverseLegXnY;
	//}mapto;
public:
	CSchemaRod();
	bool ClearMatchRodInfo(){m_bUpdateMatchRodInfo=false;}
	void CopyProperty(CSchemaRod* pScrSchemaRod);
	void InitSchemaAngle(CLDSLineAngle* pSrcAngle,GECS& scs);
	bool Match(CLDSLineAngle* pDestAngle,char ciDestConnTipS0E1,GECS& lcs);
};

struct ANCHOR_LOCATION{
	BYTE ciAnchorType;	//1.�Ǹ��������Ϊ��׼;2.�˼��Ͻڵ�Ϊ��׼;3.�Ǹ����߽���;
	BOOL bIncOddEffect;	//��ciAnchorType=1ʱ����ʾ����ƫ�����Ƿ��Ƕ�ͷ����ͷ
	BYTE ciIntersPlane;	//ƽ���棬0��ʾ��ƽ��;1:X-Yƽ��;2:X-Z;3:Y-Z;TODO:δ������ciAnchorType���ϵ�һ���ֽ���
	COORD3D vPlaneNormal;	//ƽ���淨��
	UINT idPriorAngle,idSecondaryAngle;
	union INTERS_ANGLE_G{
		DWORD dwParamG;
		struct{short g,ciStyleG;};	//ciStyleG=0:g;1:g1;2:g2;3:g3;4:ָ��ֵ
	}g_arr[2];
	BYTE ciLenDirection;	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ�����Ի�׼�����ǽǸ�ʱ��Ч��
	short siLenOffset;	//��anchor.idResideAngle�Ǹֵ�����ƫ����,����ƫ��������G_ARR
};
class CSchemaBolt
{
	UINT id;
	WORD m_wStateFlag;	//���Կ�����˨��������ɲ������涯���£���ֱ���Ƿ���Ҫ����֫��ѡ����
public:
	long d;	     			// ��˨ֱ��.
	char grade[5];			// ��˨����
	bool m_bOnlyHole;		//��������˨
	DWORD dwRayNo;			// ���ڵ����������ţ���ͼʱ��
	struct WORKNORMAL{
		BYTE ciNormStyle;	//Ŀǰ���ڽǸ�ֻ��Ϊ1����ʾ�Ǹ�֫���߷���;
		BYTE ciDirection;	//0.����;1.����
		BYTE ciWorkWing;	//��˨��idResideAngleָ��Ǹֵĵļ���֫x0,y1,���idResideAngle=0����ʾ�Ըְ�Ϊ��׼
		UINT idResideAngle;	//��˨���ڵĽǸ֣�����˨���ߵĵļ������ݽǸ�
	}normal;	//��˨���������������
	struct THIN_THICKITEM{
		char ciItemType;	//0.FinalKey;1.���׹���Idֵ(0xff��ʾ�����,0x7f��ʾ�����);2.ָ�����ֵ;
		BYTE ciThickVal;	//����ciItemTypeָ���ĺ��ֵ
		THIN_THICKITEM(){ciItemType=0;ciThickVal=0;}
	};
	struct PARATHICK{	//����CParaThick
		THIN_THICKITEM items[6];
		char* ToThickStr();
	};
	struct DESIGN_BASE_POS
	{
		UINT idResideAngle;	//��˨���ڵĽǸ֣�����˨���򼰺���ƫ�����ļ������ݽǸ�(�����뷨�߲�һ��)
		ANCHOR_LOCATION anchor;	//��˨��׼���ʼ����(�����������ƶ�)�������
		static const BYTE GLOBAL_COORD = 0;
		static const BYTE LOCAL_COORD  = 1;
		BYTE cLocationStyle;	//0:GLOBAL_COORD��������Ϊ��������;1:LOCAL_COORD��������Ϊ��׼�������������
		BYTE ciDirection;			//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ�����Ի�׼�����ǽǸ�ʱ��Ч��
		BYTE ciWingOffsetStyle;	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		short wing_offset_dist,len_offset_dist;//֫��ƫ��ֵ��Y��;����ƫ��ֵ��X��;
		union{
			BYTE ciWorkWing;	//0.X֫;1.Y֫�����Ի�׼�����ǽǸ�ʱ��Ч��
			BYTE ciResidePlane;	//0.���ڸְ����(0��ʾ����;1.��ʾ��һ��������;2.��ʾ�ڶ���������) wjh-2015.2.7
		};
		PARATHICK norm_offset;	//����ƫ�����ַ���
	}des_base_pos;
	PARATHICK through;	// ��˨ͨ�����.��ӦL0
public:
	CSchemaBolt();
	void InitThrough(const char* thickStr);
	void SetKey(DWORD key){id=key;}
	ADD_READONLY_PROPERTY(UINT,Id){return id;}
};

//��¡�ְ�
class CClonePlate{
protected:
	//0.��ƥ�� 
	//1.��ȫƥ��(�����ͬ)  �Ƕ�ƥ�� ���߽Ǹֹ��ƥ�� ���Ĺ��ƥ�� ���߽Ǹ�������ͬ
	//2.���߸˼�����ƥ����ƥ�䵫�ǶȲ�ƥ��  ==>ȡ�ְ������Ϣ,���߽Ǹ���˨��Ϣ��׼�Ǹ���˨��Ϣ,�Զ���������ͷ
	//3.�����߸˼�����ƥ��
	int m_iMatchResult;	//ƥ����
	IModel *m_pTower;
	CLDSPlate *m_pSrcPlate;	//��¼ָ��Դ�ְ��ָ��
	CLDSNode *m_pDestNode;	//��Ҫճ���ְ�Ľڵ�
	LINEPARTSET partset;	//�ڵ㵱ǰ����ظ˼�����
	CHashListEx<CSchemaRod> hashLinkRods;
	CHashSet<CSchemaRod*> hashLinkRodsByH;
	void EmptyLinkRods();
public:
	char m_cMateria;
	int m_nThick;
	BYTE m_nPlaneN;			//�ְ幤������>1��ʾ������
	UINT m_idDatumRod;		//��׼�˼��ı�ʶId
	BYTE m_ciProfileSty;	//���������巽ʽ
	BOOL m_bFlexibleDesign;	//�����������
	char ciDatumAxis;		//һ��Ϊ'Y'
	short ciDirectionAxisY;
	short ciDirectionAxisZ;	//0��ʾ��ǰ����֫������;1����ʾ��ǰ����֫������
	ANCHOR_LOCATION origin,huoqu_pt[2];
	struct HUOQU_NORM{
		UINT m_idViceRod;
		UINT m_idCrossRod;
	}huoqu_norm[2];
	struct VECTORS{
		GEPOINT vDatumLenVec;	//��׼�˼����췽��һ��ҲΪY�᷽�򣩣�
		GEPOINT vDatumPlaneNorm;//�ְ��׼ƽ��ķ��߷���һ��ҲΪ��׼�ǸֵĹ���֫���߷���
		GEPOINT vBendPlaneNorm[2];
	}xLocalVec;	//xLocalVecΪԴģ��ֲ�����ϵ�µ�ʸ��,xModelVec��ǰģ������ϵ�µ�ʸ��
	VECTORS UpdateModelCSVectors(GECS acs);	//�ְ�ֲ�װ������ϵ
	int JustifySonAngleResideQuad(GECS acs,CLDSLineAngle* pAngle);
public:
	CClonePlate();
	~CClonePlate();
	CClonePlate(IModel *pTower,CLDSPlate *pPlate=NULL,CLDSNode *pNode=NULL);
	void SetSuperiorTower(IModel *pTower);
	CSchemaRod* AppendSchemaRod(long hLinkRod,DWORD keyid=0);
	bool RemoveSchemaRod(CSchemaRod* pSchemaRod);
	void AnalyzePlateInfo(CLDSPlate *pPlate,IModel *pTower=NULL);	//�����ְ���Ϣ
	int IntelligentMatchPlate(CLDSNode *pNode,PARTSET& DispPartSet,IModel *pTower=NULL);//����ʶ��ǰ�ڵ����Ƿ��������ͬ��ŵĸְ�
	CLDSPlate *CreatePlate(PARTSET& DispPartSet,IModel *pTower=NULL);	//����ƥ���������¸ְ�
	void ToClipperBoard(CBuffer* pClipperBoard,IModel *pTower=NULL);		//���ְ���Ϣ�洢����������
	void FromClipperBoard(CBuffer* pClipperBoard,IModel *pTower=NULL);		//�Ӽ��а��ж�ȡ�ְ���Ϣ
	CLDSLinePart *FindMatchPoleBySrcHandle(long hLinePart);	//ͨ��Դ�˼����������֮��ƥ��ĸ˼�
	CLDSLinePart *FindSrcPoleByMatchHandle(long hMatchPole);//ͨ��ƥ��˼����������֮��ƥ���Դ�˼�
	CSchemaRod* QuerySchemaRod(long hRod,char ciRodTypeSrc0Map1=0);
	bool UpdateTipPosition(long hSchemaRelaRod,CLDSPlate* pNewPlate=NULL);
	//����ڵ�˼����� 0.�����߸˼����׼�˼�fabs(cosa)����  1.����ʱ�뷽������(��ָ�����߸˼����ڸְ�)
	void SortLinePartSet(CLDSLinePart *pDatumLinePart=NULL,int iSortType=0,CLDSPlate *pPlate=NULL);
	void CorrectPlateDesignPara(CLDSPlate *pPlate);	//�������߸˼��������ְ����߸˼�����
	void CorrectPlateLsInfo(CLDSPlate *pPlate);		//���ݸ˼���˨��Ϣ������˨����Լ���˨���
	void UpdateRayLinePartOffset(CLDSPlate *pPlate,int nAutoAdjustTubePos=0);//���¸ְ����߸˼���Ϣ
	void UpdateRayLinePartOddment(CLDSPlate *pPlate);//�������߸˼�����ͷ
};
class CDeepClonePlate : public CClonePlate
{
	struct XmlNode{
		char* sElemName;
		void* pRelaObj;
		XmlNode(char* elemName,void* pObj=NULL){
			sElemName=elemName;
			pRelaObj=pObj;
		}
	};
protected:
	CHashListEx<CSchemaBolt> hashTemplBolts;
	CHashSet<CSchemaBolt*> hashBoltById;
protected:
	int GetSchemaRodId(long hRod,BOOL bMatchRod=FALSE);
#ifdef __LDS_CONTEXT_
	bool ImportTemplChildXmlNode(CMarkup xml,const XmlNode& xmlnode);
#endif
	bool UpdateBoltBySchema(CSchemaBolt* pSchemaBolt,CLDSBolt* pBolt);
	char* GetThroughStr(CSchemaBolt* pSchemaBolt);
	void UpdatePlateDesUcs(CLDSPlate* pNewPlate);
public:
	CDeepClonePlate(IModel *pTower);
	~CDeepClonePlate();
	GECS CalACS(CSchemaRod* pDatumSchemaRod);
	//
	void ExtractPlateInfo(CLDSPlate* pScrPlate);
	CLDSPlate *CreatePlate(PARTSET& dispPartSet,IModel *pTower=NULL);
#ifdef __LDS_CONTEXT_
	//ģ�����ݲ���
	BOOL FromXmlSchemaFile(const char* filename);
	BOOL ToXmlSchemaFile(const char* filename);
	BOOL WriteToClipboard();
	BOOL ReadFromClipboard();
#endif
};