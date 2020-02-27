#ifndef __EMEND_BOLT_GASKET_
#define __EMEND_BOLT_GASKET_
//#ifdef __SMART_DRAW_INC_

#include "OutputInfo.h"
///////////////////////////////////////////////////////////////////////////////////////////////
//-------VVVVVVVVVVV------- ��������Ϊ����TAP�Ľ��ĵ����Ч��϶������ wjh-2017.12.5
class CEmendBoltGasket;
struct BOLT_RELA_PART{
	short _zDepth;	//�����װ����˨��������˨����ģ���ƫ����,(������˨λ�ÿ��ܵ���,�����ֵ���岻��,������Խ�������zDepth��ֵ������)
	short siThick;	//��Խ������zDepth������˨����ڵ�ͨ��ƫ����(����Ϊ��ֵ)
	DWORD hPart;	//������װ�乹����¼����Id
	CFGWORD cfgword;
	BOLT_RELA_PART(CLDSPart* pRelaPart=NULL);
	void SetKey(DWORD key){hPart=key;}
	bool IsWasherPlate();
public:	//����
	double get_zDepth() const{return _zDepth*0.1;}
	void set_zDepth(double vf){_zDepth=(short)ftoi(vf*10);}
	_declspec(property(put=set_zDepth,get=get_zDepth)) double zDepth;
	//CLDSPart* pPart����
	CLDSPart* get_pPart();
	void set_pPart(CLDSPart* pRelaPart);
	_declspec(property(put=set_pPart,get=get_pPart)) CLDSPart* pPart;
protected:
	CLDSPart* _pRelaPart;
};
class CBoltGasket;
class CBoltRelaPartList{
	DWORD m_idAssmBolt;	//װ����˨������Id
	DWORD m_idAnchorPart;
	CXhSimpleList<BOLT_RELA_PART> listRelaObjs;
	CEmendBoltGasket* m_pContext;	//����������ģ��
	//DWORD m_idFilterInstance;	//��¼��ǰ��������
public:
	const static BYTE TYPE_SUCCESS				= 0;	//0.�޶��ɹ�
	const static BYTE TYPE_NONE_CONNECTED		= 1;	//1.�޹������� 
	const static BYTE TYPE_VIRTUAL_BOLT			= 2;	//2.�ǽŶ���˨������һ������
	const static BYTE TYPE_MISS_CROSSED_PAD		= 3;	//3.����㴦�ٵ�Ƭ
	const static BYTE TYPE_MISS_END_PAD			= 4;	//4.��ͷ��Ҫ�����˨��Ƭ
	const static BYTE TYPE_PART_POS_ERROR		= 5;	//5.����λ�ô���
	const static BYTE TYPE_ERROR_GASKET_THICK	= 6;	//6.��Ȧ��Ȳ���ȷ
	const static BYTE TYPE_WIRE_VIRTUAL_BOLT	= 7;	//7.���߿׼���˨
	const static BYTE TYPE_NONE					= 0xFF;	//δ��ʼ��
	BYTE m_cErrorType;	//�޶���˨ʱ�������� 
public:
	CBoltRelaPartList();
	void SetKey(DWORD key){m_idAssmBolt=key;}
	CEmendBoltGasket* GetContext(){return m_pContext;}
	void SetContext(CEmendBoltGasket* pContext){m_pContext=pContext;}
	BOLT_RELA_PART* AddRelaPart(DWORD idAssmPart,double zDepth,short siThick,UINT idInstance=0,BOOL bSort=TRUE);
	bool RemoveRelaPart(DWORD idAssmPart,UINT idInstance=0);
	BOLT_RELA_PART* EnumFirstRelaPart();
	BOLT_RELA_PART* EnumNextRelaPart();
	bool IsVirtualHole();	//�Ƿ�Ϊ����ף����Ŷ��������һ����������˨��������ϽǸ��ϵĹ��߿״������Ǹ֣�
	//DWORD IntelliRecogAnchorAssmPart();	//������ѵ���˨êλ��׼����
	double GetMaxSpaceAndSumThroughThick(WORD *pwSumThick=NULL,
										 BOLT_RELA_PART **ppBtmRelaPart1=NULL,
										 BOLT_RELA_PART **ppTopRelaPart2=NULL,
										 BOLT_RELA_PART **ppGasketRelaPart=NULL,
										 BOOL bInGasketThick=TRUE);
	bool InitializeBoltGasket(CBoltGasket* pBoltGasket);
	bool IsThroughGroupAngle();
	int GetRelaPartCount(bool bIncExistGasket=true);
public:	//����
	READONLY_PROPERTY(DWORD,Id);	//װ����˨������Id
	GET(Id){return m_idAssmBolt;}
	DECLARE_PROPERTY(DWORD,AnchorPartId);	//װ����˨������Id
	DECLARE_PROPERTY(BOLT_RELA_PART*,pExistGasket);	//װ����˨������Id
	//WRITEONLY_PROPERTY(DWORD,FilterInstanceId);
	//SET(FilterInstanceId){m_idFilterInstance=value;}
};
class CAssemblePart{
protected:
	long m_hPart;
	CLDSPart* m_pPart;
	CEmendBoltGasket* m_pContext;	//����������ģ��
	CHashList<CLDSPart::HOLE> hashHoles;
public:
	CAssemblePart();
	void SetContext(CEmendBoltGasket* pContainer,CLDSPart* pPart);
	void ClearBoltHoleRefList(){hashHoles.Empty();}
	void SetKey(long key){m_hPart=key;}
	BOOL DeleteHole(DWORD hBolt);
	UINT StatHoles();
	CLDSPart::HOLE *GetBoltHole(DWORD hBolt);
	DWORD GetHoleRefCount(){return hashHoles.GetNodeNum();}
	CLDSPart::HOLE *EnumHoleRefFirst(){return hashHoles.GetFirst();}
	CLDSPart::HOLE *EnumHoleRefNext(){return hashHoles.GetNext();}
public:	//�������Զ���
	CEmendBoltGasket* get_pContext(){return m_pContext;}
	__declspec(property(get=get_pContext)) CEmendBoltGasket* pContext;
	long get_hPart(){return m_hPart;}
	__declspec(property(get=get_hPart)) long hPart;
	SmartPartPtr get_pSmartPart(){return m_pPart;}
	__declspec(property(get=get_pSmartPart)) SmartPartPtr pSmartPart;
};
//��������Ϊ����TAP�Ľ��ĵ����Ч��϶������ wjh-2017.12.5
//-------------^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^---------------------------------
#include "LifeObj.h"
class CBoltGasket
{	//�����п�����Ҫ��˨��Ȧ\�����˨
public:
	long feature;
	CLDSBolt *pBolt;	//��׼��˨
	double gasketOffset;//����Ի�׼��˨�ڻ�׼�����ϵĴ��彻���ƫ��ֵ������ȷ�����λ�ã���׼�����Ĺ�������Ϊƫ������)
	double washerOffset;//��Ȧ����˨��ƫ�ƾ�
	double gasketThick;	//����Ȧ���
	double boltLengthPos;	//��˨�ڶ�λ��׼�Ǹ�������ͶӰ����λ�ã�ʼ->�շ����ʼ�������룩,����������ʱʹ�ò���
	//����˨����Ϊ��׼ʱ������Ҫ��ӵ��λ�õĵײ������붥���������
	long hBtmGasketRelaPart,hTopGasketRelaPart;
	CHashSet<CLDSPart*> hashRelaPartPtrs;//����˨������Ĺ���
	CBoltGasket(){hBtmGasketRelaPart=hTopGasketRelaPart=feature=0;gasketOffset=gasketThick=0;washerOffset=0;boltLengthPos=0;pBolt=NULL;}
public:	//ϵͳ��ǰ���(��Ȧ)�ĺ������
	WORD get_wiCurrDianQuanThick();
	WORD get_wiCurrGasketThick();
	__declspec(property(get=get_wiCurrDianQuanThick)) WORD wiCurrDianQuanThick;
	__declspec(property(get=get_wiCurrGasketThick))   WORD wiCurrGasketThick;
};
typedef CBoltGasket* BOLTGASKET_PTR;
class CDatumPartBolts
{
public:
	f3dPoint norm;
	f3dPoint datum_origin;
	CXhPtrSet<CBoltGasket> boltGasketSet;
};
enum EMENDGASKET_TYPE{
	ERROR_ISOLATED_BOLT=1,		//�޹��������Ĺ�����˨, ����ʽΪ:���Ի�ȷ�����
	ERROR_VIRTUAL_HOLE_LIKELY,	//Ŀǰ��һ�����������ķǽŶ���������ף�����ʽΪ��ȷ����Ϊ������˨��
	ERROR_VIRTUAL_WIREHOLE_LIKELY,	//���߿�������˨������ʽΪ��ȷ����Ϊ������˨��
	ERROR_MISSING_CROSSGASKET,	//����㴦���ȱʧ������ʽΪ����ӵ����Ȧ
	ERROR_MISSING_ENDGASKET,	//�����Ӵ����ȱʧ������ʽΪ����ӵ����Ȧ
	ERROR_GASKET_THICK,			//�����Ȧ��ȴ��󣬴���ʽΪ�������޶������Ȧ���
	ERROR_PART_POS,				//����λ�ô���
	ERROR_BOLTPAD_CUTANGLE,		//���ȱ���нǴ���
	ERROR_GASKET_PROFILE,		//������β����ʣ�����ʽΪ�������������
	ERROR_REDUNDANT_GASKET,		//�����壬����ʽΪ��������
	ERROR_SPECIAL_GASKET,		//������(���λ�����������)

	NEW_BOLTPAD,			//�����ĵ��
	EMEND_BOLTPAD_THICK,	//���������
	EMEND_BOLTPAD_PROFILE,	//�����������
	REDUNDANT_BOLTPAD,		//�������˨��Ƭ
	BOLTPAD_CUTANTLE,		//����нǴ���
	NEW_GASKET,				//������Ȧ
	EMEND_GASKET,			//������Ȧ
	ONE_PART_BOLT,          //��Ч����˨
	EMEND_OTHER,
};
struct GASKET_WARNING_RECORD
{
	DWORD idDatumBolt;	//��Ȧ���������˨�е�һ�Ż�׼������˨
	void SetKey(DWORD key){idDatumBolt=key;}
	long feature;
	EMENDGASKET_TYPE style;	
	int gasketNum;
	double thick;	//�����Ȧ���
	long idRawGasket;
	CLDSBolt* pBolt;
	CLDSLineAngle* pCutAngle;
	union{
		CLDSPart* pPart;
		CLDSPlate* pGasketPlate;	//��Ӧ��ERROR_GASKET_THICK,ERROR_REDUNDANT_GASKET
	};
	double gasketOffset;//����Ի�׼��˨�ڻ�׼�����ϵĴ��彻���ƫ��ֵ������ȷ�����λ�ã���׼�����Ĺ�������Ϊƫ������)
	double washerOffset;//��Ȧ����˨��ƫ�ƾ�
	CLDSPart* pAttachPart;
	GASKET_WARNING_RECORD();
	CXhChar50 GetDescription();
};
//ԭʼ���
class CRawGasket
{
	UINT _id;
public:
	static const int ONEHOLE        = 1;	//����
	static const int MULTIHOLE      = 2;	//�ǽ�����
	static const int CROSSMULTIHOLE = 3;	//������
	static const int WASHER		    = 4;	//��Ȧ
public:
	int type;				//�������	1.���� 2.�ǽ����� 3.�����׵���
	CLDSNode *pCrossNode;	//��¼��׽�������Ӧ�Ľ���ڵ�
	CLDSLineAngle* pCutAngle;//�Ե������нǵ��и�Ǹ�
	CHashSet<CBoltGasket*> boltGasketSet;	//������˨
	CRawGasket(){type=0;pCrossNode=NULL;pCutAngle=NULL;}
	void SetKey(DWORD key){_id=key;}
	UINT get_id(){return _id;}
	__declspec(property(get=get_id)) UINT Id;
};
struct INTERS_POINT{
	CLDSPart *pPart;
	f3dPoint inters_pt;
	double distance;
	double GetHalfThick()
	{
		if(pPart)
			return pPart->GetThick()*0.5;
		else 
			return 0;
	}
	INTERS_POINT(){pPart=NULL;distance=0;}
	INTERS_POINT(CLDSPart *pPart,f3dPoint intersPt,double dist)
	{
		this->pPart=pPart;
		inters_pt = intersPt;
		distance = dist;
	}
};
struct BOLT_ITEM 
{
	double d;		//��˨ֱ��
	f2dPoint vertex;//ģʽ����ϵ�µĶ�������
};
struct VERTEX_ITEM
{
	PROFILE_VERTEX* pVertex;	//���������ϵ�µĶ�����Ϣ
	f2dPoint vertex2d;	//ģʽ�����µĶ�������
	BOOL bRectangular;	//�Ƿ���ֱ��
	int feature;	//�����ע
	VERTEX_ITEM(){bRectangular=FALSE;feature=-1;}
};
class CBoltPadProperty
{
public:
	static const int CUTANGELBOLTPAD  = 1;	//�нǵ��
	static const int RECTANGLEBOLTPAD = 2;	//���ε��
	static const int SPECIALBOLTPAD	  = 3;	//�������
private:
	UCS_2dSTRU modeUcs;	//ģʽ����ϵ
	int m_iSignIndex;	//���㿪ʼ�������
	ARRAY_LIST<VERTEX_ITEM> vertexItemArr;	//���������б�
	ARRAY_LIST<BOLT_ITEM> boltItemArr;	//��˨�����б�
	//���ھ��ε�岻��Ψһȷ��ƥ��ģʽ���꣬��Ҫͨ�����׼������˨����ԭ�����·�ת�����ҷ�ת���ԽǷ�ת�ȽϺ����ȷ��
	UCS_2dSTRU cs2d[4];
	ATOM_LIST<BOLT_ITEM> boltItemList[4]; //cs2d[4]�����µ���˨�����б�
public:
	CLDSPlate *m_pBoltPad;
	int m_iType;
public:
	CBoltPadProperty();
	void InitProperty(CLDSPlate* pPlate);	
	void InitCutAnglePadProperty();
	void InitRectanglePadProperty();
	BOOL CompareBoltInfo(CBoltPadProperty* pDatumPadProp);
	void MarkProfileVertexs();
	void UpdateMinProfileVertex(CBoltPadProperty* pDatumPadProp);
	BOOL EmendProfileVertexs(CBoltPadProperty* pDatumPadProp);
public:	//���Ժ���
	void Test_InitProperty(int index,CLDSPlate* boltPad);
	void Test_CompareBoltInfo(int index,CBoltPadProperty* datumPadProp);
	void Test_MarkProfileVertexs(int index);
	void Test_UpdateMinProfileVertex(int index,CBoltPadProperty* datumPadProp);
	void Test_EmendProfileVertexs(int index,CBoltPadProperty* datumPadProp);
};
struct SAMESCHEMA_BOLTPAD  
{
	CBoltPadProperty* pDatumProperty; 
	CHashList<CLDSPlate*> boltPadSet;
	SAMESCHEMA_BOLTPAD(){pDatumProperty=NULL;}
	int IsSameSpec(CBoltPadProperty* pPadProperty);	// 0.����� 1.���
};
//�޶���˨��Ƭ���Ȧ
class CEmendBoltGasket
{
public:
	static const int MINMODIFYDIST = 1;
	static const int MINSPACING = 3;			//��С��϶
	static const int BOLTSPACE_THRESHOLD = 150;	//�ж��Ƿ�Ϊ��׵�Ƭʱ��Ҫ����˨���ֵ

	CHashList<CBoltGasket> hashBoltGaskets;	//������Ҫ��ӵ�Ȧ���Ƭ����˨
	CHashListEx<CRawGasket> hashRawGaskets;     //δ�ӹ��ĵ�Ƭ���� 
	PLATESET existingBoltPadSet;				//�Ѵ��ڵĵ�Ƭ
	CHashList<CLDSPlate*> hashNewGaskets;		//��¼�����ɵ�Ƭ
	CHashListEx<GASKET_WARNING_RECORD> hashErrorRecords;	//��¼��Ƭ���Ȧ��������Ϣ
	CHashSet<GASKET_WARNING_RECORD*>   hashErrorRecordsView;//������˨�����λ��ʾ��Ϣ(��׵��ʱ������˨��������˨ͨ���˹�ϣ�����Ҷ�Ӧ�ľ�ʾ��Ϣ)
	GASKET_WARNING_RECORD* GetWarningRecord(long hBolt);
private:
	//������˨������Ϣ��������λ�úͺ��
	//1.����������˨��������Ҫ��ӵ�Ȧ���Ƭ����˨
	void InitGasketBoltSet(bool advanceMode=true);
	//3.�������е����Ϣ������������˨��gasketBoltSet���Ƴ�
	void EmendExistingBoltPadSet(bool advanceMode=true);
	//4.��ʼ��δ�ӹ��ĵ�Ƭ���ϣ�����ʼ�����ģʽ(��Ȧ�����ס���׼���׽������)
	void InitRawGasketSet();
	void InitRawGasketSetAdvance();
	//5.��������ΪWAHSER�ĵ���˨��Ȧ��Ϣ������rawGasketSet�Ƴ���
	void EmendBoltWasherInfo();
	//6.����δ�ӹ���Ƭ,����ӵ�newGasketSet��
	void CreateGaskets();
	//7.�ϲ�ͬһ������������ĵ�Ƭ
	void MergeSimilarGasketInSameSeg();
	void AppendOutputInfoToList(GASKET_WARNING_RECORD* pRecordPart,CTaskOutputList* pInfoList);
public:
	CEmendBoltGasket(IModel* pModel){m_pModel=pModel;}
	void Execute(DATACENTERCMD_CONTEXT *pContext,bool advanceMode=true);
	void InitOutputInfoList(CTaskOutputList* pInfoList);
	//����Ϊ����TAP�Ľ��ĵ����Ч��϶������ wjh-2017.12.5
protected:
	IModel* m_pModel;
	struct WIRE_ANGLE{
		long idAssmAngle;
		BYTE ciWireWing;	//'X':X֫Ϊ����֫;'Y':Y֫Ϊ����֫
		WIRE_ANGLE(long _idAssmAngle=0,BYTE _ciWireWing=0)
		{
			idAssmAngle=_idAssmAngle;
			ciWireWing=_ciWireWing;
		}
	};
	CHashList<WIRE_ANGLE> hashWireAngles;
	CHashListEx<CBoltRelaPartList> hashBoltRelaObjs;
	CHashListEx<CAssemblePart> hashAssmParts;
	BYTE JustifyErrorEndPosAngle(CAssemblePart* pAssmPart,CBoltGasket* pGasket);
	BYTE ValidateTheroyGasket(CBoltGasket* pGasket,long* phErrPosAngle=NULL);
public:
	void StatWireAngles();
	BYTE GetAngleWireWing(long idAssmAngle);//0:�ǹ��߽Ǹ�;'X':X֫Ϊ����֫;'Y':Y֫Ϊ����֫
	void UpdateBoltRelaObjs(DWORD* pAssmBoltIds=NULL,UINT count=0);	//pAssmBolts=NULL,��ʾ����ȫ����˨������Ϣ�����������ָ����˨����
	void UpdateBoltHoles(DWORD* phAssmParts=NULL,UINT count=1);
	//idAssmBolt:ê�̶�λ��˨װ�����;idCurrAssmPart:��ǰ��Ҫ���涨λ�Ĺ���װ�����;pAnchorPoint���ط��涨λê�̵�
	//bool CalDestAnchorPoint(DWORD idAssmBolt,DWORD idCurrAssmPart,CAssemblePart::ANCHOR_POINT* pAnchorPoint);
	CBoltRelaPartList* GetBoltRelaParts(DWORD hBolt);
	CBoltRelaPartList* AddBoltRelaParts(DWORD hBolt);
	CAssemblePart* GetAssemblePart(DWORD hPart);
	CAssemblePart* AddAssemblePart(CLDSPart* pPart);
	CLDSPart* FromPartHandle(long hPart,int idClassType=0){return m_pModel->FromPartHandle(hPart,idClassType);}
	CAssemblePart* FromAssmPartHandle(long hPart,int idClassType=0);
	BOOL ExecuteEmendBoltSpec(DATACENTERCMD_CONTEXT *pContext);
};
#endif