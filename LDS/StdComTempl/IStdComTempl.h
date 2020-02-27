#pragma once
#include "Buffer.h"
#include "ArrayList.h"
#include "LifeObj.h"
#include "Expression.h"
#include "XhBitMap.h"
#include "Hashtable.h"
#include "XhCharString.h"
#include "ComplexId.h"
#include "DesignParamItem.h"
#include "AngleLayout.h"
#include "Catlog.h"
#include "../Global.h"
#include "XhLdsLm.h"	//defined MIRMSG class

union TMPLID{
	struct{
		WORD wiTmplId;
		union{
			struct{BYTE ciSubType,ciGroup;};
			WORD wiGroupType;
		};
	};
	DWORD dwTmplId;
	TMPLID(DWORD _dwTmplId){dwTmplId=_dwTmplId;}
	TMPLID(BYTE _ciGroup,BYTE _ciSubType,WORD _wiTmplId){
		ciGroup  =_ciGroup;
		ciSubType=_ciSubType;
		wiTmplId =_wiTmplId;
	}
	TMPLID(WORD wiGroupType,WORD _wiTmplId){
		BYTE* pcbGroupType=(BYTE*)&wiGroupType;
		ciSubType=*pcbGroupType;
		ciGroup=*(pcbGroupType+1);
		wiTmplId =_wiTmplId;
	}
	operator DWORD(){return *((DWORD*)this);}
};
class IModel;
class CLDSNode;
class CLDSBolt;
class CLDSLinePart;
class CLDSLineAngle;
class CLDSParamPlate;

struct IStdComTempl
{
	struct TEMPL_SOURCE{
		char ciSourceType;	//0.��ʾ��idStdComTempl��ȷ��;1.��ʾ��ָ���ļ���ȷ��
		UINT idStdComTempl;
		UINT dwTemplParam;
		CXhChar200 szTemplFilePath;
		TEMPL_SOURCE(char _ciSourceType=0,UINT _idStdComTempl=1,UINT _dwTemplParam=0)
		{
			ciSourceType=_ciSourceType;
			idStdComTempl=_idStdComTempl;
			dwTemplParam=_dwTemplParam;
		}
	};
	struct IMAGE_INFO{
		char ciSrcType;	//0.��ʾ��Դ�ڽ�����Դ��ͨ����ԴID����ȡ;1.��ʾָ��(���)Ŀ¼�µ�ͼ���ļ�����
		union{
			UINT idResImage;
			char szImgFile[51];	//λ��".\images\stdtmpl_images\"���Ŀ¼��
		};
	};
public:	//�ӿں�������
	virtual UINT GetTmplId()=0;
	virtual const char* GetName()=0;
	virtual IModel* SetBelongModel(IModel* pModel)=0;
	virtual void FromBuffer(BUFFER_IO* pIO,DWORD version=0)=0;
	virtual void ToBuffer(BUFFER_IO* pIO,DWORD version=0)=0;
	virtual bool CloneStdTmplFrom(IStdComTempl* pStdComTmpl)=0;
	virtual bool IsWireStruTmpl(){return false;}
	virtual bool GetSketchImage(IMAGE_INFO* pSketchImage)=0;
	virtual void InitComponentTempl(TEMPL_SOURCE* pTemplSource=NULL)=0;
	virtual bool ClearTmplParams(){return false;}
	virtual bool IsMeetConditions()=0;
	virtual BOOL DeleteMappingNode(long idTemplNode)=0;
	virtual long MappingLdsNodeH(long idTemplNode)=0;
	virtual long SetMappingNode(long idTemplNode,long hLdsNode)=0;
	virtual bool SetMappingNode(long idTemplNode,CLDSNode *pNode)=0;
	virtual long ToLdsHandle(DWORD idObj)=0;
public:
	BYTE get_ciGroupType(){
		TMPLID tmplid=GetTmplId();
		return tmplid.ciGroup;
	}
	__declspec(property(get=get_ciGroupType)) BYTE ciGroupType;
	static const BYTE STDTMPL_FRAMELINE		= 0x00;
	static const BYTE STDTMPL_STRUCTURE		= 0x01;
	static const BYTE STDTMPL_CONNPOINT		= 0x02;
	static const BYTE STDTMPL_NODEPLATE		= 0x03;
	//���߿�����
	static const DWORD STDTMPL_FRAMELINE_HYJ1	= 0x00000001;	//HYJ1
	static const DWORD STDTMPL_FRAMELINE_HX3	= 0x00000002;	//HX3

	static const DWORD STDTMPL_STRU_TUBE_WALKWAY	= 0x01000001;	//�ֹ����ߵ���
	static const DWORD STDTMPL_STRU_TUBE_WORKBENCH	= 0x01000002;	//�ֹ����ϻ���վλƽ̨��
	//static const DWORD STDTMPL_STRU_TUBE_SAFEHOLDER	= 0x01000003;	//�ֹ����ϻ��ΰ�ȫ���ҵ�
	static const DWORD STDTMPL_STRU_HOOF_PLATES		= 0x01050001;	//��ϽǸ�˫�䵥��ţ��������Ϊһ�鵹K��
	static const DWORD STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1	= 0x01020001;	//EB    ������ֱ�������߹ҵ�֧��
	static const DWORD STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM2	= 0x01020002;	//EB����������ֱ�������߹ҵ�֧��
	static const DWORD STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3	= 0x0102000E;	//EB���������������Ǻᵣ���߹ҵ�֧��
	static const DWORD STDTMPL_STRU_EB_EARTH_WIRE_ARM			= 0x01020003;	//EB�����������߹ҵ�֧��
	static const DWORD STDTMPL_STRU_EB_EARTH_WIRE_PLATE			= 0x01020004;	//EB�����������߹ҵ㶥���
	static const DWORD STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM1		= 0x01020005;	//EB������ֱ��������I������(���Ǽ�)
	static const DWORD STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM2		= 0x01020006;	//EB������ֱ��������I������(���Ǻᵣƽ��)
	static const DWORD STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM3		= 0x01020007;	//EB������ֱ��������I������(ˮƽ�ᵣ)
	static const DWORD STDTMPL_STRU_EB_WIRE_V_ARM1				= 0x01020008;	//EB�͵��ҵ�ֱ��������V������(�ᵣ�������Ӵ�)
	static const DWORD STDTMPL_STRU_EB_WIRE_V_ARM2				= 0x01020009;	//EB��˫�ҵ�ֱ��������V������(�ᵣ�������Ӵ�)
	static const DWORD STDTMPL_STRU_EB_WIRE_V_ARM3				= 0x0102000A;	//EB�͵��ҵ�ֱ��������V������(���κᵣ)
	static const DWORD STDTMPL_STRU_EB_WIRE_V_ARM4				= 0x0102000B;	//EB�͵��ҵ�ֱ��������V������(���ߺᵣ���ҵ�)
	static const DWORD STDTMPL_STRU_EB_WIRE_V_ARM5				= 0x0102000C;	//EB�͵��ҵ�ֱ��������V������(����)
	static const DWORD STDTMPL_STRU_EB_WIRE_V_ARM6				= 0x0102000D;	//EB�͵��ҵ�ֱ��������V������(���ർ��V���ڲ�ҵ�)

	//static const DWORD STDTMPL_STRU_GD_WIRE_ARM					= 0x01030001;	//GD���߹ҵ�֧��
	//static const DWORD STDTMPL_STRU_GD_WIRE_ARM1				= 0x01030002;	//GD���߹ҵ�֧�ܣ��̽Ǹ��ڲ��棩
	//static const DWORD STDTMPL_STRU_GD_WIRE_ARM2				= 0x01030003;	//GD���߹ҵ�֧�ܣ��̽Ǹ��ڵ��棩
	static const DWORD STDTMPL_STRU_META_SECT_BTM_1_EB			= 0x01030004;	//ֱ��������ĵ�EB�ҵ㣨�̽Ǹ��ڵ��棩
	static const DWORD STDTMPL_STRU_META_SECT_BTM_2_EB			= 0x01030005;	//ֱ���������˫EB�ҵ㣨�̽Ǹ��ڵ��棩
	static const DWORD STDTMPL_STRU_META_SECT_FRT_1_GD			= 0x01030006;	//����������������浥GD�ҵ㣨�̽Ǹ������棩
	static const DWORD STDTMPL_STRU_META_SECT_BTM_1_GD			= 0x01030007;	//������������ĵ��浥GD�ҵ㣨�̽Ǹ��ڵ��棩
	
	//static const DWORD STDTMPL_STRU_V_OUTER_1_EB				= 0x01030008;	//ֱ����V�����൥EB�ҵ�
	//static const DWORD STDTMPL_STRU_V_OUTER_2_EB				= 0x01030009;	//ֱ����V������˫EB�ҵ�
	static const DWORD STDTMPL_STRU_NZ_FRT_2_GD					= 0x0103000A;	//����������˫GD�ҵ㣨�̽Ǹ������棩
	static const DWORD STDTMPL_STRU_NZ_BTM_2_GD					= 0x0103000B;	//����������˫GD�ҵ㣨�̽Ǹ��ڵ��棩
	static const DWORD STDTMPL_STRU_NZ_BTM_3_GD					= 0x0103000D;	//������������GD�ҵ㣨�̽Ǹ��ڵ��棩
};

struct TMPLOBJID{
	DWORD dwId;
	TMPLOBJID(DWORD _id=0){dwId=_id;}
	TMPLOBJID(KEY4C keyid){dwId=keyid.dwItem;}
	TMPLOBJID(DWORD _id,char _ciMirTransType);
	operator DWORD(){return dwId;}
	operator KEY4C(){return dwId;}
	char set_ciMirTransType(char ciMirType);
	char get_ciMirTransType();
	//�����ַ�����ϣ����Id���Զ�ʶ�����Գƶ���ı�ʶ��0��ʾԭʼ����,'X','Y','Z'�ֱ��ʾ��Ӧ��Գ�
	__declspec(property(put=set_ciMirTransType,get=get_ciMirTransType)) char ciMirTransType;
	bool set_bLongStrId(bool bLongStr);
	bool get_bLongStrId();
	//�����ַ�����ϣIdʱ����ַ���������4(��4)���ַ�Ϊ�̱�ʶ�Զ�ת�����������ϣ����(���ܴ����ص�)
	__declspec(property(put=set_bLongStrId,get=get_bLongStrId)) bool bLongStrId;
	UINT set_uiLongStrSerial(UINT uiSerial);
	UINT get_uiLongStrSerial();
	//�����ַ�����ϣ����Id�ظ�ʱ���Զ��趨�ĸ������
	__declspec(property(put=set_uiLongStrSerial,get=get_uiLongStrSerial)) UINT uiLongStrSerial;
	//UINT set_uiOriginalId(UINT uiOrgId);
	UINT get_uiOriginalId(){return dwId&0x7f7fffff;}
	//���ػ�����
	__declspec(property(get=get_uiOriginalId)) UINT uiOriginalId;
};
union CMPSTID{
	UINT uidRod;	//�߽�˼�����ͨ���Ӹ˼�
	struct{	//��ʶ��ϽǸ��ӽǸֵ�Id
		short sidGroupRod;	//��ϽǸ���ģ���е�����Id
		short sidSerial;	//�ӽǸ�����ϽǸ��е��������(1 based serial)
	};
	CMPSTID(UINT uidCmplxId=0){
		uidRod=uidCmplxId;
	}
	CMPSTID(short sidCmpstId,short siSerialIndex){
		sidGroupRod=sidCmpstId;
		sidSerial=siSerialIndex;
	}
	operator UINT(){return uidRod;}
};
struct ListTmplIdNode{
	TMPLOBJID id;
	ListTmplIdNode* next;
	char label[8];
public:
	ListTmplIdNode(DWORD hashid=0,const char* szLabel=NULL);
	~ListTmplIdNode();
	ListTmplIdNode* AppendListNode(const char* szLabel);
	UINT MappingId(const char* szLabel);
protected:
	bool IsEqual(const char* szLabel);
};
class CTmplIdManager
{
protected:
	CHashList<ListTmplIdNode> idcenter;
	UINT HashId(const char* id_str,int nidstr=0);
public:
	CTmplIdManager();
	UINT AddLabel2Id(const char* label_str);
	UINT Label2Id(const char* label_str);
};

class CStdTemplParaRod;
class CStdTemplParaPlate;
class CStdTemplPart{
public:
	KEY4C keyLabel;	//��ǩ���ַ���ʽ�ı�ʶ�����ڲ�������ģʱ����Id�������б��ڹ�����ţ�
	char cMaterial;	//'S':Q235;'H':Q345;'G':Q390;'T':Q420;'U':Q460
	char sPreferLayer[4];	//��ģ��ָ�����Ż�ͼ����
	BYTE cbMirCreateFlag;		//�Գ�������Ϣ,0x01:X��Գ�;0x02:Y��Գ�;0x04:Z��Գ�
	long m_idPartClsType;		//�ø˼��Ĺ������ͣ���Ǹ֡��ֹܡ��ְ��, CLS_LINEANGLE,CLS_LINETUBE
	char exist_logicexpr[20];	//�ýڵ�����Ƿ���ڵ��߼����ʽ��Ŀǰ�ñ��ʽ��ֻ�ܳ���һ��'<'��'>'��'='��'<='��'>='��'!='�߼�������
public:
	CStdTemplPart(){
 		memset(this,0,sizeof(CStdTemplPart));
		m_idPartClsType=4;//CLS_PART;
		strcpy(exist_logicexpr,"1=0");
	}
	void SetKey(DWORD key){keyLabel=key;}
	UINT get_Id(){return keyLabel.dwItem;}
	__declspec(property(get=get_Id)) UINT Id;
public:
	struct TAG_ROD_INFO : public TAG_INFO<CStdTemplParaRod>
	{
		CLDSLinePart* pModelRod;
		TAG_ROD_INFO(){pModelRod=NULL;}
	};
	struct TAG_PLATE_INFO : public TAG_INFO<CStdTemplParaPlate>
	{
		CLDSParamPlate* pModelPlate;
		TAG_PLATE_INFO(){pModelPlate=NULL;}
	};
};

class CStdTemplParaNode{
public:
	//����������ͣ�attach_type
	static const BYTE INDEPENDENT = 0;	//�������κ������Ľڵ�;
	static const BYTE COORD_X_SPECIFIED = 1;	//�˼���X����ֵ�����;
	static const BYTE COORD_Y_SPECIFIED = 2;	//�˼���Y����ֵ�����;
	static const BYTE COORD_Z_SPECIFIED = 3;	//�˼���Z����ֵ�����;
	static const BYTE INTERSECTION = 4;	//���˼����潻��;
	static const BYTE SCALE = 5;	//�˼��ϵı����ȷֵ�;
	static const BYTE PLANE_XY_SPECIFIED = 6;	//�����󽻵�
	static const BYTE PLANE_YZ_SPECIFIED = 7;	//�����󽻵�
	static const BYTE PLANE_XZ_SPECIFIED = 8;	//�����󽻵�
	static const BYTE OFFSET = 9;	//ƫ�Ƶ�
	//
	struct TAG_NODE_INFO : public TAG_INFO<CStdTemplParaNode>
	{
		CLDSNode* pModelNode;
		TAG_NODE_INFO(){pModelNode=NULL;}
	};
protected:
	TAG_NODE_INFO* pTagInfo;
public:
	CLDSNode* pRelaNode;
	GEPOINT xPosition;
	bool m_bNeedUpdateRelaInfo;
	bool m_bNecessaryNode;		//��׼����еı�Ҫ�ڵ㣨������ģ���б����Ѵ�����Ӧ��ƥ��ڵ�)
	bool m_bOptionExterNode;	//��׼����еķǱ�Ҫ�ڵ㣨����ģ���еĿ�ѡ�ڵ㣬�����ڱ�׼��������ɵĽڵ�)
	char sPreferLayer[3];		//��ģ��ָ�����Ż�ͼ����
	KEY4C keyLabel;				//�ڵ��ģ���ʶ
	void SetKey(DWORD key){keyLabel=key;}
	UINT get_Id(){return keyLabel.dwItem;}
	__declspec(property(get=get_Id)) UINT Id;
	KEY4C keyLabelFatherRod;	//�������˼���ģ���ʶ
	char posx_expression[50];	//
	char posy_expression[50];
	char posz_expression[50];
	char scale_expression[50];
	char offset_expression[50];
	char exist_logicexpr[20];	//�ýڵ�����Ƿ���ڵ��߼����ʽ��Ŀǰ�ñ��ʽ��ֻ�ܳ���һ��'<'��'>'��'='��'<='��'>='��'!='�߼�������
	KEY4C keyRepeatParam;		//>0ʱ��ʾ����ýڵ��ظ�������CBlockParaModel::listParams������ֵ���ظ�����Ϊ"i"����ֵΪ�ظ��ڵ��������Ľڵ��ظ����ߵı�ʶ
	/*
	0-�������κ������Ľڵ�;
	1-�˼���X����ֵ�����;
	2-�˼���Y����ֵ�����;
	3-�˼���Z����ֵ�����;
	4-���˼����潻��;
	5-�˼��ϵı����ȷֵ�;
	6-�����󽻵�
	7-ƫ�Ƶ�
	*/
	char ciPosCalType;	
	union{
		char ciOffsetType;	//0:����;'X':X�������;'Y':Y�������;'Z':Z�������
		char ciMirTransType;//
	};
	double attach_scale;	//��ֵ��ʾС����ʽ�ı���λ��ֵ,��ֵ��ʾ����.��ĸ
	WORD wiOffsetByPrevNode;	//i>ciOffsetByPrevNodeʱ:��ʾ��ǰһƫ�ƽڵ�Ϊ��׼����ƫ��
	TMPLOBJID attachNodeArr[4];
public:
	CStdTemplParaNode();
	virtual ~CStdTemplParaNode();
	//
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	void SetTagInfo(void* pTag){pTagInfo=(TAG_NODE_INFO*)pTag;}
	TAG_NODE_INFO* TagInfo(){return pTagInfo;}
};
struct BOLT_ORIGIN{
	//0.�˼���ǰ�˵�(�Ǹ����);1.�ڵ�ͶӰ����;2.�ڵ�ƽ�ƽ���;3.����һ�˼������߽���;4.������һ����˨;
	//5.��֤����ս���������϶�����ϲ���
	static const BYTE LOCA_RODEND	= 0;
	static const BYTE LOCA_RODNODE	= 1;
	static const BYTE LOCA_AXISINTERS = 3;
	static const BYTE LOCA_KEEPSPACE2ROD = 5;
	BYTE ciPosCalStyle;
	BYTE ciPlaneType;		//0.X-Yƽ��;1.Y-Zƽ��;2.X-Zƽ��
	static const BYTE POS_G		= 0;
	static const BYTE POS_G1	= 1;
	static const BYTE POS_G2	= 2;
	static const BYTE POS_G3	= 3;
	static const BYTE POS_WdS	= 5;	//W deduct(-) S����С��϶��߾ࣩ
	static const BYTE POS_nS	= 6;	//negative(-)S
	static const BYTE POS_WaS	= 7;	//W add(+) S
	BYTE ciStyleG;		//0.g;1.g1;2.g2;3.g3;5.W-S;6.-S;7.W+S;S��ʾ��˨��֫�ߵ���С�߾���϶(ȡ������˨ֱ��)
	BYTE ciStyleCrossG;	//widCrossBoltAngle��ָ�򽻲�Ǹֵ��ľ�
	DWORD widBaseNodeOrBolt;//0:��ʾ��ǰ���Ӱ��׼�ڵ�����������˨��ʶ;
	CMPSTID widBaseAngle;	//��ciStyleG��Ӧ��	  0:��ʾ����Ҫ������;<0xffff��ʾͨ����˨��ָ���Ǹ�ֱ��;0xffff��ʾ�붨λ��׼�Ǹ�ֱ��
	CMPSTID widCrossAngle;	//��ciStyleCrossG��Ӧ,0:��ʾ����Ҫ������;<0xffff��ʾͨ����˨��ָ���Ǹ�ֱ��;0xffff��ʾ�붨λ��׼�Ǹ�ֱ��
	static int CalWidthByStyleG(int niMd,double dfWidth,BYTE ciStyleG=0);
	BOLT_ORIGIN(){widBaseNodeOrBolt=widBaseAngle=widCrossAngle=ciPosCalStyle=ciPlaneType=ciStyleG=ciStyleCrossG=0;}
};
struct BOLT_LOCATION
{
	BOLT_ORIGIN basepoint;
	char ciDatumWing;	//���ڵĽǸ�֫	'X' or 'Y'
	static const char FLOW_StoE	= 0;
	static const char FLOW_EtoS	= 1;
	char ciDirection;	//�����������0.�˼�ʼ->��;1.����
	short siLenOffset,siWingOffset;	//��˨���򼰺���ƫ����
	CLDSBolt* pRelaBolt;
	GEPOINT xTempPos;	//��ʱ���������˨����λ�ã��Ա�ƥ����˨��
	BOLT_LOCATION(const double* pxTempPos = NULL) {
		siLenOffset = siWingOffset = ciDatumWing = 0;
		ciDirection = FLOW_StoE;
		pRelaBolt = NULL;
		xTempPos = GEPOINT(pxTempPos);
	}
	bool TestCalBoltPosition(CLDSLineAngle* pDatumAngle,CLDSNode* pBaseNode=NULL);
	void WriteParamToBolt(CLDSBolt* pDestBolt,CLDSLineAngle* pDatumAngle,CLDSNode* pBaseNode=NULL);
};
class CLDSLineAngle;
struct BOLT_LAYOUT{
	BOLT_ORIGIN basepoint;
	BYTE ciBoldMd;			//������˨��ֱ��
	BYTE cnCount;			//���ոò���ģʽ���õ���˨��,0.��ʾʣ���������˨
	BYTE ciRows;			//0.�Զ��ж�����;1.����;2.˫��
	BYTE cnMaxSingularLineBolts;//�����������ʱ�����Ų��������˨��
	CMPSTID idDirectConnectToAngle1, idDirectConnectToAngle2;	//ֱ�����ӵ�����Ǹ�
	static const BYTE LAYOUT_ONESIDE  = 0;
	static const BYTE LAYOUT_DUALSIDE = 1;
	static BYTE MAX_SINGULARLINE_BOLTS;
	BYTE ciLayoutStyle;		//0.���಼��;1.˫��ԳƲ���;2.��ͷ��ʽ����֫���浥����
	bool blPreferBerSide;	//���߲����Ȳ���
	char ciDatumWing;		//���ڵĽǸ�֫:'X' or 'Y'
	char ciTipS0E1;	//��ǰ���Ӷ�,0.��ʾʼ��;1.��ʾ�ն�
	static const char FLOW_StoE	= 0;
	static const char FLOW_EtoS	= 1;
	char ciFlowDirection;	//��˨�����õ�����0.�˼�ʼ->��;1.����
	char szLenOffsetOf1stBolt[8];	//�׿���˨�Զ�λ���������ƫ����,"L0"���಼��ʱĬ��Ϊ�˾�,˫�಼��ʱĬ��Ϊ0��"LE"�˾�,"LS"������˨���,"LD"˫����˨���
public:
	BOLT_LAYOUT(char _ciLayoutStyle=0,char _cnCount=0,char _ciRows=0);
	//��˨����
	char get_ciSafeDirection();
	__declspec(property(get=get_ciSafeDirection)) char ciSafeDirection;
	//������˨
	UINT LayoutBolts(short g[4],IXhList<BOLT_LOCATION>* pListBoltLocations,double fEndSpace,double fBaseSpace,double fDoubleRowSpace);
	UINT LayoutBolts(long g[4],IXhList<BOLT_LOCATION>* pListBoltLocations,double fEndSpace,double fBaseSpace,double fDoubleRowSpace);
	//
	void ToBuffer(BUFFER_IO *pIO, DWORD dwVersion = 0);
	void FromBuffer(BUFFER_IO *pIO, DWORD dwVersion = 0);
};
struct CONNROD_FLAG{
	//0x01�̳й���֫;0x02��ʾ����֫ɸѡ;0x04������ɸѡ;0x08ɸѡ�˼���������;
	//0x10��֤��С��˨��;0x20ɸѡˮƽ��ĸ˼�
	BYTE _cbValue;
public:
	CONNROD_FLAG(BYTE cbConnRodFlag=0x01){_cbValue=cbConnRodFlag;}
	operator BYTE(){return _cbValue;}
public:
	//�Ƿ�̳й���֫��һЩ�������ԣ��Ǹֹ���֫�ɸ���ʵ������任��
	bool set_bInheritWorkWing(bool bFilterVal);
	bool get_bInheritWorkWing(){return (_cbValue&0x01)>0;}
	__declspec(property(put=set_bInheritWorkWing,get=get_bInheritWorkWing)) bool bInheritWorkWing;
	//��ǰ�趨����֫�Ƿ�Ϊɸѡ����
	bool set_bFilterWorkWing(bool bFilterVal);
	bool get_bFilterWorkWing(){return (_cbValue&0x02)>0;}
	__declspec(property(put=set_bFilterWorkWing,get=get_bFilterWorkWing)) \
		bool bFilterWorkWing;	//��ǰ�趨����֫�Ƿ�Ϊɸѡ����
	//��ǰ�Ǹ��������Ƿ�Ϊɸѡ����
	bool set_bFilterInsideAngle(bool bFilterVal);
	bool get_bFilterInsideAngle(){return (_cbValue&0x04)>0;}
	__declspec(property(put=set_bFilterInsideAngle,get=get_bFilterInsideAngle)) bool bFilterInsideAngle;
	//��ǰ�˼����������Ƿ�Ϊɸѡ����
	bool set_bFilterSectType(bool bFilterVal);
	bool get_bFilterSectType() { return (_cbValue & 0x08) > 0; }
	__declspec(property(put = set_bFilterSectType, get = get_bFilterSectType)) bool bFilterSectType;
	//��ǰ�˼�����������˨���Ƿ�Ϊɸѡ����
	bool set_bFilterConnBoltN(bool bFilterVal);
	bool get_bFilterConnBoltN() { return (_cbValue & 0x10) > 0; }
	__declspec(property(put = set_bFilterConnBoltN, get = get_bFilterConnBoltN)) bool bFilterConnBoltN;
	//��ǰ�˼�Ϊˮƽ����Ƿ�Ϊɸѡ����
	bool set_bFilterHoriPrimaryRod(bool bFilterVal);
	bool get_bFilterHoriPrimaryRod() { return (_cbValue & 0x20) > 0; }
	__declspec(property(put = set_bFilterHoriPrimaryRod, get = get_bFilterHoriPrimaryRod)) bool bFilterHoriPrimaryRod;
};
struct SCHEMA_ROD{
	WORD widRod;		//1 based index;
	char ciRodType;		//'A':Across���˼���Խ��׼�ڵ�;'R':Radiation�˼��ӻ�׼�ڵ����߷���
	char szRodSectType[4];	//�˼���������,'L','T','D','X'
	char ciNecessity;	//��Ҫ�ԣ�0.������涨;1.��Ҫ;2.���п���
	bool get_blNecessary() { return ciNecessity==1; }
	__declspec(property(get=get_blNecessary)) bool blNecessary;	//�����ӵ�ģ��ƥ��ʱ��ģ���б����еĸ˼� wjh-2019.8.20
	char ciPlaneNo;		//������ţ�0.�Զ��ж�(�ݲ�֧��);1.����;2.��һ������;3.�ڶ�������
	BYTE cnMinBoltN;	//�������������˨��
	//���ڿ�Խ�Ǹָ˼��ж�֫������Ҫʼ->�շ��򣬴�ʱ��ͼ��X���������������Ϊ׼
	ANGLE_WORKWING ciAngleWorkWing;	//'X','x','Y','y',��дΪ������СдΪ������Ӧ(�ǸֵĲ��÷�ʽ1,3,4,2);<=0��ʾ��ָ��֫�������������֫���߿�
	bool dualSideEdge;	//�Ƿ�ͬʱӰ��ʼĩ����ְ�߽�;
	CONNROD_FLAG xFlag;
	short siMinRayAngle,siMaxRayAngle;	//������ʼ(��С)����ֹ(���)�ǣ������ͼ���Ӿ�����ʱ�������X��ļнǣ��൱��X->��Y����
	//CLDSLinePart* pMapRod;
	long hMapRod;
	struct ANGLE_LOCATION{
		char ciMasterIndex;	//ciRodSetTypeΪ��ϽǸ�ʱ����ֵ��ʾ��׼�ӽǸֵ������ţ�0 base index
		char ciLocationType;	//0.��֫�������;1.����ƽ�����;2.�������ߣ��������ߣ�;3.����ƫ��׼��
		WORD widBendlineAngle;	//���������ݽǸ�
		ANGLE_LOCATION(){widBendlineAngle=ciMasterIndex=ciLocationType=0;}
	}angle;
	SCHEMA_ROD(){
		ciPlaneNo=1;
		hMapRod=0;//pMapRod=NULL;
		dualSideEdge=false;
		cnMinBoltN = 1;
		szRodSectType[0]='L';
		szRodSectType[1]=szRodSectType[2]=szRodSectType[3]=0;
		ciRodType='R';ciNecessity=ciAngleWorkWing=0;widRod=siMinRayAngle=siMaxRayAngle=0;
	}
};
class CStdTemplParaRod : public CStdTemplPart{
protected:
	TAG_ROD_INFO* pTagInfo;
	bool m_bShortAngle;	//ȱ�ٶ˽ڵ�ĸ��Ӷ̽Ǹ�
public:
	char m_ciNecessaryRod;		//��׼����еı�Ҫ�˼���������ģ���б����Ѵ�����Ӧ��ƥ��˼�)
	CLDSLinePart* pRelaRod;
	struct EDGEROD{
		bool bReverseDirection;
		float lenpos;
		CLDSLinePart* pMatchRod;
		EDGEROD(CLDSLinePart* pRod=NULL,float lenpos_x=0,bool reverse=false){
			pMatchRod=pRod;
			lenpos=lenpos_x;
			bReverseDirection=reverse;
		}
	};
	PRESET_ARRAY4<EDGEROD> relaRods;
	bool m_bNeedUpdateRelaInfo;
	bool m_bNeedUpdateNormal;	//���Ѵ��ڵĸ˼��Ƿ���Ҫ���¹������򣨽Ǹ�֫����
	BYTE m_cbNeedUpdatePosition;	//���Ѵ��ڵĸ˼��Ƿ���Ҫ����λ��
	bool set_bNeedUpdatePosition(bool bUpdatePos);
	bool set_bUpdateStartPos(bool bUpdatePos);
	bool get_bUpdateStartPos();
	bool set_bUpdateEndPos(bool bUpdatePos);
	bool get_bUpdateEndPos();
	//�Ƿ��������
	__declspec(property(put=set_bNeedUpdatePosition))	  bool m_bNeedUpdatePosition;
	//����ʼ������
	__declspec(property(put=set_bUpdateStartPos,get=get_bUpdateStartPos))	  bool bUpdateStartPos;
	//�����ն�����
	__declspec(property(put=set_bUpdateEndPos  ,get=get_bUpdateEndPos  ))	  bool bUpdateEndPos;
	char get_ciNecessaryRod();
	char set_ciNecessaryRod(char _ciNecessaryRod);
	__declspec(property(put=set_ciNecessaryRod  ,get=get_ciNecessaryRod  ))	  char ciNecessaryRod;	//false(0):�Ǳ���˼�;1:���йؼ��˼�;2:����(ȱ��ʱ�Զ����)
	bool get_blNecessaryRod() { return m_ciNecessaryRod>0; }
	bool set_blNecessaryRod(bool _blNecessaryRod) { 
		m_ciNecessaryRod=_blNecessaryRod?0x01:0; 
		return m_ciNecessaryRod!=0;
	}
	__declspec(property(put=set_blNecessaryRod  ,get=get_blNecessaryRod  ))	  bool m_bNecessaryRod;	//false(0):�Ǳ���˼�;1:���йؼ��˼�;2:����(ȱ��ʱ�Զ����)
	bool get_bShortAngle();
	bool set_bShortAngle(bool _bShortAngle);
	__declspec(property(put=set_bShortAngle  ,get=get_bShortAngle  ))	  bool blShortAngle;
	GEPOINT xStartPoint,xEndPoint,xMidPoint;
	GEPOINT vLocalNearVecS2E;	//�˼�ʼ->������Ľ���ֵ, �ڴ���ģ�幤��(�ֲ�)����ϵ��
	GEPOINT vVerifyLenStdVec;	//��������֤�˼�ʼĩ���Ƿ���ģ�嶨���෴���
	WORD wiStartOddCalStyle,wiEndOddCalStyle;	//0.��ʾ������ײ���ո˼�����;1.������˨λ�ü���;2.��ʾ����������ͷ
	short siMinRayAngle,siMaxRayAngle;	//������ʼ(��С)����ֹ(���)�ǣ������ͼ���Ӿ�����ʱ�������X��ļнǣ��൱��X->��Y����
	union{
		COORD3D eyeSideNorm;	//���ڽǸ����۹ۿ���ķ��߷���
		COORD3D pickWorkPlane;	//����ȷ���ֹܻ�׼�����ڹ������ʰȡ�� wjh-2015.9.1
	};
	BOOL m_bMajorMat;		//����
	bool bReverseStartEnd;	//�����ģ��ƥ��ʱʵʱ��ֵ�ı���������Ϊģ��Ĺ������Դ洢
	bool bReverseWingX_Y;	//
	bool bReverseMasterAngle;//��ϽǸ�ʱ��ģ��Ԥ����ϽǸֵ����Ǹ������ʵ�ʵߵ�
	BYTE cnLiveShareBoltN;	//���Ӱ���ƹ����е�ʵʱ���ù�����˨��������Ϊģ������һ���ִ洢�� wjh-2019.6.13
public:
	CStdTemplParaRod();
	virtual ~CStdTemplParaRod();
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual bool IsAngleObject(){return false;}
	void SetTagInfo(void* pTag){pTagInfo=(TAG_ROD_INFO*)pTag;}
	TAG_ROD_INFO* TagInfo(){return pTagInfo;}
	KEY4C keyStartNode,keyEndNode,keyMidNode;	//��ͬ��idStartNode,idEndNode;
	//ʼ�˽ڵ��ֵ
	TMPLOBJID get_idStartNode(){return keyStartNode.dwItem;}
	TMPLOBJID set_idStartNode(UINT uidNode){return keyStartNode.dwItem=uidNode;}
	__declspec(property(put=set_idStartNode,get=get_idStartNode)) TMPLOBJID idStartNode;
	//�ն˽ڵ��ֵ
	TMPLOBJID get_idEndNode(){return keyEndNode.dwItem;}
	TMPLOBJID set_idEndNode(UINT uidNode){return keyEndNode.dwItem=uidNode;}
	__declspec(property(put=set_idEndNode,get=get_idEndNode))	  TMPLOBJID idEndNode;
	//�м�ڵ��ֵ
	TMPLOBJID get_idMidNode(){return keyMidNode.dwItem;}
	TMPLOBJID set_idMidNode(UINT uidNode){return keyMidNode.dwItem=uidNode;}
	__declspec(property(put=set_idMidNode,get=get_idMidNode))	  TMPLOBJID idMidNode;
	CXhChar16 m_sGuige;
	struct NORMAL{
		char ciNormalCalStyle;		//�Ǹֳ�����㷽ʽ:0.���ֲ��䣨������֫����;1.�Ǹ�֫����2.�˼������淨��;//3.������������;4.������Է���
		char ciNormalDirection;		//���ƹ������ߣ�����֫������ӽ��������ᣩX��Y��Z������Сд��ʾ���᷽��,0��ʾ�Զ��ж�
		bool blSyncOtherAngleLeg;	//90�ȸ��½Ǹ���һ֫
		ANGLE_LAYOUT xAngleLayout;	//'X','x','Y','y',��дΪ������СдΪ������Ӧ(�ǸֵĲ��÷�ʽ1,3,4,2);<=0��ʾ��ָ��֫�������������֫���߿�
		UINT uidRefRodOfWorkWing;	//����֫���ո˼�
		NORMAL() { memset(this,0,sizeof(NORMAL)); ciNormalCalStyle = 2; blSyncOtherAngleLeg=true; }
	}normal;
	//�˼���������
	char ciSectFlag;	//�������͵�4λ��ʾ�������ͣ���4λ��ʾ��ϽǸ�ʱ��׼�ӽǸ�������
	char get_ciSectStyle();
	char set_ciSectStyle(char ciStyle);
	//0.��ʾ����ԭ�����ͣ�0='L';1='D';2='T';3='X'��ʾָ���������
	__declspec(property(put=set_ciSectStyle,get=get_ciSectStyle)) char ciSectStyle;
	//��׼�ӽǸֵ�������,ֻ���ڸ˼�Ϊ��ϽǸ�ʱ����Ч
	char get_ciMasterIndex();
	char set_ciMasterIndex(char _ciMasterIndex);
	__declspec(property(put=set_ciMasterIndex,get=get_ciMasterIndex)) char ciMasterIndex;
};
struct GROUPANGLE_PROP{
	BYTE _cbValue;	//0x02,������ϽǸ�����ͷ��(ͬ)������Ϊ�첽��0x03,����Ǹ����ӽǸ�����ͷͬ��
	GROUPANGLE_PROP(BYTE cbState=0){_cbValue=cbState;}
	operator BYTE(){return _cbValue;}
	bool get_bUpdateOddSynState();
	bool set_bUpdateOddSynState(bool bSynState);
	__declspec(property(get=get_bUpdateOddSynState)) \
		bool bUpdateOddSynState;	//����������ϽǸ���ʵ��Ǹּ��ͬ����ϵ����
	bool set_bSynAnglesOdd(bool bSynState);
	bool get_bSynAnglesOdd();
	__declspec(property(put=set_bSynAnglesOdd,get=get_bSynAnglesOdd)) \
		bool bSynAnglesOdd;	//����������ϽǸ���ʵ��Ǹּ��ͬ����ϵ����
	bool set_bAsynAnglesOdd(bool bAsynState);
	bool get_bAsynAnglesOdd();
	__declspec(property(put=set_bAsynAnglesOdd,get=get_bAsynAnglesOdd)) \
		bool bAsynAnglesOdd;	//����������ϽǸ���ʵ��Ǹּ���첽��ϵ����
	BYTE set_ciMasterIndex(BYTE ciIndex);
	BYTE get_ciMasterIndex();
	__declspec(property(put=set_ciMasterIndex,get=get_ciMasterIndex)) \
		BYTE ciMasterIndex;	//��������ϽǸ�ͬ֫���ʵ���ӽǸ�������
};
class CStdTemplParaSlot : public CStdTemplParaRod{
public:
	WORD wiAttachTube;			//���������ֹܼ���۸ַ���ƫ������hAttachTube>0x20��ʾ�ò۸�Ϊ�Ŷ�����
	double fLen;				//�������볤�ȼ���׼��λ�ü���۸�ʼ�ն�λ��
	struct COLUMN_INTERS{		//�����
		COORD3D sect_norm;		//���淨��
		WORD wiAttachNode;		//����Բ�����ڽڵ�
		double len_offset;		//Բ���������ƫ����
		char ciNormalStyle;		//0.ˮƽ��;1.�ֹܴ�ֱ����
		char ciRotFromEdgeStyle;//��ת��λ�ǵ���ʼ�߶�������0.ray_vec����;1.�ֹܻ�׼��;2.�ֹܺ�����
		COORD3D ray_vec;		//���߷���
		double ray_angle;		//����ƫ�ƽǶ�
	}datum_pos;
public:
	CStdTemplParaSlot();
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
};
class CStdTemplParaAngle : public CStdTemplParaRod{
public:
	//----VVVV----------------------------------------
	//ʼ(ĩ)�˽ڵ�Ϊ0ʱ�ĽǸ�Ϊ�����ĽǸ֣������ĽǸֵĶ�λȡ���ڻ�׼�����Ǹ�
	struct ATTACHANGLE{
		DWORD idBaseAngle;		//�̽ǸֵĶ�λ��׼�Ǹ�
		DWORD idBaseNode;		//�̽Ǹֵ������������λ�ڵ�
		DWORD idEndTipBaseAngle;//�̽Ǹֵ��ն˶�λ��׼�Ǹ֣�Ϊ��ʱȡֵidBaseAngle)
		DWORD idEndTipBaseNode;	//����ϳ��̽Ǹֵ��ն˶�����λ�ڵ㣨Ϊ��ʱȡֵidBaseNode)
		char ciLayer;	//����λ�ò㼶��0.��ʾ�붨λ�Ǹֽ�����Խ���ʾԽ���
		char ciDatumAngleWing;	//�ְ�Ķ�λ��׼�ǸֵĹ���֫��'X','Y','I'(�ڰ���),'O'(�����)
		char ciAssistWing;		//�̽Ǹֵķǹ���֫�������:0.��ʾ��ֱ����֫;'X'.��λ�Ǹֵ�X֫;'Y'.��λ�Ǹֵ�Y֫
		char ciNormalOffset;	//0.��ʾ�Զ��ж�
		char ciAngleLayout;		//������ʽ��0:����˳��;1:������;2.��������;3.��������;4.��� 5.����Ǹ�;
		char get_ciFixWorkWing() const;
		__declspec(property(get=get_ciFixWorkWing)) char ciFixWorkWing;	//���������׼�Ǹֵ������ϵĽǸֹ���֫
		char ciLenDirection;	//�̽Ǹ�ʼ->�ս��Ʒ�����ӽ��������ᣩX��Y��Z����Сд��ʾ���᷽������ZK14�еİ����޷�ͨ��ciAngleWorkWing�ж�֫����ʱ�� wjh-2019.11.10
		short siLenStartTipPos,siLenEndTipPos;	//Ĭ��ƫ�������˳�����ȼ�Ϊ:���߷���>ʼ->�շ���
		/*WORD width,width2,thick;	//����֫����һ֫��֫��
		struct HOLE{
			char ciResideWing;		//'X'��'Y'
			char ciLocationMode;	//0.�̽Ǹ��е�ʼ->�շ���ƫ��;'S':��ʼ���濪ʼ���ն�ƫ��;'E'���ն��濪ʼ��ʼ��ƫ��
			short siLenOffset;
			char m_ciHoleType;		//0.��ͨ������˨��1.���߹ҿף�2.���߹ҿף�3.���߹ҿף�4.�ӵؿף�5.ʩ���ף��Ƿ��Ϊ�����ϵĴ��ף���ҿס�ʩ���ס��ӵؿ׵ȣ�
			float sfHoleDiameter;
		};
		PRESET_ARRAY4<HOLE> holes;	//���߿�
		*/
		ATTACHANGLE();
		//void CloneFrom(ATTACHANGLE* pFrom);
	}attachangle;
	//����ʼĩ�˽ڵ�ĸ˼��Ǹ�
	struct POS{
		BYTE cTouchLine;			//0.X֫����;1.Y֫����;2.����;3.0xff�Զ�ʶ�����ڲ����ж������˼���ǰ֫�����
		BYTE cTouchLineType;		//0.����;1.������;2.ƫ��׼��;3.���Ƶ�
		BYTE cFaceOffsetDatumLine;  //ƽ��ģʽ�µ�����ƽ�ƻ�׼��, 0:����;'X':X֫����;'Y':Y֫����
		short siDatumLenOffset;		//�ش�ӻ�׼�˼�ʼ->�����췽���ƫ����
		GEPOINT face_offset_norm;	//ƽ���淨��
		UINT uidTouchLineRod;	//��������˼���cTouchLine���ʹ�ã�һ��Ϊ1�Ż�׼�˼� wjh-2019.6.12
		UINT uidBendByRod;	//�����������˼�,0��ʾĬ��Ϊ�˽ڵ㸸�˼�
		BYTE coordCtrlType;	//0:X-Z����;1:Y-Z����;2.X-Y����(����spatialOperationStyle=15ʱ��Ч)3.�˵�λ�ڽڵ�ָ������(����������)4.�˵�λ�ڽڵ�ƽ�ƽ���(����������)
		union{
			BYTE cMainCoordCtrlWing;//�Ǹ�����֫��'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
			BYTE ciSectionPlane;	//0.�˵�λ�ڽڵ�ͶӰ����;'X'.�˵�λ�ڽڵ��Y-Z����;'Y'.�˵�λ�ڽڵ��X-Z����;'Z'.�˵�λ�ڽڵ��X-Y����
		};
		CXhChar50 szXorCtrlWingOffset;//[4];//WING_OFFSET_STYLE XOrCtrlWing ;�Ǹ���Ҫ����֫(�����������������֫)
		CXhChar50 szYorOthrWingOffset;//[4];//WING_OFFSET_STYLE YOrOtherWing;�Ǹָ�������֫(����һ�����������֫)
		POS() {
			memset(this, 0, sizeof(POS));
			cTouchLine = 0xff; 
			uidTouchLineRod = 1;
		}
	}desStart,desEnd;
public://Ԥ���Ǹֵ�ǰ����֫
	char get_ciAngleWorkWing() {
		return normal.xAngleLayout.ciAngleWorkWing;
	}
	char set_ciAngleWorkWing(char ciAngleWorkWing) {
		return normal.xAngleLayout.ciAngleWorkWing=ciAngleWorkWing;
	}
	__declspec(property(put=set_ciAngleWorkWing,get=get_ciAngleWorkWing)) \
		char ciAngleWorkWing;	//'X','x','Y','y',��дΪ������СдΪ������Ӧ(�ǸֵĲ��÷�ʽ1,3,4,2);<=0��ʾ��ָ��֫����
	char get_ciAngleLayout() {
		return normal.xAngleLayout.ciLayoutStyle;
	}
	char set_ciAngleLayout(char ciLayoutStyle) {
		return normal.xAngleLayout.ciLayoutStyle=ciLayoutStyle;
	}
	__declspec(property(put=set_ciAngleLayout,get=get_ciAngleLayout)) \
		char ciAngleLayout;	//�ǸֵĲ��÷�ʽ1,2,3,4;<=0��ʾ��ָ��֫����
	CONNROD_FLAG xFlag;
	short siMinRayDegAngle,siMaxRayDegAngle;	//�ø˼���X����С(��)���߼нǷ�Χ����=0����ʾ��������
	GROUPANGLE_PROP cbGroupAngle;	//��ϽǸ�����ͷ��(ͬ)�����Ը���״̬
	WORD wiGroupAngleSpace;
	bool dualSideEdge;	//�Ƿ�ͬʱӰ��ʼĩ����ְ�߽�
	BYTE cnMinBoltN;	//����������˨��
	BYTE cnMaxSingularLineBolts;	//�����������ʱ�����Ų��������˨��
	//---------------------------------------------
	PRESET_ARRAY4<BOLT_LAYOUT> m_layoutBoltArr;	//��˨���ò���
public:
	CStdTemplParaAngle();
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual bool IsAngleObject(){return true;}
};
class CStdTemplParaBolt{
protected:
	UINT id;
	long m_idPartClsType;	//�ø˼��Ĺ������ͣ���Ǹ֡��ֹܡ��ְ��, CLS_LINEANGLE,CLS_LINETUBE
public:
	long m_hBoltFamily;	//������˨���ϵ��
	static const BYTE HOLE_CONNECT_BOLT	= 0;//0.������˨��
	static const BYTE HOLE_CONDUCT_WIRE	= 1;//1.���߹��߿�;
	static const BYTE HOLE_EARTH_WIRE	= 2;//2.���߹��߿�;
	static const BYTE HOLE_JUMPER_WIRE	= 3;//3.���߹��߿�;
	static const BYTE HOLE_GROUND_DIVERSION	= 4;//4.�ӵص�����;
	static const BYTE HOLE_ASSEMBLY		= 5;//5.װ���;
	static const BYTE HOLE_BRAND		= 6;//6.���ƿ�;
	static const BYTE HOLE_WATER		= 7;//7.������
	char m_ciHoleType;	//0.��ͨ������˨��1.���߹ҿף�2.���߹ҿף�3.���߹ҿף�4.�ӵؿף�5.ʩ���ף��Ƿ��Ϊ�����ϵĴ��ף���ҿס�ʩ���ס��ӵؿ׵ȣ�
	WORD m_wiDiameter;	//��˨ֱ��������ֱ����,mm
	float m_fHoleIncrement;	//�׾�����˨ֱ����ʵ������,mm
	int m_iWireGroup;		//�����Ĺҵ���
	BYTE cbMirCreateFlag;	//�Գ�������Ϣ,0x01:X��Գ�;0x02:Y��Գ�;0x04:Z��Գ�
	struct ORIGIN{
		char ciDatumPointCalStyle;	//0.�����ϵľֲ�����;1.�Ǹ����;2.�Ǹ���ָ���ڵ����;3.���߽���;4.��һ��˨
		UINT idBaseNode;	//��λ�ڵ��ʶ
		UINT idBasePart;	//ָ��˼�ʱ����ʾ����ƫ�Ƶ������׼�˼�
		UINT idCrossRod;	//���߽���ʱ����һ���ո˼�
		UINT idWorkSonJg;	//TODO:��ϽǸ��еĹ����ӽǸ�(δ�����) wjh-2019.9.3
		char ciDatumWing;	//��׼����Ϊ�Ǹ�ʱ����˨���ڽǸ�֫��0.X֫;1.Y֫
		char ciLenDirect;	//����ƫ��ʱ��ƫ��������0.ʼ->��;1.��->ʼ
		char szWingG[4];	//"g","g1","g2","w-30","40"�ȸ�ʽ
		char szCrossWingG[4];	//TODO:δ����� wjh-2019.9.3
		short siLenOffset;	//����ƫ����,mm
		char szZvOffset;	//��˨����ƫ�������ʽ
		//
		ORIGIN(){ memset(this, 0, sizeof(ORIGIN)); }
	}org;
	struct NORMAL{
		char ciNormalCalStyle;	//��˨������㷽ʽ:0.ָ��;1.�Ǹ�֫����2.�˼������淨��;3.������������;4.������Է���
		char ciResidePlane;		//�����ʱ����˨��������ƽ��:0.����;>=1.��Ӧ������
		char ciNormalDirect;	//��˨��������0.��������
		WORD widBasePart;		//0.��ʾ��org.widBasePart
		//
		NORMAL() { memset(this, 0, sizeof(NORMAL)); ciNormalCalStyle = 1; }
	}normal;
	struct THROUGH_ITEM{
		static const int PART_THROUGH	 =1;	//��������
		static const int THICK_THROUGH	 =2;	//��������ָ�����
		char cItemType;	//0.FinalKey;1.���׹���Idֵ;2.���ֵ;
		char cPartType;	//0.����;1.�Ǹ�;2.�ְ�
		WORD val;
		//
		THROUGH_ITEM() {cItemType = 2; val = 0; cPartType = 0;}
	}through[5];	//���������5������
public:
	CStdTemplParaBolt();
	~CStdTemplParaBolt();
	//��˨��ֵ
	void SetKey(DWORD key){id=key;}
	UINT get_Id(){return id;}
	__declspec(property(get=get_Id)) UINT Id;
	//�׾�
	double get_dfHoleDia();
	double set_dfHoleDia(double diameter);
	__declspec(property(put=set_dfHoleDia,get=get_dfHoleDia)) double dfHoleDia;
	//
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	static int CalWingOffDist(int niMd, double dfWingW, char* szWingG);
};
struct CONNECT_ROD{
	CMPSTID idRod;
	short get_sidRod(){return idRod.sidGroupRod;}
	short set_sidRod(short sid){return idRod.sidGroupRod=sid;}
	__declspec(property(put=set_sidRod,get=get_sidRod)) short sidRod;
	char ciCmpstSerial;	//0.��ʾ����Ǹ�;>=1.��ʾ��Ӧ���(��1)���ӽǸ�
	char ciNecessity;	//��Ҫ�ԣ�0.������涨;1.��Ҫ;2.���п���
	bool blCheckAngleConnWing;	//ǿ���Լ��Ǹֹ���֫�Ƿ���ȷ
	char ciCurrTipSideS0E1;	//0.ʼ��Ϊ��ǰ���Ӷ�;1.�ն˱ߵ�ǰ���Ӷ�;2.�м�����
	char ciConnectWing;	//0��ʾ�Զ��ж�����֫; 'X' or 'Y',��д��ʾʼ������Сд��ʾ�ն�����
	char ciPlaneNo;		//���:1,2,3
	bool dualSideEdge;	//�Ƿ�ͬʱӰ��ʼĩ����ְ�߽�
	BYTE cnMinShareBoltN;	//���ٵ�ֱ���ӹ�����˨����

	static const BYTE SPACE_UDF	   =0;	//ָ����ֵ�߾�
	static const BYTE SPACE_BOLTEND=1;	//��׼��˨�˾�
	static const BYTE SPACE_TOEDGE =2;	//���쵽��Ե
	static const BYTE SPACE_TOBER  =3;	//���쵽���߱�Ե-֫���
	//static const BYTE SPACE_UDF	  =0;	//ָ����ֵ�߾�
	//static const BYTE SPACE_BOLTEND =1;	//��׼��˨�˾�
	static const BYTE SPACE_ALIGN	  =2;	//��׼��˨�˾�+�����˨��ࣨ������֫�������)
	ANGLE_SPACE_FLAG cbSpaceFlag;		//���߲�߾�ռ��1,2��λ;֫���߾�ռ��3,4��λ;�˾�ռ��5,6��λ
	bool bEndLjJg;			//��ͷֱ���Ǹ�
	char ciOddCalStyle;		//-1.��������;0.��ʾ������ײ���ո˼�����;1.������˨λ�ü���;2.��ʾ����������ͷ;3.�ӱ�׼��ͷ��ʼ������ײԭ������ͷ�����������
	union{
		short siSpecOddment;	//ָ������ͷֵ��һ�������һЩ����ط�ָ��-5��-10��֤�Ǹּ�϶ wjh-2019.8.23
		short sidOddRefRod[2];	//���˾����ʱ��ײ���ݽǸ�
	};
	struct DYNROD{
		short sidStartNode,sidEndNode;	//��̬�˽ڵ㶨��˼�
		short siRayMinDegAngle,siRayMaxDegAngle;	//widEndNode=0ʱ��ʾָ����Χ�������߸˼�
	}dynaRod;
	//double end_space,wing_space,ber_space;	//���������ʱ�Ĳ���
	//��˨������Ϣ
	char ciBoltNCalMode;	//-1:�Ǹֶ�ͷ˫����˨��;0.�Ǹֶ�ͷ������˨��;1.��Ϊ��׼�Ǹ�ʱ�������߽Ǹ����¼�����˨��;
	bool get_blCalBoltNByRayRodsOnBaseRod() { return ciBoltNCalMode>0; }	//��Ϊ��׼�Ǹ�ʱ�������߽Ǹ����¼�����˨��
	__declspec(property(get=get_blCalBoltNByRayRodsOnBaseRod)) bool blCalBoltNByRayRodsOnBaseRod;
	BYTE cnMinBoltN;	//����������˨��
	BYTE cnMaxSingularLineBolts;//�����������ʱ�����Ų��������˨��
	PRESET_ARRAY4<BOLT_LAYOUT> layoutbolts;

public:
	CONNECT_ROD(short idRod=0);
	void InitTipInfo(char ciTipS0E1Else2,char ciOddStyle=0,short sidOddRefRod1=1,short sidOddRefRod2=0);
	void InitTipSpecOddment(char ciTipS0E1Else2,short siSpecifyOddment=0);
	void InitBasicInfo(int idRod,bool bNecessity,BYTE ciFaceNo,int nMinBoltN,bool bDualSideEdge = false);
	void ToBuffer(BUFFER_IO *pIO, DWORD dwVersion = 0);
	void FromBuffer(BUFFER_IO *pIO, DWORD dwVersion = 0);
};
class CStdTemplParaPlate : public CStdTemplPart{
protected:
	TAG_PLATE_INFO* pTagInfo;
public:
	//�ְ�����
	static const WORD TYPE_D_TOPPLATE	=1;	//�˳Ŷ���
	static const WORD TYPE_D_ENDPLATE	=2;	//�˳Ŷ˰�
	static const WORD TYPE_D_CHAPLATE	=3;	//�˳Ų��
	static const WORD TYPE_D_LINKPLATE	=4;	//�˳�����
	static const WORD TYPE_L_NODEPLATE	=5;	//��������
	static const WORD TYPE_L_TOPPLATE	=6;	//��������
	static const WORD TYPE_L_JIAPLATE	=7;	//�����а�
	static const WORD TYPE_Q_TOPPLATE	=8;	//��������
	static const WORD TYPE_Q_BASEPLATE	=9;	//��������
	static const WORD TYPE_Q_RIBPLATE	=10;//�����߰�
	static const WORD TYPE_W_TUBECIRPLATE=13;	//�ְ�֧���ߵ��Ļ���Ӿ���
	static const WORD TYPE_W_CIRBOLTPLATE=14;	//�Ǹ�֧���ߵ�����˨���Ӱ�
	static const WORD TYPE_LINEANGLERIB_PLATE = 12;	//�Ǹ��߰�
	static const WORD TYPE_CONNECT_PLATE = 100;	//���Ӱ�
	static const WORD TYPE_WASHER_PLATE  = 101;	//��˨���
	static const WORD TYPE_WRAPPER_PLATE	= 102;	//���ְ�
	static const WORD TYPE_RIB_PLATE		= 103;	//��ǿ���߰�
	static const WORD TYPE_ELSE_PLATE	=1000;	//�����
	static const WORD TYPE_VIRTUAL_PLATE=1001;	//����(��)������
	WORD wiType;	//<100������������;100(CONNECT_PLATE).��ʾ��ͨ�ְ�;>100��ʾ�������������
public:
	CDesignLjPara designInfo;
	struct DESIGN_INFO{
		long idBaseRod;
		long idBaseNode;
		long idBasePlate;
		long idAssistPart;
		char ciDatumAngleWing;	//'X','Y�ֱ��ʾX֫��Y֫
		char ciDatumLenAxis;	//'X','Y'
		char ciProfileStyle;	//���������巽ʽ,0:��׼����1:��������2:��������3:�Զ�������
		struct BENDPLANE{
			char ciBendMode;	//0.ָ�������淨��;1.ָ��ָ����Ƕ�;2.ָ���Ǹ�֫����;3.���˼����淨��
			char ciBendEdgeWing;	//'X','x','Y','y':��д��ʾ�����߲࣬Сд��֫���
			WORD widAngleBendBy;	//��һ��������������߶�λ��׼�Ǹ֣���ְ巨���ڲ���Ϊ���������Ϊ�����ȣ�0��ʾƽ����ȷ��������
			BYTE biSelfCalBendline;	//0:ָ��������|1:�����󽻼���
			short siBendlineSpace;	//��������Ǹ���Ե�߼�ļ�϶
			union NORMAL{
				COORD3D vBendplaneNormal;
				float fBendDegAngle;	//����ǣ��������߻�׼�Ǹ�ʼ->��Ϊ����ʱ����תΪ��(�Ƕ��ƣ�0��ʾ�Զ�����)
				struct{	//2.ָ���Ǹ�֫����
					DWORD idBendPlaneByAngle;
					char cDatumWingXorY;	//0.�Զ��жϹ���֫����;'X':X֫����;'Y':Y֫����
				};
				struct{	//3.���˼����淨��
					WORD widBendPlaneRefRod1,widBendPlaneRefRod2;
					char ciDirectRefAxisXorY;//0.����淨��һ��;'X','Y','Z'�����᷽��Сд��ʾ���᷽��
				};
			}normal;
			BENDPLANE();
		}bends[2];
		BENDPLANE& get_bend() { return bends[0]; }
		BENDPLANE& get_bend2() { return bends[1]; }
		__declspec(property(get=get_bend)) BENDPLANE& bend;
		__declspec(property(get=get_bend2)) BENDPLANE& bend2;
		//char szNormalOffset[8];
	}design;
	struct HOLE{
		BYTE ciResidePlane;
		double x,y;
		float holed;		//�׾�
		//WORD wiDistance;	//�����׼�ǸֵĴ�ֱ����
		//short siUpperOffset;//�ְ�ֲ�Y��������ƫ�ƾ���
		float ciBushTubeDiameter;//�׹��⾶
		float ciBushTubeThick;	//�׹ܱں�
		float ciBushTubeHeight;	//�׹ܸ߶�
	};
	union{
		struct WIREPLATE{
			char ciCurveEdgeMode;	//0.��͹ʽ;1.����ʽ
			char ciEdgeRadius;	//�ҿױ߾��Բ���뾶
			char ciFilletRadius;//�ҿױ��ڰ��Ǵ�������Բ���뾶
		}wireplate;
		struct ANGLEPLATE{
			char ciRibType;	//�Ǹּ�ǿ�߰�����
			char ciLocationMode;	//0.�̽Ǹ��е�ʼ->�շ���ƫ��;'S':��ʼ���濪ʼ���ն�ƫ��;'E'���ն��濪ʼ��ʼ��ƫ��
			short siLenOffset;
			char thick;
			//void Init();
		}anglerib;
	};
	PRESET_ARRAY8<CONNECT_ROD> connrods;
public:
	CXhSimpleList<HOLE> listOnlyHoles;	//����׶���
	long _thick;
	BOOL _bStdPart;	//TRUE:��׼���� FALSE:�Ǳ�׼����
	KEY4C keyAssistPart,keyAssistPart2;
	/*ATOM_LIST<ATTACHPART_ITEM> AttachPart;
	CLDSVector heteroFlConnVec[3];	//���´����ͷ����������˼����ӷ���
	ATOM_LIST<CONNECT_DIRECTION> ConnDirectList;
	ATOM_LIST<DESIGN_PLATE_ITEM>designItemList;*/
	char off_expression[50];	
	int feature;			//��ʱʹ��
public:
	CStdTemplParaPlate();
	~CStdTemplParaPlate();
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	//
	void SetTagInfo(void* pTag){pTagInfo=(TAG_PLATE_INFO*)pTag;}
	TAG_PLATE_INFO* TagInfo(){return pTagInfo;}
};
#include "ExpressionValue.h"
struct XHSTR_LEN8 {static const int BUF_LEN =  9;};
typedef CXhCharTempl<XHSTR_LEN8> CXhChar8;
#include "Catlog.h"
struct STDTEMPL_PARAM_ITEM{
	KEY4C key;			//�ò����ı�ʶ��ֵ���򲻳���4�������ַ�����
	UINT uiCategoryId;	//��λΪ1��uiCategoryId&0x80000000>0��ʾ�ò�������ΪĿ¼��
	CXhChar8 szVarName;
	CXhChar50 sLabel;	//����������ʾ�Ķ����Ʊ�ǩ
	CXhChar100 sNotes;	//�����жԸ������ԵĽ���������
	char exprstr[51];	//���ʽ
	EXPRESSION_VALUE value;
public:
	STDTEMPL_PARAM_ITEM();
	void SetKey(DWORD keyid);
};
//��׼���ģ�嶨����
class CStdComTempl : public IStdComTempl
{
protected:
	GECS ocs;
	UINT m_idStdComTmpl;					//ģ��ı�ʶId
	IModel* m_pBelongModel;
	bool m_bTmplInitialized;				//��ʾ��ģ���Ƿ������
	bool m_bEnableIntelliConnPointDesign;	//�Ƿ񾭹�������֤���Կ���ʽӦ�������������������ı�׼�ڵ��ģ�� wjh-2019.4.23
	bool m_bIntelliRecogRodNodes;			//�Ƿ����Զ�����ʶ��˼��ڵ㹦��(SetMappingNodeʱ����)
	virtual CXhChar50 _InternalGetSketchImgFileSubFolder();
public:
	class MATCH_CONNECT_POINT {
	public:
		UINT idNode;		//ƥ�����ӵ����ڵĻ�׼�ڵ�
		UINT idStdConnPoint;//ƥ��ı�׼���ӵ�ģʽ
		UINT idStdNodePlate;//���������ӵ㴦��Ҫʵ����ƵĽڵ���׼����ģʽ
		UINT idBaseRod, idRayRodOnAxisY;	//���ӵ�Ļ�׼�˼�,Y�������߸˼�
		BYTE cbMirCreateFlag;			//�Գ�������Ϣ,0x01:X��Գ�;0x02:Y��Գ�;0x04:Z��Գ�
		PRESET_ARRAY8<UINT> xarrMapConnRods;//���ӵ��漰�����߸˼���Ϊ��ʱ��ʾ�Զ���ȡ���ӵ����߸˼�
	public:
		MATCH_CONNECT_POINT();
	};
	static CXhChar500 szImgRootPath;
	CXhChar50 name;
	CXhChar50 szSketchImgFile;
	XHBITMAP sketchimg;	//ʾ��ͼ
	//1.��Ʋ�������
	static const BYTE MIR_CREATE_X	= 0x01;
	static const BYTE MIR_CREATE_Y	= 0x02;
	static const BYTE MIR_CREATE_Z	= 0x04;
	static const BYTE MIR_CREATE_ALL= 0x07;
	char m_ciBaseMirType;	//0.�޻�׼�Գ�;'X','Y','Z'ʱ�ֱ��ʾ��id����ӦX��Գ����ݶ���
	char m_ciMirrorMatchMode;	//ʵ�ʽṹ��ģ����ƥ�������Ƿ�Ϊ����Գ�ƥ��
	BYTE m_cbDisableConnPlaneFlag;	//��Ƶ�ǰ������ͬʱ��ֹ���ӵ���������ʶ���ͷ(0x80)
	CHashListEx<CATLOG_ITEM>hashCategoryItems;	//������������ʾ¼�����ʱ��Ŀ¼�ּ�
	CHashListEx<STDTEMPL_PARAM_ITEM>listParams;
	//2.�����б���������
	CHashList<long>hashMatchNodeHid;//��ֵΪ���ģ���е�Id������Ϊ��ӦLDSģ���еĽڵ���
	CHashList<long>hashMatchRodHid;	//��ֵΪ���ģ���е�Id������Ϊ��ӦLDSģ���еĸ˼����
	CHashList<long>hashMatchPlates;	//��ֵΪ���ģ���е�Id������Ϊ��ӦLDSģ���еĸ˼����
	CHashList<long>hashMatchBolts;	//��ֵΪ���ģ���е�Id������Ϊ��ӦLDSģ���е���˨���
	CHashList<long>hashMatchFittings;//��ֵΪ���ģ���е�Id������Ϊ��ӦLDSģ���еĽ�߾��
	//3.�˼���λ����
	//4.ʵ��ģ������
	CHashListEx<CStdTemplParaNode> listParaNodes;
	CSuperHashList<CStdTemplParaRod> listParaRods;
	CHashListEx<CStdTemplParaPlate> listParaPlates;
	CHashListEx<CStdTemplParaBolt> listParaBolts;
	CXhSimpleList<MATCH_CONNECT_POINT> listStdConnPoints;	//�ṹ�����й����ı�׼���ӵ�
protected:
	void InitNodePlateTemp_HD1();
	void InitNodePlateTemp_HD1A();
	void InitNodePlateTemp_HD2();
	void InitNodePlateTemp_HD3();
	void InitNodePlateTemp_HD3_2();
	void InitNodePlateTemp_HD3_G5();
	void InitNodePlateTemp_HD4(bool blShrinkRodTrue2False1=true);
	void InitNodePlateTemp_HD5();
	void InitNodePlateTemp_HD6();
	void InitNodePlateTemp_HD7();
	void InitNodePlateTemp_HD7_T();
	void InitNodePlateTemp_HD8();
	void InitNodePlateTemp_HD8_YS();
	void InitNodePlateTemp_HD9();
	void InitNodePlateTemp_HD9_T();
	void InitNodePlateTemp_HD10();
	void InitNodePlateTemp_HD10_T();
	void InitNodePlateTemp_HD3A();
	void InitNodePlateTemp_HD12();
	void InitNodePlateTemp_HD12X();
	void InitNodePlateTemp_HD13();
	void InitNodePlateTemp_HD14();
	void InitNodePlateTemp_BP1or2(long idStdComTmpl);
	void InitNodePlateTemp_BP3();
	void InitNodePlateTemp_ZK14_K1();
	void InitNodePlateTemp_ZK14_K1_SIDE();
	void InitNodePlateTemp_ZK14_K2();
	void InitNodePlateTemp_ZK14_K3();
	void InitNodePlateTemp_ZK14_K3_SIDE();
	void InitNodePlateTemp_DX3();
	void InitNodePlateTemp_DX3A();
	void InitNodePlateTemp_GX1();
	void InitNodePlateTemp_GX2();
	void InitNodePlateTemp_GX3();
	void InitNodePlateTemp_GX4();
	void InitNodePlateTemp_GX5();
	void InitNodePlateTemp_DM1();
	void InitNodePlateTemp_DM1A();
	void InitNodePlateTemp_DM2();
	void InitNodePlateTemp_DM3();
public:
	CStdComTempl(void);
	virtual ~CStdComTempl(void);
	//
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual bool CloneStdTmplFrom(IStdComTempl* pStdComTmpl);
	static bool IsStdTmplChecked(UINT idStdComTmpl);
	static bool IsStdTmplAuthorized(UINT idStdComTmpl, UINT uiDogKey=0, UINT uiClientSerial= 0);
	virtual bool IsAuthorized(UINT uiDogKey=0, UINT uiClientSerial=0);
	bool get_blEnableIntelliConnPointDesign() const{return m_bEnableIntelliConnPointDesign;}
	__declspec(property(get=get_blEnableIntelliConnPointDesign)) bool blEnableIntelliConnPointDesign;
	bool EnableIntelliRecogRodNodes(bool enabled=true){return m_bIntelliRecogRodNodes=enabled;};
public:	//���ؽӿں���
	virtual bool get_blTmplIntialized(){return this->m_bTmplInitialized;}
	__declspec(property(get=get_blTmplIntialized)) bool blTmplIntialized;
	virtual UINT SetTmplId(UINT idStdComTmpl){return m_idStdComTmpl=idStdComTmpl;}
	virtual UINT GetTmplId(){return m_idStdComTmpl;}
	__declspec(property(put=SetTmplId,get=GetTmplId)) UINT Id;
	virtual const char* GetName(){return name;}
	virtual IModel* SetBelongModel(IModel* pModel){return m_pBelongModel=pModel;}
	virtual bool GetSketchImage(IMAGE_INFO* pSketchImage);
	virtual bool LoadSketchImage();
	virtual void InitComponentTempl(TEMPL_SOURCE* pTemplSource=NULL);
	virtual void InitComponentTempl(UINT idStdTmpl,UINT dwTmplParam=0){
		TEMPL_SOURCE source(0,idStdTmpl,dwTmplParam);
		InitComponentTempl(&source);
	}
	virtual bool ClearTmplParams();
	virtual bool IsMeetConditions();
	virtual bool RecogMatchRod(CStdTemplParaRod* pParaRod);
	virtual int RecogMatchRods(UINT idRecogRod=0);
	virtual BOOL DeleteMappingNode(long idTemplNode){return hashMatchNodeHid.DeleteNode(idTemplNode);}
	virtual long MappingLdsNodeH(long idTemplNode);
	virtual long SetMappingNode(long idTemplNode,long hLdsNode);
	virtual bool SetMappingNode(long idTemplNode,CLDSNode *pNode);
	virtual long ToLdsHandle(DWORD idObj);
	virtual void Empty();

	//��CParaModelNode�Ѿ��йҽӶ���ʱִ��TagInfo(...), ���򴴽��¹ҽ����ݶ���󷵻�
	//CStdTemplParaNode::TAG_NODE_INFO* LoadTagInfo(CStdTemplParaNode* pItem);
	//CStdTemplParaRod::TAG_ROD_INFO*  LoadTagInfo(CStdTemplParaRod*  pItem);
	//CStdTemplParaPlate::TAG_PLATE_INFO* LoadTagInfo(CStdTemplParaPlate* pItem);
};
class CStdComTemplConnPoint : public CStdComTempl
{	//��ͨ�ṹ���ӵ㹹��ģ��
	CXhChar50 description;	//�����ڲ��з���Ա��ʱ��ע���ӵ�����ʱ�� wjh-2018.11.29
protected:
	virtual CXhChar50 _InternalGetSketchImgFileSubFolder() { return "Node\\"; }
public:
	struct MATCHROD_INFO{
		bool matched;
		char ciConnTipS0E1Els2;
		WORD wiRayDegAngle;
		CLDSLinePart* pRod;
		MATCHROD_INFO(CLDSLinePart* _pRod=NULL){
			matched=false;
			wiRayDegAngle=ciConnTipS0E1Els2=0;
			pRod=_pRod;
		}
	};
	struct MAPPING_ROD { 
		long idConnPointRod;			//�����ӵ�ģ���и˼�Id
		union{
			long idNodePlateRod;		//����ƥ��ڵ��ģ��˼�Id
			long idCoupleConnPointRod;	//�������ӵ�ģ����ӳ��ĸ˼�Id
		};
		MAPPING_ROD(long _hConnRod=0,long _hPlateRod=0) {
			idConnPointRod=_hConnRod;
			idNodePlateRod=_hPlateRod;
		}
	};
	class CMatchStdPlate{
	protected:
		UINT _idStdNodePlate;
	public:
		bool blTmpPassedVerify;	//�Ƿ�ͨ���Ϲ���Ч����֤����ʱ������ģ������ wjh-2019.6.12
		UINT get_idStdNodePlate(){return _idStdNodePlate;}
		__declspec(property(get=get_idStdNodePlate)) UINT idStdNodePlate;
	public:
		//������ӦStdNodePlte.connrods,��ֵ��ӦStdConnPoint.SchemaRod.Id��StdNodePlate�е�ParaRod.Id
		PRESET_ARRAY8<long> xarrMapConnRods;
		PRESET_ARRAY4<MAPPING_ROD> xarrReflectMapRods;	//�����Ӱ巴�䵽���ӵ�ĸ˼�ӳ���
		CMatchStdPlate(){_idStdNodePlate=0;}
		void SetKey(UINT key){
			_idStdNodePlate = key; 
			blTmpPassedVerify = true;
		}
		void InitDefaultMatchRods(int count){
			for(int i=0;i<count;i++)
				xarrMapConnRods.Set(i,i+1);
		}
	};
	struct VIEWPLANE {
		char ciBaseAxisXMode;	//0.ʼ->�շ���;1.���߷���;2.���߷����򣬼�SCHEMA_ROD�нǶ�ֵ�Ļ�׼X��(������ʼ�ߣ�
		char ciViewPlaneNormal;	//�۲�ͶӰ�淨��,'X','Y','Z',Сд��ĸ��ʾ������0��ʾ������,'H'��ʾˮƽ����
		short siMinBaseRodAngle, siMaxBaseRodAngle;
		VIEWPLANE() { ciViewPlaneNormal = ciBaseAxisXMode = 0; siMinBaseRodAngle = siMaxBaseRodAngle = 0; }
	}xView;
	struct COUPLE_CONNPOINT{
		UINT idSideStdConnPoint;	//������Ҫƥ��ı�׼���ӵ㣬0��ʾ����Ҫ����ƥ��
		UINT idSideStdNodePlate;	//������Ҫƥ��ı�׼���Ӱ壬0��ʾ����Ҫ����ƥ��
		SIMPLE_ARRAY<MAPPING_ROD,PRESET_OBJS4> xarrShareRods;	//�����Ӱ巴�䵽���ӵ�ĸ˼�ӳ���
		COUPLE_CONNPOINT() { 
			idSideStdConnPoint=idSideStdNodePlate=0;
		}
	}xSideConnPoint;
	//
	WORD widParentRod;	//>0��Ҫָ���ڵ㸸�˼�
	WORD widBaseRod;	//���Ӱ�Ķ�λ�����˼�
	WORD idRayRodOnAxisY;	//���ӵ�Ļ�׼�˼�,Y�������߸˼�
	char ciBaseRodSearchMode;	//0.�Խڵ㸸�˼�Ϊ��׼�˼���1.�Զ����ݹ���ƽ����ͼ������λ��׼�˼�
	char cnConnectPlane;	//����������0��ʾ����Ϊ����ɸѡ����
	bool blSetParentRodOfBaseNode;//�Զ��趨�������Ķ�λ��׼�˼�Ϊ�ڵ�ĸ��˼�
	bool m_bStrictReview;	//����Ӹ˼����ͼ������ϸ��������ӵ�����
	BYTE ciBaseRodConnState;//0.������;1.�м������;2.�νӶ�ĩ������;3.�νӶνضϵ�����;4.���߽ضϵ�����
	CHashListEx<CMatchStdPlate> hashMaybeTmplPlates;	//����Ӱ�䵽�Ľڵ㹹��ģ��
	PRESET_ARRAY8<SCHEMA_ROD> rodarr;	//Ԥ��8���˼�(����׼�˼�)��������һ��widRod=0�˼����� 
protected:
	void InitConnPointTempl_HD1();
	void InitConnPointTempl_HD1A();
	void InitConnPointTempl_HD2();
	void InitConnPointTempl_HD3();
	void InitConnPointTempl_HD3_G5();
	void InitConnPointTempl_HD4();
	void InitConnPointTempl_HD5();
	void InitConnPointTempl_HD6();
	void InitConnPointTempl_HD7();
	void InitConnPointTempl_HD8();
	void InitConnPointTempl_HD9();
	void InitConnPointTempl_HD10();
	void InitConnPointTempl_HD3A();
	void InitConnPointTempl_HD12();
	void InitConnPointTempl_HD12X();
	void InitConnPointTempl_HD13();
	void InitConnPointTempl_HD14();
	void InitConnPointTempl_BP1or2(long idStdComTmpl);
	void InitConnPointTempl_BP3();
	void InitConnPointTempl_ZK14_K1();	//�����������Ĺ��߽ض�
	void InitConnPointTempl_ZK14_K1_SIDE();	//�Ʊ���������K�ڵ���棨���棩
	void InitConnPointTempl_ZK14_K2();	//�������۲��Ͽ�
	void InitConnPointTempl_ZK14_K3();	//�������۱��½ض�
	void InitConnPointTempl_ZK14_K3_SIDE();	//�Ʊ���������K�ڵ���棨˫����£�
	void InitConnPointTempl_DX3();
	void InitConnPointTempl_DX3A();
	void InitConnPointTempl_GX1();
	void InitConnPointTempl_GX2();
	void InitConnPointTempl_GX3();
	void InitConnPointTempl_GX4();
	void InitConnPointTempl_GX5();
	void InitConnPointTempl_DM1();
	void InitConnPointTempl_DM2();
	void InitConnPointTempl_DM3();
public:
	CStdComTemplConnPoint();
	~CStdComTemplConnPoint() { ; }
	//
	void AddConnPointAcsParam();
	virtual bool GetSketchImage(IMAGE_INFO* pSketchImage);
	virtual void InitComponentTempl(TEMPL_SOURCE* pTemplSource=NULL);
	virtual bool ClearTmplParams();
	virtual bool IntelliRecogRayRods();
	virtual bool UpdateMatchStdPlateTmpl(CStdComTempl* pStdPlateTmpl,BYTE ciMirMode=0);
};
//
class CStdComTemplExpression : public CExpression
{
protected:
	CStdComTempl* m_pContextStdTempl;
public:
	GECS acs;	//��׼����ڲ����涯��λ����ϵ����ģ������ϵ�µ���������Ҳ����׼�����װ������ϵ
	virtual EXPRESSION_FUNC* FromFunctionName(const char* func_label);
	CStdComTemplExpression(CStdComTempl* pStdTempl = NULL, GECS* pAcs = NULL);
	void InitExpressionVarList(CStdComTempl* pStdTempl);
	void SetAcs(GECS cs) { acs = cs; }
	CStdComTempl* get_pStdTempl() { return m_pContextStdTempl; }
	__declspec(property(get = get_pStdTempl)) CStdComTempl* pStdTempl;
};
// CStdComTmplLibraryDlg �Ի���
class CStdComTmplLibrary : public IGlobalInitializer
{
	bool m_bTmplLibraryCompiled;
protected:
	CSuperHashList<CStdComTempl> hashStdTmplPool;
	CATLOG_TREEITEM* InsertTreeNodeItem(CATLOG_TREEITEM* pParentItem,UINT idStdComTmpl, const char* szTreeItemName);
	void CompileTmplLibrary();
	void InitStdConnPointTmpls();
public:
	CStdComTmplLibrary();
	bool GlobalInitialze();
	CATLOG_TREEITEM xFrameCatlog,xStruCatlog,xNodeCatlog;
	CStdComTempl* LoadStdComTempl(const char* szStdComTemplId);
	CStdComTempl* LoadStdComTempl(UINT idStdComTempl);
	CStdComTempl* EnumFirstStdFrameTempl();
	CStdComTempl* EnumNextStdFrameTempl();
	CStdComTempl* EnumFirstStdStruTempl();
	CStdComTempl* EnumNextStdStruTempl();
	CStdComTempl* EnumFirstStdNodeTempl(bool bIncConnPoint=true,bool bIncNodePlate=true);
	CStdComTempl* EnumNextStdNodeTempl (bool bIncConnPoint=true,bool bIncNodePlate=true);
};
extern CStdComTmplLibrary globalStdComTmplLibrary;
