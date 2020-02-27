// ExtendItemInfo.h: interface for the COutputInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTENDITEMINFO_H__6FA66137_1CA6_4EA8_834A_67510409C72C__INCLUDED_)
#define AFX_EXTENDITEMINFO_H__6FA66137_1CA6_4EA8_834A_67510409C72C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "f_ent.h"
#include "SegI.h"
#include "ArrayList.h"
#include "XhCharString.h"
#include "BasicOutput.h"
#include "MemberProperty.h"
#include "lds_part.h"

//��չ������
enum OUTPUT_INFO_TYPE{
	//1.��������
	INFO_EMEND_SINGULAR_BOLTLO=1,	//��˨ͨ���޶�
	INFO_EMEND_CROSSRODINFO,		//���½���ļ�����Ϣ
	INFO_EMEND_MIRRODINFO,			//���������Գ���Ϣ
	INFO_EMEND_BOLTGASKET,			//�����޶���˨��Ƭ���Ȧ
	INFO_EMEND_LAYER,				//��������ͼ������
	INFO_EMEND_TUBEENDJOINTLJ,		//У�������ֹܶ�ͷ������Ϣ
	//2.����У��
	//���ǰ���
	INFO_CHK_MIRROD,				//�ԱȶԳƸ˼��Ĺ����ʼ���׼���ȣ��Ա���һ�����й������
	INFO_CHK_MIRROD_LENGTH,			//�ԱȶԳƸ˼��Ļ�׼����
	INFO_CHK_MIRPLATE,				//�ԳƸְ���
	INFO_CHK_BOLTOVERLAP,			//�ص���˨���
	INFO_CHK_HOLEMISSING,			//©�׼��
	INFO_CHK_REDUNDANTBOLT,			//������˨���
	//��˨�Ϸ���У��
	INFO_CHK_BOLT_ROLLINGSPACE,		//�Ǹ����Ʊ߾�
	INFO_CHK_BOLT_CUTANGLESPACE,	//�Ǹ��нǱ߾�
	INFO_CHK_BOLT_ANGLE_ENDSPACE,	//�Ǹ���˨�˾�
	INFO_CHK_BOLT_ANGLE_FORGEERR,	//�Ǹ�ѹ������˨��λ��ͻ
	INFO_CHK_BOLT_PLATE_ENDSPACE,	//������˨�߾�
	INFO_CHK_BOLT_HUOQULINE_SPACE,	//������˨�߾�(��������)
	INFO_CHK_BOLT_L0,				//��˨ͨ��Χ
	INFO_CHK_BOLT_SPEC,				//��˨�Ϸ����
	INFO_CHK_BOLT_GRADE,			//��˨����������Ƿ����
	//����������У�ˣ�����������У��(������ʵ�������޷���ʾ�Ĺ���, ��Ϊ�幹��)
	INFO_ROD_LOST_STARTNODE,		//�˼�ʼ�˽ڵ㶪ʧ
	INFO_ROD_LOST_ENDNODE,			//�˼��ն˽ڵ㶪ʧ
	INFO_NODE_LOST_FATHERROD,		//�ڵ㸸�˼���ʧ
	INFO_NODE_LOST_ATTACHNODE,		//�����ڵ�
	INFO_NODE_LOST_ATTACHROD,		//�����˼�
	INFO_NODE_LOST_ATTACHPLANE,		//����ƽ��
	INFO_NODE_LOST_ATTACHPOINT,		//������׼��
	INFO_PART_LOST_MATERIAL,		//����ȱ�ٲ�����Ϣ
	INFO_PART_INVALID_MATERIAL,		//����������Ϣ����ȷ
	INFO_ANGLE_LOST_LSLIST,			//�Ǹ�ȱ����˨����
	INFO_ANGLESTART_LOST_BENDLINE,	//�Ǹ�ʼ��ȱ�ٻ�����׼�Ǹ�
	INFO_ANGLESTART_LOST_PRJLINE,	//�Ǹ�ʼ��ȱ�ٹ���λ�û�׼�Ǹ�
	INFO_ANGLEEND_LOST_BENDLINE,	//�Ǹ��ն�ȱ�ٻ�����׼�Ǹ�
	INFO_ANGLEEND_LOST_PRJLINE,		//�Ǹ��ն�ȱ�ٹ���λ�û�׼�Ǹ�
	INFO_ANGLEWINGX_LOST_REF_ROD,	//�Ǹ�X֫���߼��������ȱ�ٲ��ո˼�
	INFO_ANGLEWINGY_LOST_REF_ROD,	//�Ǹ�Y֫���߼��������ȱ�ٲ��ո˼�
	INFO_PLATE_LOST_DATUM_REF_NODE,	//��׼�ڵ���Ϊ��
	INFO_PLATE_LOST_DATUM_POS_DES_PARA,	//��׼�����
	INFO_PLATE_LOST_NORM_DES_PARA,	//��׼�淨�߲���
	INFO_PLATE_LOST_LINK_REF_ROD,	//���Ӹ˼�
	INFO_BOLT_LOST_DATUM_REF_NODE,	//��˨��ʧ��׼�ڵ�
	INFO_BOLT_LOST_DATUM_REF_PART,	//��˨��ʧ��׼����
	INFO_BOLT_LOST_NORM_DES_PARA,	//��˨��ʧ������Ʋ���
	//�ϲ��ظ��ڵ㼰�ظ��˼�
	INFO_EMEND_OVERLAPNODE,			//�ϲ��ظ��ڵ�(����)
	INFO_EMEND_OVERLAPROD,			//�ϲ��ظ��˼�(����)
	//����ʵ�����ݺϷ��Լ��
	INFO_BOLT_NORM_ZERO,			//��˨����Ϊ��
	INFO_PART_LOCATION_ABNORMAL,	//����λ���쳣
	INFO_INVALID_PART_UCS,			//����ϵ���Ϸ�
	INFO_INVALID_PROFILE,			//�Ƿ�����,ʵ�嶥�������١����Ȼ��Ȼ���Ϊ0
	//
	INFO_CHK_MODELINTEGRITY,		//ģ���ڸ˼��ڵ��ƥ���ϵ���
	INFO_CHK_BOLTCONNECT,			//�˼�����˨����ǿ��
	INFO_CHK_MODELSTABILITY,		//��ϵ�ȶ��Լ��
	//3.���У��
	//�˼�
	INFO_CHK_NULLPARTNO,			//�ձ�Ź���
	INFO_CHK_PARTTYPE_DIFFER,		//�������Ͳ�ͬ
	INFO_CHK_RODINFO,				//ͬ��Ÿ˼����ȼ�����ͷ����񡢲��ʵȻ�����Ϣ
	INFO_CHK_RODBOLTINFO,			//ͬ��Ÿ˼�����˨���������,�˼�����˨λ����Ϣ�����ߵ��Աȣ�
	INFO_CHK_ANGLECUT,				//�Ǹ����н���Ϣ�����ߵ��Աȣ�
	//�ְ�
	INFO_CHK_PLATEINFO,				//ͬ��Ÿְ��ȡ����ʡ���������
	INFO_CHK_PLATEPROFILEANDBOLT,	//�ְ���˨�������������Կ�λ����߼����Σ��ߵ�������
	//Ӱ�乹��
	INFO_CHK_SHADOWPARTINFO,		//Ӱ�乹����ʼ�漶�����ĶԱ���Ϣ
	//4.��ײУ��
	INFO_CHK_COLLIDE_RODS,		//�˼���˼���ײ(��ָ������)
	INFO_CHK_COLLIDE_RODBOLT,	//�˼�����˨��ײ(��ָ������)
	INFO_CHK_COLLIDE_PLATEBOLT,	//�ְ�����˨��ײ(��ָ������)
	INFO_CHK_COLLIDE_BOLTS,		//��˨����˨��ײ(��ָ������)
	INFO_CHK_COLLIDE_RODPLATE,	//�˼���������ײ(��ָ������)
	INFO_CHK_COLLIDE_PLATES,	//������������ײ(��ָ������)
	//
	INFO_ONEKEY_CUTANGLE_CHECK,	//һ���нǼ��
	//5.����ͳ��
	INFO_STAT_BOLTRELAPARTS,	//��ѯ��˨��������
	INFO_STAT_RODLENGTH,		//���������ڵĸ˼�ͳ��
	INFO_STAT_RODSIZE,			//ָ�����ĸ˼�ͳ��
	//6.���㱨��ȶ�
	INFO_COMPARE_REPORT_WARNNING,	//���㱨��ȶ��еĴ��ı�������Ϣ
	INFO_COMPARE_REPORT_NODES,		//���㱨������ڵ���صľ�����Ϣ
	INFO_COMPARE_REPORT_ELEMS,		//���㱨�����뵥Ԫ��صľ�����Ϣ
	INFO_COMPARE_REPORT_RODS,		//���㱨������˼���صľ�����Ϣ
};
struct AUTOCORRECT_CMDINFO{
	//0.�ɰ�ȫ���Զ����д�������;1.�ɴ��з���(��Ҫͨ���˵�����ȷ��)���Զ����д�������;2.��Ҫ�û���������������
	short safeType;
	CXhChar16 nativeName;
};
class COutputInfo : public IBasicOutput
{
protected:
	bool state;	//true:�ѽ��;false:�����
public:
	int m_uType;//����
	//long m_hRelaObj1;		//��������id1
	//long m_hRelaObj2;		//��������id2
	double tolerance;		//����У��ʱ��������Χ,Ĭ��0.5mm
	CString m_sDescription;	//������Ϣ
	GEPOINT collisionPt;	//ʵ����ײ��
#ifdef __PART_DESIGN_INC_
public:	//��Ԫ�������ȶ���Ϣ, �Ժ�Ӧ��ʱ�䵥���COutputElemComparison���� wjh-2017.3.15
	static const BYTE ELEM_DIFF_BOTH_COMPRESSION = 1;		//ͬʱ��ѹ����
	static const BYTE ELEM_DIFF_TENSION_REDUCTION_COEF		= 2;//����ǿ���ۼ�ϵ��
	static const BYTE ELEM_DIFF_COMPRESSION_REDUCTION_COEF	= 3;//��ѹǿ���ۼ�ϵ��
	static const BYTE ELEM_DIFF_BLOCK_SHEAR_STATE	= 4;//����ܿ�״̬
	static const BYTE ELEM_DIFF_STABLE_COEF			= 5;//ѹ��ϵ��
	static const BYTE ELEM_DIFF_STABLE_mN			= 6;//ѹ���ȶ�ǿ���ۼ�ϵ��
	static const BYTE ELEM_DIFF_LAMDA_L				= 7;//���㳤��
	static const BYTE ELEM_DIFF_GYRATION			= 8;//��ת�뾶
	static const BYTE ELEM_DIFF_LAMDA				= 9;//��ϸ��
	static const BYTE ELEM_DIFF_MAX_TENSION			=10;//�������,N
	static const BYTE ELEM_DIFF_MAX_TENSION_CASE	=11;//�����������
	static const BYTE ELEM_DIFF_MAX_COMPRESSION		=12;//���ѹ��,N
	static const BYTE ELEM_DIFF_MAX_COMPRESSION_CASE=13;//���ѹ������
	static const BYTE ELEM_DIFF_MAX_STABLE_N		=14;//���ѹ����,N
	static const BYTE ELEM_DIFF_MAX_STABLE_SIGEMA	=15;//���ѹ��Ӧ��,MPa
	static const BYTE ELEM_DIFF_MAX_STABLE_CASE		=16;//���ѹ��������
	static const BYTE ELEM_DIFF_DANGEROUS_CASE		=17;//�ܿس�������
	static const BYTE ELEM_DIFF_MAX_WORK_SIGEMA		=18;//�����Ӧ��,MPa
	static const BYTE ELEM_DIFF_BEARLOAD_COEF		=19;//����ϵ��
	static const BYTE ELEM_DIFF_OUTSIDE_MISS	=100;	//��絼�뵥Ԫȱʧ
	BYTE m_ciElemDiffType;
	union COMPARE_ITEM_VALUE{
		bool bValue;
		long iValue;
		double fValue;
		char strbuf[16];
	}compare;
	PROPERTY(long,m_idElem);	//��ǰģ�͵�Ԫ
	GET(m_idElem){return m_hRelaObj1;}
	SET(m_idElem){m_hRelaObj1=value;}
	PROPERTY(long,m_idElem2);	//���ձȶԵ�Ԫ
	GET(m_idElem2){return m_hRelaObj2;}
	SET(m_idElem2){m_hRelaObj2=value;}
#endif
public:
	COutputInfo();
	virtual ~COutputInfo();
	virtual void SetMessage(const char* message){m_sDescription=message;}
	virtual const char* get_Message(){return m_sDescription;}
	virtual const char* GetTypeName();
	virtual void SelectRelationObj();
	virtual CString GetKeyStr();
	virtual bool CopyFrom(IBasicOutput* pOutput);

	int GetLegacyTypeId() const {return m_uType;}
	DWORD ChkStyleFlag();
	bool IsCollisionOutputType();
	bool SolvedState(){return state;}
	virtual bool CorrectError();
	// ����ֵ>=0��ʾ֧�ִ�����������
	virtual int CorrectErrorCmdInfo(AUTOCORRECT_CMDINFO &cmdInfo);
	virtual bool Recheck();
	static const char* GetTypeName(int uiType,int m_idOutputType=1);
};
class COutputElemComparison : public IBasicOutput
{
public:
	PROPERTY(long,m_idElem);	//��ǰģ�͵�Ԫ
	GET(m_idElem){return m_hRelaObj1;}
	SET(m_idElem){m_hRelaObj1=value;}
	PROPERTY(long,m_idElem2);	//���ձȶԵ�Ԫ
	GET(m_idElem2){return m_hRelaObj2;}
	SET(m_idElem2){m_hRelaObj2=value;}
	//long m_hRelaObj1;		//��������id1
	//long m_hRelaObj2;		//��������id2
	//double tolerance;		//����У��ʱ��������Χ,Ĭ��0.5mm
	//CString m_sDescription;	//������Ϣ
public:
	COutputElemComparison();
	virtual ~COutputElemComparison();
	//virtual void SetMessage(const char* message){m_sDescription=message;}
	virtual const char* GetTypeName();
	virtual void SelectRelationObj();
	virtual CString GetKeyStr();
};
class COutputInfoAgent{
	void* m_pContext;
public:
	COutputInfoAgent(void* pContext){m_pContext=pContext;}
	OUTPUT_INFO_TYPE m_uType;
	COutputInfo* (*ListAppendFunc)(void* pContext);
	COutputInfo* Append(long hRelaObj1,long hRelaObj2,char* format,...);
};
class CCheckPartNoInfo : public COutputInfo
{
public:
	CString m_sPartNo;
public:
	CCheckPartNoInfo();
	~CCheckPartNoInfo();
	CCheckPartNoInfo(CString sPartNo,long hObj1,long hObj2,CString sDescription);
	virtual bool Recheck();
	static void PartNoCheck(CTaskOutputList &itemList,CString sSegStr,double fScope,char *sPartNo=NULL);
};
void AppendCheckLabelOutputInfo(CTaskOutputList* pItemList,CCheckPartNoInfo& checklabel);
class CCheckGasketErrorInfo : public COutputInfo
{
public:
	CCheckGasketErrorInfo();
	~CCheckGasketErrorInfo();
	//CCheckPartNoInfo(CString sPartNo,long hObj1,long hObj2,CString sDescription);
	short siGasketThick;//����Ȧ���(��ֵ��ʾ����Ի�������˨�����������£�
	struct ANCHOR{
		long hAnchorPart;	//������ӵ��ʱ�����Ķ�λ��׼����
		double gasketOffset;//����Ի�׼��˨�ڻ�׼�����ϵĴ��彻���ƫ��ֵ������ȷ�����λ�ã���׼�����Ĺ�������Ϊƫ������)
		double washerOffset;//��Ȧ����˨��ƫ�ƾ�
		ANCHOR(){hAnchorPart=0;gasketOffset=washerOffset=0;}
	}anchor;
	//this->m_uType=ERROR_GASKET_PROFILEʱ��ʾ��Ҫͳһ���εĶ�׵����
	//this->m_uType=ERROR_MISSING_ENDGASKET��ERROR_MISSING_CROSSGASKETʱ��ʾ��Ҫ��ӵ������������˨�������
	//this->m_uType==ERROR_BOLTPAD_CUTANGLEʱ��ʾ����нǵĲ��սǸ־������ʱ���׸�Ԥ��������Ч
	PRESET_LONG4ARR relaObjIds; 
	virtual bool CorrectError();
	// ����ֵ>=0��ʾ֧�ִ�����������
	virtual int CorrectErrorCmdInfo(AUTOCORRECT_CMDINFO &cmdInfo);
	virtual bool Recheck();
	virtual bool CopyFrom(IBasicOutput* pOutput);
public:	//����
	long get_hRelaBolt();
	long set_hRelaBolt(long hBolt);
	__declspec(property(put=set_hRelaBolt,get=get_hRelaBolt)) long hRelaBolt;
	long get_hRelaGasket();
	long set_hRelaGasket(long hGasket);
	__declspec(property(put=set_hRelaGasket,get=get_hRelaGasket)) long hRelaGasket;
};
class DATACENTERCMD_CONTEXT
{
public:
	void* m_pOutputInfoList;
	void* m_pMainWnd;
	void* m_pTag;
	DWORD chkStyleFlag;
	double tolerance;	//���(deviation)����Χ
	long nMaxWarnings;	//��������Ŀ������ֵʱ��ֹ����У��
	CHashList<SEGI> segTable;
	DATACENTERCMD_CONTEXT(){
		m_pOutputInfoList=NULL;
		m_pMainWnd=NULL;
		m_pTag=NULL;
		chkStyleFlag=0;
		nMaxWarnings=0;
		tolerance=0.5;
	}
	void InitByCmd(CMDCFG_ITEM &cmdItem)
	{
		int j=0;
#ifndef __ENABLE_XHSTRING_LENGTH_PROPERTY_
		if(cmdItem.properties[0].bActive&&cmdItem.properties[0].name.Length()==0)
#else
		if(cmdItem.properties[0].bActive&&cmdItem.properties[0].name.Length==0)
#endif
			chkStyleFlag=cmdItem.properties[0].val.dwVal;	//���õ�У������������
		else
			j=1;
		for(j=j;j<4;j++)
		{
			if(!cmdItem.properties[j].bActive)
				break;
#ifdef AFX_TARG_ENU_ENGLISH
			else if(cmdItem.properties[j].name.Equal("Permissible error(mm)"))
#else 
			else if(cmdItem.properties[j].name.Equal("�������(mm)"))
#endif
				tolerance=cmdItem.properties[j].val.dblVal;
		}
	}
	BOOL IsGreaterThanMaxWarnings(int nWarnings)
	{
		if(nMaxWarnings>0&&nWarnings>=nMaxWarnings)
			return TRUE;
		return FALSE;
	}
};
#ifdef __COMMON_PART_INC_
//INFO_CHK_NULLPARTNO �����ż�鹲��ͬһ������
typedef ARRAY_LIST<CLDSPart*> PART_PTR_ARR;
typedef ARRAY_LIST<CLDSPolyPart*>POLYPART_PTR_ARR;
class PARTNO_CHK_CONTEXT : public DATACENTERCMD_CONTEXT
{
public:
	int nSumPartNum;	//�����Ĺ�������(��ͨ���������乹���ܺ�)����Ҫ������ʾ������
	CHashStrList<PART_PTR_ARR> partsHasListPartNo;
	CHashStrList<POLYPART_PTR_ARR> polyPartsHasListPartNo;
	CXhChar16 sSpecifyPartNo;
	BOOL m_bOutputToNotepad;
	PARTNO_CHK_CONTEXT()
	{
		nSumPartNum=0;
		m_bOutputToNotepad=FALSE;
		partsHasListPartNo.Empty();
		polyPartsHasListPartNo.Empty();
	}
};
#endif
extern CHashStrList<COutputInfo> markInfoList;
#endif // !defined(AFX_EXTENDITEMINFO_H__6FA66137_1CA6_4EA8_834A_67510409C72C__INCLUDED_)