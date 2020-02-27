#pragma once

#include "f_ent.h"
#include "list.h"
#include "Variant.h"
#include "..\StdFacePanel.h"
#include ".\IStdComTempl.h"
#include ".\StdComModel.h"

class CArmHeadComTempl;
struct CWireStruComTemplRepository
{
	struct CONDITION{
		long idProp;
		WORD wiLogicOperator;	//'>','=','<',">=","<=","!="
		CVariant varVal;
	};
	int Query(CONDITION* conditions,int count,IXhList<CArmHeadComTempl*> listRslts);
};
//CArmHeadComTempl����Ҫ���ڸ����滮ʱ����ͷ�������εĿ�������
class CArmHeadComTempl : public CStdComTempl
{
protected:
	virtual CXhChar50 _InternalGetSketchImgFileSubFolder() { return ""; }
public:
	struct INERNAL_PANEL{
		UINT id;
		union{
			struct{
				char ciPanelSchema;	//0.������;1.�ڼ���
				BYTE cidPanelType;	//�ڼ����ӱ�ʶ���
			};
			WORD widPanelType;
		};
		char ciSymmetryType;	//�Գ����,'X','Y','Z','A';
		char description[17];	//����
		void SetKey(DWORD key){id=key;}
	};
	struct WIRE_CLEARANCE_POINT{
		GEPOINT xWirePoint;		//���߹ҵ�λ��
		GEPOINT vWireFlowDirect;//��·����
		GEPOINT xRotAxisOrg,vRotAxisVec;//�ҵ���ת����㣬��ת�᷽��
		short siFromSwingDegAngle;	//<=0	ҡ�ڽǵ���ʼֵ
		short siToSwingDegAngle;	//>=0	ҡ�ڽǵ���ֵֹ
		double fMinElecDistance;	//��С������϶��mm
	};
public:
	UINT m_idWireStruTmplType;	//�����ߺᵣ����ͷģ������Id
	CHashListEx<CParaDatumPlane> listParaPlanes;
public:
	CArmHeadComTempl(void);
	virtual ~CArmHeadComTempl(void);
	//
	CVariant GetPropVal(long idProp);
	bool IsAdapt(long idProp,CVariant& varPropVal);
	DESIGN_PARAM_ITEM* EnumFirstProfileParam();	//������һ�����γߴ���Ʋ���
	DESIGN_PARAM_ITEM* EnumNextProfileParam();	//������һ�����γߴ���Ʋ���
	INERNAL_PANEL* EnumFirstStructPanel();
	INERNAL_PANEL* EnumNextStructPanel();
	bool IntelliPlanSubPanels();	//���ܹ滮��Ƹ��ӹ�����
	//������϶У��ӿ�
	WIRE_CLEARANCE_POINT* AddElecClearanceCheckPoint(WIRE_CLEARANCE_POINT* pCheckPoint=NULL);
	WIRE_CLEARANCE_POINT* EnumFirstElecCheckPoint();
	WIRE_CLEARANCE_POINT* EnumNextElecCheckPoint();
	bool IntelliDesignProfileParams();	//���ݵ�����϶���������ͷ�ᵣ�ĳߴ���Ʋ���
	//�ṹ�����ж�����
	BYTE RecogTemplSubTypeId();
public:
	//UCS_STRU rot_ucs;
	//CHashListEx<DESIGN_PARAM_ITEM>listParams;
	//CHashListEx<CParaNode>listParaNodes;
	//CSuperHashList<CParaRod>listParaRods;
	//��ֵ��Ӧ������ӦCStdTemplParaBolt�ı�ʶ����ֵ��Ӧ������Ӧ���������ϵ�һ��ҿ�
	//CHashList<CXhChar100> listWireHoleSet;
public:
	//CParaTemplate();
	//virtual ~CParaTemplate();
	long KeyNodeNum();
	void UpdateNodePos();
	void ToStream(IStream *pStream);
	void FromStream(IStream *pStream);
	bool AssertParamsValid();//���ø�ģ����Ӧ֧�ֵĲ��������Ϸ���У��
public:
	//struct TmplPropListUI{
	//	virtual long InitPropHashtable(long initId=1,long initHashSize=50,long initStatusHashSize=50);	//������һ����Id
	//	virtual long GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	//	virtual void DisplayPropertyList(int overwrite0_append1=0,DWORD dwPropGroup=1);
	//	virtual bool ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt);
	//	virtual bool ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt);
	//	virtual BOOL FinishSelectObjOper(long hPickObj,long idEventProp);
	//	virtual BOOL HelpPromptProperty(CPropTreeItem* pItem);
	//}ui;
public:
	virtual const char* GetName();
	virtual bool IsWireStruTmpl(){return true;}
	virtual bool GetSketchImage(IMAGE_INFO* pSketchImage);
	virtual void InitComponentTempl(TEMPL_SOURCE* pTemplSource=NULL);
	virtual bool IsMeetConditions();
	virtual long MappingLdsNodeH(long idTemplNode);
	virtual long SetMappingNode(long idTemplNode,long hLdsNode);
	virtual bool SetMappingNode(long idTemplNode,CLDSNode *pNode);
};
//////////////////////////////////////////////////////////////////////////
#ifdef __COMMON_PART_INC_
struct MAP_RODnNODE{
	static const BYTE TYPE_NODE = 0;
	static const BYTE TYPE_ROD  = 1;
public:
	BYTE ciNode0Rod1;	//0.ӳ��ڵ�;1.ӳ��˼�
	BYTE ciCondGroup;	//ͬһ��ŵ���������ͬʱ�����Ϊ�������
	long idNodeOrRod;
	union{
		CLDSNode* pNode;
		CLDSLinePart* pRod;
	};
	void Set(long idTmplObj=0,CLDSDbObject* pMapLdsObj=NULL);
	MAP_RODnNODE(long idTmplObj=0,CLDSDbObject* pMapLdsObj=NULL){Set(idTmplObj,pMapLdsObj);}
};
struct WIREPOINT_GROUP {
private:
	BYTE _cnHoleD;
	WORD _wnReservedWidth;
public:
	BYTE ciGroupSerial;			//�ҵ�����
	BYTE ciFittingType;			//�������0.EB�Ұ�|1.GD�Ұ�|2.U����˨
	BYTE cnShaftD;				//��߶���ֱ��,mm
	WORD wnFittingWidth;		//�ҽӽ�߿��
	WORD wiInsulatorStyle;		//�ҽӴ��ͣ��絥(I)����˫��(����)��˫��˫�ҡ�V��
	char szFittingSizeCode[17];	//����ͺ�
	char szUringSizeCode[17];	//EB/GD����µĹҽ�U�λ��ͺ�
	BYTE cnWirePointCount;		//�ҵ�(��)����
	BYTE ciResidePlane;			//0.��ǰ����ƽ�棨���߽Ǹֹ���֫��ְ�ƽ�棩;1.���߽Ǹ���һ֫
	BYTE ciLocateStyle;			//�ҵ㶨λ����0.�ӹ��߽Ǹ��е�������ԳƲ��ã������������붨λ;1.�ҵ��븽���ᵣ�Ǹּ�ĸ����϶
	union {
		WORD wiSpaceL;		//�ҿ׼俪������
		WORD wiClearance;	//�����϶
	};
	float sfMasterSwingAngle;	//����߰ڶ��ǣ���
	float sfSlaveUrSwingAngle;	//�ҽ�U�λ��ڶ��ǣ���
public:
	WIREPOINT_GROUP();
	//Ԥ����װ�׾�,mm��һ��cnHoleD=cnShaftD+2
	BYTE get_cnHoleD() const {
		if (_cnHoleD > cnShaftD)
			return _cnHoleD;
		else
			return cnShaftD + 2;
	}
	BYTE set_cnHoleD(BYTE hole_d) {
		return _cnHoleD = hole_d; 
	}
	__declspec(property(put = set_cnHoleD,get = get_cnHoleD)) BYTE cnHoleD;
	//���Ԥ�����,mm��һ��wnReservedWidth=wnFittingWidth+4
	WORD get_wnReservedWidth() const { 
		if (_wnReservedWidth > wnFittingWidth)
			return _wnReservedWidth;
		else
			return wnFittingWidth + 4;
	}
	WORD set_wnReservedWidth(WORD reserved_width){
		return _wnReservedWidth = reserved_width;
	}
	__declspec(property(put = set_wnReservedWidth,get = get_wnReservedWidth)) WORD wnReservedWidth;
};
class CStdComTemplWireStructure : public CArmHeadComTempl
{
protected:
	void InitComTempl_EB_DANGLE_EARTH_WIRE_ARM1();
	void InitComTempl_EB_DANGLE_EARTH_WIRE_ARM2();
	void InitComTempl_EB_DANGLE_EARTH_WIRE_ARM3();
	void InitComTempl_EB_EARTH_WIRE_ARM();
	void InitComTempl_EB_EARTH_WIRE_PLATE();
	void InitComTempl_EB_DANGLE_WIRE_I_ARM1();
	void InitComTempl_EB_DANGLE_WIRE_I_ARM2();
	void InitComTempl_EB_DANGLE_WIRE_I_ARM3();
	void InitComTempl_EB_WIRE_V_ARM1();
	void InitComTempl_EB_WIRE_V_ARM2();
	void InitComTempl_EB_WIRE_V_ARM3();
	void InitComTempl_EB_WIRE_V_ARM4();
	void InitComTempl_EB_WIRE_V_ARM5();
	void InitComTempl_EB_WIRE_V_ARM6();
	void InitComTempl_META_SECT_BTM_1_EB();
	void InitComTempl_META_SECT_BTM_2_EB();
	void InitComTempl_META_SECT_FRT_1_GD();
	void InitComTempl_META_SECT_BTM_1_GD();
	void InitComTempl_NZ_FRT_2_GD();
	void InitComTempl_NZ_BTM_2_GD();
	void InitComTempl_NZ_BTM_3_GD();
	//
	bool TransTemlInstance_EB_DANGLE_EARTH_WIRE_ARM1();
	bool TransTemlInstance_EB_DANGLE_EARTH_WIRE_ARM2();
	bool TransTemlInstance_EB_DANGLE_EARTH_WIRE_ARM3();
	bool TransTemlInstance_EB_DANGLE_WIRE_I_ARM1();
	bool TransTemlInstance_EB_DANGLE_WIRE_I_ARM2();
	bool TransTemlInstance_NZ_FRT_2_GD();
	bool TransTemlInstance_NZ_BTM_2_GD();
	bool TransTemlInstance_NZ_BTM_3_GD();
	bool TransTemlInstance_SECT_BTM_1_EB();
	bool TransTemlInstance_SECT_BTM_2_EB();
	bool TransTemlInstance_SECT_FRT_1_GD();
	bool TransTemlInstance_SECT_BTM_1_GD();
public:
	static const UINT N10_ID=1;	//��ǵ��߹ҵ�Ķ����ڵ�
	static const UINT N20_ID=2;	//���߹ҵ�
	static const UINT N30_ID=3;	//���߼�ˮƽ������Ĳ�˽ڵ�
	static const UINT N40_ID=4;	//���߼�������ˮƽ���ཻ�ڵ�
	static const UINT N50_ID=5;	//���߽Ǹ��е㣬��EB�ҵ��װ���
	static const UINT N60_ID=6;	//���߽Ǹ��е㣬��EB�ҵ��װ���
	static const UINT N70_ID=7;	//
	static const UINT N80_ID=8;	//
	//�˼�
	static const UINT R_W_T=4;	//T����Ϲ��߽Ǹ�
	PRESET_ARRAY4<MAP_RODnNODE> xarrKeyRodNodes;
	WIREPOINT_GROUP xarrWireGroups[2];	//Ŀǰģ���֧����������ҵ�
	char m_cnWireGroupCount;			//��ǰ���߼ܵĹ��߽Ǹ��Ϲҵ��������
	BYTE m_cnFixWireAngleConnBoltsN;	//�̽ӹ��߶̽Ǹֵ�������˨��(���������������Ǹ֣����Ա�����ż����
public:
	CStdComTemplWireStructure();
	bool TranslateTmplInstance();
	//
	virtual void FromBuffer(BUFFER_IO* pIO,DWORD version=0);
	virtual void ToBuffer(BUFFER_IO* pIO,DWORD version=0);
	virtual bool CloneStdTmplFrom(IStdComTempl* pStdComTmpl);
	virtual void InitComponentTempl(TEMPL_SOURCE* pTemplSource=NULL);
	virtual bool ClearTmplParams();
	virtual int RecogMatchRods(UINT idRecogRod=0);
	virtual bool IsMeetConditions();
	virtual bool SetRecogKeyNodesAndRods(MAP_RODnNODE* parrMapObjs=NULL,long count=1);
	virtual bool MeetRecogKeyObjs(MAP_RODnNODE* parrMapObjs=NULL,long count=1,MAP_RODnNODE* parrMeetMapObjs=NULL,long lnMaxMeetCount=4);
	virtual long SetMappingNode(long idTemplNode,long hLdsNode);
	virtual bool SetMappingNode(long idTemplNode,CLDSNode *pNode);
};
#endif
//////////////////////////////////////////////////////////////////////////
//
#ifdef __LDS_
struct TOWER_PLANINFO{
	bool biCircuitDC;	//�Ƿ�Ϊֱ�� 
	char ciTensionType;	//��������:0.ֱ��������;1.������;2.�ն���;3.�����;4.��λ��
	WORD wiVoltGrade;	//��ѹ�ȼ�kV
	WORD wiCircuits;	//��·��
	WORD wiSeaLevel;	//���θ߶�,m
	UINT uiNamedHeight;	//����(mm),0��ʾ�������ͼ���ѹ�ȼ��Զ�ѡ�����
	CLDSNode* pSetupNode10,*pSetupNode20;
	CXhChar16 szTowerTypeCode;	//�����ͺŴ���
	CXhChar16 szTowerTypeName;	//��������
	TOWER_PLANINFO();
};
#include "..\ShankDefCls.h"
class CTowerPlanner : public CShankDefine
{
	bool AppendWireStruArms(IModel* pModel);
public:
	struct SETUP_PLANE{
		long ziPlane;
		double xfSectCorner,yfSectCorner;
	};
	struct ARM_ASSMBLY{
		long id;
		char ciDirection;		//'X'��X��ᵣ����ͷ;'x'�����X��ᵣ��'Y'��Y��ᵣ��'y'���Y��ᵣ
		long idWireStruTmpl;	//������ͷ��ᵣ��ģ������Id
		SETUP_PLANE xN10,xN20;
		CXhChar50 name;
		void SetKey(DWORD keyid){id=keyid;}
		ARM_ASSMBLY(){ciDirection='X';id=0;idWireStruTmpl=1;}
	};
	TOWER_PLANINFO xPlanInfo;	//��·�滮��Ʋ�����Ϣ
	CHashListEx<ARM_ASSMBLY> hashArmAssmblies;
	bool Planning(TOWER_PLANINFO& planInfo,CTower* pTower);
};
class CWireStruComTemplModel : public CStdComModel
{	
protected:
	//virtual bool BuildAcsOfAssmComponent();
public:
	CWireStruComTemplModel(IModel* pModel=NULL);
	~CWireStruComTemplModel(void){;}
	virtual bool IntelliDesignTowerParams(TOWER_PLANINFO& planinfo);
public:	//�麯��
	virtual CLDSNode* GetLdsNodeFromId(TMPLOBJID idNode);
};
#endif
