//Translated by hxr 2014-12-8
// Tower.h: interface for the CTower class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOWER_H__06FC49B6_50F1_4315_A1B2_BAFF7A74AA17__INCLUDED_)
#define AFX_TOWER_H__06FC49B6_50F1_4315_A1B2_BAFF7A74AA17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdlib.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "list.h"
#include "HashTable.h"
#include ".\Buffer.h"
#include "lds_part.h"
#include "DefinedView.h"
#include "lds_co_part.h"
#include "lds_list.h"
#include "ldsptr_list.h"
#include "Block.h"
#include "lslist.h"
#include "HoleMaker.h"
#include "CirBufferStack.h"
#include "ArrayList.h"
#include "Interpolator.h"
#include "FoundationParam.h"
#include "Catlog.h"
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
#include "Resolver.h"
#endif

template<class BASE_CLS> class CSuperSmartPtr
{
	BASE_CLS* m_pSrcPtr;
	BASE_CLS* ValidPointer()const{
		if(!m_pSrcPtr)
#ifdef	AFX_TARG_ENU_ENGLISH
			throw "Encounter an pointer error";
#else
			throw "ָ������ʧ��";
#endif
		return (BASE_CLS*)m_pSrcPtr;
	};
public:
	CSuperSmartPtr(BASE_CLS* pSrcPtr=NULL){m_pSrcPtr = pSrcPtr;}
	~CSuperSmartPtr(){;}
	bool IsHasPtr()
	{
		if(m_pSrcPtr!=NULL)
			return TRUE;
		else 
			return FALSE;
	}
	bool IsNULL()
	{
		if(m_pSrcPtr==NULL)
			return TRUE;
		else 
			return FALSE;
	}
	__declspec(property(get=IsHasPtr)) bool blValid;
	BOOL IsEqualPtr(const void* rPtr)
	{
		if(m_pSrcPtr==rPtr) return TRUE;
		else return FALSE;
	}
	BOOL operator == (CSuperSmartPtr& rPtr)
	{	return m_pSrcPtr==rPtr.LinePartPointer();}
	CSuperSmartPtr& operator = (CSuperSmartPtr& rPtr)
	{
		m_pSrcPtr = rPtr;
		return *this;
	}
	operator BASE_CLS*(){return ValidPointer();}
	CSuperSmartPtr& operator = (BASE_CLS* rPtr)
	{
		m_pSrcPtr = rPtr;
		return *this;
	}
	BASE_CLS* operator->()const{return ValidPointer();}
	CLDSPart *PartPointer(){
		if(!ValidPointer()->IsPart())
		{
			char error[100];
#ifdef  AFX_TARG_ENU_ENGLISH
			sprintf(error,"0X%X type cast error!",m_pSrcPtr->handle);
#else
			sprintf(error,"0X%X����ת��ʧ��!",m_pSrcPtr->handle);
#endif
			throw error;
		}
		return (CLDSPart*)ValidPointer();
	}
	__declspec( property(get=PartPointer)) CLDSPart* pPart;
	CLDSLinePart *RodPointer(){
		if(!ValidPointer()->IsLinePart())
		{
			char error[100];
#ifdef  AFX_TARG_ENU_ENGLISH
			sprintf(error,"0X%X type cast error!",m_pSrcPtr->handle);
#else
			sprintf(error,"0X%X����ת��ʧ��!",m_pSrcPtr->handle);
#endif
			throw error;
		}
		return (CLDSLinePart*)ValidPointer();
	}
	CLDSLinePart *LinePartPointer(){return RodPointer();}
	__declspec( property(get=RodPointer)) CLDSLinePart* pRod;

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLDSLineAngle *LineAnglePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_LINEANGLE&&m_pSrcPtr->GetClassTypeId()!=CLS_GROUPLINEANGLE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSLineAngle*)ValidPointer();
	}
	__declspec( property(get=LineAnglePointer)) CLDSLineAngle* pAngle;
	CLDSGroupLineAngle *GroupAnglePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_GROUPLINEANGLE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSGroupLineAngle*)ValidPointer();
	}
	__declspec( property(get=GroupAnglePointer)) CLDSGroupLineAngle* pGroupAngle;

	CLDSLineTube *LineTubePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_LINETUBE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSLineTube*)ValidPointer();
	}
	__declspec( property(get=LineTubePointer)) CLDSLineTube* pTube;
#ifdef __GGG_
	CLDSConeTube *ConeTubePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_CONETUBE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSConeTube*)ValidPointer();
	}
#endif
	CLDSGeneralPlate *GeneralPlatePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_PLATE&&m_pSrcPtr->GetClassTypeId()!=CLS_PARAMPLATE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSGeneralPlate*)ValidPointer();
	}
	__declspec( property(get=GeneralPlatePointer)) CLDSGeneralPlate* pBasicPlate;

	CLDSPlate *PlatePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_PLATE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSPlate*)ValidPointer();
	}
	__declspec( property(get=PlatePointer)) CLDSPlate* pPlate;

	CLDSParamPlate *ParamPlatePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_PARAMPLATE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSParamPlate*)ValidPointer();
	}
	__declspec( property(get=ParamPlatePointer)) CLDSParamPlate* pParamPlate;

	CLDSBolt *BoltPointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_BOLT)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSBolt*)ValidPointer();
	}
	__declspec( property(get=BoltPointer)) CLDSBolt* pBolt;

	CLDSLineSlot *LineSlotPointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_LINESLOT)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSLineSlot*)ValidPointer();
	}
	__declspec( property(get=LineSlotPointer)) CLDSLineSlot* pSlot;

	CLDSLineFlat *LineFlatPointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_LINEFLAT)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSLineFlat*)ValidPointer();
	}
	__declspec( property(get=LineFlatPointer)) CLDSLineFlat* pFlat;

	CLDSArcPart *ArcPartPointer(){
		if(!ValidPointer()->IsArcPart())
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSArcPart*)ValidPointer();
	}
	__declspec( property(get=ArcPartPointer)) CLDSArcPart* pArcRod;

	CLDSArcAngle *ArcAnglePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_ARCANGLE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSArcAngle*)ValidPointer();
	}
	__declspec( property(get=ArcAnglePointer)) CLDSArcAngle* pArcAngle;

	CLDSArcSlot *ArcSlotPointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_ARCSLOT)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSArcSlot*)ValidPointer();
	}
	__declspec( property(get=ArcSlotPointer)) CLDSArcSlot* pArcSlot;

	CLDSArcFlat *ArcFlatPointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_ARCFLAT)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSArcFlat*)ValidPointer();
	}
	__declspec( property(get=ArcFlatPointer)) CLDSArcFlat* pArcFlat;

	CLDSSphere *SpherePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_SPHERE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSSphere*)ValidPointer();
	}
#endif
	__declspec( property(get=SpherePointer)) CLDSSphere* pSphere;

	CLDSNode *NodePointer(){
		if(ValidPointer()->GetClassTypeId()!=CLS_NODE)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Type cast error!";
#else
			throw "����ת��ʧ��!";
#endif
		return (CLDSNode*)ValidPointer();
	}
	__declspec( property(get=NodePointer)) CLDSNode* pNode;

};
#define SmartPartPtr CSuperSmartPtr<CLDSPart>
#define SmartPlatePtr CSuperSmartPtr<CLDSGeneralPlate>
#define SmartArcPtr CSuperSmartPtr<CLDSArcPart>
#define SmartRodPtr CSuperSmartPtr<CLDSLinePart>
//����ؽṹ
typedef struct tagWIND_LOAD
{
	//X/Y/Z����ķ���ػ���ϵ�����Զ���������ʱ�ֱ��ʾ���١���ѹ����ϵ���ͷ����
	double velocity;	//����
	long  azimuth;		//����λ�ǣ��㣩������·����Ϊ׼����Y������
}WIND_LOAD;
//�ڵ㼯�к���,��ת��lds_part.h wjh-2014.4.27
class CExternalNodeLoad
{
public:
	double permanentFz;	//���������׼���������ֵ(N)
	GEPOINT Fxyz;		//���������᷽��ļ��пɱ������ֵ(N)
	GEPOINT designLoad;	//�������ֵ�������ڸ�����Ƽ��㣨�����ṹ��Ҫ��ϵ�����ڵ�����ϵ��������������ȡ��Fxyz��permanentZ
	GEPOINT stdLoad;	//���ر�׼ֵ�������ڻ������������㣨�����ṹ��Ҫ��ϵ�����ڵ�����ϵ����
	CExternalNodeLoad(){permanentFz=0.0;}
};
class CTower;
struct SUB_OBJ{
public:
	BYTE ciSubType;	//�Ӷ�������IDbEntity::GetDbEntType()������
	long iSubId;	//���黺���ڣ������Թ�ϣ����ʽ���ڣ��ı�ʶ��,һ�������ϣ������Ԫ��ʱ�ĳ�ʼ�������н��и�ֵ
	long hObj;		//�ϼ�����ı�ʶ�Ż򹹼��ױ�ʱ��ԭʼ��˨
	void* pSubObj;		//�������Ӷ���
public:
	SUB_OBJ(){memset(this,0,sizeof(SUB_OBJ));}
	friend void LoadSubGeometryEnt(SUB_OBJ *pSubObj,DWORD serial);
	long SubId(){return iSubId;}
	void* SubObj(IModel* pModel);
};
struct PROP_OBJ{
public:
	static const BYTE TYPE_LDSOBJ	= 0;
	static const BYTE TYPE_DBENTITY	= 1;
	static const BYTE TYPE_HOLEWALL	= 2;
	static const BYTE TYPE_WIREPOINTARROW	= 3;
	BYTE ciObjType;	//0.��ͨLDS���ݶ���;1.�߿�ͼԪ����Դ��ͼֽͼԪIDbEntity;2.�����Ͽױ�ʵ��
	UINT uiSolidId;	//ͼ����������ʾʵ����ڲ���ʶId
public:
	union{
		CLDSObject* pLdsObj;
		struct{
			CLDSBolt* pBolt;		//�������Ӷ���
			long hResidePart;	//�ױڵ���������
		}hole;
		struct SUB_OBJ{
			BYTE ciSubType;	//�Ӷ�������IDbEntity::GetDbEntType()������
			long hObj;		//�ϼ�����ı�ʶ�Ż򹹼��ױ�ʱ��ԭʼ��˨
			long iSubId;	//���黺���ڣ������Թ�ϣ����ʽ���ڣ��ı�ʶ��,һ�������ϣ������Ԫ��ʱ�ĳ�ʼ�������н��и�ֵ
			void* pSubObj;	//�������Ӷ���
		}subobj;
	}obj;
public:
	PROP_OBJ(CLDSObject* pObj=NULL);
	PROP_OBJ(CLDSBolt* pBolt,long hResidePart);
	void SetKey(DWORD key){uiSolidId=key;}
	//LDS����
	CLDSObject *set_LdsObject(CLDSObject* pPropObj);
	CLDSObject *get_LdsObject();
	__declspec(property(put=set_LdsObject,get=get_LdsObject)) CLDSObject* pObj;
	//LDS�����ϵ��߿�ͼԪ�Ӷ�������
	long get_hObject();
	__declspec(property(get=get_hObject)) long hObj;
	long SubId();
	__declspec(property(get=SubId)) long liSubId;
	void* SubObj();
	__declspec(property(get=SubObj)) void* pSubObj;
	//�����ϵĿױ�ʵ�帴�϶���
	CLDSBolt* set_pBolt(CLDSBolt* pHoleBolt);
	CLDSBolt* get_pBolt();
	__declspec(property(put=set_pBolt,get=get_pBolt)) CLDSBolt* pBolt;
	long set_hResidePart(long hPart);
	long get_hResidePart();
	__declspec(property(put=set_hResidePart,get=get_hResidePart)) long hResidePart;
};
class CWorkStatus : public CLDSObject
{
	//���º������ֵȡֵϵ�����ܻ���Ϊ�û��趨��ѭ�淶�Ĳ�ͬ�Զ�ȡֵ����Ӧ��ȡֵ�����ۺϿ��Ǻ�ȡֵ
	//�˴���ȡֵ��Ϊ��Ԫ��
	struct DESIGN_COEF_ICE
	{
		//���������ص�����ϵ����һ����ݸ�����ȶ���5mmȡ1.1,10mmȡ1.2,15mm ����ȡ1.6��20mm ȡ1.8��20mm ���ϱ���ȡ2.0��2.5
		double B_I;
		double gamaQ;	//�ɱ���ط���ϵ��
		double Kesai;	//�׿ɱ�������ϵ��
	}coefIce;	//�������ؼ������淶������ɱ���صĺ������ȡֵϵ��
	struct DESIGN_COEF_WIND
	{
		double gamaQ;	//�ɱ���ط���ϵ��
		double Kesai;	//�׿ɱ�������ϵ��
	}coefWind,coefAn,coefU,coefT;	//����ء���װ���ء����߼������ȸ������µĲ��������ء��¶����ú��صĺ������ȡֵϵ��
	double gamaEh,gamaEv;	//ˮƽ������������ú��صķ���ϵ��
	double GamaQi;	//��i��ɱ���صķ���ϵ����Ӧȡ1.4
	/*�ɱ���ص����ϵ��:
	������ѹ��·���������������Ӧȡfai=1.0;220V�������͵���·�Ķ�������͸���
	��ѹ��·�İ�װ�����Ӧȡfai=0.9;������ѹ��·�����������110V��·�Ķ��������
	Ӧȡfai=0.75��
	*/
	double Fai;
	int m_iEiffelCaseNo;	//��ʱ����
public:
	CWorkStatus();
	~CWorkStatus();
	CTower* SuperiorTower();
	void FromRawBuffer(CBuffer &buffer,long version=0);	//���ڴӺ��ؼ�����������ݿ�
	void ToRawBuffer(CBuffer &buffer,long version=0,long doc_type=0);	//��������ؼ�����򵼳����ݿ�
	void FromBasicCoefBuffer(CBuffer &buffer);	//���ڹ����俽������ϵ������������
	void ToBasicCoefBuffer(CBuffer &buffer);	//���ڹ����俽������ϵ������������
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tsa3_lds4_default3=3);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void Serialize(CArchive &ar); 
	int SetEiffelCaseNo(int iCaseNo);
	int GetEiffelCaseNo();
	CXhChar16 ToCaseNoString();
	long GetEiffelStatusId();	//���ذ������ƶ�ЧӦ�����ŵ����Id
	long GetEiffelStatusNo();	//���ذ������ƶ�ЧӦ�����ŵ�������
	//���ؼ���ϵ��
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	double SetBw_I(double Bw_I);		//����ʱ�������������ϵ��
	double SetGamaQ(double GamaQ);		//����ط���ϵ��
	double SetGamaQ_W(double GamaQ_W);	//����ط���ϵ��
	double SetGamaQ_I(double GamaQ_I);	//�������ط���ϵ��
	double SetGamaQ_A(double GamaQ_A);	//��װ���ط���ϵ��
	double SetGamaQ_U(double GamaQ_U);	//���ߡ������ȸ������ط���ϵ��
	double SetGamaQ_T(double GamaQ_T);	//�¶Ⱥ��ط���ϵ��
	double SetGamaQ_Eh(double GamaQ_Eh);	//ˮƽ�������ط���ϵ��
	double SetGamaQ_Ev(double GamaQ_Ev);	//���������ط���ϵ��
	double SetKesai(double Kesai_W);	//��������ϵ��
	double SetKesai_W(double Kesai_W);	//��������ϵ��
	double SetKesai_I(double Kesai_I);	//�����������ϵ��
	double SetKesai_A(double Kesai_A);	//��װ�������ϵ��
	double SetKesai_U(double Kesai_U);	//���ߡ������ȸ����������ϵ��
	double SetKesai_T(double Kesai_T);	//�¶Ⱥ������ϵ��
	double Bw_I();		//����ʱ�������������ϵ��
	double GamaQ();		//�������������ط���ϵ��
	double GamaQ_W();	//����ط���ϵ��
	double GamaQ_I();	//�������ط���ϵ��
	double GamaQ_A();	//��װ���ط���ϵ��
	double GamaQ_U();	//���ߡ������ȸ������ط���ϵ��
	double GamaQ_T();	//�¶Ⱥ��ط���ϵ��
	double GamaQ_Eh();	//ˮƽ�������ط���ϵ��
	double GamaQ_Ev();	//���������ط���ϵ��
	double Kesai();		//��������ϵ��
	double Kesai_W();	//��������ϵ��
	double Kesai_I();	//�����������ϵ��
	double Kesai_A();	//��װ�������ϵ��
	double Kesai_U();	//���ߡ������ȸ����������ϵ��
	double Kesai_T();	//�¶Ⱥ������ϵ��
#endif
public:
	struct WINDLOAD_REFPARAM{
		double shieldCoef;
		double attachMass;				//������ʩ������Ч��ÿ�������ڵ��ϵ���������(kg)
		//���ӷ�����ڽڵ��ϲ����ķ����(����AttachWindLoad.m_fBetaZ,��δ���Ƿ�ѹ�߶ȱ仯ϵ��MuZ),��λN��
		//��ʱ���ݲ��洢���ļ���
		f3dPoint basicAttachWindLoadF;
		WINDLOAD_REFPARAM(){shieldCoef=1.0;attachMass=0;}
	};
	CHashList<WINDLOAD_REFPARAM> AttachWindLoad;	//���ӷ���صĵ���ϵ���б�keyֵ���Ǹ��ӷ���صľ��ֵ
	CHashList<CExternalNodeLoad>hashNodeLoad;		//�洢���ؽڵ�,keyֵΪ�ڵ���
	long iNo;
	long iWireLayoutNo;	//���߷������
	char description[MAX_TEMP_CHAR_100+1];		//���ڹ����ļ�Ҫ����
	char limit_str[MAX_TEMP_CHAR_100+1];		//����ģ�������ַ���
	BYTE m_cMainLoadType;	//0:���1:����2:����3:���߼������ȸ���4:��װ5:ˮƽ����6:�������7:��ˮƽ����8:���������
	bool m_bCheckWorkCond;	//���㹤��
	bool m_bSpecLoadFactors;//ָ���������ϵ������G,��Q���ף�����������o
	bool m_bLoadIncFactor;	//ֱ��Ӧ�ð����и���ϵ������o����G,��Q���ף��ĺ������ֵCExternalNodeLoad::designLoad
	BYTE m_cHoriEarthQuakeAxis;	//ˮƽ��������ʱ���񶯷���ǡ㣬'X'=0��'Y'=90,ASCII��ֵ'X':88��'Y':89
	char vibration_mode_str[MAX_TEMP_CHAR_50+1];	//���ͷֽⷴӦ�׷�����ʱ��Ҫ���ǵ������б�

	double ice_thick;	//�����������(mm)
	WIND_LOAD WindLoad;	//�����
	//������
	DECLARE_PROP_FUNC(CWorkStatus);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);
#if defined(__PART_DESIGN_INC_)&&!defined(__DRAWING_CONTEXT_)&&!defined(__IGNORE_VERSION_COMPARE_)
	bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents);
#endif
};
//
struct MODULE_LEG : public ICompareClass {
private:
	UINT m_uiLegSerial;
public:
	BOOL valid;
	double m_fLowestZ;
	CLDSLinePart* pLegPostRod;
	CFGWORD cfgword;
	MODULE_LEG(UINT uiLegSerial = 1) {
		valid = false;
		m_fLowestZ = 0;
		pLegPostRod = NULL;
		m_uiLegSerial = uiLegSerial;
	}
	UINT get_uiLegSerial(){return m_uiLegSerial;}
	__declspec(property(get=get_uiLegSerial)) UINT uiLegSerial;
	virtual int Compare(const ICompareClass *pCompareObj)const
	{	//����Ĭ���ɸߵ�������
		MODULE_LEG* pSubLeg = (MODULE_LEG*)pCompareObj;
		if (m_fLowestZ > pSubLeg->m_fLowestZ)
			return -1;
		else if (m_fLowestZ < pSubLeg->m_fLowestZ)
			return 1;
		else
			return 0;
	}
};
//����������ϵ������ȡ��

int GetMinNo(DWORD dwWord);
int GetMaxNo(DWORD dwWord);
typedef void* MODULE_LEGVOIDPTR;
class CLDSModule : public CLDSObject
{
	//ATOM_LIST<long> *AttachWindLoad;	//��������
	double lowest_z;
	WORD m_iBodyNo;			//������(��&ͷ��)����Ӧ����ģ����ȣ���
public:
	CLDSModule();
	CTower* SuperiorTower();
	CFGWORD GetBodyCfgWord();			//��ĺ�m_iBodyLegNo����һ�ֱ�����ʽ
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromCdfBuffer(CBuffer &buffer,long version=0);
	void ToCdfBuffer(CBuffer &buffer,long version=0);	
	void ToXmlFile(FILE* fp,DWORD schema);
	//void Serialize(CArchive &ar);
	//ֻ���ɷ��ϱ�����ģ�������Ľڵ�͸˼�����,�������������Ľڵ㼯�͸˼���
	void RefreshModule(NODESET &nodeset,LINEPARTSET &poleset,BOOL bVerifyLeg=FALSE);
#ifdef __PART_DESIGN_INC_
	bool SubBlockCheck(NODESET &boundary,CXhChar50* error_info);
#endif
public:
	//���ز�����͵Ļ���Z���꣨���ո˼�����͵���㣩
	//���ز�����ߵ�ͷ��Z���꣨���ո˼�����ߵ���㣩
	//����ȫ�����ܸ߶�(mm)
	double GetModuleScopeZ(double *highest=NULL,double *lowest=NULL);
	double LowestZ(){return lowest_z;}
	BOOL IsSonPart(CLDSDbObject *pObj,CFGWORD *pSpecCfgWord=NULL,bool bVerityActLeg=true);
	WORD SetBodyNo(WORD iBodyNo);
	WORD GetBodyNo(bool bVerityValid=true) const;	//����Ϊһ��������Ϊ��ֹ����ų���1~24���޵���Ӧ��ʱ����Խ�� wjh-2013.4.12
	WORD get_idBodyNo(){return GetBodyNo();}
	__declspec(property(get=get_idBodyNo)) WORD idBodyNo;
public:
	bool m_bUserSpecifyLowestZ;//false:��ʾϵͳ�Զ��������Z0��׼ƽ��;true:�����û�ָ��ֵ����
	double m_fNamedHeight;			//ָ���������߼���ʱ����0��׼ƽ���Z����ֵ
	double set_NamedHeight(double fNamedHeight);
	double get_NamedHeight();
	__declspec(property(put=set_NamedHeight,get=get_NamedHeight)) double NamedHeight;
	bool m_bSpecifyLevelheightDiff;
	WORD m_wnLevelHeight;			//���߸��ӽ��Ȳ㼶��߶Ȳ�ֵ(mm)
	UINT set_uiLevelHeight(UINT uiLevelHeightDiff);
	UINT get_uiLevelHeight();
	__declspec(property(put=set_uiLevelHeight,get=get_uiLevelHeight)) UINT uiLevelHeight;
	GEPOINT xarrBaseLocations[24];		//ÿ�������������24���ȣ������д洢ÿ���ȵĻ����������ĵ�λ�ã�����TIDʱ��ʱ���´洢��
	bool UpdateFoundationLocations();	//����TIDʱ�ô˺�����ʱ���������xarrBaseLocations���� wjh-2019.3.31
	GEPOINT GetFoundationLocation(BYTE ciLegSerial=1);	//ȡֵ1~24����ʾ���Ⱥ�
	UINT UpdateValidLegs(MODULE_LEGVOIDPTR parrSortedLegs=NULL,UINT uiMaxCount=16);
public:
	DWORD ActiveQuadLegDword();
	short GetCfgWordLegCount();	//��ǰ����m_dwLegCfgWord�����а����Ľ�������
	//���ݵ�ǰ�����ڵĽ�����ţ�1Ϊ����,1~24��ת��Ϊȫģ�͵�������(1~192) wjh-2019.7.22
	WORD  ToLegSerialBySerialInHeight(int serialInHeight);
	short GetLegCount();	//ͳ�Ƶ�ǰ�����������ʵ����Ч�������� wjh-2016.6.14
	long iNo;
	char description[MAX_TEMP_CHAR_50+1];		//���ڼ���ģ�͵ļ�Ҫ����
	char segStr[MAX_TEMP_CHAR_200+1];			//�������������Ķκ��ַ���
	double land_high;				//���θ�(m)�����Լ��������ѹ�߶ȱ仯ϵ��
	CFGWORD m_dwLegCfgWord;		//������ĺ�
	WORD m_arrActiveQuadLegNo[4];	//��ģ�͵�ǰ��һ�������޵ĵ�ǰ���Ⱥ�
	double m_fBurglarLsStartZ;	//������˨��Χ��ʼZ����
	double m_fBurglarLsEndZ;	//������˨��Χ��ֹZ����
	double m_fLockPadStartZ;	//���ɵ�Ȧ��Χ��ʼZ����  wht 09-12-22
	double m_fLockPadEndZ;		//���ɵ�Ȧ��Χ��ֹZ����
};
#ifdef __PART_DESIGN_INC_
struct FREEDOMGROUP
{
	long hNode;	//�ڵ���
	int dimension;
	double v[6];
};
class CVibrationMode{
public:
	CVibrationMode();
	~CVibrationMode();
	double frequency;	//Ƶ��
	FREEDOMGROUP *freedom_arr;
};
//����������Է����������
struct DYNAMIC_ANAL_NODE
{
	DWORD  id;		//�ڵ��ʶ����ʱ�洢�ڵ�ָ�롢��Ż���
	double mass;	//��Ӧ�ĵ�Ч�ڵ�����(kg)
};
#endif
class CConnectFL
{
	BYTE condFlag;	//0x01ָ����������;0x02ָ����˨Բ;0x04ָ����˨ֱ��;0x08ָ����˨����
public:
	CConnectFL(){memset(this,0,sizeof(CConnectFL));}
	double m_fThickTube;			//���ܱں�(mm)
	double m_fInnerRadius;			//��׼�����ڻ��뾶(mm)
	double m_fCoupleInnerRadius;	//��Է����ڻ��뾶(mm)
	char bolt_grade[9];		//��˨����
	long bolt_d;			//��˨ֱ�������
	long bolt_n;			//��˨���е���˨����
	double N,M;				//N ������(N),M���(N*m)
	double m_fBoltRadius;	//��˨Բ�뾶(mm)
	long m_nThickFL;		//�����̺��(mm)
	long m_nWidthFL;		//�������δ���(mm)
	long m_nThickRib;		//�߰���(mm),���Ϊ0��ʾ�޼Ӿ���
	long m_nHeightRib;		//�߰�߶�(mm)
	long m_nRibS;			//�߰庸���нǸ�(��)��
	long m_nCoupleRibS;		//����߰庸���нǸ�(��)��
	long m_nCoupleThickFL;	//�����̺��(mm)
	long m_nCoupleHeightRib;//����߰�߶�(mm)
	char cMaterial;			//ͬʱ��ʾ�����弰�Ӿ��߰�Ĳ��ʣ�Ŀǰ��֧�ַ������߰�ͬ���ʵ����
	BYTE cFLType;			//0.��ͨ������1.ƽ��������2.�Ժ�����
	char m_sPartNo[16];     //�������
	double SingleWeight();	//����������
	double SumWeight();		//�ɶԷ�����
	void Optimize(BYTE flag);
	BOOL IsEqual(CConnectFL flConnectStru,BOOL isNear = FALSE);//isNear=truesʱ����������ж�
};
struct ANGLE_JOINT
{
private:
	DWORD _id;
public:
	//0.�ڰ�������ְ壻1.�������2.���������'A'.��ϽǸֽ�ͷA��'B'.��ϽǸֽ�ͷB��'C'��ϽǸֽ�ͷC��'D'.��ϽǸֽ�ͷD
	BYTE cJointType;
	long hNode;				//��ͷ�����ڵ�
	long hSmallRod,hBigRod;	//��׼�˼�(���ӵ���ͷ�˼�)�빲�߶Խӵĸ˼�
	struct BOLT{
		BYTE rows;	//��˨����
		BYTE n;		//��˨����
		BYTE grade;	//��˨����,48: 4.8����68:6.8����88:8.8����109: 10.9��
		BYTE d;		//��˨ֱ��
		double holeout_coef;//����ϵ��
	}bolt;
	SIZE_WTM innerAngle;	//�ڰ���֫��֫�񼰲���
	union OUTER{
		SIZE_WTM plate;	//����ְ��񼰲���
		SIZE_WTM angle;	//�����֫��֫�񼰲���
	}outer;
	SIZE_WTM smallAngle;	//��hSmallRod��Ӧ�ĽǸֹ��
	SIZE_WTM bigAngle;		//��hBigRod��Ӧ�Ǹֹ��
	MIRMSG mirinfo;			//��ͷ�Գ���Ϣ
	ANGLE_JOINT();
	~ANGLE_JOINT();
	DWORD Id(){return _id;}
	DWORD InitId(DWORD id);
	ANGLE_JOINT operator =(ANGLE_JOINT& joint);
	bool VerifyLinkAngles();
	int  TestCalMinBoltN();	//����������Ҫ����˨��
	bool DesignCheckAreaAndBoltForceScale();	//���������ڱ�ǩ����
	CXhChar50 GetJointNo();
	CXhChar100 GetBoltInfo(bool bAlignRowCount=TRUE);
	CXhChar100 GetInnerPartSpec(bool bIncAngleType=FALSE);
	CXhChar100 GetOutterPartSpec(bool bIncAngleType=FALSE);
public:	//���ӱ�ǩ���Լ�������������Ƶ���ʱͳ��ϵ�������ô洢��
	struct ITEM_TAG_INFO : public TAG_INFO<ANGLE_JOINT>{
	public:
		CLDSNode* pNode;
		CLDSLineAngle* pSmallAngle,*pBigAngle;
		double scaleAreaJ2A1;	//��ͷ�뱻����С�ǸֵĽ����֮��
		double scaleAreaJ2A2;	//��ͷ�뱻���Ӵ�ǸֵĽ����֮��
		double scaleForceB2A1;	//��˨�뱻����С�Ǹֵĳ�������֮��
		double scaleForceB2A2;	//��˨�뱻���Ӵ�Ǹֵĳ�������֮��
	public:
		ITEM_TAG_INFO();
		virtual ~ITEM_TAG_INFO(){DetachObject();}
	}*pTagInfo; //��ʱʹ�ò��洢����ʡȥÿ�α�����������ʱ�ظ����ҵ�ǰ����ָ�� wht 09-12-12
	void SetTagInfo(ITAG* pTag){pTagInfo=(ITEM_TAG_INFO*)pTag;}
	//��PART_ITEM�Ѿ��йҽӶ���ʱִ��TagInfo(...), ���򴴽��¹ҽ����ݶ���󷵻�
	ITEM_TAG_INFO* LoadTagInfo();
public:
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
private:
	static ATOM_LIST<ITEM_TAG_INFO> listTagInfo;	//��ʱ�ҽӵĸ��ӵ�������{ANGLE_JOINT}�ϵ����ݶ�������
};
ANGLE_JOINT::ITEM_TAG_INFO* TagInfo(ANGLE_JOINT* pItem);

typedef struct BOLT_SELCOND{
	long d;			//������˨���ֱ��
	char grade[8];	//������˨�ȼ�
	char matStr[8];	//�ɲ��ʱ�ʶ����ɵ��������Ӽ����ʷ���
	long upWidthLim;//��֫��ֽ�����
	BOLT_SELCOND(){memset(this,0,sizeof(BOLT_SELCOND));d=16;strcpy(grade,"6.8");strcpy(matStr,"*");upWidthLim=56;}
}*BOLT_SELCONDPTR;
class CTowerArchiveLibrary
{
public:
	ARRAY_LIST<ANGLE_G_REC>AngleG;
	ARRAY_LIST<STEELMAT>SteelMaterial;
	ARRAY_LIST<JG_PARA_TYPE>AngleSize;
	ARRAY_LIST<TUBE_PARA_TYPE>TubeSize;
	ARRAY_LIST<LSSPACEEX>BoltSpace;
	//CHashList<CLsFamily> BoltLib;	//��ѧ��������˨����޹ز���Ҫ��Ӵ�����־
	//�ֹ����ı�׼����
	int rollend_param_N,uend_param_N,xend_param_N,fl_couple_rule_N;
	int fld_param_N,flp_param_N,flg_param_N,flr_param_N;
	ARRAY_LIST<INSERT_PLATE_PARAM> rollend_param_table;//[200]
	ARRAY_LIST<INSERT_PLATE_PARAM> uend_param_table;//[200]
	ARRAY_LIST<INSERT_PLATE_PARAM> xend_param_table;//[200]
	ARRAY_LIST<FL_COUPLE_RULE> fl_couple_rule_table;//[200]
	ARRAY_LIST<FL_PARAM> fld_param_table;//[200]
	ARRAY_LIST<FL_PARAM> flp_param_table;//[200]
	ARRAY_LIST<FL_PARAM2> flg_param_tbl;//[200]
	ARRAY_LIST<FL_PARAM2> flr_param_tbl;//[200]
	CTowerArchiveLibrary(){
		rollend_param_N=uend_param_N=xend_param_N=fl_couple_rule_N=0;
		fld_param_N=flp_param_N=flg_param_N=flr_param_N=0;
	}
};
class COldVerDataFile{
public:
	struct GROUP_ANGLE{
		long hGroupAngle;
		WORD wiSectStyle;	//0.�Խ�˫ƴ;1.T��˫ƴ
		GROUP_ANGLE(){hGroupAngle=0;wiSectStyle=1;}
		void SetKey(DWORD key){hGroupAngle=key;}
	};
	struct SIMPLE_DATUMPOINT{
		long hNodeOrRod;
		long hNewTmpDatumPoint;
		char ciHidType;	//handle����0.CLDSPoint.handle;'1':CLDSNode.handle;'S':CLDSLinePart.handle->PosStart;'E':CLDSLinePart.handle->PosEnd
		SIMPLE_DATUMPOINT(long _hNode=0,long _hNewDatumPoint=0);
		void SetKey(DWORD key){hNodeOrRod=key;}
	};
	CHashListEx<GROUP_ANGLE> hashGroupAngles;			//��Ҫ��ciMasterIndex��1->0ת������ϽǸ�
	CHashListEx<SIMPLE_DATUMPOINT> hashTmpDatumPoints;	//ֱ��ͨ���ڵ㶨��Ļ�׼�棬����ʱӦ��ʱתΪ��������
	void Empty();
	char SafeAngleWing(long hGroupAngle,char cAngleWing,long doc_type,long version);
};
//typedef BOLT_SELCOND* BOLT_SELCONDPTR;
//from LoadLibrary("WireLayoutEvolver.dll")
typedef bool  (*DefWriteWeatherFunc)(char* weather_buf,unsigned long size);
typedef unsigned long (*DefReadWireLayoutDataFunc)(char* pBuff,unsigned long dwBufSize,long version,long doc_type);
//���ش�pBuffer�гɹ���ȡ���ֽ���
DWORD ReadWireLayoutData(char* pBuff,DWORD dwBufSize,long version=0,long doc_type=0);
class CBriefWeldPart;
class CUniWireModel;
class CExterDataModel;	//�������ģ����CTower����ȫ��Ψһ�����ʶ
#ifdef __LDS_CONTEXT_
struct IMemberDesignCode;
#endif
class CTower : public IModel
{
	long m_nFileDivisionPos;
	bool m_bInitNewObjDefaultInfo;
	CParaCS defaultAcsMirX,defaultAcsMirY,defaultAcsMirZ;
	CFGWORD cfglegMap[160];	//���ھɰ���ĺ���Ǩ�任��ӳ���
public:
	static double W0_EIFFEL_FACTOR;	//����簣�ƶ�ЧӦ��ѹ�ۼ�ϵ��,�ݰ��淶ȷ��
	CHashListEx<CParaCS> Acs;			//������ϵidΪ��ֵ�洢����ϵ
	CTowerArchiveLibrary archiveLibrary;//��Ϊ�汾�Ƚ�ʱ�浵���ø�׼���ݱ�
	CExterDataModel* m_pExterModel;
	ATOM_LIST<CLDSParamPlate> localStdPartLibray;	//���������
	CTower();
	virtual ~CTower();
	long hTower;
	double m_fTowerHeight;	//ȫ���ߵ�λ(m)
	ATOM_LIST<BOLT_SELCOND>listSelBoltCond;	//��˨ѡ����������
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	struct{
		BYTE grade;			//�����Ҷ�6,7,8,9
		double alfa_max;	//ˮƽ����Ӱ��ϵ�����ֵ
		BYTE cLandType;		//�������1,2,3,4
		BYTE cDesignGroup;	//��Ƶ������1,2,3
		int nFreqsRank;		//�������Է���ʱ������ͽ����������ǰnFreqsRank�����ͣ����ֵ������8;
		double Tg;			//��������
		double zeta;		//����Ȧ�=0.02s
		double gamaRE;		//�������������ϵ��,һ���Խ��ȡ0.85��������ȡ0.8
		double alfa[16];	//��������ʱ������ˮƽ����Ӱ��ϵ��
		VIBRAMODE_PROP modes_arr[16];	//�������洢ǰ16�����͵�Ƶ��
	}EarthQuake;
#endif
public:
	bool m_bUserSpecifyNamedHeightZeroZ;//false:��ʾϵͳ�Զ��������Z0��׼ƽ��;true:�����û�ָ��ֵ����
	double m_fNamedHeightZeroZ;			//ָ���������߼���ʱ����0��׼ƽ���Z����ֵ
	double set_NamedHeightZeroZ(double fNamedHeightZeroZ);
	double get_NamedHeightZeroZ();
	__declspec(property(put=set_NamedHeightZeroZ,get=get_NamedHeightZeroZ)) double NamedHeightZeroZ;
	double _IntelliCalNamedHeightZeroZ(bool *pblCalSuccessed=NULL);
public:
	UCS_STRU ucs;
	CLdsPartList Parts;					//�����͸��Ƴɵ�������������Ǹ֡��ְ塢�ֹܡ��۸֡���˨��
	CHashListEx<CONSTRAINT>hashConstraints;
	CXhSimpleList<CATEGORY> listParamCategory;	//��ϵͳȫ�ֲ�������������ʾʱ��Ŀ¼��
	CHashList<DESIGN_PARAM_ITEM>hashParams;	//��ΪKEY4C���ͣ����Ա�ʾ��������ģ���е�ȫ�ֲ���������hashSlaveNodes���ʹ��
	CHashListEx<CATLOG_ITEM> hashCatlogs;	//ϵͳ������Ŀ¼���嵥
	CHashListEx<SLAVE_NODE>hashSlaveNodes;	//��Ϊ(����)�ܿؽڵ�ľ����ֵΪ�ڵ���ܿ���ϢSLAVE_NODE����
	CHashList<CKaoMeAngle>hashKaoMeAngles;	//��ϽǸ��м�ʮ������(��)��ӹ�����Ŀ�ģ�Ǹּ���
	CBlockList Block;
    CLdsList<CLDSNode>  Node;			//�ڵ�
	CLdsList<CLDSModule> Module;		//ģ��
	CLdsList<CLDSPoint> Point;			//��׼��
	CLdsList<CLDSLine>  Line;			//��׼��
	CLdsList<CLDSPlane> Plane;			//��׼ƽ��
	CLdsList<CLDSArcLift> ArcLift;		//Բ��Ԥ����
	CLdsList<CDisplayView> View;		//��ʾ��ͼ
	CLdsList<CBlockReference> BlockRef;
	FOUNDATION_PARA m_xFoundation;
	CLdsList<CLDSFoundation> Foundations;	//ģ�����ʵ��
	CHashListEx<NODERELAINFO> hashNodeRelations;
	virtual void* Get_hashNodeRelations(){return &hashNodeRelations;}
	virtual NODERELAINFO* GetNodeRelaInfo(long hNode){return hashNodeRelations.GetValue(hNode);}
#ifdef __COMMON_PART_INC_
	CLdsList<CSupplementPart> SuppPart;	//���ӹ���
	CLdsList<CLDSPartGroup> PartGroup;
#endif
	const static int BOM_CONFIG = 1;	//���ܻ�ͼ���ݻ���
	const static int SMART_DRAW = 2;	//���ܻ�ͼ���ݻ���
	const static int TOWER_MODEL= 3;	//�汾��־��ʼģ�����ݻ���
	const static int LOGGER		= 4;	//�汾��־ģʽ���������ݻ���
	const static int CLIPPER	= 5;	//���а�
	const static int WIRELOAD_MODEL = 6;//�����ߺ��ؼ���
	const static int RSLT_REPORT= 7;	//��������������
	const static int PRJPARAMPART_LIBRAY=8;	//���̲����������(��Ҫָ���̷�Χ����Եı�׼�������幹��)
	const static int PPE_MODEL	= 9;	//���չ����� wht 16-03-18
	const static int BOM_COMPARE_MODEL=10;	//ʩ��ͼͼ����Ϣ��������ϸ����Ϣ, BOM�Ա�����
	const static int MODEL_SIZE_CHECK =11;	//��άģ�͵�У����Ϣ
	const static int TSA_TURBO_MODEL  =12;	//���ڼ�������Ԫ���㼰��Ԫ�������ʱ��TSAģ�ͻ�����Ϣ
	const static int UNI_WIREPOINT_MODEL  =13;	//�ҵ�ģ��
	CHashPtrList<CAttachBuffer>AttachBuffer;	//���ӻ���
#ifdef __PART_DESIGN_INC_
	CLdsList<CWorkStatus>WorkStatus;	//����״��
	CLdsList<CAttachWindLoad>AttachWindLoad;	//���ӷ����
	CMapList<CElemInfo>Elems;			//�����������ڵ����е�Ԫ����,��ֵΪ��ĩ�˽ڵ�ľ��
	ARRAY_LIST<ELEMINFO_PTR>ElemPtrArr;	//��Elemsͬ����������ʵ����CElemInfo::Id-1Ϊ������Ҫ����ͼ����ʾ���������б�ʶ����
	//ATOM_LIST<CWireLayout>groupWireLayout; ����WireLayoutsEvolvement.h(.cpp)�У�wjh-2014.4.28
	ATOM_LIST<CWindSegment>WindLoadSeg;	//��ѹ����Ϣ
#endif
	ATOM_LIST<CConnectFL>hashConnectFl;
	CHashList<ANGLE_JOINT>hashAngleJoint;
	CHashList<SUB_OBJ>GeEnts;	//��ǰ��ʾ�Ŀɶ�λ��ʶ�ļ���ͼԪ����
	CHashSet<CLDSObject*>hashObjs;//�����ϣ��
	BOOL m_bCooperativeWork;
	char version[20];
	char segstr[MAX_TEMP_CHAR_200+1];
	CObjNoManager NoManager;
	//PARTSET DispPartSet;
	//NODESET DispNodeSet;
	//LINEPARTSET RodSet;
	long m_idServerFile;	//��TMSͬ��ʱ��Ӧ��TMS���ݿ��е��ļ�Id
	WORD m_wiVerSerial;		//���������в�����TMS�ļ��汾�洢��ˮ��(�����ݸ�ʽ�޹�) Ĭ��ֵΪ0 wjh-2017.1.17
	THANDLE m_hActiveModule;	//��ǰ���ģ��
	THANDLE m_hActiveStatus;	//��ǰ�����
	WORD m_arrActiveQuadLegNo[4];	//��ǰ��һ~�����޵ĵ�ǰ���Ⱥ�
	CFGWORD cfgword;		//���й���������ĺ�
	THANDLE handle;				//��ǰ�����������
	BOOL m_bDataModified;		//�����޸ı�ʶ
	WORD m_iCurLeg;				//��ǰ��ʾ�Ľ��ȣ����������ã�
	SEGI m_iCurSeg;				//��ǰ�����ķֶκ�(ACAD��ͼʱ����)
	double m_fMaxXieJGLen;		//���б�ĳ���(mm)
	BYTE m_cStructureType;		//�ṹ���ͣ�0�����������������(���ǰ��ƶ���ѹЧӦ)��1: ��ͨ(���)���Ϲ���
	union{
		DWORD user_pipeline_no;		//�û���ˮ��
		DWORD m_uiOriginalDogKey;	//�ļ��������ʱ�ļ�������
	};
	DWORD m_uiLastSaveDogKey;	//�ļ����һ�δ洢ʹ�õļ�������
	CString m_sPrjCode;		//���̱��
	CString m_sCompanyName;		//��Ƶ�λ
	char m_sPrjName[MAX_TEMP_CHAR_50+1];		//��������
	char m_sTaType [MAX_TEMP_CHAR_50+1];		//����
	char m_sTaAlias [MAX_TEMP_CHAR_50+1];		//����
	char m_sTaStampNo [MAX_TEMP_CHAR_50+1];		//��ӡ��
	char m_sOperator [MAX_TEMP_CHAR_50+1];		//����Ա���Ʊ��ˣ�
	char m_sAuditor [MAX_TEMP_CHAR_50+1];		//�����
	char m_sCritic [MAX_TEMP_CHAR_50+1];		//������
	int m_nTaNum;				//������
	/** ����TMAV5.1.1.0֮ǰ�汾��ģʽ���е�λ���� wjh-2016.4.28
		��TMAV5.1.1.0�������߽���ģʽ��CLDSPoint������������֫����ƫ�����Ĵ�ӽǸֵ�λ��������ϸ΢������
		Ϊ�����¾ɰ汾��ʽת������������Կ���
	*/
	BYTE m_ciRepositionAlgor;//0.��ʾ��ǰ;1:V5.1.1֮ǰ;2.V5.1.7֮ǰ;������Խ��Խ�£�m_bRepositionByModeBeforeV51;
#ifdef __COMMON_PART_INC_
	//�ְ��ߴ���ʽ     0.չ���󵹽Ǵ�Ϊ����ֱ�� 
	//					   1.���ٵ�����֤������߳���ΪL-N 
	//					   2.���ٵ�����֤����ڲ�߳���ΪL-N  wht 11-04-18
	//�ְ�ֻ��0��1�֣� �����0��1��2����
	BYTE m_ciRollProcessType;
	//�ְ����ߵļ��㷽ʽ 0--���(��ǰ����ʽ)
	//					   1--�ڲ�(���ڴ���ʽ)
	BYTE m_ciRollOffsetType;	//���ƫ�Ʒ�ʽ����������ߺ;���ߣ�
	//�ְ�������δ���ʽ 0.װ���� 1.���Բ� 2.͹��  3.����
	BYTE m_ciDeformType;
	double m_fDeformCoef;	//��������ϵ��
	double m_fHuoquR;		//�����뾶
	C2dInterpolator mapNeutroCoef,mapBendR;
#endif
	//CBuffer m_bufferDrawingScript;	//��ͼ�ű�������,��Ҫ�洢	wht 10-12-14
	//CBuffer m_bufferSumTaMatCfg;		//���ϻ��������ļ�������,��Ҫ�洢
#ifdef __PART_DESIGN_INC_
	long m_iIteration;		//�������
	WEATHER_REGION weather;
	char weatherCode[50];
	long volt_grade;		//��ѹ�ȼ�:330,500
	double m_fIceDensity;	//���������ض�kg/m3
	double m_fVibrationT1;	//������������(s)
	char m_cLandscape;	//��λ����:'A','B','C','D'
	char m_cLineRoadType;//��··��:0ƽ·��·;1������·2ɽ����·
	/*��ȫ�ȼ�(�����ṹ��Ҫ��ϵ��gama0)��
	0:һ���ر���Ҫ�ĸ����ṹ(gama0=1.1)��
	1:����������ѹ��·�ĸ������(gama0=1.0)��
	2:������ʱʹ�õĸ������(gama0=0.9).*/
	short m_siSafetyGrade;
	double m_fSafetyCoef;	//�ṹ��Ҫ��ϵ����o
	//������Ʋ�������̹淶��أ�
	int m_iLoadExecuteCode;		//0:������·�淶1:���ʽṹ�淶2:�����ṹ���ع淶3:�û�ָ��;���ؼ���ִ�й淶������ϵ������������ϵ�����ɱ�������ϵ��������ϵ��ȡֵ����ѭ�Ĺ淶
	int m_iMemberSizeCheckCode;	//�����������ѡ�Ĺ淶0:������·�淶1:���ʽṹ�淶2:����ASCE10-97
	int m_iSlendernessLimCode;	//�˼���ϸ�����ƹ淶0:������·�淶1:���ʽṹ�淶2:����ASCE10-97��3:�û�ָ��
	double m_fGamaGForDesign;		//�������ʱ�������ط���ϵ��
	double m_fGamaGForFoundation;	//�������ʱ�������ط���ϵ��
	double m_fGamaQForTowerWindLoad;//�����������ӷ���ط���ϵ��
	double m_fLimitBearCoef;		//����ϵ��������ֵ
	bool m_bWeightAmplifyCoefIncBraceRods;	//��ѹ������ϵ�����Ƿ�������ĸ˼� wjh-2019.7.13
	bool m_bAreaAmplifyCoefIncBraceRods;	//��ѹ������Ŵ�ϵ�����Ƿ�������ĸ˼� wjh-2019.7.13
	//��Ӧ���Ż��ȲĲ���
	struct{
		long m_nMainPoleSlendernessLimit;
		long m_nXiePoleSlendernessLimit;
		long m_nAuxPoleSlendernessLimit;
		long m_nTensivePoleSlendernessLimit;
		long m_nMainTubeSlendernessLimit;
		long m_nXieTubeSlendernessLimit;
		long m_nAuxTubeSlendernessLimit;
		long m_nTensiveTubeSlendernessLimit;
		char m_csMinAngleSize[10],m_csMinMainAngleSize[10],m_csMinForceAngleSize[10];
		BOOL m_bPreferSingleAngle;
		int  m_iSizeOrderRule;	//0.��������������1.����ת�뾶�������2.��Ⱥ������
		long m_nMaxSelMatCount;
		long m_nConvergeLim;	//ѡ��������ֵ
	}SelMatPara;
#endif

//���г�Ա����
public:
	//�����Ѷ�ȡ��Ta.Module��ʼ���ɰ�LDSV1.2.0.6(����)֮ǰ��ĺŵ��ݽ���Ǩӳ���
	bool InitEvolveCfgwordMap(int version,long doc_type);
	bool EvolveCfgword(CFGWORD& cfgword,int version,long doc_type);	//�ɰ�LDSV1.2.0.6(����)֮ǰ��ĺŵ��ݽ���Ǩ
	bool ReplaceObjBodyNo(const int* fromBodyNoArr,const int* toBodyNoArr,int nBodyNo=1);
	bool ReBuildObjsHashTable();
	long GetMinIdleHandle(long hStart=0x20);
	//��������
	bool IsNeedInitNewObjDefaultInfo(){return m_bInitNewObjDefaultInfo;}
	void InitTower();
	CLDSModule* InitModule();
	void Empty();
	BOOL SetDocModifiedFlag(BOOL bModified=TRUE);
	BOOL IsDataModified();
	BOOL SelectNodeSet(NODESET* pNodeSet,char* key,...);

	//��û�����Ϣ
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	void CorrectErrorBefore(long nVersion,long doc_type,BOOL bHasRepeatHandle);
	//��öκ�������߶κ��ַ�����֧��ָ��ģ�� wht 10-11-04
	//bEqualCfg=TRUEʱ�����ָ��ģ����ĺ���ȫ��ȵĶκ�������ַ��� wht 11-02-19
	int GetSegStr(char* seg_str,CLDSModule *pModule=NULL,BOOL bEqualCfg=FALSE);
	int GetSegNoList(ATOM_LIST<SEGI> &segList,CLDSModule *pModule=NULL,BOOL bEqualCfg=FALSE);
	static CXhChar200 SegNoListToString(ATOM_LIST<SEGI> &segList);
	int GetSegNoHashList(CHashList<SEGI> &segNoHashList,CLDSModule *pModule=NULL,BOOL bEqualCfg=FALSE);
	//�����¶κ��޸�ĳ�ι�������
	void ModifyPartNoByNewSegNo(SEGI oldSegI,SEGI newSegI);
	void ToTowerSolidDataExchangeBuffer(BUFFER_IO* pIO,BUFFER_IO *pObjHandleIO=NULL);
	void ToTowerAttachBuffer(CBuffer &buffer);
	bool ExportTowerSolidDataExchangeFile(char *file);
	CLDSFoundation* AddFoundation(BYTE ciHeightSerial,BYTE ciLegSerial,BYTE ciLegQuad);
	CLDSFoundation* GetFoundation(BYTE ciHeightSerial,BYTE ciLegSerial,BYTE ciLegQuad);
	bool RetrieveWireModelFromNodes(CUniWireModel* pWireModel);
	UINT RetrieveWireHoleSet(CUniWireModel* pWireModel);
#endif
	CFGWORD GetDefaultCfgPartNo(int iQuad=0);
	CLDSModule *GetActiveModule();
	//
	BOOL SetDesignItem(DWORD dwKey,DESIGN_PARAM_ITEM& param,BOOL bModify=FALSE);
	BOOL SetDesignItemValue(DWORD dwKey,double fValue,int iDataType=0,BOOL bModify=FALSE);
	//��ʾ��ͼ��������
	CDisplayView *AppendDisplayView(){return View.append();}
	CDisplayView *GetFirstDisplayView(){return View.GetFirst();}
	CDisplayView *GetNextDisplayView(){return View.GetNext();}
	int GetDisplayViewNum(){return View.GetNodeNum();}

	THANDLE (*GetCooperativeNewHandle)();
	void GetTaSizeScope(SCOPE_STRU &scope);
	long GetSlopeList(ARRAY_LIST<GEPOINT2D>& joint_z_arr,ATOM_LIST<f3dLine>* slopeset);
	//ref_part_arrָ������˨���õĹ�������
	int GetLsRefCounts(THANDLE hLs,long **ref_part_arr=NULL);
	//���ݲ�ѯ
	CLDSNode *FromPointI(long point_i);
	//idClassType����ָ�����Ͷ���ID���滻��һ���ò�����Ҫ���ڶ���������ʱ����ͬ��������ظ�������
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	//�Ƴ���ָ�������������������
	int RemoveRelativeObjs(CLDSDbObject *pDbObj);
	int RemoveNodeRelativeObjs(CLDSNode *pNode);
	BOOL AssertValid();
	PROGRESS_CONTEXT progress;
	void (*DisplayProcess)(int percent,const char *sTitle);	//������ʾ�ص�����
	void (*ParseDrawingScriptBuffer)(CBuffer &buffer);	//������ͼ�ű�����
	void PrepareOldVerInfo(COldVerDataFile* pOldVerInfo,int doc_type,long version);
	bool FromBuffer(CBuffer &buffer,long doc_tma1_lma2_tsa3_lds4_default0=0,bool loadLibToArchive=false);
	void ToBuffer(CBuffer &buffer,DWORD cursor_dog_no,char *file_version,char ciDocType=0);
	void ToXmlFile(FILE* fp,DWORD schema,DWORD cursor_dog_no,char *file_version);
	void FromAttachBuffer(CBuffer &buffer);
	void ToAttachBuffer(CBuffer &buffer);
	bool IsHasRsltBuffer(const char* itemKeyStr);
	bool RemoveRsltBuffer(const char* itemKeyStr);
	bool ReadRsltBuffer(const char* itemKeyStr,CBuffer &buffer);
	bool WriteRsltBuffer(const char* itemKeyStr,CBuffer &buffer);
	DWORD ModelContentsLength(){return m_nFileDivisionPos;}
	void SetModelContentsLen(DWORD dwFileLen){m_nFileDivisionPos=dwFileLen;}
	void WriteAdditionalBuffer(CBuffer &buffer);
	void ReadAdditionalBuffer(CBuffer &buffer);
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	UINT GetNewStatNo();
	double SafetyCoef();
	static DefWriteWeatherFunc WriteWireLayoutWeather;
	static DefReadWireLayoutDataFunc ReadWireLayoutData;
#ifdef __LDS_CONTEXT_
	IMemberDesignCode* DesignCode();	//���ص�ǰ��Ԫ�����ִ�еĹ淶/�涨
#endif
	//����һ������ڵ���������ڵ����и˼�(�����߽�˼�)��ͶӰ��ѹ�������(m2)
	//double SnapNodeFace(NODESET &nodeset,f3dPoint prjOrg,double W,int iLoadExecuteCode,
	//						BOOL bAutoCalK,double *weighted_coef,double *net_area,double *BtoH);
	//�����ѹ�εĳ��ܷ�ѹ��ͶӰ�����iLoadExecuteCode=0:������Ƽ����涨1:���ʹ淶2:�����ṹ���ع淶
	//void CalWindSegAf(int iLoadExecuteCode);
	//void CalWindLoad(double preferredBetaZ=0.0,double magnifyCoef=1.0);		//�����ѹ�εĳ��ܷ�ѹ����
	//��ԭʼ������ʵ�ʹ���ͬ�������ü��㹦����������������WireLoad.exe��
	//void SynchronizeWorkStatus(CWireLayout *pWireLayout);
	//���㵼������������,�ü��㹦����������������WireLoad.exe��
	//bool CalWireLoad(CWireLayout *pWireLayout,char* sError,int max_error_len=100);
	//��*.frq�ļ��ж�ȡ�����������ͷ���ʱ���ڵ�ĵ�Ч�ڵ�����
	long ReadVibraNodeMass(FILE *fp,DYNAMIC_ANAL_NODE *node_arr);
	//iMode��0��ʼ��������ʾ��iMode+1������
	double ReadVibraMode(FILE *fp,int iMode, bool iReadAllFreqs=false);	//���ӵ�ǰ���͵�����Ƶ��
	BOOL ImportLoadFile(char *file,double z2_coef=1.0);
	BOOL ExportLoadFile(char *file,double z2_coef=1.0);
	int GetLoadNodeNum();
	CWorkStatus *GetActiveWorkStatus();
	void AppendGeneralStatus();
    CLDSNode* GetForceNodeByIndex(int index);	//���������Ż�ýڵ�
	int GetForceNodeNum();	//��������Ķ˵�����
	int GetForcePoleNum();	//����������
	CLDSNode* FromPointI(int point_i);	//���ݽڵ��Ż�ýڵ�
	int GetForceIndexFromHandle(THANDLE handle);//���ݾ����ýڵ������Ķ˵�������
	int GetNodeIndexFromHandle(THANDLE handle);
#endif
	void RetrieveUdfViews();
	void RetrieveDrawingViews();
	bool UpgradeToCtrlNode(CLDSNode* pNode);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	BOOL CheckPartGroupValidAndCorrect(CLDSPartGroup *pPartGroup,BOOL bAutoCorrect);
	void AmendPlateWeldLocationY();
	void RetrieveWeldLineInfo();
	void RetrieveBriefWeldPartGroups(CHashListEx<CBriefWeldPart>& hashPartGroups,const char* segstr="*");
	void RefreshPartGroups(CHashList<SEGI>& hashSegTbl,BOOL bDisplayProcess=FALSE);
	int RetrievePolyPartList(POLYPARTLIST &list);
	static int RetrievePolyPartList(IModel *pModel,POLYPARTLIST &list);
	//base���ʾ��˨�Ļ���λ��(�������ϵ��),ucs��ʾһ����˨�����ڵ��������ϵ,
	//������ϵ������һ��������˨�Ĺ��������࿿��,iNormX0Y1Z2�ͱ�ʾ
	//����˨���������࿿�����������ϵ��������
	CLDSBolt* FindLsByUcsPos(f3dPoint base,UCS_STRU ucs,int iNormX0Y1Z2,double epsilon=EPS2);
	CLDSBolt* FindLsByPos(f3dPoint base,f3dPoint work_norm,double epsilon=EPS2);
	void RefreshBurglarLs();//ˢ�·�����˨
	void RefreshLockPad();	//ˢ�·��ɵ�Ȧ
	CLDSObject* FromPartNo(const char *sPartNo);
	//ɾ������
	int DelBoltAndRelativeBoltRef(THANDLE ls_handle,BOOL bIncPerm=TRUE);
	BOOL DelTubeEndLj(THANDLE handle);	//ɾ���ֹܶ����� ��Ҫ����ɾ��ʮ�ֲ��
	//����IModel���� wjh-2014.7.01
	//void SmartCalAngleWingNorm(CLDSLineAngle *pLineAngle);
#endif
	// Undo&Redo��������ʵ����
private:
	bool m_bGroupUndoStart;	//���������е�BeginUndoListen��EndUndoListen��Ч
	enum UNDO_OBJECT_TYPE{UNDO_DELETE_OBJ,UNDO_INSERT_OBJ,UNDO_MODIFY_OBJ};
	// �洢Undo��Redo�Ĳ�����¼
	CCirBufferStack UndoBufferStack;
	// �洢ĳһ����ǰ������ݱ仯��Ϣ
	CBuffer UndoBuffer;
	//���һЩ��Ч�ľ������Ӧ����Ķ����Ѳ����ڣ����Ա���������ʱ�����������⡡wjh-2015.3.8
	void CleanInvalidHandleRef();
public:
	bool GroupUndoStarted(){return m_bGroupUndoStart;}
	void StartUndoGroup();	//���ó���Undo����
	bool EndUndoGroup(int iOperType=0);	//��ֹ����Undo����
	// ��ʼ����Undo����
	void BeginUndoListen(void);
	// ��ʼ����Undo����
	bool EndUndoListen(int iOperType=0);
	// ִ��Undo����
	void UndoTower(void);
	// ִ��Redo����
	void RedoTower(void);
	// ����ִ��Undo����
	BOOL CanUndoOper(void);
	// ����ִ��Redo����
	BOOL CanRedoOper(void);
	// ���ص�ǰUndo��¼�Ĳ�������
	int CurrentUndoType();
	// �Ƿ����µ�Undo��Ϣ��Ҫ�洢
	BOOL IsHasUndoInfo();
	bool CompareVersion(CTower* pSrcTower,void* pLogContents);
	//bool ImportVersionLog(CTower* pSrcTower,CBuffer& buffer,void* pLogContents);
public:
	long (*GetNewHandleHookFunc)();	//������Ҿ�����ɺ���
 	long GetNewHandle();
	virtual DWORD ShrinkObjHandle(DWORD hStartShrink=0x20);//>=hStartShrink�Ķ������Żᱻ���������
	virtual long GetMinAvailableHandle();//��ʼ����С���ܵ�ͼԪ���;
	virtual bool IsTowerModel(){return true;}
	virtual BYTE RepositionAlgor(){return m_ciRepositionAlgor>0?m_ciRepositionAlgor:255;}
	virtual BYTE GetRollProcessType(){return m_ciRollProcessType;}
	virtual BYTE GetRollOffsetType(){return m_ciRollOffsetType;}
	virtual BYTE GetDeformType(){return m_ciDeformType;}
	virtual double GetDeformCoef();
	virtual double GetHuoquR(){return m_fHuoquR;}
	//��ȡ����ģ�Ϳռ���ֱ�ӹ���ʵ�ʴ��ڵĹ������飬filterShadowPart=true��ʾֻ����ĸ��������Ӱ�乹�������S��
	//pPolyPartListΪCLdsList<CLDSPolyPart>*����
	virtual bool GetRealityPartArr(DYN_ARRAY<CLDSBolt*>& arrBoltPtr,DYN_ARRAY<CLDSPart*>& arrPartPtr,
								   void *pPolyPartList,bool filterShadowPart=false);
	virtual CDisplayView *FromViewHandle(long h){return View.FromHandle(h);}
	virtual CLDSObject *FromHandle(long h,BOOL bDbObjOnly=FALSE);
	virtual CLDSPoint *FromPointHandle(long h){return Point.FromHandle(h);}
	virtual CLDSLine *FromLineHandle(long h){return Line.FromHandle(h);}
	virtual CLDSPlane *FromPlaneHandle(long h){return Plane.FromHandle(h);}
	virtual CLDSNode *FromNodeHandle(long h){return Node.FromHandle(h);}
	virtual CLDSPart *FromPartHandle(long h,int CLASS_TYPEID=0){return Parts.FromHandle(h,CLASS_TYPEID);}
	virtual CLDSPart* FromPartHandle(THANDLE h,int CLASS_TYPEID1,int CLASS_TYPEID2)
	{		return Parts.FromHandle(h,CLASS_TYPEID1,CLASS_TYPEID2);	}//���ݾ����ýڵ�
    virtual CLDSPart* FromPartHandle(THANDLE h,int CLASS_TYPEID1,int CLASS_TYPEID2,int CLASS_TYPEID3)
	{		return Parts.FromHandle(h,CLASS_TYPEID1,CLASS_TYPEID2,CLASS_TYPEID3);	}//���ݾ����ýڵ�
	virtual CLDSLinePart *FromRodHandle(long hRod);
	virtual CLDSAnchorBolt* FromAnchorBolt(long hAnchorBolt);
	virtual CLDSArcLift *FromArcLiftHandle(long h){return ArcLift.FromHandle(h);}
public:	//Լ����صĺ���
	virtual CONSTRAINT* AppendConstraint(CONSTRAINT* pCopyFrom=NULL);
	virtual CONSTRAINT* GetConstraint(long constraint_id);
	virtual bool RemoveConstraint(long constraint_id,long hFromMasterObj=0);
	virtual CONSTRAINT* EnumConstraintFirst(BOOL bIterDelete=FALSE);
	virtual CONSTRAINT* EnumConstraintNext(BOOL bIterDelete=FALSE);
	virtual void ClearConstraintSyncStates();
public:	//��������
	virtual long GetObjNum(int CLASS_TYPEID);
	virtual CDisplayView* EnumViewFirst(BOOL bIterDelete=FALSE){return View.GetFirst(bIterDelete);}
	virtual CDisplayView* EnumViewNext(BOOL bIterDelete=FALSE){return View.GetNext(bIterDelete);}
	virtual CLDSNode* EnumNodeFirst(BOOL bIterDelete=FALSE){return Node.GetFirst(bIterDelete);}
	virtual CLDSNode* EnumNodeNext(BOOL bIterDelete=FALSE){return Node.GetNext(bIterDelete);}
	virtual CLDSPart* EnumPartFirst(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE){return Parts.GetFirst(CLASS_TYPEID,bIterDelete);}
	virtual CLDSPart* EnumPartNext(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE){return Parts.GetNext(CLASS_TYPEID,bIterDelete);}
	virtual CLDSNode* MoveToTailNode(BOOL bIterDelete = FALSE) { return Node.GetTail(bIterDelete); }
	virtual CLDSPart* MoveToTailPart(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE){return Parts.GetTail(CLASS_TYPEID,bIterDelete);}
	virtual CLDSLinePart* EnumRodFirst(){return Parts.GetFirstLinePart();}
	virtual CLDSLinePart* EnumRodNext(){return Parts.GetNextLinePart();}
	virtual CLDSPoint* EnumPointFirst(BOOL bIterDelete=FALSE){return Point.GetFirst(bIterDelete);}
	virtual CLDSPoint* EnumPointNext(BOOL bIterDelete=FALSE){return Point.GetNext(bIterDelete);}
	virtual CLDSLine* EnumLineFirst(BOOL bIterDelete=FALSE){return Line.GetFirst(bIterDelete);}
	virtual CLDSLine* EnumLineNext(BOOL bIterDelete=FALSE){return Line.GetNext(bIterDelete);}
	virtual CLDSPlane* EnumPlaneFirst(BOOL bIterDelete=FALSE){return Plane.GetFirst(bIterDelete);}
	virtual CLDSPlane* EnumPlaneNext(BOOL bIterDelete=FALSE){return Plane.GetNext(bIterDelete);}
	virtual int PushNodeStack(){return Node.push_stack();}
	virtual bool PopNodeStack(int pos=-1){return Node.pop_stack(pos);}
	virtual int PushPartStack(){return Parts.push_stack();}
	virtual bool PopPartStack(int pos=-1){return Parts.pop_stack(pos);}
#ifdef __COMMON_PART_INC_
	virtual CLDSPartGroup* EnumPartGroupFirst(){return PartGroup.GetFirst();}
	virtual CLDSPartGroup* EnumPartGroupNext(){return PartGroup.GetNext();}
	virtual int GetPartGroupPartSet(long hPartGroup,PARTSET &partset,int CLASS_TYPEID=0,bool bClearLock=true,long* phExcludePartBranchArr=0,int count=1);
	virtual long GetPartGroupNum(){return PartGroup.GetNodeNum();}
	UINT AddJointTubeWeldRelations(CLDSPart* pTubePart);	//����ԽӸֹ���Ϣ(ȱ�ٺ��Ӹ�����������ͬ���ӹ�ϵ) wjh-2017.10.14
	UINT AddJointTubeWeldRelations(CHashList<SEGI>* pFilterSegHashtable=NULL);
#endif
public:	//�������ݶ���
	virtual CDisplayView *AppendView(BOOL bGetNewHandle=TRUE){return View.append(bGetNewHandle);}
	virtual CLDSPoint *AppendPoint(BOOL bGetNewHandle=TRUE){return Point.append(bGetNewHandle);}
	virtual CLDSLine *AppendLine(BOOL bGetNewHandle=TRUE){return Line.append(bGetNewHandle);}
	virtual CLDSPlane *AppendPlane(BOOL bGetNewHandle=TRUE){return Plane.append(bGetNewHandle);}
	virtual CLDSNode *AppendNode(BOOL bGetNewHandle=TRUE){return Node.append(bGetNewHandle);}
	virtual CLDSPart *AppendPart(int CLASS_TYPEID,BOOL bGetNewHandle=TRUE){return Parts.append(CLASS_TYPEID,bGetNewHandle);}
	virtual CLDSAnchorBolt* AppendAnchorBolt();
public:	//ɾ�����ݶ���
	virtual BOOL DeleteView(long h,BOOL bIncPerm=TRUE){return View.DeleteNode(h,bIncPerm);}
	virtual BOOL DeleteNode(long h,BOOL bIncPerm=TRUE){return Node.DeleteNode(h,bIncPerm);}
	virtual BOOL DeletePart(long h,BOOL bIncPerm=TRUE){return Parts.DeleteNode(h,bIncPerm);}
public:
	static BOOL IsSonPart(CLDSDbObject *pObj,CFGWORD *pBodyWord=NULL,CFGWORD* pLegWord=NULL);
	//����װ��ʱ����λ��ƥ��ڵ�
	CLDSNode* MatchExterNode(const double* nodecoords);
	CLDSLinePart* MatchExterRod(int idClsType,BYTE cbValidProp,const double* xStartPosCoords,const double* xEndPosCoords,const double* vxWingNorm=NULL,const double* vyWingNorm=NULL);
	CLDSBolt* MatchExterBolt(WORD d,WORD wL,const double* xBasePosition,const double* vWorkNorm);
	//����Ƿ���ָ��(��ǰ)ģ���д����ص��Ľڵ�
	CLDSNode* CheckOverlappedNode(f3dPoint pos,const CFGINFO& cfginfo,bool bFromStart=true,long hIgnoreHandle=0);
	CLDSNode* CheckOverlappedNode(f3dPoint pos,CLDSModule* pInModule=NULL,CFGWORD* pBodyWord=NULL,CFGWORD* pLegWord=NULL);
	//����Ƿ���ָ��(��ǰ)ģ���д����ص��ĸ˼�
	CLDSLinePart* CheckOverLappedLinePart(f3dPoint start,f3dPoint end,const CFGINFO& cfginfo,long hIgnoreHandle=0,const double* vxWingNorm=NULL,const double* vyWingNorm=NULL);
	CLDSLinePart* CheckOverLappedLinePart(f3dPoint start,f3dPoint end,CLDSModule* pInModule=NULL,CFGWORD* pBodyWord=NULL,CFGWORD* pLegWord=NULL);
	CLDSLinePart* CheckOverLappedLinePart(f3dPoint start,f3dPoint end,CLDSDbObject* pTogetherLiveWith);
	//����Ƿ���ָ��(��ǰ)ģ���д����ص�����˨
	CLDSBolt* CheckOverLappedBolt(const double* xBasePosition,const double* vWorkNorm,long hIgnoreHandle=0);
	//���ܺϲ������뵱ǰģ���д��ڵ��ص��Ǹֺ���˨
	BOOL IntelliMergeLineAngles(CLDSLineAngle* pModelJg,CLDSLineAngle* pBlockJg);
	BOOL IntelliMergeBolt(CLDSBolt* pModelBolt,CLDSBolt* pBlockBolt);
	//�Ƿ����ļ�����֮ǰ�İ汾
	static bool IsPrevDeflateVersion(long version,long doc_type);
	static BOOL InitDisplaySetByPolyRgn(POLYGON &selRgn,CLDSDbObject* pSameLifeRefObj=NULL,
		NODESET* pRsltNodeSet=NULL,PARTSET* pRsltPartSet=NULL,bool rapidmode=false);
	static BOOL InitDisplaySetByMirPoint(CLDSLinePart* pLinePart,CLDSNode* pMirStart,CLDSNode* pMirEnd);
public:	//װ������ϵ��غ���
	virtual CParaCS *AppendAcs();				//����װ������ϵ
	virtual BOOL DeleteAcs(long hAcs);			//ɾ��װ������ϵ
	virtual CParaCS *FromAcsHandle(long hAcs);	//���ݾ����ѯװ������ϵ
};
BOOL VersionOldLTMAFile(char* lic_file,char* version_convert_exefile,CBuffer &buffer);
void DeleteNodePtrFromDisplaySet(CTower* pTower,CLDSObject *pNode);
class CUndoOperObject
{
	CTower *m_pTower;
	bool m_bObjectStart;
	bool m_bGroupUndo;
	int  m_iOperType;
public:
	static const int UNDO_REPOSITION = 1;
	CUndoOperObject(CTower *pTower,bool bGroupUndo=false,int iOperType=0,bool bStartListenInstant=true);
	~CUndoOperObject();
	bool StartUndoListen();
};
#endif // !defined(AFX_TOWER_H__06FC49B6_50F1_4315_A1B2_BAFF7A74AA17__INCLUDED_)
