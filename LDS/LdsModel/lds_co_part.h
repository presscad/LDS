// tma_co_part.h: interface for the CJoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TDA_CO_PART_H__415A1A1F_534B_4ACC_B588_13AA8178D691__INCLUDED_)
#define AFX_TDA_CO_PART_H__415A1A1F_534B_4ACC_B588_13AA8178D691__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __COMMON_PART_INC_
#include "lds_part.h"
#include "lds_list.h"
#include "ldsptr_list.h"
//��ͷ
typedef struct tagPART_ITEM
{
	long h;
	char cID;
}PART_ITEM;
class CJoint : public CLDSPart
{
	ATOM_LIST<PART_ITEM> *part_list;//�ӹ������������ְ塢�Ǹֵȣ�
public:
	CJoint();	//����
	~CJoint();	//����
	// ��ͷ��ʽ0.����� 1.������� 2.�ڰ�������ְ� 3.������ڰ��ְ�4.�ڰ���X֫�ְ�ض�
	// 5.�ڰ���Y֫�ְ�ض�6.�İ���X֫�ְ�ض�7.�İ���Y֫�ְ�ض�8.ʮ�ֽ�ͷX֫�ְ�ض�9.ʮ�ֽ�ͷY֫�ְ�ض�
	long joint_style;
	double length;	//��ͷ����
	//0:Y��Գ�;1:X��Գ�;2:Z��Գ�
	THANDLE mir_joint_handle[3];
	THANDLE base_node_handle;
	THANDLE base_jg_handle_arr[2];
	THANDLE get_hMainAngle() { return base_jg_handle_arr[0]; }
	THANDLE set_hMainAngle(THANDLE hAngle) { return base_jg_handle_arr[0] = hAngle; }
	__declspec(property(put = set_hMainAngle, get = get_hMainAngle)) THANDLE hMainAngle;	//��ͷ�Ǹ�
	THANDLE get_hCoupleAngle() { return base_jg_handle_arr[1]; }
	THANDLE set_hCoupleAngle(THANDLE hAngle) { return base_jg_handle_arr[1] = hAngle; }
	__declspec(property(put = set_hCoupleAngle, get = get_hCoupleAngle)) THANDLE hCoupleAngle;	//��ͷ�Ǹ�
	bool blInheritExistBoltsInMainAngle, blInheritExistBoltsInCoupleAngle;
	bool blIntelliCalOddment;	//�Զ�����Ǹֶ���ƫ����(����ͷ)
public:
	CLsRefList boltRefList;
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//void Serialize(CArchive &ar);
	CLDSLineAngle *AppendWrapJg(char cId);
	CLDSLineAngle *AppendWrapJg(CLDSLineAngle *pJg,char cId);
	BOOL AppendWrapJg(long h,char cId);
	//CLDSPolyAngle *AppendHuoQuWrapJg(char cId);
	//CLDSPolyAngle *AppendHuoQuWrapJg(CLDSPolyAngle *pHuoQuJg,char cId);
	CLDSPlate* AppendWrapPlank(char cId);
	BOOL AppendWrapPlank(long h,char cId);
	CLDSPlate* AppendWrapPlank(CLDSPlate *pPlank,char cId);
	CLDSPart *GetFirstPart();
	CLDSPart *GetNextPart();
	PART_ITEM *GetFirstPartItem(){return part_list->GetFirst();}
	PART_ITEM *GetNextPartItem(){return part_list->GetNext();}
	long GetPartCount(){return part_list->GetNodeNum();}
	PART_ITEM *FromPartId(char cId);
	void EmptyAllSubPart();
	void EmptyAllSubPartH(){part_list->Empty();}
};

//�����Ǹ���,ÿһֱ�߶�ģ��
class CHuoQuSeg{
public:
	CLDSNode *pStart,*pEnd;
	THANDLE handle;
	f3dPoint start_xyz,end_xyz;
	f3dPoint norm_x_wing,norm_y_wing;
	double huoqu_r;
	CHuoQuSeg();
public:
	void getUCS(UCS_STRU& ucs);
};
//�˼����ε�
class CLDSPolyJoint : public CLDSPart
{
public:
	static UINT crPolyJoint;
	double rBending;	//����ʱ���������Բ���뾶(mm)
	CLDSLinePart *pPrevLinePart,*pNextLinePart;
	CLDSPolyJoint();
	~CLDSPolyJoint();
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//���Թ���
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	int GetPropValueStr(long id,char* valueStr);		//��������ID�õ�����ֵ
	static void InitPolyJointPropHashtable();
	UPDATE_PROPLISTITEM_FUNC(CLDSPolyJoint,propHashtable)	//������������º���
	static long GetPropID(char* propStr);	//�õ�����ID
	static int GetPropStr(long id,char* propStr);		//�õ������ַ���
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
};

class CLDSGroupLineAngle : public CLDSLineAngle
{
	union GROUP_STYLE{
		long dwStyle;	//ʵ��״̬ʱ�Ļ����Ǹ������ʽ0:�Խ�;1:T��;2:ʮ�ֽ�����
		struct{
			WORD wSectStyle;	//��������
			BYTE ciMasterIndex;	//������ϽǸֵ������ӽǸ�������0 based index
			BYTE ciMasterDirectIndex;	//֫����,0:������ϽǸ�;1:1���ӽǸ�;2:2���ӽǸ�,����3��4���ӽǸ�
		};
	}style;
	double _jg_space;	//�ӽǸ�֮���϶
public:	//��������
	PROPERTY(WORD,group_style);
	GET(group_style);
	SET(group_style);
	PROPERTY(WORD,wSectStyle);
	GET(wSectStyle){return group_style;}
	SET(wSectStyle){group_style=value;}
	
	PROPERTY(BYTE,ciMasterIndex);	//��׼�ӽǸֵ������ţ�0 base index
	GET(ciMasterIndex);
	SET(ciMasterIndex);
	PROPERTY(BYTE,ciMasterDirectIndex);	//֫����,0:������ϽǸ�;1:1���ӽǸ�;2:2���ӽǸ�,����3��4���ӽǸ�
	GET(ciMasterDirectIndex);
	SET(ciMasterDirectIndex);
	//
	READONLY_PROPERTY(long,m_hDatumSonAngle);
	GET(m_hDatumSonAngle);
	READONLY_PROPERTY(CLDSLineAngle*,m_pDatumSonAngle);
	GET(m_pDatumSonAngle);

	PROPERTY(double,jg_space); 
	GET(jg_space){
		CLDSGroupLineAngle* pMotherObj=(CLDSGroupLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _jg_space;
		else
			return pMotherObj->jg_space;
	}
	SET(jg_space){
		if(value>=0)
		{
			CLDSGroupLineAngle* pMotherObj=(CLDSGroupLineAngle*)MotherObject();
			_jg_space= value;
			if(pMotherObj!=this)
				pMotherObj->jg_space=value;
		}
	}
public:
	virtual char* GetBriefPartTypeMark();
	bool IsNeedUpdateGroupStyle(){return style.wSectStyle!=group_style;}
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void UpdateSonJgPerm();
	int AppendSonJg();
	virtual bool SetDirectMasterAngle();	//�趨��ϽǸ�����Ϊ��ϽǸּ��ӽǸֵ�֫���߼����׼�Ǹ� wjh-2016.9.19
	void cal_y_wing_norm();
	void cal_x_wing_norm();
	virtual void CalPosition(bool bIgnoreRepeatFlag=FALSE);
	virtual void CalStartOddment(char ciAutoCalMode=0,int liInitOddment=0);	//0.��ͨ��ײ������;1.ǿ���԰�ָ����ͷ�������ײ������;2.�������н�ʱ��֤�нǱ߾��������ײ������
	virtual void CalEndOddment(char ciAutoCalMode=0,int liInitOddment=0);	//0.��ͨ��ײ������;1.ǿ���԰�ָ����ͷ�������ײ������;2.�������н�ʱ��֤�нǱ߾��������ײ������
	void UpdateSonJgBasicInfo();
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void ToNormInfoBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromNormInfoBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual bool IsBelongTo(int iClassTypeId)const;
	//��Эͬ����ͨ���ܵ���ȡ����
	virtual void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	//ͨ���ܵ���Эͬ����д������
	virtual void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	void UpdateSonJgWingNorm(BYTE cbSonAnglesFlag=0xff);
	void UpdateSonJgPos();
	void UpdateSonJgOddment();
	bool ShiftMasterIndex(BYTE masterIndex=0);	//�����ӽǸֵ�������0 based index
	CLDSGroupLineAngle();
	~CLDSGroupLineAngle();
	BOOL m_bUpdateSonAngleOddment;
	//����
	int GetPropValueStr(long id,char* valueStr);	//��������ID�õ�����ֵ�ַ���
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static void InitGroupLineAnglePropHashtable();
	ADD_PROPLISTITEM_FUNC(CLDSGroupLineAngle,propHashtable)	//������������º���
	static PROPLIST_ITEM* UpdatePropItem(char* propKey,char* propName=NULL);		//�õ������ַ���
	static int GetPropStr(long id,char* propStr);		//�õ������ַ���
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static CHashTable<DWORD>propStatusHashtable;		//����״̬��ϣ��
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//�õ��������Ե�Ĭ��ֻ��״̬
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//�õ��������Ե�Ĭ��ֻ��״̬
	static int m_iCurDisplayPropGroup;					//��¼��������ǰ��ʾ���ID
	/*CLsRef* AppendStartLsRef(CLsRef ls_ref,BOOL bNearLsCheck=TRUE);
	CLsRef* AppendMidLsRef(CLsRef ls_ref,BOOL bNearLsCheck=TRUE);
	CLsRef* AppendEndLsRef(CLsRef ls_ref,BOOL bNearLsCheck=TRUE);
	BOOL DeleteLsRef(THANDLE ls_handle);
	int GetLsCount(short d=0);
	void EmptyLsRef();*/
};
class CGroupAngleLifeObj{
private:
	bool m_bRestore;
	long m_iVersion;
	long m_iDocType;
	char _pPresetMemPool[360];	//Ϊ�����ٶ�Ϊm_xBufferԤ�����ڴ�
	CBuffer m_xBuffer;
	CLDSGroupLineAngle* m_pSrcAngle;
public:
	CGroupAngleLifeObj();
	CGroupAngleLifeObj(CLDSGroupLineAngle* pAngle,long version=0,long doc_type=0);
	~CGroupAngleLifeObj();
};
//�׽Ű�
class CFoot : public CLDSPart
{
public:
	ATOM_LIST<THANDLE> *plank_list;
	THANDLE attach_jg_handle[2];		//�����������Ǹֵľ��
	THANDLE xie_jg_handle[4];		//�������Ǹ��������ӵ�����б�ĵľ��
	THANDLE base_node_handle;		//�����ڵ���
	short int A;					//ѥ��ǰ��̨�׸�
	short int B;					//ѥ�嶥�߾�Ǹ���Ƥ�ı߾�
	short int C;					//�����Ǹֵ׶���͵��װ���
	BOOL joint_in_zhun;			//б�Ľ�������׼��,��������Ƥ
	BOOL lj_by_weld;				//ѥ��Ǹ��������ͣ�TRUE:�Ժ� FALSE:��˨����
public:
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer);
	void ToXmlFile(FILE* fp,DWORD schema);
	CFoot();
	~CFoot();
	void Create3dFoot();
	void DesignXieJg(f3dPoint norm[]);
	BOOL CreateRefAxis();	//����ѥ��ṹ�õ��������ϵ

	CLDSPlate* GetFirstPlank();
	CLDSPlate* GetNextPlank();
	CLDSPlate* AppendPlank();
	void PushPlankPos(){if(plank_list) plank_list->push_stack();}
	void PopPlankPos(){if(plank_list) plank_list->pop_stack();}
	BOOL DeletePlank(long ii){return plank_list->DeleteAt(ii);}
	int GetPlankCount(){return plank_list->GetNodeNum();}
};

class CWeldPartView
{
public:
	UCS_STRU ucs;
	char sName[50];
	struct PART_ITEM{long hSonPart;BOOL bSectionView;};
	ATOM_LIST<PART_ITEM> SonPart;
	CWeldPartView()	{	strcpy(sName,"");	}
};
//������ϼ�
class CWeldPart : public CLDSDbObject
{
public:
	void FromBuffer(CBuffer &buffer,long version,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer);
	void ToXmlFile(FILE* fp,DWORD schema);
	SEGI iSeg;	//�������Ķκ�
	ULONG  m_uStatMatCoef;		//ͳ��ϵ��
	char sPartNo[MAX_CHAR_GUIGE_16+1],sName[MAX_TEMP_CHAR_50+1];
	struct PART_ITEM{long hSonPart; long num;};
	ATOM_LIST<PART_ITEM> SonPart;
	ATOM_LIST<CWeldPartView> SonView;
	CWeldPart()
	{
		m_nClassTypeId = CLS_WELDPART;
		//strcpy(m_sClassName,"CWeldPart");
		m_uStatMatCoef = 1;
		strcpy(sPartNo,"");
		strcpy(sName,"");
	}
	~CWeldPart()
	{
		SonPart.Empty();
		SonView.Empty();
	}
};
typedef CLdsList<CLDSPolyPart>POLYPARTLIST;
#endif
#endif // !defined(AFX_TDA_CO_PART_H__415A1A1F_534B_4ACC_B588_13AA8178D691__INCLUDED_)
